# This file was automatically generated by SWIG (http://www.swig.org).
# Version 1.3.33
#
# Don't modify this file, modify the SWIG interface instead.

import _imap
import new
new_instancemethod = new.instancemethod
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if (name == "thisown"): return self.this.own(value)
        if hasattr(self,name) or (name == "this"):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


import core
import iengine
import ivideo
import csgfx
import isndsys
_SetSCFPointer = _imap._SetSCFPointer
_GetSCFPointer = _imap._GetSCFPointer
if not "core" in dir():
    core = __import__("cspace").__dict__["core"]
core.AddSCFLink(_SetSCFPointer)
CSMutableArrayHelper = core.CSMutableArrayHelper

KEEP_ALL = _imap.KEEP_ALL
KEEP_USED = _imap.KEEP_USED
class iMissingLoaderData(core.iBase):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def MissingMaterial(*args): return _imap.iMissingLoaderData_MissingMaterial(*args)
    def MissingTexture(*args): return _imap.iMissingLoaderData_MissingTexture(*args)
    def MissingShader(*args): return _imap.iMissingLoaderData_MissingShader(*args)
    def MissingFactory(*args): return _imap.iMissingLoaderData_MissingFactory(*args)
    def MissingMesh(*args): return _imap.iMissingLoaderData_MissingMesh(*args)
    def MissingSector(*args): return _imap.iMissingLoaderData_MissingSector(*args)
    def MissingLight(*args): return _imap.iMissingLoaderData_MissingLight(*args)
    scfGetVersion = staticmethod(_imap.iMissingLoaderData_scfGetVersion)
    __swig_destroy__ = _imap.delete_iMissingLoaderData
    __del__ = lambda self : None;
iMissingLoaderData_swigregister = _imap.iMissingLoaderData_swigregister
iMissingLoaderData_swigregister(iMissingLoaderData)
iMissingLoaderData_scfGetVersion = _imap.iMissingLoaderData_scfGetVersion

