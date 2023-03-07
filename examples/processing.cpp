
#include <dshello.hpp>

#include <DataSource.hpp>
#include <SoundSource.hpp>
#include <Channel.hpp>
#include <sndfile.h>
#include <Container.hpp>


bool 
loadFile(const char * fileName)
{
   std::vector<Channel> * d;

   char num[8];

   try {
 
      DBG_MSG(loadFile, "\t\t\tOpening the file %s...", fileName);
      FILE * input = fopen(fileName, "r");
      if ( input == nullptr ) { 
         DBG_MSG(loadFile, "\t\t\tOpening FAILED.");
         return false;
      }
 
      DBG_MSG(loadFile, "\t\t\tMaking the container object...");
      Container * c = new Container(input);

      uint1 channels = c->channels();
      const uint8 length = c->length();
 
      DBG_MSG(loadFile, "\t\t\tMaking the channels...");
      d = new std::vector<Channel>;
      for (int channelN = 0; channelN < channels; channelN++) {
         d->emplace_back();
	 d->back().inflate(length);
         sprintf(num, "%d", channelN);
         string x = "Channel ";
	 d->back().setName(x + num);
         DBG_MSG(loadFile, "\t\t\t%s has room for %ld samples now.", 
	 		d->back().name().data(), length);
      }

      assert(d->size() == channels);

      // PIPELINE00: Assembling...
      DBG_MSG(loadFile, "\t\t\tConnecting the channels to the container...");
      for (uint1 n = 0; n < c->channels(); n++) {
	 std::next(d->begin(), n)->attach(c->soundForChannel(n));
         DBG_MSG(loadFile, "\t\t\t\"%s\" connected.", 
	 			std::next(d->begin(), n)->name().data());
      }

      DBG_MSG(loadFile, "\t\t\tMarking the writing operation as started...");
      for (auto ch = d->begin(); ch != d->end(); ch++) { ch->setWritingStarted(); }
 
      uint8 frames_decoded = 0;
      bool pipelinesWork = true;

      do 
      {
 
         DBG_MSG(loadFile, "\t\t\t=========================================================");
         DBG_MSG(loadFile, "\t\t\tLoading from the frame %ld.", frames_decoded);
         DBG_MSG(loadFile, "\t\t\t=========================================================");
         DBG_MSG(loadFile, "\t\t\t===> Operating the container...");

	 // PIPELINE00 NODE0 Operated from here...
         c->getSound();
         
         DBG_MSG(loadFile, "\t\t\t===> Operating the writer...");

	 uint4 samplesAdded;

	 for (auto ch = d->begin(); ch != d->end(); ch++) 
	 { 
            DBG_MSG(loadFile, "\t\t\t--> Starting filling the input_sound of %s with %d samples...", 
	    						ch->name().data(), c->amount());
	    // PIPELINE00 NODE1 Operated from here (we consider a channel the receiver)...
	    samplesAdded = ch->write(); 
            if ( samplesAdded ) 
               DBG_MSG(loadFile, "\t\t\tThe input_sound of %s filled.", ch->name().data());
	 }
 
         frames_decoded += samplesAdded;

	 for (auto ch = d->begin(); ch != d->end(); ch++) 
	 { 
            if ( ch->writingFinished() ) pipelinesWork = false;
	    else { pipelinesWork = true; break; }
	 }

      } while ( pipelinesWork );
 
      // PIPELINE00: Disassembling...
      DBG_MSG(loadFile, "\t\t\tDisconnecting the sources...");
      for (auto ch = d->begin(); ch != d->end(); ch++) ch->detach();
 
      DBG_MSG(loadFile, "");
      DBG_MSG(loadFile, "\t\t\t====================== THE FILE LOADED. =======================");
      DBG_MSG(loadFile, "");
      DBG_MSG(loadFile, "\t\t\tThe document length = %ld", length);

      DBG_MSG(loadFile, "\t\t\tDeleting the container object...");
      delete c;
 
      DBG_MSG(loadFile, "\t\t\tClosing the input file...");
      fclose(input);
 
   } catch (DataSourceException & e) { 

      // A major TODO: reporting exceptions
      return false; 
   }

   return true;

}



