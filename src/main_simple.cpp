#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <vita2d.h>
#include <iostream>

int main() {
    std::cout << "PlayStation Vita EPUB Reader - Simple Test" << std::endl;
    
    // Initialize vita2d
    vita2d_init();
    vita2d_set_clear_color(RGBA8(255, 255, 255, 255));
    
    // Main loop
    while (true) {
        SceCtrlData ctrl;
        sceCtrlPeekBufferPositive(0, &ctrl, 1);
        
        if (ctrl.buttons & SCE_CTRL_START) {
            break;
        }
        
        vita2d_start_drawing();
        vita2d_clear_screen();
        
        // Simple text rendering test
        vita2d_font* font = vita2d_load_default_font();
        if (font) {
            vita2d_font_draw_text(font, 100, 100, RGBA8(0, 0, 0, 255), 20, "EPUB Reader Test");
            vita2d_font_draw_text(font, 100, 130, RGBA8(0, 0, 0, 255), 16, "Press START to exit");
        }
        
        vita2d_end_drawing();
        vita2d_swap_buffers();
        
        sceKernelDelayThread(16667); // ~60 FPS
    }
    
    vita2d_fini();
    
    std::cout << "EPUB Reader test exiting..." << std::endl;
    sceKernelExitProcess(0);
    return 0;
}