#!/bin/sh
# Checks for an installed nasm

# Check if NASM is installed and if it has the right version
NASMBIN=`which nasm 2>&1 | grep -v "[Nn]o"`

if [ -n "${NASMBIN}" ]; then
  #create a dummy NASM program
  echo "%xdefine TEST" >conftest.asm
  # Well, we really should check here for obj format...
  # but we'll use ELF as it really doesn't matter
  ${NASMBIN} -f elf conftest.asm -o conftest.o 2>/dev/null && echo "NASM.INSTALLED = yes"
  rm -f conftest.asm conftest.o
fi

# Check if makedep is installed and is the right version
[ -z "${MAKEDEP}" ] && MAKEDEP=`which makedep 2>&1 | grep -v "[Nn]o"`
if [ -n "${MAKEDEP}" ]; then
  echo "DEPEND_TOOL = mkdep"
  MAKEDEP_VERSION=`makedep -V | sed -e "s/.*Version *//"`
  if [ `expr "${MAKEDEP_VERSION}" ">" 0.0.0` = "1" ]; then
    echo "MAKEDEP.INSTALLED = yes"
  fi
fi

# do we have bison/flex
BISONBIN=`which bison 2>&1 | grep -v "[Nn]o"`
FLEXBIN=`which flex 2>&1 | grep -v "[Nn]o"`
if [ -n "${FLEXBIN}" ]; then
    echo "FLEXBIN = ${FLEXBIN}"
fi
if [ -n "${BISONBIN}" ]; then
    echo "BISONBIN = ${BISONBIN}"
fi

# do we have swig < 1.3
SWIGBIN=`which swig 2>&1 | grep -v "[Nn]o"`
if [ -n "${SWIGBIN}" ]; then
    echo "SWIGBIN = ${SWIGBIN}"
fi

