// awsControlBar.cpp: implementation of the awsControlBar class.
//
//////////////////////////////////////////////////////////////////////
#include "cssysdef.h"
#include "awsControlBar.h"
#include "awsfparm.h"
#include "aws3dfrm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int awsControlBar::alignTop = 1;
const int awsControlBar::alignBottom = 2;
const int awsControlBar::alignCenter = 3;


awsControlBar::awsControlBar() :
  hGap(0),
  vGap(0),
  vert_align(alignTop),
  size_to_fit_horz(true),
  size_to_fit_vert(true),  
  vertical(false),
  stretch_items(false)
{
}

awsControlBar::~awsControlBar()
{
}

void awsControlBar::AddChild(iAwsComponent* comp)
{
  awsPanel::AddChild(comp);
  comps.Push(comp);
  DoLayout();
}

void awsControlBar::RemoveChild(iAwsComponent* comp)
{
  comps.Delete(comp);
  awsPanel::RemoveChild(comp);
  DoLayout();
}

bool awsControlBar::Execute(const char* action, iAwsParmList* parmlist)
{
  if (!parmlist)
    return false;
  if(strcmp(action, "AddComponent") == 0)
  {
    iAwsComponent* child;
    if(parmlist->GetInt("Component", (int*)&child) && child)
    {
      AddChild(child);
      return true;
    }
  }
  else if(strcmp(action, "RemoveComponent") == 0)
  {
    iAwsComponent* child;
    if(parmlist->GetInt("Component", (int*)&child) && child)
    {
      RemoveChild(child);
      return true;
    }
  }
  return false;
}

bool awsControlBar::Setup(iAws *_wmgr, iAwsComponentNode* settings)
{
	frame_drawer.SetBackgroundAlpha(96);

  if(!awsPanel::Setup(_wmgr, settings)) return false;

  iAwsPrefManager* pm = WindowManager()->GetPrefMgr();

  pm->GetInt(settings, "HorizontalGap", hGap);
  pm->GetInt(settings, "VerticalGap", vGap);
  pm->GetInt(settings, "Alignment", vert_align);
  int v = vertical;
  pm->GetInt(settings, "Vertical", v);
  vertical = v;

  //Show();

  return true;
}

void awsControlBar::DoLayout()
{
  if(vertical)
  {
    // pass once through all the components to find the max width
    int max_width = 0;
    int i;
    for(i = 0; i < comps.Length(); i++)
    {
      if(comps.Get(i)->isHidden()) continue;
      if(comps.Get(i)->Flags() & AWSF_CMP_INVISIBLE) continue;
      csRect r = comps.Get(i)->Frame();
      max_width = MAX(max_width, r.Width());
    }
    
    // do we size ourselves?
    if(size_to_fit_horz)
    {
      csRect r = getInsets();

      Resize(max_width + 2*hGap + r.xmin + r.xmax, Frame().Height());
    }
    
    // pass again to lay them all out
    int y = ClientFrame().ymin + vGap;
    for(i = 0; i < comps.Length(); i++)
    {
      if(comps.Get(i)->isHidden()) continue;
      if(comps.Get(i)->Flags() & AWSF_CMP_INVISIBLE) continue;
      csRect r = comps.Get(i)->Frame();
      if(stretch_items)
      {
        comps.Get(i)->MoveTo(ClientFrame().xmin + hGap, y);
        comps.Get(i)->Resize(max_width, r.Height());
      }
      else
      {
        if(vert_align == alignTop)
          comps.Get(i)->MoveTo(ClientFrame().xmin + hGap, y);
        else if(vert_align == alignBottom)
          comps.Get(i)->MoveTo(ClientFrame().xmax - hGap - r.Width(), y);
        else if(vert_align == alignCenter)
          comps.Get(i)->MoveTo(ClientFrame().xmin + ClientFrame().Width()/2 - r.Width()/2, y);
      }
      y += r.Height();
      y += vGap;
    }
    
    // do we size ourselves?
    if(size_to_fit_vert)
    {
      Resize(Frame().Width(), y - Frame().ymin);
    }
  }
  else //if(!vertical)
  {
    // pass once through all the components to find the max height
    int max_height = 0;
    int i;
    for(i = 0; i < comps.Length(); i++)
    {
      if(comps.Get(i)->isHidden()) continue;
      if(comps.Get(i)->Flags() & AWSF_CMP_INVISIBLE) continue;
      csRect r = comps.Get(i)->Frame();
      max_height = MAX(max_height, r.Height());
    }
    
    // do we size ourselves?
    if(size_to_fit_vert)
    {
      csRect r = getInsets();
      Resize(Frame().Width(), max_height + 2*vGap + r.ymin + r.ymax);
    }
    
    // pass again to lay them all out
    int x = ClientFrame().xmin + hGap;
    for(i = 0; i < comps.Length(); i++)
    {
      if(comps.Get(i)->isHidden()) continue;
      if(comps.Get(i)->Flags() & AWSF_CMP_INVISIBLE) continue;
      csRect r = comps.Get(i)->Frame();
      if(stretch_items)
      {
        comps.Get(i)->MoveTo(x, ClientFrame().ymin + vGap);
        comps.Get(i)->Resize(r.Width(), max_height);
      }
      else
      {
      if(vert_align == alignTop)
        comps.Get(i)->MoveTo(x, ClientFrame().ymin + vGap);
      else if(vert_align == alignBottom)
        comps.Get(i)->MoveTo(x, ClientFrame().ymax - vGap - r.Height());
      else if(vert_align == alignCenter)
        comps.Get(i)->MoveTo(x, ClientFrame().ymin + ClientFrame().Height()/2 - r.Height()/2);
      }
      x += r.Width();
      x += hGap;
    }
    
    // do we size ourselves?
    if(size_to_fit_horz)
    {
      Resize(x - Frame().xmin, Frame().Height());
    }
  }

  Invalidate();
}

