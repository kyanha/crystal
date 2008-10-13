%{
	typedef unsigned int StringIDValue;
%}

#undef TYPEMAP_CSEVENTID_ARRAY
%define TYPEMAP_CSEVENTID_ARRAY
%typemap(in) csEventID[]
{
	jlongArray longEventsArray = (jlongArray)$input;
	jlong * larray = JCALL2(GetLongArrayElements,jenv,longEventsArray,0);
	int totalElem = JCALL1(GetArrayLength,jenv,longEventsArray);
	csEventID * csEvents = new csEventID[totalElem];
	for (int j=0;j<totalElem;j++) 
	{
		csEvents[j] = csEventID(larray[j]);
	}
	JCALL3(ReleaseLongArrayElements,jenv,longEventsArray,larray,JNI_ABORT);
	$1 = csEvents;
}
%typemap(jni) csEventID[] "jlongArray"
%typemap(jtype) csEventID[] "long[]"
%typemap(jstype) csEventID[] "long[]"
%typemap(javain) csEventID[] "$javainput"
%typemap(javaout) csEventID[] { return $jnicall; }
%typemap(freearg) csEventID[]
{
  delete[]($1);
}
%enddef
TYPEMAP_CSEVENTID_ARRAY
#undef TYPEMAP_CSEVENTID_ARRAY

#undef TYPEMAP_CSEVENTID
%define TYPEMAP_CSEVENTID
%typemap(in) csEventID
{
	csEventID csEvent = csEventID((long)$input);
	$1 = csEvent;
}
%typemap(out) csEventID
{
	$result = (jlong)(StringIDValue)(csEventID)$1;
}
%typemap(jni) csEventID "jlong"
%typemap(jtype) csEventID "long"
%typemap(jstype) csEventID "long"
%typemap(javain) csEventID "$javainput"
%typemap(javaout) csEventID { return $jnicall; }
%enddef
TYPEMAP_CSEVENTID
#undef TYPEMAP_CSEVENTID

