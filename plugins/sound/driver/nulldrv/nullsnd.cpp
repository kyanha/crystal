/*
        NULL sound driver by Norman Kr�mer <normank@lycosmail.com>

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

#include "cssysdef.h"
#include "nullsnd.h"

IMPLEMENT_FACTORY(csSoundDriverNULL)

EXPORT_CLASS_TABLE(snddrv0)
    EXPORT_CLASS(csSoundDriverNULL, "crystalspace.sound.driver.null", "NULL Sounddriver for Crystal Space")    
EXPORT_CLASS_TABLE_END;

IMPLEMENT_IBASE(csSoundDriverNULL)
  IMPLEMENTS_INTERFACE(iPlugIn)
  IMPLEMENTS_INTERFACE(iSoundDriver)
IMPLEMENT_IBASE_END;

