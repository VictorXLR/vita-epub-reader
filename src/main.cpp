#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <psp2/sysmodule.h>
#include <vita2d.h>
#include <iostream>

// Include all our components
#include "epub_parser.h"
#include "text_renderer.h"
#include "downloader.h"
#include "memory_manager.h"
#include "file_manager.h"
#include "gpu_renderer.h"

// Include UI components
#include "ui/menu.cpp"
#include "ui/reader.cpp"
#include "ui/settings.cpp"
#include "epub/navigation.cpp"

class EPUBReaderApp {
private:
    EPUBParser epub_parser;
    TextRenderer text_renderer;
    EPUBDownloader downloader;
    MemoryManager memory_manager;
    GPURenderer gpu_renderer;
    
    // UI components
    MainMenu* main_menu;
    BookList* book_list;
    BookReader* book_reader;
    SettingsMenu* settings_menu;
    
    // Application state
    enum AppState {
        MAIN_MENU,
        BOOK_LIST,
        READING,
        DOWNLOADING,
        SETTINGS
    } current_state;
    
    uint32_t last_buttons;
    
public:
    EPUBReaderApp() : current_state(MAIN_MENU), last_buttons(0) {
        main_menu = nullptr;
        book_list = nullptr;
        book_reader = nullptr;
        settings_menu = nullptr;
    }
    
    bool initialize() {
        std::cout << "Initializing EPUB Reader..." << std::endl;
        
        // Initialize system modules
        sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
        sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);
        
        // Initialize application components
        if (!memory_manager.initialize()) {
            std::cerr << "Failed to initialize memory manager" << std::endl;
            return false;
        }
        
        if (!FileManager::initialize_directories()) {
            std::cerr << "Failed to initialize file manager" << std::endl;
            return false;
        }
        
        if (!gpu_renderer.initialize()) {
            std::cerr << "Failed to initialize GPU renderer" << std::endl;
            return false;
        }
        
        if (!text_renderer.initialize("assets/fonts/default.ttf")) {
            std::cerr << "Failed to initialize text renderer" << std::endl;
            return false;
        }
        
        if (!downloader.initialize()) {
            std::cerr << "Failed to initialize downloader" << std::endl;
            return false;
        }
        
        // Initialize UI components
        main_menu = new MainMenu(&gpu_renderer);
        book_list = new BookList(&gpu_renderer);
        book_reader = new BookReader(&gpu_renderer, &epub_parser);
        settings_menu = new SettingsMenu(&gpu_renderer);
        
        current_state = MAIN_MENU;
        
        std::cout << "EPUB Reader initialized successfully!" << std::endl;
        return true;
    }
    
    void main_loop() {
        std::cout << "Starting main loop..." << std::endl;
        
        while (true) {
            // Read input
            SceCtrlData ctrl;
            sceCtrlPeekBufferPositive(0, &ctrl, 1);
            
            // Check for global exit
            if (ctrl.buttons & SCE_CTRL_START) {
                std::cout << "Exit requested by user" << std::endl;
                break;
            }
            
            // Update application state
            switch (current_state) {
                case MAIN_MENU:
                    handle_main_menu(ctrl);
                    break;
                case BOOK_LIST:
                    handle_book_list(ctrl);
                    break;
                case READING:
                    handle_reading(ctrl);
                    break;
                case DOWNLOADING:
                    handle_downloading(ctrl);
                    break;
                case SETTINGS:
                    handle_settings(ctrl);
                    break;
            }
            
            // Render current state
            gpu_renderer.begin_frame();
            render_current_state();
            gpu_renderer.end_frame();
            
            // Update last buttons state
            last_buttons = ctrl.buttons;
            
            // ~60 FPS
            sceKernelDelayThread(16667);
        }
    }
    
