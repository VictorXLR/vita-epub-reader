#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <curl/curl.h>
#include <string>
#include <functional>
#include <chrono>

class EPUBDownloader {
public:
    struct DownloadProgress {
        size_t total_bytes;
        size_t downloaded_bytes;
        double speed_bytes_per_sec;
        bool completed;
        bool cancelled;
        std::string error_message;
        
        DownloadProgress() : total_bytes(0), downloaded_bytes(0), speed_bytes_per_sec(0.0), 
                           completed(false), cancelled(false) {}
    };
    
    using ProgressCallback = std::function<void(const DownloadProgress&)>;

private:
    CURL* curl_handle;
    ProgressCallback progress_callback;
    DownloadProgress current_progress;
    std::chrono::steady_clock::time_point start_time;
    
public:
    bool initialize();
    bool download_epub(const std::string& url, const std::string& destination, ProgressCallback callback = nullptr);
    void cancel_download();
    void cleanup();
    
private:
    static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp);
    static int progress_callback_internal(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow);
    bool verify_ssl_setup();
};

#endif // DOWNLOADER_H