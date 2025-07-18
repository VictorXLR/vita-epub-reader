#ifndef GPU_RENDERER_H
#define GPU_RENDERER_H

#include <vita2d.h>
#include <string>
#include <vector>

class GPURenderer {
private:
    vita2d_texture* text_cache_texture;
    vita2d_font* default_font;
    vita2d_font* bold_font;
    vita2d_font* italic_font;
    
    // Screen dimensions
    static const int SCREEN_WIDTH = 960;
    static const int SCREEN_HEIGHT = 544;
    
public:
    bool initialize();
    void begin_frame();
    void end_frame();
    void clear_screen(uint32_t color = RGBA8(255, 255, 255, 255));
    
    // Text rendering functions
    void render_text_gpu(const std::string& text, int x, int y, uint32_t color = RGBA8(0, 0, 0, 255), int size = 16);
    void render_text_wrapped(const std::string& text, int x, int y, int max_width, uint32_t color = RGBA8(0, 0, 0, 255), int size = 16);
    int get_text_width(const std::string& text, int size = 16);
    int get_text_height(int size = 16);
    
    // Shape rendering functions
    void render_rectangle(int x, int y, int width, int height, uint32_t color);
    void render_rectangle_outline(int x, int y, int width, int height, uint32_t color, int thickness = 1);
    void render_line(int x1, int y1, int x2, int y2, uint32_t color);
    
    // Page rendering functions
    void render_cached_page(const std::vector<std::string>& lines, int scroll_offset);
    void render_menu_item(const std::string& text, int x, int y, bool selected, uint32_t color = RGBA8(0, 0, 0, 255));
    void render_progress_bar(int x, int y, int width, int height, float progress, uint32_t fg_color, uint32_t bg_color);
    
    // Utility functions
    void set_clip_rect(int x, int y, int width, int height);
    void clear_clip_rect();
    
    void cleanup();
    
private:
    std::vector<std::string> wrap_text_to_width(const std::string& text, int max_width, int font_size);
};

#endif // GPU_RENDERER_H