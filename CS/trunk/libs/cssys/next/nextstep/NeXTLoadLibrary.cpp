//=============================================================================
//
//	Copyright (C)1999 by Eric Sunshine <sunshine@sunshineco.com>
//
// The contents of this file are copyrighted by Eric Sunshine.  This work is
// distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE.  You may distribute this file provided that this
// copyright notice is retained.  Send comments to <sunshine@sunshineco.com>.
//
//=============================================================================
//-----------------------------------------------------------------------------
// NeXTLoadLibrary.cpp
//
//	NextStep-specific dynamic library loading and symbol lookup.
//
//-----------------------------------------------------------------------------
#include "cssysdef.h"
#include "cssys/csshlib.h"

extern "C" {
#include <mach-o/rld.h>
#include <objc/objc-load.h>
#include <streams/streams.h>
}

//@@TODO: upon startup call csAddLibraryPath (OS_NEXT_PLUGIN_DIR)

csLibraryHandle csFindLoadLibrary (const char *iName)
{
  return csFindLoadLibrary (NULL, iName, OS_NEXT_PLUGIN_EXT);
}

//-----------------------------------------------------------------------------
// csLoadLibrary
//-----------------------------------------------------------------------------
csLibraryHandle csLoadLibrary( char const* lib )
    {
    csLibraryHandle handle = 0;
    char const* const files[2] = { lib, 0 };
    struct mach_header* header = 0;
    NXStream* stream = NXOpenFile( 1, NX_WRITEONLY );
    if (objc_loadModules( (char**)files, stream, 0, &header, 0 ) == 0)
	handle = (csLibraryHandle)header;
    else
	NXPrintf( stream, "Unable to load library '%s'.\n", file );
    NXClose( stream );

    return handle;
    }


//-----------------------------------------------------------------------------
// csGetLibrarySymbol
//-----------------------------------------------------------------------------
void* csGetLibrarySymbol( csLibraryHandle handle, char const* s )
    {
    char* symbol = new char[ strlen(s) + 2 ];	// Prepend an underscore '_'.
    *symbol = '_';
    strcpy( symbol + 1, s );

    unsigned long address = 0;
    NXStream* stream = NXOpenFile( 1, NX_WRITEONLY );
    if (rld_lookup( stream, symbol, &address ) == 0)
	NXPrintf( stream, "Symbol undefined: %s\n", symbol );
    NXClose( stream );

    delete[] symbol;
    return (void*)address;
    }


//-----------------------------------------------------------------------------
// csUnloadLibrary
//-----------------------------------------------------------------------------
bool csUnloadLibrary( csLibraryHandle )
    {
    return true; // Unimplemented.
    }
