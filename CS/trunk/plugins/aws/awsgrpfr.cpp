#include "cssysdef.h"
#include "awsgrpfr.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/fontserv.h"
#include "csutil/scfstr.h"
#include "csutil/csevent.h"
#include "iutil/evdefs.h"
#include "aws3dfrm.h"

#include <stdio.h>

SCF_IMPLEMENT_IBASE(awsGroupFrame)
  SCF_IMPLEMENTS_INTERFACE(awsComponent)
SCF_IMPLEMENT_IBASE_END

const int awsGroupFrame::fsBump =0x0;
const int awsGroupFrame::fsSimple =0x1;
const int awsGroupFrame::fsRaised =0x2;
const int awsGroupFrame::fsSunken =0x3;

const int awsGroupFrame::signalClicked=0x1;

awsGroupFrame::awsGroupFrame():frame_style(0), alpha_level(96), bkg(NULL), caption(NULL) 
{}

awsGroupFrame::~awsGroupFrame()
{}

char *
awsGroupFrame::Type() 
{ return "Group Frame"; }

bool
awsGroupFrame::Setup(iAws *_wmgr, awsComponentNode *settings)
{
 if (!awsComponent::Setup(_wmgr, settings)) return false;

 iAwsPrefManager *pm=WindowManager()->GetPrefMgr();
 
 pm->LookupIntKey("OverlayTextureAlpha", alpha_level);
 pm->GetInt(settings, "Style", frame_style);
 pm->GetString(settings, "Caption", caption);

 bkg=pm->GetTexture("Texture");
  
 return true;
}

bool 
awsGroupFrame::GetProperty(char *name, void **parm)
{
  if (awsComponent::GetProperty(name, parm)) return true;

  if (strcmp("Caption", name)==0)
  {
    char *st = NULL;

    if (caption) st=caption->GetData();

    iString *s = new scfString(st);
    *parm = (void *)s;
    return true;
  }

  return false;
}

bool 
awsGroupFrame::SetProperty(char *name, void *parm)
{
  if (awsComponent::SetProperty(name, parm)) return true;

  if (strcmp("Caption", name)==0)
  {
    iString *s = (iString *)(parm);
    
    if (s)
    {
      if (caption) caption->DecRef();
      caption=s;
      caption->IncRef();
      Invalidate();
    }
    
    return true;
  }
  
  return false;
}

void 
awsGroupFrame::OnDraw(csRect clip)
{

  iGraphics2D *g2d = WindowManager()->G2D();
    
  aws3DFrame frame3d;

  frame3d.Draw(WindowManager(), Window(), Frame(), frame_style, bkg, alpha_level);
  
  // Draw the caption, if there is one 
  if (caption)
  {     
    int tw, th, tx, ty;
    
    // Get the size of the text
    WindowManager()->GetPrefMgr()->GetDefaultFont()->GetDimensions(caption->GetData(), tw, th);

    // Calculate the center
    tx = 10; //(Frame().Width()>>1) -  (tw>>1);
    ty = 8;   //(Frame().Height()>>1) - (th>>1);

    // Draw the text
    g2d->Write(WindowManager()->GetPrefMgr()->GetDefaultFont(),
               Frame().xmin+tx,
               Frame().ymin+ty,
               WindowManager()->GetPrefMgr()->GetColor(AC_TEXTFORE),
               -1,
               caption->GetData());
    
  }
}

bool 
awsGroupFrame::OnMouseDown(int button, int x, int y)
{
  return false;
}
    
bool 
awsGroupFrame::OnMouseUp(int button, int x, int y)
{  
  return false;
}
    
bool
awsGroupFrame::OnMouseMove(int button, int x, int y)
{
  return false;
}

bool
awsGroupFrame::OnMouseClick(int button, int x, int y)
{
  return false;
}

bool
awsGroupFrame::OnMouseDoubleClick(int button, int x, int y)
{
  return false;
}

bool 
awsGroupFrame::OnMouseExit()
{
  return false;
}

bool
awsGroupFrame::OnMouseEnter()
{
  return false;
}

bool
awsGroupFrame::OnKeypress(int key, int modifiers)
{
  return false;
}
    
bool
awsGroupFrame::OnLostFocus()
{
  return false;
}

bool 
awsGroupFrame::OnGainFocus()
{
  return false;
}

/************************************* Command Button Factory ****************/
SCF_IMPLEMENT_IBASE(awsGroupFrameFactory)
  SCF_IMPLEMENTS_INTERFACE(iAwsComponentFactory)
SCF_IMPLEMENT_IBASE_END

awsGroupFrameFactory::awsGroupFrameFactory(iAws *wmgr):awsComponentFactory(wmgr)
{
  Register("Group Frame");
  RegisterConstant("gfsBump",  awsGroupFrame::fsBump);
  RegisterConstant("gfsSimple", awsGroupFrame::fsSimple);
  RegisterConstant("gfsSunken",  awsGroupFrame::fsSunken);
  RegisterConstant("gfsRaised", awsGroupFrame::fsRaised);

  RegisterConstant("signalGroupFrameClicked",  awsGroupFrame::signalClicked);
}

awsGroupFrameFactory::~awsGroupFrameFactory()
{
 // empty
}

iAwsComponent *
awsGroupFrameFactory::Create()
{
 return new awsGroupFrame; 
}

