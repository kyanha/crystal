/*
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter

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

#ifndef __CS_CSSYS_WIN32_REGISTRYCFG_H__
#define __CS_CSSYS_WIN32_REGISTRYCFG_H__

#include "iutil/cfgfile.h"
#include "csutil/scf.h"
#include "csutil/strhash.h"
#include "csutil/parray.h"

#include <windows.h>

class csRegistryIterator;

/**
 * An iConfigFile, storing the settings in the Windows registry.
 */
class csRegistryConfig : public iConfigFile
{
private:
  friend class csRegistryIterator;

  HKEY hKey;
  char* Prefix;
  // whether this key is opened w/ write access.
  bool writeAccess;
  char* Key;

  typedef struct 
  {
    csStringHash strings;
  } rcStatus;
  rcStatus* status;

  csPArray<csRegistryIterator> iters;

  // convert CS "x.y.z" keys to registry "x\y\z"
  void ReplaceSeparators (char* key) const;

  bool TryOpen (HKEY& regKey, DWORD access, const char* keyName, bool create);

  // convenience class, used to delete[] a buffer on function return
  struct Block_O_Mem
  {
    BYTE* data;
    DWORD size;
    Block_O_Mem () : data(0), size(0) { }
    void Clear() { delete[] data; }
    void SetSize (DWORD sz) { Clear(); size = sz; data = new BYTE[sz]; }
    ~Block_O_Mem() { Clear(); }
  };
  // To shorten calls to RegSetValueEx()
  bool InternalSetValue (const char* Key,
    DWORD type, const void* data, DWORD datasize);
  // To shorten calls to RegQueryValueEx()
  bool InternalGetValue (const char* Key,
    DWORD& type, Block_O_Mem& data) const;

  /*
    Helper functions to convert the data from the registry to the 
    requested format
  */
  int RegToInt (DWORD type, Block_O_Mem& data, int Def) const;
  float RegToFloat (DWORD type, Block_O_Mem& data, float Def) const;
  const char* RegToStr (DWORD type, Block_O_Mem& data, const char* Def) const;
  bool RegToBool (DWORD type, Block_O_Mem& data, bool Def) const;

  // Check whether we have registry write access.
  bool WriteAccess();
public:
  SCF_DECLARE_IBASE;

  csRegistryConfig ();
  virtual ~csRegistryConfig();

  bool Open (const char* Key);

  virtual const char* GetFileName () const;
  virtual iVFS* GetVFS () const;
  virtual void SetFileName (const char*, iVFS*);
  virtual bool Load (const char* iFileName, iVFS* = 0, bool Merge = false,
    bool NewWins = true);
  virtual bool Save ();
  virtual bool Save (const char *iFileName, iVFS* = 0);
  
  virtual void Clear ();
  
  virtual csPtr<iConfigIterator> Enumerate (const char *Subsection = 0);
  virtual bool KeyExists (const char *Key) const;
  virtual bool SubsectionExists (const char *Subsection) const;
  
  virtual int GetInt (const char *Key, int Def = 0) const;
  virtual float GetFloat (const char *Key, float Def = 0.0) const;
  virtual const char *GetStr (const char *Key, const char *Def = "") const;
  virtual bool GetBool (const char *Key, bool Def = false) const;
  virtual const char *GetComment (const char *Key) const;

  virtual void SetStr (const char *Key, const char *Val);
  virtual void SetInt (const char *Key, int Value);
  virtual void SetFloat (const char *Key, float Value);
  virtual void SetBool (const char *Key, bool Value);
  virtual bool SetComment (const char *Key, const char *Text);
  virtual void DeleteKey (const char *Key);
  virtual const char *GetEOFComment () const;
  virtual void SetEOFComment (const char *Text);
};

/**
 * Iterates over a registry key subkeys and values.
 */
class csRegistryIterator : public iConfigIterator
{
  csRef<csRegistryConfig> owner;

  typedef struct 
  {
    csStringHash strings;
  } riStatus;
  riStatus* status;

  DWORD EnumIndex;

  char* SubsectionName;

  // shortcut to RegEnumValue/RegQueryValueEx
  bool GetCurrentData (DWORD& type, 
    csRegistryConfig::Block_O_Mem& data) const;
public:
  SCF_DECLARE_IBASE;

  csRegistryIterator (csRegistryConfig* Owner, 
    const char* Subsection);
  virtual ~csRegistryIterator();

  virtual iConfigFile *GetConfigFile () const;
  virtual const char *GetSubsection () const;

  virtual void Rewind ();
  virtual bool Next();

  virtual const char *GetKey (bool Local = false) const;
  virtual int GetInt () const;
  virtual float GetFloat () const;
  virtual const char *GetStr () const;
  virtual bool GetBool () const;
  virtual const char *GetComment () const;
};

#endif

