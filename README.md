# data-source
A C++ implementation of circular buffers, allowing for multiple readers, reading
and writing blocks of data, and moving back and forth during reading.

It is a class template, DataSource\<T\>, where T is the type of data items being
passed from one object to another. An object that needs to participate in a data
pipeline should inherit from a specialized class, (e.g. DataSource\<float\>),
and so become a "data source". Then it has the exclusive write access to the buffer,
whereas any other object may register as a reader. Writing and (multiple) reading
sessions are supposed to come in turn one after another, which is the responsibility
of the calling functions. 

All the details about gaining access, querying free and occupied space, reading 
and writing are explained in the file DataSourceUsage.txt.
