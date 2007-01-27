/*
  Copyright (C) 2003 Rene Jager <renej_frog@users.sourceforge.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
  Python specific stuff for SWIG interface in pre-include phase.
  See include/bindings/cspace.i
*/

#ifdef SWIGPYTHON

%include "bindings/python/pyattributes.i"

%ignore ::operator+;
%ignore ::operator-;
%ignore ::operator*;
%ignore ::operator/;
%ignore ::operator%;
%ignore ::operator<;
%ignore ::operator>;
%ignore ::operator==;
%ignore ::operator!=;
%ignore ::operator*=;
%ignore ::operator/=;
%ignore ::operator>>;
%ignore ::operator<<;

%rename(assign) *::operator=;

// iutil/databuff.h
%rename(asString) iDataBuffer::operator * () const;

// ivaria/script.h
%rename(SetFloat) iScriptObject::Set (const char *, float);
%rename(GetFloat) iScriptObject::Get (const char *, float &) const;
%rename(StoreFloat) iScript::Store (const char *, float);
%rename(RetrieveFloat) iScript::Retrieve (const char *, float &) const;

// iutil/string.h
%ignore iString::operator[];

// csutil/csstring.h
%ignore csString::operator[];

%{
CS_EXPORT_SYM PyObject *
_csRef_to_Python (const csRef<iBase> & ref, void * ptr, const char * name)
{
  if (!ref.IsValid())
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
  ref->IncRef();
  return SWIG_NewPointerObj((void *)ptr, SWIG_TypeQuery(name), 1);
}
%}

/*
  ptr  : either a csRef<type> or csPtr<type>
  name : type name, e.g. "iEngine *"
  type : type of pointer

  In actual practice, 'ptr' is really of type SwigValueWrapper<csRef<T>>
  or SwigValueWrapper<csPtr<T>>.  The SwigValueWrapper wrapper is added
  automatically by Swig and is outside of our control.	SwigValueWrapper
  has a cast operator which will return csRef<T>& (or csPtr<T>&), which
  should allow us to assign a SwigValueWrapper to a csRef.
  Unfortunately, unlike other compilers, however, MSVC considers
  assignment of SwigValueWrapper to csRef ambiguous, so we must manually
  invoke SwigValueWrapper's operator T& before actually assigning the
  value to a csRef.  This hack is noted by "* explicit cast *".
*/
%define TYPEMAP_OUT_csRef_BODY(ptr, name, type, wrapper)
  csRef<type> ref((wrapper<type>&)ptr); /* explicit cast */
  $result = _csRef_to_Python(csRef<iBase>(
    (type *)ref), (void *)(type *)ref, name);
%enddef
/*
  if (ref.IsValid())
  {
    ref->IncRef();
    $result = SWIG_NewPointerObj((void*)(type*)ref, SWIG_TypeQuery(name), 1);
  }
  else
  {
    Py_INCREF(Py_None);
    $result = Py_None;
  }
*/

