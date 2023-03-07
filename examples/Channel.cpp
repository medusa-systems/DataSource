#include <dshello.hpp>
#include <DataSource.hpp>
#include <Channel.hpp>

extern template class DataSource<SAMPLE>;


//**********************************************************************************************

uint4
Channel::write()
{

   // PIPELINE00 NODE1: Finding out the amount of new data...
   uint4 samplesRemaining = source->startDataSource(0); 
   
   DBG_MSG(Channel::write, "\t\tFrom a circular buffer with %d samples.", samplesRemaining);
   
   if ( samplesRemaining == 0 ) {
   
      // PIPELINE00 NODE1: No new data -- deciding whether the pipeline is finished...
      DBG_MSG(Channel::write, "\t\tNo new data; now checking if the source buffer has finished...");
      DBG_MSG(Channel::write, "\t\t%s", (source->dataSourceFinished() ? 
      						"Yes, it has." : "No, it hasn't."));
      if ( source->dataSourceFinished() ) setWritingFinished();
      return 0;
   }
   
   try { 
   
      DBG_MSG(Channel::write, "\t\tGetting the data... (length now = %ld; remaining length = %d)", 
      								n_samples, samplesRemaining);
      // PIPELINE00 NODE1: Reading the data...
      source->getData(current, samplesRemaining); 
      DBG_MSG(Channel::write, "\t\tCopied %d samples to the storage; the buffer's state:", 
      								samplesRemaining);
      DBG_MSG(Channel::write, "\t\t%s", source->dataSourceState());
      n_samples += samplesRemaining;
      current += samplesRemaining;
   
   
   } catch (DataSourceException&) { source->dataSourceShift(-samplesRemaining); throw; }
   
   // PIPELINE00 NODE1: Discarding the read data...
   source->stopDataSource(samplesRemaining); 
   DBG_MSG(Channel::write, "\t\tReleased %d samples from the buffer.", samplesRemaining);

   return samplesRemaining;


}


//**********************************************************************************************


Channel::Channel(uint8 len) 
: DataSource<SAMPLE>(DEFAULT_CHANNEL_CBPOW)
{

   n_samples = len;
   storage = reinterpret_cast<SAMPLE*>(malloc(len * sizeof(SAMPLE)));
   current = storage;

   DBG_MSG(Channel::Channel, "\t\tA channel with room for %ld samples.", n_samples);
}


//**********************************************************************************************

Channel::Channel(Channel && oCh) noexcept
: DataSource<SAMPLE>(std::move(oCh))

{
   DBG_MSG(Channel::Channel, "\t\t(move) from %s; this channel has %ld samples.", oCh.name().data(), n_samples);
      
   n_samples   = oCh.n_samples;
   storage     = oCh.storage;
   current     = oCh.current;
   oCh.storage = nullptr;

   channel_name = oCh.channel_name;

}


//**********************************************************************************************

Channel &
Channel::operator=(Channel && oCh) 
{
   DBG_MSG(Channel::Channel, "\t\t(move) from %s; this record has %ld samples.", oCh.name().data(), n_samples);
      
   DataSource<SAMPLE>(*this) = move(oCh);

   n_samples   = oCh.n_samples;
   storage     = oCh.storage;
   current     = oCh.current;
   oCh.storage = nullptr;

   channel_name = std::move(oCh.channel_name);

   return *this;

}


//**********************************************************************************************

Channel::~Channel()
{ 
   DBG_MSG(Channel::~Channel, "\t\t%s", name().data()); 
   free(storage);
}












