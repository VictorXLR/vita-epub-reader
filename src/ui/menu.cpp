#include <psp2/ctrl.h>
#include "gpu_renderer.h"
#include <vector>
#include <string>

class MainMenu {
private:
    int selected_item;
    std::vector<std::string> menu_items;
    GPURenderer* renderer;
    
public:
    enum MenuResult {
        MENU_CONTINUE,
        MENU_READ_BOOKS,
        MENU_DOWNLOAD_BOOKS,
        MENU_SETTINGS,
        MENU_EXIT
    };
    
    MainMenu(GPURenderer* gpu_renderer) : selected_item(0), renderer(gpu_renderer) {
        menu_items = {
            "Read Books",
            "Download Books",
            "Settings",
            "Exit"
        };
    }
    
    MenuResult update(const SceCtrlData& ctrl, uint32_t last_buttons) {
        // Menu navigation
        if ((ctrl.buttons & SCE_CTRL_UP) && !(last_buttons & SCE_CTRL_UP)) {
            selected_item = (selected_item - 1 + menu_items.size()) % menu_items.size();
        }
        if ((ctrl.buttons & SCE_CTRL_DOWN) && !(last_buttons & SCE_CTRL_DOWN)) {
            selected_item = (selected_item + 1) % menu_items.size();
        }
        
        // Menu selection
        if ((ctrl.buttons & SCE_CTRL_CROSS) && !(last_buttons & SCE_CTRL_CROSS)) {
            switch (selected_item) {
                case 0: return MENU_READ_BOOKS;
                case 1: return MENU_DOWNLOAD_BOOKS;
                case 2: return MENU_SETTINGS;
                case 3: return MENU_EXIT;
            }
        }
        
        return MENU_CONTINUE;
    }
    
    void render() {
        // Render title
        renderer->render_text_gpu("EPUB Reader", 100, 80, RGBA8(0, 0, 0, 255), 32);
        
        // Render menu items
        int y_start = 200;
        int y_spacing = 50;
        
        for (size_t i = 0; i < menu_items.size(); ++i) {
            bool selected = (i == selected_item);
            renderer->render_menu_item(menu_items[i], 150, y_start + i * y_spacing, selected);
        }
        
        // Render instructions
        renderer->render_text_gpu("Use D-Pad to navigate, X to select", 100, 450, RGBA8(100, 100, 100, 255), 16);
    }
};