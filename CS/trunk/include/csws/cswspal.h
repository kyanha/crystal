/*
    Crystal Space Windowing System: CSWS palette definition
    Copyright (C) 1998,1999 by Andrew Zabolotny <bit@eltech.ru>

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

#ifndef __CSWSPAL_H__
#define __CSWSPAL_H__

class csApp;

/**
 * Control palette export structure
 */
struct csPaletteExport
{
  int *Palette;
  int Size;
};

/**
 * This structure is used to change color scheme.
 * The Color, Contrast and Blend values gives control over
 * the overall palette look, besides the "BaseTone", which
 * defines the "base" color for scheme. The "normal"
 * values for Color,Contrast,Blend are 100, which means
 * the respective parameters will get same value like
 * in the "default" palette.
 */
struct csColorScheme
{
  /// Base tone for scheme (e.g. red, cyan, gray etc)
  int BaseTone;
  /// (H) The difference (-100..0..+100) between the base tone and other colors
  int8 Color;
  /// (L) Contrast (-100..0..+100) betweem base tone and bright/dark variants
  int8 Contrast;
  /// (S) The attraction factor (-100..0..+100) towards base tone
  int8 Blend;
};

/// A global array containing CSWS palette for all component types
extern csPaletteExport *cswsPalette;
/// Total number of palettes
extern int cswsPaletteSize;

/**
 * Register a new color palette (returns the palette index).
 * Registered palettes will be affected by color scheme changes;
 * unregistered won't be.
 */
extern int csRegisterPalette (int *Palette, int Size);
/// Change the global color scheme (or restore scheme if NULL passed)
extern void csSetColorScheme (csApp *iApp, csColorScheme &Scheme);

/**
 * These are indices into cswsPalette for each component type
 */
/// Application palette index
#define CSPAL_APP			0
/// Button palette index
#define CSPAL_BUTTON			1
/// Dialog palette index
#define CSPAL_DIALOG			2
/// Input line palette index
#define CSPAL_INPUTLINE			3
/// List box palette index
#define CSPAL_LISTBOX			CSPAL_INPUTLINE
/// Menu item palette index
#define CSPAL_MENUITEM			4
/// Menu palette index
#define CSPAL_MENU			5
/// Static component palette index
#define CSPAL_STATIC			6
/// Title bar palette index
#define CSPAL_TITLEBAR			7
/// Window palette index
#define CSPAL_WINDOW			8
/// List box item palette index
#define CSPAL_LISTBOXITEM		9
/// Scroll bar palette index
#define CSPAL_SCROLLBAR			10
/// Notebook palette index
#define CSPAL_NOTEBOOK			11
/// GridCell palette index
#define CSPAL_GRIDCELL			12
/// GridView palette index
#define CSPAL_GRIDVIEW  		13
/// Slider palette index
#define CSPAL_SPLITTER		        14
/// Tree control item palette index
#define CSPAL_TREEITEM		        15
/// Tree control palette index
#define CSPAL_TREECTRL		        16
/// Hine component palette index
#define CSPAL_HINT			17

/**
 * csApp class color palette indices.<p>
 */
/// Workspace color index
#define CSPAL_APP_WORKSPACE	0

/**
 * csButton class color palette indices.<p>
 */
/// button background
#define CSPAL_BUTTON_BACKGROUND		0
/// default button frame
#define CSPAL_BUTTON_DEFFRAME		1
/// 3D border dark
#define CSPAL_BUTTON_DARK3D		2
/// 3D border light
#define CSPAL_BUTTON_LIGHT3D		3
/// button text
#define CSPAL_BUTTON_TEXT		4
/// button disabled text
#define CSPAL_BUTTON_DTEXT		5

/**
 * csDialog class color palette indices.<p>
 */
/// Background color index
#define CSPAL_DIALOG_BACKGROUND		0
/// 3D border dark
#define CSPAL_DIALOG_DARK3D		1
/// 3D border light
#define CSPAL_DIALOG_LIGHT3D		2
/// 2nd level 3D border dark
#define CSPAL_DIALOG_2DARK3D		3
/// 2nd level 3D border light
#define CSPAL_DIALOG_2LIGHT3D		4

