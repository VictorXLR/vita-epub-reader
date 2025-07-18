#include "gpu_renderer.h"
#include <iostream>
#include <algorithm>

bool GPURenderer::initialize() {
    vita2d_init();
    vita2d_set_clear_color(RGBA8(255, 255, 255, 255)); // White background
    
    // Load default font for text rendering
    default_font = vita2d_load_font_file("assets/fonts/default.ttf");
    if (!default_font) {
        std::cerr << "Failed to load default font, using system font" << std::endl;
        default_font = vita2d_load_default_font();
        if (!default_font) {
            std::cerr << "Failed to load system font" << std::endl;
            return false;
        }
    }
    
    // Create texture cache for pre-rendered text
    text_cache_texture = vita2d_create_empty_texture(1024, 1024);
    if (!text_cache_texture) {
        std::cerr << "Failed to create text cache texture" << std::endl;
    }
    
    std::cout << "GPU renderer initialized successfully" << std::endl;
    return true;
}

void GPURenderer::begin_frame() {
    vita2d_start_drawing();
    vita2d_clear_screen();
}

void GPURenderer::end_frame() {
    vita2d_end_drawing();
    vita2d_swap_buffers();
}

void GPURenderer::clear_screen(uint32_t color) {
    vita2d_set_clear_color(color);
    vita2d_clear_screen();
}

void GPURenderer::render_text_gpu(const std::string& text, int x, int y, uint32_t color, int size) {
    if (!default_font) return;
    
    // Use GPU-accelerated text rendering
    vita2d_font_draw_text(default_font, x, y, color, size, text.c_str());
}

void GPURenderer::render_text_wrapped(const std::string& text, int x, int y, int max_width, uint32_t color, int size) {
    std::vector<std::string> lines = wrap_text_to_width(text, max_width, size);
    int current_y = y;
    int line_height = get_text_height(size) + 4; // Add some line spacing
    
    for (const auto& line : lines) {
        render_text_gpu(line, x, current_y, color, size);
        current_y += line_height;
    }
}

int GPURenderer::get_text_width(const std::string& text, int size) {
    if (!default_font) return 0;
    return vita2d_font_text_width(default_font, size, text.c_str());
}

int GPURenderer::get_text_height(int size) {
    if (!default_font) return size;
    return vita2d_font_text_height(default_font, size, "Ay"); // Use text with ascender and descender
}

void GPURenderer::render_rectangle(int x, int y, int width, int height, uint32_t color) {
    vita2d_draw_rectangle(x, y, width, height, color);
}

void GPURenderer::render_rectangle_outline(int x, int y, int width, int height, uint32_t color, int thickness) {
    // Draw four rectangles to form an outline
    vita2d_draw_rectangle(x, y, width, thickness, color); // Top
    vita2d_draw_rectangle(x, y + height - thickness, width, thickness, color); // Bottom
    vita2d_draw_rectangle(x, y, thickness, height, color); // Left
    vita2d_draw_rectangle(x + width - thickness, y, thickness, height, color); // Right
}

void GPURenderer::render_line(int x1, int y1, int x2, int y2, uint32_t color) {
    vita2d_draw_line(x1, y1, x2, y2, color);
}

void GPURenderer::render_cached_page(const std::vector<std::string>& lines, int scroll_offset) {
    int y_pos = 50 - scroll_offset; // Start position with margin
    int line_height = 24;
    int margin_x = 50;
    
    for (const auto& line : lines) {
        // Only render visible lines to improve performance
        if (y_pos > -line_height && y_pos < SCREEN_HEIGHT + line_height) {
            render_text_gpu(line, margin_x, y_pos, RGBA8(0, 0, 0, 255), 18);
        }
        y_pos += line_height;
        
        // Stop rendering if we're way below the screen
        if (y_pos > SCREEN_HEIGHT + 100) {
            break;
        }
    }
}

void GPURenderer::render_menu_item(const std::string& text, int x, int y, bool selected, uint32_t color) {
    if (selected) {
        // Draw selection background
        int text_width = get_text_width(text, 20);
        int text_height = get_text_height(20);
        render_rectangle(x - 10, y - 5, text_width + 20, text_height + 10, RGBA8(100, 150, 255, 100));
    }
    
    render_text_gpu(text, x, y, color, 20);
}

void GPURenderer::render_progress_bar(int x, int y, int width, int height, float progress, uint32_t fg_color, uint32_t bg_color) {
    // Clamp progress to valid range
    progress = std::max(0.0f, std::min(1.0f, progress));
    
    // Draw background
    render_rectangle(x, y, width, height, bg_color);
    
    // Draw progress
    int progress_width = static_cast<int>(width * progress);
    if (progress_width > 0) {
        render_rectangle(x, y, progress_width, height, fg_color);
    }
    
    // Draw border
    render_rectangle_outline(x, y, width, height, RGBA8(0, 0, 0, 255), 1);
}

void GPURenderer::set_clip_rect(int x, int y, int width, int height) {
    vita2d_enable_clipping();
    vita2d_set_clip_rectangle(x, y, x + width, y + height);
}

void GPURenderer::clear_clip_rect() {
    vita2d_disable_clipping();
}

std::vector<std::string> GPURenderer::wrap_text_to_width(const std::string& text, int max_width, int font_size) {
    std::vector<std::string> lines;
    std::string current_line;
    std::string current_word;
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        
        if (c == ' ' || c == '\n' || i == text.length() - 1) {
            if (i == text.length() - 1 && c != ' ' && c != '\n') {
                current_word += c;
            }
            
            std::string test_line = current_line.empty() ? current_word : current_line + " " + current_word;
            int test_width = get_text_width(test_line, font_size);
            
            if (test_width <= max_width) {
                current_line = test_line;
            } else {
                if (!current_line.empty()) {
                    lines.push_back(current_line);
                    current_line = current_word;
                } else {
                    // Handle very long words by breaking them
                    if (get_text_width(current_word, font_size) > max_width) {
                        // Break the word character by character
                        std::string partial_word;
                        for (char wc : current_word) {
                            std::string test_partial = partial_word + wc;
                            if (get_text_width(test_partial, font_size) > max_width) {
                                if (!partial_word.empty()) {
                                    lines.push_back(partial_word);
                                }
                                partial_word = wc;
                            } else {
                                partial_word = test_partial;
                            }
                        }
                        current_line = partial_word;
                    } else {
                        lines.push_back(current_word);
                        current_line.clear();
                    }
                }
            }
            
            current_word.clear();
            
            if (c == '\n') {
                lines.push_back(current_line);
                current_line.clear();
            }
        } else {
            current_word += c;
        }
    }
    
    if (!current_line.empty()) {
        lines.push_back(current_line);
    }
    
    return lines;
}

void GPURenderer::cleanup() {
    if (text_cache_texture) {
        vita2d_free_texture(text_cache_texture);
        text_cache_texture = nullptr;
    }
    if (default_font) {
        vita2d_free_font(default_font);
        default_font = nullptr;
    }
    if (bold_font) {
        vita2d_free_font(bold_font);
        bold_font = nullptr;
    }
    if (italic_font) {
        vita2d_free_font(italic_font);
        italic_font = nullptr;
    }
    vita2d_fini();
}