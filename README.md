# fitsToKdb
An interface for accessing tables in \*.fits files from a q process. This project requires two libraries not in this repository. For examples and more about this interface please read the pdf located here: [wplink]

## cfitsio
The source for the cfitsio library developed by NASA can be downloaded from http://heasarc.gsfc.nasa.gov/fitsio/. Installation instructions can be found here:http://heasarc.gsfc.nasa.gov/docs/software/fitsio/quick/node3.html.

If building the 32 bit version the following may be required during installation to set up the 32 bit flags:
./configure CFLAGS="-m32 -O2 -g"

-Building a cfitsio c program from a source file called example.c:

*gcc -o example.o example.c -L\<location of folder containing cfitsio lib folder\> -lcfitsio -lm -lnsl -I\<location of folder containing cfitsio include folder\>*

## k.h
The k.h file is linked to here http://code.kx.com/wiki/Cookbook/ExtendingWithC#Portable_Example. For more on the use of k.h to extend functionality of Q please refer to the following cookbooks:
http://code.kx.com/wiki/Cookbook/ExtendingWithC
http://code.kx.com/wiki/Cookbook/InterfacingWithC

-Building a 32 bit .so for extending q from source file example.c (May require installation of gcc-multilib):

*gcc -m32 -shared -fPIC example.c -o example.so*

********************
Putting the two together allows compilation of fitsio.c into a .so file for loading functions into q via 2: (see file fitsToKdb.q for example use of 2:).

-Building a 32 bit .so for extending q using fitsio:

*gcc -m32 -shared -fPIC example.c -o example.so -L\<location of folder containing cfitsio lib folder\> -lcfitsio -lm -lnsl -I\<location of folder containing cfitsio include folder\>*