/**
 * csButton class color palette indices.<p>
 */
/// input line background
#define CSPAL_INPUTLINE_BACKGROUND	0
/// Background color for csifsThickRect
#define CSPAL_INPUTLINE_BACKGROUND2	1
/// 3D border dark
#define CSPAL_INPUTLINE_DARK3D		2
/// 3D border light
#define CSPAL_INPUTLINE_LIGHT3D		3
/// 2nd level 3D border dark
#define CSPAL_INPUTLINE_2DARK3D		4
/// 2nd level 3D border light
#define CSPAL_INPUTLINE_2LIGHT3D	5
/// input line text
#define CSPAL_INPUTLINE_TEXT		6
/// input line selection background
#define CSPAL_INPUTLINE_SELBACKGROUND	7
/// input line selected text
#define CSPAL_INPUTLINE_SELTEXT		8

/**
 * csListBox class color palette indices.<p>
 */
/// list box background
#define CSPAL_LISTBOX_BACKGROUND	CSPAL_INPUTLINE_BACKGROUND
/// background color for cslfsThickRect
#define CSPAL_LISTBOX_BACKGROUND2	CSPAL_INPUTLINE_BACKGROUND2
/// 3D border dark
#define CSPAL_LISTBOX_DARK3D		CSPAL_INPUTLINE_DARK3D
/// 3D border light
#define CSPAL_LISTBOX_LIGHT3D		CSPAL_INPUTLINE_LIGHT3D
/// 2nd level 3D border dark
#define CSPAL_LISTBOX_2DARK3D		CSPAL_INPUTLINE_2DARK3D
/// 2nd level 3D border light
#define CSPAL_LISTBOX_2LIGHT3D		CSPAL_INPUTLINE_2LIGHT3D
/// list box text
#define CSPAL_LISTBOX_TEXT		CSPAL_INPUTLINE_TEXT
/// list box selection background
#define CSPAL_LISTBOX_SELBACKGROUND	CSPAL_INPUTLINE_SELBACKGROUND
/// list box selected text
#define CSPAL_LISTBOX_SELTEXT		CSPAL_INPUTLINE_SELTEXT

/**
 * csTreeCtrl class color palette indices.<p>
 */
/// tree control background
#define CSPAL_TREECTRL_BACKGROUND	0
/// background color for cstfsThickRect
#define CSPAL_TREECTRL_BACKGROUND2	1
/// 3D border dark
#define CSPAL_TREECTRL_DARK3D		2
/// 3D border light
#define CSPAL_TREECTRL_LIGHT3D		3
/// 2nd level 3D border dark
#define CSPAL_TREECTRL_2DARK3D		4
/// 2nd level 3D border light
#define CSPAL_TREECTRL_2LIGHT3D		5
/// tree control text
#define CSPAL_TREECTRL_TEXT		6
/// tree control selection background
#define CSPAL_TREECTRL_SELBACKGROUND	7
/// tree control selected text
#define CSPAL_TREECTRL_SELTEXT		8
/// tree control selected text
#define CSPAL_TREECTRL_BRANCH		9
/// tree control branchknob color
#define CSPAL_TREECTRL_BRANCHKNOB	10

/**
 * csMenuItem class color palette indices.<p>
 */
/// Unselected menu item background
#define CSPAL_MENUITEM_BACKGROUND	0
/// Selected menu item background
#define CSPAL_MENUITEM_SELECTION	1
/// Unselected text
#define CSPAL_MENUITEM_UTEXT		2
/// Selected text
#define CSPAL_MENUITEM_STEXT		3
/// Disabled text
#define CSPAL_MENUITEM_DTEXT		4
/// Separator 3D dark
#define CSPAL_MENUITEM_DARK3D		5
/// Separator 3D light
#define CSPAL_MENUITEM_LIGHT3D		6

