#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>
#include <psp2/io/fcntl.h>
#include <psp2/io/devctl.h>
#include <psp2/io/dirent.h>

class FileManager {
public:
    static const std::string EPUB_DIR;
    static const std::string CONFIG_DIR;
    static const std::string CACHE_DIR;
    static const std::string CERT_DIR;
    
    static bool initialize_directories();
    static std::vector<std::string> list_epub_files();
    static bool check_free_space(size_t required_bytes);
    static bool create_directory(const std::string& path);
    static bool file_exists(const std::string& path);
    static size_t get_file_size(const std::string& path);
    static bool delete_file(const std::string& path);
    static std::string get_epub_metadata(const std::string& path);
    
private:
    static bool create_directory_recursive(const std::string& path);
};

#endif // FILE_MANAGER_H