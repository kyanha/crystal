// awsTabCtrl.cpp: implementation of the awsTabCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "cssysdef.h"
#include "awsTabCtrl.h"
#include "csutil/scfstr.h"
#include "awskcfct.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/fontserv.h"

/************************ awsTab ************************************/

const int awsTab::signalActivateTab   = 1;
const int awsTab::signalDeactivateTab = 2;


const int awsTab::iconLeft = 0;
const int awsTab::iconRight = 1;
const int awsTab::iconTop = 2;
const int awsTab::iconBottom = 3;

awsTab::awsTab ():
  is_active(false),
  is_first(false),
  is_top(true),
  caption (NULL),
  captured(false),
  icon_align(0),
  alpha_level(92),
  user_param(NULL)
{
  tex[0]=tex[1]=tex[2]=NULL;
}

awsTab::~awsTab ()
{
}

bool awsTab::Setup (iAws *_wmgr, awsComponentNode *settings)
{
  if (!awsComponent::Setup (_wmgr, settings)) return false;

  iAwsPrefManager *pm = WindowManager ()->GetPrefMgr ();

  pm->LookupIntKey ("OverlayTextureAlpha", alpha_level);  // global get
  pm->GetInt (settings, "Alpha", alpha_level);            // local overrides, if present.
  pm->GetInt (settings, "IconAlign", icon_align);
  pm->GetString (settings, "Caption", caption);

  iString *tn = NULL;

  tex[0] = pm->GetTexture ("Texture");
  pm->GetString (settings, "Image", tn);

  if (tn) tex[1] = pm->GetTexture (tn->GetData (), tn->GetData ());

  iString *in = NULL;
  pm->GetString (settings, "Icon", in);
  if (in) tex[2] = pm->GetTexture (in->GetData (), in->GetData ());

  return true;
}

void awsTab::OnDraw (csRect)
{
  int tw=0, th=0, tx, ty, itx=0, ity=0;

  iGraphics2D *g2d = WindowManager ()->G2D ();
  iGraphics3D *g3d = WindowManager ()->G3D ();

  iAwsPrefManager *pm = WindowManager ()->GetPrefMgr ();
  int hi = pm->GetColor (AC_HIGHLIGHT);
  int hi2 = pm->GetColor (AC_HIGHLIGHT2);
  int lo = pm->GetColor (AC_SHADOW);
  int lo2 = pm->GetColor (AC_SHADOW2);
  int fill = pm->GetColor (AC_FILL);
  int dfill = pm->GetColor (AC_DARKFILL);

  csRect r = Frame ();

  if (is_active)
  {
    g2d->DrawLine (r.xmin + 1, is_top ? r.ymin : r.ymax, r.xmax-1, is_top ? r.ymin : r.ymax, hi);
    g2d->DrawLine (r.xmin, r.ymin + 1, r.xmin, r.ymax, hi);
    g2d->DrawLine (r.xmax-1, r.ymin + 1, r.xmax-1, r.ymax, lo);
    g2d->DrawLine (r.xmax, r.ymin + 1, r.xmax, r.ymax, lo2);
  }
  else
  {
    g2d->DrawLine (r.xmin, r.ymin + 1, r.xmin, r.ymax, is_first ? hi2 : lo);
    g2d->DrawLine (r.xmin + 1, is_top ? r.ymin : r.ymax, r.xmax, is_top ? r.ymin : r.ymax, hi2);
    g2d->DrawLine (r.xmax, r.ymin + 1, r.xmax, r.ymax, lo);
  }

  g2d->DrawBox (r.xmin + 1, r.ymin + 1, r.Width () - 1, r.Height () - 1, is_active ? fill : dfill);

  if (tex[0])
    g3d->DrawPixmap (tex[0], r.xmin+1, r.ymin+1, r.Width ()-1, r.Height ()-1,
                     r.xmin+1, r.ymin+1, r.Width ()-1, r.Height ()-1, alpha_level);

  if (tex[1])
  {
    int img_w, img_h;

    tex[1]->GetOriginalDimensions (img_w, img_h);

    g3d->DrawPixmap (tex[1], r.xmin+1, r.ymin+1, r.Width ()-1, r.Height ()-1,
                     0, 0, img_w, img_h, 0);
  }

  tx = r.Width () >> 1;
  ty = r.Height () >> 1;

  if (caption)
    pm->GetDefaultFont ()->GetDimensions (caption->GetData (), tw, th);

  if (tex[2])
  {
    int img_w, img_h;
    itx = tx, ity = ty;

    tex[2]->GetOriginalDimensions (img_w, img_h);

    itx -= (img_w>>1);
    ity -= (img_h>>1);

    switch (icon_align)
    {
    case iconLeft:
      itx = tx - ((tw+img_w)>>1) - 1;
      ity = ty - (img_h>>1);
      tx = itx + img_w + 2;
      ty = ty - (th>>1);
      break;
    case iconRight:
      itx = tx + ((tw-img_w)>>1) + 1;
      ity = ty - (img_h>>1);
      tx = tx - ((tw+img_w)>>1) - 1;
      ty = ty - (th>>1);
      break;
    case iconTop:
      itx = tx - (img_w>>1);
      ity = ty - ((th+img_h)>>1) - 1;
      tx = tx - (tw>>1);
      ty = ity + img_h + 2;
      break;
    case iconBottom:
      itx = tx - (img_w>>1);
      ity = ty + ((th-img_h)>>1) + 1;
      tx = tx - (tw>>1);
      ty = ty - ((th+img_h)>>1) - 1;
      break;
    }
        
    g3d->DrawPixmap (tex[2], r.xmin + itx, r.ymin + ity, img_w, img_h,
                     0, 0, img_w, img_h, 0);
  }
  else
  {
    tx -= (tw>>1);
    ty -= (th>>1);
  }

  // Draw the caption, if there is one and the style permits it.
  if (caption)
  {

    // Draw the text
    g2d->Write (pm->GetDefaultFont (), r.xmin + tx, r.ymin + ty,
                pm->GetColor (AC_TEXTFORE), -1, caption->GetData ());

  }

}

