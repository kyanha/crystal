/*
    Copyright (C) 2003 by Greg Block

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
#include "csutil/util.h"

#include "defaultsconfig.h"

csPtr<iConfigFile> csGetPlatformConfig (const char* key)
{
  csDefaultsConfig* cfg = new csDefaultsConfig();
  if (!cfg->Open (key))
  {
    delete cfg; cfg = 0;
  }
  return csPtr<iConfigFile> (cfg);
}


SCF_IMPLEMENT_IBASE (csDefaultsConfig)
  SCF_IMPLEMENTS_INTERFACE (iConfigFile)
SCF_IMPLEMENT_IBASE_END

csDefaultsConfig::csDefaultsConfig ()
{
  SCF_CONSTRUCT_IBASE (0);

  // Domain information comes from the application's bundle identifier, generally.
  // Grab a defaults object.
  defaults = [[NSUserDefaults standardUserDefaults] retain];
  domain=nil;
  dict=nil;
}

csDefaultsConfig::~csDefaultsConfig()
{
  if (dict!=nil) {
    [dict release];
  }

  // Release the name of our domain
  if (domain!=nil) [domain release];

  // Delete our defaults object.
  if (defaults!=nil) [defaults release];
}

bool csDefaultsConfig::Open (const char* Key)
{
  domain = [[[NSString alloc] initWithCString:Key] retain];
  dict = [[[defaults persistentDomainForName:domain] mutableCopy] retain];
  
  if (dict==nil) {
      dict = [[[NSMutableDictionary alloc] init] retain];
  }
  
  return true;
}

const char* csDefaultsConfig::GetFileName () const
{
  return "NSUserDefaults";
}

iVFS* csDefaultsConfig::GetVFS () const
{
  return 0;
}

void csDefaultsConfig::SetFileName (const char*, iVFS*)
{
}

bool csDefaultsConfig::Load (const char* iFileName, iVFS*, bool Merge,
    bool NewWins)
{
  // Will at some point implement loading and merging a serialised NSDictionary.
  return false;
}

bool csDefaultsConfig::Save ()
{
  if (dict!=nil) {
    [defaults setPersistentDomain:dict forName:domain];
    [defaults synchronize];
  }
  return true;
}

bool csDefaultsConfig::Save (const char *iFileName, iVFS*)
{
  Save();
  // Will at some point implement saving to a serialised NSDictionary.
  return false;
}

void csDefaultsConfig::Clear ()
{
  [defaults removePersistentDomainForName:domain];
}

csPtr<iConfigIterator> csDefaultsConfig::Enumerate (const char *Subsection)
{
  if (!SubsectionExists (Subsection)) return 0;

  csDefaultsIterator* it = new csDefaultsIterator (this, Subsection);
  return it;
}

bool csDefaultsConfig::KeyExists (const char *Key) const
{
  NSString *keystring = [NSString stringWithCString:Key];
  NSObject *obj = [dict objectForKey:keystring];

  return (obj!=nil);
}

// Look to see, if we add a given subsection to our bundle ID, whether or not it exists.
bool csDefaultsConfig::SubsectionExists (const char *subsection) const
{
  NSString *section = [NSString stringWithFormat:@"%@.%s", domain, subsection];
  NSDictionary *subdict = [defaults persistentDomainForName:section];

  return (subdict!=nil);
}

int csDefaultsConfig::GetInt (const char *Key, int Def) const
{
  NSString *keystring = [NSString stringWithCString:Key];
  id obj = [dict objectForKey:keystring];
  if (obj!=nil && [obj isMemberOfClass:[NSNumber class]]) {
    return [obj intValue];
  } 
  else return Def;
}

float csDefaultsConfig::GetFloat (const char *Key, float Def) const
{
  NSString *keystring = [NSString stringWithCString:Key];
  id obj = [dict objectForKey:keystring];
  if (obj!=nil && [obj isMemberOfClass:[NSNumber class]]) {
    return [obj floatValue];
  } 
  else return Def;
}

const char *csDefaultsConfig::GetStr (const char *Key, const char *Def) const
{
  NSString *keystring = [NSString stringWithCString:Key];
  id obj = [dict objectForKey:keystring];
  if (obj!=nil) {
  	return [[obj description] cString];
  }
  else return Def;
}

bool csDefaultsConfig::GetBool (const char *Key, bool Def) const
{
  NSString *keystring = [NSString stringWithCString:Key];
  id obj = [dict objectForKey:keystring];
  if (obj!=nil && [obj isMemberOfClass:[NSNumber class]]) {
    return (bool)[obj boolValue];
  } 
  else return Def;
}

const char *csDefaultsConfig::GetComment (const char *Key) const
{
  return 0;
}

void csDefaultsConfig::SetStr (const char *Key, const char *Val)
{
  NSString *keystr = [NSString stringWithCString:Key];
  NSString *valstr = [NSString stringWithCString:Val];
  
  // If it's not an administrator-owned key, perform the write.
  if (!KeyExists(Key) || ![defaults objectIsForcedForKey:keystr inDomain:domain]) {
    [dict setObject:valstr forKey:keystr];
  }
}

void csDefaultsConfig::SetInt (const char *Key, int Value)
{
  NSString *keystr = [NSString stringWithCString:Key];
  NSNumber *val = [NSNumber numberWithInt:Value];

  // If it's not an administrator-owned key, perform the write.
  if (!KeyExists(Key) || ![defaults objectIsForcedForKey:keystr inDomain:domain]) {
    [dict setObject:val forKey:keystr];
  }
}

void csDefaultsConfig::SetFloat (const char *Key, float Value)
{
  NSString *keystr = [NSString stringWithCString:Key];
  NSNumber *val = [NSNumber numberWithFloat:Value];
  
  // If it's not an administrator-owned key, perform the write.
  if (!KeyExists(Key) || ![defaults objectIsForcedForKey:keystr inDomain:domain]) {
    [dict setObject:val forKey:keystr];
  }
}

void csDefaultsConfig::SetBool (const char *Key, bool Value)
{
  NSString *keystr = [NSString stringWithCString:Key];
  NSNumber *val = [NSNumber numberWithBool:Value];
  
  // If it's not an administrator-owned key, perform the write.
  if (!KeyExists(Key) || ![defaults objectIsForcedForKey:keystr inDomain:domain]) {
    [dict setObject:val forKey:keystr];
  }
}

bool csDefaultsConfig::SetComment (const char *Key, const char *Text)
{
  return false;
}

void csDefaultsConfig::DeleteKey (const char *Key)
{
  NSString *keystr = [NSString stringWithCString:Key];  

  [dict removeObjectForKey:keystr];
}

const char *csDefaultsConfig::GetEOFComment () const
{
  return 0;
}

void csDefaultsConfig::SetEOFComment (const char *Text)
{
}


SCF_IMPLEMENT_IBASE (csDefaultsIterator)
  SCF_IMPLEMENTS_INTERFACE (iConfigIterator)
SCF_IMPLEMENT_IBASE_END

csDefaultsIterator::csDefaultsIterator (csDefaultsConfig* Owner, 
  const char* Subsection)
{
  SCF_CONSTRUCT_IBASE (0);


  // Retain our calling parameters.
  owner = Owner;
  name = [[NSString stringWithCString:Subsection] retain];

  // Construct our 'section name'.
  if (Subsection!=0) {
	domain = [[NSString stringWithFormat:@"%@.%@", owner->domain, name] retain];
  } else {
    domain = [owner->domain retain];
  }
  
  // create our inner csDefaultConfig for the subdomain.
  config = new csDefaultsConfig();
  config->Open([domain cString]);
  
  // Nil out the rest.
  keyenum=nil;
  currentkey=nil;
}

csDefaultsIterator::~csDefaultsIterator()
{
  delete config;
  // Free Your Mind.
  [domain release];
  [name release];
  if (keyenum!=nil) [keyenum release];

  owner = 0;  
}

iConfigFile* csDefaultsIterator::GetConfigFile () const
{
  return owner;
}

const char *csDefaultsIterator::GetSubsection () const
{
  return [name cString];
}

void csDefaultsIterator::Rewind ()
{
  if (keyenum!=nil) [keyenum release];
  keyenum=nil;
  currentkey=nil;
}

// navigate though the reg key to the next value entry
bool csDefaultsIterator::Next()
{
  if (keyenum==nil) {
  	// Create the iterator if we haven't got one.
  	keyenum = [[config->dict keyEnumerator] retain];
  }

  currentkey = [keyenum nextObject];
  return (currentkey!=nil);
}

const char *csDefaultsIterator::GetKey (bool Local) const
{
  return [currentkey cString];
}

int csDefaultsIterator::GetInt () const
{
  return config->GetInt([currentkey cString], 0);
}

float csDefaultsIterator::GetFloat () const
{
  return config->GetFloat([currentkey cString], 0.0f);
}

const char *csDefaultsIterator::GetStr () const
{
  return config->GetStr([currentkey cString], 0);
}

bool csDefaultsIterator::GetBool () const
{
  return config->GetBool([currentkey cString], false);
}

const char *csDefaultsIterator::GetComment () const
{
  return 0;
}
