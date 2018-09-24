Assumes that the Front Holo is number #0 and has an OLED display and 12 Jewel NeoPixel ring. All other Holos are assumed to have 7 and no OLED.

The OLED support is only enabled when compiled for the Mega. It is possible to run this on an Pro Mini but you will need to create your own custom versions of all the libraries used to strip out any unused functions. The AVR toolchain doesn't strip out unused C++ member functions so if you instantiate a library object your final binary will include all functions in that library. Possible. Not recommended.

OLED Commands:

"S1|40": Play Leia movie plus display Leia light sequence for 40 seconds
"S2": Play R2 cartoon
"S3": Play Deathstar plans movie

These commands have been added because extracting the SD card can be difficult once the Holo has been installed. The [filename] is 7-characters maximum and should have no extension. The extensions BD2 will be added automatically. The filename is not case-sensitive since the SD card is a Fat32 file system.

"OD[filename]": Download movie via serial to SD card
"OP[filename]": Play movie with the specified name
"OX[filename]": Delete movie of the specifid name if it exists

The serial download is slow, but it's faster than ripping the dome apart to get to the holo.