bool awsTab::GetProperty (char *name, void **parm)
{
  if (awsComponent::GetProperty (name, parm)) return true;

  if (strcmp ("Caption", name) == 0)
  {
    char *st = NULL;

    if (caption) st = caption->GetData ();

    iString *s = new scfString (st);
    *parm = (void *)s;
    return true;
  }
  else if(strcmp("User Param", name) == 0)
  {
	*parm = user_param;
    return true;
  }
  return false;
}

bool awsTab::SetProperty (char *name, void *parm)
{
  if (awsComponent::SetProperty (name, parm)) return true;
  
  if (strcmp ("Caption", name) == 0)
  {
    iString *s = (iString *) (parm);

    SCF_DEC_REF (caption);

    if (s && s->Length ())
      (caption = s)->IncRef ();
    else
      caption = NULL;
    Invalidate ();

    return true;
  }
  else if(strcmp ("User Param", name) == 0)
  {
	user_param = parm;
	return true; 
  }
  return false;
}

bool awsTab::OnMouseDown (int, int, int)
{
  if (!is_active)
  {
    captured = true;
    WindowManager ()->CaptureMouse (this);
    return true;
  }
  return false;
}

bool awsTab::OnMouseUp (int, int x, int y)
{
  return HandleClick (x, y);
}

bool awsTab::OnMouseClick (int, int x, int y)
{
  return HandleClick (x, y);
}

bool awsTab::OnMouseDoubleClick (int, int x, int y)
{
  return HandleClick (x, y);
}

bool awsTab::HandleClick (int x, int y)
{
  if (captured)
  {
    WindowManager ()->ReleaseMouse ();
    captured = false;
    if (!is_active && Frame().Contains (x,y))
    {
      SetActive(true);
    }
    return true;
  }
  return false;
}

csRect awsTab::getPreferredSize ()
{
  return getMinimumSize ();
}

csRect awsTab::getMinimumSize ()
{
  int tw = 0, th = 0;

  if (caption)
  {
    // Get the size of the text
    WindowManager ()->GetPrefMgr ()->GetDefaultFont ()->GetDimensions (
        caption->GetData (), tw, th);
  }

  if (tex[2])
  {
    int img_w = 0, img_h = 0;
    tex[2]->GetOriginalDimensions (img_w, img_h);
    
    if (icon_align == iconLeft || icon_align == iconRight)
    {
      tw += img_w + 2;
      th = MAX(th, img_h);
    }
    else
    {
      th += img_h + 2;
      tw = MAX(tw, img_w);
    }
  }

  return csRect (0, 0, tw + 4, th + 4);
}

