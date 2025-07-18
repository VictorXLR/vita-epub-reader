cmake_minimum_required(VERSION 3.5)

# Vita toolchain setup
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

# Project definition and metadata
project(epub_reader)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "EPUB Reader")
set(VITA_TITLEID  "EPUBREDR1")  # Exactly 9 characters: XXXXYYYYY format
set(VITA_VERSION  "01.00")

# Compiler optimization flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -O3 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -Wall")

# Include directories
include_directories(
  src/
  include/
)

# Source files compilation
add_executable(${PROJECT_NAME}
  src/main.cpp
  src/epub/parser.cpp
  src/epub/renderer.cpp  
  src/epub/navigation.cpp
  src/ui/menu.cpp
  src/ui/reader.cpp
  src/ui/settings.cpp
  src/network/downloader.cpp
  src/network/ssl_handler.cpp
  src/memory/memory_manager.cpp
  src/graphics/gpu_renderer.cpp
  src/file_manager.cpp
)

# System libraries and stubs
target_link_libraries(${PROJECT_NAME}
  # Standard C++ libraries
  stdc++
  pthread
  m
  
  # Vita system stubs
  SceDisplay_stub       # Display management
  SceCtrl_stub         # Controller input
  SceGxm_stub          # Graphics API
  SceIo_stub           # File I/O operations
  SceLibKernel_stub    # Kernel functions
  SceSysmodule_stub    # Module management
  SceNet_stub          # Network base
  SceNetCtl_stub       # Network control
  SceHttp_stub         # HTTP functionality
  SceCommonDialog_stub # System dialogs
  SceTouch_stub        # Touch input
  
  # Third-party libraries for EPUB functionality
  freetype             # Font rendering
  png                  # PNG image support
  jpeg                 # JPEG image support
  zip                  # ZIP archive handling
  curl                 # HTTP/HTTPS client
  ssl                  # SSL/TLS support
  crypto               # Cryptographic functions
  z                    # Compression library
  vita2d               # 2D graphics library
)

# Build SELF and VPK packages
vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  
  # LiveArea assets
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
  
  # Application assets
  FILE assets/fonts/default.ttf assets/fonts/default.ttf
  FILE assets/sample_books/sample.epub assets/sample_books/sample.epub
)