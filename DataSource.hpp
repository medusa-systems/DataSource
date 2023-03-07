#include <cinttypes>
#include <cassert>
#include <string>
#include <vector>
#include <bitset>
#include <semaphore>
#include <exception>
#include <DataSourceException.hpp>

using std::byte;
using std::string;
using std::vector;
using std::bitset;
using std::binary_semaphore;
using std::exception;



   template<typename ITEM> class DataSource 
   {

      #define MAX_SRC_READERS	8

      private:					
 
      ITEM * ds_buffer = nullptr;	///< The buffer for storing the data items.

      uint32_t ds_size; 	/**< The size of the data buffer, in items; 
      					it is always a power of two.*/

      uint32_t ds_mask; 	/**< The buffer size, in items, minus one; e.g. 
      			     		the size = %00100000
			     		the mask = %00011111 */
                                        
      uint32_t ds_free; 	/**< The number of free places for data items in the buffer.*/

      uint32_t ds_head = 0; 	/**< The index of the position immediately after the used area.*/

      uint32_t ds_tail = 0; 	/**< The index of the first position in the used area.*/

      vector<uint32_t> ds_reader_position = { };	
      				/**< The "current" index for each reader, i.e.  
				 the index of the item from which it will
				 continue reading. It is always inside the used area.*/

      vector<uint32_t>::iterator ds_current;	
      				/**< Points to the "current" index of the current reader. */

      bitset<MAX_SRC_READERS> ds_readers_done;	
      				/**< The bit map of the readers that have read since 
				 the last releasing operation. */

      bitset<MAX_SRC_READERS> ds_readers_mask; 
      				///< For easier resetting of ds_readers_done.

      uint32_t ds_release = 0xFFFFFFFF;	
      				/**< The number of data items that all the readers
				 have read and won't read any more. */

      bool ds_more = true; 	/**< Whether this object will produce more data, 
      				 beside what's currently in the buffer. */

      binary_semaphore ds_access{1};

#ifdef VERBOSE_DATA_SOURCE
      string ds_name;	

      char ds_state[512];
#endif 


      //**************************************************************************************

      /** The size of the continuous used area starting from the "ds_current" marker,
      that is, the area terminated either by the "ds_head" marker or by the end of
      the buffer -- whichever is encountered first. */
      inline uint32_t continuousUsed()
      {
         if ( ds_free == ds_size ) return 0;
      
         if ( ds_head > ds_tail ) return ds_head - *ds_current;
         else return ((*ds_current < ds_tail) ? ds_head - *ds_current : ds_size - *ds_current); 
      }


      //**************************************************************************************

      /** The size of the continuous free area starting from the "ds_head" marker,
      that is, the area terminated either by the end of the buffer or by the
      "ds_tail" marker -- whichever is encountered first. */
      inline uint32_t continuousFree()
      {
         if ( ds_free == 0 ) return 0;
      
         return (ds_tail <= ds_head) ? ds_size - ds_head : ds_tail - ds_head;
      }


      //**************************************************************************************

      /** The number of data items in the buffer ahead of the "ds_current" marker. It
      is supposed to be used by the single reader of this object before it starts
      reading a fresh amount of data from the buffer. */
      inline uint32_t ahead()
      {
         uint32_t x = (ds_head - *ds_current) & ds_mask;
      
         if ( x == 0 ) x = (ds_free == 0 ? ds_size : 0);
      
         return x;
      }


      protected:


      //**************************************************************************************

      /** The method called at the beginning of writing in order to prevent
      collisions. */
      inline void closeDataSource() { ds_access.acquire(); }


      //**************************************************************************************

      /** The total free area of the buffer, in data items. */
      inline uint32_t dataSourceFree() { return ds_free; };


      //**************************************************************************************

      /** Add a single data item to the buffer. */
      void putData(ITEM item)
      {
         assert(ds_free > 0);
      
         *(ds_buffer + ds_head) = item;
         ds_head = (ds_head + 1) & ds_mask;
         ds_free--; 
      }


      //**************************************************************************************

      /** Add a sequence of data items from another buffer to this buffer. */
      void putData(ITEM * src, uint32_t length)
      {
         assert(ds_free >= length);
      
         uint32_t continuousAvailable = continuousFree();
      
         if ( length <= continuousAvailable ) {
            memcpy(ds_buffer + ds_head, src, length * sizeof(ITEM));
            
            ds_head += length; 
            if ( ds_head == ds_size ) ds_head = 0; 
      
         } else {
            uint32_t remainder = length - continuousAvailable;
            memcpy(ds_buffer + ds_head, src, continuousAvailable * sizeof(ITEM));
            memcpy(ds_buffer, src + continuousAvailable, remainder * sizeof(ITEM));
            ds_head = remainder; 
         }
      
         ds_free -= length; 
      }


      //**************************************************************************************

      /** Add a sequence of data items from a file to the buffer. */
      void putData(FILE * src, uint32_t length)
      {
         assert(ds_free >= length);
      
         uint32_t continuousAvailable = continuousFree();
      
         if ( length <= continuousAvailable ) {
            fread(ds_buffer + ds_head, sizeof(ITEM), length, src);
            
            ds_head += length; 
            if ( ds_head == ds_size ) ds_head = 0; 
      
         } else {
            uint32_t remainder = length - continuousAvailable;
            fread(ds_buffer + ds_head, sizeof(ITEM), continuousAvailable, src);
            fread(ds_buffer, sizeof(ITEM), remainder, src);
            ds_head = remainder; 
         }
      
         ds_free -= length; 
      }


      //**************************************************************************************

      /** Add a certain number of "zero items" (i.e. fields filled with zeroes,
      each the size of a data item) to the buffer. */
      void putNullData(uint32_t length)
      {
         assert(ds_free >= length);
      
         uint32_t continuousAvailable = continuousFree();
      
         if ( length <= continuousAvailable ) {
            memset(ds_buffer + ds_head, 0, continuousAvailable * sizeof(ITEM));
            
            ds_head += length; 
            if ( ds_head == ds_size ) ds_head = 0; 
      
         } else {
            uint32_t remainder = length - continuousAvailable;
            memset(ds_buffer + ds_head, 0, continuousAvailable * sizeof(ITEM));
            memset(ds_buffer, 0, remainder * sizeof(ITEM));
            ds_head = remainder; 
         }
      
         ds_free -= length; 
      }


      //**************************************************************************************

      /** This method should be called when it is known that this object will
      receive no new data. */
      inline void setDataSourceFinished() { ds_more = false; }


      //**************************************************************************************

      /** The method called at the end of writing in order allow reading. */
      inline void openDataSource() { ds_access.release(); }

 
      public:


      //**************************************************************************************

      /** Called by an aspiring reader before it starts reading from this object.
      Returns a number which the reader should use in subsequent calls to startDataSource()
      method. */
      uint32_t registerDataSource()
      { 
         uint8_t token = ds_reader_position.size();

         if ( token == MAX_SRC_READERS ) 
	    throw DataSourceException(DSEXC_B_TOO_MANY_READERS, 
	    		DSEXC_M_REGISTER, DSEXC_A_NEW_READER);

         try {
      
            ds_reader_position.push_back(0); 	
	    ds_readers_done.set(token, false);
	    ds_readers_mask <<= 1;
	    ds_current = std::prev(ds_reader_position.end()); 
	    					// so that *ds_current is something valid.
            return token;
      
         } catch (exception & e) { 
            throw DataSourceException(DSEXC_B_PUSH_BACK, 
	    		DSEXC_M_REGISTER, DSEXC_A_DS_CURRENT, e.what());
         }
      }


      //**************************************************************************************

      /** A reader of this object calls this method in order to be allowed to read
      from the buffer. The reader passes its token (received upon registration);
      the method returns the number of data items in the buffer ahead of the
      "ds_current" marker. */
      uint32_t startDataSource(uint8_t n)
      {
         assert(ds_reader_position.size());
         assert(n <= ds_reader_position.size());
      
         ds_access.acquire();
	 ds_readers_done.set(n);
         ds_current = std::next(ds_reader_position.begin(), n);

         return ahead();
      }


      //**************************************************************************************

      /** Tells whether there are more data to read from this object. */
      inline bool dataSourceFinished() { return !ds_more; }
 

      //**************************************************************************************

      /** Provides the data item at the arbitrary position in the buffer, specified
      by its distance (positive or negative) from the "current" position. */
      ITEM dataItemAt(int32_t n)
      {  
         assert((n >= 0 && n <= ahead()) || (n < 0 && -n >= ds_size - ds_free - ahead()));
      
         return *(ds_buffer + ((*ds_current + n) & ds_mask));
      }


      //**************************************************************************************

      /** Provides the data item at the "current" position and advances the
      "ds_current" marker by one. */
      ITEM getData()
      { 
         assert(ds_free < ds_size); 
      
         ITEM currentItem = *(ds_buffer + *ds_current); 
      
         *ds_current = (*ds_current + 1) & ds_mask;
      
         return currentItem;
      }


      //**************************************************************************************

      /** Provides a sequence of data items starting from the "ds_current" marker.
      It puts the sequence to the memory location "dest", and the number of
      items is specified by the second argument, "length". The "ds_current" marker
      is advanced by the number of copied items. */
      void getData(void * dest, uint32_t length)
      {
         assert(length <= ds_size - ds_free);
      
         uint32_t continuous = continuousUsed();
         if ( length <= continuous ) {
            memcpy(dest, ds_buffer + *ds_current, length * sizeof(ITEM));
            *ds_current += length; 
            if ( *ds_current == ds_size ) *ds_current = 0;
         } else {
            uint32_t remainder = length - continuous;
            memcpy(dest, ds_buffer + *ds_current, continuous * sizeof(ITEM));
            memcpy(reinterpret_cast<byte*>(dest) + continuous * sizeof(ITEM), 
	    					ds_buffer, remainder * sizeof(ITEM));
            *ds_current = remainder;
         }
      }


      //**************************************************************************************

      /** Provides the sequence of all data items in the buffer, starting from the
      "current" marker.  It puts the sequence to the memory location "dest". The
      "current" marker is advanced accordingly, so that in the end "ds_current" =
      "ds_head". */
      inline void getData(void * dest) { getData(dest, ahead()); }


      //**************************************************************************************

      /** Provides a sequence of data items starting from the "current" marker.
      It puts the sequence to the file pointed to by "dest", and the number of
      items is specified by the second argument, "length". The "current" marker
      is advanced by the number of copied items. */
      void getData(FILE * dest, uint32_t length)
      {
         assert(length <= ds_size - ds_free);
      
         uint32_t continuous = continuousUsed();
         if ( length <= continuous ) {
            uint64_t check = fwrite(ds_buffer + *ds_current, sizeof(ITEM), length, dest);

            if ( check != length ) 
               throw DataSourceException(DSEXC_B_FWRITE, DSEXC_M_GET, DSEXC_A_COPY_DATA);

            *ds_current += length; 
            if ( *ds_current == ds_size ) *ds_current = 0;
         } else {
            uint32_t remainder = length - continuous;
            uint64_t check = fwrite(ds_buffer + *ds_current, sizeof(ITEM), continuous, dest);

            if ( check != continuous ) 
               throw DataSourceException(DSEXC_B_FWRITE, DSEXC_M_GET, DSEXC_A_COPY_DATA);

            check = fwrite(ds_buffer, sizeof(ITEM), remainder, dest);

            if ( check != remainder ) 
               throw DataSourceException(DSEXC_B_FWRITE, DSEXC_M_GET, DSEXC_A_COPY_DATA);

            *ds_current = remainder;
         }
      }


      //**************************************************************************************

      /** Provides the sequence of all data items in the buffer, starting from
      the "ds_current" marker.  It puts the sequence to the file pointed to by
      "dest". The "current" marker is advanced accordingly, so that in the end
      "ds_current" = "ds_head". */
      inline void getData(FILE * dest) { getData(dest, ahead()); }


      //**************************************************************************************

      /** Shifts the "current" marker by the specified number of positions;
      a positive value moves the marker forward, a negative one -- backwards. */
      void dataSourceShift(int32_t amount)
      { 
         assert((amount >= 0 && amount <= ahead()) || 
	 		(amount < 0 && -amount >= ds_size - ds_free - ahead()));
      
         *ds_current = ((*ds_current + amount) & ds_mask);
      }


      //**************************************************************************************

      /** A reader of this object calls this method in order to announce that it
      has finished reading a portion of data from the buffer. The reader passes
      the number of data items it doesn't need any more, starting from the "ds_tail"
      marker. */
      void stopDataSource(uint32_t amount)
      { 
         assert(amount <= ((*ds_current == ds_tail && ds_free == 0) ? 
	 				ds_size : (*ds_current - ds_tail) & ds_mask));
      
         if ( amount < ds_release ) ds_release = amount;
      
         if ( ds_readers_done.all() ) {
      
            assert(ds_release <= ((*ds_current == ds_tail && ds_free == 0) ? 
	    				ds_size : (*ds_current - ds_tail) & ds_mask));
            
            ds_free += ds_release; 
            
            if ( ds_free == ds_size ) { 
               *ds_current = ds_head = ds_tail = 0;
            } else {
               ds_tail += ds_release;
               ds_tail &= ds_mask;
            }
            
            if ( ds_free == ds_size ) 
	    { 
	       for (auto p = ds_reader_position.begin(); p < ds_reader_position.end(); p++) 
	          *p = 0; 
	    }
      
	    ds_readers_done = ds_readers_mask;
            ds_release = 0xFFFFFFFF;
      
         }

         ds_access.release();
      }


      //**************************************************************************************

      /** A reader of this object calls this method in order to announce that it
      has finished reading a portion of data from the buffer and that all the
      remaining data may be discarded. The reader passes its token (received
      upon registration). */
      inline void stopDataSource() { stopDataSource(ds_size - ds_free); }

      /** Removes the latest n memebers from the list of registered readers.
      If n = 0, all the readers will be removed. */
      void unregisterDataSource(uint8_t n = 0)
      { 
         assert(n <= ds_reader_position.size());

         if ( n == 0 ) { 
	    ds_reader_position.clear(); 
            ds_current = ds_reader_position.end();
	    ds_readers_mask.set();
	    ds_readers_done.set(); 
	 } else {
	    ds_reader_position.erase(
	       std::next(
	          ds_reader_position.begin(), 
		  ds_reader_position.size() - n), 
	       ds_reader_position.end()
	    );
            ds_current = ds_reader_position.begin(); // So that *ds_current is something valid.
	    ds_readers_mask = (ds_readers_mask >> n) | ds_readers_mask;
	    ds_readers_done = ds_readers_mask;
	 }
         
      }


#ifdef VERBOSE_DATA_SOURCE
      //**************************************************************************************

      void setDataSourceName(string x) { ds_name = x; }


      //**************************************************************************************

      const char * dataSourceState() 
      { 
         ds_current < ds_reader_position.end() ? 
            sprintf(ds_state, "SRC %s: tail = %d; current = %d; "
	                      "head = %d; ahead = %d; free = %d", 
                   ds_name.data(), ds_tail, *ds_current, ds_head, ahead(), ds_free) :
            sprintf(ds_state, "SRC %s: tail = %d; current = ??; "
	                      "head = %d; ahead = ??; free = %d", 
                   ds_name.data(), ds_tail, ds_head, ds_free);

         return ds_state; 
      }
#endif 
 
      
      //**************************************************************************************

      /** The default constructor. The value passed is the binary logarithm of
      the buffer size, i.e. the buffer will be of size 2^z. */
      DataSource(uint8_t z = 16)
      { 
         ds_size = 1 << z;
         ds_buffer = reinterpret_cast<ITEM*>(malloc(ds_size * sizeof(ITEM)));
         if ( ds_buffer == nullptr ) 
            throw DataSourceException(DSEXC_B_MALLOC, DSEXC_M_CONSTR, DSEXC_A_ALLOCATE); 
         ds_free = ds_size;
         ds_mask = ds_size - 1;
	 ds_current = ds_reader_position.end();
	 ds_readers_done.set();
	 ds_readers_mask.set();
      }
 

      //**************************************************************************************

      /** The copy constructor. */
      DataSource(const DataSource & oSrc) : ds_access(1)
      { 
         // Make a copy of the other source's buffer.
         ds_size = oSrc.ds_size; 	
         ds_mask = oSrc.ds_mask; 		
         ds_buffer = reinterpret_cast<ITEM*>(malloc(ds_size * sizeof(ITEM)));
         if ( ds_buffer == nullptr ) 
            throw DataSourceException(DSEXC_B_MALLOC, DSEXC_M_CONSTR_COPY, DSEXC_A_ALLOCATE);
         if ( oSrc.ds_buffer != nullptr ) {
            memcpy(ds_buffer, oSrc.ds_buffer, ds_size * sizeof(ITEM));
	 }
         ds_free = oSrc.ds_free; 
      
         // Replicate the other source's state.
         ds_head = oSrc.ds_head; 		
         ds_tail = oSrc.ds_tail; 		
         uint8_t d = std::distance<vector<uint32_t>::const_iterator>
	 				(oSrc.ds_reader_position.begin(), oSrc.ds_current);
         ds_reader_position = oSrc.ds_reader_position; 	
         ds_current = std::next(ds_reader_position.begin(), d); 

         ds_readers_done = oSrc.ds_readers_done;           
         ds_readers_mask = oSrc.ds_readers_mask;           
         ds_release = oSrc.ds_release;	
         ds_more = oSrc.ds_more; 		

#ifdef VERBOSE_DATA_SOURCE
         ds_name = oSrc.ds_name;	
#endif
      }
 

      //**************************************************************************************

      /** The move constructor. */
      DataSource(DataSource && oSrc) noexcept 
      : ds_access(1), ds_reader_position(move(oSrc.ds_reader_position))
      { 
         // Take the other source's buffer.
         ds_buffer = oSrc.ds_buffer;
         oSrc.ds_buffer = nullptr;
         ds_size = oSrc.ds_size; 	
         ds_mask = oSrc.ds_mask; 		
      
         // Replicate the other source's state.
         ds_free = oSrc.ds_free; 
         ds_head = oSrc.ds_head; 		
         ds_tail = oSrc.ds_tail; 		
         ds_current = oSrc.ds_current; 

         ds_readers_done = oSrc.ds_readers_done;           
         ds_readers_mask = oSrc.ds_readers_mask;           
         ds_release = oSrc.ds_release;	
         ds_more = oSrc.ds_more; 		

#ifdef VERBOSE_DATA_SOURCE
         ds_name = move(oSrc.ds_name);	
#endif
      }
 

      //**************************************************************************************

      /** The assignment constructor. */
      DataSource & operator=(DataSource && oSrc) 
      { 
         // Take the other source's buffer.
         ds_buffer = oSrc.ds_buffer;
         oSrc.ds_buffer = nullptr;
         ds_size = oSrc.ds_size; 	
         ds_mask = oSrc.ds_mask; 		
      
         // Replicate the other source's state.
         ds_free = oSrc.ds_free; 
         ds_head = oSrc.ds_head; 		
         ds_tail = oSrc.ds_tail; 		
         ds_current = oSrc.ds_current; 
         ds_reader_position = move(oSrc.ds_reader_position); 	

         ds_readers_done = oSrc.ds_readers_done;           
         ds_readers_mask = oSrc.ds_readers_mask;           
         ds_release = oSrc.ds_release;	
         ds_more = oSrc.ds_more; 		

#ifdef VERBOSE_DATA_SOURCE
         ds_name = move(oSrc.ds_name);	
#endif
	 return *this;
      }
 

      //**************************************************************************************

      /** The default destructor. */
      virtual ~DataSource()
      { 
         free(ds_buffer); 
      }

   };