void awsTab::SetActive(bool what)
{
	if(what == is_active) return;
	is_active = what;
	Invalidate();
	if(what)
		Broadcast(signalActivateTab);
	else
		Broadcast(signalDeactivateTab);
}


/************************ awsSimpleTabCtrl ***************************/

const int awsTabCtrl::HandleSize = 15;


awsTabCtrl::awsTabCtrl ():
  first(-1),
  active(-1),
  is_top(true),
  sink(NULL),
  nextimg(NULL),
  previmg(NULL)
{
}

awsTabCtrl::~awsTabCtrl ()
{
  if(sink) delete sink;
}

bool awsTabCtrl::Setup (iAws *_wmgr, awsComponentNode *settings)
{
  if (!awsComponent::Setup (_wmgr, settings)) return false;

  sink = new awsSink (this);
  sink->RegisterTrigger ("ActivateTab", &ActivateTabCallback);

  awsKeyFactory previnfo, nextinfo;

  previnfo.Initialize (new scfString ("prev"), new scfString ("Slider Button"));
  nextinfo.Initialize (new scfString ("next"), new scfString ("Slider Button"));

  previnfo.AddIntKey (new scfString ("Style"), awsCmdButton::fsNormal);
  nextinfo.AddIntKey (new scfString ("Style"), awsCmdButton::fsNormal);

  previnfo.AddStringKey(new scfString("Icon"), new scfString("ScrollBarLt"));
  nextinfo.AddStringKey(new scfString("Icon"), new scfString("ScrollBarRt"));

  nextimg = WindowManager ()->GetPrefMgr ()->GetTexture ("ScrollBarRt");
  previmg = WindowManager ()->GetPrefMgr ()->GetTexture ("ScrollBarLt");

  if (!previmg || !nextimg) return false;

  int img_w, img_h;
  
  previmg->GetOriginalDimensions (img_w, img_h);

  csRect r(0, 0, 30, 30 ); //( HandleSize > img_w ? HandleSize : img_w), ( HandleSize > img_h ? HandleSize : img_h) + 15);

  r.Move (Frame ().Width () - 2*HandleSize-1, Frame ().Height ()-HandleSize - 1);
  previnfo.AddRectKey (new scfString ("Frame"), r);

  r.Move (HandleSize+1, 0);
  nextinfo.AddRectKey (new scfString ("Frame"), r);

  prev.SetParent (this);
  next.SetParent (this);

  prev.Setup (_wmgr, previnfo.GetThisNode ());
  next.Setup (_wmgr, nextinfo.GetThisNode ());

  //prev.SetProperty ("Image", previmg);
  //next.SetProperty ("Image", nextimg);

  sink->RegisterTrigger ("Prev", &PrevClicked);
  sink->RegisterTrigger ("Next", &NextClicked);


  slot_prev.Connect (&prev, awsCmdButton::signalClicked, sink, 
                      sink->GetTriggerID ("Prev"));
  slot_next.Connect (&next, awsCmdButton::signalClicked, sink,
                      sink->GetTriggerID ("Next"));

  prev.Hide ();
  next.Hide ();

  prev.SetFlag(AWSF_CMP_NON_CLIENT);
  next.SetFlag(AWSF_CMP_NON_CLIENT);

  AddChild (&prev);
  AddChild (&next);


  return true;
}

void awsTabCtrl::SetTopBottom (bool to_top)
{
  if (is_top != to_top)
  {
    is_top = to_top;
    DoLayout ();
  }
}

void awsTabCtrl::DoLayout ()
{
  int i, x=0;
  csRect r = Frame ();

  for (i=first-1; i >=0 ; i--)
  {
    awsTab *btn = vTabs.Get (i);
    csRect br =  btn->Frame ();
    btn->Hide ();
    r.xmax = r.xmin - 1;
    r.xmin = r.xmax - br.Width ();
    btn->ResizeTo(r);
    btn->SetTop (is_top);
  }

  r = Frame ();

  for (i=MAX(first,0); i < vTabs.Length (); i++)
  {
    awsTab *btn = vTabs.Get (i);
    csRect br =  btn->Frame ();
    r.xmax = r.xmin + br.Width ();
    btn->ResizeTo(r);
	x += r.Width ();
    r.xmin = r.xmax+1;
    btn->SetTop (is_top);
  }

  if (Frame().Width() < x)
  {
	clip_to_scroll = true;
	r = Frame();
    r.xmin = r.xmax - 2*HandleSize-1;
    r.ymax--;
    //r.ymin = r.ymax - (HandleSize<<1);
    r.xmax = r.xmin + HandleSize;
    prev.ResizeTo(r);
    prev.Show ();
    r.Move (HandleSize+1,0);
    next.ResizeTo(r);
    next.Show ();
  }
  else
  {
	clip_to_scroll = false;
    next.Hide();
    prev.Hide();
  }
}


