# PlayStation Vita EPUB Reader

A fully-featured EPUB reader for PlayStation Vita homebrew, built with VitaSDK. This application provides a complete e-book reading experience with text rendering, chapter navigation, and network download capabilities.

## Features

- **Full EPUB Support**: Parse and display EPUB files with proper chapter navigation
- **Advanced Text Rendering**: FreeType integration with glyph caching for smooth performance
- **Network Downloads**: Download EPUB files directly to your Vita via HTTP/HTTPS
- **Memory Optimized**: Custom memory management for the Vita's 512MB RAM limitation
- **GPU Accelerated**: Hardware-accelerated rendering using vita2d
- **Touch & Button Controls**: Support for both physical controls and touchscreen navigation
- **Settings Menu**: Customizable font size, line spacing, and reading preferences

## Requirements

### Development Environment
- **VitaSDK**: Complete PlayStation Vita development toolkit
- **CMake**: Version 3.2 or higher
- **Git**: For cloning dependencies

### PlayStation Vita Hardware
- **Firmware**: 3.60-3.74 with HENkaku/Ensō installed
- **Storage**: At least 50MB free space for application and books
- **Network**: WiFi connection for downloading books (optional)

## Installation

### For Users

1. Download the latest `epub_reader.vpk` from the releases page
2. Copy the VPK file to `ux0:/vpk/` on your Vita
3. Install using VitaShell (press X on the VPK file)
4. Accept extended permissions when prompted
5. Launch from LiveArea

### For Developers

1. **Install VitaSDK**:
   ```bash
   # Linux/WSL2
   export VITASDK=/usr/local/vitasdk
   export PATH=$VITASDK/bin:$PATH
   
   # Install using VDPM
   git clone https://github.com/vitasdk/vdpm
   cd vdpm
   ./bootstrap-vitasdk.sh
   ./install-all.sh
   ```

2. **Clone and Build**:
   ```bash
   git clone https://github.com/yourusername/vita-epub-reader.git
   cd vita-epub-reader
   ./deploy.sh
   ```

3. **Deploy to Vita**:
   ```bash
   # Start FTP server on Vita (VitaShell > SELECT)
   ./deploy.sh 192.168.1.XXX
   ```

## Usage

### Reading Books

1. **Add EPUB Files**: Copy `.epub` files to `ux0:data/epub_reader/books/` on your Vita
2. **Launch Application**: Start EPUB Reader from LiveArea
3. **Select Book**: Choose from your library using the D-pad
4. **Read**: Use the following controls:
   - **Left/Right**: Scroll up/down pages
   - **L/R Triggers**: Previous/next chapter
   - **Triangle**: Toggle UI overlay
   - **Circle**: Return to book list

### Navigation Controls

- **D-Pad**: Menu navigation and page scrolling
- **X Button**: Select/confirm
- **Circle Button**: Back/cancel
- **Triangle**: Context menu/UI toggle
- **L/R Triggers**: Chapter navigation
- **START**: Exit application

### Settings

Customize your reading experience:
- **Font Size**: 12-32px
- **Line Spacing**: 0-10px
- **Auto Scroll**: Enable/disable
- **Scroll Speed**: 1-10 (when auto-scroll is enabled)

## File Structure

```
epub_reader/
├── src/                    # Source code
│   ├── main.cpp           # Application entry point
│   ├── epub/              # EPUB parsing and rendering
│   ├── ui/                # User interface components
│   ├── network/           # Download functionality
│   ├── memory/            # Memory management
│   └── graphics/          # GPU rendering
├── include/               # Header files
├── assets/               # Application assets
│   ├── fonts/            # TTF font files
│   └── sample_books/     # Sample EPUB files
├── sce_sys/              # VPK system files
│   ├── icon0.png         # 128x128 app icon
│   └── livearea/         # LiveArea assets
└── CMakeLists.txt        # Build configuration
```

## Technical Details

### Architecture

The application uses a modular architecture with the following components:

- **EPUB Parser**: ZIP extraction and XML parsing using libzip and tinyxml2
- **Text Renderer**: FreeType integration with optimized glyph caching
- **Memory Manager**: Custom allocation pools for efficient memory usage
- **GPU Renderer**: Hardware-accelerated 2D rendering using vita2d
- **Network Downloader**: HTTP/HTTPS support with libcurl
- **File Manager**: Vita filesystem integration and organization

### Performance Optimizations

- **Memory Pools**: Pre-allocated memory blocks to prevent fragmentation
- **Glyph Caching**: Rendered character bitmaps cached for reuse
- **ARM NEON**: SIMD instructions for accelerated text blending
- **GPU Acceleration**: Hardware rendering for smooth 60fps performance
- **Viewport Culling**: Only render visible text lines

### Dependencies

- **VitaSDK**: Core development framework
- **vita2d**: 2D graphics library
- **libzip**: ZIP archive handling
- **tinyxml2**: XML parsing
- **FreeType**: Font rendering
- **libcurl**: HTTP client
- **OpenSSL**: SSL/TLS support

## Development

### Building

```bash
mkdir build && cd build
cmake ..
make -j4
```

### Testing

1. Use sample EPUB files from Project Gutenberg
2. Test with various file sizes and complexities
3. Verify memory usage stays within limits
4. Test network functionality on actual hardware

### Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly on actual hardware
5. Submit a pull request

## Troubleshooting

### Common Issues

**Build Errors:**
- Ensure VitaSDK is properly installed and in PATH
- Check all dependencies are available
- Verify CMake version is 3.2+

**Installation Fails:**
- Confirm HENkaku/Ensō is installed
- Check available storage space
- Verify VPK file integrity

**Books Won't Open:**
- Ensure EPUB files are valid and not corrupted
- Check file permissions
- Verify sufficient memory available

**Network Issues:**
- Confirm WiFi connection is active
- Check SSL certificate configuration
- Verify iTLS-Enso plugin if using HTTPS

### Debug Mode

Build with debug symbols:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
```

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## Credits

- **VitaSDK Team**: Development framework and tools
- **vita2d**: Graphics library by xerpi
- **FreeType**: Font rendering engine
- **libzip**: ZIP archive library
- **tinyxml2**: XML parsing library

## Support

For support and bug reports, please visit the [GitHub Issues](https://github.com/yourusername/vita-epub-reader/issues) page.

---

**Note**: This is homebrew software. Use at your own risk. The developers are not responsible for any damage to your device.