/*
    Copyright (C) 1997, 1998, 1999 by Alex Pfaffe
	(Digital Dawn Graphics Inc)
  
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
  
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
// For timing info.
#ifndef WIN32
#include <sys/types.h>
#include <sys/times.h>
#else
#include <time.h>
#endif
#ifdef DDG
#include "ddgutil.h"
#else
#include "csterr/ddgutil.h"
#endif
// ----------------------------------------------------------------------
//
// ddgUtil:
//
//  See if we are running on a Pentium III.
//

bool ddgUtil::DetectSIMD(void)
{

#ifdef WIN32
bool found_simd;
_asm

{
pushfd
pop eax // get EFLAGS into eax
mov ebx,eax // keep a copy
xor eax,0x200000 
// toggle CPUID bit

push eax
popfd // set new EFLAGS
pushfd
pop eax // EFLAGS back into eax

xor eax,ebx 
// have we changed the ID bit?

je NO_SIMD 
// No, no CPUID instruction

// we could toggle the 
// ID bit so CPUID is present
mov eax,1

cpuid // get processor features
test edx,1<<25 // check the SIMD bit
jz NO_SIMD
mov found_simd,1 
jmp DONE
NO_SIMD:
mov found_simd,0
DONE:
}

return found_simd;
#else
return false;
#endif

}

// ----------------------------------------------------------------------
float ddgAngle::_cosTable[180*ddgAngle_res+1];
float ddgAngle::_acosTable[180*ddgAngle_res+1];
float ddgAngle::pi = M_PI;
ddgAngle ddg_init;	// Ensure table gets initialized.

ddgAngle::ddgAngle()
{
	int i;
	// Calculate from 0.0 to 180.0 degrees.
	for (i = 0; i <= 180*ddgAngle_res; i++)
	{
		_cosTable[i] = ::cos(M_PI*(float)i/(float)(ddgAngle_res*180));
	}
	_cosTable[90*ddgAngle_res] = 0;
	for (i = 0; i <= 180.0*ddgAngle_res; i++)
	{
		_acosTable[i] = ddgAngle::radtodeg(::acos((float)i/(180.0*ddgAngle_res)));
	}
	_acosTable[90*ddgAngle_res] = 1;
}

// This should be inlined, but W32 wont let me use _cosTable in a DLL
// if it is inlined.
float ddgAngle::cos( float angle)
{
	float a = mod(angle);
	return a < 180.0f ?
		_cosTable[((int)a*ddgAngle_res)]:
		-_cosTable[((int)(a-180.0f)*ddgAngle_res)];
}

float ddgAngle::acos( float value)
{
	int i = int(value*180*ddgAngle_res);
	int k = i;
	if (k < 0) k = -1 *k;
	if (k > 2880) k = 2880 - k;
	float angle = _acosTable[k];
	if (value < 0.0f)
		angle += 180;
//	if (i > 2880)
//		angle = 360.0 - angle;
	return angle;
}