csRect awsTabCtrl::getInsets()
{
	if(clip_to_scroll)
		return csRect(0,0,2*HandleSize+1,0);
	else
		return csRect(0,0,0,0);
}

iAwsSource* awsTabCtrl::AddTab (iString* caption, void* user_param)
{
  if (!caption || !caption->GetData ())
  {
    csString theCap ("Tab ");
    theCap += vTabs.Length ()+1;
    caption = new scfString ((const char*)theCap);
  }

  // create a button
  awsTab *btn = new awsTab;

  // initialize and setup the button
  awsKeyFactory btninfo;

  caption->IncRef ();
  btninfo.Initialize (caption, new scfString ("Tab"));
  btninfo.AddRectKey (new scfString ("Frame"), csRect (0, 0, Frame ().Width (), Frame ().Height ()));

  btn->SetParent (this);
  btn->Setup (WindowManager (), btninfo.GetThisNode ());
  btn->SetProperty ("Caption", caption);
  btn->SetProperty ("User Param", user_param);

  // resize button
  csRect r(btn->getPreferredSize ());
  int last = vTabs.Length ();
  if (r.Height () > Frame ().Height())
  {
    int delta = r.Height () - Frame ().Height ();
    Resize(Frame().Width(), Frame().Height() + delta);
  }

  btn->ResizeTo(r);

  if (last==0)
  {
    first = 0;
    active = 0;
    ActivateTab(btn);
    btn->SetFirst (true);
  }

  AddChild (btn);

  // connect myself with button to keep informed about state changes
  slot_activate.Connect (btn, awsTab::signalActivateTab, sink, sink->GetTriggerID ("ActivateTab"));
  vTabs.Push (btn);
  DoLayout ();

  btn->Invalidate ();
  caption->DecRef ();
  return (iAwsSource*)btn;
}

void awsTabCtrl::RemoveTab(void* user_param)
{
  int idx = FindTab(user_param);
  if(idx >= 0) RemoveTab(idx);
}

void awsTabCtrl::RemoveTab (iAwsSource *src)
{
  int idx = vTabs.Find ((csSome)src->GetComponent());
  if(idx >= 0) RemoveTab(idx);
}

void awsTabCtrl::RemoveTab(int index)
{
  if (index != -1)
  {
    if (index == active)
      if (vTabs.Length () -1 == active)
        ActivateTab (active-1);
      else
        ActivateTab (active+1);

    vTabs.Get (first)->SetFirst (false);
    if ((index < first) || (index == first && (first > 0 || vTabs.Length () < 2)))
      first--;

    if (first > -1)
      vTabs.Get (first)->SetFirst (true);

    if (index < active)
      active--;

	slot_activate.Disconnect ((iAwsSource*)vTabs.Get(index), awsTab::signalActivateTab, 
                           sink, sink->GetTriggerID ("ActivateTab"));
    vTabs.Delete (index);

	DoLayout();
  }
}

void awsTabCtrl::OnDraw (csRect)
{
  iGraphics2D *g2d = WindowManager ()->G2D ();
  iAwsPrefManager *pm = WindowManager ()->GetPrefMgr ();

  int dark = pm->GetColor (AC_SHADOW);
  
  csRect r = Frame();

  int y = (is_top ? r.ymax : r.ymin);

  if (active != -1)
  {
    csRect b = vTabs.Get (active)->Frame();
    if (b.xmin >= r.xmax || b.xmax <= r.xmin)
      g2d->DrawLine (r.xmin, y, r.xmax, y, dark);
    else
    {
      if (b.xmax < r.xmax && b.xmin > r.xmin)
      {
        g2d->DrawLine (r.xmin, y, b.xmin-1, y, dark);
        g2d->DrawLine (b.xmax+1, y, r.xmax, y, dark);
      }
      else
        if (b.xmax > r.xmin && b.xmax < r.xmax)
          g2d->DrawLine (b.xmax+1, y, r.xmax, y, dark);
        else if (b.xmin > r.xmin && b.xmin < r.xmax)
          g2d->DrawLine (r.xmin, y, b.xmin-1, y, dark);
    }    
  }
  else
    g2d->DrawLine (r.xmin, y, r.xmax, y, dark);

}

