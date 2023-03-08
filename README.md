# DataSource 
This is a C++ implementation of circular buffers with some non-standard features.
It supports multiple readers of a buffer, reading and writing blocks of data,
and moving back and forth during reading.

## At a Glance 
There is a class template, DataSource\<T\>, where T is the type of data items
being passed from one object to another. An object that needs to participate in a
data pipeline should inherit from a specialized class (e.g. DataSource\<float\>)
and so become a "data source". Then it gets the exclusive write access to the
buffer, whereas any other object may register as a reader. The buffer is supposed
to be operated in a loop, where in each cycle a writing session is followed by
reading sessions, one for each reader. This scheduling is the responsibility of
the calling functions.

## The Manual
Full instructions on gaining access, querying for free and occupied space,
reading, writing, releasing space, and catching the exceptions are given in
the file **DataSourceUsage.txt**.

## How to Incorporate it
Practically, using this solution entails adding three files to one's application --
the template DataSource.hpp and two auxiliary files, DataSourceException.cpp and
DataSourceException.hpp; putting an include directive for the template header in
each file where DataSource methods are used; and furthermore adding a couple of
directives to the Makefile (or any prototype of it, such as CMakeLists.txt) -- one
for DataSourceException class and one for each specialized DataSource class. This
is also explained in the manual, and can be seen in the included example.

## Requirements
Any GCC version supporting C++20 standard will do. 

## Examples 
For the time being, there is only one packaged example -- dshello -- a skeletal
program that reads an uncompressed sound file (such as .wav, .au, or .aiff) and
stores its contents in a simple-minded manner in the main memory, using in the
process one DataSource buffer for each sound channel. Despite its simplicity,
however, it does exemplify most actions in the single-reader scenario. The
program additionally depends on libsndfile.

Much more illustrative examples are to come.

## License
It is LGPL version 2.1.

