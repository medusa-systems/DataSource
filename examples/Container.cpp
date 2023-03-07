
#include <dshello.hpp>

#include <sndfile.h>
#include <DataSource.hpp>
#include <SoundSource.hpp>
#include <Container.hpp>


//**********************************************************************************************

void 
Container::getSound()
{
   if ( end_of_stream ) 
   { 
      DBG_MSG(Container::getSound, "\t\tThe stream ended"); 
   }
   else
   { 
      // PIPELINE00 NODE0 Checking the free space in the buffer...
      uint4 minimumFree = freeSpaceInBuffers();

      DBG_MSG(Container::getSound, "\t\tMinimal free amount in a channel buffer: %d", minimumFree);
      if ( minimumFree ) 
      {
	 decoded_size = sf_readf_float(snd_file, buffer, minimumFree);
         DBG_MSG(Container::getSound, "\t\tDecoded frames: %d", decoded_size);
         // PIPELINE00 NODE0 Deciding whether to mark the buffer as finished...
         if ( decoded_size != minimumFree ) finishAllBuffers();
         
         untangle();
      }
   }
}


//**********************************************************************************************

void 
Container::setBuffers(uint1 n) 
{ 
   DBG_MSG(Container::setBuffers, "\tCreating %d DataSource<SAMPLE> objects...", n);
   n_channels = n; 
   channel_buffer.reserve(n);
   while ( n-- ) channel_buffer.emplace_back(DEFAULT_READER_CBPOW);
   DBG_MSG(Container::setBuffers, "\tDataSource<SAMPLE> objects created.", n);
}


//**********************************************************************************************

uint4 
Container::freeSpaceInBuffers() 
{ 
   for (uint1 ch = 0; ch < n_channels; ch++) channel_buffer[ch].closeDataSource(); 

   uint4 minimumFree = CONTAINER_FRAMES;
   for (uint1 ch = 0; ch < n_channels; ch++) { 
      uint4 blank = channel_buffer[ch].dataSourceFree();
      if ( blank < minimumFree ) minimumFree = blank; 
   }
   return minimumFree;

}

//**********************************************************************************************

void 
Container::untangle() 
{ 
   DBG_MSG(Container::untangle, "\t\tFilling the channel buffers...");

   // PIPELINE00 NODE0 Filling the buffer (putting individual samples)...
   for (auto n = 0; n < decoded_size * n_channels; ) 
   { 
      for (uint1 ch = 0; ch < n_channels; ch++, n++) channel_buffer[ch].putData(buffer[n]); 
      if ( (n / n_channels) % (1 << 14) == 0 ) 
      {
         DBG_MSG(Container::untangle, "\t\tShowing the state of the channel buffers "
	 					"after %d frames:", n / n_channels);
         for (uint1 ch = 0; ch < n_channels; ch++) 
            DBG_MSG(Container::untangle, "\t\t%s", channel_buffer[ch].dataSourceState());
      }
   }

   DBG_MSG(Container::untangle, "\t\tThe buffers filled; opening them for reading...");
   for (uint1 ch = 0; ch < n_channels; ch++) { channel_buffer[ch].openDataSource(); }

}


//**********************************************************************************************

void 
Container::finishAllBuffers() 
{ 
   end_of_stream = true;
   for (uint1 ch = 0; ch < n_channels; ch++) channel_buffer[ch].setDataSourceFinished(); 
}


//**********************************************************************************************

Container::Container(FILE * f) : input_file(f) 
{
   DBG_MSG(Container::Container, "\tOpening a file.");

   // Use sndfile to read the container.
   SF_INFO SFInfo;
   SF_FORMAT_INFO formatInfo;

   SFInfo.format = 0;
   snd_file = sf_open_fd(fileno(input_file), SFM_READ, &SFInfo, false);

   if ( snd_file == nullptr )
   {
      throw std::runtime_error(std::string("Libsndfile could not open the file."));
   }

   formatInfo.format = SFInfo.format;
   sf_command(snd_file, SFC_GET_FORMAT_INFO, &formatInfo, sizeof(SF_FORMAT_INFO));
   DBG_MSG(Container::Container, "\tFormat as detected by sndfile: %s ($%08X)", 
   						formatInfo.name, SFInfo.format);
   DBG_MSG(Container::Container, "\tchannels: %d; length: %ld; sample rate: %d",
   		SFInfo.channels, SFInfo.frames, SFInfo.samplerate, SFInfo.samplerate);

   setBuffers(SFInfo.channels);
   sample_rate = SFInfo.samplerate;
   frames = SFInfo.frames;

#ifdef VERBOSE_DATA_SOURCE
   // DEBUG - giving names to the readers.
   char chan_ind[4]; 
   for (uint1 n = 0; n < n_channels; n++) { 
      sprintf(chan_ind, "%d", n); 
      channel_buffer[n].setDataSourceName(std::string("contbuf") + chan_ind); 
   }
#endif

   buffer = reinterpret_cast<SAMPLE*>(malloc(CONTAINER_FRAMES * n_channels * sizeof(SAMPLE)));
   DBG_MSG(Container::Container, "\tCreated a receiving linear buffer for %d frames", 
   							CONTAINER_FRAMES * n_channels);

}

//**********************************************************************************************

Container::~Container()
{ 
   DBG_MSG(Container::~Container, ""); 
   sf_close(snd_file);
   if ( buffer ) free(buffer);
}

