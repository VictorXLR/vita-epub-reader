#!/bin/bash
# deploy.sh - Automated build and deployment script for EPUB Reader

set -e  # Exit on any error

echo "=== PlayStation Vita EPUB Reader Build & Deploy ==="
echo

# Check if VitaSDK is installed
if [ -z "$VITASDK" ]; then
    echo "ERROR: VITASDK environment variable not set"
    echo "Please install VitaSDK and set the VITASDK environment variable"
    exit 1
fi

echo "Using VitaSDK at: $VITASDK"

# Clean previous build
echo "Cleaning previous build..."
rm -rf build/
mkdir -p build

# Build the project
echo "Building EPUB Reader..."
cd build

# Configure with VitaSDK
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compile (use all available cores)
CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
echo "Compiling with $CORES cores..."
make -j$CORES

# Check if build was successful
if [ ! -f "epub_reader.vpk" ]; then
    echo "ERROR: Build failed - epub_reader.vpk not found"
    exit 1
fi

echo "Build completed successfully!"
echo "Generated files:"
ls -la *.vpk *.self 2>/dev/null || true

# Deployment
VITA_IP="${1:-}"

if [ -z "$VITA_IP" ]; then
    echo
    echo "VPK file ready for installation: $(pwd)/epub_reader.vpk"
    echo
    echo "To deploy to Vita:"
    echo "  1. Start FTP server on Vita (VitaShell > SELECT button)"
    echo "  2. Run: $0 <vita_ip_address>"
    echo "  3. Or manually copy the VPK file to your Vita"
    echo
    echo "Installation instructions:"
    echo "  1. Copy epub_reader.vpk to ux0:/vpk/ on your Vita"
    echo "  2. Install using VitaShell (X button on the VPK file)"
    echo "  3. Accept extended permissions if prompted"
    echo
    exit 0
fi

echo
echo "Attempting to deploy to Vita at $VITA_IP..."

# Test connection
if ! ping -c 1 -W 3 "$VITA_IP" >/dev/null 2>&1; then
    echo "ERROR: Cannot reach Vita at $VITA_IP"
    echo "Make sure:"
    echo "  1. Vita is connected to the same network"
    echo "  2. FTP server is running (VitaShell > SELECT button)"
    echo "  3. IP address is correct"
    exit 1
fi

# Upload VPK
echo "Uploading VPK to Vita..."
if command -v curl >/dev/null 2>&1; then
    curl -T epub_reader.vpk "ftp://$VITA_IP:1337/ux0:/vpk/" || {
        echo "ERROR: Upload failed"
        echo "Make sure FTP server is running on Vita (VitaShell > SELECT)"
        exit 1
    }
else
    echo "ERROR: curl not found"
    echo "Please install curl or manually transfer the VPK file"
    exit 1
fi

echo
echo "=== Deployment Complete ==="
echo "VPK uploaded successfully to Vita at $VITA_IP"
echo
echo "Next steps:"
echo "  1. On your Vita, navigate to ux0:/vpk/ in VitaShell"
echo "  2. Press X on epub_reader.vpk to install"
echo "  3. Accept extended permissions if prompted"
echo "  4. Return to LiveArea to launch EPUB Reader"
echo
echo "Troubleshooting:"
echo "  - If installation fails, check Vita firmware version"
echo "  - Ensure HENkaku/Ensō is properly installed"
echo "  - Check available storage space on Vita"
echo