private:
    void handle_main_menu(const SceCtrlData& ctrl) {
        MainMenu::MenuResult result = main_menu->update(ctrl, last_buttons);
        
        switch (result) {
            case MainMenu::MENU_READ_BOOKS:
                book_list->refresh_book_list();
                current_state = BOOK_LIST;
                break;
            case MainMenu::MENU_DOWNLOAD_BOOKS:
                current_state = DOWNLOADING;
                break;
            case MainMenu::MENU_SETTINGS:
                current_state = SETTINGS;
                break;
            case MainMenu::MENU_EXIT:
                sceKernelExitProcess(0);
                break;
            case MainMenu::MENU_CONTINUE:
                break;
        }
    }
    
    void handle_book_list(const SceCtrlData& ctrl) {
        BookList::BookListResult result = book_list->update(ctrl, last_buttons);
        
        switch (result) {
            case BookList::BOOKLIST_BACK:
                current_state = MAIN_MENU;
                break;
            case BookList::BOOKLIST_OPEN_BOOK: {
                std::string book_path = book_list->get_selected_book_path();
                if (!book_path.empty() && epub_parser.open_epub(book_path)) {
                    book_reader->load_chapter(0);
                    current_state = READING;
                } else {
                    std::cerr << "Failed to open EPUB file: " << book_path << std::endl;
                }
                break;
            }
            case BookList::BOOKLIST_CONTINUE:
                break;
        }
    }
    
    void handle_reading(const SceCtrlData& ctrl) {
        BookReader::ReaderResult result = book_reader->update(ctrl, last_buttons);
        
        switch (result) {
            case BookReader::READER_BACK_TO_MENU:
                epub_parser.close();
                current_state = BOOK_LIST;
                break;
            case BookReader::READER_CONTINUE:
                break;
            default:
                break;
        }
    }
    
    void handle_downloading(const SceCtrlData& ctrl) {
        // Simple download interface - for now just go back to menu
        if ((ctrl.buttons & SCE_CTRL_CIRCLE) && !(last_buttons & SCE_CTRL_CIRCLE)) {
            current_state = MAIN_MENU;
        }
    }
    
    void handle_settings(const SceCtrlData& ctrl) {
        SettingsMenu::SettingsResult result = settings_menu->update(ctrl, last_buttons);
        
        switch (result) {
            case SettingsMenu::SETTINGS_BACK:
                current_state = MAIN_MENU;
                break;
            case SettingsMenu::SETTINGS_CONTINUE:
                break;
        }
    }
    
    void render_current_state() {
        switch (current_state) {
            case MAIN_MENU:
                main_menu->render();
                break;
            case BOOK_LIST:
                book_list->render();
                break;
            case READING:
                book_reader->render();
                break;
            case DOWNLOADING:
                render_download_screen();
                break;
            case SETTINGS:
                settings_menu->render();
                break;
        }
    }
    
    void render_download_screen() {
        gpu_renderer.render_text_gpu("Download Books", 100, 80, RGBA8(0, 0, 0, 255), 32);
        gpu_renderer.render_text_gpu("Download functionality not implemented yet", 150, 200, RGBA8(100, 100, 100, 255), 20);
        gpu_renderer.render_text_gpu("Press O to go back", 150, 240, RGBA8(100, 100, 100, 255), 16);
    }
    
public:
    void cleanup() {
        std::cout << "Cleaning up EPUB Reader..." << std::endl;
        
        epub_parser.close();
        text_renderer.clear_cache();
        downloader.cleanup();
        gpu_renderer.cleanup();
        memory_manager.cleanup();
        
        // Clean up UI components
        delete main_menu;
        delete book_list;
        delete book_reader;
        delete settings_menu;
        
        std::cout << "Cleanup complete" << std::endl;
    }
};

int main() {
    std::cout << "PlayStation Vita EPUB Reader starting..." << std::endl;
    
    EPUBReaderApp app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize application" << std::endl;
        sceKernelExitProcess(-1);
    }
    
    app.main_loop();
    app.cleanup();
    
    std::cout << "EPUB Reader exiting..." << std::endl;
    sceKernelExitProcess(0);
    return 0;
}