bool awsControlBar::GetSizeToFitHorz() { return size_to_fit_horz; }
bool awsControlBar::GetSizeToFitVert() { return size_to_fit_vert; }
void awsControlBar::SetSizeToFitHorz(bool b) { size_to_fit_horz = b; DoLayout(); }
void awsControlBar::SetSizeToFitVert(bool b) { size_to_fit_vert = b; DoLayout(); }

void awsControlBar::SizeToFit()
{
  SizeToFitHorz();
  SizeToFitVert();
}

void awsControlBar::SizeToFitHorz()
{
  bool temp = size_to_fit_horz;
  size_to_fit_horz = true;
  DoLayout();
  size_to_fit_horz = temp;
}

void awsControlBar::SizeToFitVert()
{
  bool temp = size_to_fit_vert;
  size_to_fit_vert = true;
  DoLayout();
  size_to_fit_vert = temp;
}

bool awsControlBar::GetVertical() { return vertical; }
void awsControlBar::SetVertical(bool b) { vertical = b; DoLayout(); }

int awsControlBar::GetHorzGap() { return hGap; }
void awsControlBar::SetHorzGap(int gap) { hGap = gap; DoLayout(); }
int awsControlBar::GetVertGap() { return vGap; }
void awsControlBar::SetVertGap(int gap) { vGap = gap; DoLayout(); }
 
int awsControlBar::GetAlignment() { return vert_align; }
void awsControlBar::SetAlignment(int align) { vert_align = align; DoLayout(); }

bool awsControlBar::GetStretchComponents() { return stretch_items; }
void awsControlBar::SetStretchComponents(bool stretch) { stretch_items = stretch; DoLayout(); }


/*------------------------awsControlBarFactory-------------------------*/

awsControlBarFactory::awsControlBarFactory(iAws *_wmgr) :
awsComponentFactory(_wmgr)
{
  Register("Control Bar");
  RegisterConstant("gcbAlignTop", awsControlBar::alignTop);
  RegisterConstant("gcbAlignBottom", awsControlBar::alignBottom);
  RegisterConstant("gcbAlignCenter", awsControlBar::alignCenter);
}

iAwsComponent* awsControlBarFactory::Create()
{
  return (iAwsComponent*) new awsControlBar;
}

