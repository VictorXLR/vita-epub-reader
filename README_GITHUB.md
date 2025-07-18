# PlayStation Vita EPUB Reader

A fully-featured EPUB reader for PlayStation Vita homebrew, built with VitaSDK. This application provides a complete e-book reading experience optimized for the Vita's hardware.

## 🎮 Features

- **Full EPUB Support**: Complete EPUB parsing with chapter navigation
- **Optimized Text Rendering**: Hardware-accelerated text with custom font support
- **Memory Efficient**: Designed for PS Vita's 512MB RAM limitation
- **Touch & Button Controls**: Support for both D-pad and touchscreen navigation
- **Network Downloads**: Download EPUB books directly to your Vita
- **Customizable Settings**: Font size, line spacing, and reading preferences
- **GPU Accelerated**: Smooth 60fps rendering using vita2d

## 📱 Requirements

### PlayStation Vita
- Firmware 3.60-3.74 with HENkaku/Ensō
- At least 50MB free space
- WiFi connection (for downloads)

### Development
- VitaSDK installed
- CMake 3.10+
- Git

## 🚀 Installation

### For Users
1. Download the latest `epub_reader.vpk` from [Releases](../../releases)
2. Copy to `ux0:/vpk/` on your Vita
3. Install using VitaShell (press X on the VPK)
4. Launch from LiveArea

### For Developers
The project builds automatically using GitHub Actions. Check the [Actions tab](../../actions) for the latest builds.

## 🎯 Usage

### Adding Books
1. Copy `.epub` files to `ux0:data/epub_reader/books/` on your Vita
2. Launch EPUB Reader from LiveArea
3. Select books from your library

### Controls
- **D-Pad**: Menu navigation and page scrolling
- **X**: Select/confirm
- **O**: Back/cancel
- **Triangle**: Toggle UI overlay
- **L/R Triggers**: Previous/next chapter
- **START**: Exit application

### Reading Interface
- **Left/Right**: Scroll pages up/down
- **L/R**: Navigate between chapters
- **Triangle**: Show/hide reading UI
- **Circle**: Return to book list

## 🔧 Building

### GitHub Actions (Recommended)
This project uses GitHub Actions for automated building:

1. **Push to main branch** → Automatic build
2. **Create a release** → Build and attach VPK to release
3. **Manual trigger** → Build on demand

### Manual Build
```bash
# Clone the repository
git clone https://github.com/your-username/vita-epub-reader.git
cd vita-epub-reader

# Build (requires VitaSDK)
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake
make -j4
```

## 📦 Project Structure

```
epub_reader/
├── src/
│   ├── main.cpp              # Application entry point
│   ├── epub/                 # EPUB parsing and rendering
│   ├── ui/                   # User interface components
│   ├── network/              # Download functionality
│   ├── memory/               # Memory management
│   └── graphics/             # GPU rendering
├── include/                  # Header files
├── assets/                   # Fonts and sample books
├── sce_sys/                  # VPK system files
└── .github/workflows/        # GitHub Actions
```

## 🎨 Technical Details

### Architecture
- **Modular Design**: Separate components for parsing, rendering, and UI
- **Memory Pools**: Custom allocation for efficient memory usage
- **GPU Acceleration**: Hardware-accelerated 2D rendering
- **Asynchronous Loading**: Non-blocking file operations

### Dependencies
- **VitaSDK**: Core development framework
- **vita2d**: 2D graphics library
- **libzip**: ZIP archive handling
- **tinyxml2**: XML parsing
- **FreeType**: Font rendering
- **libcurl**: HTTP/HTTPS downloads

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on actual hardware
5. Submit a pull request

## 📝 License

MIT License - see [LICENSE](LICENSE) for details

## 🙏 Credits

- VitaSDK Team: Development framework
- vita2d: Graphics library
- Community: Testing and feedback

## 🐛 Support

- **Issues**: [GitHub Issues](../../issues)
- **Discussions**: [GitHub Discussions](../../discussions)
- **Wiki**: [Project Wiki](../../wiki)

## 📊 Build Status

![Build Status](../../actions/workflows/build-progressive.yml/badge.svg)

---

**Note**: This is homebrew software for educational purposes. Use at your own risk.