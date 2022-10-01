# Rasteron

A raster image loader, manipulation, and generation project. Supports PNG, TIFF, and BMP file formats. Windows is supported with a Linux port in progress.

zlib, libpng, and libtiff are built by the project and freetype is built optionally to enable font rasterisation.

Project Structure:

1. **core** contains core items necessary for generating noise images (Noise), cell algorithm images (Cellwise), and others (Image), with support for types including coordinate (pixelpoint) and volumetric (geotype)
2. **plugin** contains plugin functionality for optional animation support and font format loading
3. **helper** contains helper files for Windows and Unix specific functionality
4  **assets** contains fonts, images, and other resources