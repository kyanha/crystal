/*
    Copyright (C) 2000 by W.C.A. Wijngaards
    Copyright (C) 2000 by Andrew Zabolotny

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
#include "cstool/initapp.h"
#include "csutil/csendian.h"
#include "csutil/csstring.h"
#include "csutil/csunicode.h"
#include "csutil/getopt.h"
#include "csutil/memfile.h"
#include "csutil/util.h"
#include "iutil/comp.h"
#include "iutil/eventh.h"
#include "iutil/objreg.h"
#include "iutil/plugin.h"
#include "iutil/vfs.h"
#include "ivideo/fontserv.h"

CS_IMPLEMENT_APPLICATION

static iObjectRegistry* object_reg;

char *programversion = "0.2.0";
char *programname;

static struct option long_options[] =
{
  {"first", required_argument, 0, 'f'},
  {"glyphs", required_argument, 0, 'g'},
  {"size", required_argument, 0, 's'},
  {"output", required_argument, 0, 'o'},
  {"text", no_argument, 0, 't'},
  {"display", no_argument, 0, 'd'},
  {"help", no_argument, 0, 'h'},
  {"version", no_argument, 0, 'V'},
  {"verbose", no_argument, 0, 'v'},
  {"noalpha", no_argument, 0, 'a'},
  {0, no_argument, 0, 0}
};

static struct
{
  bool verbose;
  bool sourcecode;
  bool display;
  int fontsize;
  utf32_char first;
  int glyphs;
  char *output;
  bool do_alpha;
} opt =
{
  false,
  false,
  false,
  -1,
  32,
  1114079,
  0,
  true
};

static utf32_char lastglyph;

static int display_help ()
{
  printf ("Crystal Space font conversion/generation utility v%s\n", programversion);
  printf ("Copyright (C) 2000 by W.C.A. Wijngaards and Andrew Zabolotny\n\n");
  printf ("Usage: %s {option/s} [truetype font file] [...]\n\n", programname);
  printf ("This program allows to convert TTF font files to bitmap format CSF\n");
  printf ("which is faster to load although it is non-scalable. By default the\n");
  printf ("program will convert all the fonts given on command line to CSF.\n\n");
  printf ("  -d   --display     Display font rather than converting it\n");
  printf ("  -f#  --first=#     Start conversion at glyph # (default = 32)\n");
  printf ("  -g#  --glyphs=#    Convert just # (default = 1114079) glyphs of the font\n");
  printf ("  -s#  --size=#      Set font size # in points\n");
  printf ("  -o#  --output=#    Output CSF font to file #\n");
  printf ("  -t   --text        Generate text output (C++ code) rather than binary\n");
  printf ("  -h   --help        Display this help text\n");
  printf ("  -v   --verbose     Comment on what's happening\n");
  printf ("  -V   --version     Display program version\n");
  printf ("       --noalpha     Disable alphamap output, make unantialiased font\n");
  return 1;
}

static bool Display (iFontServer *fs, iFont *font)
{
  int l, i;
  utf32_char c;
  for (c = opt.first; c < lastglyph; c++)
  {
    csBitmapMetrics metrics;
    csRef<iDataBuffer> bitmap = font->GetGlyphBitmap (c, metrics);
    if (!bitmap)
      continue;

    CS_ALLOC_STACK_ARRAY(char, lineText, metrics.width);
    printf ("---- Character: %u\n", c);
    for (l = 0; l < metrics.height; l++)
    {
      uint8* line = bitmap->GetUint8 () + l * ((metrics.width + 7) / 8);
      for (i = 0; i < metrics.width; i++)
	lineText[i] = (line [i / 8] & (0x80 >> (i & 7))) ? '@' : '.';
      printf ("%s\n", lineText);
    }
  }
  return true;
}

class FontConverter
{
  csRef<iFont> font;
  FILE* out;
  const char* fontname;

  csMemFile fCharRanges;
  csMemFile fGlyphAdvances;
  csMemFile fBitmapMetrics;
  csMemFile fAlphaMetrics;
  csMemFile fBitmapData;
  csMemFile fAlphaData;

  csString buf;

  int numGlyphs;
  utf32_char firstConsecChar;
  int numConsecChars;
public:
  FontConverter (iFont* font, FILE* out, const char* fontname)
  {
    FontConverter::font = font;
    FontConverter::out = out;
    FontConverter::fontname = fontname;

    numGlyphs = 0;
    firstConsecChar = 0;
    numConsecChars = 0;
  }

  void WriteHeaders ()
  {
    int maxheight, maxwidth;
    font->GetMaxSize (maxwidth, maxheight);

    if (opt.sourcecode)
    {
      /// make a text version
      fprintf (out, "// %s.%d %dpx height font\n", fontname, opt.fontsize, maxheight);
      fprintf (out, "// %d ascent, %d descent\n", font->GetAscent (), font->GetDescent ());
      fprintf (out, "\n");

      buf.Format ("CharRange ranges_%s%d [] =\n{\n", fontname, opt.fontsize);
      fCharRanges.Write (buf.GetData (), buf.Length ());

      buf.Format ("int advances_%s%d [] =\n{\n", fontname, opt.fontsize);
      fGlyphAdvances.Write (buf.GetData (), buf.Length ());

      buf.Format ("iFont::BitmapMetrics metrics_%s%d [] =\n{\n", fontname, opt.fontsize);
      fBitmapMetrics.Write (buf.GetData (), buf.Length ());

      buf.Format ("uint8 font_%s%d [] =\n{\n", fontname, opt.fontsize);
      fBitmapData.Write (buf.GetData (), buf.Length ());

      if (opt.do_alpha)
      {
	buf.Format ("iFont::BitmapMetrics alphaMetrics_%s%d [] =\n{\n", fontname, opt.fontsize);
	fAlphaMetrics.Write (buf.GetData (), buf.Length ());

	buf.Format ("uint8 alpha_%s%d [] =\n{\n", fontname, opt.fontsize);
	fAlphaData.Write (buf.GetData (), buf.Length ());
      }
    }
    else 
    {
      fprintf (out, "CSF [Font=%s.%d Height=%d Ascent=%d Descent=%d "
	"HasCharRanges=1 HasBitmapMetrics=1 HasGlyphAdvance=1",
	fontname, opt.fontsize, maxheight, font->GetAscent (), font->GetDescent ());
      fprintf (out, " Alpha=%d", opt.do_alpha);
      fprintf (out, "]\n");
    }
  }

  void WriteGlyph (utf32_char c)
  {
    int i;
    bool hasGlyph = font->HasGlyph (c);

    if (!hasGlyph || (c != firstConsecChar + numConsecChars))
    {
      if (numConsecChars > 0)
      {
	if (opt.sourcecode)
	{
	  buf.Format ("{0x%06x, %d},\n", (unsigned int)firstConsecChar, numConsecChars);
	  fCharRanges.Write (buf.GetData(), buf.Length ());
	}
	else
	{
	  uint32 ui;
	  ui = convert_endian ((uint32)firstConsecChar);
	  fCharRanges.Write ((char*)&ui, sizeof (ui));
	  ui = convert_endian ((uint32)numConsecChars);
	  fCharRanges.Write ((char*)&ui, sizeof (ui));
	}
      }
      firstConsecChar = c;
      numConsecChars = 0;
    }

    if (hasGlyph)
    {
      if (opt.verbose)
      {
	if (numGlyphs == 0) printf ("character ");
	printf ("%d%s", c, (c < lastglyph - 1) ? "," : "\n");
      }

      csGlyphMetrics gMetrics;
      font->GetGlyphMetrics (c, gMetrics);

      if (opt.sourcecode)
      {
	buf.Format ("%d,\t// %8x\n", gMetrics.advance, (unsigned int)c);
	fGlyphAdvances.Write (buf.GetData (), buf.Length ());
      }
      else
      {
	uint32 ui;
	ui = convert_endian ((uint32)gMetrics.advance);
	fGlyphAdvances.Write ((char*)&ui, sizeof (ui));
      }

      // get bitmap
      csBitmapMetrics metrics;
      csRef<iDataBuffer> bitmap = font->GetGlyphBitmap (c, metrics);

      if (bitmap)
      {
	int bpc = ((metrics.width + 7) / 8) * metrics.height;
	if (opt.sourcecode)
	{

	  fBitmapData.Write ("  ", 2);
	  for (i = 0; i < bpc; i++)
	  {
	    buf.Format ("0x%02x,", bitmap->GetUint8 () [i]);
	    fBitmapData.Write (buf.GetData (), buf.Length ());
	  }
	  buf.Format ("\t// %8x\n", (unsigned int)c);
	  fBitmapData.Write (buf.GetData (), buf.Length ());
	}
	else
	{
	  fBitmapData.Write (bitmap->GetData (), bitmap->GetSize ());
	}
      }
      else
      {
	memset (&metrics, 0, sizeof (metrics));
      }
      if (opt.sourcecode)
      {
	buf.Format ("{%d, %d, %d, %d},\t// %8x\n", metrics.width, metrics.height,
	  metrics.left, metrics.top, (unsigned int)c);
	fBitmapMetrics.Write (buf.GetData(), buf.Length ());
      }
      else
      {
	int32 i;
	i = convert_endian ((int32)metrics.width);
	fBitmapMetrics.Write ((char*)&i, sizeof (i));
	i = convert_endian ((int32)metrics.height);
	fBitmapMetrics.Write ((char*)&i, sizeof (i));
	i = convert_endian ((int32)metrics.left);
	fBitmapMetrics.Write ((char*)&i, sizeof (i));
	i = convert_endian ((int32)metrics.top);
	fBitmapMetrics.Write ((char*)&i, sizeof (i));
      }

     
      csRef<iDataBuffer> alphaBitmap = font->GetGlyphAlphaBitmap (c, metrics);

      if (alphaBitmap)
      {
	int bpc = metrics.width * metrics.height;
	if (opt.sourcecode)
	{

	  fAlphaData.Write ("  ", 2);
	  for (i = 0; i < bpc; i++)
	  {
	    buf.Format ("0x%02x,", alphaBitmap->GetUint8 () [i]);
	    fAlphaData.Write (buf.GetData (), buf.Length ());
	  }
	  buf.Format ("\t// %8x\n", (unsigned int)c);
	  fAlphaData.Write (buf.GetData (), buf.Length ());
	}
	else
	{
	  fAlphaData.Write (alphaBitmap->GetData (), alphaBitmap->GetSize ());
	}
      }
      else
      {
	memset (&metrics, 0, sizeof (metrics));
      }
      if (opt.sourcecode)
      {
	buf.Format ("{%d, %d, %d, %d},\t// %8x\n", metrics.width, metrics.height,
	  metrics.left, metrics.top, (unsigned int)c);
	fAlphaMetrics.Write (buf.GetData(), buf.Length ());
      }
      else
      {
	uint32 i;
	i = convert_endian ((uint32)metrics.width);
	fAlphaMetrics.Write ((char*)&i, sizeof (i));
	i = convert_endian ((uint32)metrics.height);
	fAlphaMetrics.Write ((char*)&i, sizeof (i));
	i = convert_endian ((uint32)metrics.left);
	fAlphaMetrics.Write ((char*)&i, sizeof (i));
	i = convert_endian ((uint32)metrics.top);
	fAlphaMetrics.Write ((char*)&i, sizeof (i));
      }
      numConsecChars++;
      numGlyphs++;
    }
  }

  void EndWriting ()
  {
    if (numConsecChars != 0)
    {
      if (opt.sourcecode)
      {
	buf.Format ("{0x%06x, %d},\n", (unsigned int)firstConsecChar, numConsecChars);
	fCharRanges.Write (buf.GetData(), buf.Length ());
      }
      else
      {
	uint32 ui;
	ui = convert_endian ((uint32)firstConsecChar);
	fCharRanges.Write ((char*)&ui, sizeof (ui));
	ui = convert_endian ((uint32)numConsecChars);
	fCharRanges.Write ((char*)&ui, sizeof (ui));
      }
    }
    if (opt.sourcecode)
    {
      buf = "{0, 0}\n}\n\n";
      fCharRanges.Write (buf.GetData(), buf.Length ());

      fGlyphAdvances.Write ("}\n\n", 3);
      fBitmapMetrics.Write ("}\n\n", 3);
      fAlphaMetrics.Write ("}\n\n", 3);
      fBitmapData.Write ("}\n\n", 3);
      fAlphaData.Write ("}\n\n", 3);
    }
    else
    {
      uint32 ui;
      ui = 0;
      fCharRanges.Write ((char*)&ui, sizeof (ui));
      fCharRanges.Write ((char*)&ui, sizeof (ui));
    }

    fwrite (fCharRanges.GetData (), 1, fCharRanges.GetSize (),
      out);
    fwrite (fBitmapMetrics.GetData (), 1, fBitmapMetrics.GetSize (),
      out);
    fwrite (fAlphaMetrics.GetData (), 1, fAlphaMetrics.GetSize (),
      out);
    fwrite (fGlyphAdvances.GetData (), 1, fGlyphAdvances.GetSize (),
      out);
    fwrite (fBitmapData.GetData (), 1, fBitmapData.GetSize (),
      out);
    fwrite (fAlphaData.GetData (), 1, fAlphaData.GetSize (),
      out);
  }
};

static bool Convert (const char *fontfile)
{
  if (opt.verbose)
    printf ("Loading font %s, size = %d\n", fontfile, opt.fontsize);

  csRef<iFontServer> fs = CS_QUERY_REGISTRY (object_reg, iFontServer);
  if (!fs)
  {
    printf ("Font server plugin has not been loaded.\n");
    return false;
  }

  csRef<iFont> font = fs->LoadFont (fontfile, 
    (opt.fontsize > 0) ? opt.fontsize : 10);
  if (font == 0)
  {
    printf ("Cannot load font file %s\n", fontfile);
    return false;
  }

  {
    int oldsize = opt.fontsize;
    opt.fontsize = font->GetSize ();
    if (opt.fontsize != oldsize)
      printf ("Could not set font size %d, using size %d\n",
        oldsize, opt.fontsize);
  }

  // max height of font
  int maxheight, maxwidth;
  font->GetMaxSize (maxwidth, maxheight);

  if (maxwidth > 255)
  {
    fprintf (stderr, "Font too large (%dx%d): CSF format supports only widths < 256\n", maxwidth, maxheight);
    return false;
  }

  if (opt.display)
    return Display (fs, font);

  char fontname [CS_MAXPATHLEN + 1];
  char outfile [CS_MAXPATHLEN + 1];
  csSplitPath (fontfile, 0, 0, fontname, sizeof (fontname));
  if (fontname [0] == '*')
    strcpy (fontname, fontname + 1);
  char *dot = strchr (fontname, '.');
  if (dot) *dot = 0;
  sprintf (outfile, "%s%d.%s", fontname, opt.fontsize, opt.sourcecode ? "h" : "csf");

  FILE *out = fopen (outfile, opt.sourcecode ? "w" : "wb");
  if (!out)
  {
    printf ("Could not open output file %s\n", outfile);
    return false;
  }

  FontConverter converter (font, out, fontname);

  converter.WriteHeaders ();

  utf32_char c;

  // Output every character in turn
  for (c = opt.first; c <= lastglyph; c++)
  {
    converter.WriteGlyph (c);
  }

  // ensure default glyph is written
  if ((CS_FONT_DEFAULT_GLYPH < opt.first) || 
    (CS_FONT_DEFAULT_GLYPH > lastglyph))
  {
    converter.WriteGlyph (CS_FONT_DEFAULT_GLYPH);
  }

  converter.EndWriting ();

  fclose (out);
  return true;
}

int main (int argc, char* argv[])
{
  object_reg = csInitializer::CreateEnvironment (argc, argv);
  if (!object_reg) return -1;

  if (!csInitializer::RequestPlugins (object_reg,
  	CS_REQUEST_VFS,
	CS_REQUEST_PLUGIN ("crystalspace.font.server.freetype2", iFontServer),
	CS_REQUEST_END))
  {
    fprintf (stderr, "couldn't init app! (perhaps some plugins are missing?)");
    return -1;
  }

  programname = argv [0];

  int c;
  while ((c = getopt_long (argc, argv, "f:g:s:o:tdhvV", long_options, 0)) != EOF)
    switch (c)
    {
      case '?':
        // unknown option
	printf ("\n");
	display_help ();
        return -1;
      case 'f':
        opt.first = atoi (optarg);
        if ((opt.first < 0)
         || (opt.first > 1114111))
        {
          fprintf (stderr, "ERROR: first glyph should be 0..1114111\n");
          return -2;
        }
        break;
      case 'g':
        opt.glyphs = atoi (optarg);
        if ((opt.glyphs < 1)
         || (opt.glyphs > 1114112))
        {
          fprintf (stderr, "ERROR: glyph count should be 1..1114112\n");
          return -2;
        }
        break;
      case 's':
        opt.fontsize = atoi (optarg);
        if ((opt.fontsize < 1)
         || (opt.fontsize > 100))
        {
          fprintf (stderr, "ERROR: font size should be 1..100\n");
          return -2;
        }
        break;
      case 'o':
        opt.output = optarg;
        break;
      case 't':
        opt.sourcecode = true;
        break;
      case 'd':
        opt.display = true;
        break;
      case 'h':
        return display_help ();
      case 'v':
        opt.verbose = true;
        break;
      case 'V':
        printf ("%s version %s\n\n", programname, programversion);
        printf ("This program is distributed in the hope that it will be useful,\n");
        printf ("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
        printf ("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n");
        printf ("GNU Library General Public License for more details.\n");
        return 0;
      case 'a':
	opt.do_alpha = false;
	break;
    } /* endswitch */

  if (optind >= argc)
    return display_help ();

  lastglyph = opt.first + opt.glyphs;
  if (lastglyph > 1114111)
  {
    fprintf (stderr, "WARNING: Last glyph = %d, limiting to 1114111\n", lastglyph);
    lastglyph = 1114111;
    opt.glyphs = 1114111 - opt.first;
  }

  // Interpret the non-option arguments as file names
  for (; optind < argc; ++optind)
    if (!Convert (argv [optind]))
      return -2;

  csInitializer::DestroyApplication (object_reg);

  return 0;
}
