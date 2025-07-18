#include <psp2/ctrl.h>
#include "gpu_renderer.h"
#include "file_manager.h"
#include <vector>
#include <string>

class BookList {
private:
    int selected_book;
    std::vector<std::string> book_files;
    std::vector<std::string> book_titles;
    GPURenderer* renderer;
    int scroll_offset;
    
public:
    enum BookListResult {
        BOOKLIST_CONTINUE,
        BOOKLIST_BACK,
        BOOKLIST_OPEN_BOOK
    };
    
    BookList(GPURenderer* gpu_renderer) : selected_book(0), renderer(gpu_renderer), scroll_offset(0) {
        refresh_book_list();
    }
    
    void refresh_book_list() {
        book_files = FileManager::list_epub_files();
        book_titles.clear();
        
        for (const auto& file : book_files) {
            book_titles.push_back(FileManager::get_epub_metadata(file));
        }
        
        if (selected_book >= static_cast<int>(book_files.size())) {
            selected_book = std::max(0, static_cast<int>(book_files.size()) - 1);
        }
    }
    
    BookListResult update(const SceCtrlData& ctrl, uint32_t last_buttons) {
        if (book_files.empty()) {
            if ((ctrl.buttons & SCE_CTRL_CIRCLE) && !(last_buttons & SCE_CTRL_CIRCLE)) {
                return BOOKLIST_BACK;
            }
            return BOOKLIST_CONTINUE;
        }
        
        // Navigation
        if ((ctrl.buttons & SCE_CTRL_UP) && !(last_buttons & SCE_CTRL_UP)) {
            selected_book = (selected_book - 1 + book_files.size()) % book_files.size();
            adjust_scroll();
        }
        if ((ctrl.buttons & SCE_CTRL_DOWN) && !(last_buttons & SCE_CTRL_DOWN)) {
            selected_book = (selected_book + 1) % book_files.size();
            adjust_scroll();
        }
        
        // Open book
        if ((ctrl.buttons & SCE_CTRL_CROSS) && !(last_buttons & SCE_CTRL_CROSS)) {
            return BOOKLIST_OPEN_BOOK;
        }
        
        // Back to menu
        if ((ctrl.buttons & SCE_CTRL_CIRCLE) && !(last_buttons & SCE_CTRL_CIRCLE)) {
            return BOOKLIST_BACK;
        }
        
        return BOOKLIST_CONTINUE;
    }
    
    void render() {
        // Render title
        renderer->render_text_gpu("Book Library", 100, 80, RGBA8(0, 0, 0, 255), 32);
        
        if (book_files.empty()) {
            renderer->render_text_gpu("No EPUB files found", 150, 200, RGBA8(100, 100, 100, 255), 20);
            renderer->render_text_gpu("Copy EPUB files to: ux0:data/epub_reader/books/", 150, 240, RGBA8(100, 100, 100, 255), 16);
            renderer->render_text_gpu("Press O to go back", 150, 280, RGBA8(100, 100, 100, 255), 16);
            return;
        }
        
        // Render book list
        int y_start = 120;
        int y_spacing = 30;
        int visible_items = 12; // Number of items visible on screen
        
        // Calculate visible range
        int start_index = std::max(0, selected_book - visible_items / 2);
        int end_index = std::min(static_cast<int>(book_titles.size()), start_index + visible_items);
        
        for (int i = start_index; i < end_index; ++i) {
            bool selected = (i == selected_book);
            int y_pos = y_start + (i - start_index) * y_spacing;
            
            // Render book title
            renderer->render_menu_item(book_titles[i], 150, y_pos, selected);
            
            // Render file size
            if (selected) {
                size_t file_size = FileManager::get_file_size(book_files[i]);
                std::string size_text = format_file_size(file_size);
                renderer->render_text_gpu(size_text, 600, y_pos, RGBA8(100, 100, 100, 255), 16);
            }
        }
        
        // Render scroll indicator
        if (book_titles.size() > visible_items) {
            int indicator_height = 300 * visible_items / book_titles.size();
            int indicator_pos = 120 + (selected_book * 300 / book_titles.size());
            
            renderer->render_rectangle(920, 120, 10, 300, RGBA8(200, 200, 200, 255));
            renderer->render_rectangle(920, indicator_pos, 10, indicator_height, RGBA8(100, 100, 100, 255));
        }
        
        // Render instructions
        renderer->render_text_gpu("Use D-Pad to navigate, X to open book, O to go back", 100, 480, RGBA8(100, 100, 100, 255), 16);
    }
    
    std::string get_selected_book_path() const {
        if (selected_book >= 0 && selected_book < static_cast<int>(book_files.size())) {
            return book_files[selected_book];
        }
        return "";
    }
    
private:
    void adjust_scroll() {
        // Scroll is handled automatically by the visible range calculation
    }
    
    std::string format_file_size(size_t bytes) {
        if (bytes < 1024) {
            return std::to_string(bytes) + " B";
        } else if (bytes < 1024 * 1024) {
            return std::to_string(bytes / 1024) + " KB";
        } else {
            return std::to_string(bytes / (1024 * 1024)) + " MB";
        }
    }
};