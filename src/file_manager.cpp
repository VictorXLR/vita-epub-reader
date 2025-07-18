#include "file_manager.h"
#include <iostream>
#include <algorithm>

const std::string FileManager::EPUB_DIR = "ux0:data/epub_reader/books";
const std::string FileManager::CONFIG_DIR = "ux0:data/epub_reader/config";
const std::string FileManager::CACHE_DIR = "ux0:data/epub_reader/cache";
const std::string FileManager::CERT_DIR = "ux0:data/epub_reader/certs";

bool FileManager::initialize_directories() {
    // Create base application directory
    if (sceIoMkdir("ux0:data", 0777) < 0) {
        // Directory might already exist, which is fine
    }
    
    if (sceIoMkdir("ux0:data/epub_reader", 0777) < 0) {
        // Directory might already exist, which is fine
    }
    
    // Create subdirectories
    bool success = true;
    success &= create_directory(EPUB_DIR);
    success &= create_directory(CONFIG_DIR);
    success &= create_directory(CACHE_DIR);
    success &= create_directory(CERT_DIR);
    
    if (success) {
        std::cout << "File manager directories initialized successfully" << std::endl;
    } else {
        std::cerr << "Failed to initialize some directories" << std::endl;
    }
    
    return success;
}

bool FileManager::create_directory(const std::string& path) {
    int result = sceIoMkdir(path.c_str(), 0777);
    return result >= 0 || result == SCE_ERROR_ERRNO_EEXIST; // Success or already exists
}

std::vector<std::string> FileManager::list_epub_files() {
    std::vector<std::string> files;
    SceUID dir = sceIoDopen(EPUB_DIR.c_str());
    
    if (dir >= 0) {
        SceIoDirent dirent;
        while (sceIoDread(dir, &dirent) > 0) {
            std::string filename = dirent.d_name;
            
            // Skip hidden files and directories
            if (filename[0] == '.') {
                continue;
            }
            
            // Check if file is an EPUB
            if (filename.length() > 5 && filename.substr(filename.length() - 5) == ".epub") {
                files.push_back(EPUB_DIR + "/" + filename);
            }
        }
        
        sceIoDclose(dir);
    } else {
        std::cerr << "Failed to open EPUB directory: " << EPUB_DIR << std::endl;
    }
    
    // Sort files alphabetically
    std::sort(files.begin(), files.end());
    
    return files;
}

bool FileManager::check_free_space(size_t required_bytes) {
    SceIoDevInfo info;
    int ret = sceIoDevctl("ux0:", 0x3001, NULL, 0, &info, sizeof(info));
    
    if (ret >= 0) {
        uint64_t free_bytes = info.free_size;
        return free_bytes >= required_bytes;
    }
    
    // If we can't check free space, assume we have enough
    // This is a conservative approach to avoid blocking downloads
    return true;
}

bool FileManager::file_exists(const std::string& path) {
    SceIoStat stat;
    int result = sceIoGetstat(path.c_str(), &stat);
    return result >= 0;
}

size_t FileManager::get_file_size(const std::string& path) {
    SceIoStat stat;
    int result = sceIoGetstat(path.c_str(), &stat);
    
    if (result >= 0) {
        return static_cast<size_t>(stat.st_size);
    }
    
    return 0;
}

bool FileManager::delete_file(const std::string& path) {
    int result = sceIoRemove(path.c_str());
    return result >= 0;
}

std::string FileManager::get_epub_metadata(const std::string& path) {
    // This would extract basic metadata from the EPUB file
    // For now, just return the filename
    size_t last_slash = path.find_last_of('/');
    if (last_slash != std::string::npos) {
        std::string filename = path.substr(last_slash + 1);
        // Remove .epub extension
        if (filename.length() > 5 && filename.substr(filename.length() - 5) == ".epub") {
            filename = filename.substr(0, filename.length() - 5);
        }
        return filename;
    }
    
    return path;
}

bool FileManager::create_directory_recursive(const std::string& path) {
    // Split path into components and create each directory
    std::vector<std::string> components;
    std::string current_path;
    
    for (size_t i = 0; i < path.length(); ++i) {
        if (path[i] == '/') {
            if (!current_path.empty()) {
                components.push_back(current_path);
                current_path.clear();
            }
        } else {
            current_path += path[i];
        }
    }
    
    if (!current_path.empty()) {
        components.push_back(current_path);
    }
    
    // Create each directory in sequence
    std::string build_path;
    for (const auto& component : components) {
        if (!build_path.empty()) {
            build_path += "/";
        }
        build_path += component;
        
        if (!create_directory(build_path)) {
            return false;
        }
    }
    
    return true;
}