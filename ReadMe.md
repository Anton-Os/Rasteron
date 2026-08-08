# Rasteron

An image loader, manipulation, and synthesis project written in C, with CUDA amd video-encoding support coming soon! Windows and Linux are the target platforms, with bindings being cross-compiled for Flutter and mobile devices.

###  Enviornment

The external dependencies being built are mainly included to deal with various codecs.

1. zlib, libpng, libtiff and libjpeg for working with common file formats

###  Project Structure:

1. **core** contains core functionality to work with image types
2. **ext** contains extensible type support for image queues, fonts, and heightmaps
3. **loader** contains functionality to read/write from PNG, TIFF, and BMP, and optional file formats
4. **tools** contains executable tests and visualizers for various algorithms
5. **support** contains helper functions, types, and definitions
6. **util** contains util files for Windows and Unix specific functionality
7. **assets** contains fonts, images, and other resources
8. **scripts** contains build scripts used in the project