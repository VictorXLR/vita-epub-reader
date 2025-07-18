#include "epub_parser.h"
#include <iostream>
#include <cstring>

bool EPUBParser::open_epub(const std::string& path) {
    archive = zip_open(path.c_str(), ZIP_RDONLY, nullptr);
    if (!archive) {
        std::cerr << "Failed to open EPUB file: " << path << std::endl;
        return false;
    }
    
    return parse_container();
}

bool EPUBParser::parse_container() {
    // First, read META-INF/container.xml to find OPF location
    std::string container_xml = extract_file("META-INF/container.xml");
    if (container_xml.empty()) return false;
    
    tinyxml2::XMLDocument doc;
    if (doc.Parse(container_xml.c_str()) != tinyxml2::XML_SUCCESS) return false;
    
    auto container = doc.FirstChildElement("container");
    if (!container) return false;
    
    auto rootfiles = container->FirstChildElement("rootfiles");
    if (!rootfiles) return false;
    
    auto rootfile = rootfiles->FirstChildElement("rootfile");
    if (!rootfile) return false;
    
    const char* opf_path_attr = rootfile->Attribute("full-path");
    if (!opf_path_attr) return false;
    
    std::string opf_path = opf_path_attr;
    
    // Extract directory path for relative file resolution
    size_t last_slash = opf_path.find_last_of('/');
    if (last_slash != std::string::npos) {
        container_root = opf_path.substr(0, last_slash + 1);
    }
    
    return parse_opf(opf_path);
}

std::string EPUBParser::extract_file(const std::string& path) {
    zip_file_t* file = zip_fopen(archive, path.c_str(), 0);
    if (!file) return "";
    
    // Get file size
    zip_stat_t stat;
    if (zip_stat(archive, path.c_str(), 0, &stat) != 0) {
        zip_fclose(file);
        return "";
    }
    
    // Read file content
    std::string content(stat.size, '\0');
    zip_int64_t bytes_read = zip_fread(file, &content[0], stat.size);
    zip_fclose(file);
    
    if (bytes_read != static_cast<zip_int64_t>(stat.size)) {
        return "";
    }
    
    return content;
}

bool EPUBParser::parse_opf(const std::string& opf_path) {
    std::string opf_content = extract_file(opf_path);
    if (opf_content.empty()) return false;
    
    tinyxml2::XMLDocument doc;
    if (doc.Parse(opf_content.c_str()) != tinyxml2::XML_SUCCESS) return false;
    
    auto package = doc.FirstChildElement("package");
    if (!package) return false;
    
    // Parse manifest
    auto manifest_elem = package->FirstChildElement("manifest");
    if (manifest_elem) {
        for (auto item = manifest_elem->FirstChildElement("item");
             item; item = item->NextSiblingElement("item")) {
            
            const char* id_attr = item->Attribute("id");
            const char* href_attr = item->Attribute("href");
            const char* media_type_attr = item->Attribute("media-type");
            
            if (id_attr && href_attr && media_type_attr) {
                ManifestItem manifest_item;
                manifest_item.id = id_attr;
                manifest_item.href = href_attr;
                manifest_item.media_type = media_type_attr;
                
                manifest[manifest_item.id] = manifest_item;
            }
        }
    }
    
    // Parse spine (reading order)
    auto spine_elem = package->FirstChildElement("spine");
    if (spine_elem) {
        for (auto itemref = spine_elem->FirstChildElement("itemref");
             itemref; itemref = itemref->NextSiblingElement("itemref")) {
            
            const char* idref_attr = itemref->Attribute("idref");
            if (idref_attr) {
                SpineItem spine_item;
                spine_item.idref = idref_attr;
                spine_item.linear = true; // Default value
                
                const char* linear_attr = itemref->Attribute("linear");
                if (linear_attr && std::string(linear_attr) == "no") {
                    spine_item.linear = false;
                }
                
                spine.push_back(spine_item);
            }
        }
    }
    
    // Find and parse NCX file for table of contents
    std::string ncx_path;
    for (const auto& item : manifest) {
        if (item.second.media_type == "application/x-dtbncx+xml") {
            ncx_path = container_root + item.second.href;
            break;
        }
    }
    
    if (!ncx_path.empty()) {
        parse_ncx(ncx_path);
    }
    
    return true;
}

bool EPUBParser::parse_ncx(const std::string& ncx_path) {
    std::string ncx_content = extract_file(ncx_path);
    if (ncx_content.empty()) return false;
    
    tinyxml2::XMLDocument doc;
    if (doc.Parse(ncx_content.c_str()) != tinyxml2::XML_SUCCESS) return false;
    
    auto ncx = doc.FirstChildElement("ncx");
    if (!ncx) return false;
    
    auto navMap = ncx->FirstChildElement("navMap");
    if (!navMap) return false;
    
    toc.clear();
    for (auto navPoint = navMap->FirstChildElement("navPoint");
         navPoint; navPoint = navPoint->NextSiblingElement("navPoint")) {
        
        TOCEntry entry = parse_nav_point(navPoint);
        if (!entry.title.empty()) {
            toc.push_back(entry);
        }
    }
    
    return true;
}

EPUBParser::TOCEntry EPUBParser::parse_nav_point(tinyxml2::XMLElement* navPoint) {
    TOCEntry entry;
    
    const char* id_attr = navPoint->Attribute("id");
    const char* playOrder_attr = navPoint->Attribute("playOrder");
    
    if (id_attr) entry.id = id_attr;
    if (playOrder_attr) entry.play_order = std::stoi(playOrder_attr);
    
    // Extract title
    auto navLabel = navPoint->FirstChildElement("navLabel");
    if (navLabel) {
        auto text = navLabel->FirstChildElement("text");
        if (text && text->GetText()) {
            entry.title = text->GetText();
        }
    }
    
    // Extract content source
    auto content = navPoint->FirstChildElement("content");
    if (content) {
        const char* src_attr = content->Attribute("src");
        if (src_attr) {
            entry.content_src = src_attr;
        }
    }
    
    // Parse child nav points
    for (auto childNavPoint = navPoint->FirstChildElement("navPoint");
         childNavPoint; childNavPoint = childNavPoint->NextSiblingElement("navPoint")) {
        
        TOCEntry child_entry = parse_nav_point(childNavPoint);
        if (!child_entry.title.empty()) {
            entry.children.push_back(child_entry);
        }
    }
    
    return entry;
}

std::string EPUBParser::get_content(const std::string& href) {
    std::string full_path = container_root + href;
    return extract_file(full_path);
}

const std::vector<EPUBParser::TOCEntry>& EPUBParser::get_table_of_contents() const {
    return toc;
}

const std::vector<EPUBParser::SpineItem>& EPUBParser::get_spine() const {
    return spine;
}

void EPUBParser::close() {
    if (archive) {
        zip_close(archive);
        archive = nullptr;
    }
    
    manifest.clear();
    spine.clear();
    toc.clear();
    container_root.clear();
}