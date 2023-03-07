
#include <unistd.h>    // For getopt().
#include <dshello.hpp>
#include <DataSource.hpp>
#include <processing.hpp>

//**********************************************************************************************

int main(int argc, char **argv)
{
   // Initialize input/output.
   setlocale(LC_ALL, "en_US.utf8");

   try {

      list<string> fileNames = { };
 
      if ( optind < argc ) loadFile(argv[optind]);
      else puts("No input file.");
      
   } catch (exception & e) {

      // A major TODO: reporting exceptions.

      return -1;
   }

   DBG_MSG(main, "");
   DBG_MSG(main, "\t\t\t##############################################################################");
   DBG_MSG(main, "");
   DBG_MSG(main, "\t\t\tThat's all, folks.");
   DBG_MSG(main, "\t\t\tThe file has been loaded, each channel ending in a separate storage; ");
   DBG_MSG(main, "\t\t\tright now, that's all we've got as an illustration of DataSource's operation.");
   DBG_MSG(main, "\t\t\tStay tuned, and meanwhile you can examine the code; even though this is the");
   DBG_MSG(main, "\t\t\tvery basic usage, all the essential stuff concerning a single reader is there.");

   return 0;

}


