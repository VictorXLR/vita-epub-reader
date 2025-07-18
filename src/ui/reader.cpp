#include <psp2/ctrl.h>
#include "gpu_renderer.h"
#include "epub_parser.h"
#include <vector>
#include <string>
#include <algorithm>

class BookReader {
private:
    GPURenderer* renderer;
    EPUBParser* epub_parser;
    std::vector<std::string> current_page_lines;
    int current_chapter;
    int scroll_offset;
    int max_scroll;
    bool show_ui;
    
public:
    enum ReaderResult {
        READER_CONTINUE,
        READER_BACK_TO_MENU,
        READER_NEXT_CHAPTER,
        READER_PREV_CHAPTER
    };
    
    BookReader(GPURenderer* gpu_renderer, EPUBParser* parser) 
        : renderer(gpu_renderer), epub_parser(parser), current_chapter(0), 
          scroll_offset(0), max_scroll(0), show_ui(false) {}
    
    bool load_chapter(int chapter_index) {
        const auto& toc = epub_parser->get_table_of_contents();
        if (chapter_index >= 0 && chapter_index < static_cast<int>(toc.size())) {
            current_chapter = chapter_index;
            scroll_offset = 0;
            
            // Load chapter content
            std::string content = epub_parser->get_content(toc[chapter_index].content_src);
            
            // Parse HTML and extract text (simplified)
            std::string plain_text = extract_text_from_html(content);
            
            // Wrap text for display
            current_page_lines = renderer->wrap_text_to_width(plain_text, 860, 18);
            
            // Calculate max scroll
            int total_height = current_page_lines.size() * 24;
            max_scroll = std::max(0, total_height - 400); // 400 is visible area height
            
            return true;
        }
        return false;
    }
    
    ReaderResult update(const SceCtrlData& ctrl, uint32_t last_buttons) {
        // Toggle UI visibility
        if ((ctrl.buttons & SCE_CTRL_TRIANGLE) && !(last_buttons & SCE_CTRL_TRIANGLE)) {
            show_ui = !show_ui;
        }
        
        // Page navigation
        if ((ctrl.buttons & SCE_CTRL_LEFT) && !(last_buttons & SCE_CTRL_LEFT)) {
            scroll_offset = std::max(0, scroll_offset - 200); // Scroll up
        }
        if ((ctrl.buttons & SCE_CTRL_RIGHT) && !(last_buttons & SCE_CTRL_RIGHT)) {
            scroll_offset = std::min(max_scroll, scroll_offset + 200); // Scroll down
        }
        
        // Chapter navigation
        if ((ctrl.buttons & SCE_CTRL_LTRIGGER) && !(last_buttons & SCE_CTRL_LTRIGGER)) {
            if (current_chapter > 0) {
                load_chapter(current_chapter - 1);
            }
        }
        if ((ctrl.buttons & SCE_CTRL_RTRIGGER) && !(last_buttons & SCE_CTRL_RTRIGGER)) {
            const auto& toc = epub_parser->get_table_of_contents();
            if (current_chapter < static_cast<int>(toc.size()) - 1) {
                load_chapter(current_chapter + 1);
            }
        }
        
        // Return to menu
        if ((ctrl.buttons & SCE_CTRL_CIRCLE) && !(last_buttons & SCE_CTRL_CIRCLE)) {
            return READER_BACK_TO_MENU;
        }
        
        return READER_CONTINUE;
    }
    
    void render() {
        // Render page content
        renderer->render_cached_page(current_page_lines, scroll_offset);
        
        if (show_ui) {
            // Render UI overlay with semi-transparent background
            renderer->render_rectangle(0, 0, 960, 60, RGBA8(0, 0, 0, 180));
            renderer->render_rectangle(0, 484, 960, 60, RGBA8(0, 0, 0, 180));
            
            // Render chapter info
            const auto& toc = epub_parser->get_table_of_contents();
            if (current_chapter < static_cast<int>(toc.size())) {
                std::string chapter_title = "Chapter " + std::to_string(current_chapter + 1) + ": " + 
                                          toc[current_chapter].title;
                renderer->render_text_gpu(chapter_title, 20, 20, RGBA8(255, 255, 255, 255), 16);
            }
            
            // Render scroll indicator
            if (max_scroll > 0) {
                int indicator_height = 400 * 400 / (current_page_lines.size() * 24);
                int indicator_pos = 80 + (scroll_offset * 300 / max_scroll);
                
                renderer->render_rectangle(940, 80, 10, 400, RGBA8(100, 100, 100, 100));
                renderer->render_rectangle(940, indicator_pos, 10, indicator_height, RGBA8(255, 255, 255, 255));
            }
            
            // Render controls
            renderer->render_text_gpu("L/R: Chapters  Left/Right: Scroll  △: UI  ○: Menu", 20, 500, RGBA8(255, 255, 255, 255), 14);
        }
    }
    
private:
    std::string extract_text_from_html(const std::string& html_content) {
        // Simplified HTML text extraction
        std::string result;
        bool in_tag = false;
        bool in_script = false;
        bool in_style = false;
        
        for (size_t i = 0; i < html_content.length(); ++i) {
            char c = html_content[i];
            
            if (c == '<') {
                in_tag = true;
                
                // Check for script or style tags
                if (i + 6 < html_content.length() && 
                    html_content.substr(i, 7) == "<script") {
                    in_script = true;
                } else if (i + 5 < html_content.length() && 
                          html_content.substr(i, 6) == "<style") {
                    in_style = true;
                }
            } else if (c == '>') {
                in_tag = false;
                
                // Check for end of script or style tags
                if (in_script && i >= 8 && html_content.substr(i - 8, 9) == "</script>") {
                    in_script = false;
                } else if (in_style && i >= 7 && html_content.substr(i - 7, 8) == "</style>") {
                    in_style = false;
                }
                
                if (!in_script && !in_style) {
                    result += ' '; // Add space between tags
                }
            } else if (!in_tag && !in_script && !in_style) {
                result += c;
            }
        }
        
        // Clean up multiple spaces and newlines
        std::string cleaned;
        bool prev_space = false;
        for (char c : result) {
            if (c == ' ' || c == '\n' || c == '\t') {
                if (!prev_space) {
                    cleaned += ' ';
                    prev_space = true;
                }
            } else {
                cleaned += c;
                prev_space = false;
            }
        }
        
        return cleaned;
    }
};