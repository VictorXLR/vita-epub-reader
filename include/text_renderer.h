#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <unordered_map>
#include <string>
#include <vector>

class TextRenderer {
public:
    struct GlyphInfo {
        uint8_t* bitmap;
        int width, height;
        int left, top;
        int advance_x;
        
        GlyphInfo() : bitmap(nullptr), width(0), height(0), left(0), top(0), advance_x(0) {}
        ~GlyphInfo() { delete[] bitmap; }
    };
    
    struct TextStyle {
        std::string font_family = "default";
        int font_size = 16;
        bool bold = false;
        bool italic = false;
        uint32_t color = 0x000000FF;
        enum Alignment { LEFT, CENTER, RIGHT, JUSTIFY } align = LEFT;
        int line_height = 20;
        int margin_top = 0;
        int margin_bottom = 0;
    };

private:
    FT_Library library;
    FT_Face face;
    std::unordered_map<uint32_t, GlyphInfo*> glyph_cache;
    static const size_t MAX_CACHE_SIZE = 512;
    
public:
    bool initialize(const std::string& font_path);
    void set_font_size(int size);
    void render_text(const std::string& text, int x, int y, int max_width, const TextStyle& style);
    int calculate_text_width(const std::string& text);
    int calculate_text_height(const std::string& text, int max_width);
    std::vector<std::string> wrap_text(const std::string& text, int max_width);
    void clear_cache();
    ~TextRenderer();

private:
    const GlyphInfo* get_glyph(uint32_t charcode);
    void render_glyph(const GlyphInfo* glyph, int x, int y, uint32_t color);
    void evict_cache_entry();
};

#endif // TEXT_RENDERER_H