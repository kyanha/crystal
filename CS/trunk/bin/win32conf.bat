@echo off

  rem *** If you expect to see a sophisticated detection script here,
  rem *** you opened a wrong file :-))) The so-called "batch command
  rem *** language" is so poor that we can only hope the user have the
  rem *** configuration we are expecting.

  echo ### Testing whenever you have (the right version of) NASM installed ...
  echo %%xdefine TEST >conftest.asm
  nasm -f win32 conftest.asm -o conftest.o
  if not exist conftest.o goto nonasm

  del conftest.o
  echo $$$ O.K., setting NASM.INSTALLED to "yes"
  echo NASM.INSTALLED = yes>>config.tmp

:nonasm
  del conftest.asm

  type bin\win32conf.var >>config.tmp

:exit
