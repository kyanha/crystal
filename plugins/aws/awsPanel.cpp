// awsPanel.cpp: implementation of the awsPanel class.
//
//////////////////////////////////////////////////////////////////////
#include "cssysdef.h"
#include "awsPanel.h"
#include "aws3dfrm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int awsPanel:: fsBump = _3dfsBump;
const int awsPanel:: fsSimple = _3dfsSimple;
const int awsPanel:: fsRaised = _3dfsRaised;
const int awsPanel:: fsSunken = _3dfsSunken;
const int awsPanel:: fsFlat = _3dfsFlat;
const int awsPanel:: fsNone = _3dfsNone;
const int awsPanel:: fsBevel = _3dfsBevel;
const int awsPanel:: fsThick = _3dfsThick;
const int awsPanel:: fsBitmap = _3dfsBitmap;
const int awsPanel:: fsMask  = _3dfsMask;

// Windows like this one
const int awsPanel::fsNormal = awsPanel::fsThick;

// Command Buttons like this one
const int awsPanel::fsToolbar = awsPanel::fsFlat;


awsPanel::awsPanel() :
  style(fsFlat),
  todraw_dirty(false),
  bkg(NULL),
  ovl(NULL),
  bkg_alpha(128),
  ovl_alpha(0)
{ }

awsPanel::~awsPanel()
{
}

bool awsPanel::Setup(iAws *_wmgr, awsComponentNode *settings)
{
	if(!awsComponent::Setup(_wmgr, settings)) return false;


	iAwsPrefManager* pm = WindowManager()->GetPrefMgr();
	pm->GetInt(settings, "Style", style);
	pm->LookupIntKey("OverlayTextureAlpha", bkg_alpha);
	pm->GetInt(settings, "Alpha", bkg_alpha);
	
	bkg = pm->GetTexture ("Texture");


    iString *tn1 = NULL, *tn2 = NULL;

    pm->GetString (settings, "BitmapBackground", tn1);
    pm->GetString (settings, "BitmapOverlay", tn2);

    if (tn1) bkg = pm->GetTexture (tn1->GetData (), tn1->GetData ());
    if (tn2) ovl = pm->GetTexture (tn2->GetData (), tn2->GetData ());

    pm->GetInt(settings, "BackgroundAlpha", bkg_alpha);
    pm->GetInt(settings, "OverlayAlpha", ovl_alpha);

    bm_bkgsub.Set (0, 0, 0, 0);
    if (!pm->GetRect (settings, "BackgroundSubrect", bm_bkgsub))
      if (bkg)
        bkg->GetOriginalDimensions (bm_bkgsub.xmax, bm_bkgsub.ymax);

    bm_ovlsub.Set (0, 0, 0, 0);
    if (!pm->GetRect (settings, "OverlaySubrect", bm_ovlsub))
      if (ovl)
        ovl->GetOriginalDimensions (bm_ovlsub.xmax, bm_ovlsub.ymax);


  // these properties get stored with the frame drawer
  // for convenience
  frame_drawer.Setup(WindowManager(), bkg, bkg_alpha, ovl, ovl_alpha);

	return true;
}

void awsPanel::OnDraw(csRect )
{
	// if the child exclude region is dirty, refresh it
	if(todraw_dirty)
	{
		todraw.makeEmpty();
		todraw.Include(ClientFrame());
		for(iAwsComponent* cmp = GetTopChild(); cmp; cmp = cmp->ComponentBelow())
		{
			if(cmp->Flags() & AWSF_CMP_ALWAYSERASE) continue;
      if(cmp->isHidden()) continue;
			todraw.Exclude(cmp->Frame());
		}
		todraw_dirty = false;
	}


  csRect bkg_align = Window()->Frame();
  csRect ovl_align = Window()->Frame();
  // we may need to calculate the alignment rect of the whole texture
  // if we are only using the subrects
  if(style == fsBitmap)
  {
	bkg_align.xmin = Frame().xmin - bm_bkgsub.xmin;
	bkg_align.ymin = Frame().ymin - bm_bkgsub.ymin;
	ovl_align.xmin = Frame().xmin - bm_ovlsub.xmin;
	ovl_align.ymin = Frame().ymin - bm_ovlsub.ymin;
  }

	frame_drawer.Draw(Frame(), style, bkg_align, ovl_align, &todraw);
}

void awsPanel::AddChild(iAwsComponent* cmp)
{
	todraw_dirty = true;
	awsComponent::AddChild(cmp);
}

void awsPanel::RemoveChild(iAwsComponent* cmp)
{
	todraw_dirty = true;
	awsComponent::RemoveChild(cmp);
}

void awsPanel::Move(int delta_x, int delta_y)
{
	todraw_dirty = true;
	awsComponent::Move(delta_x, delta_y);
}

csRect awsPanel::getInsets()
{
	return frame_drawer.GetInsets(style);
}

void awsPanel::OnChildMoved()
{
	todraw_dirty = true;
}

void awsPanel::OnResized()
{
	todraw_dirty = true;
}

void awsPanel::OnChildShow()
{
	todraw_dirty = true;
}

void awsPanel::OnChildHide()
{
	todraw_dirty = true;
}
