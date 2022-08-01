# Rasteron

A raster image loader, manipulation, and generation project. Supports PNG, TIFF, and BMP file formats. Windows is supported with a Linux port in progress.

zlib, libpng, and libtiff are built by the project and freetype is built optionally to enable font rasterisation.

Project Structure:

1. **core** contians format support, basic datatypes, and utility functions. Datatypes are defined in Image.h for raster data and Dimensional.h for sprites, heightmaps, and extensible types. File formats are supported in Loader.h providing support for the aforementioned format types. Utility functions are defined inside Toolbox.h for manipulating image data.
2. **plugin** contains optional built-in modules. Containers with multiple images are stored within Animation.h while containers with text objects are stored within Font.h that enables font baking. Neighboring cell logic and distance field equations are defined in Cellwise.h while random-value logic and noise equations are defined in Noise.h
3. **helper** contains helper modules for working with OS-specific functionality, defined in OS_Util.h