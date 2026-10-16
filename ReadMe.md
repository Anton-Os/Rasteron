# Rasteron

An image loader, manipulation, and synthesis project written in C, with CUDA amd video-encoding support coming soon! Windows and Linux are the target platforms, with bindings being cross-compiled for Flutter and mobile devices.

###  Enviornment

The external dependencies being built are mainly included to deal with various codecs.

1. zlib, libpng, libtiff and libjpeg for working with common image file formats
2. FreeType for rasterization of common font file formats

###  Project Structure:

1. **core** contains core functionality to work with image types
2. **plugin* contains extensible type support for image queues, fonts, and heightmaps
3. **loader** contains functionality to read/write from PNG, TIFF, and BMP, and optional file formats
4. **tests** contains executable tests for various algorithms
5. **tools** contains command line tools for producing 2D raster effects
6. **support** contains helper functions, types, and definitions
7. **util** contains util files for Windows and Unix specific functionality
8. **assets** contains fonts, images, and other resources
9. **scripts** contains build scripts used in the project