/**
 * csMenu class color palette indices.<p>
 */
/// Menu background
#define CSPAL_MENU_BACKGROUND		0
/// 3D border dark
#define CSPAL_MENU_DARK3D		1
/// 3D border light
#define CSPAL_MENU_LIGHT3D		2
/// 3D border 2nd level dark
#define CSPAL_MENU_2DARK3D		3
/// 3D border 2nd level light
#define CSPAL_MENU_2LIGHT3D		4

/**
 * csStatic class color palette indices.<p>
 */
/// static component background
#define CSPAL_STATIC_BACKGROUND		0
/// static component text / inactive
#define CSPAL_STATIC_ITEXT		1
/// static component text / active
#define CSPAL_STATIC_ATEXT		2
/// static component text / disabled
#define CSPAL_STATIC_DTEXT		3
/// 3D border dark
#define CSPAL_STATIC_DARK3D		4
/// 3D border light
#define CSPAL_STATIC_LIGHT3D		5

/**
 * csTitleBar class color palette indices.<p>
 */
/// 3D border dark
#define CSPAL_TITLEBAR_DARK3D		0
/// 3D border light
#define CSPAL_TITLEBAR_LIGHT3D		1
/// passive title background
#define CSPAL_TITLEBAR_PBACKGROUND	2
/// passive title text
#define CSPAL_TITLEBAR_PTEXT		3
/// passive title 3D bars dark
#define CSPAL_TITLEBAR_PDARK3D		4
/// passive title 3D bars light
#define CSPAL_TITLEBAR_PLIGHT3D		5
/// active title background
#define CSPAL_TITLEBAR_ABACKGROUND	6
/// active title text
#define CSPAL_TITLEBAR_ATEXT		7
/// active title 3D bars dark
#define CSPAL_TITLEBAR_ADARK3D		8
/// active title 3D bars light
#define CSPAL_TITLEBAR_ALIGHT3D		9

/**
 * csWindow class color palette indices.<p>
 */
/// 3D border dark
#define CSPAL_WINDOW_DARK3D		0
/// 3D border light
#define CSPAL_WINDOW_LIGHT3D		1
/// 3D border 2nd level dark
#define CSPAL_WINDOW_2DARK3D		2
/// 3D border 2nd level light
#define CSPAL_WINDOW_2LIGHT3D		3
/// border normal
#define CSPAL_WINDOW_BORDER		4

/**
 * csListBoxItem class color palette indices.<p>
 */
/// Unselected listbox item background
#define CSPAL_LISTBOXITEM_BACKGROUND	0
/// Selected listbox item background
#define CSPAL_LISTBOXITEM_SELECTION	1
/// Thin rectangle around selected item
#define CSPAL_LISTBOXITEM_SELRECT	2
/// Disabled text
#define CSPAL_LISTBOXITEM_DTEXT		3
/// Unselected normal text
#define CSPAL_LISTBOXITEM_UNTEXT	4
/// Selected normal text
#define CSPAL_LISTBOXITEM_SNTEXT	5
/// Unselected emphasized text
#define CSPAL_LISTBOXITEM_UETEXT	6
/// Selected emphasized text
#define CSPAL_LISTBOXITEM_SETEXT	7

/**
 * csTreeItem class color palette indices.<p>
 */
/// Unselected tree item background
#define CSPAL_TREEITEM_BACKGROUND	0
/// Selected tree item background
#define CSPAL_TREEITEM_SELECTION	1
/// Thin rectangle around selected item
#define CSPAL_TREEITEM_SELRECT		2
/// Disabled text
#define CSPAL_TREEITEM_DTEXT		3
/// Unselected normal text
#define CSPAL_TREEITEM_UNTEXT		4
/// Selected normal text
#define CSPAL_TREEITEM_SNTEXT		5
/// Unselected emphasized text
#define CSPAL_TREEITEM_UETEXT		6
/// Selected emphasized text
#define CSPAL_TREEITEM_SETEXT		7

