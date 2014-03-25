/********************************************************************************
 * Add Intwine information here
 ********************************************************************************/

// This file contains device-specific data information

#ifdef WIN32

  #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
  #include <windows.h>
  #include <stdio.h>
  #include <string.h>

#else

  #include "HardwareProfile.h"
//  #include "Metrics.h"
  #include <stdio.h>
  #include <string.h>

#endif

void Assert(int flag, char *file, unsigned long line, char *msg);

void Assert(int flag, char *file, unsigned long line, char *msg)
{
    char *just_the_file_please;

  if(flag == 0)
  {
    just_the_file_please = strrchr(file, '\\');
    if(just_the_file_please != NULL)
    {
      // Add 1 to get past the actual character
      printf("Assert in File: %s,  Line: %ld,  Problem: %s\n", just_the_file_please+1, line);
    }
    else
    {
      // Not found, just show the entire file information
      printf("Assert in File: %s,  Line: %ld,  Problem: %s\n", file, line);
    }
//    IncrementMetrics(WATCHDOG_ASSERT);

//    RadioReset();
    while(1);
  }
}

void AssertInt(int flag, char *file, unsigned long line, char *msg, int intval)
{
  if(flag == 0)
  {
//      IncrementMetrics(intval);
      Assert(flag, file, line, msg);
  }
}

