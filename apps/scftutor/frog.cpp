/*
    This program demonstrates an SCF object with an embedded interface
    made using multiple inheritance. Also it demonstrates that there
    are no special tricks for using a SCF class either with static linking
    (linked into executable) or with dynamic linking. For this class is
    always statically linked into the executable and then is registered
    at run time using scfRegisterClass()
*/

#include "sysdef.h"
#include "ifrog.h"
#include "iname.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

class csFrog : public iFrog, public iName
{
  char *Name;
public:
  DECLARE_IBASE;

  csFrog (iBase *iParent);
  virtual ~csFrog ();
  virtual void Jump ();
  virtual void Croak (char *iWhat);
  virtual char *GetName ();
  virtual void SetName (char *iName);
};

//--------------- implementation ----------------

IMPLEMENT_IBASE (csFrog)
  IMPLEMENTS_INTERFACE (iFrog)
  IMPLEMENTS_INTERFACE (iName)
IMPLEMENT_IBASE_END

csFrog::csFrog (iBase *iParent)
{
  CONSTRUCT_IBASE (iParent);
  Name = NULL;
}

csFrog::~csFrog ()
{
  printf ("Frog %s dies ...\n", Name);
  if (Name)
    free (Name);
}

void csFrog::Jump ()
{
  printf ("Frog %s jumps\n", Name);
}

void csFrog::Croak (char *iWhat)
{
  printf ("%s: Croak, Croa-a-a-ak, %s!\n", Name, iWhat);
}

char *csFrog::GetName ()
{
  return Name;
}

void csFrog::SetName (char *iName)
{
  if (Name)
    free (Name);
  Name = strdup (iName);
}

// ... and now export all classes

IMPLEMENT_FACTORY (csFrog)

EXPORT_CLASS_TABLE (Frog)
  EXPORT_CLASS (csFrog, "test.frog", "A Frog that croaks")
EXPORT_CLASS_TABLE_END
