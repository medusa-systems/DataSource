#include <DataSourceException.hpp>


char DataSourceException::dse_string[DS_EXC_STRINGS][DS_EXC_STR_SIZE] = 
{
   "Too many readers for a data source",      
   "LibC++ error (vector<uint4>::push_back)", 
   "LibC error (fwrite)",                     
   "LibC error (malloc)",                     

   "DataSource::registerDataSource", 
   "DataSource::get",                
   "DataSource::DataSource",         
   "DataSource::DataSource (copy)",

   "add a new reader",
   "create ds_current index for a sound source",
   "copy the data to the file block",
   "allocate space for the circular buffer"
};

thread_local char * DataSourceException::dse_brief;
thread_local char * DataSourceException::dse_method;
thread_local char * DataSourceException::dse_action;
thread_local char DataSourceException::orig_msg[ORIG_MSG_SIZE];

