#include <psp2/ctrl.h>
#include "gpu_renderer.h"
#include <vector>
#include <string>

class SettingsMenu {
private:
    int selected_item;
    std::vector<std::string> setting_items;
    GPURenderer* renderer;
    
    // Settings values
    int font_size;
    int line_spacing;
    bool auto_scroll;
    int scroll_speed;
    
public:
    enum SettingsResult {
        SETTINGS_CONTINUE,
        SETTINGS_BACK
    };
    
    SettingsMenu(GPURenderer* gpu_renderer) : selected_item(0), renderer(gpu_renderer),
                                            font_size(18), line_spacing(4), auto_scroll(false), scroll_speed(2) {
        setting_items = {
            "Font Size",
            "Line Spacing", 
            "Auto Scroll",
            "Scroll Speed",
            "Back"
        };
    }
    
    SettingsResult update(const SceCtrlData& ctrl, uint32_t last_buttons) {
        // Menu navigation
        if ((ctrl.buttons & SCE_CTRL_UP) && !(last_buttons & SCE_CTRL_UP)) {
            selected_item = (selected_item - 1 + setting_items.size()) % setting_items.size();
        }
        if ((ctrl.buttons & SCE_CTRL_DOWN) && !(last_buttons & SCE_CTRL_DOWN)) {
            selected_item = (selected_item + 1) % setting_items.size();
        }
        
        // Value adjustment
        if ((ctrl.buttons & SCE_CTRL_LEFT) && !(last_buttons & SCE_CTRL_LEFT)) {
            adjust_setting(-1);
        }
        if ((ctrl.buttons & SCE_CTRL_RIGHT) && !(last_buttons & SCE_CTRL_RIGHT)) {
            adjust_setting(1);
        }
        
        // Back to menu
        if ((ctrl.buttons & SCE_CTRL_CROSS) && !(last_buttons & SCE_CTRL_CROSS)) {
            if (selected_item == 4) { // Back option
                return SETTINGS_BACK;
            }
        }
        
        if ((ctrl.buttons & SCE_CTRL_CIRCLE) && !(last_buttons & SCE_CTRL_CIRCLE)) {
            return SETTINGS_BACK;
        }
        
        return SETTINGS_CONTINUE;
    }
    
    void render() {
        // Render title
        renderer->render_text_gpu("Settings", 100, 80, RGBA8(0, 0, 0, 255), 32);
        
        // Render settings items
        int y_start = 150;
        int y_spacing = 40;
        
        for (size_t i = 0; i < setting_items.size(); ++i) {
            bool selected = (i == selected_item);
            int y_pos = y_start + i * y_spacing;
            
            // Render setting name
            renderer->render_menu_item(setting_items[i], 150, y_pos, selected);
            
            // Render setting value
            if (i < 4) { // Don't render value for "Back" option
                std::string value = get_setting_value(i);
                renderer->render_text_gpu(value, 400, y_pos, RGBA8(0, 0, 0, 255), 20);
            }
        }
        
        // Render instructions
        renderer->render_text_gpu("Use D-Pad to navigate, Left/Right to adjust values", 100, 450, RGBA8(100, 100, 100, 255), 16);
        renderer->render_text_gpu("X to select, O to go back", 100, 470, RGBA8(100, 100, 100, 255), 16);
    }
    
    // Getters for settings values
    int get_font_size() const { return font_size; }
    int get_line_spacing() const { return line_spacing; }
    bool get_auto_scroll() const { return auto_scroll; }
    int get_scroll_speed() const { return scroll_speed; }
    
private:
    void adjust_setting(int direction) {
        switch (selected_item) {
            case 0: // Font Size
                font_size += direction * 2;
                if (font_size < 12) font_size = 12;
                if (font_size > 32) font_size = 32;
                break;
            case 1: // Line Spacing
                line_spacing += direction;
                if (line_spacing < 0) line_spacing = 0;
                if (line_spacing > 10) line_spacing = 10;
                break;
            case 2: // Auto Scroll
                auto_scroll = !auto_scroll;
                break;
            case 3: // Scroll Speed
                scroll_speed += direction;
                if (scroll_speed < 1) scroll_speed = 1;
                if (scroll_speed > 10) scroll_speed = 10;
                break;
        }
    }
    
    std::string get_setting_value(size_t index) {
        switch (index) {
            case 0: return std::to_string(font_size) + "px";
            case 1: return std::to_string(line_spacing) + "px";
            case 2: return auto_scroll ? "On" : "Off";
            case 3: return std::to_string(scroll_speed);
            default: return "";
        }
    }
};