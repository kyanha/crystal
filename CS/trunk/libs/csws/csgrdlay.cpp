/////////////////////////////////////////////////////////////////////////////
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows license
//
// Ported to Crystal Space by Norman Kr�mer <norman@users.sourceforge.net>
/////////////////////////////////////////////////////////////////////////////

#include "cssysdef.h"
#include "csws/csgrdlay.h"


csGridLayout::csGridLayout (csComponent *pParent)
  : csLayout (pParent), mRows (1), mCols (1), mHgap (0), mVgap (0)
{}

csGridLayout::csGridLayout (csComponent *pParent, int rows, int cols)
  : csLayout (pParent), mRows (rows), mCols (cols), mHgap (0), mVgap (0)
{}

csGridLayout::csGridLayout (csComponent *pParent, int rows, int cols, int hgap, int vgap)
  : csLayout (pParent), mRows (rows), mCols (cols), mHgap (hgap), mVgap (vgap)
{}

// impl. of LayoutManager interface

void csGridLayout::SuggestSize (int &sugw, int &sugh)
{
  int i=0, cnt=vConstraints.Length ();
  sugw = sugh = 0;

  if (!cnt) return;

  for (i=0; i<cnt; i++)
  {
    int w=0, h=0;
    vConstraints.Get (i)->comp->SuggestSize (w, h);

    if ( w  > sugw  ) sugw  = w;
    if ( h  > sugh  ) sugh  = w;
  }

  int nRows = cnt / mCols + ( ( cnt % mCols ) ? 1 : 0 );
  int nCols = ( nRows != 0 ) ? mCols : cnt;

  if (nRows > mRows) nRows = mRows;

  int hgaps = ( nCols > 1 ) ? ( nCols - 1 ) * mHgap : 0;
  int vgaps = ( nRows > 1 ) ? ( nRows - 1 ) * mVgap : 0;

  sugw = sugw * nCols  + hgaps + insets.left + insets.right;
  sugh =sugh* nRows  + vgaps + insets.top  + insets.bottom;
}

void csGridLayout::LayoutContainer ()
{
  int i=0, cnt=vConstraints.Length ();
  if (!cnt) return;

  int nRows = cnt / mCols + ( ( cnt % mCols ) ? 1 : 0 );
  int nCols = ( nRows != 0 ) ? mCols : cnt;

  if ( nRows > mRows ) nRows = mRows;

  int hgaps = ( nCols > 1 ) ? ( nCols - 1 ) * mHgap : 0;
  int vgaps = ( nRows > 1 ) ? ( nRows - 1 ) * mVgap : 0;

  // actual layouting

  int x = 0, y = 0;
  int dimWidth=0, dimHeight=0;
  dimWidth = bound.Width ();
  dimHeight = bound.Height ();

  x += insets.left;
  y += insets.top;

  if (dimWidth < 0) { dimWidth = 0;  hgaps = 0; }
  if (dimHeight < 0) { dimHeight = 0; vgaps = 0; }

  dimWidth  -= insets.left + insets.right;
  dimHeight -= insets.top  + insets.bottom;

  int colWidth  = ( dimWidth  - hgaps ) / nCols;
  int rowHeight = ( dimHeight - vgaps ) / nRows;

  i = 0;

  for (int row=0; row != nRows; ++row, y += (rowHeight + ((row == 0) ? 0 : mVgap)))
    for (int col = 0; col != nCols; ++col)
    {
      if ( i < cnt )
      {
	vConstraints.Get (i)->comp->SetPos (x + col*colWidth + col * mHgap, y);
	vConstraints.Get (i)->comp->SetSize (colWidth, rowHeight );
	++i;
      }
    }
}
