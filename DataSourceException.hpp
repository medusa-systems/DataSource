#include <cinttypes>
#include <cstring>

   enum 
   { 
      DSEXC_B_TOO_MANY_READERS,
      DSEXC_B_PUSH_BACK,
      DSEXC_B_FWRITE,
      DSEXC_B_MALLOC,

      DSEXC_M_REGISTER,  
      DSEXC_M_GET,       
      DSEXC_M_CONSTR,    
      DSEXC_M_CONSTR_COPY,

      DSEXC_A_NEW_READER,
      DSEXC_A_DS_CURRENT,
      DSEXC_A_COPY_DATA,
      DSEXC_A_ALLOCATE
   };


   class DataSourceException
   {
   
      public:
   
#define ORIG_MSG_SIZE 1024
#define DS_EXC_STR_SIZE  64
#define DS_EXC_STRINGS   12

      private:

      static char dse_string[DS_EXC_STRINGS][DS_EXC_STR_SIZE];

      thread_local static char * dse_brief;
      thread_local static char * dse_method;
      thread_local static char * dse_action;

      thread_local static char orig_msg[ORIG_MSG_SIZE];

      public:

      char * const brief()   { return dse_brief;  }
      char * const method()  { return dse_method; }
      char * const action()  { return dse_action; }
      char * const origMsg() { return orig_msg;   }
      bool hasOrigMsg()      { return (orig_msg[0] != 0); }

      DataSourceException(uint8_t b, uint8_t m, uint8_t a)
      {
         dse_brief  = dse_string[b];
         dse_method = dse_string[m];
         dse_action = dse_string[a];
         orig_msg[0] = 0;
      }

      DataSourceException(uint8_t b, uint8_t m, uint8_t a, const char * o)
      {
         dse_brief  = dse_string[b];
         dse_method = dse_string[m];
         dse_action = dse_string[a];
         strncpy(orig_msg, o, ORIG_MSG_SIZE);
      }

   };

