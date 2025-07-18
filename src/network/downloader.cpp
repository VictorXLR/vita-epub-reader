#include "downloader.h"
#include <iostream>
#include <fstream>
#include <psp2/io/fcntl.h>

bool EPUBDownloader::initialize() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_handle = curl_easy_init();
    
    if (!curl_handle) {
        return false;
    }
    
    // Basic curl configuration
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "PS Vita EPUB Reader/1.0");
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 300L); // 5 minute timeout
    curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 30L);
    
    // SSL configuration
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 2L);
    
    // Try to use system CA bundle or custom one
    curl_easy_setopt(curl_handle, CURLOPT_CAINFO, "ux0:data/epub_reader/certs/ca-bundle.crt");
    
    // Callbacks
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_handle, CURLOPT_PROGRESSFUNCTION, progress_callback_internal);
    curl_easy_setopt(curl_handle, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
    
    return verify_ssl_setup();
}

bool EPUBDownloader::download_epub(const std::string& url, const std::string& destination, ProgressCallback callback) {
    progress_callback = callback;
    current_progress = DownloadProgress(); // Reset progress
    start_time = std::chrono::steady_clock::now();
    
    // Ensure destination directory exists
    size_t last_slash = destination.find_last_of('/');
    if (last_slash != std::string::npos) {
        std::string dir = destination.substr(0, last_slash);
        // Create directory (implementation depends on available functions)
        sceIoMkdir(dir.c_str(), 0777);
    }
    
    // Open output file
    std::ofstream output_file(destination, std::ios::binary);
    if (!output_file.is_open()) {
        current_progress.error_message = "Cannot create output file: " + destination;
        return false;
    }
    
    // Set URL and output file
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &output_file);
    
    // Perform download
    CURLcode res = curl_easy_perform(curl_handle);
    
    output_file.close();
    
    if (res != CURLE_OK) {
        current_progress.error_message = curl_easy_strerror(res);
        // Remove partial file
        sceIoRemove(destination.c_str());
        return false;
    }
    
    // Check HTTP response code
    long response_code;
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    
    if (response_code >= 400) {
        current_progress.error_message = "HTTP error: " + std::to_string(response_code);
        sceIoRemove(destination.c_str());
        return false;
    }
    
    current_progress.completed = true;
    if (progress_callback) {
        progress_callback(current_progress);
    }
    
    return true;
}

void EPUBDownloader::cancel_download() {
    current_progress.cancelled = true;
}

size_t EPUBDownloader::write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* output_file = static_cast<std::ofstream*>(userp);
    size_t total_size = size * nmemb;
    
    output_file->write(static_cast<const char*>(contents), total_size);
    
    return total_size;
}

int EPUBDownloader::progress_callback_internal(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
    EPUBDownloader* downloader = static_cast<EPUBDownloader*>(clientp);
    
    if (downloader->current_progress.cancelled) {
        return 1; // Abort download
    }
    
    downloader->current_progress.total_bytes = static_cast<size_t>(dltotal);
    downloader->current_progress.downloaded_bytes = static_cast<size_t>(dlnow);
    
    if (dltotal > 0) {
        // Calculate download speed
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - downloader->start_time).count();
        
        if (elapsed > 0) {
            downloader->current_progress.speed_bytes_per_sec = dlnow / elapsed;
        }
    }
    
    if (downloader->progress_callback) {
        downloader->progress_callback(downloader->current_progress);
    }
    
    return 0; // Continue download
}

bool EPUBDownloader::verify_ssl_setup() {
    // For now, assume SSL is properly configured
    // In a full implementation, we would check for certificates
    return true;
}

void EPUBDownloader::cleanup() {
    if (curl_handle) {
        curl_easy_cleanup(curl_handle);
        curl_handle = nullptr;
    }
    curl_global_cleanup();
}