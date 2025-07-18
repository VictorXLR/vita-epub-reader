# Build Verification Guide

This document explains how to verify that the PS Vita EPUB Reader builds correctly and produces a working VPK file.

## GitHub Actions Workflows

### 1. Test Build Workflow (`.github/workflows/test-build.yml`)
**Purpose**: Quick verification that VPK compilation works
**Triggers**: 
- Manual trigger (workflow_dispatch)
- Push to main branch (when workflow files change)

**What it does**:
- Installs VitaSDK on Ubuntu
- Creates minimal placeholder assets
- Attempts simple build first (basic functionality)
- Attempts full build (complete EPUB reader)
- Uploads both VPK files as artifacts

### 2. Progressive Build Workflow (`.github/workflows/build-progressive.yml`)
**Purpose**: Full build with filesystem deployment
**Triggers**: Push to main, pull requests

**What it does**:
- Two-stage build process
- Creates filesystem deployment scripts
- Generates deployment packages for Windows and Linux/macOS

### 3. Build and Release Workflow (`.github/workflows/build-and-release.yml`)
**Purpose**: Release automation
**Triggers**: GitHub releases

## Build Verification Steps

### Step 1: Trigger Test Build
1. Go to your GitHub repository
2. Click "Actions" tab
3. Select "Test VPK Build" workflow
4. Click "Run workflow" → "Run workflow"

### Step 2: Monitor Build Progress
Watch the build logs to see:
- VitaSDK installation
- Asset creation
- Simple build attempt
- Full build attempt
- Build results summary

### Step 3: Check Build Results
After the build completes:
- Check the "Summary" section for build status
- Download artifacts to get VPK files
- Look for `epub_reader.vpk` in the downloaded artifacts

### Step 4: Test VPK File
Once you have the VPK:
1. **Size check**: VPK should be several MB in size
2. **Format check**: It should be a valid ZIP archive
3. **Content check**: Should contain SELF file and assets

## Expected Build Outputs

### Successful Simple Build
- `epub_reader.vpk` (test version)
- `epub_reader.self` (executable)
- Size: ~1-5 MB

### Successful Full Build
- `epub_reader.vpk` (complete EPUB reader)
- `epub_reader.self` (executable)
- Size: ~5-20 MB (depending on included assets)

## Build Troubleshooting

### Common Issues and Solutions

1. **VitaSDK Installation Fails**
   - Check internet connection
   - Verify VitaSDK URL is accessible
   - Look for dependency installation errors

2. **CMake Configuration Fails**
   - Check toolchain file path
   - Verify VitaSDK environment variables
   - Look for missing dependencies

3. **Compilation Fails**
   - Check for missing libraries
   - Verify source code syntax
   - Check library linking errors

4. **VPK Creation Fails**
   - Verify SELF file was created
   - Check asset file paths
   - Verify vita.cmake is working

### Debugging Steps

1. **Check Build Logs**
   - Look for error messages
   - Check compiler output
   - Verify library availability

2. **Verify Assets**
   - Ensure placeholder images are created
   - Check file permissions
   - Verify directory structure

3. **Test Components**
   - Try simple build first
   - Check individual source files
   - Verify CMakeLists.txt syntax

## Deployment After Build

### Filesystem Deployment (Recommended)
1. Download VPK from GitHub Actions artifacts
2. Connect Vita via USB
3. Enable USB mode in VitaShell (START + SELECT)
4. Copy VPK to `ux0:/vpk/` folder
5. Install via VitaShell

### Manual Verification
1. Check VPK file size (should be > 1MB)
2. Verify it's a valid ZIP: `unzip -l epub_reader.vpk`
3. Check for required files:
   - `eboot.bin` (the executable)
   - `sce_sys/param.sfo` (metadata)
   - `sce_sys/icon0.png` (icon)

## Success Criteria

✅ **Build succeeds**: No compilation errors
✅ **VPK created**: epub_reader.vpk file exists
✅ **Correct size**: VPK is several MB in size
✅ **Valid format**: VPK can be extracted as ZIP
✅ **Contains SELF**: eboot.bin is present
✅ **Installs on Vita**: VitaShell can install the VPK
✅ **Launches**: Application appears in LiveArea and starts

## Next Steps After Successful Build

1. **Test on real hardware**: Install and run on actual Vita
2. **Add EPUB files**: Copy sample books to test functionality
3. **Verify features**: Test reading interface, navigation, settings
4. **Performance testing**: Check memory usage and rendering speed
5. **User testing**: Get feedback from other Vita users

## Support

If builds fail consistently:
1. Check GitHub Actions logs for specific errors
2. Verify the VitaSDK installation process
3. Test with minimal source code changes
4. Report issues with full build logs

The goal is to have a working VPK file that can be installed and run on a PlayStation Vita with HENkaku/Ensō.