/*
    This header is used by COMP_BC, COMP_VC and COMP_GCC for all OS_WIN32 builds
    You can change these macros to suit your own needs
    For a description of what each macro does, see mk/user.mak
*/
#ifndef __VOLATILE_H__
#define __VOLATILE_H__

#define OS_WIN32
#define PROC_INTEL

#if defined(__BORLANDC__)
  #define COMP_BC
  #define __NEED_GENERIC_ISDIR
#else
# if defined(__MINGW32__)
// Don't need no stinkin' Generic ISDIR
#	define COMP_GCC
# else
#	define COMP_VC
# endif
#endif

#if !defined (COMP_GCC)
  #define DO_SOUND
  #define DO_GIF
  #define DO_BMP
  #define DO_TGA
  #define DO_PNG
  #define DO_JPG
  #define DO_AIFF
  #define DO_IFF
  #define DO_WAV
  #define DO_AU
  #define DO_MMX
  #define ZLIB_DLL
#endif

// if you don't want to use direct input, comment this out
// keyboard handler will default to window message handling.
#define DO_DINPUT_KEYBOARD

#if defined(COMP_VC)
// #ifdef _DEBUG
    //Right now, Inline assembler doesn' work any more on MSVC, 
    //This needs to be examined further. Thomas Hieber. 07/17/1999
    #define NO_ASSEMBLER
// #endif
#endif

#endif // __VOLATILE_H__