#undef TYPEMAP_OUT_csRef
%define TYPEMAP_OUT_csRef(T)
  %typemap(out) csRef<T>
  {
    TYPEMAP_OUT_csRef_BODY($1, #T " *", T, csRef)
  }
%enddef

#undef TYPEMAP_OUT_csPtr
%define TYPEMAP_OUT_csPtr(T)
  %typemap(out) csPtr<T>
  {
    TYPEMAP_OUT_csRef_BODY($1, #T " *", T, csPtr)
  }
%enddef

%{
PyObject *
_csWrapPtr_to_Python (const csWrapPtr & wp)
{
  if (!wp.Ref.IsValid())
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
  iBase * ibase = (iBase *)wp.Ref;
  void * ptr = ibase->QueryInterface(iSCF::SCF->GetInterfaceID(wp.Type), wp.Version);
  ibase->DecRef(); // Undo IncRef from QueryInterface

  // This is a bit tricky: We want the generated Python 'result' object
  // to own one reference to the wrapped object, so we want to call
  // Python's iBase.IncRef() to gain that reference. In order to do this,
  // we create a second Python object (an iBase) and invoke its IncRef()
  // method. This IncRef() is done on the Python side so that Python code
  // can override IncRef() if needed. Note carefully that this means we
  // are actually creating _two_ Python objects, and each Python object
  // will invoke DecRef() on the wrapped pointer at destruction time. (We
  // destroy the iBase object manually below; and the 'result' object
  // gets destroyed when it goes out of scope in the Python program.)
  // Because DecRef() is being invoked twice (once for each Python
  // object), we must ensure that IncRef() is invoked twice, hence the
  // extra C++ `ibase->IncRef()'. (We don't bother doing this on the
  // Python side because this reference count manipulation is for our own
  // internal correctness.)

  CS_ALLOC_STACK_ARRAY(char, type_name, strlen(wp.Type) + 3);
  strcat(strcpy(type_name, wp.Type), " *");
  PyObject *result = SWIG_NewPointerObj(ptr, SWIG_TypeQuery(type_name), 1);
  PyObject * ibase_obj = SWIG_NewPointerObj (
    ptr, SWIG_TypeQuery(type_name), 1);
  PyObject * res_obj = PyObject_CallMethod(ibase_obj, "IncRef", "()");
  if (!res_obj)
  {
    // Calling Python IncRef() failed; something wrong here.
    Py_XDECREF(result);
    result = 0;
  }
  Py_XDECREF(ibase_obj);
  Py_XDECREF(res_obj);
  return result;
}
%}

#undef TYPEMAP_OUT_csWrapPtr
%define TYPEMAP_OUT_csWrapPtr
  %typemap(out) csWrapPtr
  {
    $result = _csWrapPtr_to_Python($1);
  }
%enddef

%typemap(in) (const char * iface, int iface_ver) (csString className)
{
  PyObject *pyname = PyObject_GetAttrString($input, "__name__");
  className = csString(PyString_AsString(pyname));
  Py_XDECREF(pyname);
  $1 = (char*)className.GetData(); // SWIG declares $1 non-const for some reason
  PyObject *pyver = PyObject_CallMethod($input, "scfGetVersion", NULL);
  $2 = PyInt_AsLong(pyver);
  Py_XDECREF(pyver);
}

%typemap(in) (int argc, char const * const argv[])
{
  if (!PyList_Check($input))
  {
    PyErr_SetString(PyExc_TypeError, "not a list");
    return 0;
  }
  $1 = PyList_Size($input);
  typedef char * Char_Ptr;
  $2 = new Char_Ptr[$1 + 1];
  for (int i = 0; i < $1; ++i)
  {
    PyObject * o = PyList_GetItem($input, i);
    if (!PyString_Check(o))
    {
      PyErr_SetString(PyExc_TypeError, "list must contain strings");
      delete [] $2;
      return 0;
    }
    $2[i] = PyString_AsString(o);
  }
  $2[$1] = 0;
}

%typemap(freearg) (int argc, char const * const argv[])
{
  delete [] $2;
}

%typemap(in) (const char * description, ...)
{
  $1 = "%s";
  $2 = (void *) PyString_AsString($input);
}

#undef TYPEMAP_OUTARG_ARRAY_BODY
%define TYPEMAP_OUTARG_ARRAY_BODY(array_type, base_type, cnt, ptr, to_item)
  PyObject * result_tuple;
  if (!PyTuple_Check($result))
  {
    result_tuple = $result;
    $result = PyTuple_New(1);
    PyTuple_SetItem($result, 0, $result);
  }
  PyObject * arr = PyList_New(cnt);
  for (int i = 0; i < cnt; ++i)
  {
    base_type * item = new base_type(to_item ptr[i]);
    PyObject* o = SWIG_NewPointerObj((void*)item, $descriptor(basetype*), 1);
    PyList_SetItem(arr, i, o);
  }
  PyList_Append(l, arr);
  PyObject * arr_tuple = PyTuple_New(1);
  PyTuple_SetItem(arr_tuple, 0, arr);
  result_tuple = $result;
  $result = PySequence_Concat(result_tuple, arr_tuple);
  Py_DECREF(result_tuple);
  Py_DECREF(arr_tuple);
%enddef

#undef TYPEMAP_OUTARG_ARRAY_CNT_PTR
%define TYPEMAP_OUTARG_ARRAY_CNT_PTR(pattern, ptr_init, to_item)
  %typemap(in, numinputs=0) pattern ($2_type ptr, $1_basetype cnt)
  {
    $1 = &cnt;
    $2 = ($2_type) ptr_init;
  }
  %typemap(outarg) pattern
  {
    TYPEMAP_OUTARG_ARRAY_BODY($*2_type, $2_basetype, $1, $2, to_item)
  }
  %typemap(freearg) pattern
  {
    delete [] $2;
  }
%enddef

#undef TYPEMAP_OUTARG_ARRAY_PTR_CNT
%define TYPEMAP_OUTARG_ARRAY_PTR_CNT(pattern, ptr_init, to_item)
  %typemap(in, numinputs=0) pattern ($1_type ptr, $2_basetype cnt)
  {
    $1 = ($1_type) ptr_init;
    $2 = &cnt;
  }
  %typemap(outarg) pattern
  {
    TYPEMAP_OUTARG_ARRAY_BODY($*1_type, $1_basetype, $2, $1, to_item)
  }
  %typemap(freearg) pattern
  {
    delete [] $1;
  }
%enddef

#undef TYPEMAP_IN_ARRAY_BODY
%define TYPEMAP_IN_ARRAY_BODY(array_type, base_type, base_descriptor, cnt, ptr, to_item)
  if (!PyList_Check($input))
  {
    PyErr_SetString(PyExc_TypeError, "not a list");
    return 0;
  }
  cnt = PyList_Size($input);
  typedef array_type Array_Type;
  ptr = new Array_Type[cnt];
  for (int i = 0; i < cnt; ++i)
  {
    base_type * p;
    PyObject * o = PyList_GetItem($input, i);
    if ((SWIG_ConvertPtr(o, (void **) &p, base_descriptor,
      SWIG_POINTER_EXCEPTION)) == -1)
    {
      PyErr_SetString(PyExc_TypeError, "list must contain " #base_type "'s");
      delete [] ptr;
      return 0;
    }
    ptr[i] = to_item p;
  }
%enddef

#undef TYPEMAP_IN_ARRAY_CNT_PTR
%define TYPEMAP_IN_ARRAY_CNT_PTR(pattern, to_item)
  %typemap(in) pattern
  {
    TYPEMAP_IN_ARRAY_BODY($*2_type, $2_basetype, $2_descriptor, $1,$2, to_item)
  }
  %typemap(freearg) pattern
  {
    delete [] $2;
  }
%enddef

#undef TYPEMAP_IN_ARRAY_PTR_CNT
%define TYPEMAP_IN_ARRAY_PTR_CNT(pattern, to_item)
  %typemap(in) pattern
  {
    TYPEMAP_IN_ARRAY_BODY($*1_type, $1_basetype, $1_descriptor, $2,$1, to_item)
  }
  %typemap(freearg) pattern
  {
    delete [] $1;
  }
%enddef

%typemap(in) csEventID[]
{
  int cnt; 
  if (!PyList_Check($input))
  {
    PyErr_SetString(PyExc_TypeError, "not a list");
    return 0;
  }
  cnt = PyList_Size($input);
  csEventID * ptr = new csEventID[cnt];
  for (int i = 0; i < cnt; ++i)
  {
    PyObject * o = PyList_GetItem($input, i);
    if (!PyNumber_Check(o))
    {
      PyErr_SetString(PyExc_TypeError, "list must contain numbers");
      delete [] ptr;
      return 0;
    }
    ptr[i] = (csEventID) PyInt_AsUnsignedLongMask(o);
  }
  $1 = ptr;
}

#ifndef CS_MINI_SWIG
/*
 * Using the shadow feature allows us to define our own python code to replace
 * the automatically generated wrapper for a function.	This is also the only
 * way I could locate to allow us to inject code into a ?proxy? object.	 (Not
 * sure if that's the correct SWIG terminology.)  This means if you want to
 * add a python method to a class, there needs to be some underlying method
 * that SWIG is trying to wrap whose auto-generated code you will replace.
 * If you want to introduce a completely new method, I imagine this means you
 * would need to use %extend to introduce such a function.
 *
 * I suppose the following could be accomplished via a %define, but this
 * seems much more readable/intuitive.
 */

// Let cspace.i know that we are providing overrides for these methods.
#define CS_SWIG_PUBLISH_IGENERAL_FACTORY_STATE_ARRAYS

%feature("shadow") iGeneralFactoryState::GetVertices()
%{
  def GetVertices(self):
    return CSMutableArrayHelper(self.GetVertexByIndex, self.GetVertexCount)
%}

%feature("shadow") iGeneralFactoryState::GetTexels()
%{
  def GetTexels(self):
    return CSMutableArrayHelper(self.GetTexelByIndex, self.GetVertexCount)
%}

%feature("shadow") iGeneralFactoryState::GetNormals()
%{
  def GetNormals(self):
    # iGeneralFactoryState::GetVertices()
    return CSMutableArrayHelper(self.GetNormalByIndex, self.GetVertexCount)
%}

%feature("shadow") iGeneralFactoryState::GetTriangles()
%{
  def GetTriangles(self):
    return CSMutableArrayHelper(self.GetTriangleByIndex, self.GetTriangleCount)
%}

%feature("shadow") iGeneralFactoryState::GetColors()
%{
  def GetColors(self):
    return CSMutableArrayHelper(self.GetColorByIndex, self.GetVertexCount)
%}

%feature("shadow") iPolygonMesh::GetTriangles()
%{
  def GetTriangles(self):
    return CSMutableArrayHelper(self.GetTriangleByIndex, self.GetTriangleCount)
%}
%feature("shadow") iPolygonMesh::GetPolygons()
%{
  def GetPolygons(self):
    return CSMutableArrayHelper(self.GetPolygonByIndex, self.GetPolygonCount)
%}
%feature("shadow") iPolygonMesh::GetVertices()
%{
  def GetVertices(self):
    return CSMutableArrayHelper(self.GetVertexByIndex, self.GetVertexCount)
%}

/* Macro to add an iterator to a python proxy
   requires the class to have __len__ and __getitem__ methods */
%define PYITERATOR_PROTOCOL(classname)
%extend classname
{
    %pythoncode %{
        def content_iterator(self):
                for idx in xrange(len(self)):
                        yield self.__getitem__(idx)
        def __iter__(self): return self.content_iterator()  %}
}
%enddef

/* List Methods*/

%define PYLIST_BASE_FUNCTIONS(classname,typename,idxtype,countmethod,getmethod,addmethod,removemethod,findmethod)
%extend classname {
        typename __getitem__( idxtype n) {return self-> ## getmethod ## (n);}
	bool __contains__(typename obj) {
		if (self-> ## findmethod ## (obj) == 
				(idxtype)csArrayItemNotFound)
			return false;
		return true;
	}
        bool __delitem__(idxtype n) { return self-> ## removemethod ## (n); }
        int __len__() { return self-> ## countmethod ## (); }
        void append(typename e) { self-> ## addmethod ## (e); }
}
PYITERATOR_PROTOCOL(classname)
%enddef
%define PYLIST_BYNAME_FUNCTIONS(classname,typename,findbynamemethod)
%extend classname {
	typename __getitem__( const char* name) {return self-> ## findbynamemethod ## (name);}
	bool __contains__(const char *name) {
		if (self-> ## findbynamemethod ## (name))
			return true;
		return false;
	}
}
%enddef
/* Array Functions */
#undef ARRAY_OBJECT_FUNCTIONS
%define ARRAY_OBJECT_FUNCTIONS(classname,typename)
        PYLIST_BASE_FUNCTIONS(classname,typename,size_t,GetSize,Get,Push,Delete,Find)
%enddef
/* Pseudo-List Functions */
%define LIST_OBJECT_FUNCTIONS(classname,typename)
        PYLIST_BASE_FUNCTIONS(classname,typename *,int,GetCount,Get,Add,Remove,Find)
        PYLIST_BYNAME_FUNCTIONS(classname,typename *,FindByName)
%enddef

%define SET_OBJECT_FUNCTIONS(classname,typename)
%extend classname {
	int __len__() {return self->GetSize();}
	bool __contains__( typename o) {return self->Contains(o);}
	void append( typename o) {return self->Add(o);}
	bool __delitem__( typename o) { return self->Delete(o);}
}
%enddef

/*
 * Macro for custom ARGOUT pointers. 
 * Use when you need to declare some input argument as output only.
 * Used like in:
 * TYPEMAP_ARGOUT_PTR(csKeyModifiers)
 * APPLY_TYPEMAP_ARGOUT_PTR(csKeyModifiers, csKeyModifiers& modifiers)
 */
#undef TYPEMAP_ARGOUT_PTR
%define TYPEMAP_ARGOUT_PTR(Type)
%typemap (in,numinputs=0) Type * ARGOUT
{
   $1 = new Type();
}
%typemap (argout) Type * ARGOUT
{
   $result = SWIG_NewPointerObj((void*)$1, SWIG_TypeQuery( #Type " *"), 1);
}
%enddef

#undef APPLY_TYPEMAP_ARGOUT_PTR
%define APPLY_TYPEMAP_ARGOUT_PTR(Type,Args)
%apply Type * ARGOUT { Args };
%enddef

/*
 * Macro for implementing the python iterator protocol for objects
 * with Next and HasNext functions.
 * Must be used inside an %extend block for ClassName.
*/
#undef ITERATOR_FUNCTIONS
%define ITERATOR_FUNCTIONS(ClassName)
%pythoncode %{
def __iter__(self):
    while self.HasNext():
        yield self.Next() %}
%enddef

%ignore iPen::Rotate;

// csStringFast typemaps
%typemap(out) csStringFast *
{
        const char *res = $1->GetData();
        $result = SWIG_FromCharPtr(res);
}

%typemap(in) csStringFast *
{
        $1 = new $1_basetype (PyString_AsString($input));
}

%typemap(freearg) csStringFast *
{
   delete $1;
}

#endif // ifndef CS_MINI_SWIG

#endif // SWIGPYTHON
