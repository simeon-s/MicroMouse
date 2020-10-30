#ifndef STRINGS_H_
#define STRINGS_H_

#include <stdint.h>
#include <stdbool.h>


#define BIT_TYPE	2				// for check boxes - 0 or 1

typedef const char * ptr_STR;
typedef char ptr_S[16];

extern ptr_STR const str_MazeStart[];
extern ptr_STR const str_MazeRun[];


#endif /*STRINGS_H_*/
