/*
   REXX script for autodetecting build environment on OS/2 systems

   Written by Andrew Zabolotny, <bit@eltech.ru>
*/

  '@echo off';

  parse arg args;
  /* Interpret all args as they would be assignments */
  do i = 1 to words(args)
    interpret word(args, i)' = "'word(args, i + 1)'"';
    i = i + 1;
  end;

  call RxFuncAdd "SysLoadFuncs", "RexxUtil", "SysLoadFuncs";
  call SysLoadFuncs;
  call InitializeANSI;

  call saycon ANSI.DGRAY"��������������������������������������"
  call saycon ANSI.WHITE" Sniffing your OS/2 build environment"
  call saycon ANSI.DGRAY"��������������������������������������"

  call outcon ANSI.LGREEN"Detecting your C compiler ... "
  CC = SysSearchPath("PATH", "gcc.exe");

  if (CC = "") then
  do
    call saycon;
    call problem "high",
      "Cannot find a suitable C compiler."NL,
      " You should install a version of GNU C compiler to use this target."
    exit 1;
  end;

  call saycon ANSI.WHITE||CC;
  say "CC = "filespec('Name', left(CC, length(CC) - 4))" -c";

  call outcon ANSI.LGREEN"Detecting your C++ compiler ... "

  /* First, check which C++ compiler we are using */
  CXX = SysSearchPath("PATH", "g++.exe");
  if (CXX = "") then
    CXX = SysSearchPath("PATH", "gpp.exe");
  if (CXX = "") then
    CXX = SysSearchPath("PATH", "gcc.exe");

  if (CXX = "") then
  do
    call saycon;
    call problem "high",
      "Cannot find a suitable C++ compiler."NL,
      " You should install a version of GNU C++ compiler to use this target."
    exit 1;
  end;

  call saycon ANSI.WHITE||CXX;
  say "CXX = "filespec('Name', left(CXX, length(CXX) - 4))" -c";

  call saycon ANSI.LGREEN"Checking for advanced C++ compiler options"

  /* Now create a dummy C++ source file */
  testcpp = "conftest.cpp";
  testobj = "conftest.o";
  call lineout testcpp, "int main () {}"
  call stream testcpp, "C", "CLOSE";

  /* Check if compiler supports exceptions */
  call cxxcheck "-fno-exceptions"

  /* Check if compiler supports rtti */
  call cxxcheck "-fno-rtti"

  /* Check if compiler supports pentium target arch */
  call cxxcheck "-mpentium -march=pentium"

  /* Remove temporary files */
  call SysFileDelete testcpp
  call SysFileDelete testobj

  testasm = "conftest.asm";
  call lineout testasm, "%xdefine TEST"
  call stream testasm, "C", "CLOSE";

  call saycon ANSI.LGREEN"Testing whenever you have (the right version of) NASM installed"

  "nasm -f win32 "testasm" -o "testobj" >nul 2>&1"
  if rc = 0 then
  do
    call saycon  ANSI.LCYAN||"      supported: "ANSI.WHITE"USE_NASM = yes"
    say "USE_NASM = yes"
  end;
  /* Remove temporary files */
  call SysFileDelete testasm
  call SysFileDelete testobj

  /* Now find Resource Compiler */
  testrc = "conftest.rc";
  testres = "conftest.res";
  call lineout testrc, ""
  call stream testrc, "C", "CLOSE";

  call outcon ANSI.LGREEN"Detecting your Resource Compiler ... "

  RCC = SysSearchPath("PATH", "rc.exe");
  if (RCC = "") then
  do
    call problem "fatal",
      "Resource Compiler has not been found along your PATH."NL,
      " You won't be able to compile Crystal Space without this tool."NL,
      " Please install it using Selective Install and re-configure.";
    exit 1;
  end;

  call saycon ANSI.WHITE||RCC;
  say "RC = "filespec('Name', left(RCC, length(RCC) - 4));

  call saycon ANSI.LGREEN"Checking for advanced Resource Compiler options"

  call checkopt RCC" -r", "-n", testrc, "RCFLAGS.SYSTEM";
  call checkopt RCC" -r", "-x2", testrc, "RCFLAGS.SYSTEM";

  /* Remove temporary files */
  call SysFileDelete testrc
  call SysFileDelete testres

  /* Finally, check if some Unix-like tools are present */

  call outcon ANSI.LGREEN"Checking for a Unix-like shell ... "
  SHELL = value('SHELL',,'os2environment');
  if (SHELL = "") then
    SHELL = SysSearchPath("PATH", "sh.exe");
  if (SHELL = "") then
    SHELL = SysSearchPath("PATH", "ash.exe");
  if (SHELL = "") then
    SHELL = SysSearchPath("PATH", "bash.exe");
  if (SHELL = "") then
    SHELL = SysSearchPath("PATH", "ksh.exe");
  if (SHELL = "") then
    SHELL = SysSearchPath("PATH", "csh.exe");
  if (SHELL = "") then
    call problem "medium",
      "A Unix-like (Bourne-like, to be correct) shell"NL,
      " has not been found along your PATH. It is not strictly required,"NL,
      " however, you should be prepared CMD.EXE to crash on very long"NL,
      " command lines";
  else
  do
    call saycon ANSI.WHITE||SHELL;
    say "SHELL = "SHELL;
  end;

  if (SHELL = "") | (pos("CMD", translate(SHELL)) > 0) then
  do
    say '" ='
    say '| = �'
  end

  if (SysSearchPath("PATH", "sed.exe") = "") then
    call problem "low",
      "GNU sed has not been found along your PATH."NL,
      " You won't be able to build dependencies (MAKE depend)";
  if (SysSearchPath("PATH", "rm.exe") = "") | (SysSearchPath("PATH", "mkdir.exe") = "") then
    call problem "medium",
      "GNU file utilites have not been found along your PATH."NL,
      " GNU futils are used in several places in makefiles, for example rm is used"NL,
      " in 'make cleanXXX' targets, mkdir.exe is used to create output directories.";
  if (SysSearchPath("PATH", "doc++.exe") = "") then
    call problem "low",
      "doc++ has not been found along your PATH. You won't"NL,
      " be able to re-build automatically-generated documentation files"NL,
      " ('make doc' and 'make api' targets)";

  /* Restore console color to gray */
  call outcon ANSI.LGRAY;