class csLoadResult(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    success = _swig_property(_imap.csLoadResult_success_get, _imap.csLoadResult_success_set)
    result = _swig_property(_imap.csLoadResult_result_get, _imap.csLoadResult_result_set)
    def __init__(self, *args): 
        this = _imap.new_csLoadResult(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _imap.delete_csLoadResult
    __del__ = lambda self : None;
csLoadResult_swigregister = _imap.csLoadResult_swigregister
csLoadResult_swigregister(csLoadResult)

class csLoaderReturn(object):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _imap.new_csLoaderReturn(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _imap.delete_csLoaderReturn
    __del__ = lambda self : None;
    def IsFinished(*args): return _imap.csLoaderReturn_IsFinished(*args)
    def WasSuccessful(*args): return _imap.csLoaderReturn_WasSuccessful(*args)
    def GetResultPtr(*args): return _imap.csLoaderReturn_GetResultPtr(*args)
    def GetResultRefPtr(*args): return _imap.csLoaderReturn_GetResultRefPtr(*args)
    def MarkFinished(*args): return _imap.csLoaderReturn_MarkFinished(*args)
    def MarkSuccessful(*args): return _imap.csLoaderReturn_MarkSuccessful(*args)
    def SetResult(*args): return _imap.csLoaderReturn_SetResult(*args)
    def Copy(*args): return _imap.csLoaderReturn_Copy(*args)
    def Wait(*args): return _imap.csLoaderReturn_Wait(*args)
csLoaderReturn_swigregister = _imap.csLoaderReturn_swigregister
csLoaderReturn_swigregister(csLoaderReturn)

class iSectorLoaderIterator(core.iBase):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Next(*args): return _imap.iSectorLoaderIterator_Next(*args)
    def HasNext(*args): return _imap.iSectorLoaderIterator_HasNext(*args)
    __swig_destroy__ = _imap.delete_iSectorLoaderIterator
    __del__ = lambda self : None;
iSectorLoaderIterator_swigregister = _imap.iSectorLoaderIterator_swigregister
iSectorLoaderIterator_swigregister(iSectorLoaderIterator)

class iMeshFactLoaderIterator(core.iBase):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Next(*args): return _imap.iMeshFactLoaderIterator_Next(*args)
    def HasNext(*args): return _imap.iMeshFactLoaderIterator_HasNext(*args)
    __swig_destroy__ = _imap.delete_iMeshFactLoaderIterator
    __del__ = lambda self : None;
iMeshFactLoaderIterator_swigregister = _imap.iMeshFactLoaderIterator_swigregister
iMeshFactLoaderIterator_swigregister(iMeshFactLoaderIterator)

class iMeshLoaderIterator(core.iBase):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Next(*args): return _imap.iMeshLoaderIterator_Next(*args)
    def HasNext(*args): return _imap.iMeshLoaderIterator_HasNext(*args)
    __swig_destroy__ = _imap.delete_iMeshLoaderIterator
    __del__ = lambda self : None;
iMeshLoaderIterator_swigregister = _imap.iMeshLoaderIterator_swigregister
iMeshLoaderIterator_swigregister(iMeshLoaderIterator)

class iCamposLoaderIterator(core.iBase):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Next(*args): return _imap.iCamposLoaderIterator_Next(*args)
    def HasNext(*args): return _imap.iCamposLoaderIterator_HasNext(*args)
    __swig_destroy__ = _imap.delete_iCamposLoaderIterator
    __del__ = lambda self : None;
iCamposLoaderIterator_swigregister = _imap.iCamposLoaderIterator_swigregister
iCamposLoaderIterator_swigregister(iCamposLoaderIterator)

class iTextureLoaderIterator(core.iBase):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Next(*args): return _imap.iTextureLoaderIterator_Next(*args)
    def HasNext(*args): return _imap.iTextureLoaderIterator_HasNext(*args)
    __swig_destroy__ = _imap.delete_iTextureLoaderIterator
    __del__ = lambda self : None;
iTextureLoaderIterator_swigregister = _imap.iTextureLoaderIterator_swigregister
iTextureLoaderIterator_swigregister(iTextureLoaderIterator)

class iMaterialLoaderIterator(core.iBase):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Next(*args): return _imap.iMaterialLoaderIterator_Next(*args)
    def HasNext(*args): return _imap.iMaterialLoaderIterator_HasNext(*args)
    __swig_destroy__ = _imap.delete_iMaterialLoaderIterator
    __del__ = lambda self : None;
iMaterialLoaderIterator_swigregister = _imap.iMaterialLoaderIterator_swigregister
iMaterialLoaderIterator_swigregister(iMaterialLoaderIterator)

class iSharedVarLoaderIterator(core.iBase):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Next(*args): return _imap.iSharedVarLoaderIterator_Next(*args)
    def HasNext(*args): return _imap.iSharedVarLoaderIterator_HasNext(*args)
    __swig_destroy__ = _imap.delete_iSharedVarLoaderIterator
    __del__ = lambda self : None;
iSharedVarLoaderIterator_swigregister = _imap.iSharedVarLoaderIterator_swigregister
iSharedVarLoaderIterator_swigregister(iSharedVarLoaderIterator)

class iThreadedLoader(core.iBase):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetLoaderSectors(*args): return _imap.iThreadedLoader_GetLoaderSectors(*args)
    def GetLoaderMeshFactories(*args): return _imap.iThreadedLoader_GetLoaderMeshFactories(*args)
    def GetLoaderMeshes(*args): return _imap.iThreadedLoader_GetLoaderMeshes(*args)
    def GetLoaderCameraPositions(*args): return _imap.iThreadedLoader_GetLoaderCameraPositions(*args)
    def GetLoaderTextures(*args): return _imap.iThreadedLoader_GetLoaderTextures(*args)
    def GetLoaderMaterials(*args): return _imap.iThreadedLoader_GetLoaderMaterials(*args)
    def GetLoaderSharedVariables(*args): return _imap.iThreadedLoader_GetLoaderSharedVariables(*args)
    def AddSectorToList(*args): return _imap.iThreadedLoader_AddSectorToList(*args)
    def AddMeshFactToList(*args): return _imap.iThreadedLoader_AddMeshFactToList(*args)
    def AddMeshToList(*args): return _imap.iThreadedLoader_AddMeshToList(*args)
    def AddCamposToList(*args): return _imap.iThreadedLoader_AddCamposToList(*args)
    def AddTextureToList(*args): return _imap.iThreadedLoader_AddTextureToList(*args)
    def AddMaterialToList(*args): return _imap.iThreadedLoader_AddMaterialToList(*args)
    def AddSharedVarToList(*args): return _imap.iThreadedLoader_AddSharedVarToList(*args)
    __swig_destroy__ = _imap.delete_iThreadedLoader
    __del__ = lambda self : None;
iThreadedLoader_swigregister = _imap.iThreadedLoader_swigregister
iThreadedLoader_swigregister(iThreadedLoader)

class iLoader(core.iBase):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def LoadSoundSysData(*args): return _imap.iLoader_LoadSoundSysData(*args)
    def LoadSoundStream(*args): return _imap.iLoader_LoadSoundStream(*args)
    def LoadSoundWrapper(*args): return _imap.iLoader_LoadSoundWrapper(*args)
    def LoadImage(*args): return _imap.iLoader_LoadImage(*args)
    def LoadMeshObjectFactory(*args): return _imap.iLoader_LoadMeshObjectFactory(*args)
    def LoadMeshObject(*args): return _imap.iLoader_LoadMeshObject(*args)
    def LoadShader(*args): return _imap.iLoader_LoadShader(*args)
    def LoadTexture(*args): return _imap.iLoader_LoadTexture(*args)
    def LoadMapFile(*args): return _imap.iLoader_LoadMapFile(*args)
    def LoadMap(*args): return _imap.iLoader_LoadMap(*args)
    def LoadLibraryFile(*args): return _imap.iLoader_LoadLibraryFile(*args)
    def LoadLibrary(*args): return _imap.iLoader_LoadLibrary(*args)
    def Load(*args): return _imap.iLoader_Load(*args)
    scfGetVersion = staticmethod(_imap.iLoader_scfGetVersion)
    __swig_destroy__ = _imap.delete_iLoader
    __del__ = lambda self : None;
iLoader_swigregister = _imap.iLoader_swigregister
iLoader_swigregister(iLoader)
iLoader_scfGetVersion = _imap.iLoader_scfGetVersion

class iLoaderPlugin(core.iBase):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Parse(*args): return _imap.iLoaderPlugin_Parse(*args)
    def IsThreadSafe(*args): return _imap.iLoaderPlugin_IsThreadSafe(*args)
    scfGetVersion = staticmethod(_imap.iLoaderPlugin_scfGetVersion)
    __swig_destroy__ = _imap.delete_iLoaderPlugin
    __del__ = lambda self : None;
iLoaderPlugin_swigregister = _imap.iLoaderPlugin_swigregister
iLoaderPlugin_swigregister(iLoaderPlugin)
iLoaderPlugin_scfGetVersion = _imap.iLoaderPlugin_scfGetVersion

class iBinaryLoaderPlugin(core.iBase):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Parse(*args): return _imap.iBinaryLoaderPlugin_Parse(*args)
    def IsThreadSafe(*args): return _imap.iBinaryLoaderPlugin_IsThreadSafe(*args)
    scfGetVersion = staticmethod(_imap.iBinaryLoaderPlugin_scfGetVersion)
    __swig_destroy__ = _imap.delete_iBinaryLoaderPlugin
    __del__ = lambda self : None;
iBinaryLoaderPlugin_swigregister = _imap.iBinaryLoaderPlugin_swigregister
iBinaryLoaderPlugin_swigregister(iBinaryLoaderPlugin)
iBinaryLoaderPlugin_scfGetVersion = _imap.iBinaryLoaderPlugin_scfGetVersion

class iSaver(core.iBase):
    thisown = _swig_property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SaveMapFile(*args): return _imap.iSaver_SaveMapFile(*args)
    def SaveAllCollections(*args): return _imap.iSaver_SaveAllCollections(*args)
    def SaveCollectionFile(*args): return _imap.iSaver_SaveCollectionFile(*args)
    def SaveCollection(*args): return _imap.iSaver_SaveCollection(*args)
    def SavePortal(*args): return _imap.iSaver_SavePortal(*args)
    __swig_destroy__ = _imap.delete_iSaver
    __del__ = lambda self : None;
iSaver_swigregister = _imap.iSaver_swigregister
iSaver_swigregister(iSaver)

def CS_REQUEST_LEVELLOADER ():
  return core.CS_REQUEST_PLUGIN("crystalspace.level.loader", iLoader)



