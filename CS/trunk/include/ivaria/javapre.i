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

#ifdef SWIGJAVA

#include <stdio.h>

%javaconst(1);

// Following are declared as constants here to prevent javac
// complaining about finding a 'long' where an 'int' is expected.
%constant int CS_CRYSTAL_PROTOCOL = 0x43533030;
%constant int CS_MUSCLE_PROTOCOL = 0x504d3030;
%constant int CS_XML_PROTOCOL = 0x584d4d30;
%ignore CS_CRYSTAL_PROTOCOL;
%ignore CS_MUSCLE_PROTOCOL;
%ignore CS_XML_PROTOCOL;

%typemap(javabase) iBase "cspace";

%ignore *::operator[];
%rename(get) *::operator[] const;
%ignore *::operator();
%rename(add) *::operator+;
%rename(subtract) *::operator-;
%rename(multiply) *::operator*;
%rename(divide) *::operator/;
%rename(modulo) *::operator%;
%rename(leftShift) *::operator<<;
%rename(rightShift) *::operator>>;
%rename(bitAnd) *::operator&;
%rename(bitOr) *::operator|;
%rename(bitXor) *::operator^;
%rename(and) *::operator&&;
%rename(or) *::operator||;
%rename(isLessThan) *::operator<;
%rename(equalsOrLess) *::operator<=;
%rename(isGreaterThen) *::operator>;
%rename(equalsOrGreater) *::operator>=;
%rename(equals) *::operator==;
%rename(equalsNot) *::operator!=;

%ignore *::operator+();
%rename(negate) *::operator-();
%rename(not) *::operator!;
%rename(bitComplement) *::operator~;
%rename(increment) *::operator++();
%rename(getAndIncrement) *::operator++(int);
%rename(decrement) *::operator--();
%rename(getAndDecrement) *::operator--(int);

%rename(assign) *::operator=;
%rename(addAssign) *::operator+=;
%rename(subtractAssign) *::operator-=;
%rename(multiplyAssign) *::operator*=;
%rename(divideAssign) *::operator/=;
%rename(moduloAssign) *::operator%=;
%rename(leftShiftAssign) *::operator<<=;
%rename(rightShiftAssign) *::operator>>=;
%rename(bitAssign) *::operator&=;
%rename(bitOrAssign) *::operator|=;
%rename(bitXorAssign) *::operator^=;

// csgeom/plane3.h
%ignore csPlane3::Normal () const;

// csgeom/sphere.h
%ignore csSphere::GetCenter () const;

// csgeom/poly2d.h
%ignore csPoly2D::GetVertexCount () const;

// csgeom/transfrm.h
%rename(mul2) csTransform::operator * (const csReversibleTransform &);
%extend csReversibleTransform
{
	csTransform operator * (const csReversibleTransform & t) const
		{ return *self * t; } 
}

// iengine/camera.h
%ignore iCamera::GetTransform () const;

// iutil/event.h
%{
    // Workaround for bug in SWIG 1.3.19: reversed accessor functions!

    #define iEvent_get_Key iEvent_Key_get
    #define iEvent_get_Mouse iEvent_Mouse_get
    #define iEvent_get_Joystick iEvent_Joystick_get
    #define iEvent_get_Command iEvent_Command_get
    #define iEvent_get_Network iEvent_Network_get

    #define csEventNetworkData_get_From csEventNetworkData_From_get
    #define csEventNetworkData_get_From2 csEventNetworkData_From2_get
    #define csEventNetworkData_get_Data csEventNetworkData_Data_get
    #define csEventNetworkData_get_Data2 csEventNetworkData_Data2_get
%}

%{
jobject
_csRef_to_Java (const csRef<iBase> & ref, void * ptr, const char * name,
    const char * clazz, JNIEnv * jenv)
{
	if (!ref.IsValid())
	{
        return 0;
	}
	ref->IncRef();
    jlong cptr = 0;
    *(void **)&cptr = ptr; 
    jclass cls = jenv->FindClass(clazz);
    jmethodID mid = jenv->GetMethodID(cls, "<init>", "(JZ)V");
    return jenv->NewObject(cls, mid, cptr, false);
}
%}

/*
	ptr   : either a csRef<type> or csPtr<type>
	name  : type name, e.g. "iEngine *"
	type  : type of pointer
	clazz : class name, e.g. "net/sourceforge/crystal/iEngine"
*/
%define TYPEMAP_OUT_csRef_BODY(ptr, name, type, clazz)
	csRef<type> ref(ptr);
	$result = _csRef_to_Java(csRef<iBase>(
		(type *)ref), (void *)(type *)ref, name, clazz, jenv);
