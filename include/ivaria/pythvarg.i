/*
    Copyright (C) 2003 Rene Jager <renej_frog@users.sourceforge.net>

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

/*
	Python specific stuff for SWIG interface in post-include phase
    dealing with variable argument lists. You know, the ... stuff.

	See include/ivaria/cspace.i
*/

#ifdef SWIGPYTHON

%extend iGraphics2D
{
	PyObject * _PerformExtension (const char * clazz, const char * command, PyObject * args)
    {
        if (!command || !strlen(command)) {
            // error
        }
        else if (!strcasecmp(command, "flush")) {
            // csGraphics2DGLCommon
            bool ok = self->PerformExtension(command);
            return PyInt_FromLong(long(ok));
        } else if (!strcasecmp(command, "getstatecache")) {
            // csGraphics2DGLCommon
            // not implemented
        } else if (!strcasecmp(command, "getextmanager")) {
            // csGraphics2DGLCommon
            // not implemented
        }
        else if (!strcasecmp(command, "fullscreen")) {
            // csGraphics2DDDraw8, csGraphics2DGLX
            int val = PyInt_AsLong(PyTuple_GetItem(args, 0));
            bool ok = self->PerformExtension(command, val);
            return PyInt_FromLong(long(ok));
        }
        else if (!strcasecmp(command, "hardware_accelerated")) {
            // csGraphics2DOpenGL, csGraphics2DGLX
            bool yes = false;
            bool ok = self->PerformExtension(command, &yes);
            PyObject * res = PyTuple_New(2);
            PyTuple_SetItem(res, 0, PyInt_FromLong(long(ok)));
            PyTuple_SetItem(res, 1, PyInt_FromLong(long(yes)));
            return res;
        } else if (!strcasecmp(command, "configureopengl")) {
            // csGraphics2DOpenGL
            bool ok = self->PerformExtension(command);
            return PyInt_FromLong(long(ok));
        }
        return PyInt_FromLong(0);
    }

	%pythoncode %{
		def PerformExtension (self, command, *args):
            self._PerformExtension(self.__class__.__name__, command, args);
	%}
}

#endif // SWIGPYTHON