/**
 * csScrollBar class color palette indices.<p>
 */
/// Scroll bar background
#define CSPAL_SCROLLBAR_BACKGROUND	0
/// Scroll bar selected background
#define CSPAL_SCROLLBAR_SELBACKGROUND	1
/// 3D border dark
#define CSPAL_SCROLLBAR_DARK3D		2
/// 3D border light
#define CSPAL_SCROLLBAR_LIGHT3D		3

/**
 * csNotebook class color palete indices.<p>
 */
/// Notebook background
#define CSPAL_NOTEBOOK_BACKGROUND	0
/// Unselected tab background
#define CSPAL_NOTEBOOK_TAB_UNSEL	1
/// The text on unselected tabs
#define CSPAL_NOTEBOOK_UNSEL_TEXT	2
/// The most dark color of unselected tab border
#define CSPAL_NOTEBOOK_TAB_UNSELB1	3
/// The second color of unselected tab border
#define CSPAL_NOTEBOOK_TAB_UNSELB2	4
/// The third color of unselected tab border
#define CSPAL_NOTEBOOK_TAB_UNSELB3	5
/// Selected tab background
#define CSPAL_NOTEBOOK_TAB_SEL		6
/// The text on selected tab
#define CSPAL_NOTEBOOK_SEL_TEXT		7
/// The most dark color of selected tab border
#define CSPAL_NOTEBOOK_TAB_SELB1	8
/// The second color of selected tab border
#define CSPAL_NOTEBOOK_TAB_SELB2	9
/// The third color of selected tab border
#define CSPAL_NOTEBOOK_TAB_SELB3	10
/// The information text
#define CSPAL_NOTEBOOK_INFO_TEXT	11
/// 3D border dark
#define CSPAL_NOTEBOOK_DARK3D		12
/// 3D border light
#define CSPAL_NOTEBOOK_LIGHT3D		13
/// 3D border very dark
#define CSPAL_NOTEBOOK_2DARK3D		14
/// 3D border middle light
#define CSPAL_NOTEBOOK_2LIGHT3D		15

/**
 * csGridCell class color palete indices.<p>
 */
#define CSPAL_GRIDCELL_BACKGROUND	0
#define CSPAL_GRIDCELL_BORDER_FG	1
#define CSPAL_GRIDCELL_BORDER_BG	2
#define CSPAL_GRIDCELL_SEL_BACKGROUND	3
#define CSPAL_GRIDCELL_SEL_BORDER_FG	4
#define CSPAL_GRIDCELL_SEL_BORDER_BG	5
#define CSPAL_GRIDCELL_DATA_FG	        6
#define CSPAL_GRIDCELL_DATA_BG	        7

/**
 * csGridView class color palete indices.<p>
 */
#define CSPAL_GRIDVIEW_BACKGROUND	0
/// 3D border dark
#define CSPAL_GRIDVIEW_DARK3D		1
/// 3D border light
#define CSPAL_GRIDVIEW_LIGHT3D		2

/**
 * csSplitter class color palete indices.<p>
 */
/// Splitter inactive background
#define CSPAL_SPLITTER_IBACKGROUND	0
/// Splitter inactive light 3D color
#define CSPAL_SPLITTER_ILIGHT3D		1
/// Splitter inactive dark 3D color
#define CSPAL_SPLITTER_IDARK3D		2
/// Splitter active background
#define CSPAL_SPLITTER_ABACKGROUND	3
/// Splitter active light 3D color
#define CSPAL_SPLITTER_ALIGHT3D		4
/// Splitter active dark 3D color
#define CSPAL_SPLITTER_ADARK3D		5

/**
 * csTip class color palete indices.<p>
 */
/// Background tip color
#define CSPAL_HINT_BACKGROUND		0
/// Tip text color
#define CSPAL_HINT_TEXT			1
/// Tip border
#define CSPAL_HINT_BORDER        	2

#endif // __CSWSPAL_H__
