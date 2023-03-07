
   /* An object representing a sound file. */

   class Container 
   {


      // The member variables
      // ------------------------------------------------------------------------
      
      protected:

      FILE * input_file = nullptr;		// The input file

      SNDFILE * snd_file;			// The libsnd's representation of the input file

      SAMPLE * buffer;				// The buffer containg still interleaved decoded samples

      uint1 n_channels = 1;			// The number of channels in the file

      uint8 frames = 0;				// The total number of frames in the file

      uint4 sample_rate;			// The sample rate of the file

      uint4 decoded_size = 0;			// How many samples have been produced by decoding in one run

      bool end_of_stream = false;		// Whether we have read all the sound from the file

      vector<SoundSource> channel_buffer = { }; // Output circular buffers, one for each channel


      // Auxiliary methods
      // ------------------------------------------------------------------------

      /* Prepare the buffers, one for each channel. */
      void setBuffers(uint1 n);

      /* Fill the channel buffers with the sound from the file. */
      void untangle();

      /* Tell everyone that we have reached the end of a reading or writing operation. */
      void finishAllBuffers();


      // Public methods
      // ------------------------------------------------------------------------

      public:

      uint4 length() { return frames; }

      uint4 sampleRate() { return sample_rate; }

      uint1 channels() { return n_channels; }

      uint4 amount() { return decoded_size; }

      bool done() { return end_of_stream; }

      uint4 freeSpaceInBuffers();

      void getSound();

      /* Export the n-th circular buffer, so that others can read from it. */
      SoundSource & soundForChannel(uint1 n) { return channel_buffer[n]; }


      /** The constructor. */
      Container(FILE * f);

      /** The destructor. */
      ~Container();

   };

