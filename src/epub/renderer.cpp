#include "text_renderer.h"
#include <vita2d.h>
#include <algorithm>
#include <cstring>

bool TextRenderer::initialize(const std::string& font_path) {
    if (FT_Init_FreeType(&library)) {
        return false;
    }
    
    if (FT_New_Face(library, font_path.c_str(), 0, &face)) {
        FT_Done_FreeType(library);
        return false;
    }
    
    set_font_size(16); // Default size
    return true;
}

void TextRenderer::set_font_size(int size) {
    FT_Set_Pixel_Sizes(face, 0, size);
}

const TextRenderer::GlyphInfo* TextRenderer::get_glyph(uint32_t charcode) {
    auto it = glyph_cache.find(charcode);
    if (it != glyph_cache.end()) {
        return it->second;
    }
    
    // Cache size management
    if (glyph_cache.size() >= MAX_CACHE_SIZE) {
        evict_cache_entry();
    }
    
    // Load glyph from FreeType
    if (FT_Load_Char(face, charcode, FT_LOAD_RENDER)) {
        return nullptr;
    }
    
    FT_GlyphSlot slot = face->glyph;
    
    // Create cache entry
    GlyphInfo* glyph_info = new GlyphInfo();
    glyph_info->width = slot->bitmap.width;
    glyph_info->height = slot->bitmap.rows;
    glyph_info->left = slot->bitmap_left;
    glyph_info->top = slot->bitmap_top;
    glyph_info->advance_x = slot->advance.x >> 6;
    
    // Copy bitmap data
    size_t bitmap_size = glyph_info->width * glyph_info->height;
    if (bitmap_size > 0) {
        glyph_info->bitmap = new uint8_t[bitmap_size];
        std::memcpy(glyph_info->bitmap, slot->bitmap.buffer, bitmap_size);
    }
    
    // Store in cache
    glyph_cache[charcode] = glyph_info;
    return glyph_info;
}

void TextRenderer::render_text(const std::string& text, int x, int y, int max_width, const TextStyle& style) {
    set_font_size(style.font_size);
    
    std::vector<std::string> lines = wrap_text(text, max_width);
    int current_y = y;
    
    for (const auto& line : lines) {
        int current_x = x;
        
        // Apply text alignment
        if (style.align == TextStyle::CENTER) {
            int line_width = calculate_text_width(line);
            current_x = x + (max_width - line_width) / 2;
        } else if (style.align == TextStyle::RIGHT) {
            int line_width = calculate_text_width(line);
            current_x = x + max_width - line_width;
        }
        
        // Render each character
        for (size_t i = 0; i < line.length(); ++i) {
            uint32_t charcode = static_cast<uint32_t>(line[i]);
            const GlyphInfo* glyph = get_glyph(charcode);
            if (glyph) {
                render_glyph(glyph, current_x + glyph->left, current_y - glyph->top, style.color);
                current_x += glyph->advance_x;
            }
        }
        
        current_y += style.line_height;
    }
}

std::vector<std::string> TextRenderer::wrap_text(const std::string& text, int max_width) {
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
            int test_width = calculate_text_width(test_line);
            
            if (test_width <= max_width) {
                current_line = test_line;
            } else {
                if (!current_line.empty()) {
                    lines.push_back(current_line);
                    current_line = current_word;
                } else {
                    lines.push_back(current_word); // Handle very long words
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

int TextRenderer::calculate_text_width(const std::string& text) {
    int width = 0;
    for (char c : text) {
        const GlyphInfo* glyph = get_glyph(static_cast<uint32_t>(c));
        if (glyph) {
            width += glyph->advance_x;
        }
    }
    return width;
}

int TextRenderer::calculate_text_height(const std::string& text, int max_width) {
    std::vector<std::string> lines = wrap_text(text, max_width);
    return static_cast<int>(lines.size()) * 20; // Default line height
}

void TextRenderer::render_glyph(const GlyphInfo* glyph, int x, int y, uint32_t color) {
    if (!glyph || !glyph->bitmap) return;
    
    // Convert RGBA color to components
    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
    uint8_t a = color & 0xFF;
    
    // Render glyph bitmap using vita2d
    for (int py = 0; py < glyph->height; ++py) {
        for (int px = 0; px < glyph->width; ++px) {
            uint8_t alpha = glyph->bitmap[py * glyph->width + px];
            if (alpha > 0) {
                // Alpha blending
                uint8_t final_alpha = (alpha * a) / 255;
                uint32_t pixel_color = RGBA8(r, g, b, final_alpha);
                vita2d_draw_pixel(x + px, y + py, pixel_color);
            }
        }
    }
}

void TextRenderer::evict_cache_entry() {
    if (glyph_cache.empty()) return;
    
    // Simple eviction: remove first entry
    auto it = glyph_cache.begin();
    delete it->second;
    glyph_cache.erase(it);
}

void TextRenderer::clear_cache() {
    for (auto& pair : glyph_cache) {
        delete pair.second;
    }
    glyph_cache.clear();
}

TextRenderer::~TextRenderer() {
    clear_cache();
    if (face) {
        FT_Done_Face(face);
    }
    if (library) {
        FT_Done_FreeType(library);
    }
}