/*
  Copyright (C) 2005 by Christopher Nelson

  Originally developed using code from libfvg:
  Copyright � 2002 USC/Information Sciences Institute

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

#ifndef __AWS_FVG_H__
#define __AWS_FVG_H__

#include "csutil/xmltiny.h"
#include "csutil/scfstr.h"
#include "fvg_shape.h"

#include "csutil/custom_new_disable.h"
#include <map>
#include "csutil/custom_new_enable.h"

/**\file 
 * Flexible Vector Graphics parser
 */

namespace aws
{  
  namespace fvg
  {

    /** An object is made up of one or more shapes. */
    class object
    {
      csDirtyAccessArray< shape *> shapes;

    public:
      /// Adds a shape to the draw stack.
      void AddShape (shape *s)
      {
        shapes.Push (s);
      }

      /// Draws out the fvg object to a pen.
      void Draw (iPen *pen)
      {
        for(size_t i=0; i < shapes.Length (); ++i)
        {
          shapes[i]->Draw (pen);
        }
      }
    };


    /** 
     * Containes a map of parsed shapes.  Allows you to add more shapes to the 
     * system, and to find and draw any given shape. 
     */
    class fvg_parser
    {
    protected:    
      /** The type for maps of objects. */
      typedef std::map<csString, object *> shape_map_type;

      /** The map of objects. */
      shape_map_type fvg_shapes;

    protected:
      /**
       * Turns an RGBA value into a color.  If the A is omitted, it is 
       * automatically full. 
       */
      csColor4 ParseColor (const char *val);

      /**
       * Parses a floating point value out of some text, returns the number of 
       * characters used. 
       */
      uint ParseFloat (const char *base, float &rv);

      /** 
       * Fills a shape attribute structure from the attributes values in the node. 
       */
      shape_attr* FillAttribute (csRef<iDocumentNode> &pos, 
                                  shape_attr *parent_attr);

      /// Parses a shape node.
      void ParseNode (object *vo, csRef<iDocumentNodeIterator> &pos, shape_attr *attr);

      /** 
       * Parses a path node, generating drawing instructions into the given 
       * vector object. 
       */
      void ParsePath (object *vo, csString &path, shape_attr *attr);

    public:
      fvg_parser();
      virtual ~fvg_parser();

      /** 
       * Parses the given text into a vector representation of the fvg 
       * information. 
       */
      virtual bool Parse (const scfString &txt /*, autom::scope *sc*/);

      /// Draws a shape.
      virtual bool Draw (const csString &name, iPen *pen);

      /// Finds a shape. 
      virtual object *Find (const csString &name);
    };

  } // end fvg namespace
} // end aws namespace

#endif
