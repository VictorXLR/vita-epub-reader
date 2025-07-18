# VPK Assets

This directory contains the PlayStation Vita VPK (Vita Package) assets.

## Required Files

### icon0.png
- Size: 128x128 pixels
- Format: PNG-8 with indexed colors
- Purpose: Application icon displayed in LiveArea

### livearea/contents/bg.png
- Size: 960x544 pixels
- Format: PNG
- Purpose: Background image for LiveArea

### livearea/contents/startup.png
- Size: 960x544 pixels (can be smaller)
- Format: PNG
- Purpose: Loading screen image

### livearea/contents/template.xml
- Format: XML
- Purpose: LiveArea layout configuration

## Creating Assets

Use image editing software like GIMP, Photoshop, or online tools to create these images.

### Icon Guidelines
- Use simple, recognizable imagery
- Ensure good contrast and readability at small sizes
- Consider the PlayStation Vita's OLED screen characteristics

### Background Guidelines
- Use high-quality images that represent your app
- Avoid overly busy or cluttered designs
- Test on actual hardware for best results

## Installation

These assets are automatically included in the VPK file during the build process.