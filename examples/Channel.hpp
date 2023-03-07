
   /* Our improvised sound storage; basically just a linear buffer. */

   class Channel : public DataSource<SAMPLE>
   {

      private:

      SAMPLE * storage = nullptr;	// The buffer itself

      SAMPLE * current = nullptr;	// Pointer to a "current" sample in the buffer

      uint8 n_samples = 0;		// The number of samples in the buffer

      DataSource<SAMPLE> * source = nullptr; // Where we get the sound from

      uint1 source_token;		// The token used for accessing the sound source

      bool writing_in_progress = false;	

      string channel_name = string("(-unnamed channel-)");

      public:

      /* Provide room in the buffer for the specified number of samples. */
      void inflate(uint8 len) 
      { 
         storage = reinterpret_cast<SAMPLE*>(malloc(len * sizeof(SAMPLE))); 
         current = storage;
      }

      /* Start working with a data source; see the documentation for DataSource. */
      inline void attach(DataSource<SAMPLE> & s) 
      { 
         source = &s; 
	 source_token = source->registerDataSource(); 
      }

      /* Finish working with the data source; see the documentation for DataSource. */
      inline void detach() 
      { 
         if ( source ) source->unregisterDataSource(1); 
	 source = nullptr;  
      }

      /* Aquire sound from the sound source previously set by attach(). */
      uint4 write();

      inline void setWritingStarted() { writing_in_progress = true; }

      inline void setWritingFinished() { writing_in_progress = false; }

      /* Lets others know whether the channel is currently being filled. */
      bool writingFinished() { return !writing_in_progress; }

      uint8 length() const { return n_samples; }

      string name() const { return channel_name; }

      void setName(string s) { channel_name = s; }


      /* The default constructor */
      Channel(uint8 len = 0);

      /* The move constructor */
      Channel(Channel && oCh) noexcept;

      /* The move-assignment operator */
      Channel & operator=(Channel && oCh);

      /* The destructor */
      ~Channel();

   };



