Assumes that the Front Holo is number #0 and has an OLED display and 12 Jewel NeoPixel ring. All other Holos are assumed to have 7 and no OLED.

The OLED support is only enabled when compiled for the Mega. It is possible to run this on an Pro Mini but you will need to create your own custom versions of all the libraries used to strip out any unused functions. The AVR toolchain doesn't strip out unused C++ member functions so if you instantiate a library object your final binary will include all functions in that library. Possible. Not recommended.