%enddef

#undef TYPEMAP_OUT_csRef
%define TYPEMAP_OUT_csRef(T)
	%typemap(out) csRef<T>
	{
		TYPEMAP_OUT_csRef_BODY($1, #T " *", T, "net/sourceforge/crystal/" #T)
	}
    %typemap(jni) csRef<T> "jobject";
    %typemap(jtype) csRef<T> #T;
    %typemap(jstype) csRef<T> #T;
    %typemap(javain) csRef<T> "$javainput";
    %typemap(javaout) csRef<T> { return $jnicall; }
%enddef

#undef TYPEMAP_OUT_csPtr
%define TYPEMAP_OUT_csPtr(T)
	%typemap(out) csPtr<T>
	{
		TYPEMAP_OUT_csRef_BODY($1, #T " *", T, "net/sourceforge/crystal/" #T)
	}
    //%typemap(out) csPtr<T> %{ $result = $1; %}
    %typemap(jni) csPtr<T> "jobject";
    %typemap(jtype) csPtr<T> #T;
    %typemap(jstype) csPtr<T> #T;
    %typemap(javain) csPtr<T> "$javainput";
    %typemap(javaout) csPtr<T> { return $jnicall; }
%enddef


#undef TYPEMAP_OUT_csWrapPtr
%define TYPEMAP_OUT_csWrapPtr
	%typemap(out) csWrapPtr
	{
        void * ptr = 0;
        if ($1.VoidPtr)
        {
            ptr = $1.VoidPtr;
        }
        else
        {
            ptr = iBase__DynamicCast((iBase *)$1.Ref, $1.Type).VoidPtr;
        }
    	//ref->IncRef();
        jlong cptr = 0;
        *(void **)&cptr = ptr;
        char cls_name[1024];
        strcat(strcpy(cls_name, "net/sourceforge/crystal/"), $1.Type);
        jclass cls = jenv->FindClass(cls_name);
        jmethodID mid = jenv->GetMethodID(cls, "<init>", "(JZ)V");
        $result = jenv->NewObject(cls, mid, cptr, false);
	}
	//%typemap(out) csWrapPtr %{ $result = $1; %}
	%typemap(jni) csWrapPtr "jobject";
	%typemap(jtype) csWrapPtr "Object";
	%typemap(jstype) csWrapPtr "Object";
	%typemap(javain) csWrapPtr "$javainput";
	//%typemap(javaout) csWrapPtr { return $jnicall; }
	%typemap(javaout) csWrapPtr { Object _obj = $jnicall; iBase ibase = (iBase) _obj; ibase.IncRef(); return _obj; }
%enddef

#undef INTERFACE_APPLY
%define INTERFACE_APPLY(T)
	%typemap(javacode) T %{
	    public boolean equals (Object obj)
	    {
	        boolean equal = false;
	        if (obj instanceof $javaclassname)
	            equal = ((($javaclassname)obj).swigCPtr == this.swigCPtr);
	        return equal;
	    }
	%}
%enddef
APPLY_FOR_EACH_INTERFACE
#undef INTERFACE_EQUALS

// argc-argv handling
%typemap(in) (int argc, char const * const argv[])
{
    $1 = jenv->GetArrayLength($input);
    $2 = (char **) malloc(($1 + 1) * sizeof(char *));
    /* make a copy of each string */
    int i;
    for (i = 0; i < $1; ++i) {
        jstring j_string = (jstring) jenv->GetObjectArrayElement($input, i);
        const char * c_string = jenv->GetStringUTFChars(j_string, 0);
        $2[i] = (char *) malloc(strlen(c_string + 1)*sizeof(const char *));
        strcpy($2[i], c_string);
        jenv->ReleaseStringUTFChars(j_string, c_string);
        jenv->DeleteLocalRef(j_string);
    }
    $2[i] = 0;
}

%typemap(freearg) (int argc, char const * const argv[])
{
    for (int i = 0; i < $1 - 1; ++i)
        free($2[i]);
    free($2);
}

%typemap(jni) (int argc, char const * const argv []) "jobjectArray"
%typemap(jtype) (int argc, char const * const argv []) "String[]"
%typemap(jstype) (int argc, char const * const argv []) "String[]"
%typemap(javain) (int argc, char const * const argv []) "$javainput"

// ivaria/event.h
%typemap(javacode) iEventOutlet
%{
    public void Broadcast (int iCode) { Broadcast(iCode, null); }
%}

#endif // SWIGJAVA