void awsTabCtrl::ScrollLeft ()
{
  if (vTabs.Length () && first != vTabs.Length ()-1)
  {
    int xdelta = vTabs.Get (first)->Frame ().Width ()+1;
    vTabs.Get (first)->SetFirst (false);
    for (int i=0; i < vTabs.Length (); i++)
      vTabs.Get (i)->Move (-xdelta,0);
    
    first++;
    vTabs.Get (first)->SetFirst (true);
  }
  Invalidate();
}

void awsTabCtrl::ScrollRight ()
{
  if (vTabs.Length () && first != 0)
  {
    int xdelta = vTabs.Get (first-1)->Frame ().Width ()+1;
    vTabs.Get (first)->SetFirst (false);
    for (int i=0; i < vTabs.Length (); i++)
      vTabs.Get (i)->Move (xdelta,0);
    
    first--;
    vTabs.Get (first)->SetFirst (true);
  }
  Invalidate();
}

void awsTabCtrl::MakeVisible (int idx)
{
  // make sure the <idx>-th button is visible
  csRect r = vTabs.Get (idx)->Frame ();
  csRect cr = Frame();
  if (first != idx && r.xmax > cr.xmax)
  {
    // scroll buttons left until the <idx>-th button becomes visible
    while (first != idx && r.xmax > cr.xmax)
      ScrollLeft ();
  }
  else
  if (first != idx && r.xmin < cr.xmin)
  {
    // scroll buttons right until the <idx>-th button becomes visible
    while (first != idx && r.xmin < cr.xmin)
      ScrollRight ();
  }
}

int awsTabCtrl::FindTab(void* user_param)
{
  for(int i = 0; i < vTabs.Length(); i++)
  {
	 void* p;
	 vTabs.Get(i)->GetProperty("User Param", &p);
	 if( p == user_param )
	   return i;
  }
  return -1;
}

void awsTabCtrl::ActivateTab(void* param)
{
  int idx = FindTab(param);
  if(idx >= 0)
	ActivateTab(idx);
}

void awsTabCtrl::ActivateTab(iAwsSource *src)
{
  int idx = vTabs.Find((csSome)src->GetComponent());
  if(idx >= 0)
	ActivateTab(idx);
}

void awsTabCtrl::ActivateTab (int idx)
{
  // we need to set active before calling SetActive because SetActive will
  // fire a signal back to the ActivateTabCallback which will recurse back to 
  // here if the signal doesn't come from the component marked as being active
  int old_active = active;
  active = idx;
  vTabs.Get (idx)->SetActive(true);
  if(old_active >= 0)
	vTabs.Get (old_active)->SetActive (false);
}

csRect awsTabCtrl::getPreferredSize()
{
   int width = 0;
   for(int i = 0; i < vTabs.Length(); i++)
   {
	   width += vTabs.Get(i)->getPreferredSize().Width();
   }

   csRect r = Frame();
   r.xmax = r.xmin + width;
   return r;
}

void awsTabCtrl::OnResized()
{
  DoLayout();
}

void awsTabCtrl::ActivateTabCallback (void *p, iAwsSource *source)
{
  awsTabCtrl *tc = (awsTabCtrl *)p;
  int idx = tc->vTabs.Find((csSome)source->GetComponent());
  if (idx != -1 && tc->active != idx)
  {
    // hide the active and make the new one active
    tc->ActivateTab (idx);
    tc->MakeVisible (idx);
  }
}

void awsTabCtrl::PrevClicked (void *p, iAwsSource *)
{
  awsTabCtrl *tc = (awsTabCtrl *)p;
  tc->ScrollRight ();
}

void awsTabCtrl::NextClicked (void *p, iAwsSource *)
{
  awsTabCtrl *tc = (awsTabCtrl *)p;
  tc->ScrollLeft ();
}
