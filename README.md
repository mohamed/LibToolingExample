# Clang LibTooling Example

To build this repository under Ubuntu 18.04, you need to do the following:

	$ sudo apt-get install clang-7 libclang-7-dev clang-tools-7
	$ git clone https://github.com/mohamed/LibToolingExample.git

Then build and run the Example.cpp file:

	$ mkdir build
  $ cd build
  $ cmake ..
  $ cmake --build .
  $ cmake --buiild . --target test

Or alternatively, you can run the binary as follows:

  $ cd build
  $ ./LibToolingExample ../test.c --