// Typemap to handle csRef<xxx> & OUTPUT parameters
#undef OUTPUT_TYPEMAP_CSREF
%define OUTPUT_TYPEMAP_CSREF(type)
	%typemap(jni) csRef<type> &OUTPUT "jobjectArray"
	%typemap(jtype) csRef<type> &OUTPUT "type[]"
	%typemap(jstype) csRef<type> &OUTPUT "type[]"
	%typemap(javain) csRef<type> &OUTPUT "$javainput"
	%typemap(in) csRef<type> &OUTPUT($*1_ltype temp)
	{
		if (!$input) {
			SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
			return $null;
		}
		if (JCALL1(GetArrayLength, jenv, $input) == 0) {
			SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
			return $null;
		}
		$1 = &temp;
	}
	%typemap(argout) csRef<type> &OUTPUT 
	{
		JCALL3(SetObjectArrayElement, jenv, $input, 0, _csRef_to_Java(csRef<iBase>(
		(type*)temp$argnum), (void*)(type*)temp$argnum, #type " *", "org/crystalspace3d/" #type, jenv));
	}
%enddef
#undef INTERFACE_APPLY
%define INTERFACE_APPLY(T) 
	OUTPUT_TYPEMAP_CSREF(T) 
%enddef
APPLY_FOR_ALL_INTERFACES
#undef INTERFACE_APPLY
#undef OUTPUT_TYPEMAP_CSREF

#undef OUTPUT_TYPEMAP_STRING
%define OUTPUT_TYPEMAP_STRING
	%typemap(jni) char *OUTPUT "jobjectArray"
	%typemap(jtype) char *OUTPUT "String[]"
	%typemap(jstype) char *OUTPUT "String[]"
	%typemap(javain) char *OUTPUT "$javainput"
	%typemap(in) char *OUTPUT($*1_ltype temp)
	{
		if (!$input) {
			SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
			return $null;
		}
		if (JCALL1(GetArrayLength, jenv, $input) == 0) {
			SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
			return $null;
		}
		$1 = &temp;
	}
	%typemap(argout) char *OUTPUT
	{
		JCALL3(SetObjectArrayElement, jenv, $input, 0, JCALL1(NewStringUTF, jenv, temp$argnum));
	}
	%typemap(freearg) char *OUTPUT ""
%enddef
OUTPUT_TYPEMAP_STRING
#undef OUTPUT_TYPEMAP_STRING

#undef INOUT_TYPEMAP_STRING
%define INOUT_TYPEMAP_STRING
	%typemap(jni) char *INOUT "jobjectArray"
	%typemap(jtype) char *INOUT "String[]"
	%typemap(jstype) char *INOUT "String[]"
	%typemap(javain) char *INOUT "$javainput"
	%typemap(in) char *INOUT($*1_ltype temp,jstring theString)
	{
		if (!$input) {
			SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
			return $null;
		}
		if (JCALL1(GetArrayLength, jenv, $input) == 0) {
			SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
			return $null;
		}
		theString = (jstring)JCALL2(GetObjectArrayElement,jenv,$input,0);
		if (theString) {
			temp = (char *)(jbyte *)JCALL2(GetStringUTFChars,jenv,theString,0);
		}
		$1 = &temp;
	}
	%typemap(argout) char *INOUT
	{
		JCALL3(SetObjectArrayElement, jenv, $input, 0, JCALL1(NewStringUTF, jenv, temp$argnum));
		
	}
	%typemap(freearg) char *INOUT 
	{
		if (theString$argnum) {
			jenv->ReleaseStringUTFChars(theString$argnum,temp$argnum);
		}
	}
%enddef
INOUT_TYPEMAP_STRING
#undef INOUT_TYPEMAP_STRING

#undef INPUT_TYPEMAP_CSTYPE_ARRAY
%define INPUT_TYPEMAP_CSTYPE_ARRAY(type)
	%typemap(jni) (type *INPUT,size_t INPUT) "jobjectArray"
	%typemap(jtype) (type *INPUT,size_t INPUT) "long[]"
	%typemap(jstype) (type *INPUT,size_t INPUT) "type[]"
	%typemap(javain) (type *INPUT,size_t INPUT) "cspaceUtils._ConvertArrayToNative($javainput)"
	%typemap(in) (type *INPUT,size_t INPUT) (type ** temp,size_t size)
	{
		if (!$input) {
			SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
			return $null;
		}
		size = JCALL1(GetArrayLength, jenv, $input);
		if (size == 0) {
			SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
			return $null;
		}
		jlongArray jlarray = (jlongArray)$input;
		jlong * larray = JCALL2(GetLongArrayElements,jenv,jlarray,0);
		temp = new type*[size];
		for (unsigned int i=0;i<size;i++) {
			temp[i] = (type*)(void*)(long)larray[i];
		}
		JCALL3(ReleaseLongArrayElements,jenv,jlarray,larray,JNI_ABORT);
		$1 = *temp;
		$2 = size;
	}
	%typemap(freearg) (type *INPUT,size_t INPUT)
	{
		delete[](temp$argnum);
	}
%enddef
INPUT_TYPEMAP_CSTYPE_ARRAY(csTriangleMeshEdge) 
INPUT_TYPEMAP_CSTYPE_ARRAY(csPlane3) 
#undef INPUT_TYPEMAP_CSTYPE_ARRAY

#undef INPUT_TYPEMAP_CSTYPE_ARRAY
%define INPUT_TYPEMAP_CSTYPE_ARRAY(type)
	%typemap(jni) (type *INPUT,size_t &INPUT) "jobjectArray"
	%typemap(jtype) (type *INPUT,size_t &INPUT) "long[]"
	%typemap(jstype) (type *INPUT,size_t &INPUT) "type[]"
	%typemap(javain) (type *INPUT,size_t &INPUT) "cspaceUtils._ConvertArrayToNative($javainput)"
	%typemap(in) (type *INPUT,size_t &INPUT) (type ** temp,size_t size)
	{
		if (!$input) {
			SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
			return $null;
		}
		size = JCALL1(GetArrayLength, jenv, $input);
		if (size == 0) {
			SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
			return $null;
		}
		jlongArray jlarray = (jlongArray)$input;
		jlong * larray = JCALL2(GetLongArrayElements,jenv,jlarray,0);
		temp = new type*[size];
		for (unsigned int i=0;i<size;i++) {
			temp[i] = (type*)(void*)(long)larray[i];
		}
		JCALL3(ReleaseLongArrayElements,jenv,jlarray,larray,JNI_ABORT);
		$1 = *temp;
		$2 = &size;
	}
	%typemap(freearg) (type *INPUT,size_t &INPUT)
	{
		delete[](temp$argnum);
	}
%enddef
INPUT_TYPEMAP_CSTYPE_ARRAY(iTriangleMesh) 
#undef INPUT_TYPEMAP_CSTYPE_ARRAY

%define OUTPUT_TYPEMAP_CSTYPE_ARRAY(type)
	%typemap(jni) (type *OUTPUT,size_t &OUTPUT) "jobjectArray"
	%typemap(jtype) (type *OUTPUT,size_t &OUTPUT) "type[][]"
	%typemap(jstype) (type *OUTPUT,size_t &OUTPUT) "type[][]"
	%typemap(javain) (type *OUTPUT,size_t &OUTPUT) "$javainput"
	%typemap(in) (type *OUTPUT,size_t &OUTPUT) (size_t size)
	{
		if (!$input) {
			SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
			return $null;
		}
		size = JCALL1(GetArrayLength, jenv, $input);
		if (size == 0) {
			SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
			return $null;
		}
		$1 = 0;
		$2 = &size;
	}
	%typemap(argout) (type *OUTPUT,size_t &OUTPUT)
	{
		jclass clazz = jenv->FindClass("org/crystalspace3d/" #type);
		jmethodID mid = jenv->GetMethodID(clazz, "<init>", "(JZ)V");
		jobjectArray oarray = JCALL3(NewObjectArray, jenv, (long)size$argnum, clazz, 0);
		for (unsigned int i = 0;i<size$argnum;i++) {
			JCALL3(SetObjectArrayElement, jenv, oarray, i, jenv->NewObject(clazz, mid, (jlong)(void*)($1+i), true));
		}
		JCALL3(SetObjectArrayElement, jenv, $input, 0, oarray);
	}
	%typemap(freearg) (type *OUTPUT,size_t &OUTPUT) ""
%enddef
OUTPUT_TYPEMAP_CSTYPE_ARRAY(csTriangleMeshEdge) 
#undef OUTPUT_TYPEMAP_CSTYPE_ARRAY

%define OUTPUT_TYPEMAP_CSTYPE_ARRAY(type)
	%typemap(jni) (type *OUTPUT) "jobjectArray"
	%typemap(jtype) (type *OUTPUT) "type[]"
	%typemap(jstype) (type *OUTPUT) "type[]"
	%typemap(javain) (type *OUTPUT) "$javainput"
	%typemap(in) (type *OUTPUT) (size_t size)
	{
		if (!$input) {
			SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
			return $null;
		}
		size = JCALL1(GetArrayLength, jenv, $input);
		if (size == 0) {
			SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
			return $null;
		}
		$1 = new type[size];
	}
	%typemap(argout) (type *OUTPUT)
	{
		jclass clazz = jenv->FindClass("org/crystalspace3d/" #type);
		jmethodID mid = jenv->GetMethodID(clazz, "<init>", "(JZ)V");
		for (unsigned int i = 0;i<size$argnum;i++) {
			JCALL3(SetObjectArrayElement, jenv, $input, i, jenv->NewObject(clazz, mid, (jlong)(void*)($1+i), true));
		}
	}
%enddef
OUTPUT_TYPEMAP_CSTYPE_ARRAY(csPlane3) 
OUTPUT_TYPEMAP_CSTYPE_ARRAY(csVector3) 
#undef OUTPUT_TYPEMAP_CSTYPE_ARRAY

/*
#undef OUTPUT_TYPEMAP_OUTLINE_EDGES
%define OUTPUT_TYPEMAP_OUTLINE_EDGES
	%typemap(jni) (size_t* outline_edges, size_t& num_outline_edges,bool* outline_verts) "jobject"
	%typemap(jtype) (size_t* outline_edges, size_t& num_outline_edges,bool* outline_verts) "csOutlineEdgesContainer"
	%typemap(jstype) (size_t* outline_edges, size_t& num_outline_edges,bool* outline_verts) "csOutlineEdgesContainer"
	%typemap(javain) (size_t* outline_edges, size_t& num_outline_edges,bool* outline_verts) "$javainput"
	%typemap(in) (size_t* outline_edges, size_t& num_outline_edges,bool* outline_verts) (size_t ** outline_edges,size_t num_outline_edges,bool ** outline_verts)
	{
	}
	%typemap(argout) (size_t* outline_edges, size_t& num_outline_edges,bool* outline_verts)
	{
	}
	%typemap(freearg) (size_t* outline_edges, size_t& num_outline_edges,bool* outline_verts)
	{
	}
%enddef
OUTPUT_TYPEMAP_OUTLINE_EDGES
#undef OUTPUT_TYPEMAP_OUTLINE_EDGES
*/

#undef OUTPUT_TYPEMAP_VOIDP
%define OUTPUT_TYPEMAP_VOIDP
	%typemap(jni) (void *&OUTPUT,size_t &OUTPUT) "jobjectArray"
	%typemap(jtype) (void *&OUTPUT,size_t &OUTPUT) "byte[][]"
	%typemap(jstype) (void *&OUTPUT,size_t &OUTPUT) "byte[][]"
	%typemap(javain) (void *&OUTPUT,size_t &OUTPUT) "$javainput"
	%typemap(in) (void *&OUTPUT,size_t &OUTPUT) (char * temp,size_t size)
	{
		if (!$input) {
			SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
			return $null;
		}
		if (JCALL1(GetArrayLength, jenv, $input) == 0) {
			SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
			return $null;
		}
		$1 = (void**)&temp;
		$2 = &size;
	}
	%typemap(argout) (void *&OUTPUT,size_t &OUTPUT)
	{
		jbyteArray barray = JCALL1(NewByteArray, jenv, (long)size$argnum);
		jbyte * pbarray = JCALL2(GetByteArrayElements,jenv,barray,0);
		memcpy(pbarray,temp$argnum,size$argnum);
		JCALL3(ReleaseByteArrayElements, jenv, barray, pbarray, JNI_COMMIT);
		JCALL3(SetObjectArrayElement, jenv, $input, 0, barray);
	}
	%typemap(freearg) (void *&OUTPUT,size_t &OUTPUT) ""
%enddef
OUTPUT_TYPEMAP_VOIDP
#undef OUTPUT_TYPEMAP_VOIDP

#undef INPUT_TYPEMAP_VOIDP
%define INPUT_TYPEMAP_VOIDP
	%typemap(jni) (void *INPUT,size_t INPUT) "jbyteArray"
	%typemap(jtype) (void *INPUT,size_t INPUT) "byte[]"
	%typemap(jstype) (void *INPUT,size_t INPUT) "byte[]"
	%typemap(javain) (void *INPUT,size_t INPUT) "$javainput"
	%typemap(in) (void *INPUT,size_t INPUT) (char * temp,size_t size)
	{
		if (!$input) {
			SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
			return $null;
		}
		size = JCALL1(GetArrayLength, jenv, $input);
		if (size == 0) {
			SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
			return $null;
		}
		temp = (char *)JCALL2(GetByteArrayElements,jenv,$input,0);
		$1 = (void *)temp;
		$2 = size;
	}
	%typemap(freearg) (void *INPUT,size_t INPUT)
	{
		JCALL3(ReleaseByteArrayElements, jenv, $input, (jbyte *)temp$argnum, JNI_ABORT);
	}
%enddef
INPUT_TYPEMAP_VOIDP
#undef INPUT_TYPEMAP_VOIDP

#undef INPUT_BONE_INDICES
%define INPUT_BONE_INDICES
%typemap(in) csArray<csArray<unsigned int> >& INPUT
{
	csArray<csArray<unsigned int> > * uiaa = new csArray<csArray<unsigned int> >();
	jobjectArray boneIndicesarray = (jobjectArray)$input;
	int length = JCALL1(GetArrayLength,jenv,boneIndicesarray);
	for (int i=0;i<length;i++) 
	{
		csArray<unsigned int> uia;
		jlongArray indice = (jlongArray)JCALL2(GetObjectArrayElement,jenv,boneIndicesarray,i);
		jlong * larray = JCALL2(GetLongArrayElements,jenv,indice,0);
		int lengthinner = JCALL1(GetArrayLength,jenv,indice);
		int j = 0;
		for (j=0;j<lengthinner;j++) 
		{
			uia.Push((unsigned int)larray[j]);
		}
		uiaa->Push(uia);
		JCALL3(ReleaseLongArrayElements,jenv,indice,larray,JNI_ABORT);
	}
	$1 = uiaa;
}
%typemap(jni) csArray<csArray<unsigned int> >& INPUT "jobjectArray"
%typemap(jtype) csArray<csArray<unsigned int> >& INPUT "long[][]"
%typemap(jstype) csArray<csArray<unsigned int> >& INPUT "long[][]"
%typemap(javain) csArray<csArray<unsigned int> >& INPUT "$javainput"
%typemap(freearg) csArray<csArray<unsigned int> >& INPUT
{
  delete($1);
}
%enddef
INPUT_BONE_INDICES
#undef INPUT_BONE_INDICES

#undef INPUT_IRENDERBUFFER_INDICES
%define INPUT_IRENDERBUFFER_INDICES
%typemap(in) csArray<iRenderBuffer*>& INPUT
{
	csArray<iRenderBuffer*> * ira = new csArray<iRenderBuffer*>();
	// Fills the arrays
	jlongArray indicesarray = (jlongArray)$input;
	jlong * larray = JCALL2(GetLongArrayElements,jenv,indicesarray,0);
	int length = JCALL1(GetArrayLength,jenv,indicesarray);
	int i = 0;
	for (i=0;i<length;i++) 
	{
		iRenderBuffer* ir = (iRenderBuffer*)(void*)(long)(larray[i]);
		ira->Push(ir);
	}
	JCALL3(ReleaseLongArrayElements,jenv,indicesarray,larray,JNI_ABORT);
	$1 = ira;
}
%typemap(jni) csArray<iRenderBuffer*>& INPUT "jlongArray"
%typemap(jtype) csArray<iRenderBuffer*>& INPUT "long[]"
%typemap(jstype) csArray<iRenderBuffer*>& INPUT "SWIGTYPE_p_iRenderBuffer []"
%typemap(javain) csArray<iRenderBuffer*>& INPUT "cspaceUtils._ConvertArrayToNative($javainput)"
%typemap(freearg) csArray<iRenderBuffer*>& INPUT
{
  delete($1);
}
%enddef
INPUT_IRENDERBUFFER_INDICES
#undef INPUT_IRENDERBUFFER_INDICES

#undef TYPEMAP_CSUTILS
%define TYPEMAP_CSUTILS
%typemap(javacode) cspaceUtils 
%{
	public static long[] _ConvertArrayToNative(final Object [] array) 
	{
		if (array == null) 
		{
			return new long[0];
		}
		final long[] pointers = new long[array.length];
		for (int i=0;i<array.length;i++) 
		{
			try 
			{
				if (array[i] == null) continue;
				java.lang.reflect.Field swigCPtr = array[i].getClass().getDeclaredField("swigCPtr");
				swigCPtr.setAccessible(true);
				pointers[i] = ((Long)swigCPtr.get(array[i])).longValue();
			}
			catch (Exception ignore){}
		}
		return pointers;
	}
%}
%enddef
TYPEMAP_CSUTILS
#undef TYPEMAP_CSUTILS

/*
#undef ARRAY_TYPEMAP_CSTYPE
%define ARRAY_TYPEMAP_CSTYPE(type)
	%typemap(jni) type **INPUT "jlongArray"
	%typemap(jtype) type **INPUT "long[]"
	%typemap(jstype) type **INPUT "type[]"
	%typemap(javain) type **INPUT "cspaceUtils._ConvertArrayToNative($javainput)"
	%typemap(javaout) type **INPUT "zut($javaoutput)"
	%typemap(in) type **INPUT (type** temp,int totalElem)
	{
		if ($input) {
			jlong * larray = jenv->GetLongArrayElements($input,0);
			totalElem = jenv->GetArrayLength($input);
			temp = new type*[totalElem];		
			for (int j=0;j<totalElem;j++) 
			{
				temp[j] = ((type*)(void*)larray[j]);
			}
			jenv->ReleaseLongArrayElements($input,larray,JNI_ABORT);
			$1 = temp;
		}
	}
	%typemap(freearg) type **INPUT
	{
		if (temp$argnum) {
			delete temp$argnum;
		}
	}
%enddef

%inline %{
	class JavaUtils {
		public:
		static unsigned int GetListLength(jobject list) 
		{
			return 0;
		}
		static jlong GetNativePointer(jobject obj) 
		{
			return 0;
		}
		static jobject NewList() 
		{
			return 0;
		}
		static jobject GetListElement(jobject list,unsigned int index) 
		{
			return 0;
		}
		static jobject AddToList(jobject list,jlong pointer,char * className,jobject refList) 
		{
			return 0;
		}
		static jobject ClearListAndAddAll(jobject list,jobject listToAdd) 
		{
			return 0;
		}
	}
%}
#undef OBJECT_LIST_INOUT_TYPEMAP_CSTYPE
%define OBJECT_LIST_INOUT_TYPEMAP_CSTYPE(type)
	%typemap(jni) type **INPUT "jobject"
	%typemap(jtype) type **INPUT "java.util.List<type>"
	%typemap(jstype) type **INPUT "java.util.List<type>"
	%typemap(javain) type **INPUT "$javainput"
	%typemap(javaout) type **INPUT "$javaoutput"
	%typemap(in) type **INPUT (type** temp,int totalElem,jobject obj)
	{
		if ($input) {
			obj = $input;	
			totalElem = JavaUtils::GetListLength($input);
			temp = new type*[totalElem];		
			for (int j=0;j<totalElem;j++) 
			{
				temp[j] = ((type*)(void*)JavaUtils::GetNativePointer(_csjava_GetListElement($input,j)));
			}
			$1 = temp;
		}
	}
	%typemap(argout) type **INPUT
	{
		jobject newList = JavaUtils::NewList();
		int j = 0;
		while (true) 
		{
			type * val = dynamic_cast<type*>(temp$argnum[j]);
			if (val != 0) {
				JavaUtils::AddToList(newList,(jlong)(void*)val,"org/crystalspace3d/"#type,obj$argnum);
			} else {
				break;
			}
			j++;
		}
		JavaUtils::ClearListAndAddAll($input,newList);
	}
	%typemap(out) type **INPUT
	{
		jobject newList = JavaUtils::NewList();
		int j = 0;
		while (true) 
		{
			type * val = dynamic_cast<type*>(temp$argnum[j]);
			if (val != 0) {
				JavaUtils::AddToList(newList,(jlong)(void*)val,"org/crystalspace3d/"#type,0);
			} else {
				break;
			}
			j++;
		}
		$result = newList;
	}
	%typemap(freearg) type **INPUT
	{
		if (temp$argnum) {
			delete temp$argnum;
		}
	}
%enddef
*/

