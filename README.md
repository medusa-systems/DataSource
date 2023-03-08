# DataSource 
This is a C++ implementation of circular buffers with some non-standard features.
It supports multiple readers of a buffer, reading and writing blocks of data,
and moving back and forth during reading.

## At a Glance 
It is a class template, DataSource\<T\>, where T is the type of data items being
passed from one object to another. An object that needs to participate in a data
pipeline should inherit from a specialized class (e.g. DataSource\<float\>)
and so become a "data source". Then it has the exclusive write access to the
buffer, whereas any other object may register as a reader. Writing and (multiple)
reading sessions are supposed to come in turn one after another, which is the
responsibility of the calling functions.

## The Manual
All the details about gaining access, querying for free and occupied
space, reading, writing, and possible exceptions are explained in the file
DataSourceUsage.txt.

## How to Incorporate it
Practically, using this solution entails adding three files to one's application
-- the template DataSource.hpp and two auxiliary files, DataSourceException.cpp
and DataSourceException.hpp -- and furthermore adding a couple of directives
to the Makefile (or any its prototype, such as CMakeLists.txt) -- one for
DataSourceException class and one for each specialized DataSource class, which
is also explained in the manual.

## Requirements
Any GCC version supporting the C++20 standard will do. 

## Examples
For the time being, there is only one packaged example, a skeletal program
that reads an uncompressed sound file (such as .wav, .au, or .aiff) and stores
its contents in the most simple-minded manner in the main memory, using in the
process one DataSource circular buffer for each channel. Despite its simplicity,
however, it does employ most actions of the single-reader scenario. The program
depends on libsndfile.

Much more illustrative examples are to come.


