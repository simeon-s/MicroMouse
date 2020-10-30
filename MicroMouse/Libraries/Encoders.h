/*
Copyright (c) Simeon Simeonov
 Author : Simeon Simeonov
 Name : Opical encoders driver
 Ver: 1.0.0

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "../Inc/main.h"


// ensure this library description is only included once
#ifndef OPT_ENCODERS_H
#define OPT_ENCODERS_H

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif


extern volatile uint32_t enc_leftCounter;
extern volatile uint32_t enc_rightCounter;

extern volatile uint32_t enc_last_leftCounter;
extern volatile uint32_t enc_last_rightCounter;

extern volatile uint32_t enc_left_speed;
extern volatile uint32_t enc_right_speed;


// library interface description

  void EncodersInit(void);

  // get Counters :
  uint32_t enc_getLeftCounter(void);
  uint32_t enc_getRightCounter(void);

  // Clear counters
  void clear_encoders(void);
  void enc_clearLeftCounter(void);
  void enc_clearRightCounter(void);

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif


