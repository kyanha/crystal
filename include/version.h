#ifndef __CS_VERSION_H__
#define __CS_VERSION_H__

#define CS_VERSION	"0.17"	/* NOTE: Update docs/texinfo/version.txi  */
#define CS_SUBVERSION	"001"	/* whenever updating these two values.    */
#define RELEASE_DATE	"Wed, 1-Sep-2000"

#if defined(OS_SOLARIS)
#  define OS_VERSION "Solaris"
#elif defined(OS_LINUX)
#  define OS_VERSION "Linux"
#elif defined(OS_BSD)
#  define OS_VERSION "BSD"
#elif defined(OS_IRIX)
#  define OS_VERSION "Irix"
#elif defined(OS_NEXT)
#  define OS_VERSION OS_NEXT_DESCRIPTION    /* Must appear before OS_UNIX */
#elif defined(OS_BE)
#  define OS_VERSION "BeOS"                 /* Must appear before OS_UNIX */
#elif defined(OS_UNIX)
#  define OS_VERSION "Unix"
#elif defined(OS_DOS)
#  define OS_VERSION "DOS"
#elif defined(OS_MACOS)
#  define OS_VERSION "Macintosh"
#elif defined(OS_WIN32)
#  define OS_VERSION "Win32"
#elif defined(OS_OS2)
#  define OS_VERSION "OS/2"
#elif defined(OS_PS2)
#  define OS_VERSION "Playstation/2"
#elif defined(OS_UNKNOWN)
#  define OS_VERSION "Unknown"
#else
#  error "Unspecified operating system!"
#endif

#if defined(PROC_INTEL)
#  define PR_VERSION "Intel"
#elif defined(PROC_SPARC)
#  define PR_VERSION "SPARC"
#elif defined(PROC_MIPS)
#  define PR_VERSION "MIPS"
#elif defined(PROC_POWERPC)
#  define PR_VERSION "PowerPC"
#elif defined(PROC_M68K)
#  define PR_VERSION "M68K"
#elif defined(PROC_HPPA)
#  define PR_VERSION "PA-RISC"
#elif defined(PROC_ALPHA)
#  define PR_VERSION "Alpha"
#elif defined(PROC_EE)
#  define PR_VERSION "EmotionEngine"
# elif defined(PROC_UNKNOWN)
#  define PR_VERSION "Unknown"
#else
#  error "Unspecified processor!"
#endif

#if defined(COMP_GCC)
#  define CC_VERSION "GCC"
#elif defined(COMP_MWERKS)
#  define CC_VERSION "MWERKS"
#elif defined(COMP_VC)
#  define CC_VERSION "VisualC"
#elif defined(COMP_BC)
#  define CC_VERSION "Borland"
#elif defined(COMP_UNKNOWN)
#  define CC_VERSION "Unknown"
#else
#  error "Unspecified compiler!"
#endif

#define VERSION CS_VERSION \
  " r" CS_SUBVERSION " [" OS_VERSION "-" PR_VERSION "-" CC_VERSION "]"

#endif // __CS_VERSION_H__