exit 0;

/* Report a problem */
problem:
  parse arg severity text;
  call saycon ANSI.YELLOW"��[ PROBLEM ]��"
  call saycon ANSI.BROWN"  SEVERITY: "ANSI.LRED||severity
  call saycon ANSI.BROWN"  DESCRIPTION: "ANSI.WHITE||text
return;

/* Check if C++ compiler supports a specific switch */
cxxcheck:
  parse arg opt;
return checkopt(CXX" -c", opt, testcpp, "CFLAGS.SYSTEM");

/* Check if a tool supports a specific option */
checkopt:
  parse arg tool, opt, testfile, varname;

  tool" "opt" "testfile" >nul 2>&1"
  if (rc = 0) then
  do
    call saycon  ANSI.LCYAN||"      supported: "ANSI.WHITE||opt
    say varname" += "opt;
  end; else
  do
    call saycon ANSI.YELLOW||"  not supported: "ANSI.WHITE||opt
    return 0;
  end;
return 1;

/* Output a string to console independently of where stdout is redirected */
saycon:
  parse arg outstr;
  call charout "/dev/con", outstr||NL;
return;

/* Same but without ending CR/LF */
outcon:
  parse arg outstr;
  call charout "/dev/con", outstr;
return;

/* Output a error string and exit with an error status */
sayerr:
  parse arg outstr;
  call charout "/dev/con", NL||ANSI.LRED||outstr||ANSI.LGRAY||NL;
exit 1;

/*
 * Initialize ANSI control strings
 */
InitializeANSI:
  ANSI.BLACK    = '1B'x'[0;30m';
  ANSI.BLUE     = '1B'x'[0;34m';
  ANSI.GREEN    = '1B'x'[0;32m';
  ANSI.CYAN     = '1B'x'[0;36m';
  ANSI.RED      = '1B'x'[0;31m';
  ANSI.MAGENTA  = '1B'x'[0;35m';
  ANSI.BROWN    = '1B'x'[0;33m';
  ANSI.LGRAY    = '1B'x'[0;37m';

  ANSI.DGRAY    = '1B'x'[1;30m';
  ANSI.LBLUE    = '1B'x'[1;34m';
  ANSI.LGREEN   = '1B'x'[1;32m';
  ANSI.LCYAN    = '1B'x'[1;36m';
  ANSI.LRED     = '1B'x'[1;31m';
  ANSI.LMAGENTA = '1B'x'[1;35m';
  ANSI.YELLOW   = '1B'x'[1;33m';
  ANSI.WHITE    = '1B'x'[1;37m';

  NL="0D0A"x;
return;
