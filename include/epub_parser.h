#ifndef EPUB_PARSER_H
#define EPUB_PARSER_H

#include <zip.h>
#include <tinyxml2.h>
#include <string>
#include <vector>
#include <unordered_map>

class EPUBParser {
public:
    struct ManifestItem {
        std::string id;
        std::string href;
        std::string media_type;
    };
    
    struct SpineItem {
        std::string idref;
        bool linear;
    };
    
    struct TOCEntry {
        std::string id;
        std::string title;
        std::string content_src;
        int play_order;
        std::vector<TOCEntry> children;
    };

private:
    zip_t* archive;
    std::string container_root;
    std::unordered_map<std::string, ManifestItem> manifest;
    std::vector<SpineItem> spine;
    std::vector<TOCEntry> toc;

public:
    bool open_epub(const std::string& path);
    bool parse_container();
    bool parse_opf(const std::string& opf_path);
    bool parse_ncx(const std::string& ncx_path);
    std::string get_content(const std::string& href);
    const std::vector<TOCEntry>& get_table_of_contents() const;
    const std::vector<SpineItem>& get_spine() const;
    void close();
    
private:
    std::string extract_file(const std::string& path);
    TOCEntry parse_nav_point(tinyxml2::XMLElement* navPoint);
};

#endif // EPUB_PARSER_H