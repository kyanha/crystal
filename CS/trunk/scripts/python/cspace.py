# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _cspace
import new
new_instancemethod = new.instancemethod
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


CS_WCHAR_T_SIZE = _cspace.CS_WCHAR_T_SIZE
class csWrapPtr(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csWrapPtr, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csWrapPtr, name)
    __repr__ = _swig_repr
    __swig_setmethods__["Ref"] = _cspace.csWrapPtr_Ref_set
    __swig_getmethods__["Ref"] = _cspace.csWrapPtr_Ref_get
    if _newclass:Ref = property(_cspace.csWrapPtr_Ref_get, _cspace.csWrapPtr_Ref_set)
    __swig_getmethods__["Type"] = _cspace.csWrapPtr_Type_get
    if _newclass:Type = property(_cspace.csWrapPtr_Type_get)
    __swig_setmethods__["Version"] = _cspace.csWrapPtr_Version_set
    __swig_getmethods__["Version"] = _cspace.csWrapPtr_Version_get
    if _newclass:Version = property(_cspace.csWrapPtr_Version_get, _cspace.csWrapPtr_Version_set)
    def __init__(self, *args): 
        this = _cspace.new_csWrapPtr(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csWrapPtr
    __del__ = lambda self : None;
csWrapPtr_swigregister = _cspace.csWrapPtr_swigregister
csWrapPtr_swigregister(csWrapPtr)

def fix_args(funct):
    def _inner(self, args):
        if type(args) == tuple:
            args = (self,) + args
        else:
            args = (self, args)
        return funct(*args)
    return _inner

class csArrayThresholdVariable(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csArrayThresholdVariable, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csArrayThresholdVariable, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csArrayThresholdVariable(*args)
        try: self.this.append(this)
        except: self.this = this
    def GetThreshold(*args): return _cspace.csArrayThresholdVariable_GetThreshold(*args)
    __swig_destroy__ = _cspace.delete_csArrayThresholdVariable
    __del__ = lambda self : None;
csArrayThresholdVariable_swigregister = _cspace.csArrayThresholdVariable_swigregister
csArrayThresholdVariable_swigregister(csArrayThresholdVariable)

class csArrayThresholdVariableCapacityLinear(csArrayThresholdVariable):
    __swig_setmethods__ = {}
    for _s in [csArrayThresholdVariable]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csArrayThresholdVariableCapacityLinear, name, value)
    __swig_getmethods__ = {}
    for _s in [csArrayThresholdVariable]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csArrayThresholdVariableCapacityLinear, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csArrayThresholdVariableCapacityLinear(*args)
        try: self.this.append(this)
        except: self.this = this
    def IsCapacityExcessive(*args): return _cspace.csArrayThresholdVariableCapacityLinear_IsCapacityExcessive(*args)
    def GetCapacity(*args): return _cspace.csArrayThresholdVariableCapacityLinear_GetCapacity(*args)
    __swig_destroy__ = _cspace.delete_csArrayThresholdVariableCapacityLinear
    __del__ = lambda self : None;
csArrayThresholdVariableCapacityLinear_swigregister = _cspace.csArrayThresholdVariableCapacityLinear_swigregister
csArrayThresholdVariableCapacityLinear_swigregister(csArrayThresholdVariableCapacityLinear)

class csArrayCapacityDefault(csArrayThresholdVariableCapacityLinear):
    __swig_setmethods__ = {}
    for _s in [csArrayThresholdVariableCapacityLinear]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csArrayCapacityDefault, name, value)
    __swig_getmethods__ = {}
    for _s in [csArrayThresholdVariableCapacityLinear]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csArrayCapacityDefault, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csArrayCapacityDefault(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csArrayCapacityDefault
    __del__ = lambda self : None;
csArrayCapacityDefault_swigregister = _cspace.csArrayCapacityDefault_swigregister
csArrayCapacityDefault_swigregister(csArrayCapacityDefault)

scfCompatibleVersion = _cspace.scfCompatibleVersion
class scfInterfaceMetadata(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, scfInterfaceMetadata, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, scfInterfaceMetadata, name)
    __repr__ = _swig_repr
    __swig_setmethods__["interfaceName"] = _cspace.scfInterfaceMetadata_interfaceName_set
    __swig_getmethods__["interfaceName"] = _cspace.scfInterfaceMetadata_interfaceName_get
    if _newclass:interfaceName = property(_cspace.scfInterfaceMetadata_interfaceName_get, _cspace.scfInterfaceMetadata_interfaceName_set)
    __swig_setmethods__["interfaceID"] = _cspace.scfInterfaceMetadata_interfaceID_set
    __swig_getmethods__["interfaceID"] = _cspace.scfInterfaceMetadata_interfaceID_get
    if _newclass:interfaceID = property(_cspace.scfInterfaceMetadata_interfaceID_get, _cspace.scfInterfaceMetadata_interfaceID_set)
    __swig_setmethods__["interfaceVersion"] = _cspace.scfInterfaceMetadata_interfaceVersion_set
    __swig_getmethods__["interfaceVersion"] = _cspace.scfInterfaceMetadata_interfaceVersion_get
    if _newclass:interfaceVersion = property(_cspace.scfInterfaceMetadata_interfaceVersion_get, _cspace.scfInterfaceMetadata_interfaceVersion_set)
    def __init__(self, *args): 
        this = _cspace.new_scfInterfaceMetadata(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_scfInterfaceMetadata
    __del__ = lambda self : None;
scfInterfaceMetadata_swigregister = _cspace.scfInterfaceMetadata_swigregister
scfInterfaceMetadata_swigregister(scfInterfaceMetadata)
cvar = _cspace.cvar
csArrayItemNotFound = cvar.csArrayItemNotFound

class scfInterfaceMetadataList(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, scfInterfaceMetadataList, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, scfInterfaceMetadataList, name)
    __repr__ = _swig_repr
    __swig_setmethods__["metadata"] = _cspace.scfInterfaceMetadataList_metadata_set
    __swig_getmethods__["metadata"] = _cspace.scfInterfaceMetadataList_metadata_get
    if _newclass:metadata = property(_cspace.scfInterfaceMetadataList_metadata_get, _cspace.scfInterfaceMetadataList_metadata_set)
    __swig_setmethods__["metadataCount"] = _cspace.scfInterfaceMetadataList_metadataCount_set
    __swig_getmethods__["metadataCount"] = _cspace.scfInterfaceMetadataList_metadataCount_get
    if _newclass:metadataCount = property(_cspace.scfInterfaceMetadataList_metadataCount_get, _cspace.scfInterfaceMetadataList_metadataCount_set)
    def __init__(self, *args): 
        this = _cspace.new_scfInterfaceMetadataList(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_scfInterfaceMetadataList
    __del__ = lambda self : None;
scfInterfaceMetadataList_swigregister = _cspace.scfInterfaceMetadataList_swigregister
scfInterfaceMetadataList_swigregister(scfInterfaceMetadataList)

class iBase(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, iBase, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, iBase, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def IncRef(*args): return _cspace.iBase_IncRef(*args)
    def DecRef(*args): return _cspace.iBase_DecRef(*args)
    def GetRefCount(*args): return _cspace.iBase_GetRefCount(*args)
    def QueryInterface(*args): return _cspace.iBase_QueryInterface(*args)
    def AddRefOwner(*args): return _cspace.iBase_AddRefOwner(*args)
    def RemoveRefOwner(*args): return _cspace.iBase_RemoveRefOwner(*args)
    def GetInterfaceMetadata(*args): return _cspace.iBase_GetInterfaceMetadata(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iBase_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iBase_scfGetVersion)
iBase_swigregister = _cspace.iBase_swigregister
iBase_swigregister(iBase)
iBase_scfGetVersion = _cspace.iBase_scfGetVersion

class iSCF(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSCF, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSCF, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_getmethods__["SCF"] = _cspace.iSCF_SCF_get
    if _newclass:SCF = property(_cspace.iSCF_SCF_get)
    def RegisterClasses(*args): return _cspace.iSCF_RegisterClasses(*args)
    def ClassRegistered(*args): return _cspace.iSCF_ClassRegistered(*args)
    def CreateInstance(*args): return _cspace.iSCF_CreateInstance(*args)
    def GetClassDescription(*args): return _cspace.iSCF_GetClassDescription(*args)
    def GetClassDependencies(*args): return _cspace.iSCF_GetClassDependencies(*args)
    def GetPluginMetadata(*args): return _cspace.iSCF_GetPluginMetadata(*args)
    def UnloadUnusedModules(*args): return _cspace.iSCF_UnloadUnusedModules(*args)
    def RegisterClass(*args): return _cspace.iSCF_RegisterClass(*args)
    def RegisterFactoryFunc(*args): return _cspace.iSCF_RegisterFactoryFunc(*args)
    def UnregisterClass(*args): return _cspace.iSCF_UnregisterClass(*args)
    def GetInterfaceName(*args): return _cspace.iSCF_GetInterfaceName(*args)
    def GetInterfaceID(*args): return _cspace.iSCF_GetInterfaceID(*args)
    def Finish(*args): return _cspace.iSCF_Finish(*args)
    def QueryClassList(*args): return _cspace.iSCF_QueryClassList(*args)
    def ScanPluginsPath(*args): return _cspace.iSCF_ScanPluginsPath(*args)
    def RegisterPlugin(*args): return _cspace.iSCF_RegisterPlugin(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSCF_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSCF_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSCF
    __del__ = lambda self : None;
iSCF_swigregister = _cspace.iSCF_swigregister
iSCF_swigregister(iSCF)
iSCF_scfGetVersion = _cspace.iSCF_scfGetVersion

SCF_STATIC_CLASS_CONTEXT = _cspace.SCF_STATIC_CLASS_CONTEXT
class iFactory(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iFactory, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iFactory, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CreateInstance(*args): return _cspace.iFactory_CreateInstance(*args)
    def TryUnload(*args): return _cspace.iFactory_TryUnload(*args)
    def QueryDescription(*args): return _cspace.iFactory_QueryDescription(*args)
    def QueryDependencies(*args): return _cspace.iFactory_QueryDependencies(*args)
    def QueryClassID(*args): return _cspace.iFactory_QueryClassID(*args)
    def QueryModuleName(*args): return _cspace.iFactory_QueryModuleName(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iFactory_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iFactory_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iFactory
    __del__ = lambda self : None;
iFactory_swigregister = _cspace.iFactory_swigregister
iFactory_swigregister(iFactory)
iFactory_scfGetVersion = _cspace.iFactory_scfGetVersion

SCF_VERBOSE_NONE = _cspace.SCF_VERBOSE_NONE
SCF_VERBOSE_PLUGIN_SCAN = _cspace.SCF_VERBOSE_PLUGIN_SCAN
SCF_VERBOSE_PLUGIN_LOAD = _cspace.SCF_VERBOSE_PLUGIN_LOAD
SCF_VERBOSE_PLUGIN_REGISTER = _cspace.SCF_VERBOSE_PLUGIN_REGISTER
SCF_VERBOSE_CLASS_REGISTER = _cspace.SCF_VERBOSE_CLASS_REGISTER
SCF_VERBOSE_ALL = _cspace.SCF_VERBOSE_ALL
scfRegisterStaticClasses = _cspace.scfRegisterStaticClasses
scfRegisterStaticFactoryFunc = _cspace.scfRegisterStaticFactoryFunc
CS_DBGHELP_UNITTEST = _cspace.CS_DBGHELP_UNITTEST
CS_DBGHELP_BENCHMARK = _cspace.CS_DBGHELP_BENCHMARK
CS_DBGHELP_TXTDUMP = _cspace.CS_DBGHELP_TXTDUMP
CS_DBGHELP_GFXDUMP = _cspace.CS_DBGHELP_GFXDUMP
CS_DBGHELP_STATETEST = _cspace.CS_DBGHELP_STATETEST
class iDebugHelper(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDebugHelper, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDebugHelper, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetSupportedTests(*args): return _cspace.iDebugHelper_GetSupportedTests(*args)
    def UnitTest(*args): return _cspace.iDebugHelper_UnitTest(*args)
    def StateTest(*args): return _cspace.iDebugHelper_StateTest(*args)
    def Benchmark(*args): return _cspace.iDebugHelper_Benchmark(*args)
    def Dump(*args): return _cspace.iDebugHelper_Dump(*args)
    def DebugCommand(*args): return _cspace.iDebugHelper_DebugCommand(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iDebugHelper_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iDebugHelper_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iDebugHelper
    __del__ = lambda self : None;
iDebugHelper_swigregister = _cspace.iDebugHelper_swigregister
iDebugHelper_swigregister(iDebugHelper)
scfRegisterStaticClass = _cspace.scfRegisterStaticClass
iDebugHelper_scfGetVersion = _cspace.iDebugHelper_scfGetVersion

class iCommandLineParser(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iCommandLineParser, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iCommandLineParser, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Initialize(*args): return _cspace.iCommandLineParser_Initialize(*args)
    def Reset(*args): return _cspace.iCommandLineParser_Reset(*args)
    def GetName(*args): return _cspace.iCommandLineParser_GetName(*args)
    def AddOption(*args): return _cspace.iCommandLineParser_AddOption(*args)
    def AddName(*args): return _cspace.iCommandLineParser_AddName(*args)
    def ReplaceOption(*args): return _cspace.iCommandLineParser_ReplaceOption(*args)
    def ReplaceName(*args): return _cspace.iCommandLineParser_ReplaceName(*args)
    def GetBoolOption(*args): return _cspace.iCommandLineParser_GetBoolOption(*args)
    def GetResourceDir(*args): return _cspace.iCommandLineParser_GetResourceDir(*args)
    def GetAppDir(*args): return _cspace.iCommandLineParser_GetAppDir(*args)
    def GetAppPath(*args): return _cspace.iCommandLineParser_GetAppPath(*args)
    def GetOptionName(*args): return _cspace.iCommandLineParser_GetOptionName(*args)
    def GetOption(*args): return _cspace.iCommandLineParser_GetOption(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iCommandLineParser_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iCommandLineParser_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iCommandLineParser
    __del__ = lambda self : None;
iCommandLineParser_swigregister = _cspace.iCommandLineParser_swigregister
iCommandLineParser_swigregister(iCommandLineParser)
iCommandLineParser_scfGetVersion = _cspace.iCommandLineParser_scfGetVersion

class csColor(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csColor, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csColor, name)
    __repr__ = _swig_repr
    __swig_setmethods__["red"] = _cspace.csColor_red_set
    __swig_getmethods__["red"] = _cspace.csColor_red_get
    if _newclass:red = property(_cspace.csColor_red_get, _cspace.csColor_red_set)
    __swig_setmethods__["green"] = _cspace.csColor_green_set
    __swig_getmethods__["green"] = _cspace.csColor_green_get
    if _newclass:green = property(_cspace.csColor_green_get, _cspace.csColor_green_set)
    __swig_setmethods__["blue"] = _cspace.csColor_blue_set
    __swig_getmethods__["blue"] = _cspace.csColor_blue_get
    if _newclass:blue = property(_cspace.csColor_blue_get, _cspace.csColor_blue_set)
    def __init__(self, *args): 
        this = _cspace.new_csColor(*args)
        try: self.this.append(this)
        except: self.this = this
    def Set(*args): return _cspace.csColor_Set(*args)
    def Clamp(*args): return _cspace.csColor_Clamp(*args)
    def ClampDown(*args): return _cspace.csColor_ClampDown(*args)
    def IsBlack(*args): return _cspace.csColor_IsBlack(*args)
    def assign(*args): return _cspace.csColor_assign(*args)
    def __iadd__(*args): return _cspace.csColor___iadd__(*args)
    def __isub__(*args): return _cspace.csColor___isub__(*args)
    def __imul__(*args): return _cspace.csColor___imul__(*args)
    def __mul__(*args): return _cspace.csColor___mul__(*args)
    def __eq__(*args): return _cspace.csColor___eq__(*args)
    def __ne__(*args): return _cspace.csColor___ne__(*args)
    def Add(*args): return _cspace.csColor_Add(*args)
    def Subtract(*args): return _cspace.csColor_Subtract(*args)
    def Luminance(*args): return _cspace.csColor_Luminance(*args)
    def __add__(*args): return _cspace.csColor___add__(*args)
    def __sub__(*args): return _cspace.csColor___sub__(*args)
    def __rmul__(*args): return _cspace.csColor___rmul__(*args)
    __swig_destroy__ = _cspace.delete_csColor
    __del__ = lambda self : None;
csColor_swigregister = _cspace.csColor_swigregister
csColor_swigregister(csColor)

class csColor4(csColor):
    __swig_setmethods__ = {}
    for _s in [csColor]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csColor4, name, value)
    __swig_getmethods__ = {}
    for _s in [csColor]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csColor4, name)
    __repr__ = _swig_repr
    __swig_setmethods__["alpha"] = _cspace.csColor4_alpha_set
    __swig_getmethods__["alpha"] = _cspace.csColor4_alpha_get
    if _newclass:alpha = property(_cspace.csColor4_alpha_get, _cspace.csColor4_alpha_set)
    def __init__(self, *args): 
        this = _cspace.new_csColor4(*args)
        try: self.this.append(this)
        except: self.this = this
    def Set(*args): return _cspace.csColor4_Set(*args)
    def assign(*args): return _cspace.csColor4_assign(*args)
    def __imul__(*args): return _cspace.csColor4___imul__(*args)
    def __iadd__(*args): return _cspace.csColor4___iadd__(*args)
    def __isub__(*args): return _cspace.csColor4___isub__(*args)
    def __eq__(*args): return _cspace.csColor4___eq__(*args)
    def __ne__(*args): return _cspace.csColor4___ne__(*args)
    __swig_destroy__ = _cspace.delete_csColor4
    __del__ = lambda self : None;
csColor4_swigregister = _cspace.csColor4_swigregister
csColor4_swigregister(csColor4)

class csCommandLineHelper(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csCommandLineHelper, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csCommandLineHelper, name)
    __repr__ = _swig_repr
    __swig_getmethods__["Help"] = lambda x: _cspace.csCommandLineHelper_Help
    if _newclass:Help = staticmethod(_cspace.csCommandLineHelper_Help)
    __swig_getmethods__["CheckHelp"] = lambda x: _cspace.csCommandLineHelper_CheckHelp
    if _newclass:CheckHelp = staticmethod(_cspace.csCommandLineHelper_CheckHelp)
    def __init__(self, *args): 
        this = _cspace.new_csCommandLineHelper(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csCommandLineHelper
    __del__ = lambda self : None;
csCommandLineHelper_swigregister = _cspace.csCommandLineHelper_swigregister
csCommandLineHelper_swigregister(csCommandLineHelper)
csCommandLineHelper_Help = _cspace.csCommandLineHelper_Help
csCommandLineHelper_CheckHelp = _cspace.csCommandLineHelper_CheckHelp

class csStringSet(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csStringSet, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csStringSet, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csStringSet(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csStringSet
    __del__ = lambda self : None;
    def assign(*args): return _cspace.csStringSet_assign(*args)
    def Request(*args): return _cspace.csStringSet_Request(*args)
    def Contains(*args): return _cspace.csStringSet_Contains(*args)
    def Delete(*args): return _cspace.csStringSet_Delete(*args)
    def Empty(*args): return _cspace.csStringSet_Empty(*args)
    def Clear(*args): return _cspace.csStringSet_Clear(*args)
    def GetSize(*args): return _cspace.csStringSet_GetSize(*args)
    def IsEmpty(*args): return _cspace.csStringSet_IsEmpty(*args)
csStringSet_swigregister = _cspace.csStringSet_swigregister
csStringSet_swigregister(csStringSet)

class csStringIDSet(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csStringIDSet, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csStringIDSet, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csStringIDSet(*args)
        try: self.this.append(this)
        except: self.this = this
    def Add(*args): return _cspace.csStringIDSet_Add(*args)
    def AddNoTest(*args): return _cspace.csStringIDSet_AddNoTest(*args)
    def Contains(*args): return _cspace.csStringIDSet_Contains(*args)
    def In(*args): return _cspace.csStringIDSet_In(*args)
    def DeleteAll(*args): return _cspace.csStringIDSet_DeleteAll(*args)
    def Empty(*args): return _cspace.csStringIDSet_Empty(*args)
    def Delete(*args): return _cspace.csStringIDSet_Delete(*args)
    def GetSize(*args): return _cspace.csStringIDSet_GetSize(*args)
    def IsEmpty(*args): return _cspace.csStringIDSet_IsEmpty(*args)
    def __len__(*args): return _cspace.csStringIDSet___len__(*args)
    def __contains__(*args): return _cspace.csStringIDSet___contains__(*args)
    def append(*args): return _cspace.csStringIDSet_append(*args)
    def __delitem__(*args): return _cspace.csStringIDSet___delitem__(*args)
    __swig_destroy__ = _cspace.delete_csStringIDSet
    __del__ = lambda self : None;
csStringIDSet_swigregister = _cspace.csStringIDSet_swigregister
csStringIDSet_swigregister(csStringIDSet)

class iString(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iString, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iString, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetData(*args): return _cspace.iString_GetData(*args)
    def DeleteAt(*args): return _cspace.iString_DeleteAt(*args)
    def StartsWith(*args): return _cspace.iString_StartsWith(*args)
    def __ne__(*args): return _cspace.iString___ne__(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iString_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iString_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iString
    __del__ = lambda self : None;
    def __getitem__(*args): return _cspace.iString___getitem__(*args)
    def __setitem__(*args): return _cspace.iString___setitem__(*args)
iString_swigregister = _cspace.iString_swigregister
iString_swigregister(iString)
iString_scfGetVersion = _cspace.iString_scfGetVersion

class csStringParent(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csStringParent, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csStringParent, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csStringParent(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csStringParent
    __del__ = lambda self : None;
    def assign(*args): return _cspace.csStringParent_assign(*args)
    def GetData(*args): return _cspace.csStringParent_GetData(*args)
    def GetCapacity(*args): return _cspace.csStringParent_GetCapacity(*args)
    def ShrinkBestFit(*args): return _cspace.csStringParent_ShrinkBestFit(*args)
    def Free(*args): return _cspace.csStringParent_Free(*args)
    def Detach(*args): return _cspace.csStringParent_Detach(*args)
csStringParent_swigregister = _cspace.csStringParent_swigregister
csStringParent_swigregister(csStringParent)

class csString(csStringParent):
    __swig_setmethods__ = {}
    for _s in [csStringParent]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csString, name, value)
    __swig_getmethods__ = {}
    for _s in [csStringParent]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csString, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csString(*args)
        try: self.this.append(this)
        except: self.this = this
    def __getitem__(*args): return _cspace.csString___getitem__(*args)
    def __setitem__(*args): return _cspace.csString___setitem__(*args)
    def __delitem__(*args): return _cspace.csString___delitem__(*args)
    __swig_destroy__ = _cspace.delete_csString
    __del__ = lambda self : None;
csString_swigregister = _cspace.csString_swigregister
csString_swigregister(csString)

class csRefCount(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csRefCount, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csRefCount, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csRefCount(*args)
        try: self.this.append(this)
        except: self.this = this
    def IncRef(*args): return _cspace.csRefCount_IncRef(*args)
    def DecRef(*args): return _cspace.csRefCount_DecRef(*args)
    def GetRefCount(*args): return _cspace.csRefCount_GetRefCount(*args)
csRefCount_swigregister = _cspace.csRefCount_swigregister
csRefCount_swigregister(csRefCount)

class csVector2(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csVector2, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csVector2, name)
    __repr__ = _swig_repr
    __swig_setmethods__["x"] = _cspace.csVector2_x_set
    __swig_getmethods__["x"] = _cspace.csVector2_x_get
    if _newclass:x = property(_cspace.csVector2_x_get, _cspace.csVector2_x_set)
    __swig_setmethods__["y"] = _cspace.csVector2_y_set
    __swig_getmethods__["y"] = _cspace.csVector2_y_get
    if _newclass:y = property(_cspace.csVector2_y_get, _cspace.csVector2_y_set)
    def __init__(self, *args): 
        this = _cspace.new_csVector2(*args)
        try: self.this.append(this)
        except: self.this = this
    def Description(*args): return _cspace.csVector2_Description(*args)
    def Set(*args): return _cspace.csVector2_Set(*args)
    def Get(*args): return _cspace.csVector2_Get(*args)
    def Norm(*args): return _cspace.csVector2_Norm(*args)
    def SquaredNorm(*args): return _cspace.csVector2_SquaredNorm(*args)
    def Rotate(*args): return _cspace.csVector2_Rotate(*args)
    def IsLeft(*args): return _cspace.csVector2_IsLeft(*args)
    def __iadd__(*args): return _cspace.csVector2___iadd__(*args)
    def __isub__(*args): return _cspace.csVector2___isub__(*args)
    def __imul__(*args): return _cspace.csVector2___imul__(*args)
    def __idiv__(*args): return _cspace.csVector2___idiv__(*args)
    def __pos__(*args): return _cspace.csVector2___pos__(*args)
    def __neg__(*args): return _cspace.csVector2___neg__(*args)
    def __add__(*args): return _cspace.csVector2___add__(*args)
    def __sub__(*args): return _cspace.csVector2___sub__(*args)
    def __mul__(*args): return _cspace.csVector2___mul__(*args)
    def __div__(*args): return _cspace.csVector2___div__(*args)
    def __eq__(*args): return _cspace.csVector2___eq__(*args)
    def __ne__(*args): return _cspace.csVector2___ne__(*args)
    def __lt__(*args): return _cspace.csVector2___lt__(*args)
    def __gt__(*args): return _cspace.csVector2___gt__(*args)
    def __rmul__(*args): return _cspace.csVector2___rmul__(*args)
    def __abs__(*args): return _cspace.csVector2___abs__(*args)
    def __getitem__(*args): return _cspace.csVector2___getitem__(*args)
    def __setitem__(*args): return _cspace.csVector2___setitem__(*args)
    __swig_destroy__ = _cspace.delete_csVector2
    __del__ = lambda self : None;
csVector2_swigregister = _cspace.csVector2_swigregister
csVector2_swigregister(csVector2)

CS_AXIS_NONE = _cspace.CS_AXIS_NONE
CS_AXIS_X = _cspace.CS_AXIS_X
CS_AXIS_Y = _cspace.CS_AXIS_Y
CS_AXIS_Z = _cspace.CS_AXIS_Z
CS_AXIS_W = _cspace.CS_AXIS_W
class csVector3(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csVector3, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csVector3, name)
    __repr__ = _swig_repr
    __swig_setmethods__["x"] = _cspace.csVector3_x_set
    __swig_getmethods__["x"] = _cspace.csVector3_x_get
    if _newclass:x = property(_cspace.csVector3_x_get, _cspace.csVector3_x_set)
    __swig_setmethods__["y"] = _cspace.csVector3_y_set
    __swig_getmethods__["y"] = _cspace.csVector3_y_get
    if _newclass:y = property(_cspace.csVector3_y_get, _cspace.csVector3_y_set)
    __swig_setmethods__["z"] = _cspace.csVector3_z_set
    __swig_getmethods__["z"] = _cspace.csVector3_z_get
    if _newclass:z = property(_cspace.csVector3_z_get, _cspace.csVector3_z_set)
    def __init__(self, *args): 
        this = _cspace.new_csVector3(*args)
        try: self.this.append(this)
        except: self.this = this
    def Description(*args): return _cspace.csVector3_Description(*args)
    def Cross(*args): return _cspace.csVector3_Cross(*args)
    def __iadd__(*args): return _cspace.csVector3___iadd__(*args)
    def __isub__(*args): return _cspace.csVector3___isub__(*args)
    def __pos__(*args): return _cspace.csVector3___pos__(*args)
    def __neg__(*args): return _cspace.csVector3___neg__(*args)
    def Set(*args): return _cspace.csVector3_Set(*args)
    def Get(*args): return _cspace.csVector3_Get(*args)
    def Norm(*args): return _cspace.csVector3_Norm(*args)
    def SquaredNorm(*args): return _cspace.csVector3_SquaredNorm(*args)
    def Unit(*args): return _cspace.csVector3_Unit(*args)
    def Normalize(*args): return _cspace.csVector3_Normalize(*args)
    def IsZero(*args): return _cspace.csVector3_IsZero(*args)
    def UnitAxisClamped(*args): return _cspace.csVector3_UnitAxisClamped(*args)
    def DominantAxis(*args): return _cspace.csVector3_DominantAxis(*args)
    def __add__(*args): return _cspace.csVector3___add__(*args)
    def __sub__(*args): return _cspace.csVector3___sub__(*args)
    def __mul__(*args): return _cspace.csVector3___mul__(*args)
    def __eq__(*args): return _cspace.csVector3___eq__(*args)
    def __ne__(*args): return _cspace.csVector3___ne__(*args)
    def __lt__(*args): return _cspace.csVector3___lt__(*args)
    def __gt__(*args): return _cspace.csVector3___gt__(*args)
    def __imul__(*args): return _cspace.csVector3___imul__(*args)
    def __idiv__(*args): return _cspace.csVector3___idiv__(*args)
    def __div__(*args): return _cspace.csVector3___div__(*args)
    def project(*args): return _cspace.csVector3_project(*args)
    def __rmul__(*args): return _cspace.csVector3___rmul__(*args)
    def __abs__(*args): return _cspace.csVector3___abs__(*args)
    def __getitem__(*args): return _cspace.csVector3___getitem__(*args)
    def __setitem__(*args): return _cspace.csVector3___setitem__(*args)
    def __nonzero__(*args): return _cspace.csVector3___nonzero__(*args)
    __swig_destroy__ = _cspace.delete_csVector3
    __del__ = lambda self : None;
csVector3_swigregister = _cspace.csVector3_swigregister
csVector3_swigregister(csVector3)

class csVector4Float(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csVector4Float, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csVector4Float, name)
    __repr__ = _swig_repr
    __swig_setmethods__["x"] = _cspace.csVector4Float_x_set
    __swig_getmethods__["x"] = _cspace.csVector4Float_x_get
    if _newclass:x = property(_cspace.csVector4Float_x_get, _cspace.csVector4Float_x_set)
    __swig_setmethods__["y"] = _cspace.csVector4Float_y_set
    __swig_getmethods__["y"] = _cspace.csVector4Float_y_get
    if _newclass:y = property(_cspace.csVector4Float_y_get, _cspace.csVector4Float_y_set)
    __swig_setmethods__["z"] = _cspace.csVector4Float_z_set
    __swig_getmethods__["z"] = _cspace.csVector4Float_z_get
    if _newclass:z = property(_cspace.csVector4Float_z_get, _cspace.csVector4Float_z_set)
    __swig_setmethods__["w"] = _cspace.csVector4Float_w_set
    __swig_getmethods__["w"] = _cspace.csVector4Float_w_get
    if _newclass:w = property(_cspace.csVector4Float_w_get, _cspace.csVector4Float_w_set)
    def __init__(self, *args): 
        this = _cspace.new_csVector4Float(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csVector4Float
    __del__ = lambda self : None;
csVector4Float_swigregister = _cspace.csVector4Float_swigregister
csVector4Float_swigregister(csVector4Float)

class csVector4(csVector4Float):
    __swig_setmethods__ = {}
    for _s in [csVector4Float]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csVector4, name, value)
    __swig_getmethods__ = {}
    for _s in [csVector4Float]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csVector4, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csVector4(*args)
        try: self.this.append(this)
        except: self.this = this
    def assign(*args): return _cspace.csVector4_assign(*args)
    def __add__(*args): return _cspace.csVector4___add__(*args)
    def __sub__(*args): return _cspace.csVector4___sub__(*args)
    def __mul__(*args): return _cspace.csVector4___mul__(*args)
    def __div__(*args): return _cspace.csVector4___div__(*args)
    def __eq__(*args): return _cspace.csVector4___eq__(*args)
    def __ne__(*args): return _cspace.csVector4___ne__(*args)
    def __lt__(*args): return _cspace.csVector4___lt__(*args)
    def __gt__(*args): return _cspace.csVector4___gt__(*args)
    def __rmul__(*args): return _cspace.csVector4___rmul__(*args)
    def __abs__(*args): return _cspace.csVector4___abs__(*args)
    def __getitem__(*args): return _cspace.csVector4___getitem__(*args)
    def __setitem__(*args): return _cspace.csVector4___setitem__(*args)
    def __nonzero__(*args): return _cspace.csVector4___nonzero__(*args)
    __swig_destroy__ = _cspace.delete_csVector4
    __del__ = lambda self : None;
csVector4_swigregister = _cspace.csVector4_swigregister
csVector4_swigregister(csVector4)

class csMatrix2(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csMatrix2, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csMatrix2, name)
    __repr__ = _swig_repr
    __swig_setmethods__["m11"] = _cspace.csMatrix2_m11_set
    __swig_getmethods__["m11"] = _cspace.csMatrix2_m11_get
    if _newclass:m11 = property(_cspace.csMatrix2_m11_get, _cspace.csMatrix2_m11_set)
    __swig_setmethods__["m12"] = _cspace.csMatrix2_m12_set
    __swig_getmethods__["m12"] = _cspace.csMatrix2_m12_get
    if _newclass:m12 = property(_cspace.csMatrix2_m12_get, _cspace.csMatrix2_m12_set)
    __swig_setmethods__["m21"] = _cspace.csMatrix2_m21_set
    __swig_getmethods__["m21"] = _cspace.csMatrix2_m21_get
    if _newclass:m21 = property(_cspace.csMatrix2_m21_get, _cspace.csMatrix2_m21_set)
    __swig_setmethods__["m22"] = _cspace.csMatrix2_m22_set
    __swig_getmethods__["m22"] = _cspace.csMatrix2_m22_get
    if _newclass:m22 = property(_cspace.csMatrix2_m22_get, _cspace.csMatrix2_m22_set)
    def __init__(self, *args): 
        this = _cspace.new_csMatrix2(*args)
        try: self.this.append(this)
        except: self.this = this
    def Row1(*args): return _cspace.csMatrix2_Row1(*args)
    def Row2(*args): return _cspace.csMatrix2_Row2(*args)
    def Col1(*args): return _cspace.csMatrix2_Col1(*args)
    def Col2(*args): return _cspace.csMatrix2_Col2(*args)
    def Set(*args): return _cspace.csMatrix2_Set(*args)
    def __iadd__(*args): return _cspace.csMatrix2___iadd__(*args)
    def __isub__(*args): return _cspace.csMatrix2___isub__(*args)
    def __imul__(*args): return _cspace.csMatrix2___imul__(*args)
    def __idiv__(*args): return _cspace.csMatrix2___idiv__(*args)
    def __pos__(*args): return _cspace.csMatrix2___pos__(*args)
    def __neg__(*args): return _cspace.csMatrix2___neg__(*args)
    def Transpose(*args): return _cspace.csMatrix2_Transpose(*args)
    def GetTranspose(*args): return _cspace.csMatrix2_GetTranspose(*args)
    def GetInverse(*args): return _cspace.csMatrix2_GetInverse(*args)
    def Invert(*args): return _cspace.csMatrix2_Invert(*args)
    def Determinant(*args): return _cspace.csMatrix2_Determinant(*args)
    def Identity(*args): return _cspace.csMatrix2_Identity(*args)
    __swig_destroy__ = _cspace.delete_csMatrix2
    __del__ = lambda self : None;
csMatrix2_swigregister = _cspace.csMatrix2_swigregister
csMatrix2_swigregister(csMatrix2)

class csMatrix3(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csMatrix3, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csMatrix3, name)
    __repr__ = _swig_repr
    __swig_setmethods__["m11"] = _cspace.csMatrix3_m11_set
    __swig_getmethods__["m11"] = _cspace.csMatrix3_m11_get
    if _newclass:m11 = property(_cspace.csMatrix3_m11_get, _cspace.csMatrix3_m11_set)
    __swig_setmethods__["m12"] = _cspace.csMatrix3_m12_set
    __swig_getmethods__["m12"] = _cspace.csMatrix3_m12_get
    if _newclass:m12 = property(_cspace.csMatrix3_m12_get, _cspace.csMatrix3_m12_set)
    __swig_setmethods__["m13"] = _cspace.csMatrix3_m13_set
    __swig_getmethods__["m13"] = _cspace.csMatrix3_m13_get
    if _newclass:m13 = property(_cspace.csMatrix3_m13_get, _cspace.csMatrix3_m13_set)
    __swig_setmethods__["m21"] = _cspace.csMatrix3_m21_set
    __swig_getmethods__["m21"] = _cspace.csMatrix3_m21_get
    if _newclass:m21 = property(_cspace.csMatrix3_m21_get, _cspace.csMatrix3_m21_set)
    __swig_setmethods__["m22"] = _cspace.csMatrix3_m22_set
    __swig_getmethods__["m22"] = _cspace.csMatrix3_m22_get
    if _newclass:m22 = property(_cspace.csMatrix3_m22_get, _cspace.csMatrix3_m22_set)
    __swig_setmethods__["m23"] = _cspace.csMatrix3_m23_set
    __swig_getmethods__["m23"] = _cspace.csMatrix3_m23_get
    if _newclass:m23 = property(_cspace.csMatrix3_m23_get, _cspace.csMatrix3_m23_set)
    __swig_setmethods__["m31"] = _cspace.csMatrix3_m31_set
    __swig_getmethods__["m31"] = _cspace.csMatrix3_m31_get
    if _newclass:m31 = property(_cspace.csMatrix3_m31_get, _cspace.csMatrix3_m31_set)
    __swig_setmethods__["m32"] = _cspace.csMatrix3_m32_set
    __swig_getmethods__["m32"] = _cspace.csMatrix3_m32_get
    if _newclass:m32 = property(_cspace.csMatrix3_m32_get, _cspace.csMatrix3_m32_set)
    __swig_setmethods__["m33"] = _cspace.csMatrix3_m33_set
    __swig_getmethods__["m33"] = _cspace.csMatrix3_m33_get
    if _newclass:m33 = property(_cspace.csMatrix3_m33_get, _cspace.csMatrix3_m33_set)
    def __init__(self, *args): 
        this = _cspace.new_csMatrix3(*args)
        try: self.this.append(this)
        except: self.this = this
    def Description(*args): return _cspace.csMatrix3_Description(*args)
    def Row1(*args): return _cspace.csMatrix3_Row1(*args)
    def Row2(*args): return _cspace.csMatrix3_Row2(*args)
    def Row3(*args): return _cspace.csMatrix3_Row3(*args)
    def Col1(*args): return _cspace.csMatrix3_Col1(*args)
    def Col2(*args): return _cspace.csMatrix3_Col2(*args)
    def Col3(*args): return _cspace.csMatrix3_Col3(*args)
    def Set(*args): return _cspace.csMatrix3_Set(*args)
    def assign(*args): return _cspace.csMatrix3_assign(*args)
    def __iadd__(*args): return _cspace.csMatrix3___iadd__(*args)
    def __isub__(*args): return _cspace.csMatrix3___isub__(*args)
    def __idiv__(*args): return _cspace.csMatrix3___idiv__(*args)
    def __pos__(*args): return _cspace.csMatrix3___pos__(*args)
    def __neg__(*args): return _cspace.csMatrix3___neg__(*args)
    def Transpose(*args): return _cspace.csMatrix3_Transpose(*args)
    def GetTranspose(*args): return _cspace.csMatrix3_GetTranspose(*args)
    def GetInverse(*args): return _cspace.csMatrix3_GetInverse(*args)
    def Invert(*args): return _cspace.csMatrix3_Invert(*args)
    def Determinant(*args): return _cspace.csMatrix3_Determinant(*args)
    def Identity(*args): return _cspace.csMatrix3_Identity(*args)
    def IsIdentity(*args): return _cspace.csMatrix3_IsIdentity(*args)
    def __add__(*args): return _cspace.csMatrix3___add__(*args)
    def __sub__(*args): return _cspace.csMatrix3___sub__(*args)
    def __div__(*args): return _cspace.csMatrix3___div__(*args)
    def __eq__(*args): return _cspace.csMatrix3___eq__(*args)
    def __ne__(*args): return _cspace.csMatrix3___ne__(*args)
    def __lt__(*args): return _cspace.csMatrix3___lt__(*args)
    def __mul__(*args): return _cspace.csMatrix3___mul__(*args)
    def __imul__(*args): return _cspace.csMatrix3___imul__(*args)
    def __rmul__(*args): return _cspace.csMatrix3___rmul__(*args)
    __swig_destroy__ = _cspace.delete_csMatrix3
    __del__ = lambda self : None;
csMatrix3_swigregister = _cspace.csMatrix3_swigregister
csMatrix3_swigregister(csMatrix3)

class csXRotMatrix3(csMatrix3):
    __swig_setmethods__ = {}
    for _s in [csMatrix3]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csXRotMatrix3, name, value)
    __swig_getmethods__ = {}
    for _s in [csMatrix3]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csXRotMatrix3, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csXRotMatrix3(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csXRotMatrix3
    __del__ = lambda self : None;
csXRotMatrix3_swigregister = _cspace.csXRotMatrix3_swigregister
csXRotMatrix3_swigregister(csXRotMatrix3)

class csYRotMatrix3(csMatrix3):
    __swig_setmethods__ = {}
    for _s in [csMatrix3]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csYRotMatrix3, name, value)
    __swig_getmethods__ = {}
    for _s in [csMatrix3]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csYRotMatrix3, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csYRotMatrix3(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csYRotMatrix3
    __del__ = lambda self : None;
csYRotMatrix3_swigregister = _cspace.csYRotMatrix3_swigregister
csYRotMatrix3_swigregister(csYRotMatrix3)

class csZRotMatrix3(csMatrix3):
    __swig_setmethods__ = {}
    for _s in [csMatrix3]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csZRotMatrix3, name, value)
    __swig_getmethods__ = {}
    for _s in [csMatrix3]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csZRotMatrix3, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csZRotMatrix3(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csZRotMatrix3
    __del__ = lambda self : None;
csZRotMatrix3_swigregister = _cspace.csZRotMatrix3_swigregister
csZRotMatrix3_swigregister(csZRotMatrix3)

class csXScaleMatrix3(csMatrix3):
    __swig_setmethods__ = {}
    for _s in [csMatrix3]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csXScaleMatrix3, name, value)
    __swig_getmethods__ = {}
    for _s in [csMatrix3]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csXScaleMatrix3, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csXScaleMatrix3(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csXScaleMatrix3
    __del__ = lambda self : None;
csXScaleMatrix3_swigregister = _cspace.csXScaleMatrix3_swigregister
csXScaleMatrix3_swigregister(csXScaleMatrix3)

class csYScaleMatrix3(csMatrix3):
    __swig_setmethods__ = {}
    for _s in [csMatrix3]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csYScaleMatrix3, name, value)
    __swig_getmethods__ = {}
    for _s in [csMatrix3]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csYScaleMatrix3, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csYScaleMatrix3(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csYScaleMatrix3
    __del__ = lambda self : None;
csYScaleMatrix3_swigregister = _cspace.csYScaleMatrix3_swigregister
csYScaleMatrix3_swigregister(csYScaleMatrix3)

class csZScaleMatrix3(csMatrix3):
    __swig_setmethods__ = {}
    for _s in [csMatrix3]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csZScaleMatrix3, name, value)
    __swig_getmethods__ = {}
    for _s in [csMatrix3]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csZScaleMatrix3, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csZScaleMatrix3(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csZScaleMatrix3
    __del__ = lambda self : None;
csZScaleMatrix3_swigregister = _cspace.csZScaleMatrix3_swigregister
csZScaleMatrix3_swigregister(csZScaleMatrix3)

class csTransform(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csTransform, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csTransform, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_csTransform
    __del__ = lambda self : None;
    def __init__(self, *args): 
        this = _cspace.new_csTransform(*args)
        try: self.this.append(this)
        except: self.this = this
    def Description(*args): return _cspace.csTransform_Description(*args)
    def Identity(*args): return _cspace.csTransform_Identity(*args)
    def IsIdentity(*args): return _cspace.csTransform_IsIdentity(*args)
    def GetO2T(*args): return _cspace.csTransform_GetO2T(*args)
    def GetO2TTranslation(*args): return _cspace.csTransform_GetO2TTranslation(*args)
    def GetOrigin(*args): return _cspace.csTransform_GetOrigin(*args)
    def SetO2T(*args): return _cspace.csTransform_SetO2T(*args)
    def SetO2TTranslation(*args): return _cspace.csTransform_SetO2TTranslation(*args)
    def SetOrigin(*args): return _cspace.csTransform_SetOrigin(*args)
    def Translate(*args): return _cspace.csTransform_Translate(*args)
    def Other2ThisRelative(*args): return _cspace.csTransform_Other2ThisRelative(*args)
    def Other2This(*args): return _cspace.csTransform_Other2This(*args)
    __swig_getmethods__["GetReflect"] = lambda x: _cspace.csTransform_GetReflect
    if _newclass:GetReflect = staticmethod(_cspace.csTransform_GetReflect)
    def GetFront(*args): return _cspace.csTransform_GetFront(*args)
    def GetUp(*args): return _cspace.csTransform_GetUp(*args)
    def GetRight(*args): return _cspace.csTransform_GetRight(*args)
    def __mul__(*args): return _cspace.csTransform___mul__(*args)
    def __rmul__(*args): return _cspace.csTransform___rmul__(*args)
csTransform_swigregister = _cspace.csTransform_swigregister
csTransform_swigregister(csTransform)
csTransform_GetReflect = _cspace.csTransform_GetReflect

class csReversibleTransform(csTransform):
    __swig_setmethods__ = {}
    for _s in [csTransform]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csReversibleTransform, name, value)
    __swig_getmethods__ = {}
    for _s in [csTransform]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csReversibleTransform, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csReversibleTransform(*args)
        try: self.this.append(this)
        except: self.this = this
    def GetT2O(*args): return _cspace.csReversibleTransform_GetT2O(*args)
    def GetT2OTranslation(*args): return _cspace.csReversibleTransform_GetT2OTranslation(*args)
    def GetInverse(*args): return _cspace.csReversibleTransform_GetInverse(*args)
    def SetO2T(*args): return _cspace.csReversibleTransform_SetO2T(*args)
    def SetT2O(*args): return _cspace.csReversibleTransform_SetT2O(*args)
    def This2OtherRelative(*args): return _cspace.csReversibleTransform_This2OtherRelative(*args)
    def This2Other(*args): return _cspace.csReversibleTransform_This2Other(*args)
    def RotateOther(*args): return _cspace.csReversibleTransform_RotateOther(*args)
    def RotateThis(*args): return _cspace.csReversibleTransform_RotateThis(*args)
    def LookAt(*args): return _cspace.csReversibleTransform_LookAt(*args)
    def __imul__(*args): return _cspace.csReversibleTransform___imul__(*args)
    def __mul__(*args): return _cspace.csReversibleTransform___mul__(*args)
    def __idiv__(*args): return _cspace.csReversibleTransform___idiv__(*args)
    def __div__(*args): return _cspace.csReversibleTransform___div__(*args)
    __swig_destroy__ = _cspace.delete_csReversibleTransform
    __del__ = lambda self : None;
csReversibleTransform_swigregister = _cspace.csReversibleTransform_swigregister
csReversibleTransform_swigregister(csReversibleTransform)

class csOrthoTransform(csReversibleTransform):
    __swig_setmethods__ = {}
    for _s in [csReversibleTransform]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csOrthoTransform, name, value)
    __swig_getmethods__ = {}
    for _s in [csReversibleTransform]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csOrthoTransform, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csOrthoTransform(*args)
        try: self.this.append(this)
        except: self.this = this
    def SetO2T(*args): return _cspace.csOrthoTransform_SetO2T(*args)
    def SetT2O(*args): return _cspace.csOrthoTransform_SetT2O(*args)
    __swig_destroy__ = _cspace.delete_csOrthoTransform
    __del__ = lambda self : None;
csOrthoTransform_swigregister = _cspace.csOrthoTransform_swigregister
csOrthoTransform_swigregister(csOrthoTransform)

class csSphere(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSphere, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csSphere, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csSphere(*args)
        try: self.this.append(this)
        except: self.this = this
    def GetCenter(*args): return _cspace.csSphere_GetCenter(*args)
    def SetCenter(*args): return _cspace.csSphere_SetCenter(*args)
    def GetRadius(*args): return _cspace.csSphere_GetRadius(*args)
    def SetRadius(*args): return _cspace.csSphere_SetRadius(*args)
    def Union(*args): return _cspace.csSphere_Union(*args)
    def __iadd__(*args): return _cspace.csSphere___iadd__(*args)
    def TestIntersect(*args): return _cspace.csSphere_TestIntersect(*args)
    def __imul__(*args): return _cspace.csSphere___imul__(*args)
    def __div__(*args): return _cspace.csSphere___div__(*args)
    __swig_destroy__ = _cspace.delete_csSphere
    __del__ = lambda self : None;
csSphere_swigregister = _cspace.csSphere_swigregister
csSphere_swigregister(csSphere)

class csEllipsoid(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csEllipsoid, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csEllipsoid, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csEllipsoid(*args)
        try: self.this.append(this)
        except: self.this = this
    def GetCenter(*args): return _cspace.csEllipsoid_GetCenter(*args)
    def SetCenter(*args): return _cspace.csEllipsoid_SetCenter(*args)
    def GetRadius(*args): return _cspace.csEllipsoid_GetRadius(*args)
    def SetRadius(*args): return _cspace.csEllipsoid_SetRadius(*args)
    __swig_destroy__ = _cspace.delete_csEllipsoid
    __del__ = lambda self : None;
csEllipsoid_swigregister = _cspace.csEllipsoid_swigregister
csEllipsoid_swigregister(csEllipsoid)

CS_POLY_IN = _cspace.CS_POLY_IN
CS_POLY_ON = _cspace.CS_POLY_ON
CS_POLY_OUT = _cspace.CS_POLY_OUT
class csPlane2(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPlane2, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csPlane2, name)
    __repr__ = _swig_repr
    __swig_setmethods__["norm"] = _cspace.csPlane2_norm_set
    __swig_getmethods__["norm"] = _cspace.csPlane2_norm_get
    if _newclass:norm = property(_cspace.csPlane2_norm_get, _cspace.csPlane2_norm_set)
    __swig_setmethods__["CC"] = _cspace.csPlane2_CC_set
    __swig_getmethods__["CC"] = _cspace.csPlane2_CC_get
    if _newclass:CC = property(_cspace.csPlane2_CC_get, _cspace.csPlane2_CC_set)
    def __init__(self, *args): 
        this = _cspace.new_csPlane2(*args)
        try: self.this.append(this)
        except: self.this = this
    def Normal(*args): return _cspace.csPlane2_Normal(*args)
    def GetNormal(*args): return _cspace.csPlane2_GetNormal(*args)
    def A(*args): return _cspace.csPlane2_A(*args)
    def B(*args): return _cspace.csPlane2_B(*args)
    def C(*args): return _cspace.csPlane2_C(*args)
    def Set(*args): return _cspace.csPlane2_Set(*args)
    __swig_getmethods__["Classify"] = lambda x: _cspace.csPlane2_Classify
    if _newclass:Classify = staticmethod(_cspace.csPlane2_Classify)
    def Distance(*args): return _cspace.csPlane2_Distance(*args)
    def SquaredDistance(*args): return _cspace.csPlane2_SquaredDistance(*args)
    def Invert(*args): return _cspace.csPlane2_Invert(*args)
    def Normalize(*args): return _cspace.csPlane2_Normalize(*args)
    __swig_destroy__ = _cspace.delete_csPlane2
    __del__ = lambda self : None;
csPlane2_swigregister = _cspace.csPlane2_swigregister
csPlane2_swigregister(csPlane2)
csPlane2_Classify = _cspace.csPlane2_Classify

class csPlane3(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPlane3, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csPlane3, name)
    __repr__ = _swig_repr
    __swig_setmethods__["norm"] = _cspace.csPlane3_norm_set
    __swig_getmethods__["norm"] = _cspace.csPlane3_norm_get
    if _newclass:norm = property(_cspace.csPlane3_norm_get, _cspace.csPlane3_norm_set)
    __swig_setmethods__["DD"] = _cspace.csPlane3_DD_set
    __swig_getmethods__["DD"] = _cspace.csPlane3_DD_get
    if _newclass:DD = property(_cspace.csPlane3_DD_get, _cspace.csPlane3_DD_set)
    def __init__(self, *args): 
        this = _cspace.new_csPlane3(*args)
        try: self.this.append(this)
        except: self.this = this
    def __eq__(*args): return _cspace.csPlane3___eq__(*args)
    def Normal(*args): return _cspace.csPlane3_Normal(*args)
    def A(*args): return _cspace.csPlane3_A(*args)
    def B(*args): return _cspace.csPlane3_B(*args)
    def C(*args): return _cspace.csPlane3_C(*args)
    def D(*args): return _cspace.csPlane3_D(*args)
    def GetNormal(*args): return _cspace.csPlane3_GetNormal(*args)
    def Set(*args): return _cspace.csPlane3_Set(*args)
    def SetOrigin(*args): return _cspace.csPlane3_SetOrigin(*args)
    __swig_getmethods__["Classify"] = lambda x: _cspace.csPlane3_Classify
    if _newclass:Classify = staticmethod(_cspace.csPlane3_Classify)
    def Distance(*args): return _cspace.csPlane3_Distance(*args)
    def Invert(*args): return _cspace.csPlane3_Invert(*args)
    def Normalize(*args): return _cspace.csPlane3_Normalize(*args)
    def FindPoint(*args): return _cspace.csPlane3_FindPoint(*args)
    __swig_getmethods__["FindOrthogonalPoints"] = lambda x: _cspace.csPlane3_FindOrthogonalPoints
    if _newclass:FindOrthogonalPoints = staticmethod(_cspace.csPlane3_FindOrthogonalPoints)
    def ClipPolygon(*args): return _cspace.csPlane3_ClipPolygon(*args)
    def Description(*args): return _cspace.csPlane3_Description(*args)
    def __imul__(*args): return _cspace.csPlane3___imul__(*args)
    def __idiv__(*args): return _cspace.csPlane3___idiv__(*args)
    def __div__(*args): return _cspace.csPlane3___div__(*args)
    __swig_destroy__ = _cspace.delete_csPlane3
    __del__ = lambda self : None;
csPlane3_swigregister = _cspace.csPlane3_swigregister
csPlane3_swigregister(csPlane3)
csPlane3_Classify = _cspace.csPlane3_Classify
csPlane3_FindOrthogonalPoints = _cspace.csPlane3_FindOrthogonalPoints

class csMath2(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csMath2, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csMath2, name)
    __repr__ = _swig_repr
    __swig_getmethods__["WhichSide2D"] = lambda x: _cspace.csMath2_WhichSide2D
    if _newclass:WhichSide2D = staticmethod(_cspace.csMath2_WhichSide2D)
    __swig_getmethods__["InPoly2D"] = lambda x: _cspace.csMath2_InPoly2D
    if _newclass:InPoly2D = staticmethod(_cspace.csMath2_InPoly2D)
    __swig_getmethods__["Area2"] = lambda x: _cspace.csMath2_Area2
    if _newclass:Area2 = staticmethod(_cspace.csMath2_Area2)
    __swig_getmethods__["Right"] = lambda x: _cspace.csMath2_Right
    if _newclass:Right = staticmethod(_cspace.csMath2_Right)
    __swig_getmethods__["Left"] = lambda x: _cspace.csMath2_Left
    if _newclass:Left = staticmethod(_cspace.csMath2_Left)
    __swig_getmethods__["Visible"] = lambda x: _cspace.csMath2_Visible
    if _newclass:Visible = staticmethod(_cspace.csMath2_Visible)
    __swig_getmethods__["PlanesEqual"] = lambda x: _cspace.csMath2_PlanesEqual
    if _newclass:PlanesEqual = staticmethod(_cspace.csMath2_PlanesEqual)
    __swig_getmethods__["PlanesClose"] = lambda x: _cspace.csMath2_PlanesClose
    if _newclass:PlanesClose = staticmethod(_cspace.csMath2_PlanesClose)
    def __init__(self, *args): 
        this = _cspace.new_csMath2(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csMath2
    __del__ = lambda self : None;
csMath2_swigregister = _cspace.csMath2_swigregister
csMath2_swigregister(csMath2)
csMath2_WhichSide2D = _cspace.csMath2_WhichSide2D
csMath2_InPoly2D = _cspace.csMath2_InPoly2D
csMath2_Area2 = _cspace.csMath2_Area2
csMath2_Right = _cspace.csMath2_Right
csMath2_Left = _cspace.csMath2_Left
csMath2_Visible = _cspace.csMath2_Visible
csMath2_PlanesEqual = _cspace.csMath2_PlanesEqual
csMath2_PlanesClose = _cspace.csMath2_PlanesClose

class csIntersect2(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csIntersect2, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csIntersect2, name)
    __repr__ = _swig_repr
    __swig_getmethods__["PlanePolygon"] = lambda x: _cspace.csIntersect2_PlanePolygon
    if _newclass:PlanePolygon = staticmethod(_cspace.csIntersect2_PlanePolygon)
    __swig_getmethods__["SegmentSegment"] = lambda x: _cspace.csIntersect2_SegmentSegment
    if _newclass:SegmentSegment = staticmethod(_cspace.csIntersect2_SegmentSegment)
    __swig_getmethods__["SegmentLine"] = lambda x: _cspace.csIntersect2_SegmentLine
    if _newclass:SegmentLine = staticmethod(_cspace.csIntersect2_SegmentLine)
    __swig_getmethods__["LineLine"] = lambda x: _cspace.csIntersect2_LineLine
    if _newclass:LineLine = staticmethod(_cspace.csIntersect2_LineLine)
    __swig_getmethods__["SegmentPlane"] = lambda x: _cspace.csIntersect2_SegmentPlane
    if _newclass:SegmentPlane = staticmethod(_cspace.csIntersect2_SegmentPlane)
    __swig_getmethods__["SegmentPlaneNoTest"] = lambda x: _cspace.csIntersect2_SegmentPlaneNoTest
    if _newclass:SegmentPlaneNoTest = staticmethod(_cspace.csIntersect2_SegmentPlaneNoTest)
    __swig_getmethods__["PlanePlane"] = lambda x: _cspace.csIntersect2_PlanePlane
    if _newclass:PlanePlane = staticmethod(_cspace.csIntersect2_PlanePlane)
    __swig_getmethods__["SegmentBox"] = lambda x: _cspace.csIntersect2_SegmentBox
    if _newclass:SegmentBox = staticmethod(_cspace.csIntersect2_SegmentBox)
    def __init__(self, *args): 
        this = _cspace.new_csIntersect2(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csIntersect2
    __del__ = lambda self : None;
csIntersect2_swigregister = _cspace.csIntersect2_swigregister
csIntersect2_swigregister(csIntersect2)
csIntersect2_PlanePolygon = _cspace.csIntersect2_PlanePolygon
csIntersect2_SegmentSegment = _cspace.csIntersect2_SegmentSegment
csIntersect2_SegmentLine = _cspace.csIntersect2_SegmentLine
csIntersect2_LineLine = _cspace.csIntersect2_LineLine
csIntersect2_SegmentPlane = _cspace.csIntersect2_SegmentPlane
csIntersect2_SegmentPlaneNoTest = _cspace.csIntersect2_SegmentPlaneNoTest
csIntersect2_PlanePlane = _cspace.csIntersect2_PlanePlane
csIntersect2_SegmentBox = _cspace.csIntersect2_SegmentBox

class csPoly2D(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPoly2D, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csPoly2D, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csPoly2D(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csPoly2D
    __del__ = lambda self : None;
    def assign(*args): return _cspace.csPoly2D_assign(*args)
    def MakeEmpty(*args): return _cspace.csPoly2D_MakeEmpty(*args)
    def GetVertexCount(*args): return _cspace.csPoly2D_GetVertexCount(*args)
    def GetVertices(*args): return _cspace.csPoly2D_GetVertices(*args)
    def GetVertex(*args): return _cspace.csPoly2D_GetVertex(*args)
    def GetFirst(*args): return _cspace.csPoly2D_GetFirst(*args)
    def GetLast(*args): return _cspace.csPoly2D_GetLast(*args)
    __swig_getmethods__["In"] = lambda x: _cspace.csPoly2D_In
    if _newclass:In = staticmethod(_cspace.csPoly2D_In)
    def MakeRoom(*args): return _cspace.csPoly2D_MakeRoom(*args)
    def SetVertexCount(*args): return _cspace.csPoly2D_SetVertexCount(*args)
    def AddVertex(*args): return _cspace.csPoly2D_AddVertex(*args)
    def SetVertices(*args): return _cspace.csPoly2D_SetVertices(*args)
    def ClipAgainst(*args): return _cspace.csPoly2D_ClipAgainst(*args)
    def Intersect(*args): return _cspace.csPoly2D_Intersect(*args)
    def ClipPlane(*args): return _cspace.csPoly2D_ClipPlane(*args)
    def ExtendConvex(*args): return _cspace.csPoly2D_ExtendConvex(*args)
    def GetSignedArea(*args): return _cspace.csPoly2D_GetSignedArea(*args)
    def Random(*args): return _cspace.csPoly2D_Random(*args)
    def __getitem__(*args): return _cspace.csPoly2D___getitem__(*args)
    def __setitem__ (self, i, v):
      own_v = self.__getitem__(i)
      for i in range(2):
        own_v[i] = v[i]

csPoly2D_swigregister = _cspace.csPoly2D_swigregister
csPoly2D_swigregister(csPoly2D)
csPoly2D_In = _cspace.csPoly2D_In

class csPoly2DFactory(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPoly2DFactory, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csPoly2DFactory, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_csPoly2DFactory
    __del__ = lambda self : None;
    def Create(*args): return _cspace.csPoly2DFactory_Create(*args)
    def __init__(self, *args): 
        this = _cspace.new_csPoly2DFactory(*args)
        try: self.this.append(this)
        except: self.this = this
csPoly2DFactory_swigregister = _cspace.csPoly2DFactory_swigregister
csPoly2DFactory_swigregister(csPoly2DFactory)

class csMath3(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csMath3, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csMath3, name)
    __repr__ = _swig_repr
    __swig_getmethods__["WhichSide3D"] = lambda x: _cspace.csMath3_WhichSide3D
    if _newclass:WhichSide3D = staticmethod(_cspace.csMath3_WhichSide3D)
    __swig_getmethods__["Visible"] = lambda x: _cspace.csMath3_Visible
    if _newclass:Visible = staticmethod(_cspace.csMath3_Visible)
    __swig_getmethods__["Between"] = lambda x: _cspace.csMath3_Between
    if _newclass:Between = staticmethod(_cspace.csMath3_Between)
    __swig_getmethods__["SetMinMax"] = lambda x: _cspace.csMath3_SetMinMax
    if _newclass:SetMinMax = staticmethod(_cspace.csMath3_SetMinMax)
    __swig_getmethods__["DoubleArea3"] = lambda x: _cspace.csMath3_DoubleArea3
    if _newclass:DoubleArea3 = staticmethod(_cspace.csMath3_DoubleArea3)
    __swig_getmethods__["Direction3"] = lambda x: _cspace.csMath3_Direction3
    if _newclass:Direction3 = staticmethod(_cspace.csMath3_Direction3)
    __swig_getmethods__["CalcNormal"] = lambda x: _cspace.csMath3_CalcNormal
    if _newclass:CalcNormal = staticmethod(_cspace.csMath3_CalcNormal)
    __swig_getmethods__["CalcPlane"] = lambda x: _cspace.csMath3_CalcPlane
    if _newclass:CalcPlane = staticmethod(_cspace.csMath3_CalcPlane)
    __swig_getmethods__["PlanesEqual"] = lambda x: _cspace.csMath3_PlanesEqual
    if _newclass:PlanesEqual = staticmethod(_cspace.csMath3_PlanesEqual)
    __swig_getmethods__["PlanesClose"] = lambda x: _cspace.csMath3_PlanesClose
    if _newclass:PlanesClose = staticmethod(_cspace.csMath3_PlanesClose)
    __swig_getmethods__["OuterPlanes"] = lambda x: _cspace.csMath3_OuterPlanes
    if _newclass:OuterPlanes = staticmethod(_cspace.csMath3_OuterPlanes)
    __swig_getmethods__["FindObserverSides"] = lambda x: _cspace.csMath3_FindObserverSides
    if _newclass:FindObserverSides = staticmethod(_cspace.csMath3_FindObserverSides)
    __swig_getmethods__["SpherePosition"] = lambda x: _cspace.csMath3_SpherePosition
    if _newclass:SpherePosition = staticmethod(_cspace.csMath3_SpherePosition)
    def __init__(self, *args): 
        this = _cspace.new_csMath3(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csMath3
    __del__ = lambda self : None;
csMath3_swigregister = _cspace.csMath3_swigregister
csMath3_swigregister(csMath3)
csMath3_WhichSide3D = _cspace.csMath3_WhichSide3D
csMath3_Visible = _cspace.csMath3_Visible
csMath3_Between = _cspace.csMath3_Between
csMath3_SetMinMax = _cspace.csMath3_SetMinMax
csMath3_DoubleArea3 = _cspace.csMath3_DoubleArea3
csMath3_Direction3 = _cspace.csMath3_Direction3
csMath3_CalcNormal = _cspace.csMath3_CalcNormal
csMath3_CalcPlane = _cspace.csMath3_CalcPlane
csMath3_PlanesEqual = _cspace.csMath3_PlanesEqual
csMath3_PlanesClose = _cspace.csMath3_PlanesClose
csMath3_OuterPlanes = _cspace.csMath3_OuterPlanes
csMath3_FindObserverSides = _cspace.csMath3_FindObserverSides
csMath3_SpherePosition = _cspace.csMath3_SpherePosition

class csSquaredDist(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSquaredDist, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csSquaredDist, name)
    __repr__ = _swig_repr
    __swig_getmethods__["PointPoint"] = lambda x: _cspace.csSquaredDist_PointPoint
    if _newclass:PointPoint = staticmethod(_cspace.csSquaredDist_PointPoint)
    __swig_getmethods__["PointLine"] = lambda x: _cspace.csSquaredDist_PointLine
    if _newclass:PointLine = staticmethod(_cspace.csSquaredDist_PointLine)
    __swig_getmethods__["PointPlane"] = lambda x: _cspace.csSquaredDist_PointPlane
    if _newclass:PointPlane = staticmethod(_cspace.csSquaredDist_PointPlane)
    __swig_getmethods__["PointPoly"] = lambda x: _cspace.csSquaredDist_PointPoly
    if _newclass:PointPoly = staticmethod(_cspace.csSquaredDist_PointPoly)
    def __init__(self, *args): 
        this = _cspace.new_csSquaredDist(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csSquaredDist
    __del__ = lambda self : None;
csSquaredDist_swigregister = _cspace.csSquaredDist_swigregister
csSquaredDist_swigregister(csSquaredDist)
csSquaredDist_PointPoint = _cspace.csSquaredDist_PointPoint
csSquaredDist_PointLine = _cspace.csSquaredDist_PointLine
csSquaredDist_PointPlane = _cspace.csSquaredDist_PointPlane
csSquaredDist_PointPoly = _cspace.csSquaredDist_PointPoly

class csIntersect3(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csIntersect3, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csIntersect3, name)
    __repr__ = _swig_repr
    __swig_getmethods__["PlanePolygon"] = lambda x: _cspace.csIntersect3_PlanePolygon
    if _newclass:PlanePolygon = staticmethod(_cspace.csIntersect3_PlanePolygon)
    __swig_getmethods__["SegmentFrustum"] = lambda x: _cspace.csIntersect3_SegmentFrustum
    if _newclass:SegmentFrustum = staticmethod(_cspace.csIntersect3_SegmentFrustum)
    __swig_getmethods__["SegmentTriangle"] = lambda x: _cspace.csIntersect3_SegmentTriangle
    if _newclass:SegmentTriangle = staticmethod(_cspace.csIntersect3_SegmentTriangle)
    __swig_getmethods__["SegmentPolygon"] = lambda x: _cspace.csIntersect3_SegmentPolygon
    if _newclass:SegmentPolygon = staticmethod(_cspace.csIntersect3_SegmentPolygon)
    __swig_getmethods__["SegmentPlanes"] = lambda x: _cspace.csIntersect3_SegmentPlanes
    if _newclass:SegmentPlanes = staticmethod(_cspace.csIntersect3_SegmentPlanes)
    __swig_getmethods__["SegmentPlane"] = lambda x: _cspace.csIntersect3_SegmentPlane
    if _newclass:SegmentPlane = staticmethod(_cspace.csIntersect3_SegmentPlane)
    __swig_getmethods__["ThreePlanes"] = lambda x: _cspace.csIntersect3_ThreePlanes
    if _newclass:ThreePlanes = staticmethod(_cspace.csIntersect3_ThreePlanes)
    __swig_getmethods__["PlaneXPlane"] = lambda x: _cspace.csIntersect3_PlaneXPlane
    if _newclass:PlaneXPlane = staticmethod(_cspace.csIntersect3_PlaneXPlane)
    __swig_getmethods__["PlaneYPlane"] = lambda x: _cspace.csIntersect3_PlaneYPlane
    if _newclass:PlaneYPlane = staticmethod(_cspace.csIntersect3_PlaneYPlane)
    __swig_getmethods__["PlaneZPlane"] = lambda x: _cspace.csIntersect3_PlaneZPlane
    if _newclass:PlaneZPlane = staticmethod(_cspace.csIntersect3_PlaneZPlane)
    __swig_getmethods__["PlaneAxisPlane"] = lambda x: _cspace.csIntersect3_PlaneAxisPlane
    if _newclass:PlaneAxisPlane = staticmethod(_cspace.csIntersect3_PlaneAxisPlane)
    __swig_getmethods__["SegmentZ0Plane"] = lambda x: _cspace.csIntersect3_SegmentZ0Plane
    if _newclass:SegmentZ0Plane = staticmethod(_cspace.csIntersect3_SegmentZ0Plane)
    __swig_getmethods__["SegmentXPlane"] = lambda x: _cspace.csIntersect3_SegmentXPlane
    if _newclass:SegmentXPlane = staticmethod(_cspace.csIntersect3_SegmentXPlane)
    __swig_getmethods__["SegmentYPlane"] = lambda x: _cspace.csIntersect3_SegmentYPlane
    if _newclass:SegmentYPlane = staticmethod(_cspace.csIntersect3_SegmentYPlane)
    __swig_getmethods__["SegmentZPlane"] = lambda x: _cspace.csIntersect3_SegmentZPlane
    if _newclass:SegmentZPlane = staticmethod(_cspace.csIntersect3_SegmentZPlane)
    __swig_getmethods__["SegmentAxisPlane"] = lambda x: _cspace.csIntersect3_SegmentAxisPlane
    if _newclass:SegmentAxisPlane = staticmethod(_cspace.csIntersect3_SegmentAxisPlane)
    __swig_getmethods__["SegmentXFrustum"] = lambda x: _cspace.csIntersect3_SegmentXFrustum
    if _newclass:SegmentXFrustum = staticmethod(_cspace.csIntersect3_SegmentXFrustum)
    __swig_getmethods__["SegmentYFrustum"] = lambda x: _cspace.csIntersect3_SegmentYFrustum
    if _newclass:SegmentYFrustum = staticmethod(_cspace.csIntersect3_SegmentYFrustum)
    __swig_getmethods__["BoxSegment"] = lambda x: _cspace.csIntersect3_BoxSegment
    if _newclass:BoxSegment = staticmethod(_cspace.csIntersect3_BoxSegment)
    __swig_getmethods__["ClipSegmentBox"] = lambda x: _cspace.csIntersect3_ClipSegmentBox
    if _newclass:ClipSegmentBox = staticmethod(_cspace.csIntersect3_ClipSegmentBox)
    __swig_getmethods__["BoxFrustum"] = lambda x: _cspace.csIntersect3_BoxFrustum
    if _newclass:BoxFrustum = staticmethod(_cspace.csIntersect3_BoxFrustum)
    __swig_getmethods__["BoxSphere"] = lambda x: _cspace.csIntersect3_BoxSphere
    if _newclass:BoxSphere = staticmethod(_cspace.csIntersect3_BoxSphere)
    __swig_getmethods__["BoxPlane"] = lambda x: _cspace.csIntersect3_BoxPlane
    if _newclass:BoxPlane = staticmethod(_cspace.csIntersect3_BoxPlane)
    __swig_getmethods__["BoxTriangle"] = lambda x: _cspace.csIntersect3_BoxTriangle
    if _newclass:BoxTriangle = staticmethod(_cspace.csIntersect3_BoxTriangle)
    __swig_getmethods__["BoxBox"] = lambda x: _cspace.csIntersect3_BoxBox
    if _newclass:BoxBox = staticmethod(_cspace.csIntersect3_BoxBox)
    __swig_getmethods__["FrustumFrustum"] = lambda x: _cspace.csIntersect3_FrustumFrustum
    if _newclass:FrustumFrustum = staticmethod(_cspace.csIntersect3_FrustumFrustum)
    __swig_getmethods__["TriangleTriangle"] = lambda x: _cspace.csIntersect3_TriangleTriangle
    if _newclass:TriangleTriangle = staticmethod(_cspace.csIntersect3_TriangleTriangle)
    def __init__(self, *args): 
        this = _cspace.new_csIntersect3(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csIntersect3
    __del__ = lambda self : None;
csIntersect3_swigregister = _cspace.csIntersect3_swigregister
csIntersect3_swigregister(csIntersect3)
csIntersect3_PlanePolygon = _cspace.csIntersect3_PlanePolygon
csIntersect3_SegmentFrustum = _cspace.csIntersect3_SegmentFrustum
csIntersect3_SegmentTriangle = _cspace.csIntersect3_SegmentTriangle
csIntersect3_SegmentPolygon = _cspace.csIntersect3_SegmentPolygon
csIntersect3_SegmentPlanes = _cspace.csIntersect3_SegmentPlanes
csIntersect3_SegmentPlane = _cspace.csIntersect3_SegmentPlane
csIntersect3_ThreePlanes = _cspace.csIntersect3_ThreePlanes
csIntersect3_PlaneXPlane = _cspace.csIntersect3_PlaneXPlane
csIntersect3_PlaneYPlane = _cspace.csIntersect3_PlaneYPlane
csIntersect3_PlaneZPlane = _cspace.csIntersect3_PlaneZPlane
csIntersect3_PlaneAxisPlane = _cspace.csIntersect3_PlaneAxisPlane
csIntersect3_SegmentZ0Plane = _cspace.csIntersect3_SegmentZ0Plane
csIntersect3_SegmentXPlane = _cspace.csIntersect3_SegmentXPlane
csIntersect3_SegmentYPlane = _cspace.csIntersect3_SegmentYPlane
csIntersect3_SegmentZPlane = _cspace.csIntersect3_SegmentZPlane
csIntersect3_SegmentAxisPlane = _cspace.csIntersect3_SegmentAxisPlane
csIntersect3_SegmentXFrustum = _cspace.csIntersect3_SegmentXFrustum
csIntersect3_SegmentYFrustum = _cspace.csIntersect3_SegmentYFrustum
csIntersect3_BoxSegment = _cspace.csIntersect3_BoxSegment
csIntersect3_ClipSegmentBox = _cspace.csIntersect3_ClipSegmentBox
csIntersect3_BoxFrustum = _cspace.csIntersect3_BoxFrustum
csIntersect3_BoxSphere = _cspace.csIntersect3_BoxSphere
csIntersect3_BoxPlane = _cspace.csIntersect3_BoxPlane
csIntersect3_BoxTriangle = _cspace.csIntersect3_BoxTriangle
csIntersect3_BoxBox = _cspace.csIntersect3_BoxBox
csIntersect3_FrustumFrustum = _cspace.csIntersect3_FrustumFrustum
csIntersect3_TriangleTriangle = _cspace.csIntersect3_TriangleTriangle

class csGeomDebugHelper(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csGeomDebugHelper, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csGeomDebugHelper, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csGeomDebugHelper(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csGeomDebugHelper
    __del__ = lambda self : None;
    def GetSupportedTests(*args): return _cspace.csGeomDebugHelper_GetSupportedTests(*args)
    def UnitTest(*args): return _cspace.csGeomDebugHelper_UnitTest(*args)
    def StateTest(*args): return _cspace.csGeomDebugHelper_StateTest(*args)
    def Benchmark(*args): return _cspace.csGeomDebugHelper_Benchmark(*args)
    def Dump(*args): return _cspace.csGeomDebugHelper_Dump(*args)
    def DebugCommand(*args): return _cspace.csGeomDebugHelper_DebugCommand(*args)
csGeomDebugHelper_swigregister = _cspace.csGeomDebugHelper_swigregister
csGeomDebugHelper_swigregister(csGeomDebugHelper)

CS_POL_SAME_PLANE = _cspace.CS_POL_SAME_PLANE
CS_POL_FRONT = _cspace.CS_POL_FRONT
CS_POL_BACK = _cspace.CS_POL_BACK
CS_POL_SPLIT_NEEDED = _cspace.CS_POL_SPLIT_NEEDED
class csPoly3D(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPoly3D, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csPoly3D, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csPoly3D(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csPoly3D
    __del__ = lambda self : None;
    def MakeEmpty(*args): return _cspace.csPoly3D_MakeEmpty(*args)
    def GetVertexCount(*args): return _cspace.csPoly3D_GetVertexCount(*args)
    def GetVertices(*args): return _cspace.csPoly3D_GetVertices(*args)
    def GetVertex(*args): return _cspace.csPoly3D_GetVertex(*args)
    def GetFirst(*args): return _cspace.csPoly3D_GetFirst(*args)
    def GetLast(*args): return _cspace.csPoly3D_GetLast(*args)
    __swig_getmethods__["In"] = lambda x: _cspace.csPoly3D_In
    if _newclass:In = staticmethod(_cspace.csPoly3D_In)
    def MakeRoom(*args): return _cspace.csPoly3D_MakeRoom(*args)
    def SetVertexCount(*args): return _cspace.csPoly3D_SetVertexCount(*args)
    def AddVertex(*args): return _cspace.csPoly3D_AddVertex(*args)
    def SetVertices(*args): return _cspace.csPoly3D_SetVertices(*args)
    def ProjectXPlane(*args): return _cspace.csPoly3D_ProjectXPlane(*args)
    def ProjectYPlane(*args): return _cspace.csPoly3D_ProjectYPlane(*args)
    def ProjectZPlane(*args): return _cspace.csPoly3D_ProjectZPlane(*args)
    def ProjectAxisPlane(*args): return _cspace.csPoly3D_ProjectAxisPlane(*args)
    def Classify(*args): return _cspace.csPoly3D_Classify(*args)
    def ClassifyX(*args): return _cspace.csPoly3D_ClassifyX(*args)
    def ClassifyY(*args): return _cspace.csPoly3D_ClassifyY(*args)
    def ClassifyZ(*args): return _cspace.csPoly3D_ClassifyZ(*args)
    def ClassifyAxis(*args): return _cspace.csPoly3D_ClassifyAxis(*args)
    def IsAxisAligned(*args): return _cspace.csPoly3D_IsAxisAligned(*args)
    def ComputeMainNormalAxis(*args): return _cspace.csPoly3D_ComputeMainNormalAxis(*args)
    def CutToPlane(*args): return _cspace.csPoly3D_CutToPlane(*args)
    def SplitWithPlane(*args): return _cspace.csPoly3D_SplitWithPlane(*args)
    def SplitWithPlaneX(*args): return _cspace.csPoly3D_SplitWithPlaneX(*args)
    def SplitWithPlaneY(*args): return _cspace.csPoly3D_SplitWithPlaneY(*args)
    def SplitWithPlaneZ(*args): return _cspace.csPoly3D_SplitWithPlaneZ(*args)
    def ComputeNormal(*args): return _cspace.csPoly3D_ComputeNormal(*args)
    def ComputePlane(*args): return _cspace.csPoly3D_ComputePlane(*args)
    def GetArea(*args): return _cspace.csPoly3D_GetArea(*args)
    def GetCenter(*args): return _cspace.csPoly3D_GetCenter(*args)
    def InSphere(*args): return _cspace.csPoly3D_InSphere(*args)
    def __getitem__(*args): return _cspace.csPoly3D___getitem__(*args)
    def __setitem__ (self, i, v):
      own_v = self.__getitem__(i)
      for i in range(3):
        own_v[i] = v[i]

csPoly3D_swigregister = _cspace.csPoly3D_swigregister
csPoly3D_swigregister(csPoly3D)
csPoly3D_In = _cspace.csPoly3D_In

class csCompressVertex(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csCompressVertex, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csCompressVertex, name)
    __repr__ = _swig_repr
    __swig_setmethods__["orig_idx"] = _cspace.csCompressVertex_orig_idx_set
    __swig_getmethods__["orig_idx"] = _cspace.csCompressVertex_orig_idx_get
    if _newclass:orig_idx = property(_cspace.csCompressVertex_orig_idx_get, _cspace.csCompressVertex_orig_idx_set)
    __swig_setmethods__["x"] = _cspace.csCompressVertex_x_set
    __swig_getmethods__["x"] = _cspace.csCompressVertex_x_get
    if _newclass:x = property(_cspace.csCompressVertex_x_get, _cspace.csCompressVertex_x_set)
    __swig_setmethods__["y"] = _cspace.csCompressVertex_y_set
    __swig_getmethods__["y"] = _cspace.csCompressVertex_y_get
    if _newclass:y = property(_cspace.csCompressVertex_y_get, _cspace.csCompressVertex_y_set)
    __swig_setmethods__["z"] = _cspace.csCompressVertex_z_set
    __swig_getmethods__["z"] = _cspace.csCompressVertex_z_get
    if _newclass:z = property(_cspace.csCompressVertex_z_get, _cspace.csCompressVertex_z_set)
    __swig_setmethods__["new_idx"] = _cspace.csCompressVertex_new_idx_set
    __swig_getmethods__["new_idx"] = _cspace.csCompressVertex_new_idx_get
    if _newclass:new_idx = property(_cspace.csCompressVertex_new_idx_get, _cspace.csCompressVertex_new_idx_set)
    __swig_setmethods__["used"] = _cspace.csCompressVertex_used_set
    __swig_getmethods__["used"] = _cspace.csCompressVertex_used_get
    if _newclass:used = property(_cspace.csCompressVertex_used_get, _cspace.csCompressVertex_used_set)
    def __init__(self, *args): 
        this = _cspace.new_csCompressVertex(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csCompressVertex
    __del__ = lambda self : None;
csCompressVertex_swigregister = _cspace.csCompressVertex_swigregister
csCompressVertex_swigregister(csCompressVertex)

class csVector3Array(csPoly3D):
    __swig_setmethods__ = {}
    for _s in [csPoly3D]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csVector3Array, name, value)
    __swig_getmethods__ = {}
    for _s in [csPoly3D]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csVector3Array, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csVector3Array(*args)
        try: self.this.append(this)
        except: self.this = this
    def AddVertexSmart(*args): return _cspace.csVector3Array_AddVertexSmart(*args)
    __swig_getmethods__["CompressVertices"] = lambda x: _cspace.csVector3Array_CompressVertices
    if _newclass:CompressVertices = staticmethod(_cspace.csVector3Array_CompressVertices)
    __swig_destroy__ = _cspace.delete_csVector3Array
    __del__ = lambda self : None;
csVector3Array_swigregister = _cspace.csVector3Array_swigregister
csVector3Array_swigregister(csVector3Array)
csVector3Array_CompressVertices = _cspace.csVector3Array_CompressVertices

class TriangleInt(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TriangleInt, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TriangleInt, name)
    __repr__ = _swig_repr
    __swig_setmethods__["a"] = _cspace.TriangleInt_a_set
    __swig_getmethods__["a"] = _cspace.TriangleInt_a_get
    if _newclass:a = property(_cspace.TriangleInt_a_get, _cspace.TriangleInt_a_set)
    __swig_setmethods__["b"] = _cspace.TriangleInt_b_set
    __swig_getmethods__["b"] = _cspace.TriangleInt_b_get
    if _newclass:b = property(_cspace.TriangleInt_b_get, _cspace.TriangleInt_b_set)
    __swig_setmethods__["c"] = _cspace.TriangleInt_c_set
    __swig_getmethods__["c"] = _cspace.TriangleInt_c_get
    if _newclass:c = property(_cspace.TriangleInt_c_get, _cspace.TriangleInt_c_set)
    def Set(*args): return _cspace.TriangleInt_Set(*args)
    def __init__(self, *args): 
        this = _cspace.new_TriangleInt(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_TriangleInt
    __del__ = lambda self : None;
TriangleInt_swigregister = _cspace.TriangleInt_swigregister
TriangleInt_swigregister(TriangleInt)

class csTriangle(TriangleInt):
    __swig_setmethods__ = {}
    for _s in [TriangleInt]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csTriangle, name, value)
    __swig_getmethods__ = {}
    for _s in [TriangleInt]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csTriangle, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csTriangle(*args)
        try: self.this.append(this)
        except: self.this = this
    def __getitem__(*args): return _cspace.csTriangle___getitem__(*args)
    def __setitem__(*args): return _cspace.csTriangle___setitem__(*args)
    def __nonzero__(*args): return _cspace.csTriangle___nonzero__(*args)
    __swig_destroy__ = _cspace.delete_csTriangle
    __del__ = lambda self : None;
csTriangle_swigregister = _cspace.csTriangle_swigregister
csTriangle_swigregister(csTriangle)

class csRect(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csRect, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csRect, name)
    __repr__ = _swig_repr
    __swig_setmethods__["xmin"] = _cspace.csRect_xmin_set
    __swig_getmethods__["xmin"] = _cspace.csRect_xmin_get
    if _newclass:xmin = property(_cspace.csRect_xmin_get, _cspace.csRect_xmin_set)
    __swig_setmethods__["ymin"] = _cspace.csRect_ymin_set
    __swig_getmethods__["ymin"] = _cspace.csRect_ymin_get
    if _newclass:ymin = property(_cspace.csRect_ymin_get, _cspace.csRect_ymin_set)
    __swig_setmethods__["xmax"] = _cspace.csRect_xmax_set
    __swig_getmethods__["xmax"] = _cspace.csRect_xmax_get
    if _newclass:xmax = property(_cspace.csRect_xmax_get, _cspace.csRect_xmax_set)
    __swig_setmethods__["ymax"] = _cspace.csRect_ymax_set
    __swig_getmethods__["ymax"] = _cspace.csRect_ymax_get
    if _newclass:ymax = property(_cspace.csRect_ymax_get, _cspace.csRect_ymax_set)
    def __init__(self, *args): 
        this = _cspace.new_csRect(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csRect
    __del__ = lambda self : None;
    def Intersect(*args): return _cspace.csRect_Intersect(*args)
    def Intersects(*args): return _cspace.csRect_Intersects(*args)
    def Union(*args): return _cspace.csRect_Union(*args)
    def Exclude(*args): return _cspace.csRect_Exclude(*args)
    def Subtract(*args): return _cspace.csRect_Subtract(*args)
    def IsEmpty(*args): return _cspace.csRect_IsEmpty(*args)
    def MakeEmpty(*args): return _cspace.csRect_MakeEmpty(*args)
    def Set(*args): return _cspace.csRect_Set(*args)
    def SetPos(*args): return _cspace.csRect_SetPos(*args)
    def SetSize(*args): return _cspace.csRect_SetSize(*args)
    def Move(*args): return _cspace.csRect_Move(*args)
    def Width(*args): return _cspace.csRect_Width(*args)
    def Height(*args): return _cspace.csRect_Height(*args)
    def Contains(*args): return _cspace.csRect_Contains(*args)
    def ContainsRel(*args): return _cspace.csRect_ContainsRel(*args)
    def Equal(*args): return _cspace.csRect_Equal(*args)
    def Normalize(*args): return _cspace.csRect_Normalize(*args)
    def Area(*args): return _cspace.csRect_Area(*args)
    def AddAdjacent(*args): return _cspace.csRect_AddAdjacent(*args)
    def __eq__(*args): return _cspace.csRect___eq__(*args)
    def __ne__(*args): return _cspace.csRect___ne__(*args)
    def Extend(*args): return _cspace.csRect_Extend(*args)
    def Join(*args): return _cspace.csRect_Join(*args)
    def Outset(*args): return _cspace.csRect_Outset(*args)
    def Inset(*args): return _cspace.csRect_Inset(*args)
    def ClipLineGeneral(*args): return _cspace.csRect_ClipLineGeneral(*args)
    def ClipLine(*args): return _cspace.csRect_ClipLine(*args)
    def ClipLineSafe(*args): return _cspace.csRect_ClipLineSafe(*args)
csRect_swigregister = _cspace.csRect_swigregister
csRect_swigregister(csRect)

class csRectRegion(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csRectRegion, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csRectRegion, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csRectRegion(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csRectRegion
    __del__ = lambda self : None;
    def Include(*args): return _cspace.csRectRegion_Include(*args)
    def Exclude(*args): return _cspace.csRectRegion_Exclude(*args)
    def ClipTo(*args): return _cspace.csRectRegion_ClipTo(*args)
    def Count(*args): return _cspace.csRectRegion_Count(*args)
    def RectAt(*args): return _cspace.csRectRegion_RectAt(*args)
    def MakeEmpty(*args): return _cspace.csRectRegion_MakeEmpty(*args)
csRectRegion_swigregister = _cspace.csRectRegion_swigregister
csRectRegion_swigregister(csRectRegion)

class csQuaternion(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csQuaternion, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csQuaternion, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csQuaternion(*args)
        try: self.this.append(this)
        except: self.this = this
    def Set(*args): return _cspace.csQuaternion_Set(*args)
    def SetIdentity(*args): return _cspace.csQuaternion_SetIdentity(*args)
    def __iadd__(*args): return _cspace.csQuaternion___iadd__(*args)
    def __isub__(*args): return _cspace.csQuaternion___isub__(*args)
    def __imul__(*args): return _cspace.csQuaternion___imul__(*args)
    def GetConjugate(*args): return _cspace.csQuaternion_GetConjugate(*args)
    def Conjugate(*args): return _cspace.csQuaternion_Conjugate(*args)
    def Dot(*args): return _cspace.csQuaternion_Dot(*args)
    def SquaredNorm(*args): return _cspace.csQuaternion_SquaredNorm(*args)
    def Norm(*args): return _cspace.csQuaternion_Norm(*args)
    def Unit(*args): return _cspace.csQuaternion_Unit(*args)
    def Rotate(*args): return _cspace.csQuaternion_Rotate(*args)
    def SetAxisAngle(*args): return _cspace.csQuaternion_SetAxisAngle(*args)
    def GetAxisAngle(*args): return _cspace.csQuaternion_GetAxisAngle(*args)
    def SetEulerAngles(*args): return _cspace.csQuaternion_SetEulerAngles(*args)
    def GetEulerAngles(*args): return _cspace.csQuaternion_GetEulerAngles(*args)
    def SetMatrix(*args): return _cspace.csQuaternion_SetMatrix(*args)
    def GetMatrix(*args): return _cspace.csQuaternion_GetMatrix(*args)
    def NLerp(*args): return _cspace.csQuaternion_NLerp(*args)
    def SLerp(*args): return _cspace.csQuaternion_SLerp(*args)
    def Log(*args): return _cspace.csQuaternion_Log(*args)
    def Exp(*args): return _cspace.csQuaternion_Exp(*args)
    def Squad(*args): return _cspace.csQuaternion_Squad(*args)
    __swig_setmethods__["v"] = _cspace.csQuaternion_v_set
    __swig_getmethods__["v"] = _cspace.csQuaternion_v_get
    if _newclass:v = property(_cspace.csQuaternion_v_get, _cspace.csQuaternion_v_set)
    __swig_setmethods__["w"] = _cspace.csQuaternion_w_set
    __swig_getmethods__["w"] = _cspace.csQuaternion_w_get
    if _newclass:w = property(_cspace.csQuaternion_w_get, _cspace.csQuaternion_w_set)
    def __add__(*args): return _cspace.csQuaternion___add__(*args)
    def __sub__(*args): return _cspace.csQuaternion___sub__(*args)
    def __mul__(*args): return _cspace.csQuaternion___mul__(*args)
    __swig_destroy__ = _cspace.delete_csQuaternion
    __del__ = lambda self : None;
csQuaternion_swigregister = _cspace.csQuaternion_swigregister
csQuaternion_swigregister(csQuaternion)

class csSpline(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSpline, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csSpline, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_csSpline
    __del__ = lambda self : None;
    def GetDimensionCount(*args): return _cspace.csSpline_GetDimensionCount(*args)
    def GetPointCount(*args): return _cspace.csSpline_GetPointCount(*args)
    def InsertPoint(*args): return _cspace.csSpline_InsertPoint(*args)
    def RemovePoint(*args): return _cspace.csSpline_RemovePoint(*args)
    def SetTimeValues(*args): return _cspace.csSpline_SetTimeValues(*args)
    def SetTimeValue(*args): return _cspace.csSpline_SetTimeValue(*args)
    def GetTimeValues(*args): return _cspace.csSpline_GetTimeValues(*args)
    def GetTimeValue(*args): return _cspace.csSpline_GetTimeValue(*args)
    def SetDimensionValues(*args): return _cspace.csSpline_SetDimensionValues(*args)
    def SetDimensionValue(*args): return _cspace.csSpline_SetDimensionValue(*args)
    def GetDimensionValues(*args): return _cspace.csSpline_GetDimensionValues(*args)
    def GetDimensionValue(*args): return _cspace.csSpline_GetDimensionValue(*args)
    def SetIndexValues(*args): return _cspace.csSpline_SetIndexValues(*args)
    def GetIndexValues(*args): return _cspace.csSpline_GetIndexValues(*args)
    def Calculate(*args): return _cspace.csSpline_Calculate(*args)
    def GetCurrentIndex(*args): return _cspace.csSpline_GetCurrentIndex(*args)
    def GetInterpolatedDimension(*args): return _cspace.csSpline_GetInterpolatedDimension(*args)
csSpline_swigregister = _cspace.csSpline_swigregister
csSpline_swigregister(csSpline)

class csCubicSpline(csSpline):
    __swig_setmethods__ = {}
    for _s in [csSpline]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csCubicSpline, name, value)
    __swig_getmethods__ = {}
    for _s in [csSpline]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csCubicSpline, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csCubicSpline(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csCubicSpline
    __del__ = lambda self : None;
    def Calculate(*args): return _cspace.csCubicSpline_Calculate(*args)
    def GetInterpolatedDimension(*args): return _cspace.csCubicSpline_GetInterpolatedDimension(*args)
csCubicSpline_swigregister = _cspace.csCubicSpline_swigregister
csCubicSpline_swigregister(csCubicSpline)

class csBSpline(csSpline):
    __swig_setmethods__ = {}
    for _s in [csSpline]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csBSpline, name, value)
    __swig_getmethods__ = {}
    for _s in [csSpline]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csBSpline, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csBSpline(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csBSpline
    __del__ = lambda self : None;
    def Calculate(*args): return _cspace.csBSpline_Calculate(*args)
    def GetInterpolatedDimension(*args): return _cspace.csBSpline_GetInterpolatedDimension(*args)
csBSpline_swigregister = _cspace.csBSpline_swigregister
csBSpline_swigregister(csBSpline)

class csCatmullRomSpline(csBSpline):
    __swig_setmethods__ = {}
    for _s in [csBSpline]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csCatmullRomSpline, name, value)
    __swig_getmethods__ = {}
    for _s in [csBSpline]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csCatmullRomSpline, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csCatmullRomSpline(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csCatmullRomSpline
    __del__ = lambda self : None;
    def Clone(*args): return _cspace.csCatmullRomSpline_Clone(*args)
csCatmullRomSpline_swigregister = _cspace.csCatmullRomSpline_swigregister
csCatmullRomSpline_swigregister(csCatmullRomSpline)

CS_BOUNDINGBOX_MAXVALUE = _cspace.CS_BOUNDINGBOX_MAXVALUE
CS_BOX_CORNER_xy = _cspace.CS_BOX_CORNER_xy
CS_BOX_CORNER_xY = _cspace.CS_BOX_CORNER_xY
CS_BOX_CORNER_Xy = _cspace.CS_BOX_CORNER_Xy
CS_BOX_CORNER_XY = _cspace.CS_BOX_CORNER_XY
CS_BOX_CENTER2 = _cspace.CS_BOX_CENTER2
CS_BOX_EDGE_xy_Xy = _cspace.CS_BOX_EDGE_xy_Xy
CS_BOX_EDGE_Xy_xy = _cspace.CS_BOX_EDGE_Xy_xy
CS_BOX_EDGE_Xy_XY = _cspace.CS_BOX_EDGE_Xy_XY
CS_BOX_EDGE_XY_Xy = _cspace.CS_BOX_EDGE_XY_Xy
CS_BOX_EDGE_XY_xY = _cspace.CS_BOX_EDGE_XY_xY
CS_BOX_EDGE_xY_XY = _cspace.CS_BOX_EDGE_xY_XY
CS_BOX_EDGE_xY_xy = _cspace.CS_BOX_EDGE_xY_xy
CS_BOX_EDGE_xy_xY = _cspace.CS_BOX_EDGE_xy_xY
class csBox2(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csBox2, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csBox2, name)
    __repr__ = _swig_repr
    def MinX(*args): return _cspace.csBox2_MinX(*args)
    def MinY(*args): return _cspace.csBox2_MinY(*args)
    def MaxX(*args): return _cspace.csBox2_MaxX(*args)
    def MaxY(*args): return _cspace.csBox2_MaxY(*args)
    def Min(*args): return _cspace.csBox2_Min(*args)
    def Max(*args): return _cspace.csBox2_Max(*args)
    def Area(*args): return _cspace.csBox2_Area(*args)
    def GetCorner(*args): return _cspace.csBox2_GetCorner(*args)
    def GetCenter(*args): return _cspace.csBox2_GetCenter(*args)
    def SetCenter(*args): return _cspace.csBox2_SetCenter(*args)
    def SetSize(*args): return _cspace.csBox2_SetSize(*args)
    def GetEdgeInfo(*args): return _cspace.csBox2_GetEdgeInfo(*args)
    def GetEdge(*args): return _cspace.csBox2_GetEdge(*args)
    def Intersect(*args): return _cspace.csBox2_Intersect(*args)
    def In(*args): return _cspace.csBox2_In(*args)
    def Overlap(*args): return _cspace.csBox2_Overlap(*args)
    def Contains(*args): return _cspace.csBox2_Contains(*args)
    def Empty(*args): return _cspace.csBox2_Empty(*args)
    def SquaredOriginDist(*args): return _cspace.csBox2_SquaredOriginDist(*args)
    def SquaredOriginMaxDist(*args): return _cspace.csBox2_SquaredOriginMaxDist(*args)
    def SquaredPosDist(*args): return _cspace.csBox2_SquaredPosDist(*args)
    def SquaredPosMaxDist(*args): return _cspace.csBox2_SquaredPosMaxDist(*args)
    def StartBoundingBox(*args): return _cspace.csBox2_StartBoundingBox(*args)
    def AddBoundingVertex(*args): return _cspace.csBox2_AddBoundingVertex(*args)
    def AddBoundingVertexSmart(*args): return _cspace.csBox2_AddBoundingVertexSmart(*args)
    def AddBoundingVertexTest(*args): return _cspace.csBox2_AddBoundingVertexTest(*args)
    def AddBoundingVertexSmartTest(*args): return _cspace.csBox2_AddBoundingVertexSmartTest(*args)
    def __init__(self, *args): 
        this = _cspace.new_csBox2(*args)
        try: self.this.append(this)
        except: self.this = this
    def Set(*args): return _cspace.csBox2_Set(*args)
    def SetMin(*args): return _cspace.csBox2_SetMin(*args)
    def SetMax(*args): return _cspace.csBox2_SetMax(*args)
    def Description(*args): return _cspace.csBox2_Description(*args)
    def __iadd__(*args): return _cspace.csBox2___iadd__(*args)
    def __imul__(*args): return _cspace.csBox2___imul__(*args)
    def TestIntersect(*args): return _cspace.csBox2_TestIntersect(*args)
    def __mul__(*args): return _cspace.csBox2___mul__(*args)
    def __ne__(*args): return _cspace.csBox2___ne__(*args)
    def __gt__(*args): return _cspace.csBox2___gt__(*args)
    def __add__(*args): return _cspace.csBox2___add__(*args)
    def __lt__(*args): return _cspace.csBox2___lt__(*args)
    __swig_destroy__ = _cspace.delete_csBox2
    __del__ = lambda self : None;
csBox2_swigregister = _cspace.csBox2_swigregister
csBox2_swigregister(csBox2)

CS_BOX_CORNER_xyz = _cspace.CS_BOX_CORNER_xyz
CS_BOX_CORNER_xyZ = _cspace.CS_BOX_CORNER_xyZ
CS_BOX_CORNER_xYz = _cspace.CS_BOX_CORNER_xYz
CS_BOX_CORNER_xYZ = _cspace.CS_BOX_CORNER_xYZ
CS_BOX_CORNER_Xyz = _cspace.CS_BOX_CORNER_Xyz
CS_BOX_CORNER_XyZ = _cspace.CS_BOX_CORNER_XyZ
CS_BOX_CORNER_XYz = _cspace.CS_BOX_CORNER_XYz
CS_BOX_CORNER_XYZ = _cspace.CS_BOX_CORNER_XYZ
CS_BOX_CENTER3 = _cspace.CS_BOX_CENTER3
CS_BOX_SIDE_x = _cspace.CS_BOX_SIDE_x
CS_BOX_SIDE_X = _cspace.CS_BOX_SIDE_X
CS_BOX_SIDE_y = _cspace.CS_BOX_SIDE_y
CS_BOX_SIDE_Y = _cspace.CS_BOX_SIDE_Y
CS_BOX_SIDE_z = _cspace.CS_BOX_SIDE_z
CS_BOX_SIDE_Z = _cspace.CS_BOX_SIDE_Z
CS_BOX_INSIDE = _cspace.CS_BOX_INSIDE
CS_BOX_EDGE_Xyz_xyz = _cspace.CS_BOX_EDGE_Xyz_xyz
CS_BOX_EDGE_xyz_Xyz = _cspace.CS_BOX_EDGE_xyz_Xyz
CS_BOX_EDGE_xyz_xYz = _cspace.CS_BOX_EDGE_xyz_xYz
CS_BOX_EDGE_xYz_xyz = _cspace.CS_BOX_EDGE_xYz_xyz
CS_BOX_EDGE_xYz_XYz = _cspace.CS_BOX_EDGE_xYz_XYz
CS_BOX_EDGE_XYz_xYz = _cspace.CS_BOX_EDGE_XYz_xYz
CS_BOX_EDGE_XYz_Xyz = _cspace.CS_BOX_EDGE_XYz_Xyz
CS_BOX_EDGE_Xyz_XYz = _cspace.CS_BOX_EDGE_Xyz_XYz
CS_BOX_EDGE_Xyz_XyZ = _cspace.CS_BOX_EDGE_Xyz_XyZ
CS_BOX_EDGE_XyZ_Xyz = _cspace.CS_BOX_EDGE_XyZ_Xyz
CS_BOX_EDGE_XyZ_XYZ = _cspace.CS_BOX_EDGE_XyZ_XYZ
CS_BOX_EDGE_XYZ_XyZ = _cspace.CS_BOX_EDGE_XYZ_XyZ
CS_BOX_EDGE_XYZ_XYz = _cspace.CS_BOX_EDGE_XYZ_XYz
CS_BOX_EDGE_XYz_XYZ = _cspace.CS_BOX_EDGE_XYz_XYZ
CS_BOX_EDGE_XYZ_xYZ = _cspace.CS_BOX_EDGE_XYZ_xYZ
CS_BOX_EDGE_xYZ_XYZ = _cspace.CS_BOX_EDGE_xYZ_XYZ
CS_BOX_EDGE_xYZ_xYz = _cspace.CS_BOX_EDGE_xYZ_xYz
CS_BOX_EDGE_xYz_xYZ = _cspace.CS_BOX_EDGE_xYz_xYZ
CS_BOX_EDGE_xYZ_xyZ = _cspace.CS_BOX_EDGE_xYZ_xyZ
CS_BOX_EDGE_xyZ_xYZ = _cspace.CS_BOX_EDGE_xyZ_xYZ
CS_BOX_EDGE_xyZ_xyz = _cspace.CS_BOX_EDGE_xyZ_xyz
CS_BOX_EDGE_xyz_xyZ = _cspace.CS_BOX_EDGE_xyz_xyZ
CS_BOX_EDGE_xyZ_XyZ = _cspace.CS_BOX_EDGE_xyZ_XyZ
CS_BOX_EDGE_XyZ_xyZ = _cspace.CS_BOX_EDGE_XyZ_xyZ
class csBox3(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csBox3, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csBox3, name)
    __repr__ = _swig_repr
    def MinX(*args): return _cspace.csBox3_MinX(*args)
    def MinY(*args): return _cspace.csBox3_MinY(*args)
    def MinZ(*args): return _cspace.csBox3_MinZ(*args)
    def MaxX(*args): return _cspace.csBox3_MaxX(*args)
    def MaxY(*args): return _cspace.csBox3_MaxY(*args)
    def MaxZ(*args): return _cspace.csBox3_MaxZ(*args)
    def Min(*args): return _cspace.csBox3_Min(*args)
    def Max(*args): return _cspace.csBox3_Max(*args)
    def Volume(*args): return _cspace.csBox3_Volume(*args)
    def Area(*args): return _cspace.csBox3_Area(*args)
    def GetCorner(*args): return _cspace.csBox3_GetCorner(*args)
    def GetEdgeInfo(*args): return _cspace.csBox3_GetEdgeInfo(*args)
    def GetFaceEdges(*args): return _cspace.csBox3_GetFaceEdges(*args)
    def GetCenter(*args): return _cspace.csBox3_GetCenter(*args)
    def SetCenter(*args): return _cspace.csBox3_SetCenter(*args)
    def SetSize(*args): return _cspace.csBox3_SetSize(*args)
    def GetSize(*args): return _cspace.csBox3_GetSize(*args)
    def GetSide(*args): return _cspace.csBox3_GetSide(*args)
    def GetAxisPlane(*args): return _cspace.csBox3_GetAxisPlane(*args)
    def GetVisibleSides(*args): return _cspace.csBox3_GetVisibleSides(*args)
    __swig_getmethods__["OtherSide"] = lambda x: _cspace.csBox3_OtherSide
    if _newclass:OtherSide = staticmethod(_cspace.csBox3_OtherSide)
    def GetEdge(*args): return _cspace.csBox3_GetEdge(*args)
    def In(*args): return _cspace.csBox3_In(*args)
    def Overlap(*args): return _cspace.csBox3_Overlap(*args)
    def Contains(*args): return _cspace.csBox3_Contains(*args)
    def Empty(*args): return _cspace.csBox3_Empty(*args)
    def StartBoundingBox(*args): return _cspace.csBox3_StartBoundingBox(*args)
    def AddBoundingVertex(*args): return _cspace.csBox3_AddBoundingVertex(*args)
    def AddBoundingVertexSmart(*args): return _cspace.csBox3_AddBoundingVertexSmart(*args)
    def AddBoundingVertexTest(*args): return _cspace.csBox3_AddBoundingVertexTest(*args)
    def AddBoundingVertexSmartTest(*args): return _cspace.csBox3_AddBoundingVertexSmartTest(*args)
    def AddBoundingBox(*args): return _cspace.csBox3_AddBoundingBox(*args)
    def __init__(self, *args): 
        this = _cspace.new_csBox3(*args)
        try: self.this.append(this)
        except: self.this = this
    def Set(*args): return _cspace.csBox3_Set(*args)
    def SetMin(*args): return _cspace.csBox3_SetMin(*args)
    def SetMax(*args): return _cspace.csBox3_SetMax(*args)
    def GetMin(*args): return _cspace.csBox3_GetMin(*args)
    def GetMax(*args): return _cspace.csBox3_GetMax(*args)
    def Description(*args): return _cspace.csBox3_Description(*args)
    def Split(*args): return _cspace.csBox3_Split(*args)
    def TestSplit(*args): return _cspace.csBox3_TestSplit(*args)
    def AdjacentX(*args): return _cspace.csBox3_AdjacentX(*args)
    def AdjacentY(*args): return _cspace.csBox3_AdjacentY(*args)
    def AdjacentZ(*args): return _cspace.csBox3_AdjacentZ(*args)
    def Adjacent(*args): return _cspace.csBox3_Adjacent(*args)
    def CalculatePointSegment(*args): return _cspace.csBox3_CalculatePointSegment(*args)
    def GetConvexOutline(*args): return _cspace.csBox3_GetConvexOutline(*args)
    def Between(*args): return _cspace.csBox3_Between(*args)
    def ManhattanDistance(*args): return _cspace.csBox3_ManhattanDistance(*args)
    def SquaredOriginDist(*args): return _cspace.csBox3_SquaredOriginDist(*args)
    def SquaredOriginMaxDist(*args): return _cspace.csBox3_SquaredOriginMaxDist(*args)
    def SquaredPosDist(*args): return _cspace.csBox3_SquaredPosDist(*args)
    def SquaredPosMaxDist(*args): return _cspace.csBox3_SquaredPosMaxDist(*args)
    def ProjectBox(*args): return _cspace.csBox3_ProjectBox(*args)
    def ProjectOutline(*args): return _cspace.csBox3_ProjectOutline(*args)
    def ProjectBoxAndOutline(*args): return _cspace.csBox3_ProjectBoxAndOutline(*args)
    def __iadd__(*args): return _cspace.csBox3___iadd__(*args)
    def __imul__(*args): return _cspace.csBox3___imul__(*args)
    def TestIntersect(*args): return _cspace.csBox3_TestIntersect(*args)
    def __mul__(*args): return _cspace.csBox3___mul__(*args)
    def __ne__(*args): return _cspace.csBox3___ne__(*args)
    def __gt__(*args): return _cspace.csBox3___gt__(*args)
    def __add__(*args): return _cspace.csBox3___add__(*args)
    def __lt__(*args): return _cspace.csBox3___lt__(*args)
    __swig_destroy__ = _cspace.delete_csBox3
    __del__ = lambda self : None;
csBox3_swigregister = _cspace.csBox3_swigregister
csBox3_swigregister(csBox3)
csBox3_OtherSide = _cspace.csBox3_OtherSide

class csSegment2(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSegment2, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csSegment2, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csSegment2(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csSegment2
    __del__ = lambda self : None;
    def Set(*args): return _cspace.csSegment2_Set(*args)
    def SetStart(*args): return _cspace.csSegment2_SetStart(*args)
    def SetEnd(*args): return _cspace.csSegment2_SetEnd(*args)
    def Start(*args): return _cspace.csSegment2_Start(*args)
    def End(*args): return _cspace.csSegment2_End(*args)
csSegment2_swigregister = _cspace.csSegment2_swigregister
csSegment2_swigregister(csSegment2)

class csSegment3(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSegment3, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csSegment3, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csSegment3(*args)
        try: self.this.append(this)
        except: self.this = this
    def Set(*args): return _cspace.csSegment3_Set(*args)
    def SetStart(*args): return _cspace.csSegment3_SetStart(*args)
    def SetEnd(*args): return _cspace.csSegment3_SetEnd(*args)
    def Start(*args): return _cspace.csSegment3_Start(*args)
    def End(*args): return _cspace.csSegment3_End(*args)
    __swig_destroy__ = _cspace.delete_csSegment3
    __del__ = lambda self : None;
csSegment3_swigregister = _cspace.csSegment3_swigregister
csSegment3_swigregister(csSegment3)

class csRGBcolor(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csRGBcolor, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csRGBcolor, name)
    __repr__ = _swig_repr
    __swig_setmethods__["red"] = _cspace.csRGBcolor_red_set
    __swig_getmethods__["red"] = _cspace.csRGBcolor_red_get
    if _newclass:red = property(_cspace.csRGBcolor_red_get, _cspace.csRGBcolor_red_set)
    __swig_setmethods__["green"] = _cspace.csRGBcolor_green_set
    __swig_getmethods__["green"] = _cspace.csRGBcolor_green_get
    if _newclass:green = property(_cspace.csRGBcolor_green_get, _cspace.csRGBcolor_green_set)
    __swig_setmethods__["blue"] = _cspace.csRGBcolor_blue_set
    __swig_getmethods__["blue"] = _cspace.csRGBcolor_blue_get
    if _newclass:blue = property(_cspace.csRGBcolor_blue_get, _cspace.csRGBcolor_blue_set)
    def __init__(self, *args): 
        this = _cspace.new_csRGBcolor(*args)
        try: self.this.append(this)
        except: self.this = this
    def Set(*args): return _cspace.csRGBcolor_Set(*args)
    def __eq__(*args): return _cspace.csRGBcolor___eq__(*args)
    def __ne__(*args): return _cspace.csRGBcolor___ne__(*args)
    def __add__(*args): return _cspace.csRGBcolor___add__(*args)
    def UnsafeAdd(*args): return _cspace.csRGBcolor_UnsafeAdd(*args)
    def SafeAdd(*args): return _cspace.csRGBcolor_SafeAdd(*args)
    __swig_destroy__ = _cspace.delete_csRGBcolor
    __del__ = lambda self : None;
csRGBcolor_swigregister = _cspace.csRGBcolor_swigregister
csRGBcolor_swigregister(csRGBcolor)

class csRGBpixel(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csRGBpixel, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csRGBpixel, name)
    __repr__ = _swig_repr
    __swig_setmethods__["red"] = _cspace.csRGBpixel_red_set
    __swig_getmethods__["red"] = _cspace.csRGBpixel_red_get
    if _newclass:red = property(_cspace.csRGBpixel_red_get, _cspace.csRGBpixel_red_set)
    __swig_setmethods__["green"] = _cspace.csRGBpixel_green_set
    __swig_getmethods__["green"] = _cspace.csRGBpixel_green_get
    if _newclass:green = property(_cspace.csRGBpixel_green_get, _cspace.csRGBpixel_green_set)
    __swig_setmethods__["blue"] = _cspace.csRGBpixel_blue_set
    __swig_getmethods__["blue"] = _cspace.csRGBpixel_blue_get
    if _newclass:blue = property(_cspace.csRGBpixel_blue_get, _cspace.csRGBpixel_blue_set)
    __swig_setmethods__["alpha"] = _cspace.csRGBpixel_alpha_set
    __swig_getmethods__["alpha"] = _cspace.csRGBpixel_alpha_get
    if _newclass:alpha = property(_cspace.csRGBpixel_alpha_get, _cspace.csRGBpixel_alpha_set)
    def __init__(self, *args): 
        this = _cspace.new_csRGBpixel(*args)
        try: self.this.append(this)
        except: self.this = this
    def __eq__(*args): return _cspace.csRGBpixel___eq__(*args)
    def __ne__(*args): return _cspace.csRGBpixel___ne__(*args)
    def asRGBcolor(*args): return _cspace.csRGBpixel_asRGBcolor(*args)
    def eq(*args): return _cspace.csRGBpixel_eq(*args)
    def Intensity(*args): return _cspace.csRGBpixel_Intensity(*args)
    def Luminance(*args): return _cspace.csRGBpixel_Luminance(*args)
    def Set(*args): return _cspace.csRGBpixel_Set(*args)
    def __iadd__(*args): return _cspace.csRGBpixel___iadd__(*args)
    def UnsafeAdd(*args): return _cspace.csRGBpixel_UnsafeAdd(*args)
    def SafeAdd(*args): return _cspace.csRGBpixel_SafeAdd(*args)
    __swig_destroy__ = _cspace.delete_csRGBpixel
    __del__ = lambda self : None;
csRGBpixel_swigregister = _cspace.csRGBpixel_swigregister
csRGBpixel_swigregister(csRGBpixel)

R_COEF = _cspace.R_COEF
G_COEF = _cspace.G_COEF
B_COEF = _cspace.B_COEF
R_COEF_SQ = _cspace.R_COEF_SQ
G_COEF_SQ = _cspace.G_COEF_SQ
B_COEF_SQ = _cspace.B_COEF_SQ
class iShaderVariableAccessor(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iShaderVariableAccessor, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iShaderVariableAccessor, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def PreGetValue(*args): return _cspace.iShaderVariableAccessor_PreGetValue(*args)
    __swig_destroy__ = _cspace.delete_iShaderVariableAccessor
    __del__ = lambda self : None;
iShaderVariableAccessor_swigregister = _cspace.iShaderVariableAccessor_swigregister
iShaderVariableAccessor_swigregister(iShaderVariableAccessor)

class csShaderVariable(csRefCount):
    __swig_setmethods__ = {}
    for _s in [csRefCount]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csShaderVariable, name, value)
    __swig_getmethods__ = {}
    for _s in [csRefCount]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csShaderVariable, name)
    __repr__ = _swig_repr
    UNKNOWN = _cspace.csShaderVariable_UNKNOWN
    INT = _cspace.csShaderVariable_INT
    FLOAT = _cspace.csShaderVariable_FLOAT
    TEXTURE = _cspace.csShaderVariable_TEXTURE
    RENDERBUFFER = _cspace.csShaderVariable_RENDERBUFFER
    VECTOR2 = _cspace.csShaderVariable_VECTOR2
    VECTOR3 = _cspace.csShaderVariable_VECTOR3
    VECTOR4 = _cspace.csShaderVariable_VECTOR4
    MATRIX = _cspace.csShaderVariable_MATRIX
    TRANSFORM = _cspace.csShaderVariable_TRANSFORM
    ARRAY = _cspace.csShaderVariable_ARRAY
    COLOR = _cspace.csShaderVariable_COLOR
    def __init__(self, *args): 
        this = _cspace.new_csShaderVariable(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csShaderVariable
    __del__ = lambda self : None;
    def assign(*args): return _cspace.csShaderVariable_assign(*args)
    def GetType(*args): return _cspace.csShaderVariable_GetType(*args)
    def SetType(*args): return _cspace.csShaderVariable_SetType(*args)
    def SetAccessor(*args): return _cspace.csShaderVariable_SetAccessor(*args)
    def SetName(*args): return _cspace.csShaderVariable_SetName(*args)
    def GetName(*args): return _cspace.csShaderVariable_GetName(*args)
    def SetValue(*args): return _cspace.csShaderVariable_SetValue(*args)
    def AddVariableToArray(*args): return _cspace.csShaderVariable_AddVariableToArray(*args)
    def RemoveFromArray(*args): return _cspace.csShaderVariable_RemoveFromArray(*args)
    def SetArraySize(*args): return _cspace.csShaderVariable_SetArraySize(*args)
    def GetArraySize(*args): return _cspace.csShaderVariable_GetArraySize(*args)
    def GetArrayElement(*args): return _cspace.csShaderVariable_GetArrayElement(*args)
    def SetArrayElement(*args): return _cspace.csShaderVariable_SetArrayElement(*args)
    def GetValue(*args): return _cspace.csShaderVariable_GetValue(*args)
csShaderVariable_swigregister = _cspace.csShaderVariable_swigregister
csShaderVariable_swigregister(csShaderVariable)

class csShaderVariableArrayReadOnly(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csShaderVariableArrayReadOnly, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csShaderVariableArrayReadOnly, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetSize(*args): return _cspace.csShaderVariableArrayReadOnly_GetSize(*args)
    def Get(*args): return _cspace.csShaderVariableArrayReadOnly_Get(*args)
    def Top(*args): return _cspace.csShaderVariableArrayReadOnly_Top(*args)
    def Find(*args): return _cspace.csShaderVariableArrayReadOnly_Find(*args)
    def GetIndex(*args): return _cspace.csShaderVariableArrayReadOnly_GetIndex(*args)
    def IsEmpty(*args): return _cspace.csShaderVariableArrayReadOnly_IsEmpty(*args)
    def GetAll(*args): return _cspace.csShaderVariableArrayReadOnly_GetAll(*args)
    __swig_destroy__ = _cspace.delete_csShaderVariableArrayReadOnly
    __del__ = lambda self : None;
csShaderVariableArrayReadOnly_swigregister = _cspace.csShaderVariableArrayReadOnly_swigregister
csShaderVariableArrayReadOnly_swigregister(csShaderVariableArrayReadOnly)

class csShaderVariableArrayChangeElements(csShaderVariableArrayReadOnly):
    __swig_setmethods__ = {}
    for _s in [csShaderVariableArrayReadOnly]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csShaderVariableArrayChangeElements, name, value)
    __swig_getmethods__ = {}
    for _s in [csShaderVariableArrayReadOnly]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csShaderVariableArrayChangeElements, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Get(*args): return _cspace.csShaderVariableArrayChangeElements_Get(*args)
    def Top(*args): return _cspace.csShaderVariableArrayChangeElements_Top(*args)
    __swig_destroy__ = _cspace.delete_csShaderVariableArrayChangeElements
    __del__ = lambda self : None;
csShaderVariableArrayChangeElements_swigregister = _cspace.csShaderVariableArrayChangeElements_swigregister
csShaderVariableArrayChangeElements_swigregister(csShaderVariableArrayChangeElements)

class csShaderVariableArray(csShaderVariableArrayChangeElements):
    __swig_setmethods__ = {}
    for _s in [csShaderVariableArrayChangeElements]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csShaderVariableArray, name, value)
    __swig_getmethods__ = {}
    for _s in [csShaderVariableArrayChangeElements]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csShaderVariableArray, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetSize(*args): return _cspace.csShaderVariableArray_SetSize(*args)
    def GetExtend(*args): return _cspace.csShaderVariableArray_GetExtend(*args)
    def Put(*args): return _cspace.csShaderVariableArray_Put(*args)
    def Push(*args): return _cspace.csShaderVariableArray_Push(*args)
    def PushSmart(*args): return _cspace.csShaderVariableArray_PushSmart(*args)
    def Pop(*args): return _cspace.csShaderVariableArray_Pop(*args)
    def Insert(*args): return _cspace.csShaderVariableArray_Insert(*args)
    def DeleteAll(*args): return _cspace.csShaderVariableArray_DeleteAll(*args)
    def Truncate(*args): return _cspace.csShaderVariableArray_Truncate(*args)
    def Empty(*args): return _cspace.csShaderVariableArray_Empty(*args)
    def DeleteIndex(*args): return _cspace.csShaderVariableArray_DeleteIndex(*args)
    def DeleteIndexFast(*args): return _cspace.csShaderVariableArray_DeleteIndexFast(*args)
    def Delete(*args): return _cspace.csShaderVariableArray_Delete(*args)
    __swig_destroy__ = _cspace.delete_csShaderVariableArray
    __del__ = lambda self : None;
csShaderVariableArray_swigregister = _cspace.csShaderVariableArray_swigregister
csShaderVariableArray_swigregister(csShaderVariableArray)

csDefaultRunLoop = _cspace.csDefaultRunLoop
csPlatformStartup = _cspace.csPlatformStartup
csPlatformShutdown = _cspace.csPlatformShutdown
csPrintf = _cspace.csPrintf
csFPrintf = _cspace.csFPrintf
csPrintfErr = _cspace.csPrintfErr
csGetTicks = _cspace.csGetTicks
csGetMicroTicks = _cspace.csGetMicroTicks
csSleep = _cspace.csSleep
csGetUsername = _cspace.csGetUsername
class csPluginRequest(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPluginRequest, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csPluginRequest, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csPluginRequest(*args)
        try: self.this.append(this)
        except: self.this = this
    def assign(*args): return _cspace.csPluginRequest_assign(*args)
    def __eq__(*args): return _cspace.csPluginRequest___eq__(*args)
    def __ne__(*args): return _cspace.csPluginRequest___ne__(*args)
    def GetClassName(*args): return _cspace.csPluginRequest_GetClassName(*args)
    def GetInterfaceName(*args): return _cspace.csPluginRequest_GetInterfaceName(*args)
    def GetInterfaceID(*args): return _cspace.csPluginRequest_GetInterfaceID(*args)
    def GetInterfaceVersion(*args): return _cspace.csPluginRequest_GetInterfaceVersion(*args)
    __swig_destroy__ = _cspace.delete_csPluginRequest
    __del__ = lambda self : None;
csPluginRequest_swigregister = _cspace.csPluginRequest_swigregister
csPluginRequest_swigregister(csPluginRequest)
csGetPlatformConfigPath = _cspace.csGetPlatformConfigPath

class csInitializer(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csInitializer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csInitializer, name)
    __repr__ = _swig_repr
    __swig_getmethods__["CreateEnvironment"] = lambda x: _cspace.csInitializer_CreateEnvironment
    if _newclass:CreateEnvironment = staticmethod(_cspace.csInitializer_CreateEnvironment)
    __swig_getmethods__["InitializeSCF"] = lambda x: _cspace.csInitializer_InitializeSCF
    if _newclass:InitializeSCF = staticmethod(_cspace.csInitializer_InitializeSCF)
    __swig_getmethods__["CreateObjectRegistry"] = lambda x: _cspace.csInitializer_CreateObjectRegistry
    if _newclass:CreateObjectRegistry = staticmethod(_cspace.csInitializer_CreateObjectRegistry)
    __swig_getmethods__["CreatePluginManager"] = lambda x: _cspace.csInitializer_CreatePluginManager
    if _newclass:CreatePluginManager = staticmethod(_cspace.csInitializer_CreatePluginManager)
    __swig_getmethods__["CreateEventQueue"] = lambda x: _cspace.csInitializer_CreateEventQueue
    if _newclass:CreateEventQueue = staticmethod(_cspace.csInitializer_CreateEventQueue)
    __swig_getmethods__["CreateVirtualClock"] = lambda x: _cspace.csInitializer_CreateVirtualClock
    if _newclass:CreateVirtualClock = staticmethod(_cspace.csInitializer_CreateVirtualClock)
    __swig_getmethods__["CreateCommandLineParser"] = lambda x: _cspace.csInitializer_CreateCommandLineParser
    if _newclass:CreateCommandLineParser = staticmethod(_cspace.csInitializer_CreateCommandLineParser)
    __swig_getmethods__["CreateVerbosityManager"] = lambda x: _cspace.csInitializer_CreateVerbosityManager
    if _newclass:CreateVerbosityManager = staticmethod(_cspace.csInitializer_CreateVerbosityManager)
    __swig_getmethods__["CreateConfigManager"] = lambda x: _cspace.csInitializer_CreateConfigManager
    if _newclass:CreateConfigManager = staticmethod(_cspace.csInitializer_CreateConfigManager)
    __swig_getmethods__["CreateInputDrivers"] = lambda x: _cspace.csInitializer_CreateInputDrivers
    if _newclass:CreateInputDrivers = staticmethod(_cspace.csInitializer_CreateInputDrivers)
    __swig_getmethods__["CreateStringSet"] = lambda x: _cspace.csInitializer_CreateStringSet
    if _newclass:CreateStringSet = staticmethod(_cspace.csInitializer_CreateStringSet)
    __swig_getmethods__["SetupConfigManager"] = lambda x: _cspace.csInitializer_SetupConfigManager
    if _newclass:SetupConfigManager = staticmethod(_cspace.csInitializer_SetupConfigManager)
    __swig_getmethods__["SetupVFS"] = lambda x: _cspace.csInitializer_SetupVFS
    if _newclass:SetupVFS = staticmethod(_cspace.csInitializer_SetupVFS)
    __swig_getmethods__["_RequestPlugins"] = lambda x: _cspace.csInitializer__RequestPlugins
    if _newclass:_RequestPlugins = staticmethod(_cspace.csInitializer__RequestPlugins)
    __swig_getmethods__["OpenApplication"] = lambda x: _cspace.csInitializer_OpenApplication
    if _newclass:OpenApplication = staticmethod(_cspace.csInitializer_OpenApplication)
    __swig_getmethods__["CloseApplication"] = lambda x: _cspace.csInitializer_CloseApplication
    if _newclass:CloseApplication = staticmethod(_cspace.csInitializer_CloseApplication)
    __swig_getmethods__["_SetupEventHandler"] = lambda x: _cspace.csInitializer__SetupEventHandler
    if _newclass:_SetupEventHandler = staticmethod(_cspace.csInitializer__SetupEventHandler)
    __swig_getmethods__["DestroyApplication"] = lambda x: _cspace.csInitializer_DestroyApplication
    if _newclass:DestroyApplication = staticmethod(_cspace.csInitializer_DestroyApplication)
    __swig_getmethods__["GetDefaultAppID"] = lambda x: _cspace.csInitializer_GetDefaultAppID
    if _newclass:GetDefaultAppID = staticmethod(_cspace.csInitializer_GetDefaultAppID)
    def __init__(self, *args): 
        this = _cspace.new_csInitializer(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csInitializer
    __del__ = lambda self : None;
csInitializer_swigregister = _cspace.csInitializer_swigregister
csInitializer_swigregister(csInitializer)
csInitializer_CreateEnvironment = _cspace.csInitializer_CreateEnvironment
csInitializer_InitializeSCF = _cspace.csInitializer_InitializeSCF
csInitializer_CreateObjectRegistry = _cspace.csInitializer_CreateObjectRegistry
csInitializer_CreatePluginManager = _cspace.csInitializer_CreatePluginManager
csInitializer_CreateEventQueue = _cspace.csInitializer_CreateEventQueue
csInitializer_CreateVirtualClock = _cspace.csInitializer_CreateVirtualClock
csInitializer_CreateCommandLineParser = _cspace.csInitializer_CreateCommandLineParser
csInitializer_CreateVerbosityManager = _cspace.csInitializer_CreateVerbosityManager
csInitializer_CreateConfigManager = _cspace.csInitializer_CreateConfigManager
csInitializer_CreateInputDrivers = _cspace.csInitializer_CreateInputDrivers
csInitializer_CreateStringSet = _cspace.csInitializer_CreateStringSet
csInitializer_SetupConfigManager = _cspace.csInitializer_SetupConfigManager
csInitializer_SetupVFS = _cspace.csInitializer_SetupVFS
csInitializer__RequestPlugins = _cspace.csInitializer__RequestPlugins
csInitializer_OpenApplication = _cspace.csInitializer_OpenApplication
csInitializer_CloseApplication = _cspace.csInitializer_CloseApplication
csInitializer__SetupEventHandler = _cspace.csInitializer__SetupEventHandler
csInitializer_DestroyApplication = _cspace.csInitializer_DestroyApplication
csInitializer_GetDefaultAppID = _cspace.csInitializer_GetDefaultAppID

class CustomAllocated(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, CustomAllocated, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, CustomAllocated, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_CustomAllocated(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_CustomAllocated
    __del__ = lambda self : None;
CustomAllocated_swigregister = _cspace.CustomAllocated_swigregister
CustomAllocated_swigregister(CustomAllocated)

class csPluginRequestArray(CustomAllocated):
    __swig_setmethods__ = {}
    for _s in [CustomAllocated]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPluginRequestArray, name, value)
    __swig_getmethods__ = {}
    for _s in [CustomAllocated]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csPluginRequestArray, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_csPluginRequestArray
    __del__ = lambda self : None;
    def __init__(self, *args): 
        this = _cspace.new_csPluginRequestArray(*args)
        try: self.this.append(this)
        except: self.this = this
    def GetSize(*args): return _cspace.csPluginRequestArray_GetSize(*args)
    def Get(*args): return _cspace.csPluginRequestArray_Get(*args)
    def Push(*args): return _cspace.csPluginRequestArray_Push(*args)
    def Pop(*args): return _cspace.csPluginRequestArray_Pop(*args)
    def Top(*args): return _cspace.csPluginRequestArray_Top(*args)
    def Insert(*args): return _cspace.csPluginRequestArray_Insert(*args)
    def Contains(*args): return _cspace.csPluginRequestArray_Contains(*args)
    def Truncate(*args): return _cspace.csPluginRequestArray_Truncate(*args)
    def Empty(*args): return _cspace.csPluginRequestArray_Empty(*args)
    def IsEmpty(*args): return _cspace.csPluginRequestArray_IsEmpty(*args)
    def SetMinimalCapacity(*args): return _cspace.csPluginRequestArray_SetMinimalCapacity(*args)
    def DeleteIndex(*args): return _cspace.csPluginRequestArray_DeleteIndex(*args)
    def DeleteIndexFast(*args): return _cspace.csPluginRequestArray_DeleteIndexFast(*args)
    def DeleteRange(*args): return _cspace.csPluginRequestArray_DeleteRange(*args)
    def __eq__(*args): return _cspace.csPluginRequestArray___eq__(*args)
    def __ne__(*args): return _cspace.csPluginRequestArray___ne__(*args)
    def GetAllocator(*args): return _cspace.csPluginRequestArray_GetAllocator(*args)
csPluginRequestArray_swigregister = _cspace.csPluginRequestArray_swigregister
csPluginRequestArray_swigregister(csPluginRequestArray)

MAX_OUTPUT_VERTICES = _cspace.MAX_OUTPUT_VERTICES
CS_CLIP_OUTSIDE = _cspace.CS_CLIP_OUTSIDE
CS_CLIP_CLIPPED = _cspace.CS_CLIP_CLIPPED
CS_CLIP_INSIDE = _cspace.CS_CLIP_INSIDE
class csVertexStatus(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csVertexStatus, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csVertexStatus, name)
    __repr__ = _swig_repr
    __swig_setmethods__["Type"] = _cspace.csVertexStatus_Type_set
    __swig_getmethods__["Type"] = _cspace.csVertexStatus_Type_get
    if _newclass:Type = property(_cspace.csVertexStatus_Type_get, _cspace.csVertexStatus_Type_set)
    __swig_setmethods__["Vertex"] = _cspace.csVertexStatus_Vertex_set
    __swig_getmethods__["Vertex"] = _cspace.csVertexStatus_Vertex_get
    if _newclass:Vertex = property(_cspace.csVertexStatus_Vertex_get, _cspace.csVertexStatus_Vertex_set)
    __swig_setmethods__["Pos"] = _cspace.csVertexStatus_Pos_set
    __swig_getmethods__["Pos"] = _cspace.csVertexStatus_Pos_get
    if _newclass:Pos = property(_cspace.csVertexStatus_Pos_get, _cspace.csVertexStatus_Pos_set)
    def __init__(self, *args): 
        this = _cspace.new_csVertexStatus(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csVertexStatus
    __del__ = lambda self : None;
csVertexStatus_swigregister = _cspace.csVertexStatus_swigregister
csVertexStatus_swigregister(csVertexStatus)

CS_VERTEX_ORIGINAL = _cspace.CS_VERTEX_ORIGINAL
CS_VERTEX_ONEDGE = _cspace.CS_VERTEX_ONEDGE
CS_VERTEX_INSIDE = _cspace.CS_VERTEX_INSIDE
class iClipper2D(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iClipper2D, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iClipper2D, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Clip(*args): return _cspace.iClipper2D_Clip(*args)
    def ClipInPlace(*args): return _cspace.iClipper2D_ClipInPlace(*args)
    def ClassifyBox(*args): return _cspace.iClipper2D_ClassifyBox(*args)
    def IsInside(*args): return _cspace.iClipper2D_IsInside(*args)
    def GetVertexCount(*args): return _cspace.iClipper2D_GetVertexCount(*args)
    def GetClipPoly(*args): return _cspace.iClipper2D_GetClipPoly(*args)
    clipperPoly = _cspace.iClipper2D_clipperPoly
    clipperBox = _cspace.iClipper2D_clipperBox
    def GetClipperType(*args): return _cspace.iClipper2D_GetClipperType(*args)
    __swig_destroy__ = _cspace.delete_iClipper2D
    __del__ = lambda self : None;
iClipper2D_swigregister = _cspace.iClipper2D_swigregister
iClipper2D_swigregister(iClipper2D)

class iDecal(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDecal, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, iDecal, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_iDecal(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_iDecal
    __del__ = lambda self : None;
iDecal_swigregister = _cspace.iDecal_swigregister
iDecal_swigregister(iDecal)

class iDecalTemplate(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDecalTemplate, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDecalTemplate, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetTimeToLive(*args): return _cspace.iDecalTemplate_GetTimeToLive(*args)
    def GetMaterialWrapper(*args): return _cspace.iDecalTemplate_GetMaterialWrapper(*args)
    def GetRenderPriority(*args): return _cspace.iDecalTemplate_GetRenderPriority(*args)
    def GetZBufMode(*args): return _cspace.iDecalTemplate_GetZBufMode(*args)
    def GetPolygonNormalThreshold(*args): return _cspace.iDecalTemplate_GetPolygonNormalThreshold(*args)
    def GetDecalOffset(*args): return _cspace.iDecalTemplate_GetDecalOffset(*args)
    def HasTopClipping(*args): return _cspace.iDecalTemplate_HasTopClipping(*args)
    def GetTopClippingScale(*args): return _cspace.iDecalTemplate_GetTopClippingScale(*args)
    def HasBottomClipping(*args): return _cspace.iDecalTemplate_HasBottomClipping(*args)
    def GetBottomClippingScale(*args): return _cspace.iDecalTemplate_GetBottomClippingScale(*args)
    def GetMinTexCoord(*args): return _cspace.iDecalTemplate_GetMinTexCoord(*args)
    def GetMainColor(*args): return _cspace.iDecalTemplate_GetMainColor(*args)
    def GetTopColor(*args): return _cspace.iDecalTemplate_GetTopColor(*args)
    def GetBottomColor(*args): return _cspace.iDecalTemplate_GetBottomColor(*args)
    def GetMaxTexCoord(*args): return _cspace.iDecalTemplate_GetMaxTexCoord(*args)
    def GetMixMode(*args): return _cspace.iDecalTemplate_GetMixMode(*args)
    def GetPerpendicularFaceThreshold(*args): return _cspace.iDecalTemplate_GetPerpendicularFaceThreshold(*args)
    def GetPerpendicularFaceOffset(*args): return _cspace.iDecalTemplate_GetPerpendicularFaceOffset(*args)
    def SetTimeToLive(*args): return _cspace.iDecalTemplate_SetTimeToLive(*args)
    def SetMaterialWrapper(*args): return _cspace.iDecalTemplate_SetMaterialWrapper(*args)
    def SetRenderPriority(*args): return _cspace.iDecalTemplate_SetRenderPriority(*args)
    def SetZBufMode(*args): return _cspace.iDecalTemplate_SetZBufMode(*args)
    def SetPolygonNormalThreshold(*args): return _cspace.iDecalTemplate_SetPolygonNormalThreshold(*args)
    def SetDecalOffset(*args): return _cspace.iDecalTemplate_SetDecalOffset(*args)
    def SetTopClipping(*args): return _cspace.iDecalTemplate_SetTopClipping(*args)
    def SetBottomClipping(*args): return _cspace.iDecalTemplate_SetBottomClipping(*args)
    def SetTexCoords(*args): return _cspace.iDecalTemplate_SetTexCoords(*args)
    def SetMixMode(*args): return _cspace.iDecalTemplate_SetMixMode(*args)
    def SetPerpendicularFaceThreshold(*args): return _cspace.iDecalTemplate_SetPerpendicularFaceThreshold(*args)
    def SetPerpendicularFaceOffset(*args): return _cspace.iDecalTemplate_SetPerpendicularFaceOffset(*args)
    def SetMainColor(*args): return _cspace.iDecalTemplate_SetMainColor(*args)
    def SetTopColor(*args): return _cspace.iDecalTemplate_SetTopColor(*args)
    def SetBottomColor(*args): return _cspace.iDecalTemplate_SetBottomColor(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iDecalTemplate_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iDecalTemplate_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iDecalTemplate
    __del__ = lambda self : None;
iDecalTemplate_swigregister = _cspace.iDecalTemplate_swigregister
iDecalTemplate_swigregister(iDecalTemplate)
iDecalTemplate_scfGetVersion = _cspace.iDecalTemplate_scfGetVersion

class iDecalBuilder(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDecalBuilder, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, iDecalBuilder, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_iDecalBuilder
    __del__ = lambda self : None;
    def AddStaticPoly(*args): return _cspace.iDecalBuilder_AddStaticPoly(*args)
iDecalBuilder_swigregister = _cspace.iDecalBuilder_swigregister
iDecalBuilder_swigregister(iDecalBuilder)

class iDecalManager(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDecalManager, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDecalManager, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CreateDecal(*args): return _cspace.iDecalManager_CreateDecal(*args)
    def CreateDecalTemplate(*args): return _cspace.iDecalManager_CreateDecalTemplate(*args)
    def DeleteDecal(*args): return _cspace.iDecalManager_DeleteDecal(*args)
    def GetDecalCount(*args): return _cspace.iDecalManager_GetDecalCount(*args)
    def GetDecal(*args): return _cspace.iDecalManager_GetDecal(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iDecalManager_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iDecalManager_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iDecalManager
    __del__ = lambda self : None;
iDecalManager_swigregister = _cspace.iDecalManager_swigregister
iDecalManager_swigregister(iDecalManager)
iDecalManager_scfGetVersion = _cspace.iDecalManager_scfGetVersion

class iObjectModelListener(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iObjectModelListener, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iObjectModelListener, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def ObjectModelChanged(*args): return _cspace.iObjectModelListener_ObjectModelChanged(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iObjectModelListener_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iObjectModelListener_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iObjectModelListener
    __del__ = lambda self : None;
iObjectModelListener_swigregister = _cspace.iObjectModelListener_swigregister
iObjectModelListener_swigregister(iObjectModelListener)
iObjectModelListener_scfGetVersion = _cspace.iObjectModelListener_scfGetVersion

class iTriangleMeshIterator(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iTriangleMeshIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iTriangleMeshIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def HasNext(*args): return _cspace.iTriangleMeshIterator_HasNext(*args)
    def Next(*args): return _cspace.iTriangleMeshIterator_Next(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iTriangleMeshIterator_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iTriangleMeshIterator_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iTriangleMeshIterator
    __del__ = lambda self : None;
iTriangleMeshIterator_swigregister = _cspace.iTriangleMeshIterator_swigregister
iTriangleMeshIterator_swigregister(iTriangleMeshIterator)
iTriangleMeshIterator_scfGetVersion = _cspace.iTriangleMeshIterator_scfGetVersion

class iObjectModel(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iObjectModel, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iObjectModel, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetShapeNumber(*args): return _cspace.iObjectModel_GetShapeNumber(*args)
    def GetTriangleData(*args): return _cspace.iObjectModel_GetTriangleData(*args)
    def GetTriangleDataIterator(*args): return _cspace.iObjectModel_GetTriangleDataIterator(*args)
    def SetTriangleData(*args): return _cspace.iObjectModel_SetTriangleData(*args)
    def IsTriangleDataSet(*args): return _cspace.iObjectModel_IsTriangleDataSet(*args)
    def ResetTriangleData(*args): return _cspace.iObjectModel_ResetTriangleData(*args)
    def GetTerraFormerColldet(*args): return _cspace.iObjectModel_GetTerraFormerColldet(*args)
    def GetTerrainColldet(*args): return _cspace.iObjectModel_GetTerrainColldet(*args)
    def GetObjectBoundingBox(*args): return _cspace.iObjectModel_GetObjectBoundingBox(*args)
    def SetObjectBoundingBox(*args): return _cspace.iObjectModel_SetObjectBoundingBox(*args)
    def GetRadius(*args): return _cspace.iObjectModel_GetRadius(*args)
    def AddListener(*args): return _cspace.iObjectModel_AddListener(*args)
    def RemoveListener(*args): return _cspace.iObjectModel_RemoveListener(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iObjectModel_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iObjectModel_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iObjectModel
    __del__ = lambda self : None;
iObjectModel_swigregister = _cspace.iObjectModel_swigregister
iObjectModel_swigregister(iObjectModel)
iObjectModel_scfGetVersion = _cspace.iObjectModel_scfGetVersion

class iPath(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iPath, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iPath, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Length(*args): return _cspace.iPath_Length(*args)
    def CalculateAtTime(*args): return _cspace.iPath_CalculateAtTime(*args)
    def GetCurrentIndex(*args): return _cspace.iPath_GetCurrentIndex(*args)
    def SetPositionVectors(*args): return _cspace.iPath_SetPositionVectors(*args)
    def SetUpVectors(*args): return _cspace.iPath_SetUpVectors(*args)
    def SetForwardVectors(*args): return _cspace.iPath_SetForwardVectors(*args)
    def SetPositionVector(*args): return _cspace.iPath_SetPositionVector(*args)
    def SetUpVector(*args): return _cspace.iPath_SetUpVector(*args)
    def SetForwardVector(*args): return _cspace.iPath_SetForwardVector(*args)
    def GetPositionVector(*args): return _cspace.iPath_GetPositionVector(*args)
    def GetUpVector(*args): return _cspace.iPath_GetUpVector(*args)
    def GetForwardVector(*args): return _cspace.iPath_GetForwardVector(*args)
    def GetTime(*args): return _cspace.iPath_GetTime(*args)
    def SetTime(*args): return _cspace.iPath_SetTime(*args)
    def GetInterpolatedPosition(*args): return _cspace.iPath_GetInterpolatedPosition(*args)
    def GetInterpolatedUp(*args): return _cspace.iPath_GetInterpolatedUp(*args)
    def GetInterpolatedForward(*args): return _cspace.iPath_GetInterpolatedForward(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iPath_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iPath_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iPath
    __del__ = lambda self : None;
iPath_swigregister = _cspace.iPath_swigregister
iPath_swigregister(iPath)
iPath_scfGetVersion = _cspace.iPath_scfGetVersion

class scfPath(iPath):
    __swig_setmethods__ = {}
    for _s in [iPath]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, scfPath, name, value)
    __swig_getmethods__ = {}
    for _s in [iPath]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, scfPath, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def IncRef(*args): return _cspace.scfPath_IncRef(*args)
    def DecRef(*args): return _cspace.scfPath_DecRef(*args)
    def GetRefCount(*args): return _cspace.scfPath_GetRefCount(*args)
    def QueryInterface(*args): return _cspace.scfPath_QueryInterface(*args)
    def AddRefOwner(*args): return _cspace.scfPath_AddRefOwner(*args)
    def RemoveRefOwner(*args): return _cspace.scfPath_RemoveRefOwner(*args)
    def GetInterfaceMetadata(*args): return _cspace.scfPath_GetInterfaceMetadata(*args)
scfPath_swigregister = _cspace.scfPath_swigregister
scfPath_swigregister(scfPath)

CS_TRIMESH_CLOSED = _cspace.CS_TRIMESH_CLOSED
CS_TRIMESH_NOTCLOSED = _cspace.CS_TRIMESH_NOTCLOSED
CS_TRIMESH_CONVEX = _cspace.CS_TRIMESH_CONVEX
CS_TRIMESH_NOTCONVEX = _cspace.CS_TRIMESH_NOTCONVEX
CS_TRIMESH_DEFORMABLE = _cspace.CS_TRIMESH_DEFORMABLE
class iTriangleMesh(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iTriangleMesh, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iTriangleMesh, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetVertexCount(*args): return _cspace.iTriangleMesh_GetVertexCount(*args)
    def GetVertices(self):
      return CSMutableArrayHelper(self.GetVertexByIndex, self.GetVertexCount)


    def GetTriangleCount(*args): return _cspace.iTriangleMesh_GetTriangleCount(*args)
    def GetTriangles(self):
      return CSMutableArrayHelper(self.GetTriangleByIndex, self.GetTriangleCount)


    def Lock(*args): return _cspace.iTriangleMesh_Lock(*args)
    def Unlock(*args): return _cspace.iTriangleMesh_Unlock(*args)
    def GetFlags(*args): return _cspace.iTriangleMesh_GetFlags(*args)
    def GetChangeNumber(*args): return _cspace.iTriangleMesh_GetChangeNumber(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iTriangleMesh_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iTriangleMesh_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iTriangleMesh
    __del__ = lambda self : None;
    def GetVertexByIndex(*args): return _cspace.iTriangleMesh_GetVertexByIndex(*args)
    def GetTriangleByIndex(*args): return _cspace.iTriangleMesh_GetTriangleByIndex(*args)
iTriangleMesh_swigregister = _cspace.iTriangleMesh_swigregister
iTriangleMesh_swigregister(iTriangleMesh)
iTriangleMesh_scfGetVersion = _cspace.iTriangleMesh_scfGetVersion

class csPath(scfPath):
    __swig_setmethods__ = {}
    for _s in [scfPath]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPath, name, value)
    __swig_getmethods__ = {}
    for _s in [scfPath]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csPath, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csPath(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csPath
    __del__ = lambda self : None;
    def Length(*args): return _cspace.csPath_Length(*args)
    def CalculateAtTime(*args): return _cspace.csPath_CalculateAtTime(*args)
    def GetCurrentIndex(*args): return _cspace.csPath_GetCurrentIndex(*args)
    def GetTime(*args): return _cspace.csPath_GetTime(*args)
    def SetTime(*args): return _cspace.csPath_SetTime(*args)
    def SetTimes(*args): return _cspace.csPath_SetTimes(*args)
    def GetTimes(*args): return _cspace.csPath_GetTimes(*args)
    def SetPositionVectors(*args): return _cspace.csPath_SetPositionVectors(*args)
    def SetUpVectors(*args): return _cspace.csPath_SetUpVectors(*args)
    def SetForwardVectors(*args): return _cspace.csPath_SetForwardVectors(*args)
    def SetPositionVector(*args): return _cspace.csPath_SetPositionVector(*args)
    def SetUpVector(*args): return _cspace.csPath_SetUpVector(*args)
    def SetForwardVector(*args): return _cspace.csPath_SetForwardVector(*args)
    def GetPositionVector(*args): return _cspace.csPath_GetPositionVector(*args)
    def GetUpVector(*args): return _cspace.csPath_GetUpVector(*args)
    def GetForwardVector(*args): return _cspace.csPath_GetForwardVector(*args)
    def GetInterpolatedPosition(*args): return _cspace.csPath_GetInterpolatedPosition(*args)
    def GetInterpolatedUp(*args): return _cspace.csPath_GetInterpolatedUp(*args)
    def GetInterpolatedForward(*args): return _cspace.csPath_GetInterpolatedForward(*args)
    def GetDimensionValues(*args): return _cspace.csPath_GetDimensionValues(*args)
    def GetDimensionValue(*args): return _cspace.csPath_GetDimensionValue(*args)
    def InsertPoint(*args): return _cspace.csPath_InsertPoint(*args)
    def RemovePoint(*args): return _cspace.csPath_RemovePoint(*args)
csPath_swigregister = _cspace.csPath_swigregister
csPath_swigregister(csPath)

class pycsTriangleMesh(iTriangleMesh):
    __swig_setmethods__ = {}
    for _s in [iTriangleMesh]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, pycsTriangleMesh, name, value)
    __swig_getmethods__ = {}
    for _s in [iTriangleMesh]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, pycsTriangleMesh, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def IncRef(*args): return _cspace.pycsTriangleMesh_IncRef(*args)
    def DecRef(*args): return _cspace.pycsTriangleMesh_DecRef(*args)
    def GetRefCount(*args): return _cspace.pycsTriangleMesh_GetRefCount(*args)
    def QueryInterface(*args): return _cspace.pycsTriangleMesh_QueryInterface(*args)
    def AddRefOwner(*args): return _cspace.pycsTriangleMesh_AddRefOwner(*args)
    def RemoveRefOwner(*args): return _cspace.pycsTriangleMesh_RemoveRefOwner(*args)
    def GetInterfaceMetadata(*args): return _cspace.pycsTriangleMesh_GetInterfaceMetadata(*args)
pycsTriangleMesh_swigregister = _cspace.pycsTriangleMesh_swigregister
pycsTriangleMesh_swigregister(pycsTriangleMesh)

class pycsTriangleMeshBox(iTriangleMesh):
    __swig_setmethods__ = {}
    for _s in [iTriangleMesh]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, pycsTriangleMeshBox, name, value)
    __swig_getmethods__ = {}
    for _s in [iTriangleMesh]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, pycsTriangleMeshBox, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def IncRef(*args): return _cspace.pycsTriangleMeshBox_IncRef(*args)
    def DecRef(*args): return _cspace.pycsTriangleMeshBox_DecRef(*args)
    def GetRefCount(*args): return _cspace.pycsTriangleMeshBox_GetRefCount(*args)
    def QueryInterface(*args): return _cspace.pycsTriangleMeshBox_QueryInterface(*args)
    def AddRefOwner(*args): return _cspace.pycsTriangleMeshBox_AddRefOwner(*args)
    def RemoveRefOwner(*args): return _cspace.pycsTriangleMeshBox_RemoveRefOwner(*args)
    def GetInterfaceMetadata(*args): return _cspace.pycsTriangleMeshBox_GetInterfaceMetadata(*args)
pycsTriangleMeshBox_swigregister = _cspace.pycsTriangleMeshBox_swigregister
pycsTriangleMeshBox_swigregister(pycsTriangleMeshBox)

class csTriangleMesh(pycsTriangleMesh):
    __swig_setmethods__ = {}
    for _s in [pycsTriangleMesh]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csTriangleMesh, name, value)
    __swig_getmethods__ = {}
    for _s in [pycsTriangleMesh]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csTriangleMesh, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csTriangleMesh(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csTriangleMesh
    __del__ = lambda self : None;
    def AddVertex(*args): return _cspace.csTriangleMesh_AddVertex(*args)
    def GetVertexCount(*args): return _cspace.csTriangleMesh_GetVertexCount(*args)
    def GetVertices(*args): return _cspace.csTriangleMesh_GetVertices(*args)
    def AddTriangle(*args): return _cspace.csTriangleMesh_AddTriangle(*args)
    def GetTriangles(*args): return _cspace.csTriangleMesh_GetTriangles(*args)
    def GetTriangle(*args): return _cspace.csTriangleMesh_GetTriangle(*args)
    def GetTriangleCount(*args): return _cspace.csTriangleMesh_GetTriangleCount(*args)
    def Clear(*args): return _cspace.csTriangleMesh_Clear(*args)
    def SetSize(*args): return _cspace.csTriangleMesh_SetSize(*args)
    def SetTriangles(*args): return _cspace.csTriangleMesh_SetTriangles(*args)
    def Lock(*args): return _cspace.csTriangleMesh_Lock(*args)
    def Unlock(*args): return _cspace.csTriangleMesh_Unlock(*args)
    def GetFlags(*args): return _cspace.csTriangleMesh_GetFlags(*args)
    def GetChangeNumber(*args): return _cspace.csTriangleMesh_GetChangeNumber(*args)
csTriangleMesh_swigregister = _cspace.csTriangleMesh_swigregister
csTriangleMesh_swigregister(csTriangleMesh)

class csTriangleVertex(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csTriangleVertex, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csTriangleVertex, name)
    __repr__ = _swig_repr
    __swig_setmethods__["pos"] = _cspace.csTriangleVertex_pos_set
    __swig_getmethods__["pos"] = _cspace.csTriangleVertex_pos_get
    if _newclass:pos = property(_cspace.csTriangleVertex_pos_get, _cspace.csTriangleVertex_pos_set)
    __swig_setmethods__["idx"] = _cspace.csTriangleVertex_idx_set
    __swig_getmethods__["idx"] = _cspace.csTriangleVertex_idx_get
    if _newclass:idx = property(_cspace.csTriangleVertex_idx_get, _cspace.csTriangleVertex_idx_set)
    __swig_setmethods__["con_triangles"] = _cspace.csTriangleVertex_con_triangles_set
    __swig_getmethods__["con_triangles"] = _cspace.csTriangleVertex_con_triangles_get
    if _newclass:con_triangles = property(_cspace.csTriangleVertex_con_triangles_get, _cspace.csTriangleVertex_con_triangles_set)
    __swig_setmethods__["con_vertices"] = _cspace.csTriangleVertex_con_vertices_set
    __swig_getmethods__["con_vertices"] = _cspace.csTriangleVertex_con_vertices_get
    if _newclass:con_vertices = property(_cspace.csTriangleVertex_con_vertices_get, _cspace.csTriangleVertex_con_vertices_set)
    def __init__(self, *args): 
        this = _cspace.new_csTriangleVertex(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csTriangleVertex
    __del__ = lambda self : None;
    def AddTriangle(*args): return _cspace.csTriangleVertex_AddTriangle(*args)
    def AddVertex(*args): return _cspace.csTriangleVertex_AddVertex(*args)
csTriangleVertex_swigregister = _cspace.csTriangleVertex_swigregister
csTriangleVertex_swigregister(csTriangleVertex)

class csTriangleVertices(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csTriangleVertices, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csTriangleVertices, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csTriangleVertices(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csTriangleVertices
    __del__ = lambda self : None;
    def UpdateVertices(*args): return _cspace.csTriangleVertices_UpdateVertices(*args)
    def GetVertexCount(*args): return _cspace.csTriangleVertices_GetVertexCount(*args)
    def GetVertex(*args): return _cspace.csTriangleVertices_GetVertex(*args)
csTriangleVertices_swigregister = _cspace.csTriangleVertices_swigregister
csTriangleVertices_swigregister(csTriangleVertices)

class csTriangleMeshBox(pycsTriangleMeshBox):
    __swig_setmethods__ = {}
    for _s in [pycsTriangleMeshBox]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csTriangleMeshBox, name, value)
    __swig_getmethods__ = {}
    for _s in [pycsTriangleMeshBox]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csTriangleMeshBox, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csTriangleMeshBox(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csTriangleMeshBox
    __del__ = lambda self : None;
    def SetBox(*args): return _cspace.csTriangleMeshBox_SetBox(*args)
    def GetVertexCount(*args): return _cspace.csTriangleMeshBox_GetVertexCount(*args)
    def GetVertices(self):
      return CSMutableArrayHelper(self.GetVertexByIndex, self.GetVertexCount)


    def GetTriangleCount(*args): return _cspace.csTriangleMeshBox_GetTriangleCount(*args)
    def GetTriangles(self):
      return CSMutableArrayHelper(self.GetTriangleByIndex, self.GetTriangleCount)


    def Lock(*args): return _cspace.csTriangleMeshBox_Lock(*args)
    def Unlock(*args): return _cspace.csTriangleMeshBox_Unlock(*args)
    def GetFlags(*args): return _cspace.csTriangleMeshBox_GetFlags(*args)
    def GetChangeNumber(*args): return _cspace.csTriangleMeshBox_GetChangeNumber(*args)
csTriangleMeshBox_swigregister = _cspace.csTriangleMeshBox_swigregister
csTriangleMeshBox_swigregister(csTriangleMeshBox)

class csTriangleMeshPointer(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csTriangleMeshPointer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csTriangleMeshPointer, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csTriangleMeshPointer(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csTriangleMeshPointer
    __del__ = lambda self : None;
    def GetVertexCount(*args): return _cspace.csTriangleMeshPointer_GetVertexCount(*args)
    def GetVertices(*args): return _cspace.csTriangleMeshPointer_GetVertices(*args)
    def GetTriangleCount(*args): return _cspace.csTriangleMeshPointer_GetTriangleCount(*args)
    def GetTriangles(*args): return _cspace.csTriangleMeshPointer_GetTriangles(*args)
    def Lock(*args): return _cspace.csTriangleMeshPointer_Lock(*args)
    def Unlock(*args): return _cspace.csTriangleMeshPointer_Unlock(*args)
    def GetFlags(*args): return _cspace.csTriangleMeshPointer_GetFlags(*args)
    def GetChangeNumber(*args): return _cspace.csTriangleMeshPointer_GetChangeNumber(*args)
csTriangleMeshPointer_swigregister = _cspace.csTriangleMeshPointer_swigregister
csTriangleMeshPointer_swigregister(csTriangleMeshPointer)

class csIntArray(CustomAllocated):
    __swig_setmethods__ = {}
    for _s in [CustomAllocated]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csIntArray, name, value)
    __swig_getmethods__ = {}
    for _s in [CustomAllocated]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csIntArray, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_csIntArray
    __del__ = lambda self : None;
    def __init__(self, *args): 
        this = _cspace.new_csIntArray(*args)
        try: self.this.append(this)
        except: self.this = this
    def GetSize(*args): return _cspace.csIntArray_GetSize(*args)
    def Get(*args): return _cspace.csIntArray_Get(*args)
    def Put(*args): return _cspace.csIntArray_Put(*args)
    def Push(*args): return _cspace.csIntArray_Push(*args)
    def Pop(*args): return _cspace.csIntArray_Pop(*args)
    def Top(*args): return _cspace.csIntArray_Top(*args)
    def Insert(*args): return _cspace.csIntArray_Insert(*args)
    def Contains(*args): return _cspace.csIntArray_Contains(*args)
    def Truncate(*args): return _cspace.csIntArray_Truncate(*args)
    def Empty(*args): return _cspace.csIntArray_Empty(*args)
    def IsEmpty(*args): return _cspace.csIntArray_IsEmpty(*args)
    def SetMinimalCapacity(*args): return _cspace.csIntArray_SetMinimalCapacity(*args)
    def DeleteIndex(*args): return _cspace.csIntArray_DeleteIndex(*args)
    def DeleteIndexFast(*args): return _cspace.csIntArray_DeleteIndexFast(*args)
    def DeleteRange(*args): return _cspace.csIntArray_DeleteRange(*args)
    def __eq__(*args): return _cspace.csIntArray___eq__(*args)
    def __ne__(*args): return _cspace.csIntArray___ne__(*args)
    def GetAllocator(*args): return _cspace.csIntArray_GetAllocator(*args)
    def __getitem__(*args): return _cspace.csIntArray___getitem__(*args)
    def __contains__(*args): return _cspace.csIntArray___contains__(*args)
    def __delitem__(*args): return _cspace.csIntArray___delitem__(*args)
    def __len__(*args): return _cspace.csIntArray___len__(*args)
    def append(*args): return _cspace.csIntArray_append(*args)
    def content_iterator(self):
            for idx in xrange(len(self)):
                    yield self.__getitem__(idx)
    def __iter__(self): return self.content_iterator()  
csIntArray_swigregister = _cspace.csIntArray_swigregister
csIntArray_swigregister(csIntArray)

class csIntArrayArray(CustomAllocated):
    __swig_setmethods__ = {}
    for _s in [CustomAllocated]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csIntArrayArray, name, value)
    __swig_getmethods__ = {}
    for _s in [CustomAllocated]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csIntArrayArray, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_csIntArrayArray
    __del__ = lambda self : None;
    def __init__(self, *args): 
        this = _cspace.new_csIntArrayArray(*args)
        try: self.this.append(this)
        except: self.this = this
    def GetSize(*args): return _cspace.csIntArrayArray_GetSize(*args)
    def Get(*args): return _cspace.csIntArrayArray_Get(*args)
    def Put(*args): return _cspace.csIntArrayArray_Put(*args)
    def Push(*args): return _cspace.csIntArrayArray_Push(*args)
    def Pop(*args): return _cspace.csIntArrayArray_Pop(*args)
    def Top(*args): return _cspace.csIntArrayArray_Top(*args)
    def Insert(*args): return _cspace.csIntArrayArray_Insert(*args)
    def Truncate(*args): return _cspace.csIntArrayArray_Truncate(*args)
    def Empty(*args): return _cspace.csIntArrayArray_Empty(*args)
    def IsEmpty(*args): return _cspace.csIntArrayArray_IsEmpty(*args)
    def SetMinimalCapacity(*args): return _cspace.csIntArrayArray_SetMinimalCapacity(*args)
    def DeleteIndex(*args): return _cspace.csIntArrayArray_DeleteIndex(*args)
    def DeleteIndexFast(*args): return _cspace.csIntArrayArray_DeleteIndexFast(*args)
    def DeleteRange(*args): return _cspace.csIntArrayArray_DeleteRange(*args)
    def __eq__(*args): return _cspace.csIntArrayArray___eq__(*args)
    def __ne__(*args): return _cspace.csIntArrayArray___ne__(*args)
    def GetAllocator(*args): return _cspace.csIntArrayArray_GetAllocator(*args)
    def __getitem__(*args): return _cspace.csIntArrayArray___getitem__(*args)
    def __contains__(*args): return _cspace.csIntArrayArray___contains__(*args)
    def __delitem__(*args): return _cspace.csIntArrayArray___delitem__(*args)
    def __len__(*args): return _cspace.csIntArrayArray___len__(*args)
    def append(*args): return _cspace.csIntArrayArray_append(*args)
    def content_iterator(self):
            for idx in xrange(len(self)):
                    yield self.__getitem__(idx)
    def __iter__(self): return self.content_iterator()  
csIntArrayArray_swigregister = _cspace.csIntArrayArray_swigregister
csIntArrayArray_swigregister(csIntArrayArray)

class csTriangleMinMax(csTriangle):
    __swig_setmethods__ = {}
    for _s in [csTriangle]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csTriangleMinMax, name, value)
    __swig_getmethods__ = {}
    for _s in [csTriangle]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csTriangleMinMax, name)
    __repr__ = _swig_repr
    __swig_setmethods__["minx"] = _cspace.csTriangleMinMax_minx_set
    __swig_getmethods__["minx"] = _cspace.csTriangleMinMax_minx_get
    if _newclass:minx = property(_cspace.csTriangleMinMax_minx_get, _cspace.csTriangleMinMax_minx_set)
    __swig_setmethods__["maxx"] = _cspace.csTriangleMinMax_maxx_set
    __swig_getmethods__["maxx"] = _cspace.csTriangleMinMax_maxx_get
    if _newclass:maxx = property(_cspace.csTriangleMinMax_maxx_get, _cspace.csTriangleMinMax_maxx_set)
    def __init__(self, *args): 
        this = _cspace.new_csTriangleMinMax(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csTriangleMinMax
    __del__ = lambda self : None;
csTriangleMinMax_swigregister = _cspace.csTriangleMinMax_swigregister
csTriangleMinMax_swigregister(csTriangleMinMax)

class csTriangleMeshEdge(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csTriangleMeshEdge, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csTriangleMeshEdge, name)
    __repr__ = _swig_repr
    __swig_setmethods__["vt1"] = _cspace.csTriangleMeshEdge_vt1_set
    __swig_getmethods__["vt1"] = _cspace.csTriangleMeshEdge_vt1_get
    if _newclass:vt1 = property(_cspace.csTriangleMeshEdge_vt1_get, _cspace.csTriangleMeshEdge_vt1_set)
    __swig_setmethods__["vt2"] = _cspace.csTriangleMeshEdge_vt2_set
    __swig_getmethods__["vt2"] = _cspace.csTriangleMeshEdge_vt2_get
    if _newclass:vt2 = property(_cspace.csTriangleMeshEdge_vt2_get, _cspace.csTriangleMeshEdge_vt2_set)
    __swig_setmethods__["tri1"] = _cspace.csTriangleMeshEdge_tri1_set
    __swig_getmethods__["tri1"] = _cspace.csTriangleMeshEdge_tri1_get
    if _newclass:tri1 = property(_cspace.csTriangleMeshEdge_tri1_get, _cspace.csTriangleMeshEdge_tri1_set)
    __swig_setmethods__["tri2"] = _cspace.csTriangleMeshEdge_tri2_set
    __swig_getmethods__["tri2"] = _cspace.csTriangleMeshEdge_tri2_get
    if _newclass:tri2 = property(_cspace.csTriangleMeshEdge_tri2_get, _cspace.csTriangleMeshEdge_tri2_set)
    __swig_setmethods__["active"] = _cspace.csTriangleMeshEdge_active_set
    __swig_getmethods__["active"] = _cspace.csTriangleMeshEdge_active_get
    if _newclass:active = property(_cspace.csTriangleMeshEdge_active_get, _cspace.csTriangleMeshEdge_active_set)
    def __init__(self, *args): 
        this = _cspace.new_csTriangleMeshEdge(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csTriangleMeshEdge
    __del__ = lambda self : None;
csTriangleMeshEdge_swigregister = _cspace.csTriangleMeshEdge_swigregister
csTriangleMeshEdge_swigregister(csTriangleMeshEdge)

class csTriangleMeshTools(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csTriangleMeshTools, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csTriangleMeshTools, name)
    __repr__ = _swig_repr
    __swig_getmethods__["CalculateNormals"] = lambda x: _cspace.csTriangleMeshTools_CalculateNormals
    if _newclass:CalculateNormals = staticmethod(_cspace.csTriangleMeshTools_CalculateNormals)
    __swig_getmethods__["CalculatePlanes"] = lambda x: _cspace.csTriangleMeshTools_CalculatePlanes
    if _newclass:CalculatePlanes = staticmethod(_cspace.csTriangleMeshTools_CalculatePlanes)
    __swig_getmethods__["CalculateEdges"] = lambda x: _cspace.csTriangleMeshTools_CalculateEdges
    if _newclass:CalculateEdges = staticmethod(_cspace.csTriangleMeshTools_CalculateEdges)
    __swig_getmethods__["CheckActiveEdges"] = lambda x: _cspace.csTriangleMeshTools_CheckActiveEdges
    if _newclass:CheckActiveEdges = staticmethod(_cspace.csTriangleMeshTools_CheckActiveEdges)
    __swig_getmethods__["CalculateOutline"] = lambda x: _cspace.csTriangleMeshTools_CalculateOutline
    if _newclass:CalculateOutline = staticmethod(_cspace.csTriangleMeshTools_CalculateOutline)
    __swig_getmethods__["IsMeshClosed"] = lambda x: _cspace.csTriangleMeshTools_IsMeshClosed
    if _newclass:IsMeshClosed = staticmethod(_cspace.csTriangleMeshTools_IsMeshClosed)
    __swig_getmethods__["IsMeshConvex"] = lambda x: _cspace.csTriangleMeshTools_IsMeshConvex
    if _newclass:IsMeshConvex = staticmethod(_cspace.csTriangleMeshTools_IsMeshConvex)
    __swig_getmethods__["CloseMesh"] = lambda x: _cspace.csTriangleMeshTools_CloseMesh
    if _newclass:CloseMesh = staticmethod(_cspace.csTriangleMeshTools_CloseMesh)
    __swig_getmethods__["SortTrianglesX"] = lambda x: _cspace.csTriangleMeshTools_SortTrianglesX
    if _newclass:SortTrianglesX = staticmethod(_cspace.csTriangleMeshTools_SortTrianglesX)
    __swig_getmethods__["PointInClosedMesh"] = lambda x: _cspace.csTriangleMeshTools_PointInClosedMesh
    if _newclass:PointInClosedMesh = staticmethod(_cspace.csTriangleMeshTools_PointInClosedMesh)
    __swig_getmethods__["LineInClosedMesh"] = lambda x: _cspace.csTriangleMeshTools_LineInClosedMesh
    if _newclass:LineInClosedMesh = staticmethod(_cspace.csTriangleMeshTools_LineInClosedMesh)
    __swig_getmethods__["BoxInClosedMesh"] = lambda x: _cspace.csTriangleMeshTools_BoxInClosedMesh
    if _newclass:BoxInClosedMesh = staticmethod(_cspace.csTriangleMeshTools_BoxInClosedMesh)
    __swig_getmethods__["CalculateVertexConnections"] = lambda x: _cspace.csTriangleMeshTools_CalculateVertexConnections
    if _newclass:CalculateVertexConnections = staticmethod(_cspace.csTriangleMeshTools_CalculateVertexConnections)
    def __init__(self, *args): 
        this = _cspace.new_csTriangleMeshTools(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csTriangleMeshTools
    __del__ = lambda self : None;
csTriangleMeshTools_swigregister = _cspace.csTriangleMeshTools_swigregister
csTriangleMeshTools_swigregister(csTriangleMeshTools)
csTriangleMeshTools_CalculateNormals = _cspace.csTriangleMeshTools_CalculateNormals
csTriangleMeshTools_CalculatePlanes = _cspace.csTriangleMeshTools_CalculatePlanes
csTriangleMeshTools_CalculateEdges = _cspace.csTriangleMeshTools_CalculateEdges
csTriangleMeshTools_CheckActiveEdges = _cspace.csTriangleMeshTools_CheckActiveEdges
csTriangleMeshTools_CalculateOutline = _cspace.csTriangleMeshTools_CalculateOutline
csTriangleMeshTools_IsMeshClosed = _cspace.csTriangleMeshTools_IsMeshClosed
csTriangleMeshTools_IsMeshConvex = _cspace.csTriangleMeshTools_IsMeshConvex
csTriangleMeshTools_CloseMesh = _cspace.csTriangleMeshTools_CloseMesh
csTriangleMeshTools_SortTrianglesX = _cspace.csTriangleMeshTools_SortTrianglesX
csTriangleMeshTools_PointInClosedMesh = _cspace.csTriangleMeshTools_PointInClosedMesh
csTriangleMeshTools_LineInClosedMesh = _cspace.csTriangleMeshTools_LineInClosedMesh
csTriangleMeshTools_BoxInClosedMesh = _cspace.csTriangleMeshTools_BoxInClosedMesh
csTriangleMeshTools_CalculateVertexConnections = _cspace.csTriangleMeshTools_CalculateVertexConnections

class iFrustumViewUserdata(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iFrustumViewUserdata, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iFrustumViewUserdata, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iFrustumViewUserdata_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iFrustumViewUserdata_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iFrustumViewUserdata
    __del__ = lambda self : None;
iFrustumViewUserdata_swigregister = _cspace.iFrustumViewUserdata_swigregister
iFrustumViewUserdata_swigregister(iFrustumViewUserdata)
iFrustumViewUserdata_scfGetVersion = _cspace.iFrustumViewUserdata_scfGetVersion

class csFrustumContext(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csFrustumContext, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csFrustumContext, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csFrustumContext(*args)
        try: self.this.append(this)
        except: self.this = this
    def assign(*args): return _cspace.csFrustumContext_assign(*args)
    def GetShadows(*args): return _cspace.csFrustumContext_GetShadows(*args)
    def SetShadows(*args): return _cspace.csFrustumContext_SetShadows(*args)
    def SetNewShadows(*args): return _cspace.csFrustumContext_SetNewShadows(*args)
    def IsShared(*args): return _cspace.csFrustumContext_IsShared(*args)
    def SetLightFrustum(*args): return _cspace.csFrustumContext_SetLightFrustum(*args)
    def SetNewLightFrustum(*args): return _cspace.csFrustumContext_SetNewLightFrustum(*args)
    def GetLightFrustum(*args): return _cspace.csFrustumContext_GetLightFrustum(*args)
    def SetMirrored(*args): return _cspace.csFrustumContext_SetMirrored(*args)
    def IsMirrored(*args): return _cspace.csFrustumContext_IsMirrored(*args)
    __swig_destroy__ = _cspace.delete_csFrustumContext
    __del__ = lambda self : None;
csFrustumContext_swigregister = _cspace.csFrustumContext_swigregister
csFrustumContext_swigregister(csFrustumContext)

class iFrustumView(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iFrustumView, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iFrustumView, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetFrustumContext(*args): return _cspace.iFrustumView_GetFrustumContext(*args)
    def CreateFrustumContext(*args): return _cspace.iFrustumView_CreateFrustumContext(*args)
    def CopyFrustumContext(*args): return _cspace.iFrustumView_CopyFrustumContext(*args)
    def SetFrustumContext(*args): return _cspace.iFrustumView_SetFrustumContext(*args)
    def RestoreFrustumContext(*args): return _cspace.iFrustumView_RestoreFrustumContext(*args)
    def SetObjectFunction(*args): return _cspace.iFrustumView_SetObjectFunction(*args)
    def CallObjectFunction(*args): return _cspace.iFrustumView_CallObjectFunction(*args)
    def GetRadius(*args): return _cspace.iFrustumView_GetRadius(*args)
    def GetSquaredRadius(*args): return _cspace.iFrustumView_GetSquaredRadius(*args)
    def CheckShadowMask(*args): return _cspace.iFrustumView_CheckShadowMask(*args)
    def CheckProcessMask(*args): return _cspace.iFrustumView_CheckProcessMask(*args)
    def StartNewShadowBlock(*args): return _cspace.iFrustumView_StartNewShadowBlock(*args)
    def SetUserdata(*args): return _cspace.iFrustumView_SetUserdata(*args)
    def GetUserdata(*args): return _cspace.iFrustumView_GetUserdata(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iFrustumView_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iFrustumView_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iFrustumView
    __del__ = lambda self : None;
iFrustumView_swigregister = _cspace.iFrustumView_swigregister
iFrustumView_swigregister(iFrustumView)
iFrustumView_scfGetVersion = _cspace.iFrustumView_scfGetVersion

CS_LIGHT_ACTIVEHALO = _cspace.CS_LIGHT_ACTIVEHALO
CS_LIGHT_DYNAMICTYPE_STATIC = _cspace.CS_LIGHT_DYNAMICTYPE_STATIC
CS_LIGHT_DYNAMICTYPE_PSEUDO = _cspace.CS_LIGHT_DYNAMICTYPE_PSEUDO
CS_LIGHT_DYNAMICTYPE_DYNAMIC = _cspace.CS_LIGHT_DYNAMICTYPE_DYNAMIC
CS_DEFAULT_LIGHT_LEVEL = _cspace.CS_DEFAULT_LIGHT_LEVEL
CS_NORMAL_LIGHT_LEVEL = _cspace.CS_NORMAL_LIGHT_LEVEL
CS_ATTN_NONE = _cspace.CS_ATTN_NONE
CS_ATTN_LINEAR = _cspace.CS_ATTN_LINEAR
CS_ATTN_INVERSE = _cspace.CS_ATTN_INVERSE
CS_ATTN_REALISTIC = _cspace.CS_ATTN_REALISTIC
CS_ATTN_CLQ = _cspace.CS_ATTN_CLQ
CS_LIGHT_POINTLIGHT = _cspace.CS_LIGHT_POINTLIGHT
CS_LIGHT_DIRECTIONAL = _cspace.CS_LIGHT_DIRECTIONAL
CS_LIGHT_SPOTLIGHT = _cspace.CS_LIGHT_SPOTLIGHT
class iLightCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iLightCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iLightCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def OnColorChange(*args): return _cspace.iLightCallback_OnColorChange(*args)
    def OnPositionChange(*args): return _cspace.iLightCallback_OnPositionChange(*args)
    def OnSectorChange(*args): return _cspace.iLightCallback_OnSectorChange(*args)
    def OnRadiusChange(*args): return _cspace.iLightCallback_OnRadiusChange(*args)
    def OnDestroy(*args): return _cspace.iLightCallback_OnDestroy(*args)
    def OnAttenuationChange(*args): return _cspace.iLightCallback_OnAttenuationChange(*args)
    __swig_destroy__ = _cspace.delete_iLightCallback
    __del__ = lambda self : None;
iLightCallback_swigregister = _cspace.iLightCallback_swigregister
iLightCallback_swigregister(iLightCallback)

class iLight(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iLight, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iLight, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetLightID(*args): return _cspace.iLight_GetLightID(*args)
    def QueryObject(*args): return _cspace.iLight_QueryObject(*args)
    def GetDynamicType(*args): return _cspace.iLight_GetDynamicType(*args)
    def GetCenter(*args): return _cspace.iLight_GetCenter(*args)
    def GetFullCenter(*args): return _cspace.iLight_GetFullCenter(*args)
    def SetCenter(*args): return _cspace.iLight_SetCenter(*args)
    def GetSector(*args): return _cspace.iLight_GetSector(*args)
    def GetMovable(*args): return _cspace.iLight_GetMovable(*args)
    def QuerySceneNode(*args): return _cspace.iLight_QuerySceneNode(*args)
    def GetColor(*args): return _cspace.iLight_GetColor(*args)
    def SetColor(*args): return _cspace.iLight_SetColor(*args)
    def GetSpecularColor(*args): return _cspace.iLight_GetSpecularColor(*args)
    def SetSpecularColor(*args): return _cspace.iLight_SetSpecularColor(*args)
    def GetType(*args): return _cspace.iLight_GetType(*args)
    def SetType(*args): return _cspace.iLight_SetType(*args)
    def GetAttenuationMode(*args): return _cspace.iLight_GetAttenuationMode(*args)
    def SetAttenuationMode(*args): return _cspace.iLight_SetAttenuationMode(*args)
    def SetAttenuationConstants(*args): return _cspace.iLight_SetAttenuationConstants(*args)
    def GetAttenuationConstants(*args): return _cspace.iLight_GetAttenuationConstants(*args)
    def GetCutoffDistance(*args): return _cspace.iLight_GetCutoffDistance(*args)
    def SetCutoffDistance(*args): return _cspace.iLight_SetCutoffDistance(*args)
    def GetDirectionalCutoffRadius(*args): return _cspace.iLight_GetDirectionalCutoffRadius(*args)
    def SetDirectionalCutoffRadius(*args): return _cspace.iLight_SetDirectionalCutoffRadius(*args)
    def SetSpotLightFalloff(*args): return _cspace.iLight_SetSpotLightFalloff(*args)
    def GetSpotLightFalloff(*args): return _cspace.iLight_GetSpotLightFalloff(*args)
    def CreateCrossHalo(*args): return _cspace.iLight_CreateCrossHalo(*args)
    def CreateNovaHalo(*args): return _cspace.iLight_CreateNovaHalo(*args)
    def CreateFlareHalo(*args): return _cspace.iLight_CreateFlareHalo(*args)
    def GetHalo(*args): return _cspace.iLight_GetHalo(*args)
    def GetBrightnessAtDistance(*args): return _cspace.iLight_GetBrightnessAtDistance(*args)
    def GetFlags(*args): return _cspace.iLight_GetFlags(*args)
    def SetLightCallback(*args): return _cspace.iLight_SetLightCallback(*args)
    def RemoveLightCallback(*args): return _cspace.iLight_RemoveLightCallback(*args)
    def GetLightCallbackCount(*args): return _cspace.iLight_GetLightCallbackCount(*args)
    def GetLightCallback(*args): return _cspace.iLight_GetLightCallback(*args)
    def GetLightNumber(*args): return _cspace.iLight_GetLightNumber(*args)
    def AddAffectedLightingInfo(*args): return _cspace.iLight_AddAffectedLightingInfo(*args)
    def RemoveAffectedLightingInfo(*args): return _cspace.iLight_RemoveAffectedLightingInfo(*args)
    def Setup(*args): return _cspace.iLight_Setup(*args)
    def GetSVContext(*args): return _cspace.iLight_GetSVContext(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iLight_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iLight_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iLight
    __del__ = lambda self : None;
iLight_swigregister = _cspace.iLight_swigregister
iLight_swigregister(iLight)
iLight_scfGetVersion = _cspace.iLight_scfGetVersion

class iLightList(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iLightList, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iLightList, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetCount(*args): return _cspace.iLightList_GetCount(*args)
    def Get(*args): return _cspace.iLightList_Get(*args)
    def Add(*args): return _cspace.iLightList_Add(*args)
    def Remove(*args): return _cspace.iLightList_Remove(*args)
    def RemoveAll(*args): return _cspace.iLightList_RemoveAll(*args)
    def Find(*args): return _cspace.iLightList_Find(*args)
    def FindByName(*args): return _cspace.iLightList_FindByName(*args)
    def FindByID(*args): return _cspace.iLightList_FindByID(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iLightList_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iLightList_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iLightList
    __del__ = lambda self : None;
    def __delitem__(*args): return _cspace.iLightList___delitem__(*args)
    def __len__(*args): return _cspace.iLightList___len__(*args)
    def append(*args): return _cspace.iLightList_append(*args)
    def content_iterator(self):
            for idx in xrange(len(self)):
                    yield self.__getitem__(idx)
    def __iter__(self): return self.content_iterator()  
    def __getitem__(*args): return _cspace.iLightList___getitem__(*args)
    def __contains__(*args): return _cspace.iLightList___contains__(*args)
iLightList_swigregister = _cspace.iLightList_swigregister
iLightList_swigregister(iLightList)
iLightList_scfGetVersion = _cspace.iLightList_scfGetVersion

class iLightingProcessData(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iLightingProcessData, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iLightingProcessData, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def FinalizeLighting(*args): return _cspace.iLightingProcessData_FinalizeLighting(*args)
    __swig_destroy__ = _cspace.delete_iLightingProcessData
    __del__ = lambda self : None;
iLightingProcessData_swigregister = _cspace.iLightingProcessData_swigregister
iLightingProcessData_swigregister(iLightingProcessData)

class iLightingProcessInfo(iFrustumViewUserdata):
    __swig_setmethods__ = {}
    for _s in [iFrustumViewUserdata]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iLightingProcessInfo, name, value)
    __swig_getmethods__ = {}
    for _s in [iFrustumViewUserdata]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iLightingProcessInfo, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetLight(*args): return _cspace.iLightingProcessInfo_GetLight(*args)
    def IsDynamic(*args): return _cspace.iLightingProcessInfo_IsDynamic(*args)
    def SetColor(*args): return _cspace.iLightingProcessInfo_SetColor(*args)
    def GetColor(*args): return _cspace.iLightingProcessInfo_GetColor(*args)
    def AttachUserdata(*args): return _cspace.iLightingProcessInfo_AttachUserdata(*args)
    def QueryUserdata(*args): return _cspace.iLightingProcessInfo_QueryUserdata(*args)
    def FinalizeLighting(*args): return _cspace.iLightingProcessInfo_FinalizeLighting(*args)
    __swig_destroy__ = _cspace.delete_iLightingProcessInfo
    __del__ = lambda self : None;
iLightingProcessInfo_swigregister = _cspace.iLightingProcessInfo_swigregister
iLightingProcessInfo_swigregister(iLightingProcessInfo)

class iLightIterator(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iLightIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iLightIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def HasNext(*args): return _cspace.iLightIterator_HasNext(*args)
    def Next(*args): return _cspace.iLightIterator_Next(*args)
    def GetLastSector(*args): return _cspace.iLightIterator_GetLastSector(*args)
    def Reset(*args): return _cspace.iLightIterator_Reset(*args)
    def __iter__(self):
        while self.HasNext():
            yield self.Next() 
    __swig_destroy__ = _cspace.delete_iLightIterator
    __del__ = lambda self : None;
iLightIterator_swigregister = _cspace.iLightIterator_swigregister
iLightIterator_swigregister(iLightIterator)

CS_FOG_MODE_NONE = _cspace.CS_FOG_MODE_NONE
CS_FOG_MODE_LINEAR = _cspace.CS_FOG_MODE_LINEAR
CS_FOG_MODE_EXP = _cspace.CS_FOG_MODE_EXP
CS_FOG_MODE_EXP2 = _cspace.CS_FOG_MODE_EXP2
CS_FOG_MODE_CRYSTALSPACE = _cspace.CS_FOG_MODE_CRYSTALSPACE
class csFog(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csFog, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csFog, name)
    __repr__ = _swig_repr
    __swig_setmethods__["density"] = _cspace.csFog_density_set
    __swig_getmethods__["density"] = _cspace.csFog_density_get
    if _newclass:density = property(_cspace.csFog_density_get, _cspace.csFog_density_set)
    __swig_setmethods__["color"] = _cspace.csFog_color_set
    __swig_getmethods__["color"] = _cspace.csFog_color_get
    if _newclass:color = property(_cspace.csFog_color_get, _cspace.csFog_color_set)
    __swig_setmethods__["start"] = _cspace.csFog_start_set
    __swig_getmethods__["start"] = _cspace.csFog_start_get
    if _newclass:start = property(_cspace.csFog_start_get, _cspace.csFog_start_set)
    __swig_setmethods__["end"] = _cspace.csFog_end_set
    __swig_getmethods__["end"] = _cspace.csFog_end_get
    if _newclass:end = property(_cspace.csFog_end_get, _cspace.csFog_end_set)
    __swig_setmethods__["mode"] = _cspace.csFog_mode_set
    __swig_getmethods__["mode"] = _cspace.csFog_mode_get
    if _newclass:mode = property(_cspace.csFog_mode_get, _cspace.csFog_mode_set)
    def __init__(self, *args): 
        this = _cspace.new_csFog(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csFog
    __del__ = lambda self : None;
csFog_swigregister = _cspace.csFog_swigregister
csFog_swigregister(csFog)

class iSectorCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSectorCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSectorCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Traverse(*args): return _cspace.iSectorCallback_Traverse(*args)
    __swig_destroy__ = _cspace.delete_iSectorCallback
    __del__ = lambda self : None;
iSectorCallback_swigregister = _cspace.iSectorCallback_swigregister
iSectorCallback_swigregister(iSectorCallback)

class iSectorMeshCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSectorMeshCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSectorMeshCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def NewMesh(*args): return _cspace.iSectorMeshCallback_NewMesh(*args)
    def RemoveMesh(*args): return _cspace.iSectorMeshCallback_RemoveMesh(*args)
    __swig_destroy__ = _cspace.delete_iSectorMeshCallback
    __del__ = lambda self : None;
iSectorMeshCallback_swigregister = _cspace.iSectorMeshCallback_swigregister
iSectorMeshCallback_swigregister(iSectorMeshCallback)

class iLightVisibleCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iLightVisibleCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iLightVisibleCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def LightVisible(*args): return _cspace.iLightVisibleCallback_LightVisible(*args)
    __swig_destroy__ = _cspace.delete_iLightVisibleCallback
    __del__ = lambda self : None;
iLightVisibleCallback_swigregister = _cspace.iLightVisibleCallback_swigregister
iLightVisibleCallback_swigregister(iLightVisibleCallback)

class csSectorHitBeamResult(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSectorHitBeamResult, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csSectorHitBeamResult, name)
    __repr__ = _swig_repr
    __swig_setmethods__["mesh"] = _cspace.csSectorHitBeamResult_mesh_set
    __swig_getmethods__["mesh"] = _cspace.csSectorHitBeamResult_mesh_get
    if _newclass:mesh = property(_cspace.csSectorHitBeamResult_mesh_get, _cspace.csSectorHitBeamResult_mesh_set)
    __swig_setmethods__["isect"] = _cspace.csSectorHitBeamResult_isect_set
    __swig_getmethods__["isect"] = _cspace.csSectorHitBeamResult_isect_get
    if _newclass:isect = property(_cspace.csSectorHitBeamResult_isect_get, _cspace.csSectorHitBeamResult_isect_set)
    __swig_setmethods__["polygon_idx"] = _cspace.csSectorHitBeamResult_polygon_idx_set
    __swig_getmethods__["polygon_idx"] = _cspace.csSectorHitBeamResult_polygon_idx_get
    if _newclass:polygon_idx = property(_cspace.csSectorHitBeamResult_polygon_idx_get, _cspace.csSectorHitBeamResult_polygon_idx_set)
    __swig_setmethods__["final_sector"] = _cspace.csSectorHitBeamResult_final_sector_set
    __swig_getmethods__["final_sector"] = _cspace.csSectorHitBeamResult_final_sector_get
    if _newclass:final_sector = property(_cspace.csSectorHitBeamResult_final_sector_get, _cspace.csSectorHitBeamResult_final_sector_set)
    def __init__(self, *args): 
        this = _cspace.new_csSectorHitBeamResult(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csSectorHitBeamResult
    __del__ = lambda self : None;
csSectorHitBeamResult_swigregister = _cspace.csSectorHitBeamResult_swigregister
csSectorHitBeamResult_swigregister(csSectorHitBeamResult)

class iSector(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSector, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSector, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iSector_QueryObject(*args)
    def GetMeshes(*args): return _cspace.iSector_GetMeshes(*args)
    def GetVisibleMeshes(*args): return _cspace.iSector_GetVisibleMeshes(*args)
    def GetPortalMeshes(*args): return _cspace.iSector_GetPortalMeshes(*args)
    def UnlinkObjects(*args): return _cspace.iSector_UnlinkObjects(*args)
    def AddSectorMeshCallback(*args): return _cspace.iSector_AddSectorMeshCallback(*args)
    def RemoveSectorMeshCallback(*args): return _cspace.iSector_RemoveSectorMeshCallback(*args)
    def Draw(*args): return _cspace.iSector_Draw(*args)
    def PrepareDraw(*args): return _cspace.iSector_PrepareDraw(*args)
    def GetRecLevel(*args): return _cspace.iSector_GetRecLevel(*args)
    def IncRecLevel(*args): return _cspace.iSector_IncRecLevel(*args)
    def DecRecLevel(*args): return _cspace.iSector_DecRecLevel(*args)
    def SetRenderLoop(*args): return _cspace.iSector_SetRenderLoop(*args)
    def GetRenderLoop(*args): return _cspace.iSector_GetRenderLoop(*args)
    def CreateMeshGenerator(*args): return _cspace.iSector_CreateMeshGenerator(*args)
    def GetMeshGeneratorCount(*args): return _cspace.iSector_GetMeshGeneratorCount(*args)
    def GetMeshGenerator(*args): return _cspace.iSector_GetMeshGenerator(*args)
    def GetMeshGeneratorByName(*args): return _cspace.iSector_GetMeshGeneratorByName(*args)
    def RemoveMeshGenerator(*args): return _cspace.iSector_RemoveMeshGenerator(*args)
    def RemoveMeshGenerators(*args): return _cspace.iSector_RemoveMeshGenerators(*args)
    def HasFog(*args): return _cspace.iSector_HasFog(*args)
    def GetFog(*args): return _cspace.iSector_GetFog(*args)
    def SetFog(*args): return _cspace.iSector_SetFog(*args)
    def DisableFog(*args): return _cspace.iSector_DisableFog(*args)
    def GetLights(*args): return _cspace.iSector_GetLights(*args)
    def ShineLights(*args): return _cspace.iSector_ShineLights(*args)
    def SetDynamicAmbientLight(*args): return _cspace.iSector_SetDynamicAmbientLight(*args)
    def GetDynamicAmbientLight(*args): return _cspace.iSector_GetDynamicAmbientLight(*args)
    def GetDynamicAmbientVersion(*args): return _cspace.iSector_GetDynamicAmbientVersion(*args)
    def CalculateSectorBBox(*args): return _cspace.iSector_CalculateSectorBBox(*args)
    def SetVisibilityCullerPlugin(*args): return _cspace.iSector_SetVisibilityCullerPlugin(*args)
    def GetVisibilityCuller(*args): return _cspace.iSector_GetVisibilityCuller(*args)
    def CheckFrustum(*args): return _cspace.iSector_CheckFrustum(*args)
    def HitBeamPortals(*args): return _cspace.iSector_HitBeamPortals(*args)
    def HitBeam(*args): return _cspace.iSector_HitBeam(*args)
    def FollowSegment(*args): return _cspace.iSector_FollowSegment(*args)
    def SetSectorCallback(*args): return _cspace.iSector_SetSectorCallback(*args)
    def RemoveSectorCallback(*args): return _cspace.iSector_RemoveSectorCallback(*args)
    def GetSectorCallbackCount(*args): return _cspace.iSector_GetSectorCallbackCount(*args)
    def GetSectorCallback(*args): return _cspace.iSector_GetSectorCallback(*args)
    def SetLightCulling(*args): return _cspace.iSector_SetLightCulling(*args)
    def IsLightCullingEnabled(*args): return _cspace.iSector_IsLightCullingEnabled(*args)
    def AddLightVisibleCallback(*args): return _cspace.iSector_AddLightVisibleCallback(*args)
    def RemoveLightVisibleCallback(*args): return _cspace.iSector_RemoveLightVisibleCallback(*args)
    def GetSVContext(*args): return _cspace.iSector_GetSVContext(*args)
    def PrecacheDraw(*args): return _cspace.iSector_PrecacheDraw(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSector_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSector_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSector
    __del__ = lambda self : None;
iSector_swigregister = _cspace.iSector_swigregister
iSector_swigregister(iSector)
iSector_scfGetVersion = _cspace.iSector_scfGetVersion

class iSectorList(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSectorList, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSectorList, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetCount(*args): return _cspace.iSectorList_GetCount(*args)
    def Get(*args): return _cspace.iSectorList_Get(*args)
    def Add(*args): return _cspace.iSectorList_Add(*args)
    def Remove(*args): return _cspace.iSectorList_Remove(*args)
    def RemoveAll(*args): return _cspace.iSectorList_RemoveAll(*args)
    def Find(*args): return _cspace.iSectorList_Find(*args)
    def FindByName(*args): return _cspace.iSectorList_FindByName(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSectorList_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSectorList_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSectorList
    __del__ = lambda self : None;
    def __delitem__(*args): return _cspace.iSectorList___delitem__(*args)
    def __len__(*args): return _cspace.iSectorList___len__(*args)
    def append(*args): return _cspace.iSectorList_append(*args)
    def content_iterator(self):
            for idx in xrange(len(self)):
                    yield self.__getitem__(idx)
    def __iter__(self): return self.content_iterator()  
    def __getitem__(*args): return _cspace.iSectorList___getitem__(*args)
    def __contains__(*args): return _cspace.iSectorList___contains__(*args)
iSectorList_swigregister = _cspace.iSectorList_swigregister
iSectorList_swigregister(iSectorList)
iSectorList_scfGetVersion = _cspace.iSectorList_scfGetVersion

class iSectorIterator(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSectorIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSectorIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def HasNext(*args): return _cspace.iSectorIterator_HasNext(*args)
    def Next(*args): return _cspace.iSectorIterator_Next(*args)
    def GetLastPosition(*args): return _cspace.iSectorIterator_GetLastPosition(*args)
    def Reset(*args): return _cspace.iSectorIterator_Reset(*args)
    def __iter__(self):
        while self.HasNext():
            yield self.Next() 
    __swig_destroy__ = _cspace.delete_iSectorIterator
    __del__ = lambda self : None;
iSectorIterator_swigregister = _cspace.iSectorIterator_swigregister
iSectorIterator_swigregister(iSectorIterator)

CS_ENGINE_CACHE_READ = _cspace.CS_ENGINE_CACHE_READ
CS_ENGINE_CACHE_WRITE = _cspace.CS_ENGINE_CACHE_WRITE
CS_ENGINE_CACHE_NOUPDATE = _cspace.CS_ENGINE_CACHE_NOUPDATE
CS_RENDPRI_SORT_NONE = _cspace.CS_RENDPRI_SORT_NONE
CS_RENDPRI_SORT_BACK2FRONT = _cspace.CS_RENDPRI_SORT_BACK2FRONT
CS_RENDPRI_SORT_FRONT2BACK = _cspace.CS_RENDPRI_SORT_FRONT2BACK
class iEngineFrameCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iEngineFrameCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iEngineFrameCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def StartFrame(*args): return _cspace.iEngineFrameCallback_StartFrame(*args)
    __swig_destroy__ = _cspace.delete_iEngineFrameCallback
    __del__ = lambda self : None;
iEngineFrameCallback_swigregister = _cspace.iEngineFrameCallback_swigregister
iEngineFrameCallback_swigregister(iEngineFrameCallback)

class iEngineSectorCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iEngineSectorCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iEngineSectorCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def NewSector(*args): return _cspace.iEngineSectorCallback_NewSector(*args)
    def RemoveSector(*args): return _cspace.iEngineSectorCallback_RemoveSector(*args)
    __swig_destroy__ = _cspace.delete_iEngineSectorCallback
    __del__ = lambda self : None;
iEngineSectorCallback_swigregister = _cspace.iEngineSectorCallback_swigregister
iEngineSectorCallback_swigregister(iEngineSectorCallback)

class iEngine(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iEngine, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iEngine, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iEngine_QueryObject(*args)
    def Prepare(*args): return _cspace.iEngine_Prepare(*args)
    def PrepareTextures(*args): return _cspace.iEngine_PrepareTextures(*args)
    def PrepareMeshes(*args): return _cspace.iEngine_PrepareMeshes(*args)
    def ForceRelight(*args): return _cspace.iEngine_ForceRelight(*args)
    def ShineLights(*args): return _cspace.iEngine_ShineLights(*args)
    def SetLightingCacheMode(*args): return _cspace.iEngine_SetLightingCacheMode(*args)
    def GetLightingCacheMode(*args): return _cspace.iEngine_GetLightingCacheMode(*args)
    def SetCacheManager(*args): return _cspace.iEngine_SetCacheManager(*args)
    def SetVFSCacheManager(*args): return _cspace.iEngine_SetVFSCacheManager(*args)
    def GetCacheManager(*args): return _cspace.iEngine_GetCacheManager(*args)
    def SetMaxLightmapSize(*args): return _cspace.iEngine_SetMaxLightmapSize(*args)
    def GetMaxLightmapSize(*args): return _cspace.iEngine_GetMaxLightmapSize(*args)
    def GetDefaultMaxLightmapSize(*args): return _cspace.iEngine_GetDefaultMaxLightmapSize(*args)
    def GetMaxLightmapAspectRatio(*args): return _cspace.iEngine_GetMaxLightmapAspectRatio(*args)
    def RegisterRenderPriority(*args): return _cspace.iEngine_RegisterRenderPriority(*args)
    def GetRenderPriority(*args): return _cspace.iEngine_GetRenderPriority(*args)
    def GetRenderPrioritySorting(*args): return _cspace.iEngine_GetRenderPrioritySorting(*args)
    def GetSkyRenderPriority(*args): return _cspace.iEngine_GetSkyRenderPriority(*args)
    def GetPortalRenderPriority(*args): return _cspace.iEngine_GetPortalRenderPriority(*args)
    def GetWallRenderPriority(*args): return _cspace.iEngine_GetWallRenderPriority(*args)
    def GetObjectRenderPriority(*args): return _cspace.iEngine_GetObjectRenderPriority(*args)
    def GetAlphaRenderPriority(*args): return _cspace.iEngine_GetAlphaRenderPriority(*args)
    def ClearRenderPriorities(*args): return _cspace.iEngine_ClearRenderPriorities(*args)
    def GetRenderPriorityCount(*args): return _cspace.iEngine_GetRenderPriorityCount(*args)
    def GetRenderPriorityName(*args): return _cspace.iEngine_GetRenderPriorityName(*args)
    def CreateBaseMaterial(*args): return _cspace.iEngine_CreateBaseMaterial(*args)
    def CreateMaterial(*args): return _cspace.iEngine_CreateMaterial(*args)
    def GetMaterialList(*args): return _cspace.iEngine_GetMaterialList(*args)
    def FindMaterial(*args): return _cspace.iEngine_FindMaterial(*args)
    def CreateTexture(*args): return _cspace.iEngine_CreateTexture(*args)
    def CreateBlackTexture(*args): return _cspace.iEngine_CreateBlackTexture(*args)
    def GetTextureFormat(*args): return _cspace.iEngine_GetTextureFormat(*args)
    def GetTextureList(*args): return _cspace.iEngine_GetTextureList(*args)
    def FindTexture(*args): return _cspace.iEngine_FindTexture(*args)
    def CreateLight(*args): return _cspace.iEngine_CreateLight(*args)
    def FindLight(*args): return _cspace.iEngine_FindLight(*args)
    def FindLightID(*args): return _cspace.iEngine_FindLightID(*args)
    def GetLightIterator(*args): return _cspace.iEngine_GetLightIterator(*args)
    def RemoveLight(*args): return _cspace.iEngine_RemoveLight(*args)
    def SetAmbientLight(*args): return _cspace.iEngine_SetAmbientLight(*args)
    def GetAmbientLight(*args): return _cspace.iEngine_GetAmbientLight(*args)
    def GetDefaultAmbientLight(*args): return _cspace.iEngine_GetDefaultAmbientLight(*args)
    def GetNearbyLights(*args): return _cspace.iEngine_GetNearbyLights(*args)
    def CreateSector(*args): return _cspace.iEngine_CreateSector(*args)
    def GetSectors(*args): return _cspace.iEngine_GetSectors(*args)
    def FindSector(*args): return _cspace.iEngine_FindSector(*args)
    def GetNearbySectors(*args): return _cspace.iEngine_GetNearbySectors(*args)
    def AddEngineFrameCallback(*args): return _cspace.iEngine_AddEngineFrameCallback(*args)
    def RemoveEngineFrameCallback(*args): return _cspace.iEngine_RemoveEngineFrameCallback(*args)
    def AddEngineSectorCallback(*args): return _cspace.iEngine_AddEngineSectorCallback(*args)
    def RemoveEngineSectorCallback(*args): return _cspace.iEngine_RemoveEngineSectorCallback(*args)
    def CreateMeshWrapper(*args): return _cspace.iEngine_CreateMeshWrapper(*args)
    def CreateSectorWallsMesh(*args): return _cspace.iEngine_CreateSectorWallsMesh(*args)
    def CreateThingMesh(*args): return _cspace.iEngine_CreateThingMesh(*args)
    def LoadMeshWrapper(*args): return _cspace.iEngine_LoadMeshWrapper(*args)
    def AddMeshAndChildren(*args): return _cspace.iEngine_AddMeshAndChildren(*args)
    def GetNearbyMeshes(*args): return _cspace.iEngine_GetNearbyMeshes(*args)
    def GetMeshes(*args): return _cspace.iEngine_GetMeshes(*args)
    def FindMeshObject(*args): return _cspace.iEngine_FindMeshObject(*args)
    def WantToDie(*args): return _cspace.iEngine_WantToDie(*args)
    def CreateMeshFactory(*args): return _cspace.iEngine_CreateMeshFactory(*args)
    def LoadMeshFactory(*args): return _cspace.iEngine_LoadMeshFactory(*args)
    def FindMeshFactory(*args): return _cspace.iEngine_FindMeshFactory(*args)
    def GetMeshFactories(*args): return _cspace.iEngine_GetMeshFactories(*args)
    def CreateRegion(*args): return _cspace.iEngine_CreateRegion(*args)
    def GetRegions(*args): return _cspace.iEngine_GetRegions(*args)
    def CreateCamera(*args): return _cspace.iEngine_CreateCamera(*args)
    def FindCameraPosition(*args): return _cspace.iEngine_FindCameraPosition(*args)
    def GetCameraPositions(*args): return _cspace.iEngine_GetCameraPositions(*args)
    def CreatePortal(*args): return _cspace.iEngine_CreatePortal(*args)
    def CreatePortalContainer(*args): return _cspace.iEngine_CreatePortalContainer(*args)
    def SetClearZBuf(*args): return _cspace.iEngine_SetClearZBuf(*args)
    def GetClearZBuf(*args): return _cspace.iEngine_GetClearZBuf(*args)
    def GetDefaultClearZBuf(*args): return _cspace.iEngine_GetDefaultClearZBuf(*args)
    def SetClearScreen(*args): return _cspace.iEngine_SetClearScreen(*args)
    def GetClearScreen(*args): return _cspace.iEngine_GetClearScreen(*args)
    def GetDefaultClearScreen(*args): return _cspace.iEngine_GetDefaultClearScreen(*args)
    def GetBeginDrawFlags(*args): return _cspace.iEngine_GetBeginDrawFlags(*args)
    def GetTopLevelClipper(*args): return _cspace.iEngine_GetTopLevelClipper(*args)
    def PrecacheDraw(*args): return _cspace.iEngine_PrecacheDraw(*args)
    def Draw(*args): return _cspace.iEngine_Draw(*args)
    def SetContext(*args): return _cspace.iEngine_SetContext(*args)
    def GetContext(*args): return _cspace.iEngine_GetContext(*args)
    def GetRenderLoopManager(*args): return _cspace.iEngine_GetRenderLoopManager(*args)
    def GetCurrentDefaultRenderloop(*args): return _cspace.iEngine_GetCurrentDefaultRenderloop(*args)
    def SetCurrentDefaultRenderloop(*args): return _cspace.iEngine_SetCurrentDefaultRenderloop(*args)
    def GetCurrentFrameNumber(*args): return _cspace.iEngine_GetCurrentFrameNumber(*args)
    def SetSaveableFlag(*args): return _cspace.iEngine_SetSaveableFlag(*args)
    def GetSaveableFlag(*args): return _cspace.iEngine_GetSaveableFlag(*args)
    def CreateLoaderContext(*args): return _cspace.iEngine_CreateLoaderContext(*args)
    def GetNearbyObjects(*args): return _cspace.iEngine_GetNearbyObjects(*args)
    def GetVisibleObjects(*args): return _cspace.iEngine_GetVisibleObjects(*args)
    def GetVisibleMeshes(*args): return _cspace.iEngine_GetVisibleMeshes(*args)
    def CreateFrustumView(*args): return _cspace.iEngine_CreateFrustumView(*args)
    def CreateObjectWatcher(*args): return _cspace.iEngine_CreateObjectWatcher(*args)
    def GetVariableList(*args): return _cspace.iEngine_GetVariableList(*args)
    def RemoveObject(*args): return _cspace.iEngine_RemoveObject(*args)
    def DelayedRemoveObject(*args): return _cspace.iEngine_DelayedRemoveObject(*args)
    def RemoveDelayedRemoves(*args): return _cspace.iEngine_RemoveDelayedRemoves(*args)
    def DeleteAll(*args): return _cspace.iEngine_DeleteAll(*args)
    def ResetWorldSpecificSettings(*args): return _cspace.iEngine_ResetWorldSpecificSettings(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iEngine_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iEngine_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iEngine
    __del__ = lambda self : None;
iEngine_swigregister = _cspace.iEngine_swigregister
iEngine_swigregister(iEngine)
iEngine_scfGetVersion = _cspace.iEngine_scfGetVersion

class iCameraSectorListener(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iCameraSectorListener, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iCameraSectorListener, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def NewSector(*args): return _cspace.iCameraSectorListener_NewSector(*args)
    __swig_destroy__ = _cspace.delete_iCameraSectorListener
    __del__ = lambda self : None;
iCameraSectorListener_swigregister = _cspace.iCameraSectorListener_swigregister
iCameraSectorListener_swigregister(iCameraSectorListener)

class iCamera(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iCamera, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iCamera, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Clone(*args): return _cspace.iCamera_Clone(*args)
    def QuerySceneNode(*args): return _cspace.iCamera_QuerySceneNode(*args)
    def GetFOV(*args): return _cspace.iCamera_GetFOV(*args)
    def GetInvFOV(*args): return _cspace.iCamera_GetInvFOV(*args)
    def GetFOVAngle(*args): return _cspace.iCamera_GetFOVAngle(*args)
    def SetFOV(*args): return _cspace.iCamera_SetFOV(*args)
    def SetFOVAngle(*args): return _cspace.iCamera_SetFOVAngle(*args)
    def GetShiftX(*args): return _cspace.iCamera_GetShiftX(*args)
    def GetShiftY(*args): return _cspace.iCamera_GetShiftY(*args)
    def SetPerspectiveCenter(*args): return _cspace.iCamera_SetPerspectiveCenter(*args)
    def GetTransform(*args): return _cspace.iCamera_GetTransform(*args)
    def SetTransform(*args): return _cspace.iCamera_SetTransform(*args)
    def MoveWorld(*args): return _cspace.iCamera_MoveWorld(*args)
    def Move(*args): return _cspace.iCamera_Move(*args)
    def MoveWorldUnrestricted(*args): return _cspace.iCamera_MoveWorldUnrestricted(*args)
    def MoveUnrestricted(*args): return _cspace.iCamera_MoveUnrestricted(*args)
    def GetSector(*args): return _cspace.iCamera_GetSector(*args)
    def SetSector(*args): return _cspace.iCamera_SetSector(*args)
    def Correct(*args): return _cspace.iCamera_Correct(*args)
    def IsMirrored(*args): return _cspace.iCamera_IsMirrored(*args)
    def SetMirrored(*args): return _cspace.iCamera_SetMirrored(*args)
    def GetFarPlane(*args): return _cspace.iCamera_GetFarPlane(*args)
    def SetFarPlane(*args): return _cspace.iCamera_SetFarPlane(*args)
    def GetCameraNumber(*args): return _cspace.iCamera_GetCameraNumber(*args)
    def Perspective(*args): return _cspace.iCamera_Perspective(*args)
    def InvPerspective(*args): return _cspace.iCamera_InvPerspective(*args)
    def OnlyPortals(*args): return _cspace.iCamera_OnlyPortals(*args)
    def GetOnlyPortals(*args): return _cspace.iCamera_GetOnlyPortals(*args)
    def AddCameraSectorListener(*args): return _cspace.iCamera_AddCameraSectorListener(*args)
    def RemoveCameraSectorListener(*args): return _cspace.iCamera_RemoveCameraSectorListener(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iCamera_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iCamera_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iCamera
    __del__ = lambda self : None;
iCamera_swigregister = _cspace.iCamera_swigregister
iCamera_swigregister(iCamera)
iCamera_scfGetVersion = _cspace.iCamera_scfGetVersion

class iCameraPosition(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iCameraPosition, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iCameraPosition, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iCameraPosition_QueryObject(*args)
    def Clone(*args): return _cspace.iCameraPosition_Clone(*args)
    def GetSector(*args): return _cspace.iCameraPosition_GetSector(*args)
    def SetSector(*args): return _cspace.iCameraPosition_SetSector(*args)
    def GetPosition(*args): return _cspace.iCameraPosition_GetPosition(*args)
    def SetPosition(*args): return _cspace.iCameraPosition_SetPosition(*args)
    def GetUpwardVector(*args): return _cspace.iCameraPosition_GetUpwardVector(*args)
    def SetUpwardVector(*args): return _cspace.iCameraPosition_SetUpwardVector(*args)
    def GetForwardVector(*args): return _cspace.iCameraPosition_GetForwardVector(*args)
    def SetForwardVector(*args): return _cspace.iCameraPosition_SetForwardVector(*args)
    def Set(*args): return _cspace.iCameraPosition_Set(*args)
    def Load(*args): return _cspace.iCameraPosition_Load(*args)
    def SetFarPlane(*args): return _cspace.iCameraPosition_SetFarPlane(*args)
    def ClearFarPlane(*args): return _cspace.iCameraPosition_ClearFarPlane(*args)
    def GetFarPlane(*args): return _cspace.iCameraPosition_GetFarPlane(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iCameraPosition_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iCameraPosition_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iCameraPosition
    __del__ = lambda self : None;
iCameraPosition_swigregister = _cspace.iCameraPosition_swigregister
iCameraPosition_swigregister(iCameraPosition)
iCameraPosition_scfGetVersion = _cspace.iCameraPosition_scfGetVersion

class iCameraPositionList(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iCameraPositionList, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iCameraPositionList, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def NewCameraPosition(*args): return _cspace.iCameraPositionList_NewCameraPosition(*args)
    def GetCount(*args): return _cspace.iCameraPositionList_GetCount(*args)
    def Get(*args): return _cspace.iCameraPositionList_Get(*args)
    def Add(*args): return _cspace.iCameraPositionList_Add(*args)
    def Remove(*args): return _cspace.iCameraPositionList_Remove(*args)
    def RemoveAll(*args): return _cspace.iCameraPositionList_RemoveAll(*args)
    def Find(*args): return _cspace.iCameraPositionList_Find(*args)
    def FindByName(*args): return _cspace.iCameraPositionList_FindByName(*args)
    def __delitem__(*args): return _cspace.iCameraPositionList___delitem__(*args)
    def __len__(*args): return _cspace.iCameraPositionList___len__(*args)
    def append(*args): return _cspace.iCameraPositionList_append(*args)
    def content_iterator(self):
            for idx in xrange(len(self)):
                    yield self.__getitem__(idx)
    def __iter__(self): return self.content_iterator()  
    def __getitem__(*args): return _cspace.iCameraPositionList___getitem__(*args)
    def __contains__(*args): return _cspace.iCameraPositionList___contains__(*args)
    __swig_destroy__ = _cspace.delete_iCameraPositionList
    __del__ = lambda self : None;
iCameraPositionList_swigregister = _cspace.iCameraPositionList_swigregister
iCameraPositionList_swigregister(iCameraPositionList)

class iTextureCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iTextureCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iTextureCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def UseTexture(*args): return _cspace.iTextureCallback_UseTexture(*args)
    __swig_destroy__ = _cspace.delete_iTextureCallback
    __del__ = lambda self : None;
iTextureCallback_swigregister = _cspace.iTextureCallback_swigregister
iTextureCallback_swigregister(iTextureCallback)

class iTextureWrapper(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iTextureWrapper, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iTextureWrapper, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iTextureWrapper_QueryObject(*args)
    def Clone(*args): return _cspace.iTextureWrapper_Clone(*args)
    def SetImageFile(*args): return _cspace.iTextureWrapper_SetImageFile(*args)
    def GetImageFile(*args): return _cspace.iTextureWrapper_GetImageFile(*args)
    def SetTextureHandle(*args): return _cspace.iTextureWrapper_SetTextureHandle(*args)
    def GetTextureHandle(*args): return _cspace.iTextureWrapper_GetTextureHandle(*args)
    def SetKeyColor(*args): return _cspace.iTextureWrapper_SetKeyColor(*args)
    def GetKeyColor(*args): return _cspace.iTextureWrapper_GetKeyColor(*args)
    def SetFlags(*args): return _cspace.iTextureWrapper_SetFlags(*args)
    def GetFlags(*args): return _cspace.iTextureWrapper_GetFlags(*args)
    def Register(*args): return _cspace.iTextureWrapper_Register(*args)
    def SetUseCallback(*args): return _cspace.iTextureWrapper_SetUseCallback(*args)
    def GetUseCallback(*args): return _cspace.iTextureWrapper_GetUseCallback(*args)
    def Visit(*args): return _cspace.iTextureWrapper_Visit(*args)
    def IsVisitRequired(*args): return _cspace.iTextureWrapper_IsVisitRequired(*args)
    def SetKeepImage(*args): return _cspace.iTextureWrapper_SetKeepImage(*args)
    def KeepImage(*args): return _cspace.iTextureWrapper_KeepImage(*args)
    def SetTextureClass(*args): return _cspace.iTextureWrapper_SetTextureClass(*args)
    def GetTextureClass(*args): return _cspace.iTextureWrapper_GetTextureClass(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iTextureWrapper_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iTextureWrapper_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iTextureWrapper
    __del__ = lambda self : None;
iTextureWrapper_swigregister = _cspace.iTextureWrapper_swigregister
iTextureWrapper_swigregister(iTextureWrapper)
iTextureWrapper_scfGetVersion = _cspace.iTextureWrapper_scfGetVersion

class iTextureList(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iTextureList, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iTextureList, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def NewTexture(*args): return _cspace.iTextureList_NewTexture(*args)
    def GetCount(*args): return _cspace.iTextureList_GetCount(*args)
    def Get(*args): return _cspace.iTextureList_Get(*args)
    def Add(*args): return _cspace.iTextureList_Add(*args)
    def Remove(*args): return _cspace.iTextureList_Remove(*args)
    def RemoveAll(*args): return _cspace.iTextureList_RemoveAll(*args)
    def Find(*args): return _cspace.iTextureList_Find(*args)
    def FindByName(*args): return _cspace.iTextureList_FindByName(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iTextureList_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iTextureList_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iTextureList
    __del__ = lambda self : None;
    def __delitem__(*args): return _cspace.iTextureList___delitem__(*args)
    def __len__(*args): return _cspace.iTextureList___len__(*args)
    def append(*args): return _cspace.iTextureList_append(*args)
    def content_iterator(self):
            for idx in xrange(len(self)):
                    yield self.__getitem__(idx)
    def __iter__(self): return self.content_iterator()  
    def __getitem__(*args): return _cspace.iTextureList___getitem__(*args)
    def __contains__(*args): return _cspace.iTextureList___contains__(*args)
iTextureList_swigregister = _cspace.iTextureList_swigregister
iTextureList_swigregister(iTextureList)
iTextureList_scfGetVersion = _cspace.iTextureList_scfGetVersion

class iMaterialWrapper(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMaterialWrapper, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMaterialWrapper, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iMaterialWrapper_QueryObject(*args)
    def SetMaterial(*args): return _cspace.iMaterialWrapper_SetMaterial(*args)
    def GetMaterial(*args): return _cspace.iMaterialWrapper_GetMaterial(*args)
    def Visit(*args): return _cspace.iMaterialWrapper_Visit(*args)
    def IsVisitRequired(*args): return _cspace.iMaterialWrapper_IsVisitRequired(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iMaterialWrapper_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iMaterialWrapper_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iMaterialWrapper
    __del__ = lambda self : None;
iMaterialWrapper_swigregister = _cspace.iMaterialWrapper_swigregister
iMaterialWrapper_swigregister(iMaterialWrapper)
iMaterialWrapper_scfGetVersion = _cspace.iMaterialWrapper_scfGetVersion

class iMaterialEngine(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMaterialEngine, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMaterialEngine, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetTextureWrapper(*args): return _cspace.iMaterialEngine_GetTextureWrapper(*args)
    def Visit(*args): return _cspace.iMaterialEngine_Visit(*args)
    def IsVisitRequired(*args): return _cspace.iMaterialEngine_IsVisitRequired(*args)
    __swig_destroy__ = _cspace.delete_iMaterialEngine
    __del__ = lambda self : None;
iMaterialEngine_swigregister = _cspace.iMaterialEngine_swigregister
iMaterialEngine_swigregister(iMaterialEngine)

class iMaterialList(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMaterialList, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMaterialList, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def NewMaterial(*args): return _cspace.iMaterialList_NewMaterial(*args)
    def GetCount(*args): return _cspace.iMaterialList_GetCount(*args)
    def Get(*args): return _cspace.iMaterialList_Get(*args)
    def Add(*args): return _cspace.iMaterialList_Add(*args)
    def Remove(*args): return _cspace.iMaterialList_Remove(*args)
    def RemoveAll(*args): return _cspace.iMaterialList_RemoveAll(*args)
    def Find(*args): return _cspace.iMaterialList_Find(*args)
    def FindByName(*args): return _cspace.iMaterialList_FindByName(*args)
    def __delitem__(*args): return _cspace.iMaterialList___delitem__(*args)
    def __len__(*args): return _cspace.iMaterialList___len__(*args)
    def append(*args): return _cspace.iMaterialList_append(*args)
    def content_iterator(self):
            for idx in xrange(len(self)):
                    yield self.__getitem__(idx)
    def __iter__(self): return self.content_iterator()  
    def __getitem__(*args): return _cspace.iMaterialList___getitem__(*args)
    def __contains__(*args): return _cspace.iMaterialList___contains__(*args)
    __swig_destroy__ = _cspace.delete_iMaterialList
    __del__ = lambda self : None;
iMaterialList_swigregister = _cspace.iMaterialList_swigregister
iMaterialList_swigregister(iMaterialList)

class iSceneNodeArrayReadOnly(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSceneNodeArrayReadOnly, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSceneNodeArrayReadOnly, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetSize(*args): return _cspace.iSceneNodeArrayReadOnly_GetSize(*args)
    def Get(*args): return _cspace.iSceneNodeArrayReadOnly_Get(*args)
    def Top(*args): return _cspace.iSceneNodeArrayReadOnly_Top(*args)
    def Find(*args): return _cspace.iSceneNodeArrayReadOnly_Find(*args)
    def GetIndex(*args): return _cspace.iSceneNodeArrayReadOnly_GetIndex(*args)
    def IsEmpty(*args): return _cspace.iSceneNodeArrayReadOnly_IsEmpty(*args)
    def GetAll(*args): return _cspace.iSceneNodeArrayReadOnly_GetAll(*args)
    __swig_destroy__ = _cspace.delete_iSceneNodeArrayReadOnly
    __del__ = lambda self : None;
iSceneNodeArrayReadOnly_swigregister = _cspace.iSceneNodeArrayReadOnly_swigregister
iSceneNodeArrayReadOnly_swigregister(iSceneNodeArrayReadOnly)

class iSceneNodeArray(iSceneNodeArrayReadOnly):
    __swig_setmethods__ = {}
    for _s in [iSceneNodeArrayReadOnly]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSceneNodeArray, name, value)
    __swig_getmethods__ = {}
    for _s in [iSceneNodeArrayReadOnly]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSceneNodeArray, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSceneNodeArray_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSceneNodeArray_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSceneNodeArray
    __del__ = lambda self : None;
iSceneNodeArray_swigregister = _cspace.iSceneNodeArray_swigregister
iSceneNodeArray_swigregister(iSceneNodeArray)
iSceneNodeArray_scfGetVersion = _cspace.iSceneNodeArray_scfGetVersion

class iSceneNode(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSceneNode, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSceneNode, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetMovable(*args): return _cspace.iSceneNode_GetMovable(*args)
    def QueryMesh(*args): return _cspace.iSceneNode_QueryMesh(*args)
    def QueryLight(*args): return _cspace.iSceneNode_QueryLight(*args)
    def QueryCamera(*args): return _cspace.iSceneNode_QueryCamera(*args)
    def SetParent(*args): return _cspace.iSceneNode_SetParent(*args)
    def GetParent(*args): return _cspace.iSceneNode_GetParent(*args)
    def GetChildren(*args): return _cspace.iSceneNode_GetChildren(*args)
    def GetChildrenArray(*args): return _cspace.iSceneNode_GetChildrenArray(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSceneNode_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSceneNode_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSceneNode
    __del__ = lambda self : None;
iSceneNode_swigregister = _cspace.iSceneNode_swigregister
iSceneNode_swigregister(iSceneNode)
iSceneNode_scfGetVersion = _cspace.iSceneNode_scfGetVersion

CS_ENTITY_DETAIL = _cspace.CS_ENTITY_DETAIL
CS_ENTITY_CAMERA = _cspace.CS_ENTITY_CAMERA
CS_ENTITY_INVISIBLEMESH = _cspace.CS_ENTITY_INVISIBLEMESH
CS_ENTITY_NOSHADOWS = _cspace.CS_ENTITY_NOSHADOWS
CS_ENTITY_NOLIGHTING = _cspace.CS_ENTITY_NOLIGHTING
CS_ENTITY_NOHITBEAM = _cspace.CS_ENTITY_NOHITBEAM
CS_ENTITY_NOCLIP = _cspace.CS_ENTITY_NOCLIP
CS_ENTITY_NODECAL = _cspace.CS_ENTITY_NODECAL
CS_LIGHTINGUPDATE_SORTRELEVANCE = _cspace.CS_LIGHTINGUPDATE_SORTRELEVANCE
CS_LIGHTINGUPDATE_ALWAYSUPDATE = _cspace.CS_LIGHTINGUPDATE_ALWAYSUPDATE
class iMeshDrawCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMeshDrawCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMeshDrawCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def BeforeDrawing(*args): return _cspace.iMeshDrawCallback_BeforeDrawing(*args)
    __swig_destroy__ = _cspace.delete_iMeshDrawCallback
    __del__ = lambda self : None;
iMeshDrawCallback_swigregister = _cspace.iMeshDrawCallback_swigregister
iMeshDrawCallback_swigregister(iMeshDrawCallback)

class csHitBeamResult(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csHitBeamResult, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csHitBeamResult, name)
    __repr__ = _swig_repr
    __swig_setmethods__["isect"] = _cspace.csHitBeamResult_isect_set
    __swig_getmethods__["isect"] = _cspace.csHitBeamResult_isect_get
    if _newclass:isect = property(_cspace.csHitBeamResult_isect_get, _cspace.csHitBeamResult_isect_set)
    __swig_setmethods__["r"] = _cspace.csHitBeamResult_r_set
    __swig_getmethods__["r"] = _cspace.csHitBeamResult_r_get
    if _newclass:r = property(_cspace.csHitBeamResult_r_get, _cspace.csHitBeamResult_r_set)
    __swig_setmethods__["polygon_idx"] = _cspace.csHitBeamResult_polygon_idx_set
    __swig_getmethods__["polygon_idx"] = _cspace.csHitBeamResult_polygon_idx_get
    if _newclass:polygon_idx = property(_cspace.csHitBeamResult_polygon_idx_get, _cspace.csHitBeamResult_polygon_idx_set)
    __swig_setmethods__["material"] = _cspace.csHitBeamResult_material_set
    __swig_getmethods__["material"] = _cspace.csHitBeamResult_material_get
    if _newclass:material = property(_cspace.csHitBeamResult_material_get, _cspace.csHitBeamResult_material_set)
    __swig_setmethods__["facehit"] = _cspace.csHitBeamResult_facehit_set
    __swig_getmethods__["facehit"] = _cspace.csHitBeamResult_facehit_get
    if _newclass:facehit = property(_cspace.csHitBeamResult_facehit_get, _cspace.csHitBeamResult_facehit_set)
    __swig_setmethods__["hit"] = _cspace.csHitBeamResult_hit_set
    __swig_getmethods__["hit"] = _cspace.csHitBeamResult_hit_get
    if _newclass:hit = property(_cspace.csHitBeamResult_hit_get, _cspace.csHitBeamResult_hit_set)
    def __init__(self, *args): 
        this = _cspace.new_csHitBeamResult(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csHitBeamResult
    __del__ = lambda self : None;
csHitBeamResult_swigregister = _cspace.csHitBeamResult_swigregister
csHitBeamResult_swigregister(csHitBeamResult)

class csScreenBoxResult(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csScreenBoxResult, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csScreenBoxResult, name)
    __repr__ = _swig_repr
    __swig_setmethods__["sbox"] = _cspace.csScreenBoxResult_sbox_set
    __swig_getmethods__["sbox"] = _cspace.csScreenBoxResult_sbox_get
    if _newclass:sbox = property(_cspace.csScreenBoxResult_sbox_get, _cspace.csScreenBoxResult_sbox_set)
    __swig_setmethods__["cbox"] = _cspace.csScreenBoxResult_cbox_set
    __swig_getmethods__["cbox"] = _cspace.csScreenBoxResult_cbox_get
    if _newclass:cbox = property(_cspace.csScreenBoxResult_cbox_get, _cspace.csScreenBoxResult_cbox_set)
    __swig_setmethods__["distance"] = _cspace.csScreenBoxResult_distance_set
    __swig_getmethods__["distance"] = _cspace.csScreenBoxResult_distance_get
    if _newclass:distance = property(_cspace.csScreenBoxResult_distance_get, _cspace.csScreenBoxResult_distance_set)
    def __init__(self, *args): 
        this = _cspace.new_csScreenBoxResult(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csScreenBoxResult
    __del__ = lambda self : None;
csScreenBoxResult_swigregister = _cspace.csScreenBoxResult_swigregister
csScreenBoxResult_swigregister(csScreenBoxResult)

class iMeshWrapper(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMeshWrapper, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMeshWrapper, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iMeshWrapper_QueryObject(*args)
    def GetMeshObject(*args): return _cspace.iMeshWrapper_GetMeshObject(*args)
    def SetMeshObject(*args): return _cspace.iMeshWrapper_SetMeshObject(*args)
    def GetPortalContainer(*args): return _cspace.iMeshWrapper_GetPortalContainer(*args)
    def GetLightingInfo(*args): return _cspace.iMeshWrapper_GetLightingInfo(*args)
    def GetShadowReceiver(*args): return _cspace.iMeshWrapper_GetShadowReceiver(*args)
    def GetShadowCaster(*args): return _cspace.iMeshWrapper_GetShadowCaster(*args)
    def GetFactory(*args): return _cspace.iMeshWrapper_GetFactory(*args)
    def SetFactory(*args): return _cspace.iMeshWrapper_SetFactory(*args)
    def SetLightingUpdate(*args): return _cspace.iMeshWrapper_SetLightingUpdate(*args)
    def GetMovable(*args): return _cspace.iMeshWrapper_GetMovable(*args)
    def QuerySceneNode(*args): return _cspace.iMeshWrapper_QuerySceneNode(*args)
    def FindChildByName(*args): return _cspace.iMeshWrapper_FindChildByName(*args)
    def PlaceMesh(*args): return _cspace.iMeshWrapper_PlaceMesh(*args)
    def HitBeamBBox(*args): return _cspace.iMeshWrapper_HitBeamBBox(*args)
    def HitBeamOutline(*args): return _cspace.iMeshWrapper_HitBeamOutline(*args)
    def HitBeamObject(*args): return _cspace.iMeshWrapper_HitBeamObject(*args)
    def HitBeam(*args): return _cspace.iMeshWrapper_HitBeam(*args)
    def SetDrawCallback(*args): return _cspace.iMeshWrapper_SetDrawCallback(*args)
    def RemoveDrawCallback(*args): return _cspace.iMeshWrapper_RemoveDrawCallback(*args)
    def GetDrawCallbackCount(*args): return _cspace.iMeshWrapper_GetDrawCallbackCount(*args)
    def GetDrawCallback(*args): return _cspace.iMeshWrapper_GetDrawCallback(*args)
    def SetRenderPriority(*args): return _cspace.iMeshWrapper_SetRenderPriority(*args)
    def GetRenderPriority(*args): return _cspace.iMeshWrapper_GetRenderPriority(*args)
    def SetRenderPriorityRecursive(*args): return _cspace.iMeshWrapper_SetRenderPriorityRecursive(*args)
    def GetFlags(*args): return _cspace.iMeshWrapper_GetFlags(*args)
    def SetFlagsRecursive(*args): return _cspace.iMeshWrapper_SetFlagsRecursive(*args)
    def SetZBufMode(*args): return _cspace.iMeshWrapper_SetZBufMode(*args)
    def GetZBufMode(*args): return _cspace.iMeshWrapper_GetZBufMode(*args)
    def SetZBufModeRecursive(*args): return _cspace.iMeshWrapper_SetZBufModeRecursive(*args)
    def HardTransform(*args): return _cspace.iMeshWrapper_HardTransform(*args)
    def GetWorldBoundingBox(*args): return _cspace.iMeshWrapper_GetWorldBoundingBox(*args)
    def GetTransformedBoundingBox(*args): return _cspace.iMeshWrapper_GetTransformedBoundingBox(*args)
    def GetScreenBoundingBox(*args): return _cspace.iMeshWrapper_GetScreenBoundingBox(*args)
    def GetRadius(*args): return _cspace.iMeshWrapper_GetRadius(*args)
    def ResetMinMaxRenderDistance(*args): return _cspace.iMeshWrapper_ResetMinMaxRenderDistance(*args)
    def SetMinimumRenderDistance(*args): return _cspace.iMeshWrapper_SetMinimumRenderDistance(*args)
    def GetMinimumRenderDistance(*args): return _cspace.iMeshWrapper_GetMinimumRenderDistance(*args)
    def SetMaximumRenderDistance(*args): return _cspace.iMeshWrapper_SetMaximumRenderDistance(*args)
    def GetMaximumRenderDistance(*args): return _cspace.iMeshWrapper_GetMaximumRenderDistance(*args)
    def SetMinimumRenderDistanceVar(*args): return _cspace.iMeshWrapper_SetMinimumRenderDistanceVar(*args)
    def GetMinimumRenderDistanceVar(*args): return _cspace.iMeshWrapper_GetMinimumRenderDistanceVar(*args)
    def SetMaximumRenderDistanceVar(*args): return _cspace.iMeshWrapper_SetMaximumRenderDistanceVar(*args)
    def GetMaximumRenderDistanceVar(*args): return _cspace.iMeshWrapper_GetMaximumRenderDistanceVar(*args)
    def CreateStaticLOD(*args): return _cspace.iMeshWrapper_CreateStaticLOD(*args)
    def DestroyStaticLOD(*args): return _cspace.iMeshWrapper_DestroyStaticLOD(*args)
    def GetStaticLOD(*args): return _cspace.iMeshWrapper_GetStaticLOD(*args)
    def AddMeshToStaticLOD(*args): return _cspace.iMeshWrapper_AddMeshToStaticLOD(*args)
    def RemoveMeshFromStaticLOD(*args): return _cspace.iMeshWrapper_RemoveMeshFromStaticLOD(*args)
    def GetSVContext(*args): return _cspace.iMeshWrapper_GetSVContext(*args)
    def AddExtraRenderMesh(*args): return _cspace.iMeshWrapper_AddExtraRenderMesh(*args)
    def GetExtraRenderMesh(*args): return _cspace.iMeshWrapper_GetExtraRenderMesh(*args)
    def GetExtraRenderMeshCount(*args): return _cspace.iMeshWrapper_GetExtraRenderMeshCount(*args)
    def GetExtraRenderMeshPriority(*args): return _cspace.iMeshWrapper_GetExtraRenderMeshPriority(*args)
    def GetExtraRenderMeshZBufMode(*args): return _cspace.iMeshWrapper_GetExtraRenderMeshZBufMode(*args)
    def RemoveExtraRenderMesh(*args): return _cspace.iMeshWrapper_RemoveExtraRenderMesh(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iMeshWrapper_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iMeshWrapper_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iMeshWrapper
    __del__ = lambda self : None;
iMeshWrapper_swigregister = _cspace.iMeshWrapper_swigregister
iMeshWrapper_swigregister(iMeshWrapper)
iMeshWrapper_scfGetVersion = _cspace.iMeshWrapper_scfGetVersion

class iMeshFactoryWrapper(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMeshFactoryWrapper, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMeshFactoryWrapper, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iMeshFactoryWrapper_QueryObject(*args)
    def GetMeshObjectFactory(*args): return _cspace.iMeshFactoryWrapper_GetMeshObjectFactory(*args)
    def SetMeshObjectFactory(*args): return _cspace.iMeshFactoryWrapper_SetMeshObjectFactory(*args)
    def HardTransform(*args): return _cspace.iMeshFactoryWrapper_HardTransform(*args)
    def CreateMeshWrapper(*args): return _cspace.iMeshFactoryWrapper_CreateMeshWrapper(*args)
    def GetFlags(*args): return _cspace.iMeshFactoryWrapper_GetFlags(*args)
    def GetParentContainer(*args): return _cspace.iMeshFactoryWrapper_GetParentContainer(*args)
    def SetParentContainer(*args): return _cspace.iMeshFactoryWrapper_SetParentContainer(*args)
    def GetChildren(*args): return _cspace.iMeshFactoryWrapper_GetChildren(*args)
    def GetTransform(*args): return _cspace.iMeshFactoryWrapper_GetTransform(*args)
    def SetTransform(*args): return _cspace.iMeshFactoryWrapper_SetTransform(*args)
    def CreateStaticLOD(*args): return _cspace.iMeshFactoryWrapper_CreateStaticLOD(*args)
    def DestroyStaticLOD(*args): return _cspace.iMeshFactoryWrapper_DestroyStaticLOD(*args)
    def SetStaticLOD(*args): return _cspace.iMeshFactoryWrapper_SetStaticLOD(*args)
    def GetStaticLOD(*args): return _cspace.iMeshFactoryWrapper_GetStaticLOD(*args)
    def AddFactoryToStaticLOD(*args): return _cspace.iMeshFactoryWrapper_AddFactoryToStaticLOD(*args)
    def RemoveFactoryFromStaticLOD(*args): return _cspace.iMeshFactoryWrapper_RemoveFactoryFromStaticLOD(*args)
    def SetZBufMode(*args): return _cspace.iMeshFactoryWrapper_SetZBufMode(*args)
    def GetZBufMode(*args): return _cspace.iMeshFactoryWrapper_GetZBufMode(*args)
    def SetZBufModeRecursive(*args): return _cspace.iMeshFactoryWrapper_SetZBufModeRecursive(*args)
    def SetRenderPriority(*args): return _cspace.iMeshFactoryWrapper_SetRenderPriority(*args)
    def GetRenderPriority(*args): return _cspace.iMeshFactoryWrapper_GetRenderPriority(*args)
    def SetRenderPriorityRecursive(*args): return _cspace.iMeshFactoryWrapper_SetRenderPriorityRecursive(*args)
    def GetSVContext(*args): return _cspace.iMeshFactoryWrapper_GetSVContext(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iMeshFactoryWrapper_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iMeshFactoryWrapper_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iMeshFactoryWrapper
    __del__ = lambda self : None;
iMeshFactoryWrapper_swigregister = _cspace.iMeshFactoryWrapper_swigregister
iMeshFactoryWrapper_swigregister(iMeshFactoryWrapper)
iMeshFactoryWrapper_scfGetVersion = _cspace.iMeshFactoryWrapper_scfGetVersion

class iMeshList(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMeshList, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMeshList, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetCount(*args): return _cspace.iMeshList_GetCount(*args)
    def Get(*args): return _cspace.iMeshList_Get(*args)
    def Add(*args): return _cspace.iMeshList_Add(*args)
    def Remove(*args): return _cspace.iMeshList_Remove(*args)
    def RemoveAll(*args): return _cspace.iMeshList_RemoveAll(*args)
    def Find(*args): return _cspace.iMeshList_Find(*args)
    def FindByName(*args): return _cspace.iMeshList_FindByName(*args)
    def __delitem__(*args): return _cspace.iMeshList___delitem__(*args)
    def __len__(*args): return _cspace.iMeshList___len__(*args)
    def append(*args): return _cspace.iMeshList_append(*args)
    def content_iterator(self):
            for idx in xrange(len(self)):
                    yield self.__getitem__(idx)
    def __iter__(self): return self.content_iterator()  
    def __getitem__(*args): return _cspace.iMeshList___getitem__(*args)
    def __contains__(*args): return _cspace.iMeshList___contains__(*args)
    __swig_destroy__ = _cspace.delete_iMeshList
    __del__ = lambda self : None;
iMeshList_swigregister = _cspace.iMeshList_swigregister
iMeshList_swigregister(iMeshList)

class iMeshFactoryList(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMeshFactoryList, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMeshFactoryList, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetCount(*args): return _cspace.iMeshFactoryList_GetCount(*args)
    def Get(*args): return _cspace.iMeshFactoryList_Get(*args)
    def Add(*args): return _cspace.iMeshFactoryList_Add(*args)
    def Remove(*args): return _cspace.iMeshFactoryList_Remove(*args)
    def RemoveAll(*args): return _cspace.iMeshFactoryList_RemoveAll(*args)
    def Find(*args): return _cspace.iMeshFactoryList_Find(*args)
    def FindByName(*args): return _cspace.iMeshFactoryList_FindByName(*args)
    def __delitem__(*args): return _cspace.iMeshFactoryList___delitem__(*args)
    def __len__(*args): return _cspace.iMeshFactoryList___len__(*args)
    def append(*args): return _cspace.iMeshFactoryList_append(*args)
    def content_iterator(self):
            for idx in xrange(len(self)):
                    yield self.__getitem__(idx)
    def __iter__(self): return self.content_iterator()  
    def __getitem__(*args): return _cspace.iMeshFactoryList___getitem__(*args)
    def __contains__(*args): return _cspace.iMeshFactoryList___contains__(*args)
    __swig_destroy__ = _cspace.delete_iMeshFactoryList
    __del__ = lambda self : None;
iMeshFactoryList_swigregister = _cspace.iMeshFactoryList_swigregister
iMeshFactoryList_swigregister(iMeshFactoryList)

class iMeshWrapperIterator(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMeshWrapperIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMeshWrapperIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Next(*args): return _cspace.iMeshWrapperIterator_Next(*args)
    def Reset(*args): return _cspace.iMeshWrapperIterator_Reset(*args)
    def HasNext(*args): return _cspace.iMeshWrapperIterator_HasNext(*args)
    def __iter__(self):
        while self.HasNext():
            yield self.Next() 
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iMeshWrapperIterator_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iMeshWrapperIterator_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iMeshWrapperIterator
    __del__ = lambda self : None;
iMeshWrapperIterator_swigregister = _cspace.iMeshWrapperIterator_swigregister
iMeshWrapperIterator_swigregister(iMeshWrapperIterator)
iMeshWrapperIterator_scfGetVersion = _cspace.iMeshWrapperIterator_scfGetVersion

class iMovableListener(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMovableListener, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMovableListener, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def MovableChanged(*args): return _cspace.iMovableListener_MovableChanged(*args)
    def MovableDestroyed(*args): return _cspace.iMovableListener_MovableDestroyed(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iMovableListener_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iMovableListener_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iMovableListener
    __del__ = lambda self : None;
iMovableListener_swigregister = _cspace.iMovableListener_swigregister
iMovableListener_swigregister(iMovableListener)
iMovableListener_scfGetVersion = _cspace.iMovableListener_scfGetVersion

class iMovable(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMovable, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMovable, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetSceneNode(*args): return _cspace.iMovable_GetSceneNode(*args)
    def SetSector(*args): return _cspace.iMovable_SetSector(*args)
    def ClearSectors(*args): return _cspace.iMovable_ClearSectors(*args)
    def GetSectors(*args): return _cspace.iMovable_GetSectors(*args)
    def InSector(*args): return _cspace.iMovable_InSector(*args)
    def SetPosition(*args): return _cspace.iMovable_SetPosition(*args)
    def GetPosition(*args): return _cspace.iMovable_GetPosition(*args)
    def GetFullPosition(*args): return _cspace.iMovable_GetFullPosition(*args)
    def SetTransform(*args): return _cspace.iMovable_SetTransform(*args)
    def GetTransform(*args): return _cspace.iMovable_GetTransform(*args)
    def GetFullTransform(*args): return _cspace.iMovable_GetFullTransform(*args)
    def MovePosition(*args): return _cspace.iMovable_MovePosition(*args)
    def Transform(*args): return _cspace.iMovable_Transform(*args)
    def AddListener(*args): return _cspace.iMovable_AddListener(*args)
    def RemoveListener(*args): return _cspace.iMovable_RemoveListener(*args)
    def UpdateMove(*args): return _cspace.iMovable_UpdateMove(*args)
    def GetUpdateNumber(*args): return _cspace.iMovable_GetUpdateNumber(*args)
    def IsTransformIdentity(*args): return _cspace.iMovable_IsTransformIdentity(*args)
    def IsFullTransformIdentity(*args): return _cspace.iMovable_IsFullTransformIdentity(*args)
    def TransformIdentity(*args): return _cspace.iMovable_TransformIdentity(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iMovable_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iMovable_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iMovable
    __del__ = lambda self : None;
iMovable_swigregister = _cspace.iMovable_swigregister
iMovable_swigregister(iMovable)
iMovable_scfGetVersion = _cspace.iMovable_scfGetVersion

class iRegion(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iRegion, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iRegion, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iRegion_QueryObject(*args)
    def Add(*args): return _cspace.iRegion_Add(*args)
    def Remove(*args): return _cspace.iRegion_Remove(*args)
    def Clear(*args): return _cspace.iRegion_Clear(*args)
    def DeleteAll(*args): return _cspace.iRegion_DeleteAll(*args)
    def PrepareTextures(*args): return _cspace.iRegion_PrepareTextures(*args)
    def ShineLights(*args): return _cspace.iRegion_ShineLights(*args)
    def Prepare(*args): return _cspace.iRegion_Prepare(*args)
    def FindSector(*args): return _cspace.iRegion_FindSector(*args)
    def FindMeshObject(*args): return _cspace.iRegion_FindMeshObject(*args)
    def FindMeshFactory(*args): return _cspace.iRegion_FindMeshFactory(*args)
    def FindTexture(*args): return _cspace.iRegion_FindTexture(*args)
    def FindMaterial(*args): return _cspace.iRegion_FindMaterial(*args)
    def FindCameraPosition(*args): return _cspace.iRegion_FindCameraPosition(*args)
    def IsInRegion(*args): return _cspace.iRegion_IsInRegion(*args)
    __swig_destroy__ = _cspace.delete_iRegion
    __del__ = lambda self : None;
iRegion_swigregister = _cspace.iRegion_swigregister
iRegion_swigregister(iRegion)

class iRegionList(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iRegionList, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iRegionList, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetCount(*args): return _cspace.iRegionList_GetCount(*args)
    def Get(*args): return _cspace.iRegionList_Get(*args)
    def Add(*args): return _cspace.iRegionList_Add(*args)
    def Remove(*args): return _cspace.iRegionList_Remove(*args)
    def RemoveAll(*args): return _cspace.iRegionList_RemoveAll(*args)
    def Find(*args): return _cspace.iRegionList_Find(*args)
    def FindByName(*args): return _cspace.iRegionList_FindByName(*args)
    def __delitem__(*args): return _cspace.iRegionList___delitem__(*args)
    def __len__(*args): return _cspace.iRegionList___len__(*args)
    def append(*args): return _cspace.iRegionList_append(*args)
    def content_iterator(self):
            for idx in xrange(len(self)):
                    yield self.__getitem__(idx)
    def __iter__(self): return self.content_iterator()  
    def __getitem__(*args): return _cspace.iRegionList___getitem__(*args)
    def __contains__(*args): return _cspace.iRegionList___contains__(*args)
    __swig_destroy__ = _cspace.delete_iRegionList
    __del__ = lambda self : None;
iRegionList_swigregister = _cspace.iRegionList_swigregister
iRegionList_swigregister(iRegionList)

class iVisibilityObjectIterator(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iVisibilityObjectIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iVisibilityObjectIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def HasNext(*args): return _cspace.iVisibilityObjectIterator_HasNext(*args)
    def Next(*args): return _cspace.iVisibilityObjectIterator_Next(*args)
    def Reset(*args): return _cspace.iVisibilityObjectIterator_Reset(*args)
    def __iter__(self):
        while self.HasNext():
            yield self.Next() 
    __swig_destroy__ = _cspace.delete_iVisibilityObjectIterator
    __del__ = lambda self : None;
iVisibilityObjectIterator_swigregister = _cspace.iVisibilityObjectIterator_swigregister
iVisibilityObjectIterator_swigregister(iVisibilityObjectIterator)

class iVisibilityCullerListener(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iVisibilityCullerListener, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iVisibilityCullerListener, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def ObjectVisible(*args): return _cspace.iVisibilityCullerListener_ObjectVisible(*args)
    __swig_destroy__ = _cspace.delete_iVisibilityCullerListener
    __del__ = lambda self : None;
iVisibilityCullerListener_swigregister = _cspace.iVisibilityCullerListener_swigregister
iVisibilityCullerListener_swigregister(iVisibilityCullerListener)

class iVisibilityCuller(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iVisibilityCuller, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iVisibilityCuller, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Setup(*args): return _cspace.iVisibilityCuller_Setup(*args)
    def RegisterVisObject(*args): return _cspace.iVisibilityCuller_RegisterVisObject(*args)
    def UnregisterVisObject(*args): return _cspace.iVisibilityCuller_UnregisterVisObject(*args)
    def PrecacheCulling(*args): return _cspace.iVisibilityCuller_PrecacheCulling(*args)
    def VisTest(*args): return _cspace.iVisibilityCuller_VisTest(*args)
    def IntersectSegmentSloppy(*args): return _cspace.iVisibilityCuller_IntersectSegmentSloppy(*args)
    def IntersectSegment(*args): return _cspace.iVisibilityCuller_IntersectSegment(*args)
    def CastShadows(*args): return _cspace.iVisibilityCuller_CastShadows(*args)
    def ParseCullerParameters(*args): return _cspace.iVisibilityCuller_ParseCullerParameters(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iVisibilityCuller_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iVisibilityCuller_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iVisibilityCuller
    __del__ = lambda self : None;
iVisibilityCuller_swigregister = _cspace.iVisibilityCuller_swigregister
iVisibilityCuller_swigregister(iVisibilityCuller)
iVisibilityCuller_scfGetVersion = _cspace.iVisibilityCuller_scfGetVersion

CS_CULLER_HINT_GOODOCCLUDER = _cspace.CS_CULLER_HINT_GOODOCCLUDER
CS_CULLER_HINT_BADOCCLUDER = _cspace.CS_CULLER_HINT_BADOCCLUDER
class iVisibilityObject(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iVisibilityObject, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iVisibilityObject, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetMovable(*args): return _cspace.iVisibilityObject_GetMovable(*args)
    def GetMeshWrapper(*args): return _cspace.iVisibilityObject_GetMeshWrapper(*args)
    def GetObjectModel(*args): return _cspace.iVisibilityObject_GetObjectModel(*args)
    def GetCullerFlags(*args): return _cspace.iVisibilityObject_GetCullerFlags(*args)
    __swig_destroy__ = _cspace.delete_iVisibilityObject
    __del__ = lambda self : None;
iVisibilityObject_swigregister = _cspace.iVisibilityObject_swigregister
iVisibilityObject_swigregister(iVisibilityObject)

CS_PORTAL_CLIPDEST = _cspace.CS_PORTAL_CLIPDEST
CS_PORTAL_CLIPSTRADDLING = _cspace.CS_PORTAL_CLIPSTRADDLING
CS_PORTAL_ZFILL = _cspace.CS_PORTAL_ZFILL
CS_PORTAL_WARP = _cspace.CS_PORTAL_WARP
CS_PORTAL_MIRROR = _cspace.CS_PORTAL_MIRROR
CS_PORTAL_STATICDEST = _cspace.CS_PORTAL_STATICDEST
CS_PORTAL_FLOAT = _cspace.CS_PORTAL_FLOAT
CS_PORTAL_COLLDET = _cspace.CS_PORTAL_COLLDET
CS_PORTAL_VISCULL = _cspace.CS_PORTAL_VISCULL
class iPortalCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iPortalCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iPortalCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Traverse(*args): return _cspace.iPortalCallback_Traverse(*args)
    __swig_destroy__ = _cspace.delete_iPortalCallback
    __del__ = lambda self : None;
iPortalCallback_swigregister = _cspace.iPortalCallback_swigregister
iPortalCallback_swigregister(iPortalCallback)

class iPortal(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iPortal, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iPortal, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetName(*args): return _cspace.iPortal_SetName(*args)
    def GetName(*args): return _cspace.iPortal_GetName(*args)
    def GetSector(*args): return _cspace.iPortal_GetSector(*args)
    def GetVertices(*args): return _cspace.iPortal_GetVertices(*args)
    def GetWorldVertices(*args): return _cspace.iPortal_GetWorldVertices(*args)
    def GetVertexIndices(*args): return _cspace.iPortal_GetVertexIndices(*args)
    def GetVertexIndicesCount(*args): return _cspace.iPortal_GetVertexIndicesCount(*args)
    def GetObjectPlane(*args): return _cspace.iPortal_GetObjectPlane(*args)
    def GetWorldPlane(*args): return _cspace.iPortal_GetWorldPlane(*args)
    def ComputeCameraPlane(*args): return _cspace.iPortal_ComputeCameraPlane(*args)
    def PointOnPolygon(*args): return _cspace.iPortal_PointOnPolygon(*args)
    def SetSector(*args): return _cspace.iPortal_SetSector(*args)
    def GetFlags(*args): return _cspace.iPortal_GetFlags(*args)
    def SetMaximumSectorVisit(*args): return _cspace.iPortal_SetMaximumSectorVisit(*args)
    def GetMaximumSectorVisit(*args): return _cspace.iPortal_GetMaximumSectorVisit(*args)
    def SetPortalCallback(*args): return _cspace.iPortal_SetPortalCallback(*args)
    def RemovePortalCallback(*args): return _cspace.iPortal_RemovePortalCallback(*args)
    def GetPortalCallbackCount(*args): return _cspace.iPortal_GetPortalCallbackCount(*args)
    def GetPortalCallback(*args): return _cspace.iPortal_GetPortalCallback(*args)
    def SetMissingSectorCallback(*args): return _cspace.iPortal_SetMissingSectorCallback(*args)
    def RemoveMissingSectorCallback(*args): return _cspace.iPortal_RemoveMissingSectorCallback(*args)
    def GetMissingSectorCallbackCount(*args): return _cspace.iPortal_GetMissingSectorCallbackCount(*args)
    def GetMissingSectorCallback(*args): return _cspace.iPortal_GetMissingSectorCallback(*args)
    def GetTextureFilter(*args): return _cspace.iPortal_GetTextureFilter(*args)
    def SetFilter(*args): return _cspace.iPortal_SetFilter(*args)
    def GetColorFilter(*args): return _cspace.iPortal_GetColorFilter(*args)
    def SetWarp(*args): return _cspace.iPortal_SetWarp(*args)
    def SetMirror(*args): return _cspace.iPortal_SetMirror(*args)
    def GetWarp(*args): return _cspace.iPortal_GetWarp(*args)
    def HardTransform(*args): return _cspace.iPortal_HardTransform(*args)
    def ObjectToWorld(*args): return _cspace.iPortal_ObjectToWorld(*args)
    def Warp(*args): return _cspace.iPortal_Warp(*args)
    def WarpSpace(*args): return _cspace.iPortal_WarpSpace(*args)
    def CompleteSector(*args): return _cspace.iPortal_CompleteSector(*args)
    def CheckFrustum(*args): return _cspace.iPortal_CheckFrustum(*args)
    def HitBeamPortals(*args): return _cspace.iPortal_HitBeamPortals(*args)
    def GetVerticesCount(*args): return _cspace.iPortal_GetVerticesCount(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iPortal_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iPortal_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iPortal
    __del__ = lambda self : None;
iPortal_swigregister = _cspace.iPortal_swigregister
iPortal_swigregister(iPortal)
iPortal_scfGetVersion = _cspace.iPortal_scfGetVersion

class iPortalContainer(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iPortalContainer, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iPortalContainer, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetPortalCount(*args): return _cspace.iPortalContainer_GetPortalCount(*args)
    def GetPortal(*args): return _cspace.iPortalContainer_GetPortal(*args)
    def CreatePortal(*args): return _cspace.iPortalContainer_CreatePortal(*args)
    def RemovePortal(*args): return _cspace.iPortalContainer_RemovePortal(*args)
    def Draw(*args): return _cspace.iPortalContainer_Draw(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iPortalContainer_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iPortalContainer_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iPortalContainer
    __del__ = lambda self : None;
iPortalContainer_swigregister = _cspace.iPortalContainer_swigregister
iPortalContainer_swigregister(iPortalContainer)
iPortalContainer_scfGetVersion = _cspace.iPortalContainer_scfGetVersion

class iGeneralMeshSubMesh(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iGeneralMeshSubMesh, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iGeneralMeshSubMesh, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetIndices(*args): return _cspace.iGeneralMeshSubMesh_GetIndices(*args)
    def GetMaterial(*args): return _cspace.iGeneralMeshSubMesh_GetMaterial(*args)
    def GetName(*args): return _cspace.iGeneralMeshSubMesh_GetName(*args)
    def GetMixmode(*args): return _cspace.iGeneralMeshSubMesh_GetMixmode(*args)
    def SetMaterial(*args): return _cspace.iGeneralMeshSubMesh_SetMaterial(*args)
    def GetZMode(*args): return _cspace.iGeneralMeshSubMesh_GetZMode(*args)
    def SetZMode(*args): return _cspace.iGeneralMeshSubMesh_SetZMode(*args)
    def GetRenderPriority(*args): return _cspace.iGeneralMeshSubMesh_GetRenderPriority(*args)
    def SetRenderPriority(*args): return _cspace.iGeneralMeshSubMesh_SetRenderPriority(*args)
    def SetMixmode(*args): return _cspace.iGeneralMeshSubMesh_SetMixmode(*args)
    def SetBack2Front(*args): return _cspace.iGeneralMeshSubMesh_SetBack2Front(*args)
    def GetBack2Front(*args): return _cspace.iGeneralMeshSubMesh_GetBack2Front(*args)
    __swig_destroy__ = _cspace.delete_iGeneralMeshSubMesh
    __del__ = lambda self : None;
iGeneralMeshSubMesh_swigregister = _cspace.iGeneralMeshSubMesh_swigregister
iGeneralMeshSubMesh_swigregister(iGeneralMeshSubMesh)

class iGeneralMeshCommonState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iGeneralMeshCommonState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iGeneralMeshCommonState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetLighting(*args): return _cspace.iGeneralMeshCommonState_SetLighting(*args)
    def IsLighting(*args): return _cspace.iGeneralMeshCommonState_IsLighting(*args)
    def SetManualColors(*args): return _cspace.iGeneralMeshCommonState_SetManualColors(*args)
    def IsManualColors(*args): return _cspace.iGeneralMeshCommonState_IsManualColors(*args)
    def SetShadowCasting(*args): return _cspace.iGeneralMeshCommonState_SetShadowCasting(*args)
    def IsShadowCasting(*args): return _cspace.iGeneralMeshCommonState_IsShadowCasting(*args)
    def SetShadowReceiving(*args): return _cspace.iGeneralMeshCommonState_SetShadowReceiving(*args)
    def IsShadowReceiving(*args): return _cspace.iGeneralMeshCommonState_IsShadowReceiving(*args)
    def AddRenderBuffer(*args): return _cspace.iGeneralMeshCommonState_AddRenderBuffer(*args)
    def RemoveRenderBuffer(*args): return _cspace.iGeneralMeshCommonState_RemoveRenderBuffer(*args)
    def GetRenderBufferCount(*args): return _cspace.iGeneralMeshCommonState_GetRenderBufferCount(*args)
    def GetRenderBuffer(*args): return _cspace.iGeneralMeshCommonState_GetRenderBuffer(*args)
    def GetRenderBufferName(*args): return _cspace.iGeneralMeshCommonState_GetRenderBufferName(*args)
    __swig_destroy__ = _cspace.delete_iGeneralMeshCommonState
    __del__ = lambda self : None;
iGeneralMeshCommonState_swigregister = _cspace.iGeneralMeshCommonState_swigregister
iGeneralMeshCommonState_swigregister(iGeneralMeshCommonState)

class iGeneralMeshState(iGeneralMeshCommonState):
    __swig_setmethods__ = {}
    for _s in [iGeneralMeshCommonState]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iGeneralMeshState, name, value)
    __swig_getmethods__ = {}
    for _s in [iGeneralMeshCommonState]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iGeneralMeshState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetAnimationControl(*args): return _cspace.iGeneralMeshState_SetAnimationControl(*args)
    def GetAnimationControl(*args): return _cspace.iGeneralMeshState_GetAnimationControl(*args)
    def FindSubMesh(*args): return _cspace.iGeneralMeshState_FindSubMesh(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iGeneralMeshState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iGeneralMeshState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iGeneralMeshState
    __del__ = lambda self : None;
iGeneralMeshState_swigregister = _cspace.iGeneralMeshState_swigregister
iGeneralMeshState_swigregister(iGeneralMeshState)
iGeneralMeshState_scfGetVersion = _cspace.iGeneralMeshState_scfGetVersion

class iGeneralFactoryState(iGeneralMeshCommonState):
    __swig_setmethods__ = {}
    for _s in [iGeneralMeshCommonState]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iGeneralFactoryState, name, value)
    __swig_getmethods__ = {}
    for _s in [iGeneralMeshCommonState]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iGeneralFactoryState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetColor(*args): return _cspace.iGeneralFactoryState_SetColor(*args)
    def GetColor(*args): return _cspace.iGeneralFactoryState_GetColor(*args)
    def AddVertex(*args): return _cspace.iGeneralFactoryState_AddVertex(*args)
    def SetVertexCount(*args): return _cspace.iGeneralFactoryState_SetVertexCount(*args)
    def GetVertexCount(*args): return _cspace.iGeneralFactoryState_GetVertexCount(*args)
    def GetVertices(self):
      return CSMutableArrayHelper(self.GetVertexByIndex, self.GetVertexCount)


    def GetTexels(self):
      return CSMutableArrayHelper(self.GetTexelByIndex, self.GetVertexCount)


    def GetNormals(self):
      # iGeneralFactoryState::GetVertices()
      return CSMutableArrayHelper(self.GetNormalByIndex, self.GetVertexCount)


    def GetColors(self):
      return CSMutableArrayHelper(self.GetColorByIndex, self.GetVertexCount)


    def AddTriangle(*args): return _cspace.iGeneralFactoryState_AddTriangle(*args)
    def SetTriangleCount(*args): return _cspace.iGeneralFactoryState_SetTriangleCount(*args)
    def GetTriangleCount(*args): return _cspace.iGeneralFactoryState_GetTriangleCount(*args)
    def GetTriangles(self):
      return CSMutableArrayHelper(self.GetTriangleByIndex, self.GetTriangleCount)


    def Invalidate(*args): return _cspace.iGeneralFactoryState_Invalidate(*args)
    def CalculateNormals(*args): return _cspace.iGeneralFactoryState_CalculateNormals(*args)
    def Compress(*args): return _cspace.iGeneralFactoryState_Compress(*args)
    def GenerateBox(*args): return _cspace.iGeneralFactoryState_GenerateBox(*args)
    def GenerateCapsule(*args): return _cspace.iGeneralFactoryState_GenerateCapsule(*args)
    def GenerateSphere(*args): return _cspace.iGeneralFactoryState_GenerateSphere(*args)
    def SetBack2Front(*args): return _cspace.iGeneralFactoryState_SetBack2Front(*args)
    def IsAutoNormals(*args): return _cspace.iGeneralFactoryState_IsAutoNormals(*args)
    def IsBack2Front(*args): return _cspace.iGeneralFactoryState_IsBack2Front(*args)
    def SetAnimationControlFactory(*args): return _cspace.iGeneralFactoryState_SetAnimationControlFactory(*args)
    def GetAnimationControlFactory(*args): return _cspace.iGeneralFactoryState_GetAnimationControlFactory(*args)
    def ClearSubMeshes(*args): return _cspace.iGeneralFactoryState_ClearSubMeshes(*args)
    def AddSubMesh(*args): return _cspace.iGeneralFactoryState_AddSubMesh(*args)
    def FindSubMesh(*args): return _cspace.iGeneralFactoryState_FindSubMesh(*args)
    def DeleteSubMesh(*args): return _cspace.iGeneralFactoryState_DeleteSubMesh(*args)
    def GetSubMeshCount(*args): return _cspace.iGeneralFactoryState_GetSubMeshCount(*args)
    def GetSubMesh(*args): return _cspace.iGeneralFactoryState_GetSubMesh(*args)
    def DisableAutoNormals(*args): return _cspace.iGeneralFactoryState_DisableAutoNormals(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iGeneralFactoryState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iGeneralFactoryState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iGeneralFactoryState
    __del__ = lambda self : None;
    def GetVertexByIndex(*args): return _cspace.iGeneralFactoryState_GetVertexByIndex(*args)
    def GetTexelByIndex(*args): return _cspace.iGeneralFactoryState_GetTexelByIndex(*args)
    def GetNormalByIndex(*args): return _cspace.iGeneralFactoryState_GetNormalByIndex(*args)
    def GetTriangleByIndex(*args): return _cspace.iGeneralFactoryState_GetTriangleByIndex(*args)
    def GetColorByIndex(*args): return _cspace.iGeneralFactoryState_GetColorByIndex(*args)
iGeneralFactoryState_swigregister = _cspace.iGeneralFactoryState_swigregister
iGeneralFactoryState_swigregister(iGeneralFactoryState)
iGeneralFactoryState_scfGetVersion = _cspace.iGeneralFactoryState_scfGetVersion

class iGenMeshAnimationControl(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iGenMeshAnimationControl, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iGenMeshAnimationControl, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def AnimatesVertices(*args): return _cspace.iGenMeshAnimationControl_AnimatesVertices(*args)
    def AnimatesTexels(*args): return _cspace.iGenMeshAnimationControl_AnimatesTexels(*args)
    def AnimatesNormals(*args): return _cspace.iGenMeshAnimationControl_AnimatesNormals(*args)
    def AnimatesColors(*args): return _cspace.iGenMeshAnimationControl_AnimatesColors(*args)
    def Update(*args): return _cspace.iGenMeshAnimationControl_Update(*args)
    def UpdateVertices(*args): return _cspace.iGenMeshAnimationControl_UpdateVertices(*args)
    def UpdateTexels(*args): return _cspace.iGenMeshAnimationControl_UpdateTexels(*args)
    def UpdateNormals(*args): return _cspace.iGenMeshAnimationControl_UpdateNormals(*args)
    def UpdateColors(*args): return _cspace.iGenMeshAnimationControl_UpdateColors(*args)
    __swig_destroy__ = _cspace.delete_iGenMeshAnimationControl
    __del__ = lambda self : None;
iGenMeshAnimationControl_swigregister = _cspace.iGenMeshAnimationControl_swigregister
iGenMeshAnimationControl_swigregister(iGenMeshAnimationControl)

class iGenMeshAnimationControlFactory(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iGenMeshAnimationControlFactory, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iGenMeshAnimationControlFactory, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CreateAnimationControl(*args): return _cspace.iGenMeshAnimationControlFactory_CreateAnimationControl(*args)
    def Load(*args): return _cspace.iGenMeshAnimationControlFactory_Load(*args)
    def Save(*args): return _cspace.iGenMeshAnimationControlFactory_Save(*args)
    __swig_destroy__ = _cspace.delete_iGenMeshAnimationControlFactory
    __del__ = lambda self : None;
iGenMeshAnimationControlFactory_swigregister = _cspace.iGenMeshAnimationControlFactory_swigregister
iGenMeshAnimationControlFactory_swigregister(iGenMeshAnimationControlFactory)

class iGenMeshAnimationControlType(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iGenMeshAnimationControlType, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iGenMeshAnimationControlType, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CreateAnimationControlFactory(*args): return _cspace.iGenMeshAnimationControlType_CreateAnimationControlFactory(*args)
    __swig_destroy__ = _cspace.delete_iGenMeshAnimationControlType
    __del__ = lambda self : None;
iGenMeshAnimationControlType_swigregister = _cspace.iGenMeshAnimationControlType_swigregister
iGenMeshAnimationControlType_swigregister(iGenMeshAnimationControlType)

CS_BTT_NONE = _cspace.CS_BTT_NONE
CS_BTT_SCRIPT = _cspace.CS_BTT_SCRIPT
CS_BTT_RIGID_BODY = _cspace.CS_BTT_RIGID_BODY
CS_BGT_NONE = _cspace.CS_BGT_NONE
CS_BGT_BOX = _cspace.CS_BGT_BOX
CS_BGT_SPHERE = _cspace.CS_BGT_SPHERE
CS_BGT_CYLINDER = _cspace.CS_BGT_CYLINDER
class iSkeletonBone(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeletonBone, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeletonBone, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetName(*args): return _cspace.iSkeletonBone_GetName(*args)
    def SetName(*args): return _cspace.iSkeletonBone_SetName(*args)
    def GetTransform(*args): return _cspace.iSkeletonBone_GetTransform(*args)
    def SetTransform(*args): return _cspace.iSkeletonBone_SetTransform(*args)
    def GetFullTransform(*args): return _cspace.iSkeletonBone_GetFullTransform(*args)
    def SetParent(*args): return _cspace.iSkeletonBone_SetParent(*args)
    def GetParent(*args): return _cspace.iSkeletonBone_GetParent(*args)
    def GetChildrenCount(*args): return _cspace.iSkeletonBone_GetChildrenCount(*args)
    def GetChild(*args): return _cspace.iSkeletonBone_GetChild(*args)
    def FindChild(*args): return _cspace.iSkeletonBone_FindChild(*args)
    def FindChildIndex(*args): return _cspace.iSkeletonBone_FindChildIndex(*args)
    def SetSkinBox(*args): return _cspace.iSkeletonBone_SetSkinBox(*args)
    def GetSkinBox(*args): return _cspace.iSkeletonBone_GetSkinBox(*args)
    def SetUpdateCallback(*args): return _cspace.iSkeletonBone_SetUpdateCallback(*args)
    def GetUpdateCallback(*args): return _cspace.iSkeletonBone_GetUpdateCallback(*args)
    def GetFactory(*args): return _cspace.iSkeletonBone_GetFactory(*args)
    def SetTransformMode(*args): return _cspace.iSkeletonBone_SetTransformMode(*args)
    def GetTransformMode(*args): return _cspace.iSkeletonBone_GetTransformMode(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSkeletonBone_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSkeletonBone_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSkeletonBone
    __del__ = lambda self : None;
iSkeletonBone_swigregister = _cspace.iSkeletonBone_swigregister
iSkeletonBone_swigregister(iSkeletonBone)
iSkeletonBone_scfGetVersion = _cspace.iSkeletonBone_scfGetVersion

class iSkeletonBoneUpdateCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeletonBoneUpdateCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeletonBoneUpdateCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def UpdateTransform(*args): return _cspace.iSkeletonBoneUpdateCallback_UpdateTransform(*args)
    __swig_destroy__ = _cspace.delete_iSkeletonBoneUpdateCallback
    __del__ = lambda self : None;
iSkeletonBoneUpdateCallback_swigregister = _cspace.iSkeletonBoneUpdateCallback_swigregister
iSkeletonBoneUpdateCallback_swigregister(iSkeletonBoneUpdateCallback)

class iSkeletonAnimationKeyFrame(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeletonAnimationKeyFrame, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeletonAnimationKeyFrame, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetName(*args): return _cspace.iSkeletonAnimationKeyFrame_GetName(*args)
    def SetName(*args): return _cspace.iSkeletonAnimationKeyFrame_SetName(*args)
    def GetDuration(*args): return _cspace.iSkeletonAnimationKeyFrame_GetDuration(*args)
    def SetDuration(*args): return _cspace.iSkeletonAnimationKeyFrame_SetDuration(*args)
    def GetTransformsCount(*args): return _cspace.iSkeletonAnimationKeyFrame_GetTransformsCount(*args)
    def AddTransform(*args): return _cspace.iSkeletonAnimationKeyFrame_AddTransform(*args)
    def GetTransform(*args): return _cspace.iSkeletonAnimationKeyFrame_GetTransform(*args)
    def SetTransform(*args): return _cspace.iSkeletonAnimationKeyFrame_SetTransform(*args)
    def GetKeyFrameData(*args): return _cspace.iSkeletonAnimationKeyFrame_GetKeyFrameData(*args)
    __swig_destroy__ = _cspace.delete_iSkeletonAnimationKeyFrame
    __del__ = lambda self : None;
iSkeletonAnimationKeyFrame_swigregister = _cspace.iSkeletonAnimationKeyFrame_swigregister
iSkeletonAnimationKeyFrame_swigregister(iSkeletonAnimationKeyFrame)

class iSkeletonAnimation(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeletonAnimation, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeletonAnimation, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetName(*args): return _cspace.iSkeletonAnimation_GetName(*args)
    def SetName(*args): return _cspace.iSkeletonAnimation_SetName(*args)
    def GetTime(*args): return _cspace.iSkeletonAnimation_GetTime(*args)
    def SetTime(*args): return _cspace.iSkeletonAnimation_SetTime(*args)
    def GetSpeed(*args): return _cspace.iSkeletonAnimation_GetSpeed(*args)
    def SetSpeed(*args): return _cspace.iSkeletonAnimation_SetSpeed(*args)
    def SetFactor(*args): return _cspace.iSkeletonAnimation_SetFactor(*args)
    def GetFactor(*args): return _cspace.iSkeletonAnimation_GetFactor(*args)
    def SetLoop(*args): return _cspace.iSkeletonAnimation_SetLoop(*args)
    def GetLoop(*args): return _cspace.iSkeletonAnimation_GetLoop(*args)
    def CreateFrame(*args): return _cspace.iSkeletonAnimation_CreateFrame(*args)
    def GetFramesCount(*args): return _cspace.iSkeletonAnimation_GetFramesCount(*args)
    def GetFrame(*args): return _cspace.iSkeletonAnimation_GetFrame(*args)
    def FindFrameIndex(*args): return _cspace.iSkeletonAnimation_FindFrameIndex(*args)
    def RemoveFrame(*args): return _cspace.iSkeletonAnimation_RemoveFrame(*args)
    def RemoveAllFrames(*args): return _cspace.iSkeletonAnimation_RemoveAllFrames(*args)
    def RecalcSpline(*args): return _cspace.iSkeletonAnimation_RecalcSpline(*args)
    __swig_destroy__ = _cspace.delete_iSkeletonAnimation
    __del__ = lambda self : None;
iSkeletonAnimation_swigregister = _cspace.iSkeletonAnimation_swigregister
iSkeletonAnimation_swigregister(iSkeletonAnimation)

class iSkeletonAnimationCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeletonAnimationCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeletonAnimationCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Execute(*args): return _cspace.iSkeletonAnimationCallback_Execute(*args)
    def OnFinish(*args): return _cspace.iSkeletonAnimationCallback_OnFinish(*args)
    __swig_destroy__ = _cspace.delete_iSkeletonAnimationCallback
    __del__ = lambda self : None;
iSkeletonAnimationCallback_swigregister = _cspace.iSkeletonAnimationCallback_swigregister
iSkeletonAnimationCallback_swigregister(iSkeletonAnimationCallback)

class iSkeletonUpdateCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeletonUpdateCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeletonUpdateCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Execute(*args): return _cspace.iSkeletonUpdateCallback_Execute(*args)
    __swig_destroy__ = _cspace.delete_iSkeletonUpdateCallback
    __del__ = lambda self : None;
iSkeletonUpdateCallback_swigregister = _cspace.iSkeletonUpdateCallback_swigregister
iSkeletonUpdateCallback_swigregister(iSkeletonUpdateCallback)

class iSkeletonAnimationInstance(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeletonAnimationInstance, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeletonAnimationInstance, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetSpeed(*args): return _cspace.iSkeletonAnimationInstance_GetSpeed(*args)
    def SetSpeed(*args): return _cspace.iSkeletonAnimationInstance_SetSpeed(*args)
    def SetFactor(*args): return _cspace.iSkeletonAnimationInstance_SetFactor(*args)
    def GetFactor(*args): return _cspace.iSkeletonAnimationInstance_GetFactor(*args)
    def GetDuration(*args): return _cspace.iSkeletonAnimationInstance_GetDuration(*args)
    def SetDuration(*args): return _cspace.iSkeletonAnimationInstance_SetDuration(*args)
    __swig_destroy__ = _cspace.delete_iSkeletonAnimationInstance
    __del__ = lambda self : None;
iSkeletonAnimationInstance_swigregister = _cspace.iSkeletonAnimationInstance_swigregister
iSkeletonAnimationInstance_swigregister(iSkeletonAnimationInstance)

class iSkeleton(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeleton, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeleton, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetName(*args): return _cspace.iSkeleton_GetName(*args)
    def SetName(*args): return _cspace.iSkeleton_SetName(*args)
    def GetBonesCount(*args): return _cspace.iSkeleton_GetBonesCount(*args)
    def GetBone(*args): return _cspace.iSkeleton_GetBone(*args)
    def FindBone(*args): return _cspace.iSkeleton_FindBone(*args)
    def FindBoneIndex(*args): return _cspace.iSkeleton_FindBoneIndex(*args)
    def Execute(*args): return _cspace.iSkeleton_Execute(*args)
    def Append(*args): return _cspace.iSkeleton_Append(*args)
    def Play(*args): return _cspace.iSkeleton_Play(*args)
    def ClearPendingAnimations(*args): return _cspace.iSkeleton_ClearPendingAnimations(*args)
    def GetAnimationsCount(*args): return _cspace.iSkeleton_GetAnimationsCount(*args)
    def GetAnimation(*args): return _cspace.iSkeleton_GetAnimation(*args)
    def FindAnimation(*args): return _cspace.iSkeleton_FindAnimation(*args)
    def FindSocket(*args): return _cspace.iSkeleton_FindSocket(*args)
    def StopAll(*args): return _cspace.iSkeleton_StopAll(*args)
    def Stop(*args): return _cspace.iSkeleton_Stop(*args)
    def GetFactory(*args): return _cspace.iSkeleton_GetFactory(*args)
    def SetAnimationCallback(*args): return _cspace.iSkeleton_SetAnimationCallback(*args)
    def AddUpdateCallback(*args): return _cspace.iSkeleton_AddUpdateCallback(*args)
    def GetUpdateCallbacksCount(*args): return _cspace.iSkeleton_GetUpdateCallbacksCount(*args)
    def GetUpdateCallback(*args): return _cspace.iSkeleton_GetUpdateCallback(*args)
    def RemoveUpdateCallback(*args): return _cspace.iSkeleton_RemoveUpdateCallback(*args)
    def UpdateAnimation(*args): return _cspace.iSkeleton_UpdateAnimation(*args)
    def UpdateBones(*args): return _cspace.iSkeleton_UpdateBones(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSkeleton_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSkeleton_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSkeleton
    __del__ = lambda self : None;
iSkeleton_swigregister = _cspace.iSkeleton_swigregister
iSkeleton_swigregister(iSkeleton)
iSkeleton_scfGetVersion = _cspace.iSkeleton_scfGetVersion

class iSkeletonSocket(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeletonSocket, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeletonSocket, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetName(*args): return _cspace.iSkeletonSocket_GetName(*args)
    def SetName(*args): return _cspace.iSkeletonSocket_SetName(*args)
    def GetTransform(*args): return _cspace.iSkeletonSocket_GetTransform(*args)
    def SetTransform(*args): return _cspace.iSkeletonSocket_SetTransform(*args)
    def GetFullTransform(*args): return _cspace.iSkeletonSocket_GetFullTransform(*args)
    def SetBone(*args): return _cspace.iSkeletonSocket_SetBone(*args)
    def GetBone(*args): return _cspace.iSkeletonSocket_GetBone(*args)
    def SetSceneNode(*args): return _cspace.iSkeletonSocket_SetSceneNode(*args)
    def GetSceneNode(*args): return _cspace.iSkeletonSocket_GetSceneNode(*args)
    def GetFactory(*args): return _cspace.iSkeletonSocket_GetFactory(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSkeletonSocket_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSkeletonSocket_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSkeletonSocket
    __del__ = lambda self : None;
iSkeletonSocket_swigregister = _cspace.iSkeletonSocket_swigregister
iSkeletonSocket_swigregister(iSkeletonSocket)
iSkeletonSocket_scfGetVersion = _cspace.iSkeletonSocket_scfGetVersion

class iSkeletonBoneRagdollInfo(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeletonBoneRagdollInfo, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeletonBoneRagdollInfo, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetEnabled(*args): return _cspace.iSkeletonBoneRagdollInfo_SetEnabled(*args)
    def GetEnabled(*args): return _cspace.iSkeletonBoneRagdollInfo_GetEnabled(*args)
    def SetAttachToParent(*args): return _cspace.iSkeletonBoneRagdollInfo_SetAttachToParent(*args)
    def GetAttachToParent(*args): return _cspace.iSkeletonBoneRagdollInfo_GetAttachToParent(*args)
    def SetGeomName(*args): return _cspace.iSkeletonBoneRagdollInfo_SetGeomName(*args)
    def GetGeomName(*args): return _cspace.iSkeletonBoneRagdollInfo_GetGeomName(*args)
    def SetGeomType(*args): return _cspace.iSkeletonBoneRagdollInfo_SetGeomType(*args)
    def GetGeomType(*args): return _cspace.iSkeletonBoneRagdollInfo_GetGeomType(*args)
    def SetGeomDimensions(*args): return _cspace.iSkeletonBoneRagdollInfo_SetGeomDimensions(*args)
    def GetGeomDimensions(*args): return _cspace.iSkeletonBoneRagdollInfo_GetGeomDimensions(*args)
    def SetFriction(*args): return _cspace.iSkeletonBoneRagdollInfo_SetFriction(*args)
    def GetFriction(*args): return _cspace.iSkeletonBoneRagdollInfo_GetFriction(*args)
    def SetElasticity(*args): return _cspace.iSkeletonBoneRagdollInfo_SetElasticity(*args)
    def GetElasticity(*args): return _cspace.iSkeletonBoneRagdollInfo_GetElasticity(*args)
    def SetSoftness(*args): return _cspace.iSkeletonBoneRagdollInfo_SetSoftness(*args)
    def GetSoftness(*args): return _cspace.iSkeletonBoneRagdollInfo_GetSoftness(*args)
    def SetSlip(*args): return _cspace.iSkeletonBoneRagdollInfo_SetSlip(*args)
    def GetSlip(*args): return _cspace.iSkeletonBoneRagdollInfo_GetSlip(*args)
    def SetBodyName(*args): return _cspace.iSkeletonBoneRagdollInfo_SetBodyName(*args)
    def GetBodyName(*args): return _cspace.iSkeletonBoneRagdollInfo_GetBodyName(*args)
    def SetBodyMass(*args): return _cspace.iSkeletonBoneRagdollInfo_SetBodyMass(*args)
    def GetBodyMass(*args): return _cspace.iSkeletonBoneRagdollInfo_GetBodyMass(*args)
    def SetBodyGravmode(*args): return _cspace.iSkeletonBoneRagdollInfo_SetBodyGravmode(*args)
    def GetBodyGravmode(*args): return _cspace.iSkeletonBoneRagdollInfo_GetBodyGravmode(*args)
    def SetJointName(*args): return _cspace.iSkeletonBoneRagdollInfo_SetJointName(*args)
    def GetJointName(*args): return _cspace.iSkeletonBoneRagdollInfo_GetJointName(*args)
    def SetJointMinRotContraints(*args): return _cspace.iSkeletonBoneRagdollInfo_SetJointMinRotContraints(*args)
    def GetJointMinRotContraints(*args): return _cspace.iSkeletonBoneRagdollInfo_GetJointMinRotContraints(*args)
    def SetJointMaxRotContraints(*args): return _cspace.iSkeletonBoneRagdollInfo_SetJointMaxRotContraints(*args)
    def GetJointMaxRotContraints(*args): return _cspace.iSkeletonBoneRagdollInfo_GetJointMaxRotContraints(*args)
    def SetJointMinTransContraints(*args): return _cspace.iSkeletonBoneRagdollInfo_SetJointMinTransContraints(*args)
    def GetJointMinTransContraints(*args): return _cspace.iSkeletonBoneRagdollInfo_GetJointMinTransContraints(*args)
    def SetJointMaxTransContraints(*args): return _cspace.iSkeletonBoneRagdollInfo_SetJointMaxTransContraints(*args)
    def GetJointMaxTransContraints(*args): return _cspace.iSkeletonBoneRagdollInfo_GetJointMaxTransContraints(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSkeletonBoneRagdollInfo_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSkeletonBoneRagdollInfo_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSkeletonBoneRagdollInfo
    __del__ = lambda self : None;
iSkeletonBoneRagdollInfo_swigregister = _cspace.iSkeletonBoneRagdollInfo_swigregister
iSkeletonBoneRagdollInfo_swigregister(iSkeletonBoneRagdollInfo)
iSkeletonBoneRagdollInfo_scfGetVersion = _cspace.iSkeletonBoneRagdollInfo_scfGetVersion

class iSkeletonBoneFactory(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeletonBoneFactory, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeletonBoneFactory, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetName(*args): return _cspace.iSkeletonBoneFactory_GetName(*args)
    def SetName(*args): return _cspace.iSkeletonBoneFactory_SetName(*args)
    def GetTransform(*args): return _cspace.iSkeletonBoneFactory_GetTransform(*args)
    def SetTransform(*args): return _cspace.iSkeletonBoneFactory_SetTransform(*args)
    def GetFullTransform(*args): return _cspace.iSkeletonBoneFactory_GetFullTransform(*args)
    def SetParent(*args): return _cspace.iSkeletonBoneFactory_SetParent(*args)
    def GetParent(*args): return _cspace.iSkeletonBoneFactory_GetParent(*args)
    def GetChildrenCount(*args): return _cspace.iSkeletonBoneFactory_GetChildrenCount(*args)
    def GetChild(*args): return _cspace.iSkeletonBoneFactory_GetChild(*args)
    def FindChild(*args): return _cspace.iSkeletonBoneFactory_FindChild(*args)
    def FindChildIndex(*args): return _cspace.iSkeletonBoneFactory_FindChildIndex(*args)
    def SetSkinBox(*args): return _cspace.iSkeletonBoneFactory_SetSkinBox(*args)
    def GetSkinBox(*args): return _cspace.iSkeletonBoneFactory_GetSkinBox(*args)
    def GetRagdollInfo(*args): return _cspace.iSkeletonBoneFactory_GetRagdollInfo(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSkeletonBoneFactory_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSkeletonBoneFactory_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSkeletonBoneFactory
    __del__ = lambda self : None;
iSkeletonBoneFactory_swigregister = _cspace.iSkeletonBoneFactory_swigregister
iSkeletonBoneFactory_swigregister(iSkeletonBoneFactory)
iSkeletonBoneFactory_scfGetVersion = _cspace.iSkeletonBoneFactory_scfGetVersion

class iSkeletonSocketFactory(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeletonSocketFactory, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeletonSocketFactory, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetName(*args): return _cspace.iSkeletonSocketFactory_GetName(*args)
    def SetName(*args): return _cspace.iSkeletonSocketFactory_SetName(*args)
    def GetTransform(*args): return _cspace.iSkeletonSocketFactory_GetTransform(*args)
    def SetTransform(*args): return _cspace.iSkeletonSocketFactory_SetTransform(*args)
    def GetFullTransform(*args): return _cspace.iSkeletonSocketFactory_GetFullTransform(*args)
    def SetBone(*args): return _cspace.iSkeletonSocketFactory_SetBone(*args)
    def GetBone(*args): return _cspace.iSkeletonSocketFactory_GetBone(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSkeletonSocketFactory_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSkeletonSocketFactory_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSkeletonSocketFactory
    __del__ = lambda self : None;
iSkeletonSocketFactory_swigregister = _cspace.iSkeletonSocketFactory_swigregister
iSkeletonSocketFactory_swigregister(iSkeletonSocketFactory)
iSkeletonSocketFactory_scfGetVersion = _cspace.iSkeletonSocketFactory_scfGetVersion

class iSkeletonFactory(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeletonFactory, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeletonFactory, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetName(*args): return _cspace.iSkeletonFactory_GetName(*args)
    def SetName(*args): return _cspace.iSkeletonFactory_SetName(*args)
    def CreateBone(*args): return _cspace.iSkeletonFactory_CreateBone(*args)
    def CreateAnimation(*args): return _cspace.iSkeletonFactory_CreateAnimation(*args)
    def FindAnimation(*args): return _cspace.iSkeletonFactory_FindAnimation(*args)
    def GetAnimationsCount(*args): return _cspace.iSkeletonFactory_GetAnimationsCount(*args)
    def GetAnimation(*args): return _cspace.iSkeletonFactory_GetAnimation(*args)
    def FindBone(*args): return _cspace.iSkeletonFactory_FindBone(*args)
    def FindBoneIndex(*args): return _cspace.iSkeletonFactory_FindBoneIndex(*args)
    def GetBonesCount(*args): return _cspace.iSkeletonFactory_GetBonesCount(*args)
    def GetBone(*args): return _cspace.iSkeletonFactory_GetBone(*args)
    def GetGraveyard(*args): return _cspace.iSkeletonFactory_GetGraveyard(*args)
    def CreateSocket(*args): return _cspace.iSkeletonFactory_CreateSocket(*args)
    def FindSocket(*args): return _cspace.iSkeletonFactory_FindSocket(*args)
    def GetSocket(*args): return _cspace.iSkeletonFactory_GetSocket(*args)
    def RemoveSocket(*args): return _cspace.iSkeletonFactory_RemoveSocket(*args)
    def GetSocketsCount(*args): return _cspace.iSkeletonFactory_GetSocketsCount(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSkeletonFactory_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSkeletonFactory_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSkeletonFactory
    __del__ = lambda self : None;
iSkeletonFactory_swigregister = _cspace.iSkeletonFactory_swigregister
iSkeletonFactory_swigregister(iSkeletonFactory)
iSkeletonFactory_scfGetVersion = _cspace.iSkeletonFactory_scfGetVersion

class iSkeletonGraveyard(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSkeletonGraveyard, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSkeletonGraveyard, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetFactoriesCount(*args): return _cspace.iSkeletonGraveyard_GetFactoriesCount(*args)
    def CreateFactory(*args): return _cspace.iSkeletonGraveyard_CreateFactory(*args)
    def LoadFactory(*args): return _cspace.iSkeletonGraveyard_LoadFactory(*args)
    def FindFactory(*args): return _cspace.iSkeletonGraveyard_FindFactory(*args)
    def CreateSkeleton(*args): return _cspace.iSkeletonGraveyard_CreateSkeleton(*args)
    def SetManualUpdates(*args): return _cspace.iSkeletonGraveyard_SetManualUpdates(*args)
    def Update(*args): return _cspace.iSkeletonGraveyard_Update(*args)
    def AddSkeleton(*args): return _cspace.iSkeletonGraveyard_AddSkeleton(*args)
    def RemoveSkeleton(*args): return _cspace.iSkeletonGraveyard_RemoveSkeleton(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSkeletonGraveyard_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSkeletonGraveyard_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSkeletonGraveyard
    __del__ = lambda self : None;
iSkeletonGraveyard_swigregister = _cspace.iSkeletonGraveyard_swigregister
iSkeletonGraveyard_swigregister(iSkeletonGraveyard)
iSkeletonGraveyard_scfGetVersion = _cspace.iSkeletonGraveyard_scfGetVersion

class iGenMeshSkeletonControlState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iGenMeshSkeletonControlState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iGenMeshSkeletonControlState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetAnimatedVertices(*args): return _cspace.iGenMeshSkeletonControlState_GetAnimatedVertices(*args)
    def GetAnimatedVerticesCount(*args): return _cspace.iGenMeshSkeletonControlState_GetAnimatedVerticesCount(*args)
    def GetAnimatedFaceNormals(*args): return _cspace.iGenMeshSkeletonControlState_GetAnimatedFaceNormals(*args)
    def GetAnimatedFaceNormalsCount(*args): return _cspace.iGenMeshSkeletonControlState_GetAnimatedFaceNormalsCount(*args)
    def GetAnimatedVertNormals(*args): return _cspace.iGenMeshSkeletonControlState_GetAnimatedVertNormals(*args)
    def GetAnimatedVertNormalsCount(*args): return _cspace.iGenMeshSkeletonControlState_GetAnimatedVertNormalsCount(*args)
    def GetSkeleton(*args): return _cspace.iGenMeshSkeletonControlState_GetSkeleton(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iGenMeshSkeletonControlState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iGenMeshSkeletonControlState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iGenMeshSkeletonControlState
    __del__ = lambda self : None;
iGenMeshSkeletonControlState_swigregister = _cspace.iGenMeshSkeletonControlState_swigregister
iGenMeshSkeletonControlState_swigregister(iGenMeshSkeletonControlState)
iGenMeshSkeletonControlState_scfGetVersion = _cspace.iGenMeshSkeletonControlState_scfGetVersion

class csSprite2DVertexArrayReadOnly(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSprite2DVertexArrayReadOnly, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csSprite2DVertexArrayReadOnly, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetSize(*args): return _cspace.csSprite2DVertexArrayReadOnly_GetSize(*args)
    def Get(*args): return _cspace.csSprite2DVertexArrayReadOnly_Get(*args)
    def Top(*args): return _cspace.csSprite2DVertexArrayReadOnly_Top(*args)
    def Find(*args): return _cspace.csSprite2DVertexArrayReadOnly_Find(*args)
    def GetIndex(*args): return _cspace.csSprite2DVertexArrayReadOnly_GetIndex(*args)
    def IsEmpty(*args): return _cspace.csSprite2DVertexArrayReadOnly_IsEmpty(*args)
    def GetAll(*args): return _cspace.csSprite2DVertexArrayReadOnly_GetAll(*args)
    __swig_destroy__ = _cspace.delete_csSprite2DVertexArrayReadOnly
    __del__ = lambda self : None;
csSprite2DVertexArrayReadOnly_swigregister = _cspace.csSprite2DVertexArrayReadOnly_swigregister
csSprite2DVertexArrayReadOnly_swigregister(csSprite2DVertexArrayReadOnly)

class csSprite2DVertexArrayChangeElements(csSprite2DVertexArrayReadOnly):
    __swig_setmethods__ = {}
    for _s in [csSprite2DVertexArrayReadOnly]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSprite2DVertexArrayChangeElements, name, value)
    __swig_getmethods__ = {}
    for _s in [csSprite2DVertexArrayReadOnly]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csSprite2DVertexArrayChangeElements, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Get(*args): return _cspace.csSprite2DVertexArrayChangeElements_Get(*args)
    def Top(*args): return _cspace.csSprite2DVertexArrayChangeElements_Top(*args)
    __swig_destroy__ = _cspace.delete_csSprite2DVertexArrayChangeElements
    __del__ = lambda self : None;
csSprite2DVertexArrayChangeElements_swigregister = _cspace.csSprite2DVertexArrayChangeElements_swigregister
csSprite2DVertexArrayChangeElements_swigregister(csSprite2DVertexArrayChangeElements)

class csSprite2DVertexArrayChangeAll(csSprite2DVertexArrayChangeElements):
    __swig_setmethods__ = {}
    for _s in [csSprite2DVertexArrayChangeElements]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSprite2DVertexArrayChangeAll, name, value)
    __swig_getmethods__ = {}
    for _s in [csSprite2DVertexArrayChangeElements]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csSprite2DVertexArrayChangeAll, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetSize(*args): return _cspace.csSprite2DVertexArrayChangeAll_SetSize(*args)
    def GetExtend(*args): return _cspace.csSprite2DVertexArrayChangeAll_GetExtend(*args)
    def Put(*args): return _cspace.csSprite2DVertexArrayChangeAll_Put(*args)
    def Push(*args): return _cspace.csSprite2DVertexArrayChangeAll_Push(*args)
    def PushSmart(*args): return _cspace.csSprite2DVertexArrayChangeAll_PushSmart(*args)
    def Pop(*args): return _cspace.csSprite2DVertexArrayChangeAll_Pop(*args)
    def Insert(*args): return _cspace.csSprite2DVertexArrayChangeAll_Insert(*args)
    def DeleteAll(*args): return _cspace.csSprite2DVertexArrayChangeAll_DeleteAll(*args)
    def Truncate(*args): return _cspace.csSprite2DVertexArrayChangeAll_Truncate(*args)
    def Empty(*args): return _cspace.csSprite2DVertexArrayChangeAll_Empty(*args)
    def DeleteIndex(*args): return _cspace.csSprite2DVertexArrayChangeAll_DeleteIndex(*args)
    def DeleteIndexFast(*args): return _cspace.csSprite2DVertexArrayChangeAll_DeleteIndexFast(*args)
    def Delete(*args): return _cspace.csSprite2DVertexArrayChangeAll_Delete(*args)
    __swig_destroy__ = _cspace.delete_csSprite2DVertexArrayChangeAll
    __del__ = lambda self : None;
csSprite2DVertexArrayChangeAll_swigregister = _cspace.csSprite2DVertexArrayChangeAll_swigregister
csSprite2DVertexArrayChangeAll_swigregister(csSprite2DVertexArrayChangeAll)

class csSprite2DVertex(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSprite2DVertex, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csSprite2DVertex, name)
    __repr__ = _swig_repr
    __swig_setmethods__["pos"] = _cspace.csSprite2DVertex_pos_set
    __swig_getmethods__["pos"] = _cspace.csSprite2DVertex_pos_get
    if _newclass:pos = property(_cspace.csSprite2DVertex_pos_get, _cspace.csSprite2DVertex_pos_set)
    __swig_setmethods__["color_init"] = _cspace.csSprite2DVertex_color_init_set
    __swig_getmethods__["color_init"] = _cspace.csSprite2DVertex_color_init_get
    if _newclass:color_init = property(_cspace.csSprite2DVertex_color_init_get, _cspace.csSprite2DVertex_color_init_set)
    __swig_setmethods__["color"] = _cspace.csSprite2DVertex_color_set
    __swig_getmethods__["color"] = _cspace.csSprite2DVertex_color_get
    if _newclass:color = property(_cspace.csSprite2DVertex_color_get, _cspace.csSprite2DVertex_color_set)
    __swig_setmethods__["u"] = _cspace.csSprite2DVertex_u_set
    __swig_getmethods__["u"] = _cspace.csSprite2DVertex_u_get
    if _newclass:u = property(_cspace.csSprite2DVertex_u_get, _cspace.csSprite2DVertex_u_set)
    __swig_setmethods__["v"] = _cspace.csSprite2DVertex_v_set
    __swig_getmethods__["v"] = _cspace.csSprite2DVertex_v_get
    if _newclass:v = property(_cspace.csSprite2DVertex_v_get, _cspace.csSprite2DVertex_v_set)
    def __eq__(*args): return _cspace.csSprite2DVertex___eq__(*args)
    def __init__(self, *args): 
        this = _cspace.new_csSprite2DVertex(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csSprite2DVertex
    __del__ = lambda self : None;
csSprite2DVertex_swigregister = _cspace.csSprite2DVertex_swigregister
csSprite2DVertex_swigregister(csSprite2DVertex)

class iColoredVertices(csSprite2DVertexArrayChangeAll):
    __swig_setmethods__ = {}
    for _s in [csSprite2DVertexArrayChangeAll]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iColoredVertices, name, value)
    __swig_getmethods__ = {}
    for _s in [csSprite2DVertexArrayChangeAll]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iColoredVertices, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_iColoredVertices
    __del__ = lambda self : None;
iColoredVertices_swigregister = _cspace.iColoredVertices_swigregister
iColoredVertices_swigregister(iColoredVertices)

class iSprite2DUVAnimationFrame(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSprite2DUVAnimationFrame, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSprite2DUVAnimationFrame, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetName(*args): return _cspace.iSprite2DUVAnimationFrame_SetName(*args)
    def GetName(*args): return _cspace.iSprite2DUVAnimationFrame_GetName(*args)
    def GetUVCoo(*args): return _cspace.iSprite2DUVAnimationFrame_GetUVCoo(*args)
    def GetUVCount(*args): return _cspace.iSprite2DUVAnimationFrame_GetUVCount(*args)
    def SetUV(*args): return _cspace.iSprite2DUVAnimationFrame_SetUV(*args)
    def SetFrameData(*args): return _cspace.iSprite2DUVAnimationFrame_SetFrameData(*args)
    def RemoveUV(*args): return _cspace.iSprite2DUVAnimationFrame_RemoveUV(*args)
    def GetDuration(*args): return _cspace.iSprite2DUVAnimationFrame_GetDuration(*args)
    def SetDuration(*args): return _cspace.iSprite2DUVAnimationFrame_SetDuration(*args)
    __swig_destroy__ = _cspace.delete_iSprite2DUVAnimationFrame
    __del__ = lambda self : None;
iSprite2DUVAnimationFrame_swigregister = _cspace.iSprite2DUVAnimationFrame_swigregister
iSprite2DUVAnimationFrame_swigregister(iSprite2DUVAnimationFrame)

class iSprite2DUVAnimation(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSprite2DUVAnimation, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSprite2DUVAnimation, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetName(*args): return _cspace.iSprite2DUVAnimation_SetName(*args)
    def GetName(*args): return _cspace.iSprite2DUVAnimation_GetName(*args)
    def GetFrameCount(*args): return _cspace.iSprite2DUVAnimation_GetFrameCount(*args)
    def GetFrame(*args): return _cspace.iSprite2DUVAnimation_GetFrame(*args)
    def CreateFrame(*args): return _cspace.iSprite2DUVAnimation_CreateFrame(*args)
    def MoveFrame(*args): return _cspace.iSprite2DUVAnimation_MoveFrame(*args)
    def RemoveFrame(*args): return _cspace.iSprite2DUVAnimation_RemoveFrame(*args)
    __swig_destroy__ = _cspace.delete_iSprite2DUVAnimation
    __del__ = lambda self : None;
iSprite2DUVAnimation_swigregister = _cspace.iSprite2DUVAnimation_swigregister
iSprite2DUVAnimation_swigregister(iSprite2DUVAnimation)

class iSprite2DFactoryState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSprite2DFactoryState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSprite2DFactoryState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetLighting(*args): return _cspace.iSprite2DFactoryState_SetLighting(*args)
    def HasLighting(*args): return _cspace.iSprite2DFactoryState_HasLighting(*args)
    def GetUVAnimationCount(*args): return _cspace.iSprite2DFactoryState_GetUVAnimationCount(*args)
    def CreateUVAnimation(*args): return _cspace.iSprite2DFactoryState_CreateUVAnimation(*args)
    def RemoveUVAnimation(*args): return _cspace.iSprite2DFactoryState_RemoveUVAnimation(*args)
    def GetUVAnimation(*args): return _cspace.iSprite2DFactoryState_GetUVAnimation(*args)
    __swig_destroy__ = _cspace.delete_iSprite2DFactoryState
    __del__ = lambda self : None;
iSprite2DFactoryState_swigregister = _cspace.iSprite2DFactoryState_swigregister
iSprite2DFactoryState_swigregister(iSprite2DFactoryState)

class iSprite2DState(iSprite2DFactoryState):
    __swig_setmethods__ = {}
    for _s in [iSprite2DFactoryState]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSprite2DState, name, value)
    __swig_getmethods__ = {}
    for _s in [iSprite2DFactoryState]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSprite2DState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CreateRegularVertices(*args): return _cspace.iSprite2DState_CreateRegularVertices(*args)
    def SetUVAnimation(*args): return _cspace.iSprite2DState_SetUVAnimation(*args)
    def GetUVAnimation(*args): return _cspace.iSprite2DState_GetUVAnimation(*args)
    def StopUVAnimation(*args): return _cspace.iSprite2DState_StopUVAnimation(*args)
    def PlayUVAnimation(*args): return _cspace.iSprite2DState_PlayUVAnimation(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSprite2DState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSprite2DState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSprite2DState
    __del__ = lambda self : None;
    def GetVertexByIndex(*args): return _cspace.iSprite2DState_GetVertexByIndex(*args)
    def GetVertexCount(*args): return _cspace.iSprite2DState_GetVertexCount(*args)
iSprite2DState_swigregister = _cspace.iSprite2DState_swigregister
iSprite2DState_swigregister(iSprite2DState)
iSprite2DState_scfGetVersion = _cspace.iSprite2DState_scfGetVersion

CS_SPR_LIGHTING_HQ = _cspace.CS_SPR_LIGHTING_HQ
CS_SPR_LIGHTING_LQ = _cspace.CS_SPR_LIGHTING_LQ
CS_SPR_LIGHTING_FAST = _cspace.CS_SPR_LIGHTING_FAST
CS_SPR_LIGHTING_RANDOM = _cspace.CS_SPR_LIGHTING_RANDOM
CS_SPR_LIGHT_GLOBAL = _cspace.CS_SPR_LIGHT_GLOBAL
CS_SPR_LIGHT_TEMPLATE = _cspace.CS_SPR_LIGHT_TEMPLATE
CS_SPR_LIGHT_LOCAL = _cspace.CS_SPR_LIGHT_LOCAL
CS_SPR_LOD_GLOBAL = _cspace.CS_SPR_LOD_GLOBAL
CS_SPR_LOD_TEMPLATE = _cspace.CS_SPR_LOD_TEMPLATE
CS_SPR_LOD_LOCAL = _cspace.CS_SPR_LOD_LOCAL
class iSpriteFrame(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSpriteFrame, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSpriteFrame, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetName(*args): return _cspace.iSpriteFrame_SetName(*args)
    def GetName(*args): return _cspace.iSpriteFrame_GetName(*args)
    def GetAnmIndex(*args): return _cspace.iSpriteFrame_GetAnmIndex(*args)
    def GetTexIndex(*args): return _cspace.iSpriteFrame_GetTexIndex(*args)
    __swig_destroy__ = _cspace.delete_iSpriteFrame
    __del__ = lambda self : None;
iSpriteFrame_swigregister = _cspace.iSpriteFrame_swigregister
iSpriteFrame_swigregister(iSpriteFrame)

class iSpriteAction(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSpriteAction, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSpriteAction, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetName(*args): return _cspace.iSpriteAction_SetName(*args)
    def GetName(*args): return _cspace.iSpriteAction_GetName(*args)
    def GetFrameCount(*args): return _cspace.iSpriteAction_GetFrameCount(*args)
    def GetFrame(*args): return _cspace.iSpriteAction_GetFrame(*args)
    def GetNextFrame(*args): return _cspace.iSpriteAction_GetNextFrame(*args)
    def GetFrameDelay(*args): return _cspace.iSpriteAction_GetFrameDelay(*args)
    def GetFrameDisplacement(*args): return _cspace.iSpriteAction_GetFrameDisplacement(*args)
    def AddFrame(*args): return _cspace.iSpriteAction_AddFrame(*args)
    __swig_destroy__ = _cspace.delete_iSpriteAction
    __del__ = lambda self : None;
iSpriteAction_swigregister = _cspace.iSpriteAction_swigregister
iSpriteAction_swigregister(iSpriteAction)

class iSpriteSocket(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSpriteSocket, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSpriteSocket, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetName(*args): return _cspace.iSpriteSocket_SetName(*args)
    def GetName(*args): return _cspace.iSpriteSocket_GetName(*args)
    def SetMeshWrapper(*args): return _cspace.iSpriteSocket_SetMeshWrapper(*args)
    def GetMeshWrapper(*args): return _cspace.iSpriteSocket_GetMeshWrapper(*args)
    def SetTriangleIndex(*args): return _cspace.iSpriteSocket_SetTriangleIndex(*args)
    def GetTriangleIndex(*args): return _cspace.iSpriteSocket_GetTriangleIndex(*args)
    __swig_destroy__ = _cspace.delete_iSpriteSocket
    __del__ = lambda self : None;
iSpriteSocket_swigregister = _cspace.iSpriteSocket_swigregister
iSpriteSocket_swigregister(iSpriteSocket)

class iSprite3DFactoryState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSprite3DFactoryState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSprite3DFactoryState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def AddVertices(*args): return _cspace.iSprite3DFactoryState_AddVertices(*args)
    def GetVertexCount(*args): return _cspace.iSprite3DFactoryState_GetVertexCount(*args)
    def GetVertex(*args): return _cspace.iSprite3DFactoryState_GetVertex(*args)
    def SetVertex(*args): return _cspace.iSprite3DFactoryState_SetVertex(*args)
    def GetVertices(*args): return _cspace.iSprite3DFactoryState_GetVertices(*args)
    def SetVertices(*args): return _cspace.iSprite3DFactoryState_SetVertices(*args)
    def GetTexel(*args): return _cspace.iSprite3DFactoryState_GetTexel(*args)
    def SetTexel(*args): return _cspace.iSprite3DFactoryState_SetTexel(*args)
    def GetTexels(*args): return _cspace.iSprite3DFactoryState_GetTexels(*args)
    def SetTexels(*args): return _cspace.iSprite3DFactoryState_SetTexels(*args)
    def GetNormal(*args): return _cspace.iSprite3DFactoryState_GetNormal(*args)
    def SetNormal(*args): return _cspace.iSprite3DFactoryState_SetNormal(*args)
    def GetNormals(*args): return _cspace.iSprite3DFactoryState_GetNormals(*args)
    def SetNormals(*args): return _cspace.iSprite3DFactoryState_SetNormals(*args)
    def AddTriangle(*args): return _cspace.iSprite3DFactoryState_AddTriangle(*args)
    def GetTriangle(*args): return _cspace.iSprite3DFactoryState_GetTriangle(*args)
    def GetTriangles(*args): return _cspace.iSprite3DFactoryState_GetTriangles(*args)
    def GetTriangleCount(*args): return _cspace.iSprite3DFactoryState_GetTriangleCount(*args)
    def SetTriangleCount(*args): return _cspace.iSprite3DFactoryState_SetTriangleCount(*args)
    def SetTriangles(*args): return _cspace.iSprite3DFactoryState_SetTriangles(*args)
    def AddFrame(*args): return _cspace.iSprite3DFactoryState_AddFrame(*args)
    def FindFrame(*args): return _cspace.iSprite3DFactoryState_FindFrame(*args)
    def GetFrameCount(*args): return _cspace.iSprite3DFactoryState_GetFrameCount(*args)
    def GetFrame(*args): return _cspace.iSprite3DFactoryState_GetFrame(*args)
    def AddAction(*args): return _cspace.iSprite3DFactoryState_AddAction(*args)
    def FindAction(*args): return _cspace.iSprite3DFactoryState_FindAction(*args)
    def GetFirstAction(*args): return _cspace.iSprite3DFactoryState_GetFirstAction(*args)
    def GetActionCount(*args): return _cspace.iSprite3DFactoryState_GetActionCount(*args)
    def GetAction(*args): return _cspace.iSprite3DFactoryState_GetAction(*args)
    def AddSocket(*args): return _cspace.iSprite3DFactoryState_AddSocket(*args)
    def FindSocket(*args): return _cspace.iSprite3DFactoryState_FindSocket(*args)
    def GetSocketCount(*args): return _cspace.iSprite3DFactoryState_GetSocketCount(*args)
    def GetSocket(*args): return _cspace.iSprite3DFactoryState_GetSocket(*args)
    def EnableTweening(*args): return _cspace.iSprite3DFactoryState_EnableTweening(*args)
    def IsTweeningEnabled(*args): return _cspace.iSprite3DFactoryState_IsTweeningEnabled(*args)
    def SetLightingQuality(*args): return _cspace.iSprite3DFactoryState_SetLightingQuality(*args)
    def GetLightingQuality(*args): return _cspace.iSprite3DFactoryState_GetLightingQuality(*args)
    def SetLightingQualityConfig(*args): return _cspace.iSprite3DFactoryState_SetLightingQualityConfig(*args)
    def GetLightingQualityConfig(*args): return _cspace.iSprite3DFactoryState_GetLightingQualityConfig(*args)
    def SetLodLevelConfig(*args): return _cspace.iSprite3DFactoryState_SetLodLevelConfig(*args)
    def GetLodLevelConfig(*args): return _cspace.iSprite3DFactoryState_GetLodLevelConfig(*args)
    def MergeNormals(*args): return _cspace.iSprite3DFactoryState_MergeNormals(*args)
    __swig_destroy__ = _cspace.delete_iSprite3DFactoryState
    __del__ = lambda self : None;
iSprite3DFactoryState_swigregister = _cspace.iSprite3DFactoryState_swigregister
iSprite3DFactoryState_swigregister(iSprite3DFactoryState)

class iSprite3DState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSprite3DState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSprite3DState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetMixMode(*args): return _cspace.iSprite3DState_SetMixMode(*args)
    def GetMixMode(*args): return _cspace.iSprite3DState_GetMixMode(*args)
    def SetLighting(*args): return _cspace.iSprite3DState_SetLighting(*args)
    def IsLighting(*args): return _cspace.iSprite3DState_IsLighting(*args)
    def SetFrame(*args): return _cspace.iSprite3DState_SetFrame(*args)
    def GetCurFrame(*args): return _cspace.iSprite3DState_GetCurFrame(*args)
    def GetFrameCount(*args): return _cspace.iSprite3DState_GetFrameCount(*args)
    def SetAction(*args): return _cspace.iSprite3DState_SetAction(*args)
    def SetReverseAction(*args): return _cspace.iSprite3DState_SetReverseAction(*args)
    def SetSingleStepAction(*args): return _cspace.iSprite3DState_SetSingleStepAction(*args)
    def SetOverrideAction(*args): return _cspace.iSprite3DState_SetOverrideAction(*args)
    def PropagateAction(*args): return _cspace.iSprite3DState_PropagateAction(*args)
    def GetCurAction(*args): return _cspace.iSprite3DState_GetCurAction(*args)
    def GetReverseAction(*args): return _cspace.iSprite3DState_GetReverseAction(*args)
    def EnableTweening(*args): return _cspace.iSprite3DState_EnableTweening(*args)
    def IsTweeningEnabled(*args): return _cspace.iSprite3DState_IsTweeningEnabled(*args)
    def UnsetTexture(*args): return _cspace.iSprite3DState_UnsetTexture(*args)
    def GetLightingQuality(*args): return _cspace.iSprite3DState_GetLightingQuality(*args)
    def SetLocalLightingQuality(*args): return _cspace.iSprite3DState_SetLocalLightingQuality(*args)
    def SetLightingQualityConfig(*args): return _cspace.iSprite3DState_SetLightingQualityConfig(*args)
    def GetLightingQualityConfig(*args): return _cspace.iSprite3DState_GetLightingQualityConfig(*args)
    def SetLodLevelConfig(*args): return _cspace.iSprite3DState_SetLodLevelConfig(*args)
    def GetLodLevelConfig(*args): return _cspace.iSprite3DState_GetLodLevelConfig(*args)
    def IsLodEnabled(*args): return _cspace.iSprite3DState_IsLodEnabled(*args)
    def FindSocket(*args): return _cspace.iSprite3DState_FindSocket(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSprite3DState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSprite3DState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSprite3DState
    __del__ = lambda self : None;
iSprite3DState_swigregister = _cspace.iSprite3DState_swigregister
iSprite3DState_swigregister(iSprite3DState)
iSprite3DState_scfGetVersion = _cspace.iSprite3DState_scfGetVersion

class iSpriteCal3DSocket(iSpriteSocket):
    __swig_setmethods__ = {}
    for _s in [iSpriteSocket]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSpriteCal3DSocket, name, value)
    __swig_getmethods__ = {}
    for _s in [iSpriteSocket]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSpriteCal3DSocket, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetSubmeshIndex(*args): return _cspace.iSpriteCal3DSocket_SetSubmeshIndex(*args)
    def GetSubmeshIndex(*args): return _cspace.iSpriteCal3DSocket_GetSubmeshIndex(*args)
    def SetMeshIndex(*args): return _cspace.iSpriteCal3DSocket_SetMeshIndex(*args)
    def GetMeshIndex(*args): return _cspace.iSpriteCal3DSocket_GetMeshIndex(*args)
    def SetTransform(*args): return _cspace.iSpriteCal3DSocket_SetTransform(*args)
    def GetTransform(*args): return _cspace.iSpriteCal3DSocket_GetTransform(*args)
    def GetSecondaryCount(*args): return _cspace.iSpriteCal3DSocket_GetSecondaryCount(*args)
    def GetSecondaryMesh(*args): return _cspace.iSpriteCal3DSocket_GetSecondaryMesh(*args)
    def GetSecondaryTransform(*args): return _cspace.iSpriteCal3DSocket_GetSecondaryTransform(*args)
    def SetSecondaryTransform(*args): return _cspace.iSpriteCal3DSocket_SetSecondaryTransform(*args)
    def AttachSecondary(*args): return _cspace.iSpriteCal3DSocket_AttachSecondary(*args)
    def DetachSecondary(*args): return _cspace.iSpriteCal3DSocket_DetachSecondary(*args)
    def FindSecondary(*args): return _cspace.iSpriteCal3DSocket_FindSecondary(*args)
    __swig_destroy__ = _cspace.delete_iSpriteCal3DSocket
    __del__ = lambda self : None;
iSpriteCal3DSocket_swigregister = _cspace.iSpriteCal3DSocket_swigregister
iSpriteCal3DSocket_swigregister(iSpriteCal3DSocket)

class iSpriteCal3DFactoryState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSpriteCal3DFactoryState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSpriteCal3DFactoryState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Create(*args): return _cspace.iSpriteCal3DFactoryState_Create(*args)
    def ReportLastError(*args): return _cspace.iSpriteCal3DFactoryState_ReportLastError(*args)
    def SetLoadFlags(*args): return _cspace.iSpriteCal3DFactoryState_SetLoadFlags(*args)
    def SetBasePath(*args): return _cspace.iSpriteCal3DFactoryState_SetBasePath(*args)
    def LoadCoreSkeleton(*args): return _cspace.iSpriteCal3DFactoryState_LoadCoreSkeleton(*args)
    def RescaleFactory(*args): return _cspace.iSpriteCal3DFactoryState_RescaleFactory(*args)
    def LoadCoreAnimation(*args): return _cspace.iSpriteCal3DFactoryState_LoadCoreAnimation(*args)
    def LoadCoreMesh(*args): return _cspace.iSpriteCal3DFactoryState_LoadCoreMesh(*args)
    def LoadCoreMorphTarget(*args): return _cspace.iSpriteCal3DFactoryState_LoadCoreMorphTarget(*args)
    def AddMorphAnimation(*args): return _cspace.iSpriteCal3DFactoryState_AddMorphAnimation(*args)
    def AddMorphTarget(*args): return _cspace.iSpriteCal3DFactoryState_AddMorphTarget(*args)
    def AddCoreMaterial(*args): return _cspace.iSpriteCal3DFactoryState_AddCoreMaterial(*args)
    def CalculateAllBoneBoundingBoxes(*args): return _cspace.iSpriteCal3DFactoryState_CalculateAllBoneBoundingBoxes(*args)
    def BindMaterials(*args): return _cspace.iSpriteCal3DFactoryState_BindMaterials(*args)
    def GetMeshCount(*args): return _cspace.iSpriteCal3DFactoryState_GetMeshCount(*args)
    def GetMorphAnimationCount(*args): return _cspace.iSpriteCal3DFactoryState_GetMorphAnimationCount(*args)
    def GetMorphTargetCount(*args): return _cspace.iSpriteCal3DFactoryState_GetMorphTargetCount(*args)
    def GetMeshName(*args): return _cspace.iSpriteCal3DFactoryState_GetMeshName(*args)
    def FindMeshName(*args): return _cspace.iSpriteCal3DFactoryState_FindMeshName(*args)
    def GetDefaultMaterial(*args): return _cspace.iSpriteCal3DFactoryState_GetDefaultMaterial(*args)
    def GetMorphAnimationName(*args): return _cspace.iSpriteCal3DFactoryState_GetMorphAnimationName(*args)
    def FindMorphAnimationName(*args): return _cspace.iSpriteCal3DFactoryState_FindMorphAnimationName(*args)
    def IsMeshDefault(*args): return _cspace.iSpriteCal3DFactoryState_IsMeshDefault(*args)
    def AddSocket(*args): return _cspace.iSpriteCal3DFactoryState_AddSocket(*args)
    def FindSocket(*args): return _cspace.iSpriteCal3DFactoryState_FindSocket(*args)
    def GetSocketCount(*args): return _cspace.iSpriteCal3DFactoryState_GetSocketCount(*args)
    def GetSocket(*args): return _cspace.iSpriteCal3DFactoryState_GetSocket(*args)
    def GetCal3DCoreModel(*args): return _cspace.iSpriteCal3DFactoryState_GetCal3DCoreModel(*args)
    def RegisterAnimCallback(*args): return _cspace.iSpriteCal3DFactoryState_RegisterAnimCallback(*args)
    def RemoveAnimCallback(*args): return _cspace.iSpriteCal3DFactoryState_RemoveAnimCallback(*args)
    __swig_destroy__ = _cspace.delete_iSpriteCal3DFactoryState
    __del__ = lambda self : None;
iSpriteCal3DFactoryState_swigregister = _cspace.iSpriteCal3DFactoryState_swigregister
iSpriteCal3DFactoryState_swigregister(iSpriteCal3DFactoryState)

class iAnimTimeUpdateHandler(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iAnimTimeUpdateHandler, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iAnimTimeUpdateHandler, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def UpdatePosition(*args): return _cspace.iAnimTimeUpdateHandler_UpdatePosition(*args)
    __swig_destroy__ = _cspace.delete_iAnimTimeUpdateHandler
    __del__ = lambda self : None;
iAnimTimeUpdateHandler_swigregister = _cspace.iAnimTimeUpdateHandler_swigregister
iAnimTimeUpdateHandler_swigregister(iAnimTimeUpdateHandler)

class csSpriteCal3DActiveAnim(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSpriteCal3DActiveAnim, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csSpriteCal3DActiveAnim, name)
    __repr__ = _swig_repr
    __swig_setmethods__["index"] = _cspace.csSpriteCal3DActiveAnim_index_set
    __swig_getmethods__["index"] = _cspace.csSpriteCal3DActiveAnim_index_get
    if _newclass:index = property(_cspace.csSpriteCal3DActiveAnim_index_get, _cspace.csSpriteCal3DActiveAnim_index_set)
    __swig_setmethods__["weight"] = _cspace.csSpriteCal3DActiveAnim_weight_set
    __swig_getmethods__["weight"] = _cspace.csSpriteCal3DActiveAnim_weight_get
    if _newclass:weight = property(_cspace.csSpriteCal3DActiveAnim_weight_get, _cspace.csSpriteCal3DActiveAnim_weight_set)
    def __init__(self, *args): 
        this = _cspace.new_csSpriteCal3DActiveAnim(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csSpriteCal3DActiveAnim
    __del__ = lambda self : None;
csSpriteCal3DActiveAnim_swigregister = _cspace.csSpriteCal3DActiveAnim_swigregister
csSpriteCal3DActiveAnim_swigregister(csSpriteCal3DActiveAnim)

class iSpriteCal3DState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSpriteCal3DState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSpriteCal3DState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    C3D_ANIM_TYPE_NONE = _cspace.iSpriteCal3DState_C3D_ANIM_TYPE_NONE
    C3D_ANIM_TYPE_IDLE = _cspace.iSpriteCal3DState_C3D_ANIM_TYPE_IDLE
    C3D_ANIM_TYPE_TRAVEL = _cspace.iSpriteCal3DState_C3D_ANIM_TYPE_TRAVEL
    C3D_ANIM_TYPE_CYCLE = _cspace.iSpriteCal3DState_C3D_ANIM_TYPE_CYCLE
    C3D_ANIM_TYPE_STYLE_CYCLE = _cspace.iSpriteCal3DState_C3D_ANIM_TYPE_STYLE_CYCLE
    C3D_ANIM_TYPE_ACTION = _cspace.iSpriteCal3DState_C3D_ANIM_TYPE_ACTION
    def GetAnimCount(*args): return _cspace.iSpriteCal3DState_GetAnimCount(*args)
    def GetAnimName(*args): return _cspace.iSpriteCal3DState_GetAnimName(*args)
    def GetAnimType(*args): return _cspace.iSpriteCal3DState_GetAnimType(*args)
    def FindAnim(*args): return _cspace.iSpriteCal3DState_FindAnim(*args)
    def ClearAllAnims(*args): return _cspace.iSpriteCal3DState_ClearAllAnims(*args)
    def SetAnimCycle(*args): return _cspace.iSpriteCal3DState_SetAnimCycle(*args)
    def AddAnimCycle(*args): return _cspace.iSpriteCal3DState_AddAnimCycle(*args)
    def ClearAnimCycle(*args): return _cspace.iSpriteCal3DState_ClearAnimCycle(*args)
    def GetActiveAnimCount(*args): return _cspace.iSpriteCal3DState_GetActiveAnimCount(*args)
    def GetActiveAnims(*args): return _cspace.iSpriteCal3DState_GetActiveAnims(*args)
    def SetActiveAnims(*args): return _cspace.iSpriteCal3DState_SetActiveAnims(*args)
    def SetAnimAction(*args): return _cspace.iSpriteCal3DState_SetAnimAction(*args)
    def SetVelocity(*args): return _cspace.iSpriteCal3DState_SetVelocity(*args)
    def SetDefaultIdleAnim(*args): return _cspace.iSpriteCal3DState_SetDefaultIdleAnim(*args)
    def SetLOD(*args): return _cspace.iSpriteCal3DState_SetLOD(*args)
    def AttachCoreMesh(*args): return _cspace.iSpriteCal3DState_AttachCoreMesh(*args)
    def DetachCoreMesh(*args): return _cspace.iSpriteCal3DState_DetachCoreMesh(*args)
    def BlendMorphTarget(*args): return _cspace.iSpriteCal3DState_BlendMorphTarget(*args)
    def ClearMorphTarget(*args): return _cspace.iSpriteCal3DState_ClearMorphTarget(*args)
    def FindSocket(*args): return _cspace.iSpriteCal3DState_FindSocket(*args)
    def SetMaterial(*args): return _cspace.iSpriteCal3DState_SetMaterial(*args)
    def SetTimeFactor(*args): return _cspace.iSpriteCal3DState_SetTimeFactor(*args)
    def GetTimeFactor(*args): return _cspace.iSpriteCal3DState_GetTimeFactor(*args)
    def GetAnimationTime(*args): return _cspace.iSpriteCal3DState_GetAnimationTime(*args)
    def GetAnimationDuration(*args): return _cspace.iSpriteCal3DState_GetAnimationDuration(*args)
    def SetAnimationTime(*args): return _cspace.iSpriteCal3DState_SetAnimationTime(*args)
    def SetAnimTimeUpdateHandler(*args): return _cspace.iSpriteCal3DState_SetAnimTimeUpdateHandler(*args)
    def SetUserData(*args): return _cspace.iSpriteCal3DState_SetUserData(*args)
    def GetCoreMeshShaderVarContext(*args): return _cspace.iSpriteCal3DState_GetCoreMeshShaderVarContext(*args)
    def GetSkeleton(*args): return _cspace.iSpriteCal3DState_GetSkeleton(*args)
    def GetCal3DModel(*args): return _cspace.iSpriteCal3DState_GetCal3DModel(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSpriteCal3DState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSpriteCal3DState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSpriteCal3DState
    __del__ = lambda self : None;
iSpriteCal3DState_swigregister = _cspace.iSpriteCal3DState_swigregister
iSpriteCal3DState_swigregister(iSpriteCal3DState)
iSpriteCal3DState_scfGetVersion = _cspace.iSpriteCal3DState_scfGetVersion

CS_MESH_STATICPOS = _cspace.CS_MESH_STATICPOS
CS_MESH_STATICSHAPE = _cspace.CS_MESH_STATICSHAPE
CS_FACTORY_STATICSHAPE = _cspace.CS_FACTORY_STATICSHAPE
class iMeshObjectDrawCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMeshObjectDrawCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMeshObjectDrawCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def BeforeDrawing(*args): return _cspace.iMeshObjectDrawCallback_BeforeDrawing(*args)
    __swig_destroy__ = _cspace.delete_iMeshObjectDrawCallback
    __del__ = lambda self : None;
iMeshObjectDrawCallback_swigregister = _cspace.iMeshObjectDrawCallback_swigregister
iMeshObjectDrawCallback_swigregister(iMeshObjectDrawCallback)

class iMeshObject(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMeshObject, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMeshObject, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetFactory(*args): return _cspace.iMeshObject_GetFactory(*args)
    def GetFlags(*args): return _cspace.iMeshObject_GetFlags(*args)
    def Clone(*args): return _cspace.iMeshObject_Clone(*args)
    def GetRenderMeshes(*args): return _cspace.iMeshObject_GetRenderMeshes(*args)
    def SetVisibleCallback(*args): return _cspace.iMeshObject_SetVisibleCallback(*args)
    def GetVisibleCallback(*args): return _cspace.iMeshObject_GetVisibleCallback(*args)
    def NextFrame(*args): return _cspace.iMeshObject_NextFrame(*args)
    def HardTransform(*args): return _cspace.iMeshObject_HardTransform(*args)
    def SupportsHardTransform(*args): return _cspace.iMeshObject_SupportsHardTransform(*args)
    def HitBeamOutline(*args): return _cspace.iMeshObject_HitBeamOutline(*args)
    def HitBeamObject(*args): return _cspace.iMeshObject_HitBeamObject(*args)
    def SetMeshWrapper(*args): return _cspace.iMeshObject_SetMeshWrapper(*args)
    def GetMeshWrapper(*args): return _cspace.iMeshObject_GetMeshWrapper(*args)
    def GetObjectModel(*args): return _cspace.iMeshObject_GetObjectModel(*args)
    def SetColor(*args): return _cspace.iMeshObject_SetColor(*args)
    def GetColor(*args): return _cspace.iMeshObject_GetColor(*args)
    def SetMaterialWrapper(*args): return _cspace.iMeshObject_SetMaterialWrapper(*args)
    def GetMaterialWrapper(*args): return _cspace.iMeshObject_GetMaterialWrapper(*args)
    def SetMixMode(*args): return _cspace.iMeshObject_SetMixMode(*args)
    def GetMixMode(*args): return _cspace.iMeshObject_GetMixMode(*args)
    def InvalidateMaterialHandles(*args): return _cspace.iMeshObject_InvalidateMaterialHandles(*args)
    def PositionChild(*args): return _cspace.iMeshObject_PositionChild(*args)
    def BuildDecal(*args): return _cspace.iMeshObject_BuildDecal(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iMeshObject_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iMeshObject_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iMeshObject
    __del__ = lambda self : None;
iMeshObject_swigregister = _cspace.iMeshObject_swigregister
iMeshObject_swigregister(iMeshObject)
iMeshObject_scfGetVersion = _cspace.iMeshObject_scfGetVersion

class iMeshObjectFactory(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMeshObjectFactory, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMeshObjectFactory, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetFlags(*args): return _cspace.iMeshObjectFactory_GetFlags(*args)
    def NewInstance(*args): return _cspace.iMeshObjectFactory_NewInstance(*args)
    def Clone(*args): return _cspace.iMeshObjectFactory_Clone(*args)
    def HardTransform(*args): return _cspace.iMeshObjectFactory_HardTransform(*args)
    def SupportsHardTransform(*args): return _cspace.iMeshObjectFactory_SupportsHardTransform(*args)
    def SetMeshFactoryWrapper(*args): return _cspace.iMeshObjectFactory_SetMeshFactoryWrapper(*args)
    def GetMeshFactoryWrapper(*args): return _cspace.iMeshObjectFactory_GetMeshFactoryWrapper(*args)
    def GetMeshObjectType(*args): return _cspace.iMeshObjectFactory_GetMeshObjectType(*args)
    def GetObjectModel(*args): return _cspace.iMeshObjectFactory_GetObjectModel(*args)
    def SetMaterialWrapper(*args): return _cspace.iMeshObjectFactory_SetMaterialWrapper(*args)
    def GetMaterialWrapper(*args): return _cspace.iMeshObjectFactory_GetMaterialWrapper(*args)
    def SetMixMode(*args): return _cspace.iMeshObjectFactory_SetMixMode(*args)
    def GetMixMode(*args): return _cspace.iMeshObjectFactory_GetMixMode(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iMeshObjectFactory_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iMeshObjectFactory_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iMeshObjectFactory
    __del__ = lambda self : None;
iMeshObjectFactory_swigregister = _cspace.iMeshObjectFactory_swigregister
iMeshObjectFactory_swigregister(iMeshObjectFactory)
iMeshObjectFactory_scfGetVersion = _cspace.iMeshObjectFactory_scfGetVersion

class iMeshObjectType(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMeshObjectType, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMeshObjectType, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def NewFactory(*args): return _cspace.iMeshObjectType_NewFactory(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iMeshObjectType_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iMeshObjectType_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iMeshObjectType
    __del__ = lambda self : None;
iMeshObjectType_swigregister = _cspace.iMeshObjectType_swigregister
iMeshObjectType_swigregister(iMeshObjectType)
iMeshObjectType_scfGetVersion = _cspace.iMeshObjectType_scfGetVersion

class csPolygonRange(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPolygonRange, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csPolygonRange, name)
    __repr__ = _swig_repr
    __swig_setmethods__["start"] = _cspace.csPolygonRange_start_set
    __swig_getmethods__["start"] = _cspace.csPolygonRange_start_get
    if _newclass:start = property(_cspace.csPolygonRange_start_get, _cspace.csPolygonRange_start_set)
    __swig_setmethods__["end"] = _cspace.csPolygonRange_end_set
    __swig_getmethods__["end"] = _cspace.csPolygonRange_end_get
    if _newclass:end = property(_cspace.csPolygonRange_end_get, _cspace.csPolygonRange_end_set)
    def __init__(self, *args): 
        this = _cspace.new_csPolygonRange(*args)
        try: self.this.append(this)
        except: self.this = this
    def Set(*args): return _cspace.csPolygonRange_Set(*args)
    __swig_destroy__ = _cspace.delete_csPolygonRange
    __del__ = lambda self : None;
csPolygonRange_swigregister = _cspace.csPolygonRange_swigregister
csPolygonRange_swigregister(csPolygonRange)

CS_POLY_LIGHTING = _cspace.CS_POLY_LIGHTING
CS_POLY_COLLDET = _cspace.CS_POLY_COLLDET
CS_POLY_VISCULL = _cspace.CS_POLY_VISCULL
CS_POLYINDEX_LAST = _cspace.CS_POLYINDEX_LAST
CS_THING_NOCOMPRESS = _cspace.CS_THING_NOCOMPRESS
CS_THING_MOVE_NEVER = _cspace.CS_THING_MOVE_NEVER
CS_THING_MOVE_OCCASIONAL = _cspace.CS_THING_MOVE_OCCASIONAL
class iPolygonHandle(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iPolygonHandle, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iPolygonHandle, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetThingFactoryState(*args): return _cspace.iPolygonHandle_GetThingFactoryState(*args)
    def GetMeshObjectFactory(*args): return _cspace.iPolygonHandle_GetMeshObjectFactory(*args)
    def GetThingState(*args): return _cspace.iPolygonHandle_GetThingState(*args)
    def GetMeshObject(*args): return _cspace.iPolygonHandle_GetMeshObject(*args)
    def GetIndex(*args): return _cspace.iPolygonHandle_GetIndex(*args)
    __swig_destroy__ = _cspace.delete_iPolygonHandle
    __del__ = lambda self : None;
iPolygonHandle_swigregister = _cspace.iPolygonHandle_swigregister
iPolygonHandle_swigregister(iPolygonHandle)

class iThingFactoryState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iThingFactoryState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iThingFactoryState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CompressVertices(*args): return _cspace.iThingFactoryState_CompressVertices(*args)
    def GetPolygonCount(*args): return _cspace.iThingFactoryState_GetPolygonCount(*args)
    def RemovePolygon(*args): return _cspace.iThingFactoryState_RemovePolygon(*args)
    def RemovePolygons(*args): return _cspace.iThingFactoryState_RemovePolygons(*args)
    def FindPolygonByName(*args): return _cspace.iThingFactoryState_FindPolygonByName(*args)
    def AddEmptyPolygon(*args): return _cspace.iThingFactoryState_AddEmptyPolygon(*args)
    def AddTriangle(*args): return _cspace.iThingFactoryState_AddTriangle(*args)
    def AddQuad(*args): return _cspace.iThingFactoryState_AddQuad(*args)
    def AddPolygon(*args): return _cspace.iThingFactoryState_AddPolygon(*args)
    def AddOutsideBox(*args): return _cspace.iThingFactoryState_AddOutsideBox(*args)
    def AddInsideBox(*args): return _cspace.iThingFactoryState_AddInsideBox(*args)
    def SetPolygonName(*args): return _cspace.iThingFactoryState_SetPolygonName(*args)
    def GetPolygonName(*args): return _cspace.iThingFactoryState_GetPolygonName(*args)
    def CreatePolygonHandle(*args): return _cspace.iThingFactoryState_CreatePolygonHandle(*args)
    def SetPolygonMaterial(*args): return _cspace.iThingFactoryState_SetPolygonMaterial(*args)
    def GetPolygonMaterial(*args): return _cspace.iThingFactoryState_GetPolygonMaterial(*args)
    def AddPolygonVertex(*args): return _cspace.iThingFactoryState_AddPolygonVertex(*args)
    def SetPolygonVertexIndices(*args): return _cspace.iThingFactoryState_SetPolygonVertexIndices(*args)
    def GetPolygonVertexCount(*args): return _cspace.iThingFactoryState_GetPolygonVertexCount(*args)
    def GetPolygonVertex(*args): return _cspace.iThingFactoryState_GetPolygonVertex(*args)
    def GetPolygonVertexIndices(*args): return _cspace.iThingFactoryState_GetPolygonVertexIndices(*args)
    def SetPolygonTextureMapping(*args): return _cspace.iThingFactoryState_SetPolygonTextureMapping(*args)
    def GetPolygonTextureMapping(*args): return _cspace.iThingFactoryState_GetPolygonTextureMapping(*args)
    def SetPolygonTextureMappingEnabled(*args): return _cspace.iThingFactoryState_SetPolygonTextureMappingEnabled(*args)
    def IsPolygonTextureMappingEnabled(*args): return _cspace.iThingFactoryState_IsPolygonTextureMappingEnabled(*args)
    def SetPolygonFlags(*args): return _cspace.iThingFactoryState_SetPolygonFlags(*args)
    def ResetPolygonFlags(*args): return _cspace.iThingFactoryState_ResetPolygonFlags(*args)
    def GetPolygonFlags(*args): return _cspace.iThingFactoryState_GetPolygonFlags(*args)
    def GetPolygonObjectPlane(*args): return _cspace.iThingFactoryState_GetPolygonObjectPlane(*args)
    def IsPolygonTransparent(*args): return _cspace.iThingFactoryState_IsPolygonTransparent(*args)
    def PointOnPolygon(*args): return _cspace.iThingFactoryState_PointOnPolygon(*args)
    def GetVertexCount(*args): return _cspace.iThingFactoryState_GetVertexCount(*args)
    def GetVertex(*args): return _cspace.iThingFactoryState_GetVertex(*args)
    def GetVertices(*args): return _cspace.iThingFactoryState_GetVertices(*args)
    def CreateVertex(*args): return _cspace.iThingFactoryState_CreateVertex(*args)
    def SetVertex(*args): return _cspace.iThingFactoryState_SetVertex(*args)
    def DeleteVertex(*args): return _cspace.iThingFactoryState_DeleteVertex(*args)
    def DeleteVertices(*args): return _cspace.iThingFactoryState_DeleteVertices(*args)
    def SetSmoothingFlag(*args): return _cspace.iThingFactoryState_SetSmoothingFlag(*args)
    def GetSmoothingFlag(*args): return _cspace.iThingFactoryState_GetSmoothingFlag(*args)
    def GetNormals(*args): return _cspace.iThingFactoryState_GetNormals(*args)
    def GetCosinusFactor(*args): return _cspace.iThingFactoryState_GetCosinusFactor(*args)
    def SetCosinusFactor(*args): return _cspace.iThingFactoryState_SetCosinusFactor(*args)
    def AddPolygonRenderBuffer(*args): return _cspace.iThingFactoryState_AddPolygonRenderBuffer(*args)
    def GetLightmapLayout(*args): return _cspace.iThingFactoryState_GetLightmapLayout(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iThingFactoryState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iThingFactoryState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iThingFactoryState
    __del__ = lambda self : None;
iThingFactoryState_swigregister = _cspace.iThingFactoryState_swigregister
iThingFactoryState_swigregister(iThingFactoryState)
iThingFactoryState_scfGetVersion = _cspace.iThingFactoryState_scfGetVersion

class iThingState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iThingState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iThingState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetVertexW(*args): return _cspace.iThingState_GetVertexW(*args)
    def GetVerticesW(*args): return _cspace.iThingState_GetVerticesW(*args)
    def GetMovingOption(*args): return _cspace.iThingState_GetMovingOption(*args)
    def SetMovingOption(*args): return _cspace.iThingState_SetMovingOption(*args)
    def Prepare(*args): return _cspace.iThingState_Prepare(*args)
    def Unprepare(*args): return _cspace.iThingState_Unprepare(*args)
    def ReplaceMaterial(*args): return _cspace.iThingState_ReplaceMaterial(*args)
    def ClearReplacedMaterials(*args): return _cspace.iThingState_ClearReplacedMaterials(*args)
    def SetMixMode(*args): return _cspace.iThingState_SetMixMode(*args)
    def GetMixMode(*args): return _cspace.iThingState_GetMixMode(*args)
    def CreatePolygonHandle(*args): return _cspace.iThingState_CreatePolygonHandle(*args)
    def GetPolygonWorldPlane(*args): return _cspace.iThingState_GetPolygonWorldPlane(*args)
    def GetPolygonLightmap(*args): return _cspace.iThingState_GetPolygonLightmap(*args)
    def GetPolygonPDLight(*args): return _cspace.iThingState_GetPolygonPDLight(*args)
    def GetReplacedMaterial(*args): return _cspace.iThingState_GetReplacedMaterial(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iThingState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iThingState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iThingState
    __del__ = lambda self : None;
iThingState_swigregister = _cspace.iThingState_swigregister
iThingState_swigregister(iThingState)
iThingState_scfGetVersion = _cspace.iThingState_scfGetVersion

class iThingEnvironment(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iThingEnvironment, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iThingEnvironment, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Clear(*args): return _cspace.iThingEnvironment_Clear(*args)
    def GetLightmapCellSize(*args): return _cspace.iThingEnvironment_GetLightmapCellSize(*args)
    def SetLightmapCellSize(*args): return _cspace.iThingEnvironment_SetLightmapCellSize(*args)
    def GetDefaultLightmapCellSize(*args): return _cspace.iThingEnvironment_GetDefaultLightmapCellSize(*args)
    __swig_destroy__ = _cspace.delete_iThingEnvironment
    __del__ = lambda self : None;
iThingEnvironment_swigregister = _cspace.iThingEnvironment_swigregister
iThingEnvironment_swigregister(iThingEnvironment)

class csCharArrayArray(CustomAllocated):
    __swig_setmethods__ = {}
    for _s in [CustomAllocated]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csCharArrayArray, name, value)
    __swig_getmethods__ = {}
    for _s in [CustomAllocated]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csCharArrayArray, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_csCharArrayArray
    __del__ = lambda self : None;
    def __init__(self, *args): 
        this = _cspace.new_csCharArrayArray(*args)
        try: self.this.append(this)
        except: self.this = this
    def GetSize(*args): return _cspace.csCharArrayArray_GetSize(*args)
    def Get(*args): return _cspace.csCharArrayArray_Get(*args)
    def Put(*args): return _cspace.csCharArrayArray_Put(*args)
    def Push(*args): return _cspace.csCharArrayArray_Push(*args)
    def Pop(*args): return _cspace.csCharArrayArray_Pop(*args)
    def Top(*args): return _cspace.csCharArrayArray_Top(*args)
    def Insert(*args): return _cspace.csCharArrayArray_Insert(*args)
    def Contains(*args): return _cspace.csCharArrayArray_Contains(*args)
    def Truncate(*args): return _cspace.csCharArrayArray_Truncate(*args)
    def Empty(*args): return _cspace.csCharArrayArray_Empty(*args)
    def IsEmpty(*args): return _cspace.csCharArrayArray_IsEmpty(*args)
    def SetMinimalCapacity(*args): return _cspace.csCharArrayArray_SetMinimalCapacity(*args)
    def DeleteIndex(*args): return _cspace.csCharArrayArray_DeleteIndex(*args)
    def DeleteIndexFast(*args): return _cspace.csCharArrayArray_DeleteIndexFast(*args)
    def DeleteRange(*args): return _cspace.csCharArrayArray_DeleteRange(*args)
    def __eq__(*args): return _cspace.csCharArrayArray___eq__(*args)
    def __ne__(*args): return _cspace.csCharArrayArray___ne__(*args)
    def GetAllocator(*args): return _cspace.csCharArrayArray_GetAllocator(*args)
csCharArrayArray_swigregister = _cspace.csCharArrayArray_swigregister
csCharArrayArray_swigregister(csCharArrayArray)

class iTerrainObjectState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iTerrainObjectState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iTerrainObjectState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetMaterialPalette(*args): return _cspace.iTerrainObjectState_SetMaterialPalette(*args)
    def GetMaterialPalette(*args): return _cspace.iTerrainObjectState_GetMaterialPalette(*args)
    def SetMaterialMap(*args): return _cspace.iTerrainObjectState_SetMaterialMap(*args)
    def SetMaterialAlphaMaps(*args): return _cspace.iTerrainObjectState_SetMaterialAlphaMaps(*args)
    def SetLODValue(*args): return _cspace.iTerrainObjectState_SetLODValue(*args)
    def GetLODValue(*args): return _cspace.iTerrainObjectState_GetLODValue(*args)
    def SaveState(*args): return _cspace.iTerrainObjectState_SaveState(*args)
    def RestoreState(*args): return _cspace.iTerrainObjectState_RestoreState(*args)
    def CollisionDetect(*args): return _cspace.iTerrainObjectState_CollisionDetect(*args)
    def SetStaticLighting(*args): return _cspace.iTerrainObjectState_SetStaticLighting(*args)
    def GetStaticLighting(*args): return _cspace.iTerrainObjectState_GetStaticLighting(*args)
    def SetCastShadows(*args): return _cspace.iTerrainObjectState_SetCastShadows(*args)
    def GetCastShadows(*args): return _cspace.iTerrainObjectState_GetCastShadows(*args)
    def SetMaterialMapFile(*args): return _cspace.iTerrainObjectState_SetMaterialMapFile(*args)
    def GetMaterialMapFile(*args): return _cspace.iTerrainObjectState_GetMaterialMapFile(*args)
    def SetTopNeighbor(*args): return _cspace.iTerrainObjectState_SetTopNeighbor(*args)
    def SetRightNeighbor(*args): return _cspace.iTerrainObjectState_SetRightNeighbor(*args)
    def SetLeftNeighbor(*args): return _cspace.iTerrainObjectState_SetLeftNeighbor(*args)
    def SetBottomNeighbor(*args): return _cspace.iTerrainObjectState_SetBottomNeighbor(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iTerrainObjectState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iTerrainObjectState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iTerrainObjectState
    __del__ = lambda self : None;
iTerrainObjectState_swigregister = _cspace.iTerrainObjectState_swigregister
iTerrainObjectState_swigregister(iTerrainObjectState)
iTerrainObjectState_scfGetVersion = _cspace.iTerrainObjectState_scfGetVersion

class iTerrainFactoryState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iTerrainFactoryState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iTerrainFactoryState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetTerraFormer(*args): return _cspace.iTerrainFactoryState_SetTerraFormer(*args)
    def GetTerraFormer(*args): return _cspace.iTerrainFactoryState_GetTerraFormer(*args)
    def SetSamplerRegion(*args): return _cspace.iTerrainFactoryState_SetSamplerRegion(*args)
    def GetSamplerRegion(*args): return _cspace.iTerrainFactoryState_GetSamplerRegion(*args)
    def SaveState(*args): return _cspace.iTerrainFactoryState_SaveState(*args)
    def RestoreState(*args): return _cspace.iTerrainFactoryState_RestoreState(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iTerrainFactoryState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iTerrainFactoryState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iTerrainFactoryState
    __del__ = lambda self : None;
iTerrainFactoryState_swigregister = _cspace.iTerrainFactoryState_swigregister
iTerrainFactoryState_swigregister(iTerrainFactoryState)
iTerrainFactoryState_scfGetVersion = _cspace.iTerrainFactoryState_scfGetVersion

CS_PARTICLE_SORT_NONE = _cspace.CS_PARTICLE_SORT_NONE
CS_PARTICLE_SORT_DISTANCE = _cspace.CS_PARTICLE_SORT_DISTANCE
CS_PARTICLE_SORT_DOT = _cspace.CS_PARTICLE_SORT_DOT
CS_PARTICLE_CAMERAFACE = _cspace.CS_PARTICLE_CAMERAFACE
CS_PARTICLE_CAMERAFACE_APPROX = _cspace.CS_PARTICLE_CAMERAFACE_APPROX
CS_PARTICLE_ORIENT_COMMON = _cspace.CS_PARTICLE_ORIENT_COMMON
CS_PARTICLE_ORIENT_COMMON_APPROX = _cspace.CS_PARTICLE_ORIENT_COMMON_APPROX
CS_PARTICLE_ORIENT_VELOCITY = _cspace.CS_PARTICLE_ORIENT_VELOCITY
CS_PARTICLE_ORIENT_SELF = _cspace.CS_PARTICLE_ORIENT_SELF
CS_PARTICLE_ORIENT_SELF_FORWARD = _cspace.CS_PARTICLE_ORIENT_SELF_FORWARD
CS_PARTICLE_ROTATE_NONE = _cspace.CS_PARTICLE_ROTATE_NONE
CS_PARTICLE_ROTATE_TEXCOORD = _cspace.CS_PARTICLE_ROTATE_TEXCOORD
CS_PARTICLE_ROTATE_VERTICES = _cspace.CS_PARTICLE_ROTATE_VERTICES
CS_PARTICLE_INTEGRATE_NONE = _cspace.CS_PARTICLE_INTEGRATE_NONE
CS_PARTICLE_INTEGRATE_LINEAR = _cspace.CS_PARTICLE_INTEGRATE_LINEAR
CS_PARTICLE_INTEGRATE_BOTH = _cspace.CS_PARTICLE_INTEGRATE_BOTH
CS_PARTICLE_LOCAL_MODE = _cspace.CS_PARTICLE_LOCAL_MODE
CS_PARTICLE_LOCAL_EMITTER = _cspace.CS_PARTICLE_LOCAL_EMITTER
CS_PARTICLE_WORLD_MODE = _cspace.CS_PARTICLE_WORLD_MODE
class csParticle(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csParticle, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csParticle, name)
    __repr__ = _swig_repr
    __swig_setmethods__["position"] = _cspace.csParticle_position_set
    __swig_getmethods__["position"] = _cspace.csParticle_position_get
    if _newclass:position = property(_cspace.csParticle_position_get, _cspace.csParticle_position_set)
    __swig_setmethods__["mass"] = _cspace.csParticle_mass_set
    __swig_getmethods__["mass"] = _cspace.csParticle_mass_get
    if _newclass:mass = property(_cspace.csParticle_mass_get, _cspace.csParticle_mass_set)
    __swig_setmethods__["orientation"] = _cspace.csParticle_orientation_set
    __swig_getmethods__["orientation"] = _cspace.csParticle_orientation_get
    if _newclass:orientation = property(_cspace.csParticle_orientation_get, _cspace.csParticle_orientation_set)
    __swig_setmethods__["linearVelocity"] = _cspace.csParticle_linearVelocity_set
    __swig_getmethods__["linearVelocity"] = _cspace.csParticle_linearVelocity_get
    if _newclass:linearVelocity = property(_cspace.csParticle_linearVelocity_get, _cspace.csParticle_linearVelocity_set)
    __swig_setmethods__["timeToLive"] = _cspace.csParticle_timeToLive_set
    __swig_getmethods__["timeToLive"] = _cspace.csParticle_timeToLive_get
    if _newclass:timeToLive = property(_cspace.csParticle_timeToLive_get, _cspace.csParticle_timeToLive_set)
    __swig_setmethods__["angularVelocity"] = _cspace.csParticle_angularVelocity_set
    __swig_getmethods__["angularVelocity"] = _cspace.csParticle_angularVelocity_get
    if _newclass:angularVelocity = property(_cspace.csParticle_angularVelocity_get, _cspace.csParticle_angularVelocity_set)
    __swig_setmethods__["pad"] = _cspace.csParticle_pad_set
    __swig_getmethods__["pad"] = _cspace.csParticle_pad_get
    if _newclass:pad = property(_cspace.csParticle_pad_get, _cspace.csParticle_pad_set)
    def __init__(self, *args): 
        this = _cspace.new_csParticle(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csParticle
    __del__ = lambda self : None;
csParticle_swigregister = _cspace.csParticle_swigregister
csParticle_swigregister(csParticle)

class csParticleAux(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csParticleAux, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csParticleAux, name)
    __repr__ = _swig_repr
    __swig_setmethods__["color"] = _cspace.csParticleAux_color_set
    __swig_getmethods__["color"] = _cspace.csParticleAux_color_get
    if _newclass:color = property(_cspace.csParticleAux_color_get, _cspace.csParticleAux_color_set)
    __swig_setmethods__["particleSize"] = _cspace.csParticleAux_particleSize_set
    __swig_getmethods__["particleSize"] = _cspace.csParticleAux_particleSize_get
    if _newclass:particleSize = property(_cspace.csParticleAux_particleSize_get, _cspace.csParticleAux_particleSize_set)
    __swig_setmethods__["pad"] = _cspace.csParticleAux_pad_set
    __swig_getmethods__["pad"] = _cspace.csParticleAux_pad_get
    if _newclass:pad = property(_cspace.csParticleAux_pad_get, _cspace.csParticleAux_pad_set)
    def __init__(self, *args): 
        this = _cspace.new_csParticleAux(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csParticleAux
    __del__ = lambda self : None;
csParticleAux_swigregister = _cspace.csParticleAux_swigregister
csParticleAux_swigregister(csParticleAux)

class csParticleBuffer(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csParticleBuffer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csParticleBuffer, name)
    __repr__ = _swig_repr
    __swig_setmethods__["particleData"] = _cspace.csParticleBuffer_particleData_set
    __swig_getmethods__["particleData"] = _cspace.csParticleBuffer_particleData_get
    if _newclass:particleData = property(_cspace.csParticleBuffer_particleData_get, _cspace.csParticleBuffer_particleData_set)
    __swig_setmethods__["particleAuxData"] = _cspace.csParticleBuffer_particleAuxData_set
    __swig_getmethods__["particleAuxData"] = _cspace.csParticleBuffer_particleAuxData_get
    if _newclass:particleAuxData = property(_cspace.csParticleBuffer_particleAuxData_get, _cspace.csParticleBuffer_particleAuxData_set)
    __swig_setmethods__["particleCount"] = _cspace.csParticleBuffer_particleCount_set
    __swig_getmethods__["particleCount"] = _cspace.csParticleBuffer_particleCount_get
    if _newclass:particleCount = property(_cspace.csParticleBuffer_particleCount_get, _cspace.csParticleBuffer_particleCount_set)
    def __init__(self, *args): 
        this = _cspace.new_csParticleBuffer(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csParticleBuffer
    __del__ = lambda self : None;
csParticleBuffer_swigregister = _cspace.csParticleBuffer_swigregister
csParticleBuffer_swigregister(csParticleBuffer)

class iParticleEmitter(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleEmitter, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleEmitter, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetEnabled(*args): return _cspace.iParticleEmitter_SetEnabled(*args)
    def GetEnabled(*args): return _cspace.iParticleEmitter_GetEnabled(*args)
    def SetStartTime(*args): return _cspace.iParticleEmitter_SetStartTime(*args)
    def GetStartTime(*args): return _cspace.iParticleEmitter_GetStartTime(*args)
    def SetDuration(*args): return _cspace.iParticleEmitter_SetDuration(*args)
    def GetDuration(*args): return _cspace.iParticleEmitter_GetDuration(*args)
    def SetEmissionRate(*args): return _cspace.iParticleEmitter_SetEmissionRate(*args)
    def GetEmissionRate(*args): return _cspace.iParticleEmitter_GetEmissionRate(*args)
    def SetInitialTTL(*args): return _cspace.iParticleEmitter_SetInitialTTL(*args)
    def GetInitialTTL(*args): return _cspace.iParticleEmitter_GetInitialTTL(*args)
    def SetInitialMass(*args): return _cspace.iParticleEmitter_SetInitialMass(*args)
    def GetInitialMass(*args): return _cspace.iParticleEmitter_GetInitialMass(*args)
    def Clone(*args): return _cspace.iParticleEmitter_Clone(*args)
    def ParticlesToEmit(*args): return _cspace.iParticleEmitter_ParticlesToEmit(*args)
    def EmitParticles(*args): return _cspace.iParticleEmitter_EmitParticles(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleEmitter_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleEmitter_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleEmitter
    __del__ = lambda self : None;
iParticleEmitter_swigregister = _cspace.iParticleEmitter_swigregister
iParticleEmitter_swigregister(iParticleEmitter)
iParticleEmitter_scfGetVersion = _cspace.iParticleEmitter_scfGetVersion

class iParticleEffector(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleEffector, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleEffector, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Clone(*args): return _cspace.iParticleEffector_Clone(*args)
    def EffectParticles(*args): return _cspace.iParticleEffector_EffectParticles(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleEffector_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleEffector_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleEffector
    __del__ = lambda self : None;
iParticleEffector_swigregister = _cspace.iParticleEffector_swigregister
iParticleEffector_swigregister(iParticleEffector)
iParticleEffector_scfGetVersion = _cspace.iParticleEffector_scfGetVersion

class iParticleSystemBase(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleSystemBase, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleSystemBase, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetParticleRenderOrientation(*args): return _cspace.iParticleSystemBase_SetParticleRenderOrientation(*args)
    def GetParticleRenderOrientation(*args): return _cspace.iParticleSystemBase_GetParticleRenderOrientation(*args)
    def SetRotationMode(*args): return _cspace.iParticleSystemBase_SetRotationMode(*args)
    def GetRotationMode(*args): return _cspace.iParticleSystemBase_GetRotationMode(*args)
    def SetSortMode(*args): return _cspace.iParticleSystemBase_SetSortMode(*args)
    def GetSortMode(*args): return _cspace.iParticleSystemBase_GetSortMode(*args)
    def SetIntegrationMode(*args): return _cspace.iParticleSystemBase_SetIntegrationMode(*args)
    def GetIntegrationMode(*args): return _cspace.iParticleSystemBase_GetIntegrationMode(*args)
    def SetCommonDirection(*args): return _cspace.iParticleSystemBase_SetCommonDirection(*args)
    def GetCommonDirection(*args): return _cspace.iParticleSystemBase_GetCommonDirection(*args)
    def SetTransformMode(*args): return _cspace.iParticleSystemBase_SetTransformMode(*args)
    def GetTransformMode(*args): return _cspace.iParticleSystemBase_GetTransformMode(*args)
    def SetUseIndividualSize(*args): return _cspace.iParticleSystemBase_SetUseIndividualSize(*args)
    def GetUseIndividualSize(*args): return _cspace.iParticleSystemBase_GetUseIndividualSize(*args)
    def SetParticleSize(*args): return _cspace.iParticleSystemBase_SetParticleSize(*args)
    def GetParticleSize(*args): return _cspace.iParticleSystemBase_GetParticleSize(*args)
    def SetMinBoundingBox(*args): return _cspace.iParticleSystemBase_SetMinBoundingBox(*args)
    def GetMinBoundingBox(*args): return _cspace.iParticleSystemBase_GetMinBoundingBox(*args)
    def AddEmitter(*args): return _cspace.iParticleSystemBase_AddEmitter(*args)
    def GetEmitter(*args): return _cspace.iParticleSystemBase_GetEmitter(*args)
    def RemoveEmitter(*args): return _cspace.iParticleSystemBase_RemoveEmitter(*args)
    def GetEmitterCount(*args): return _cspace.iParticleSystemBase_GetEmitterCount(*args)
    def AddEffector(*args): return _cspace.iParticleSystemBase_AddEffector(*args)
    def GetEffector(*args): return _cspace.iParticleSystemBase_GetEffector(*args)
    def RemoveEffector(*args): return _cspace.iParticleSystemBase_RemoveEffector(*args)
    def GetEffectorCount(*args): return _cspace.iParticleSystemBase_GetEffectorCount(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleSystemBase_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleSystemBase_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleSystemBase
    __del__ = lambda self : None;
iParticleSystemBase_swigregister = _cspace.iParticleSystemBase_swigregister
iParticleSystemBase_swigregister(iParticleSystemBase)
iParticleSystemBase_scfGetVersion = _cspace.iParticleSystemBase_scfGetVersion

class iParticleSystemFactory(iParticleSystemBase):
    __swig_setmethods__ = {}
    for _s in [iParticleSystemBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleSystemFactory, name, value)
    __swig_getmethods__ = {}
    for _s in [iParticleSystemBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleSystemFactory, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetDeepCreation(*args): return _cspace.iParticleSystemFactory_SetDeepCreation(*args)
    def GetDeepCreation(*args): return _cspace.iParticleSystemFactory_GetDeepCreation(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleSystemFactory_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleSystemFactory_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleSystemFactory
    __del__ = lambda self : None;
iParticleSystemFactory_swigregister = _cspace.iParticleSystemFactory_swigregister
iParticleSystemFactory_swigregister(iParticleSystemFactory)
iParticleSystemFactory_scfGetVersion = _cspace.iParticleSystemFactory_scfGetVersion

class iParticleSystem(iParticleSystemBase):
    __swig_setmethods__ = {}
    for _s in [iParticleSystemBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleSystem, name, value)
    __swig_getmethods__ = {}
    for _s in [iParticleSystemBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleSystem, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetParticleCount(*args): return _cspace.iParticleSystem_GetParticleCount(*args)
    def GetParticle(*args): return _cspace.iParticleSystem_GetParticle(*args)
    def GetParticleAux(*args): return _cspace.iParticleSystem_GetParticleAux(*args)
    def LockForExternalControl(*args): return _cspace.iParticleSystem_LockForExternalControl(*args)
    def Advance(*args): return _cspace.iParticleSystem_Advance(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleSystem_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleSystem_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleSystem
    __del__ = lambda self : None;
iParticleSystem_swigregister = _cspace.iParticleSystem_swigregister
iParticleSystem_swigregister(iParticleSystem)
iParticleSystem_scfGetVersion = _cspace.iParticleSystem_scfGetVersion

CS_PARTICLE_BUILTIN_CENTER = _cspace.CS_PARTICLE_BUILTIN_CENTER
CS_PARTICLE_BUILTIN_VOLUME = _cspace.CS_PARTICLE_BUILTIN_VOLUME
CS_PARTICLE_BUILTIN_SURFACE = _cspace.CS_PARTICLE_BUILTIN_SURFACE
class iParticleBuiltinEmitterBase(iParticleEmitter):
    __swig_setmethods__ = {}
    for _s in [iParticleEmitter]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleBuiltinEmitterBase, name, value)
    __swig_getmethods__ = {}
    for _s in [iParticleEmitter]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleBuiltinEmitterBase, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetPosition(*args): return _cspace.iParticleBuiltinEmitterBase_SetPosition(*args)
    def GetPosition(*args): return _cspace.iParticleBuiltinEmitterBase_GetPosition(*args)
    def SetParticlePlacement(*args): return _cspace.iParticleBuiltinEmitterBase_SetParticlePlacement(*args)
    def GetParticlePlacement(*args): return _cspace.iParticleBuiltinEmitterBase_GetParticlePlacement(*args)
    def SetUniformVelocity(*args): return _cspace.iParticleBuiltinEmitterBase_SetUniformVelocity(*args)
    def GetUniformVelocity(*args): return _cspace.iParticleBuiltinEmitterBase_GetUniformVelocity(*args)
    def SetInitialVelocity(*args): return _cspace.iParticleBuiltinEmitterBase_SetInitialVelocity(*args)
    def GetInitialVelocity(*args): return _cspace.iParticleBuiltinEmitterBase_GetInitialVelocity(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleBuiltinEmitterBase_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleBuiltinEmitterBase_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleBuiltinEmitterBase
    __del__ = lambda self : None;
iParticleBuiltinEmitterBase_swigregister = _cspace.iParticleBuiltinEmitterBase_swigregister
iParticleBuiltinEmitterBase_swigregister(iParticleBuiltinEmitterBase)
iParticleBuiltinEmitterBase_scfGetVersion = _cspace.iParticleBuiltinEmitterBase_scfGetVersion

class iParticleBuiltinEmitterSphere(iParticleBuiltinEmitterBase):
    __swig_setmethods__ = {}
    for _s in [iParticleBuiltinEmitterBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleBuiltinEmitterSphere, name, value)
    __swig_getmethods__ = {}
    for _s in [iParticleBuiltinEmitterBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleBuiltinEmitterSphere, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetRadius(*args): return _cspace.iParticleBuiltinEmitterSphere_SetRadius(*args)
    def GetRadius(*args): return _cspace.iParticleBuiltinEmitterSphere_GetRadius(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleBuiltinEmitterSphere_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleBuiltinEmitterSphere_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleBuiltinEmitterSphere
    __del__ = lambda self : None;
iParticleBuiltinEmitterSphere_swigregister = _cspace.iParticleBuiltinEmitterSphere_swigregister
iParticleBuiltinEmitterSphere_swigregister(iParticleBuiltinEmitterSphere)
iParticleBuiltinEmitterSphere_scfGetVersion = _cspace.iParticleBuiltinEmitterSphere_scfGetVersion

class iParticleBuiltinEmitterCone(iParticleBuiltinEmitterBase):
    __swig_setmethods__ = {}
    for _s in [iParticleBuiltinEmitterBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleBuiltinEmitterCone, name, value)
    __swig_getmethods__ = {}
    for _s in [iParticleBuiltinEmitterBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleBuiltinEmitterCone, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetExtent(*args): return _cspace.iParticleBuiltinEmitterCone_SetExtent(*args)
    def GetExtent(*args): return _cspace.iParticleBuiltinEmitterCone_GetExtent(*args)
    def SetConeAngle(*args): return _cspace.iParticleBuiltinEmitterCone_SetConeAngle(*args)
    def GetConeAngle(*args): return _cspace.iParticleBuiltinEmitterCone_GetConeAngle(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleBuiltinEmitterCone_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleBuiltinEmitterCone_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleBuiltinEmitterCone
    __del__ = lambda self : None;
iParticleBuiltinEmitterCone_swigregister = _cspace.iParticleBuiltinEmitterCone_swigregister
iParticleBuiltinEmitterCone_swigregister(iParticleBuiltinEmitterCone)
iParticleBuiltinEmitterCone_scfGetVersion = _cspace.iParticleBuiltinEmitterCone_scfGetVersion

class iParticleBuiltinEmitterBox(iParticleBuiltinEmitterBase):
    __swig_setmethods__ = {}
    for _s in [iParticleBuiltinEmitterBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleBuiltinEmitterBox, name, value)
    __swig_getmethods__ = {}
    for _s in [iParticleBuiltinEmitterBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleBuiltinEmitterBox, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetBox(*args): return _cspace.iParticleBuiltinEmitterBox_SetBox(*args)
    def GetBox(*args): return _cspace.iParticleBuiltinEmitterBox_GetBox(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleBuiltinEmitterBox_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleBuiltinEmitterBox_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleBuiltinEmitterBox
    __del__ = lambda self : None;
iParticleBuiltinEmitterBox_swigregister = _cspace.iParticleBuiltinEmitterBox_swigregister
iParticleBuiltinEmitterBox_swigregister(iParticleBuiltinEmitterBox)
iParticleBuiltinEmitterBox_scfGetVersion = _cspace.iParticleBuiltinEmitterBox_scfGetVersion

class iParticleBuiltinEmitterCylinder(iParticleBuiltinEmitterBase):
    __swig_setmethods__ = {}
    for _s in [iParticleBuiltinEmitterBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleBuiltinEmitterCylinder, name, value)
    __swig_getmethods__ = {}
    for _s in [iParticleBuiltinEmitterBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleBuiltinEmitterCylinder, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetRadius(*args): return _cspace.iParticleBuiltinEmitterCylinder_SetRadius(*args)
    def GetRadius(*args): return _cspace.iParticleBuiltinEmitterCylinder_GetRadius(*args)
    def SetExtent(*args): return _cspace.iParticleBuiltinEmitterCylinder_SetExtent(*args)
    def GetExtent(*args): return _cspace.iParticleBuiltinEmitterCylinder_GetExtent(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleBuiltinEmitterCylinder_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleBuiltinEmitterCylinder_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleBuiltinEmitterCylinder
    __del__ = lambda self : None;
iParticleBuiltinEmitterCylinder_swigregister = _cspace.iParticleBuiltinEmitterCylinder_swigregister
iParticleBuiltinEmitterCylinder_swigregister(iParticleBuiltinEmitterCylinder)
iParticleBuiltinEmitterCylinder_scfGetVersion = _cspace.iParticleBuiltinEmitterCylinder_scfGetVersion

class iParticleBuiltinEmitterFactory(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleBuiltinEmitterFactory, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleBuiltinEmitterFactory, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CreateSphere(*args): return _cspace.iParticleBuiltinEmitterFactory_CreateSphere(*args)
    def CreateCone(*args): return _cspace.iParticleBuiltinEmitterFactory_CreateCone(*args)
    def CreateBox(*args): return _cspace.iParticleBuiltinEmitterFactory_CreateBox(*args)
    def CreateCylinder(*args): return _cspace.iParticleBuiltinEmitterFactory_CreateCylinder(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleBuiltinEmitterFactory_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleBuiltinEmitterFactory_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleBuiltinEmitterFactory
    __del__ = lambda self : None;
iParticleBuiltinEmitterFactory_swigregister = _cspace.iParticleBuiltinEmitterFactory_swigregister
iParticleBuiltinEmitterFactory_swigregister(iParticleBuiltinEmitterFactory)
iParticleBuiltinEmitterFactory_scfGetVersion = _cspace.iParticleBuiltinEmitterFactory_scfGetVersion

class iParticleBuiltinEffectorForce(iParticleEffector):
    __swig_setmethods__ = {}
    for _s in [iParticleEffector]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleBuiltinEffectorForce, name, value)
    __swig_getmethods__ = {}
    for _s in [iParticleEffector]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleBuiltinEffectorForce, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetAcceleration(*args): return _cspace.iParticleBuiltinEffectorForce_SetAcceleration(*args)
    def GetAcceleration(*args): return _cspace.iParticleBuiltinEffectorForce_GetAcceleration(*args)
    def SetForce(*args): return _cspace.iParticleBuiltinEffectorForce_SetForce(*args)
    def GetForce(*args): return _cspace.iParticleBuiltinEffectorForce_GetForce(*args)
    def SetRandomAcceleration(*args): return _cspace.iParticleBuiltinEffectorForce_SetRandomAcceleration(*args)
    def GetRandomAcceleration(*args): return _cspace.iParticleBuiltinEffectorForce_GetRandomAcceleration(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleBuiltinEffectorForce_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleBuiltinEffectorForce_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleBuiltinEffectorForce
    __del__ = lambda self : None;
iParticleBuiltinEffectorForce_swigregister = _cspace.iParticleBuiltinEffectorForce_swigregister
iParticleBuiltinEffectorForce_swigregister(iParticleBuiltinEffectorForce)
iParticleBuiltinEffectorForce_scfGetVersion = _cspace.iParticleBuiltinEffectorForce_scfGetVersion

class iParticleBuiltinEffectorLinColor(iParticleEffector):
    __swig_setmethods__ = {}
    for _s in [iParticleEffector]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleBuiltinEffectorLinColor, name, value)
    __swig_getmethods__ = {}
    for _s in [iParticleEffector]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleBuiltinEffectorLinColor, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def AddColor(*args): return _cspace.iParticleBuiltinEffectorLinColor_AddColor(*args)
    def SetColor(*args): return _cspace.iParticleBuiltinEffectorLinColor_SetColor(*args)
    def GetColor(*args): return _cspace.iParticleBuiltinEffectorLinColor_GetColor(*args)
    def GetColorCount(*args): return _cspace.iParticleBuiltinEffectorLinColor_GetColorCount(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleBuiltinEffectorLinColor_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleBuiltinEffectorLinColor_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleBuiltinEffectorLinColor
    __del__ = lambda self : None;
iParticleBuiltinEffectorLinColor_swigregister = _cspace.iParticleBuiltinEffectorLinColor_swigregister
iParticleBuiltinEffectorLinColor_swigregister(iParticleBuiltinEffectorLinColor)
iParticleBuiltinEffectorLinColor_scfGetVersion = _cspace.iParticleBuiltinEffectorLinColor_scfGetVersion

CS_PARTICLE_BUILTIN_SPIRAL = _cspace.CS_PARTICLE_BUILTIN_SPIRAL
CS_PARTICLE_BUILTIN_RADIALPOINT = _cspace.CS_PARTICLE_BUILTIN_RADIALPOINT
class iParticleBuiltinEffectorVelocityField(iParticleEffector):
    __swig_setmethods__ = {}
    for _s in [iParticleEffector]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleBuiltinEffectorVelocityField, name, value)
    __swig_getmethods__ = {}
    for _s in [iParticleEffector]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleBuiltinEffectorVelocityField, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetType(*args): return _cspace.iParticleBuiltinEffectorVelocityField_SetType(*args)
    def GetType(*args): return _cspace.iParticleBuiltinEffectorVelocityField_GetType(*args)
    def SetFParameter(*args): return _cspace.iParticleBuiltinEffectorVelocityField_SetFParameter(*args)
    def GetFParameter(*args): return _cspace.iParticleBuiltinEffectorVelocityField_GetFParameter(*args)
    def GetFParameterCount(*args): return _cspace.iParticleBuiltinEffectorVelocityField_GetFParameterCount(*args)
    def SetVParameter(*args): return _cspace.iParticleBuiltinEffectorVelocityField_SetVParameter(*args)
    def GetVParameter(*args): return _cspace.iParticleBuiltinEffectorVelocityField_GetVParameter(*args)
    def GetVParameterCount(*args): return _cspace.iParticleBuiltinEffectorVelocityField_GetVParameterCount(*args)
    __swig_destroy__ = _cspace.delete_iParticleBuiltinEffectorVelocityField
    __del__ = lambda self : None;
iParticleBuiltinEffectorVelocityField_swigregister = _cspace.iParticleBuiltinEffectorVelocityField_swigregister
iParticleBuiltinEffectorVelocityField_swigregister(iParticleBuiltinEffectorVelocityField)

class iParticleBuiltinEffectorFactory(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParticleBuiltinEffectorFactory, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParticleBuiltinEffectorFactory, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CreateForce(*args): return _cspace.iParticleBuiltinEffectorFactory_CreateForce(*args)
    def CreateLinColor(*args): return _cspace.iParticleBuiltinEffectorFactory_CreateLinColor(*args)
    def CreateVelocityField(*args): return _cspace.iParticleBuiltinEffectorFactory_CreateVelocityField(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iParticleBuiltinEffectorFactory_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iParticleBuiltinEffectorFactory_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iParticleBuiltinEffectorFactory
    __del__ = lambda self : None;
iParticleBuiltinEffectorFactory_swigregister = _cspace.iParticleBuiltinEffectorFactory_swigregister
iParticleBuiltinEffectorFactory_swigregister(iParticleBuiltinEffectorFactory)
iParticleBuiltinEffectorFactory_scfGetVersion = _cspace.iParticleBuiltinEffectorFactory_scfGetVersion

class iLoaderStatus(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iLoaderStatus, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iLoaderStatus, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def IsReady(*args): return _cspace.iLoaderStatus_IsReady(*args)
    def IsError(*args): return _cspace.iLoaderStatus_IsError(*args)
    __swig_destroy__ = _cspace.delete_iLoaderStatus
    __del__ = lambda self : None;
iLoaderStatus_swigregister = _cspace.iLoaderStatus_swigregister
iLoaderStatus_swigregister(iLoaderStatus)

class iMissingLoaderData(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMissingLoaderData, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMissingLoaderData, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def MissingMaterial(*args): return _cspace.iMissingLoaderData_MissingMaterial(*args)
    def MissingTexture(*args): return _cspace.iMissingLoaderData_MissingTexture(*args)
    def MissingShader(*args): return _cspace.iMissingLoaderData_MissingShader(*args)
    def MissingFactory(*args): return _cspace.iMissingLoaderData_MissingFactory(*args)
    def MissingMesh(*args): return _cspace.iMissingLoaderData_MissingMesh(*args)
    def MissingSector(*args): return _cspace.iMissingLoaderData_MissingSector(*args)
    def MissingLight(*args): return _cspace.iMissingLoaderData_MissingLight(*args)
    __swig_destroy__ = _cspace.delete_iMissingLoaderData
    __del__ = lambda self : None;
iMissingLoaderData_swigregister = _cspace.iMissingLoaderData_swigregister
iMissingLoaderData_swigregister(iMissingLoaderData)

class iLoader(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iLoader, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iLoader, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def LoadImage(*args): return _cspace.iLoader_LoadImage(*args)
    def LoadTexture(*args): return _cspace.iLoader_LoadTexture(*args)
    def LoadSoundSysData(*args): return _cspace.iLoader_LoadSoundSysData(*args)
    def LoadSoundStream(*args): return _cspace.iLoader_LoadSoundStream(*args)
    def LoadSoundWrapper(*args): return _cspace.iLoader_LoadSoundWrapper(*args)
    def ThreadedLoadMapFile(*args): return _cspace.iLoader_ThreadedLoadMapFile(*args)
    def LoadMapFile(*args): return _cspace.iLoader_LoadMapFile(*args)
    def LoadMap(*args): return _cspace.iLoader_LoadMap(*args)
    def LoadLibraryFile(*args): return _cspace.iLoader_LoadLibraryFile(*args)
    def LoadLibrary(*args): return _cspace.iLoader_LoadLibrary(*args)
    def LoadMeshObjectFactory(*args): return _cspace.iLoader_LoadMeshObjectFactory(*args)
    def LoadMeshObject(*args): return _cspace.iLoader_LoadMeshObject(*args)
    def Load(*args): return _cspace.iLoader_Load(*args)
    def LoadShader(*args): return _cspace.iLoader_LoadShader(*args)
    def SetAutoRegions(*args): return _cspace.iLoader_SetAutoRegions(*args)
    def GetAutoRegions(*args): return _cspace.iLoader_GetAutoRegions(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iLoader_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iLoader_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iLoader
    __del__ = lambda self : None;
iLoader_swigregister = _cspace.iLoader_swigregister
iLoader_swigregister(iLoader)
iLoader_scfGetVersion = _cspace.iLoader_scfGetVersion

class iLoaderPlugin(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iLoaderPlugin, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iLoaderPlugin, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Parse(*args): return _cspace.iLoaderPlugin_Parse(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iLoaderPlugin_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iLoaderPlugin_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iLoaderPlugin
    __del__ = lambda self : None;
iLoaderPlugin_swigregister = _cspace.iLoaderPlugin_swigregister
iLoaderPlugin_swigregister(iLoaderPlugin)
iLoaderPlugin_scfGetVersion = _cspace.iLoaderPlugin_scfGetVersion

class iBinaryLoaderPlugin(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iBinaryLoaderPlugin, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iBinaryLoaderPlugin, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Parse(*args): return _cspace.iBinaryLoaderPlugin_Parse(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iBinaryLoaderPlugin_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iBinaryLoaderPlugin_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iBinaryLoaderPlugin
    __del__ = lambda self : None;
iBinaryLoaderPlugin_swigregister = _cspace.iBinaryLoaderPlugin_swigregister
iBinaryLoaderPlugin_swigregister(iBinaryLoaderPlugin)
iBinaryLoaderPlugin_scfGetVersion = _cspace.iBinaryLoaderPlugin_scfGetVersion

class iSaver(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSaver, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSaver, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SaveMapFile(*args): return _cspace.iSaver_SaveMapFile(*args)
    def SaveAllRegions(*args): return _cspace.iSaver_SaveAllRegions(*args)
    def SaveRegionFile(*args): return _cspace.iSaver_SaveRegionFile(*args)
    def SaveRegion(*args): return _cspace.iSaver_SaveRegion(*args)
    def SavePortal(*args): return _cspace.iSaver_SavePortal(*args)
    __swig_destroy__ = _cspace.delete_iSaver
    __del__ = lambda self : None;
iSaver_swigregister = _cspace.iSaver_swigregister
iSaver_swigregister(iSaver)

CS_SNDSYS_DATA_UNKNOWN_SIZE = _cspace.CS_SNDSYS_DATA_UNKNOWN_SIZE
class iSndSysData(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysData, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysData, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetFormat(*args): return _cspace.iSndSysData_GetFormat(*args)
    def GetFrameCount(*args): return _cspace.iSndSysData_GetFrameCount(*args)
    def GetDataSize(*args): return _cspace.iSndSysData_GetDataSize(*args)
    def CreateStream(*args): return _cspace.iSndSysData_CreateStream(*args)
    def SetDescription(*args): return _cspace.iSndSysData_SetDescription(*args)
    def GetDescription(*args): return _cspace.iSndSysData_GetDescription(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSndSysData_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSndSysData_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSndSysData
    __del__ = lambda self : None;
iSndSysData_swigregister = _cspace.iSndSysData_swigregister
iSndSysData_swigregister(iSndSysData)
iSndSysData_scfGetVersion = _cspace.iSndSysData_scfGetVersion

class iSndSysSoftwareFilter3DProperties(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysSoftwareFilter3DProperties, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysSoftwareFilter3DProperties, name)
    __repr__ = _swig_repr
    __swig_setmethods__["clean_buffer"] = _cspace.iSndSysSoftwareFilter3DProperties_clean_buffer_set
    __swig_getmethods__["clean_buffer"] = _cspace.iSndSysSoftwareFilter3DProperties_clean_buffer_get
    if _newclass:clean_buffer = property(_cspace.iSndSysSoftwareFilter3DProperties_clean_buffer_get, _cspace.iSndSysSoftwareFilter3DProperties_clean_buffer_set)
    __swig_setmethods__["work_buffer"] = _cspace.iSndSysSoftwareFilter3DProperties_work_buffer_set
    __swig_getmethods__["work_buffer"] = _cspace.iSndSysSoftwareFilter3DProperties_work_buffer_get
    if _newclass:work_buffer = property(_cspace.iSndSysSoftwareFilter3DProperties_work_buffer_get, _cspace.iSndSysSoftwareFilter3DProperties_work_buffer_set)
    __swig_setmethods__["buffer_samples"] = _cspace.iSndSysSoftwareFilter3DProperties_buffer_samples_set
    __swig_getmethods__["buffer_samples"] = _cspace.iSndSysSoftwareFilter3DProperties_buffer_samples_get
    if _newclass:buffer_samples = property(_cspace.iSndSysSoftwareFilter3DProperties_buffer_samples_get, _cspace.iSndSysSoftwareFilter3DProperties_buffer_samples_set)
    __swig_setmethods__["source_parameters"] = _cspace.iSndSysSoftwareFilter3DProperties_source_parameters_set
    __swig_getmethods__["source_parameters"] = _cspace.iSndSysSoftwareFilter3DProperties_source_parameters_get
    if _newclass:source_parameters = property(_cspace.iSndSysSoftwareFilter3DProperties_source_parameters_get, _cspace.iSndSysSoftwareFilter3DProperties_source_parameters_set)
    __swig_setmethods__["listener_parameters"] = _cspace.iSndSysSoftwareFilter3DProperties_listener_parameters_set
    __swig_getmethods__["listener_parameters"] = _cspace.iSndSysSoftwareFilter3DProperties_listener_parameters_get
    if _newclass:listener_parameters = property(_cspace.iSndSysSoftwareFilter3DProperties_listener_parameters_get, _cspace.iSndSysSoftwareFilter3DProperties_listener_parameters_set)
    __swig_setmethods__["sound_format"] = _cspace.iSndSysSoftwareFilter3DProperties_sound_format_set
    __swig_getmethods__["sound_format"] = _cspace.iSndSysSoftwareFilter3DProperties_sound_format_get
    if _newclass:sound_format = property(_cspace.iSndSysSoftwareFilter3DProperties_sound_format_get, _cspace.iSndSysSoftwareFilter3DProperties_sound_format_set)
    __swig_setmethods__["closest_speaker_distance"] = _cspace.iSndSysSoftwareFilter3DProperties_closest_speaker_distance_set
    __swig_getmethods__["closest_speaker_distance"] = _cspace.iSndSysSoftwareFilter3DProperties_closest_speaker_distance_get
    if _newclass:closest_speaker_distance = property(_cspace.iSndSysSoftwareFilter3DProperties_closest_speaker_distance_get, _cspace.iSndSysSoftwareFilter3DProperties_closest_speaker_distance_set)
    __swig_setmethods__["speaker_distance"] = _cspace.iSndSysSoftwareFilter3DProperties_speaker_distance_set
    __swig_getmethods__["speaker_distance"] = _cspace.iSndSysSoftwareFilter3DProperties_speaker_distance_get
    if _newclass:speaker_distance = property(_cspace.iSndSysSoftwareFilter3DProperties_speaker_distance_get, _cspace.iSndSysSoftwareFilter3DProperties_speaker_distance_set)
    __swig_setmethods__["speaker_direction_cos"] = _cspace.iSndSysSoftwareFilter3DProperties_speaker_direction_cos_set
    __swig_getmethods__["speaker_direction_cos"] = _cspace.iSndSysSoftwareFilter3DProperties_speaker_direction_cos_get
    if _newclass:speaker_direction_cos = property(_cspace.iSndSysSoftwareFilter3DProperties_speaker_direction_cos_get, _cspace.iSndSysSoftwareFilter3DProperties_speaker_direction_cos_set)
    __swig_setmethods__["channel"] = _cspace.iSndSysSoftwareFilter3DProperties_channel_set
    __swig_getmethods__["channel"] = _cspace.iSndSysSoftwareFilter3DProperties_channel_get
    if _newclass:channel = property(_cspace.iSndSysSoftwareFilter3DProperties_channel_get, _cspace.iSndSysSoftwareFilter3DProperties_channel_set)
    def __init__(self, *args): 
        this = _cspace.new_iSndSysSoftwareFilter3DProperties(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_iSndSysSoftwareFilter3DProperties
    __del__ = lambda self : None;
iSndSysSoftwareFilter3DProperties_swigregister = _cspace.iSndSysSoftwareFilter3DProperties_swigregister
iSndSysSoftwareFilter3DProperties_swigregister(iSndSysSoftwareFilter3DProperties)

class iSndSysSoftwareFilter3D(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysSoftwareFilter3D, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysSoftwareFilter3D, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Apply(*args): return _cspace.iSndSysSoftwareFilter3D_Apply(*args)
    def AddSubFilter(*args): return _cspace.iSndSysSoftwareFilter3D_AddSubFilter(*args)
    def GetSubFilter(*args): return _cspace.iSndSysSoftwareFilter3D_GetSubFilter(*args)
    def GetPtr(*args): return _cspace.iSndSysSoftwareFilter3D_GetPtr(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSndSysSoftwareFilter3D_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSndSysSoftwareFilter3D_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSndSysSoftwareFilter3D
    __del__ = lambda self : None;
iSndSysSoftwareFilter3D_swigregister = _cspace.iSndSysSoftwareFilter3D_swigregister
iSndSysSoftwareFilter3D_swigregister(iSndSysSoftwareFilter3D)
iSndSysSoftwareFilter3D_scfGetVersion = _cspace.iSndSysSoftwareFilter3D_scfGetVersion

class iSndSysSoftwareOutputFilter(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysSoftwareOutputFilter, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysSoftwareOutputFilter, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def FormatNotify(*args): return _cspace.iSndSysSoftwareOutputFilter_FormatNotify(*args)
    def DeliverData(*args): return _cspace.iSndSysSoftwareOutputFilter_DeliverData(*args)
    __swig_destroy__ = _cspace.delete_iSndSysSoftwareOutputFilter
    __del__ = lambda self : None;
iSndSysSoftwareOutputFilter_swigregister = _cspace.iSndSysSoftwareOutputFilter_swigregister
iSndSysSoftwareOutputFilter_swigregister(iSndSysSoftwareOutputFilter)

SS_FILTER_LOC_RENDEROUT = _cspace.SS_FILTER_LOC_RENDEROUT
SS_FILTER_LOC_SOURCEOUT = _cspace.SS_FILTER_LOC_SOURCEOUT
SS_FILTER_LOC_SOURCEIN = _cspace.SS_FILTER_LOC_SOURCEIN
class iSndSysListener(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysListener, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysListener, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetDirection(*args): return _cspace.iSndSysListener_SetDirection(*args)
    def SetPosition(*args): return _cspace.iSndSysListener_SetPosition(*args)
    def SetDistanceFactor(*args): return _cspace.iSndSysListener_SetDistanceFactor(*args)
    def SetRollOffFactor(*args): return _cspace.iSndSysListener_SetRollOffFactor(*args)
    def GetDirection(*args): return _cspace.iSndSysListener_GetDirection(*args)
    def GetPosition(*args): return _cspace.iSndSysListener_GetPosition(*args)
    def GetDistanceFactor(*args): return _cspace.iSndSysListener_GetDistanceFactor(*args)
    def GetRollOffFactor(*args): return _cspace.iSndSysListener_GetRollOffFactor(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSndSysListener_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSndSysListener_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSndSysListener
    __del__ = lambda self : None;
iSndSysListener_swigregister = _cspace.iSndSysListener_swigregister
iSndSysListener_swigregister(iSndSysListener)
iSndSysListener_scfGetVersion = _cspace.iSndSysListener_scfGetVersion

class iSndSysLoader(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysLoader, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysLoader, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def LoadSound(*args): return _cspace.iSndSysLoader_LoadSound(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSndSysLoader_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSndSysLoader_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSndSysLoader
    __del__ = lambda self : None;
iSndSysLoader_swigregister = _cspace.iSndSysLoader_swigregister
iSndSysLoader_swigregister(iSndSysLoader)
iSndSysLoader_scfGetVersion = _cspace.iSndSysLoader_scfGetVersion

class iSndSysWrapper(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysWrapper, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysWrapper, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iSndSysWrapper_QueryObject(*args)
    def GetData(*args): return _cspace.iSndSysWrapper_GetData(*args)
    def SetData(*args): return _cspace.iSndSysWrapper_SetData(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSndSysWrapper_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSndSysWrapper_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSndSysWrapper
    __del__ = lambda self : None;
iSndSysWrapper_swigregister = _cspace.iSndSysWrapper_swigregister
iSndSysWrapper_swigregister(iSndSysWrapper)
iSndSysWrapper_scfGetVersion = _cspace.iSndSysWrapper_scfGetVersion

class iSndSysManager(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysManager, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysManager, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CreateSound(*args): return _cspace.iSndSysManager_CreateSound(*args)
    def RemoveSound(*args): return _cspace.iSndSysManager_RemoveSound(*args)
    def RemoveSounds(*args): return _cspace.iSndSysManager_RemoveSounds(*args)
    def GetSoundCount(*args): return _cspace.iSndSysManager_GetSoundCount(*args)
    def GetSound(*args): return _cspace.iSndSysManager_GetSound(*args)
    def FindSoundByName(*args): return _cspace.iSndSysManager_FindSoundByName(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSndSysManager_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSndSysManager_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSndSysManager
    __del__ = lambda self : None;
iSndSysManager_swigregister = _cspace.iSndSysManager_swigregister
iSndSysManager_swigregister(iSndSysManager)
iSndSysManager_scfGetVersion = _cspace.iSndSysManager_scfGetVersion

CS_SNDSYS_SOURCE_DISTANCE_INFINITE = _cspace.CS_SNDSYS_SOURCE_DISTANCE_INFINITE
class iSndSysSource(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysSource, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysSource, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetVolume(*args): return _cspace.iSndSysSource_SetVolume(*args)
    def GetVolume(*args): return _cspace.iSndSysSource_GetVolume(*args)
    def GetStream(*args): return _cspace.iSndSysSource_GetStream(*args)
    def AddOutputFilter(*args): return _cspace.iSndSysSource_AddOutputFilter(*args)
    def RemoveOutputFilter(*args): return _cspace.iSndSysSource_RemoveOutputFilter(*args)
    def GetPtr(*args): return _cspace.iSndSysSource_GetPtr(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSndSysSource_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSndSysSource_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSndSysSource
    __del__ = lambda self : None;
iSndSysSource_swigregister = _cspace.iSndSysSource_swigregister
iSndSysSource_swigregister(iSndSysSource)
iSndSysSource_scfGetVersion = _cspace.iSndSysSource_scfGetVersion

class iSndSysSourceSoftware(iSndSysSource):
    __swig_setmethods__ = {}
    for _s in [iSndSysSource]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysSourceSoftware, name, value)
    __swig_getmethods__ = {}
    for _s in [iSndSysSource]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysSourceSoftware, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def MergeIntoBuffer(*args): return _cspace.iSndSysSourceSoftware_MergeIntoBuffer(*args)
    def ProcessOutputFilters(*args): return _cspace.iSndSysSourceSoftware_ProcessOutputFilters(*args)
    __swig_destroy__ = _cspace.delete_iSndSysSourceSoftware
    __del__ = lambda self : None;
iSndSysSourceSoftware_swigregister = _cspace.iSndSysSourceSoftware_swigregister
iSndSysSourceSoftware_swigregister(iSndSysSourceSoftware)

class iSndSysSourceSoftware3D(iSndSysSourceSoftware):
    __swig_setmethods__ = {}
    for _s in [iSndSysSourceSoftware]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysSourceSoftware3D, name, value)
    __swig_getmethods__ = {}
    for _s in [iSndSysSourceSoftware]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysSourceSoftware3D, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetPosition(*args): return _cspace.iSndSysSourceSoftware3D_SetPosition(*args)
    def GetPosition(*args): return _cspace.iSndSysSourceSoftware3D_GetPosition(*args)
    def SetDirection(*args): return _cspace.iSndSysSourceSoftware3D_SetDirection(*args)
    def GetDirection(*args): return _cspace.iSndSysSourceSoftware3D_GetDirection(*args)
    def SetDirectionalRadiation(*args): return _cspace.iSndSysSourceSoftware3D_SetDirectionalRadiation(*args)
    def GetDirectionalRadiation(*args): return _cspace.iSndSysSourceSoftware3D_GetDirectionalRadiation(*args)
    def SetMinimumDistance(*args): return _cspace.iSndSysSourceSoftware3D_SetMinimumDistance(*args)
    def SetMaximumDistance(*args): return _cspace.iSndSysSourceSoftware3D_SetMaximumDistance(*args)
    def GetMinimumDistance(*args): return _cspace.iSndSysSourceSoftware3D_GetMinimumDistance(*args)
    def GetMaximumDistance(*args): return _cspace.iSndSysSourceSoftware3D_GetMaximumDistance(*args)
    __swig_destroy__ = _cspace.delete_iSndSysSourceSoftware3D
    __del__ = lambda self : None;
iSndSysSourceSoftware3D_swigregister = _cspace.iSndSysSourceSoftware3D_swigregister
iSndSysSourceSoftware3D_swigregister(iSndSysSourceSoftware3D)

CSSNDSYS_SAMPLE_LITTLE_ENDIAN = _cspace.CSSNDSYS_SAMPLE_LITTLE_ENDIAN
CSSNDSYS_SAMPLE_BIG_ENDIAN = _cspace.CSSNDSYS_SAMPLE_BIG_ENDIAN
CSSNDSYS_SAMPLE_ENDIAN_MASK = _cspace.CSSNDSYS_SAMPLE_ENDIAN_MASK
class csSndSysSoundFormat(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSndSysSoundFormat, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csSndSysSoundFormat, name)
    __repr__ = _swig_repr
    __swig_setmethods__["Freq"] = _cspace.csSndSysSoundFormat_Freq_set
    __swig_getmethods__["Freq"] = _cspace.csSndSysSoundFormat_Freq_get
    if _newclass:Freq = property(_cspace.csSndSysSoundFormat_Freq_get, _cspace.csSndSysSoundFormat_Freq_set)
    __swig_setmethods__["Bits"] = _cspace.csSndSysSoundFormat_Bits_set
    __swig_getmethods__["Bits"] = _cspace.csSndSysSoundFormat_Bits_get
    if _newclass:Bits = property(_cspace.csSndSysSoundFormat_Bits_get, _cspace.csSndSysSoundFormat_Bits_set)
    __swig_setmethods__["Channels"] = _cspace.csSndSysSoundFormat_Channels_set
    __swig_getmethods__["Channels"] = _cspace.csSndSysSoundFormat_Channels_get
    if _newclass:Channels = property(_cspace.csSndSysSoundFormat_Channels_get, _cspace.csSndSysSoundFormat_Channels_set)
    __swig_setmethods__["Flags"] = _cspace.csSndSysSoundFormat_Flags_set
    __swig_getmethods__["Flags"] = _cspace.csSndSysSoundFormat_Flags_get
    if _newclass:Flags = property(_cspace.csSndSysSoundFormat_Flags_get, _cspace.csSndSysSoundFormat_Flags_set)
    def __init__(self, *args): 
        this = _cspace.new_csSndSysSoundFormat(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csSndSysSoundFormat
    __del__ = lambda self : None;
csSndSysSoundFormat_swigregister = _cspace.csSndSysSoundFormat_swigregister
csSndSysSoundFormat_swigregister(csSndSysSoundFormat)

CS_SNDSYS_STREAM_PAUSED = _cspace.CS_SNDSYS_STREAM_PAUSED
CS_SNDSYS_STREAM_UNPAUSED = _cspace.CS_SNDSYS_STREAM_UNPAUSED
CS_SNDSYS_STREAM_DONTLOOP = _cspace.CS_SNDSYS_STREAM_DONTLOOP
CS_SNDSYS_STREAM_LOOP = _cspace.CS_SNDSYS_STREAM_LOOP
CS_SND3D_DISABLE = _cspace.CS_SND3D_DISABLE
CS_SND3D_RELATIVE = _cspace.CS_SND3D_RELATIVE
CS_SND3D_ABSOLUTE = _cspace.CS_SND3D_ABSOLUTE
class iSndSysStream(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysStream, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysStream, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetDescription(*args): return _cspace.iSndSysStream_GetDescription(*args)
    def GetRenderedFormat(*args): return _cspace.iSndSysStream_GetRenderedFormat(*args)
    def Get3dMode(*args): return _cspace.iSndSysStream_Get3dMode(*args)
    def GetFrameCount(*args): return _cspace.iSndSysStream_GetFrameCount(*args)
    def GetPosition(*args): return _cspace.iSndSysStream_GetPosition(*args)
    def ResetPosition(*args): return _cspace.iSndSysStream_ResetPosition(*args)
    def SetPosition(*args): return _cspace.iSndSysStream_SetPosition(*args)
    def Pause(*args): return _cspace.iSndSysStream_Pause(*args)
    def Unpause(*args): return _cspace.iSndSysStream_Unpause(*args)
    def GetPauseState(*args): return _cspace.iSndSysStream_GetPauseState(*args)
    def SetLoopState(*args): return _cspace.iSndSysStream_SetLoopState(*args)
    def GetLoopState(*args): return _cspace.iSndSysStream_GetLoopState(*args)
    def SetPlayRatePercent(*args): return _cspace.iSndSysStream_SetPlayRatePercent(*args)
    def GetPlayRatePercent(*args): return _cspace.iSndSysStream_GetPlayRatePercent(*args)
    def SetAutoUnregister(*args): return _cspace.iSndSysStream_SetAutoUnregister(*args)
    def GetAutoUnregister(*args): return _cspace.iSndSysStream_GetAutoUnregister(*args)
    def GetAutoUnregisterRequested(*args): return _cspace.iSndSysStream_GetAutoUnregisterRequested(*args)
    def AdvancePosition(*args): return _cspace.iSndSysStream_AdvancePosition(*args)
    def GetDataPointers(*args): return _cspace.iSndSysStream_GetDataPointers(*args)
    def InitializeSourcePositionMarker(*args): return _cspace.iSndSysStream_InitializeSourcePositionMarker(*args)
    def ProcessNotifications(*args): return _cspace.iSndSysStream_ProcessNotifications(*args)
    def RegisterCallback(*args): return _cspace.iSndSysStream_RegisterCallback(*args)
    def UnregisterCallback(*args): return _cspace.iSndSysStream_UnregisterCallback(*args)
    def RegisterFrameNotification(*args): return _cspace.iSndSysStream_RegisterFrameNotification(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSndSysStream_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSndSysStream_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSndSysStream
    __del__ = lambda self : None;
iSndSysStream_swigregister = _cspace.iSndSysStream_swigregister
iSndSysStream_swigregister(iSndSysStream)
CS_SNDSYS_STREAM_UNKNOWN_LENGTH = cvar.CS_SNDSYS_STREAM_UNKNOWN_LENGTH
iSndSysStream_scfGetVersion = _cspace.iSndSysStream_scfGetVersion

class iSndSysStreamCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysStreamCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysStreamCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def StreamLoopNotification(*args): return _cspace.iSndSysStreamCallback_StreamLoopNotification(*args)
    def StreamPauseNotification(*args): return _cspace.iSndSysStreamCallback_StreamPauseNotification(*args)
    def StreamUnpauseNotification(*args): return _cspace.iSndSysStreamCallback_StreamUnpauseNotification(*args)
    def StreamFrameNotification(*args): return _cspace.iSndSysStreamCallback_StreamFrameNotification(*args)
    __swig_destroy__ = _cspace.delete_iSndSysStreamCallback
    __del__ = lambda self : None;
iSndSysStreamCallback_swigregister = _cspace.iSndSysStreamCallback_swigregister
iSndSysStreamCallback_swigregister(iSndSysStreamCallback)

CS_SNDSYS_SOURCE_STOPPED = _cspace.CS_SNDSYS_SOURCE_STOPPED
CS_SNDSYS_SOURCE_PLAYING = _cspace.CS_SNDSYS_SOURCE_PLAYING
class iSndSysRenderer(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysRenderer, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysRenderer, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetVolume(*args): return _cspace.iSndSysRenderer_SetVolume(*args)
    def GetVolume(*args): return _cspace.iSndSysRenderer_GetVolume(*args)
    def CreateStream(*args): return _cspace.iSndSysRenderer_CreateStream(*args)
    def CreateSource(*args): return _cspace.iSndSysRenderer_CreateSource(*args)
    def RemoveStream(*args): return _cspace.iSndSysRenderer_RemoveStream(*args)
    def RemoveSource(*args): return _cspace.iSndSysRenderer_RemoveSource(*args)
    def GetListener(*args): return _cspace.iSndSysRenderer_GetListener(*args)
    def RegisterCallback(*args): return _cspace.iSndSysRenderer_RegisterCallback(*args)
    def UnregisterCallback(*args): return _cspace.iSndSysRenderer_UnregisterCallback(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSndSysRenderer_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSndSysRenderer_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSndSysRenderer
    __del__ = lambda self : None;
iSndSysRenderer_swigregister = _cspace.iSndSysRenderer_swigregister
iSndSysRenderer_swigregister(iSndSysRenderer)
iSndSysRenderer_scfGetVersion = _cspace.iSndSysRenderer_scfGetVersion

class iSndSysRendererCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysRendererCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysRendererCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def StreamAddNotification(*args): return _cspace.iSndSysRendererCallback_StreamAddNotification(*args)
    def StreamRemoveNotification(*args): return _cspace.iSndSysRendererCallback_StreamRemoveNotification(*args)
    def SourceAddNotification(*args): return _cspace.iSndSysRendererCallback_SourceAddNotification(*args)
    def SourceRemoveNotification(*args): return _cspace.iSndSysRendererCallback_SourceRemoveNotification(*args)
    __swig_destroy__ = _cspace.delete_iSndSysRendererCallback
    __del__ = lambda self : None;
iSndSysRendererCallback_swigregister = _cspace.iSndSysRendererCallback_swigregister
iSndSysRendererCallback_swigregister(iSndSysRendererCallback)

class iSndSysRendererSoftware(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysRendererSoftware, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysRendererSoftware, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def AddOutputFilter(*args): return _cspace.iSndSysRendererSoftware_AddOutputFilter(*args)
    def RemoveOutputFilter(*args): return _cspace.iSndSysRendererSoftware_RemoveOutputFilter(*args)
    __swig_destroy__ = _cspace.delete_iSndSysRendererSoftware
    __del__ = lambda self : None;
iSndSysRendererSoftware_swigregister = _cspace.iSndSysRendererSoftware_swigregister
iSndSysRendererSoftware_swigregister(iSndSysRendererSoftware)

class iSndSysSoftwareDriver(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSndSysSoftwareDriver, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSndSysSoftwareDriver, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Open(*args): return _cspace.iSndSysSoftwareDriver_Open(*args)
    def Close(*args): return _cspace.iSndSysSoftwareDriver_Close(*args)
    def StartThread(*args): return _cspace.iSndSysSoftwareDriver_StartThread(*args)
    def StopThread(*args): return _cspace.iSndSysSoftwareDriver_StopThread(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSndSysSoftwareDriver_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSndSysSoftwareDriver_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSndSysSoftwareDriver
    __del__ = lambda self : None;
iSndSysSoftwareDriver_swigregister = _cspace.iSndSysSoftwareDriver_swigregister
iSndSysSoftwareDriver_swigregister(iSndSysSoftwareDriver)
iSndSysSoftwareDriver_scfGetVersion = _cspace.iSndSysSoftwareDriver_scfGetVersion

class iComponent(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iComponent, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iComponent, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Initialize(*args): return _cspace.iComponent_Initialize(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iComponent_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iComponent_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iComponent
    __del__ = lambda self : None;
iComponent_swigregister = _cspace.iComponent_swigregister
iComponent_swigregister(iComponent)
iComponent_scfGetVersion = _cspace.iComponent_scfGetVersion

class iCacheManager(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iCacheManager, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iCacheManager, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetReadOnly(*args): return _cspace.iCacheManager_SetReadOnly(*args)
    def IsReadOnly(*args): return _cspace.iCacheManager_IsReadOnly(*args)
    def SetCurrentType(*args): return _cspace.iCacheManager_SetCurrentType(*args)
    def GetCurrentType(*args): return _cspace.iCacheManager_GetCurrentType(*args)
    def SetCurrentScope(*args): return _cspace.iCacheManager_SetCurrentScope(*args)
    def GetCurrentScope(*args): return _cspace.iCacheManager_GetCurrentScope(*args)
    def CacheData(*args): return _cspace.iCacheManager_CacheData(*args)
    def ReadCache(*args): return _cspace.iCacheManager_ReadCache(*args)
    def ClearCache(*args): return _cspace.iCacheManager_ClearCache(*args)
    def Flush(*args): return _cspace.iCacheManager_Flush(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iCacheManager_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iCacheManager_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iCacheManager
    __del__ = lambda self : None;
iCacheManager_swigregister = _cspace.iCacheManager_swigregister
iCacheManager_swigregister(iCacheManager)
iCacheManager_scfGetVersion = _cspace.iCacheManager_scfGetVersion

class csFileTime(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csFileTime, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csFileTime, name)
    __repr__ = _swig_repr
    __swig_setmethods__["sec"] = _cspace.csFileTime_sec_set
    __swig_getmethods__["sec"] = _cspace.csFileTime_sec_get
    if _newclass:sec = property(_cspace.csFileTime_sec_get, _cspace.csFileTime_sec_set)
    __swig_setmethods__["min"] = _cspace.csFileTime_min_set
    __swig_getmethods__["min"] = _cspace.csFileTime_min_get
    if _newclass:min = property(_cspace.csFileTime_min_get, _cspace.csFileTime_min_set)
    __swig_setmethods__["hour"] = _cspace.csFileTime_hour_set
    __swig_getmethods__["hour"] = _cspace.csFileTime_hour_get
    if _newclass:hour = property(_cspace.csFileTime_hour_get, _cspace.csFileTime_hour_set)
    __swig_setmethods__["day"] = _cspace.csFileTime_day_set
    __swig_getmethods__["day"] = _cspace.csFileTime_day_get
    if _newclass:day = property(_cspace.csFileTime_day_get, _cspace.csFileTime_day_set)
    __swig_setmethods__["mon"] = _cspace.csFileTime_mon_set
    __swig_getmethods__["mon"] = _cspace.csFileTime_mon_get
    if _newclass:mon = property(_cspace.csFileTime_mon_get, _cspace.csFileTime_mon_set)
    __swig_setmethods__["year"] = _cspace.csFileTime_year_set
    __swig_getmethods__["year"] = _cspace.csFileTime_year_get
    if _newclass:year = property(_cspace.csFileTime_year_get, _cspace.csFileTime_year_set)
    def __init__(self, *args): 
        this = _cspace.new_csFileTime(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csFileTime
    __del__ = lambda self : None;
csFileTime_swigregister = _cspace.csFileTime_swigregister
csFileTime_swigregister(csFileTime)

VFS_PATH_DIVIDER = _cspace.VFS_PATH_DIVIDER
VFS_PATH_SEPARATOR = _cspace.VFS_PATH_SEPARATOR
VFS_MAX_PATH_LEN = _cspace.VFS_MAX_PATH_LEN
VFS_FILE_MODE = _cspace.VFS_FILE_MODE
VFS_FILE_READ = _cspace.VFS_FILE_READ
VFS_FILE_WRITE = _cspace.VFS_FILE_WRITE
VFS_FILE_APPEND = _cspace.VFS_FILE_APPEND
VFS_FILE_UNCOMPRESSED = _cspace.VFS_FILE_UNCOMPRESSED
VFS_STATUS_OK = _cspace.VFS_STATUS_OK
VFS_STATUS_OTHER = _cspace.VFS_STATUS_OTHER
VFS_STATUS_NOSPACE = _cspace.VFS_STATUS_NOSPACE
VFS_STATUS_RESOURCES = _cspace.VFS_STATUS_RESOURCES
VFS_STATUS_ACCESSDENIED = _cspace.VFS_STATUS_ACCESSDENIED
VFS_STATUS_IOERROR = _cspace.VFS_STATUS_IOERROR
class iFile(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iFile, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iFile, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetName(*args): return _cspace.iFile_GetName(*args)
    def GetSize(*args): return _cspace.iFile_GetSize(*args)
    def GetStatus(*args): return _cspace.iFile_GetStatus(*args)
    def Read(*args): return _cspace.iFile_Read(*args)
    def Write(*args): return _cspace.iFile_Write(*args)
    def Flush(*args): return _cspace.iFile_Flush(*args)
    def AtEOF(*args): return _cspace.iFile_AtEOF(*args)
    def GetPos(*args): return _cspace.iFile_GetPos(*args)
    def SetPos(*args): return _cspace.iFile_SetPos(*args)
    def GetAllData(*args): return _cspace.iFile_GetAllData(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iFile_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iFile_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iFile
    __del__ = lambda self : None;
iFile_swigregister = _cspace.iFile_swigregister
iFile_swigregister(iFile)
iFile_scfGetVersion = _cspace.iFile_scfGetVersion

class iVFS(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iVFS, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iVFS, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def ChDir(*args): return _cspace.iVFS_ChDir(*args)
    def GetCwd(*args): return _cspace.iVFS_GetCwd(*args)
    def PushDir(*args): return _cspace.iVFS_PushDir(*args)
    def PopDir(*args): return _cspace.iVFS_PopDir(*args)
    def ExpandPath(*args): return _cspace.iVFS_ExpandPath(*args)
    def Exists(*args): return _cspace.iVFS_Exists(*args)
    def FindFiles(*args): return _cspace.iVFS_FindFiles(*args)
    def Open(*args): return _cspace.iVFS_Open(*args)
    def ReadFile(*args): return _cspace.iVFS_ReadFile(*args)
    def WriteFile(*args): return _cspace.iVFS_WriteFile(*args)
    def DeleteFile(*args): return _cspace.iVFS_DeleteFile(*args)
    def Sync(*args): return _cspace.iVFS_Sync(*args)
    def SymbolicLink(*args): return _cspace.iVFS_SymbolicLink(*args)
    def Mount(*args): return _cspace.iVFS_Mount(*args)
    def Unmount(*args): return _cspace.iVFS_Unmount(*args)
    def MountRoot(*args): return _cspace.iVFS_MountRoot(*args)
    def SaveMounts(*args): return _cspace.iVFS_SaveMounts(*args)
    def LoadMountsFromFile(*args): return _cspace.iVFS_LoadMountsFromFile(*args)
    def ChDirAuto(*args): return _cspace.iVFS_ChDirAuto(*args)
    def GetFileTime(*args): return _cspace.iVFS_GetFileTime(*args)
    def SetFileTime(*args): return _cspace.iVFS_SetFileTime(*args)
    def GetFileSize(*args): return _cspace.iVFS_GetFileSize(*args)
    def GetRealPath(*args): return _cspace.iVFS_GetRealPath(*args)
    def GetMounts(*args): return _cspace.iVFS_GetMounts(*args)
    def GetRealMountPaths(*args): return _cspace.iVFS_GetRealMountPaths(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iVFS_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iVFS_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iVFS
    __del__ = lambda self : None;
iVFS_swigregister = _cspace.iVFS_swigregister
iVFS_swigregister(iVFS)
iVFS_scfGetVersion = _cspace.iVFS_scfGetVersion

class iObjectNameChangeListener(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iObjectNameChangeListener, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iObjectNameChangeListener, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def NameChanged(*args): return _cspace.iObjectNameChangeListener_NameChanged(*args)
    __swig_destroy__ = _cspace.delete_iObjectNameChangeListener
    __del__ = lambda self : None;
iObjectNameChangeListener_swigregister = _cspace.iObjectNameChangeListener_swigregister
iObjectNameChangeListener_swigregister(iObjectNameChangeListener)

class iObject(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iObject, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iObject, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetName(*args): return _cspace.iObject_SetName(*args)
    def GetName(*args): return _cspace.iObject_GetName(*args)
    def GetID(*args): return _cspace.iObject_GetID(*args)
    def SetObjectParent(*args): return _cspace.iObject_SetObjectParent(*args)
    def GetObjectParent(*args): return _cspace.iObject_GetObjectParent(*args)
    def ObjAdd(*args): return _cspace.iObject_ObjAdd(*args)
    def ObjRemove(*args): return _cspace.iObject_ObjRemove(*args)
    def ObjRemoveAll(*args): return _cspace.iObject_ObjRemoveAll(*args)
    def ObjAddChildren(*args): return _cspace.iObject_ObjAddChildren(*args)
    def GetChild(*args): return _cspace.iObject_GetChild(*args)
    def GetIterator(*args): return _cspace.iObject_GetIterator(*args)
    def ObjReleaseOld(*args): return _cspace.iObject_ObjReleaseOld(*args)
    def AddNameChangeListener(*args): return _cspace.iObject_AddNameChangeListener(*args)
    def RemoveNameChangeListener(*args): return _cspace.iObject_RemoveNameChangeListener(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iObject_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iObject_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iObject
    __del__ = lambda self : None;
iObject_swigregister = _cspace.iObject_swigregister
iObject_swigregister(iObject)
iObject_scfGetVersion = _cspace.iObject_scfGetVersion

class iObjectIterator(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iObjectIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iObjectIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Next(*args): return _cspace.iObjectIterator_Next(*args)
    def Reset(*args): return _cspace.iObjectIterator_Reset(*args)
    def GetParentObj(*args): return _cspace.iObjectIterator_GetParentObj(*args)
    def HasNext(*args): return _cspace.iObjectIterator_HasNext(*args)
    def FindName(*args): return _cspace.iObjectIterator_FindName(*args)
    __swig_destroy__ = _cspace.delete_iObjectIterator
    __del__ = lambda self : None;
iObjectIterator_swigregister = _cspace.iObjectIterator_swigregister
iObjectIterator_swigregister(iObjectIterator)

class iStringSet(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iStringSet, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iStringSet, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Request(*args): return _cspace.iStringSet_Request(*args)
    def Contains(*args): return _cspace.iStringSet_Contains(*args)
    def Delete(*args): return _cspace.iStringSet_Delete(*args)
    def Empty(*args): return _cspace.iStringSet_Empty(*args)
    def Clear(*args): return _cspace.iStringSet_Clear(*args)
    def GetSize(*args): return _cspace.iStringSet_GetSize(*args)
    def IsEmpty(*args): return _cspace.iStringSet_IsEmpty(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iStringSet_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iStringSet_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iStringSet
    __del__ = lambda self : None;
iStringSet_swigregister = _cspace.iStringSet_swigregister
iStringSet_swigregister(iStringSet)
csInvalidStringID = cvar.csInvalidStringID
iStringSet_scfGetVersion = _cspace.iStringSet_scfGetVersion

class iObjectRegistry(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iObjectRegistry, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iObjectRegistry, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Clear(*args): return _cspace.iObjectRegistry_Clear(*args)
    def Register(*args): return _cspace.iObjectRegistry_Register(*args)
    def Unregister(*args): return _cspace.iObjectRegistry_Unregister(*args)
    def Get(*args): return _cspace.iObjectRegistry_Get(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iObjectRegistry_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iObjectRegistry_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iObjectRegistry
    __del__ = lambda self : None;
iObjectRegistry_swigregister = _cspace.iObjectRegistry_swigregister
iObjectRegistry_swigregister(iObjectRegistry)
iObjectRegistry_scfGetVersion = _cspace.iObjectRegistry_scfGetVersion

class iObjectRegistryIterator(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iObjectRegistryIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iObjectRegistryIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Reset(*args): return _cspace.iObjectRegistryIterator_Reset(*args)
    def GetCurrentTag(*args): return _cspace.iObjectRegistryIterator_GetCurrentTag(*args)
    def HasNext(*args): return _cspace.iObjectRegistryIterator_HasNext(*args)
    def Next(*args): return _cspace.iObjectRegistryIterator_Next(*args)
    __swig_destroy__ = _cspace.delete_iObjectRegistryIterator
    __del__ = lambda self : None;
iObjectRegistryIterator_swigregister = _cspace.iObjectRegistryIterator_swigregister
iObjectRegistryIterator_swigregister(iObjectRegistryIterator)

csQueryRegistryTag = _cspace.csQueryRegistryTag
class iVirtualClock(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iVirtualClock, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iVirtualClock, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Advance(*args): return _cspace.iVirtualClock_Advance(*args)
    def Suspend(*args): return _cspace.iVirtualClock_Suspend(*args)
    def Resume(*args): return _cspace.iVirtualClock_Resume(*args)
    def GetElapsedTicks(*args): return _cspace.iVirtualClock_GetElapsedTicks(*args)
    def GetCurrentTicks(*args): return _cspace.iVirtualClock_GetCurrentTicks(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iVirtualClock_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iVirtualClock_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iVirtualClock
    __del__ = lambda self : None;
iVirtualClock_swigregister = _cspace.iVirtualClock_swigregister
iVirtualClock_swigregister(iVirtualClock)
iVirtualClock_scfGetVersion = _cspace.iVirtualClock_scfGetVersion

CS_MAX_MOUSE_COUNT = _cspace.CS_MAX_MOUSE_COUNT
CS_MAX_MOUSE_AXES = _cspace.CS_MAX_MOUSE_AXES
CS_MAX_MOUSE_BUTTONS = _cspace.CS_MAX_MOUSE_BUTTONS
CS_MAX_JOYSTICK_COUNT = _cspace.CS_MAX_JOYSTICK_COUNT
CS_MAX_JOYSTICK_BUTTONS = _cspace.CS_MAX_JOYSTICK_BUTTONS
CS_MAX_JOYSTICK_AXES = _cspace.CS_MAX_JOYSTICK_AXES
class iEventAttributeIterator(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iEventAttributeIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iEventAttributeIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def HasNext(*args): return _cspace.iEventAttributeIterator_HasNext(*args)
    def Next(*args): return _cspace.iEventAttributeIterator_Next(*args)
    def Reset(*args): return _cspace.iEventAttributeIterator_Reset(*args)
    __swig_destroy__ = _cspace.delete_iEventAttributeIterator
    __del__ = lambda self : None;
iEventAttributeIterator_swigregister = _cspace.iEventAttributeIterator_swigregister
iEventAttributeIterator_swigregister(iEventAttributeIterator)

class csKeyEventData(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csKeyEventData, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csKeyEventData, name)
    __repr__ = _swig_repr
    __swig_setmethods__["eventType"] = _cspace.csKeyEventData_eventType_set
    __swig_getmethods__["eventType"] = _cspace.csKeyEventData_eventType_get
    if _newclass:eventType = property(_cspace.csKeyEventData_eventType_get, _cspace.csKeyEventData_eventType_set)
    __swig_setmethods__["codeRaw"] = _cspace.csKeyEventData_codeRaw_set
    __swig_getmethods__["codeRaw"] = _cspace.csKeyEventData_codeRaw_get
    if _newclass:codeRaw = property(_cspace.csKeyEventData_codeRaw_get, _cspace.csKeyEventData_codeRaw_set)
    __swig_setmethods__["codeCooked"] = _cspace.csKeyEventData_codeCooked_set
    __swig_getmethods__["codeCooked"] = _cspace.csKeyEventData_codeCooked_get
    if _newclass:codeCooked = property(_cspace.csKeyEventData_codeCooked_get, _cspace.csKeyEventData_codeCooked_set)
    __swig_setmethods__["modifiers"] = _cspace.csKeyEventData_modifiers_set
    __swig_getmethods__["modifiers"] = _cspace.csKeyEventData_modifiers_get
    if _newclass:modifiers = property(_cspace.csKeyEventData_modifiers_get, _cspace.csKeyEventData_modifiers_set)
    __swig_setmethods__["autoRepeat"] = _cspace.csKeyEventData_autoRepeat_set
    __swig_getmethods__["autoRepeat"] = _cspace.csKeyEventData_autoRepeat_get
    if _newclass:autoRepeat = property(_cspace.csKeyEventData_autoRepeat_get, _cspace.csKeyEventData_autoRepeat_set)
    __swig_setmethods__["charType"] = _cspace.csKeyEventData_charType_set
    __swig_getmethods__["charType"] = _cspace.csKeyEventData_charType_get
    if _newclass:charType = property(_cspace.csKeyEventData_charType_get, _cspace.csKeyEventData_charType_set)
    def __init__(self, *args): 
        this = _cspace.new_csKeyEventData(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csKeyEventData
    __del__ = lambda self : None;
csKeyEventData_swigregister = _cspace.csKeyEventData_swigregister
csKeyEventData_swigregister(csKeyEventData)

class csMouseEventData(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csMouseEventData, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csMouseEventData, name)
    __repr__ = _swig_repr
    __swig_setmethods__["x"] = _cspace.csMouseEventData_x_set
    __swig_getmethods__["x"] = _cspace.csMouseEventData_x_get
    if _newclass:x = property(_cspace.csMouseEventData_x_get, _cspace.csMouseEventData_x_set)
    __swig_setmethods__["y"] = _cspace.csMouseEventData_y_set
    __swig_getmethods__["y"] = _cspace.csMouseEventData_y_get
    if _newclass:y = property(_cspace.csMouseEventData_y_get, _cspace.csMouseEventData_y_set)
    __swig_setmethods__["axes"] = _cspace.csMouseEventData_axes_set
    __swig_getmethods__["axes"] = _cspace.csMouseEventData_axes_get
    if _newclass:axes = property(_cspace.csMouseEventData_axes_get, _cspace.csMouseEventData_axes_set)
    __swig_setmethods__["numAxes"] = _cspace.csMouseEventData_numAxes_set
    __swig_getmethods__["numAxes"] = _cspace.csMouseEventData_numAxes_get
    if _newclass:numAxes = property(_cspace.csMouseEventData_numAxes_get, _cspace.csMouseEventData_numAxes_set)
    __swig_setmethods__["Button"] = _cspace.csMouseEventData_Button_set
    __swig_getmethods__["Button"] = _cspace.csMouseEventData_Button_get
    if _newclass:Button = property(_cspace.csMouseEventData_Button_get, _cspace.csMouseEventData_Button_set)
    __swig_setmethods__["Modifiers"] = _cspace.csMouseEventData_Modifiers_set
    __swig_getmethods__["Modifiers"] = _cspace.csMouseEventData_Modifiers_get
    if _newclass:Modifiers = property(_cspace.csMouseEventData_Modifiers_get, _cspace.csMouseEventData_Modifiers_set)
    def __init__(self, *args): 
        this = _cspace.new_csMouseEventData(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csMouseEventData
    __del__ = lambda self : None;
csMouseEventData_swigregister = _cspace.csMouseEventData_swigregister
csMouseEventData_swigregister(csMouseEventData)

class csJoystickEventData(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csJoystickEventData, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csJoystickEventData, name)
    __repr__ = _swig_repr
    __swig_setmethods__["number"] = _cspace.csJoystickEventData_number_set
    __swig_getmethods__["number"] = _cspace.csJoystickEventData_number_get
    if _newclass:number = property(_cspace.csJoystickEventData_number_get, _cspace.csJoystickEventData_number_set)
    __swig_setmethods__["axes"] = _cspace.csJoystickEventData_axes_set
    __swig_getmethods__["axes"] = _cspace.csJoystickEventData_axes_get
    if _newclass:axes = property(_cspace.csJoystickEventData_axes_get, _cspace.csJoystickEventData_axes_set)
    __swig_setmethods__["numAxes"] = _cspace.csJoystickEventData_numAxes_set
    __swig_getmethods__["numAxes"] = _cspace.csJoystickEventData_numAxes_get
    if _newclass:numAxes = property(_cspace.csJoystickEventData_numAxes_get, _cspace.csJoystickEventData_numAxes_set)
    __swig_setmethods__["axesChanged"] = _cspace.csJoystickEventData_axesChanged_set
    __swig_getmethods__["axesChanged"] = _cspace.csJoystickEventData_axesChanged_get
    if _newclass:axesChanged = property(_cspace.csJoystickEventData_axesChanged_get, _cspace.csJoystickEventData_axesChanged_set)
    __swig_setmethods__["Button"] = _cspace.csJoystickEventData_Button_set
    __swig_getmethods__["Button"] = _cspace.csJoystickEventData_Button_get
    if _newclass:Button = property(_cspace.csJoystickEventData_Button_get, _cspace.csJoystickEventData_Button_set)
    __swig_setmethods__["Modifiers"] = _cspace.csJoystickEventData_Modifiers_set
    __swig_getmethods__["Modifiers"] = _cspace.csJoystickEventData_Modifiers_get
    if _newclass:Modifiers = property(_cspace.csJoystickEventData_Modifiers_get, _cspace.csJoystickEventData_Modifiers_set)
    def __init__(self, *args): 
        this = _cspace.new_csJoystickEventData(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csJoystickEventData
    __del__ = lambda self : None;
csJoystickEventData_swigregister = _cspace.csJoystickEventData_swigregister
csJoystickEventData_swigregister(csJoystickEventData)

class csCommandEventData(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csCommandEventData, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csCommandEventData, name)
    __repr__ = _swig_repr
    __swig_setmethods__["Code"] = _cspace.csCommandEventData_Code_set
    __swig_getmethods__["Code"] = _cspace.csCommandEventData_Code_get
    if _newclass:Code = property(_cspace.csCommandEventData_Code_get, _cspace.csCommandEventData_Code_set)
    __swig_setmethods__["Info"] = _cspace.csCommandEventData_Info_set
    __swig_getmethods__["Info"] = _cspace.csCommandEventData_Info_get
    if _newclass:Info = property(_cspace.csCommandEventData_Info_get, _cspace.csCommandEventData_Info_set)
    def __init__(self, *args): 
        this = _cspace.new_csCommandEventData(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csCommandEventData
    __del__ = lambda self : None;
csCommandEventData_swigregister = _cspace.csCommandEventData_swigregister
csCommandEventData_swigregister(csCommandEventData)

csEventErrNone = _cspace.csEventErrNone
csEventErrLossy = _cspace.csEventErrLossy
csEventErrNotFound = _cspace.csEventErrNotFound
csEventErrMismatchInt = _cspace.csEventErrMismatchInt
csEventErrMismatchUInt = _cspace.csEventErrMismatchUInt
csEventErrMismatchFloat = _cspace.csEventErrMismatchFloat
csEventErrMismatchBuffer = _cspace.csEventErrMismatchBuffer
csEventErrMismatchEvent = _cspace.csEventErrMismatchEvent
csEventErrMismatchIBase = _cspace.csEventErrMismatchIBase
csEventErrUhOhUnknown = _cspace.csEventErrUhOhUnknown
csEventAttrUnknown = _cspace.csEventAttrUnknown
csEventAttrInt = _cspace.csEventAttrInt
csEventAttrUInt = _cspace.csEventAttrUInt
csEventAttrFloat = _cspace.csEventAttrFloat
csEventAttrDatabuffer = _cspace.csEventAttrDatabuffer
csEventAttrEvent = _cspace.csEventAttrEvent
csEventAttriBase = _cspace.csEventAttriBase
class iEvent(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iEvent, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iEvent, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_setmethods__["Name"] = _cspace.iEvent_Name_set
    __swig_getmethods__["Name"] = _cspace.iEvent_Name_get
    if _newclass:Name = property(_cspace.iEvent_Name_get, _cspace.iEvent_Name_set)
    def GetName(*args): return _cspace.iEvent_GetName(*args)
    __swig_setmethods__["Time"] = _cspace.iEvent_Time_set
    __swig_getmethods__["Time"] = _cspace.iEvent_Time_get
    if _newclass:Time = property(_cspace.iEvent_Time_get, _cspace.iEvent_Time_set)
    __swig_setmethods__["Broadcast"] = _cspace.iEvent_Broadcast_set
    __swig_getmethods__["Broadcast"] = _cspace.iEvent_Broadcast_get
    if _newclass:Broadcast = property(_cspace.iEvent_Broadcast_get, _cspace.iEvent_Broadcast_set)
    def AddInt8(*args): return _cspace.iEvent_AddInt8(*args)
    def AddUInt8(*args): return _cspace.iEvent_AddUInt8(*args)
    def AddInt16(*args): return _cspace.iEvent_AddInt16(*args)
    def AddUInt16(*args): return _cspace.iEvent_AddUInt16(*args)
    def AddInt32(*args): return _cspace.iEvent_AddInt32(*args)
    def AddUInt32(*args): return _cspace.iEvent_AddUInt32(*args)
    def AddFloat(*args): return _cspace.iEvent_AddFloat(*args)
    def AddDouble(*args): return _cspace.iEvent_AddDouble(*args)
    def AddBool(*args): return _cspace.iEvent_AddBool(*args)
    def Add(*args): return _cspace.iEvent_Add(*args)
    def RetrieveInt8(*args): return _cspace.iEvent_RetrieveInt8(*args)
    def RetrieveUInt8(*args): return _cspace.iEvent_RetrieveUInt8(*args)
    def RetrieveInt16(*args): return _cspace.iEvent_RetrieveInt16(*args)
    def RetrieveUInt16(*args): return _cspace.iEvent_RetrieveUInt16(*args)
    def RetrieveInt32(*args): return _cspace.iEvent_RetrieveInt32(*args)
    def RetrieveUInt32(*args): return _cspace.iEvent_RetrieveUInt32(*args)
    def RetrieveFloat(*args): return _cspace.iEvent_RetrieveFloat(*args)
    def RetrieveDouble(*args): return _cspace.iEvent_RetrieveDouble(*args)
    def RetrieveBool(*args): return _cspace.iEvent_RetrieveBool(*args)
    def Retrieve(*args): return _cspace.iEvent_Retrieve(*args)
    def AttributeExists(*args): return _cspace.iEvent_AttributeExists(*args)
    def GetAttributeType(*args): return _cspace.iEvent_GetAttributeType(*args)
    def Remove(*args): return _cspace.iEvent_Remove(*args)
    def RemoveAll(*args): return _cspace.iEvent_RemoveAll(*args)
    def GetAttributeIterator(*args): return _cspace.iEvent_GetAttributeIterator(*args)
    def RetrieveString(*args): return _cspace.iEvent_RetrieveString(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iEvent_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iEvent_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iEvent
    __del__ = lambda self : None;
    __swig_getmethods__["Mouse"] = _cspace.iEvent_Mouse_get
    if _newclass:Mouse = property(_cspace.iEvent_Mouse_get)
    __swig_getmethods__["Joystick"] = _cspace.iEvent_Joystick_get
    if _newclass:Joystick = property(_cspace.iEvent_Joystick_get)
    __swig_getmethods__["Command"] = _cspace.iEvent_Command_get
    if _newclass:Command = property(_cspace.iEvent_Command_get)
iEvent_swigregister = _cspace.iEvent_swigregister
iEvent_swigregister(iEvent)
iEvent_scfGetVersion = _cspace.iEvent_scfGetVersion

class iEventPlug(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iEventPlug, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iEventPlug, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetPotentiallyConflictingEvents(*args): return _cspace.iEventPlug_GetPotentiallyConflictingEvents(*args)
    def QueryEventPriority(*args): return _cspace.iEventPlug_QueryEventPriority(*args)
    def EnableEvents(*args): return _cspace.iEventPlug_EnableEvents(*args)
    __swig_destroy__ = _cspace.delete_iEventPlug
    __del__ = lambda self : None;
iEventPlug_swigregister = _cspace.iEventPlug_swigregister
iEventPlug_swigregister(iEventPlug)

class iEventOutlet(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iEventOutlet, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iEventOutlet, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CreateEvent(*args): return _cspace.iEventOutlet_CreateEvent(*args)
    def Post(*args): return _cspace.iEventOutlet_Post(*args)
    def Key(*args): return _cspace.iEventOutlet_Key(*args)
    def Mouse(*args): return _cspace.iEventOutlet_Mouse(*args)
    def Joystick(*args): return _cspace.iEventOutlet_Joystick(*args)
    def Broadcast(*args): return _cspace.iEventOutlet_Broadcast(*args)
    def ImmediateBroadcast(*args): return _cspace.iEventOutlet_ImmediateBroadcast(*args)
    __swig_destroy__ = _cspace.delete_iEventOutlet
    __del__ = lambda self : None;
iEventOutlet_swigregister = _cspace.iEventOutlet_swigregister
iEventOutlet_swigregister(iEventOutlet)

class iEventCord(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iEventCord, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iEventCord, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Insert(*args): return _cspace.iEventCord_Insert(*args)
    def Remove(*args): return _cspace.iEventCord_Remove(*args)
    def GetPass(*args): return _cspace.iEventCord_GetPass(*args)
    def SetPass(*args): return _cspace.iEventCord_SetPass(*args)
    def GetName(*args): return _cspace.iEventCord_GetName(*args)
    __swig_destroy__ = _cspace.delete_iEventCord
    __del__ = lambda self : None;
iEventCord_swigregister = _cspace.iEventCord_swigregister
iEventCord_swigregister(iEventCord)

class csKeyEventHelper(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csKeyEventHelper, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csKeyEventHelper, name)
    __repr__ = _swig_repr
    __swig_getmethods__["GetRawCode"] = lambda x: _cspace.csKeyEventHelper_GetRawCode
    if _newclass:GetRawCode = staticmethod(_cspace.csKeyEventHelper_GetRawCode)
    __swig_getmethods__["GetCookedCode"] = lambda x: _cspace.csKeyEventHelper_GetCookedCode
    if _newclass:GetCookedCode = staticmethod(_cspace.csKeyEventHelper_GetCookedCode)
    __swig_getmethods__["GetEventType"] = lambda x: _cspace.csKeyEventHelper_GetEventType
    if _newclass:GetEventType = staticmethod(_cspace.csKeyEventHelper_GetEventType)
    __swig_getmethods__["GetAutoRepeat"] = lambda x: _cspace.csKeyEventHelper_GetAutoRepeat
    if _newclass:GetAutoRepeat = staticmethod(_cspace.csKeyEventHelper_GetAutoRepeat)
    __swig_getmethods__["GetCharacterType"] = lambda x: _cspace.csKeyEventHelper_GetCharacterType
    if _newclass:GetCharacterType = staticmethod(_cspace.csKeyEventHelper_GetCharacterType)
    __swig_getmethods__["GetEventData"] = lambda x: _cspace.csKeyEventHelper_GetEventData
    if _newclass:GetEventData = staticmethod(_cspace.csKeyEventHelper_GetEventData)
    __swig_getmethods__["GetModifiersBits"] = lambda x: _cspace.csKeyEventHelper_GetModifiersBits
    if _newclass:GetModifiersBits = staticmethod(_cspace.csKeyEventHelper_GetModifiersBits)
    __swig_getmethods__["GetModifiers"] = lambda x: _cspace.csKeyEventHelper_GetModifiers
    if _newclass:GetModifiers = staticmethod(_cspace.csKeyEventHelper_GetModifiers)
    def __init__(self, *args): 
        this = _cspace.new_csKeyEventHelper(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csKeyEventHelper
    __del__ = lambda self : None;
csKeyEventHelper_swigregister = _cspace.csKeyEventHelper_swigregister
csKeyEventHelper_swigregister(csKeyEventHelper)
csKeyEventHelper_GetRawCode = _cspace.csKeyEventHelper_GetRawCode
csKeyEventHelper_GetCookedCode = _cspace.csKeyEventHelper_GetCookedCode
csKeyEventHelper_GetEventType = _cspace.csKeyEventHelper_GetEventType
csKeyEventHelper_GetAutoRepeat = _cspace.csKeyEventHelper_GetAutoRepeat
csKeyEventHelper_GetCharacterType = _cspace.csKeyEventHelper_GetCharacterType
csKeyEventHelper_GetEventData = _cspace.csKeyEventHelper_GetEventData
csKeyEventHelper_GetModifiersBits = _cspace.csKeyEventHelper_GetModifiersBits
csKeyEventHelper_GetModifiers = _cspace.csKeyEventHelper_GetModifiers

class csMouseEventHelper(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csMouseEventHelper, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csMouseEventHelper, name)
    __repr__ = _swig_repr
    __swig_getmethods__["NewEvent"] = lambda x: _cspace.csMouseEventHelper_NewEvent
    if _newclass:NewEvent = staticmethod(_cspace.csMouseEventHelper_NewEvent)
    __swig_getmethods__["GetEventType"] = lambda x: _cspace.csMouseEventHelper_GetEventType
    if _newclass:GetEventType = staticmethod(_cspace.csMouseEventHelper_GetEventType)
    __swig_getmethods__["GetNumber"] = lambda x: _cspace.csMouseEventHelper_GetNumber
    if _newclass:GetNumber = staticmethod(_cspace.csMouseEventHelper_GetNumber)
    __swig_getmethods__["GetX"] = lambda x: _cspace.csMouseEventHelper_GetX
    if _newclass:GetX = staticmethod(_cspace.csMouseEventHelper_GetX)
    __swig_getmethods__["GetY"] = lambda x: _cspace.csMouseEventHelper_GetY
    if _newclass:GetY = staticmethod(_cspace.csMouseEventHelper_GetY)
    __swig_getmethods__["GetAxis"] = lambda x: _cspace.csMouseEventHelper_GetAxis
    if _newclass:GetAxis = staticmethod(_cspace.csMouseEventHelper_GetAxis)
    __swig_getmethods__["GetNumAxes"] = lambda x: _cspace.csMouseEventHelper_GetNumAxes
    if _newclass:GetNumAxes = staticmethod(_cspace.csMouseEventHelper_GetNumAxes)
    __swig_getmethods__["GetButton"] = lambda x: _cspace.csMouseEventHelper_GetButton
    if _newclass:GetButton = staticmethod(_cspace.csMouseEventHelper_GetButton)
    __swig_getmethods__["GetButtonState"] = lambda x: _cspace.csMouseEventHelper_GetButtonState
    if _newclass:GetButtonState = staticmethod(_cspace.csMouseEventHelper_GetButtonState)
    __swig_getmethods__["GetButtonMask"] = lambda x: _cspace.csMouseEventHelper_GetButtonMask
    if _newclass:GetButtonMask = staticmethod(_cspace.csMouseEventHelper_GetButtonMask)
    __swig_getmethods__["GetModifiers"] = lambda x: _cspace.csMouseEventHelper_GetModifiers
    if _newclass:GetModifiers = staticmethod(_cspace.csMouseEventHelper_GetModifiers)
    __swig_getmethods__["GetEventData"] = lambda x: _cspace.csMouseEventHelper_GetEventData
    if _newclass:GetEventData = staticmethod(_cspace.csMouseEventHelper_GetEventData)
    def __init__(self, *args): 
        this = _cspace.new_csMouseEventHelper(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csMouseEventHelper
    __del__ = lambda self : None;
csMouseEventHelper_swigregister = _cspace.csMouseEventHelper_swigregister
csMouseEventHelper_swigregister(csMouseEventHelper)
csMouseEventHelper_NewEvent = _cspace.csMouseEventHelper_NewEvent
csMouseEventHelper_GetEventType = _cspace.csMouseEventHelper_GetEventType
csMouseEventHelper_GetNumber = _cspace.csMouseEventHelper_GetNumber
csMouseEventHelper_GetX = _cspace.csMouseEventHelper_GetX
csMouseEventHelper_GetY = _cspace.csMouseEventHelper_GetY
csMouseEventHelper_GetAxis = _cspace.csMouseEventHelper_GetAxis
csMouseEventHelper_GetNumAxes = _cspace.csMouseEventHelper_GetNumAxes
csMouseEventHelper_GetButton = _cspace.csMouseEventHelper_GetButton
csMouseEventHelper_GetButtonState = _cspace.csMouseEventHelper_GetButtonState
csMouseEventHelper_GetButtonMask = _cspace.csMouseEventHelper_GetButtonMask
csMouseEventHelper_GetModifiers = _cspace.csMouseEventHelper_GetModifiers
csMouseEventHelper_GetEventData = _cspace.csMouseEventHelper_GetEventData

class csJoystickEventHelper(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csJoystickEventHelper, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csJoystickEventHelper, name)
    __repr__ = _swig_repr
    __swig_getmethods__["NewEvent"] = lambda x: _cspace.csJoystickEventHelper_NewEvent
    if _newclass:NewEvent = staticmethod(_cspace.csJoystickEventHelper_NewEvent)
    __swig_getmethods__["GetNumber"] = lambda x: _cspace.csJoystickEventHelper_GetNumber
    if _newclass:GetNumber = staticmethod(_cspace.csJoystickEventHelper_GetNumber)
    __swig_getmethods__["GetAxis"] = lambda x: _cspace.csJoystickEventHelper_GetAxis
    if _newclass:GetAxis = staticmethod(_cspace.csJoystickEventHelper_GetAxis)
    __swig_getmethods__["GetNumAxes"] = lambda x: _cspace.csJoystickEventHelper_GetNumAxes
    if _newclass:GetNumAxes = staticmethod(_cspace.csJoystickEventHelper_GetNumAxes)
    __swig_getmethods__["GetButton"] = lambda x: _cspace.csJoystickEventHelper_GetButton
    if _newclass:GetButton = staticmethod(_cspace.csJoystickEventHelper_GetButton)
    __swig_getmethods__["GetButtonState"] = lambda x: _cspace.csJoystickEventHelper_GetButtonState
    if _newclass:GetButtonState = staticmethod(_cspace.csJoystickEventHelper_GetButtonState)
    __swig_getmethods__["GetButtonMask"] = lambda x: _cspace.csJoystickEventHelper_GetButtonMask
    if _newclass:GetButtonMask = staticmethod(_cspace.csJoystickEventHelper_GetButtonMask)
    __swig_getmethods__["GetModifiers"] = lambda x: _cspace.csJoystickEventHelper_GetModifiers
    if _newclass:GetModifiers = staticmethod(_cspace.csJoystickEventHelper_GetModifiers)
    __swig_getmethods__["GetEventData"] = lambda x: _cspace.csJoystickEventHelper_GetEventData
    if _newclass:GetEventData = staticmethod(_cspace.csJoystickEventHelper_GetEventData)
    def __init__(self, *args): 
        this = _cspace.new_csJoystickEventHelper(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csJoystickEventHelper
    __del__ = lambda self : None;
csJoystickEventHelper_swigregister = _cspace.csJoystickEventHelper_swigregister
csJoystickEventHelper_swigregister(csJoystickEventHelper)
csJoystickEventHelper_NewEvent = _cspace.csJoystickEventHelper_NewEvent
csJoystickEventHelper_GetNumber = _cspace.csJoystickEventHelper_GetNumber
csJoystickEventHelper_GetAxis = _cspace.csJoystickEventHelper_GetAxis
csJoystickEventHelper_GetNumAxes = _cspace.csJoystickEventHelper_GetNumAxes
csJoystickEventHelper_GetButton = _cspace.csJoystickEventHelper_GetButton
csJoystickEventHelper_GetButtonState = _cspace.csJoystickEventHelper_GetButtonState
csJoystickEventHelper_GetButtonMask = _cspace.csJoystickEventHelper_GetButtonMask
csJoystickEventHelper_GetModifiers = _cspace.csJoystickEventHelper_GetModifiers
csJoystickEventHelper_GetEventData = _cspace.csJoystickEventHelper_GetEventData

class csInputEventHelper(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csInputEventHelper, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csInputEventHelper, name)
    __repr__ = _swig_repr
    __swig_getmethods__["GetButton"] = lambda x: _cspace.csInputEventHelper_GetButton
    if _newclass:GetButton = staticmethod(_cspace.csInputEventHelper_GetButton)
    __swig_getmethods__["GetButtonState"] = lambda x: _cspace.csInputEventHelper_GetButtonState
    if _newclass:GetButtonState = staticmethod(_cspace.csInputEventHelper_GetButtonState)
    def __init__(self, *args): 
        this = _cspace.new_csInputEventHelper(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csInputEventHelper
    __del__ = lambda self : None;
csInputEventHelper_swigregister = _cspace.csInputEventHelper_swigregister
csInputEventHelper_swigregister(csInputEventHelper)
csInputEventHelper_GetButton = _cspace.csInputEventHelper_GetButton
csInputEventHelper_GetButtonState = _cspace.csInputEventHelper_GetButtonState

class csCommandEventHelper(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csCommandEventHelper, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csCommandEventHelper, name)
    __repr__ = _swig_repr
    __swig_getmethods__["NewEvent"] = lambda x: _cspace.csCommandEventHelper_NewEvent
    if _newclass:NewEvent = staticmethod(_cspace.csCommandEventHelper_NewEvent)
    __swig_getmethods__["GetCode"] = lambda x: _cspace.csCommandEventHelper_GetCode
    if _newclass:GetCode = staticmethod(_cspace.csCommandEventHelper_GetCode)
    __swig_getmethods__["GetInfo"] = lambda x: _cspace.csCommandEventHelper_GetInfo
    if _newclass:GetInfo = staticmethod(_cspace.csCommandEventHelper_GetInfo)
    __swig_getmethods__["GetEventData"] = lambda x: _cspace.csCommandEventHelper_GetEventData
    if _newclass:GetEventData = staticmethod(_cspace.csCommandEventHelper_GetEventData)
    def __init__(self, *args): 
        this = _cspace.new_csCommandEventHelper(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csCommandEventHelper
    __del__ = lambda self : None;
csCommandEventHelper_swigregister = _cspace.csCommandEventHelper_swigregister
csCommandEventHelper_swigregister(csCommandEventHelper)
csCommandEventHelper_NewEvent = _cspace.csCommandEventHelper_NewEvent
csCommandEventHelper_GetCode = _cspace.csCommandEventHelper_GetCode
csCommandEventHelper_GetInfo = _cspace.csCommandEventHelper_GetInfo
csCommandEventHelper_GetEventData = _cspace.csCommandEventHelper_GetEventData

RemoveWeakListener = _cspace.RemoveWeakListener
csKeyEventTypeUp = _cspace.csKeyEventTypeUp
csKeyEventTypeDown = _cspace.csKeyEventTypeDown
csMouseEventTypeMove = _cspace.csMouseEventTypeMove
csMouseEventTypeUp = _cspace.csMouseEventTypeUp
csMouseEventTypeDown = _cspace.csMouseEventTypeDown
csMouseEventTypeClick = _cspace.csMouseEventTypeClick
csMouseEventTypeDoubleClick = _cspace.csMouseEventTypeDoubleClick
csmbNone = _cspace.csmbNone
csmbLeft = _cspace.csmbLeft
csmbRight = _cspace.csmbRight
csmbMiddle = _cspace.csmbMiddle
csmbWheelUp = _cspace.csmbWheelUp
csmbWheelDown = _cspace.csmbWheelDown
csmbExtra1 = _cspace.csmbExtra1
csmbExtra2 = _cspace.csmbExtra2
csKeyModifierTypeShift = _cspace.csKeyModifierTypeShift
csKeyModifierTypeCtrl = _cspace.csKeyModifierTypeCtrl
csKeyModifierTypeAlt = _cspace.csKeyModifierTypeAlt
csKeyModifierTypeCapsLock = _cspace.csKeyModifierTypeCapsLock
csKeyModifierTypeNumLock = _cspace.csKeyModifierTypeNumLock
csKeyModifierTypeScrollLock = _cspace.csKeyModifierTypeScrollLock
csKeyModifierTypeLast = _cspace.csKeyModifierTypeLast
csKeyModifierNumLeft = _cspace.csKeyModifierNumLeft
csKeyModifierNumRight = _cspace.csKeyModifierNumRight
csKeyModifierNumAny = _cspace.csKeyModifierNumAny
class csKeyModifiers(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csKeyModifiers, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csKeyModifiers, name)
    __repr__ = _swig_repr
    __swig_setmethods__["modifiers"] = _cspace.csKeyModifiers_modifiers_set
    __swig_getmethods__["modifiers"] = _cspace.csKeyModifiers_modifiers_get
    if _newclass:modifiers = property(_cspace.csKeyModifiers_modifiers_get, _cspace.csKeyModifiers_modifiers_set)
    def __getitem__(*args): return _cspace.csKeyModifiers___getitem__(*args)
    def __init__(self, *args): 
        this = _cspace.new_csKeyModifiers(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csKeyModifiers
    __del__ = lambda self : None;
csKeyModifiers_swigregister = _cspace.csKeyModifiers_swigregister
csKeyModifiers_swigregister(csKeyModifiers)
RegisterWeakListener = _cspace.RegisterWeakListener

CSKEY_ESC = _cspace.CSKEY_ESC
CSKEY_ENTER = _cspace.CSKEY_ENTER
CSKEY_TAB = _cspace.CSKEY_TAB
CSKEY_BACKSPACE = _cspace.CSKEY_BACKSPACE
CSKEY_SPACE = _cspace.CSKEY_SPACE
CSKEY_SPECIAL_FIRST = _cspace.CSKEY_SPECIAL_FIRST
CSKEY_SPECIAL_LAST = _cspace.CSKEY_SPECIAL_LAST
CSKEY_UP = _cspace.CSKEY_UP
CSKEY_DOWN = _cspace.CSKEY_DOWN
CSKEY_LEFT = _cspace.CSKEY_LEFT
CSKEY_RIGHT = _cspace.CSKEY_RIGHT
CSKEY_PGUP = _cspace.CSKEY_PGUP
CSKEY_PGDN = _cspace.CSKEY_PGDN
CSKEY_HOME = _cspace.CSKEY_HOME
CSKEY_END = _cspace.CSKEY_END
CSKEY_INS = _cspace.CSKEY_INS
CSKEY_DEL = _cspace.CSKEY_DEL
CSKEY_CONTEXT = _cspace.CSKEY_CONTEXT
CSKEY_PRINTSCREEN = _cspace.CSKEY_PRINTSCREEN
CSKEY_PAUSE = _cspace.CSKEY_PAUSE
CSKEY_F1 = _cspace.CSKEY_F1
CSKEY_F2 = _cspace.CSKEY_F2
CSKEY_F3 = _cspace.CSKEY_F3
CSKEY_F4 = _cspace.CSKEY_F4
CSKEY_F5 = _cspace.CSKEY_F5
CSKEY_F6 = _cspace.CSKEY_F6
CSKEY_F7 = _cspace.CSKEY_F7
CSKEY_F8 = _cspace.CSKEY_F8
CSKEY_F9 = _cspace.CSKEY_F9
CSKEY_F10 = _cspace.CSKEY_F10
CSKEY_F11 = _cspace.CSKEY_F11
CSKEY_F12 = _cspace.CSKEY_F12
CSKEY_MODIFIER_FIRST = _cspace.CSKEY_MODIFIER_FIRST
CSKEY_MODIFIER_LAST = _cspace.CSKEY_MODIFIER_LAST
CSKEY_MODIFIERTYPE_SHIFT = _cspace.CSKEY_MODIFIERTYPE_SHIFT
CSKEY_PAD_FLAG = _cspace.CSKEY_PAD_FLAG
csKeyCharTypeNormal = _cspace.csKeyCharTypeNormal
csKeyCharTypeDead = _cspace.csKeyCharTypeDead
CSEVTYPE_Keyboard = _cspace.CSEVTYPE_Keyboard
CSEVTYPE_Mouse = _cspace.CSEVTYPE_Mouse
CSEVTYPE_Joystick = _cspace.CSEVTYPE_Joystick
class iEventQueue(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iEventQueue, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iEventQueue, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Process(*args): return _cspace.iEventQueue_Process(*args)
    def Dispatch(*args): return _cspace.iEventQueue_Dispatch(*args)
    def Subscribe(*args): return _cspace.iEventQueue_Subscribe(*args)
    def Unsubscribe(*args): return _cspace.iEventQueue_Unsubscribe(*args)
    def RegisterListener(*args): return _cspace.iEventQueue_RegisterListener(*args)
    def RemoveListener(*args): return _cspace.iEventQueue_RemoveListener(*args)
    def CreateEventOutlet(*args): return _cspace.iEventQueue_CreateEventOutlet(*args)
    def GetEventOutlet(*args): return _cspace.iEventQueue_GetEventOutlet(*args)
    def GetEventCord(*args): return _cspace.iEventQueue_GetEventCord(*args)
    def CreateEvent(*args): return _cspace.iEventQueue_CreateEvent(*args)
    def CreateBroadcastEvent(*args): return _cspace.iEventQueue_CreateBroadcastEvent(*args)
    def Post(*args): return _cspace.iEventQueue_Post(*args)
    def Get(*args): return _cspace.iEventQueue_Get(*args)
    def Clear(*args): return _cspace.iEventQueue_Clear(*args)
    def IsEmpty(*args): return _cspace.iEventQueue_IsEmpty(*args)
    def RemoveAllListeners(*args): return _cspace.iEventQueue_RemoveAllListeners(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iEventQueue_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iEventQueue_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iEventQueue
    __del__ = lambda self : None;
iEventQueue_swigregister = _cspace.iEventQueue_swigregister
iEventQueue_swigregister(iEventQueue)
iEventQueue_scfGetVersion = _cspace.iEventQueue_scfGetVersion

class iEventNameRegistry(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iEventNameRegistry, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iEventNameRegistry, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetID(*args): return _cspace.iEventNameRegistry_GetID(*args)
    def GetString(*args): return _cspace.iEventNameRegistry_GetString(*args)
    def GetParentID(*args): return _cspace.iEventNameRegistry_GetParentID(*args)
    def IsImmediateChildOf(*args): return _cspace.iEventNameRegistry_IsImmediateChildOf(*args)
    def IsKindOf(*args): return _cspace.iEventNameRegistry_IsKindOf(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iEventNameRegistry_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iEventNameRegistry_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iEventNameRegistry
    __del__ = lambda self : None;
iEventNameRegistry_swigregister = _cspace.iEventNameRegistry_swigregister
iEventNameRegistry_swigregister(iEventNameRegistry)
csHashCompute = _cspace.csHashCompute
iEventNameRegistry_scfGetVersion = _cspace.iEventNameRegistry_scfGetVersion

class csEventNameRegistry(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csEventNameRegistry, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csEventNameRegistry, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_csEventNameRegistry
    __del__ = lambda self : None;
    __swig_getmethods__["GetString"] = lambda x: _cspace.csEventNameRegistry_GetString
    if _newclass:GetString = staticmethod(_cspace.csEventNameRegistry_GetString)
    def GetParentID(*args): return _cspace.csEventNameRegistry_GetParentID(*args)
    def IsImmediateChildOf(*args): return _cspace.csEventNameRegistry_IsImmediateChildOf(*args)
    __swig_getmethods__["GetRegistry"] = lambda x: _cspace.csEventNameRegistry_GetRegistry
    if _newclass:GetRegistry = staticmethod(_cspace.csEventNameRegistry_GetRegistry)
    __swig_getmethods__["GetID"] = lambda x: _cspace.csEventNameRegistry_GetID
    if _newclass:GetID = staticmethod(_cspace.csEventNameRegistry_GetID)
    __swig_getmethods__["IsKindOf"] = lambda x: _cspace.csEventNameRegistry_IsKindOf
    if _newclass:IsKindOf = staticmethod(_cspace.csEventNameRegistry_IsKindOf)
csEventNameRegistry_swigregister = _cspace.csEventNameRegistry_swigregister
csEventNameRegistry_swigregister(csEventNameRegistry)
csEventNameRegistry_GetString = _cspace.csEventNameRegistry_GetString
csEventNameRegistry_GetRegistry = _cspace.csEventNameRegistry_GetRegistry
csEventNameRegistry_GetID = _cspace.csEventNameRegistry_GetID
csEventNameRegistry_IsKindOf = _cspace.csEventNameRegistry_IsKindOf

class iEventHandler(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iEventHandler, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iEventHandler, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def HandleEvent(*args): return _cspace.iEventHandler_HandleEvent(*args)
    def GenericName(*args): return _cspace.iEventHandler_GenericName(*args)
    def GenericID(*args): return _cspace.iEventHandler_GenericID(*args)
    def GenericPrec(*args): return _cspace.iEventHandler_GenericPrec(*args)
    def GenericSucc(*args): return _cspace.iEventHandler_GenericSucc(*args)
    def InstancePrec(*args): return _cspace.iEventHandler_InstancePrec(*args)
    def InstanceSucc(*args): return _cspace.iEventHandler_InstanceSucc(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iEventHandler_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iEventHandler_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iEventHandler
    __del__ = lambda self : None;
iEventHandler_swigregister = _cspace.iEventHandler_swigregister
iEventHandler_swigregister(iEventHandler)
csevMouse = _cspace.csevMouse
csevMouseOp = _cspace.csevMouseOp
csevJoystick = _cspace.csevJoystick
csevJoystickOp = _cspace.csevJoystickOp
csevCanvasOp = _cspace.csevCanvasOp
csevPreProcess = _cspace.csevPreProcess
csevProcess = _cspace.csevProcess
csevPostProcess = _cspace.csevPostProcess
csevFinalProcess = _cspace.csevFinalProcess
iEventHandler_scfGetVersion = _cspace.iEventHandler_scfGetVersion

class iPluginIterator(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iPluginIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iPluginIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def HasNext(*args): return _cspace.iPluginIterator_HasNext(*args)
    def Next(*args): return _cspace.iPluginIterator_Next(*args)
    __swig_destroy__ = _cspace.delete_iPluginIterator
    __del__ = lambda self : None;
iPluginIterator_swigregister = _cspace.iPluginIterator_swigregister
iPluginIterator_swigregister(iPluginIterator)

class iPluginManager(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iPluginManager, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iPluginManager, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def LoadPlugin(*args): return _cspace.iPluginManager_LoadPlugin(*args)
    def QueryPlugin(*args): return _cspace.iPluginManager_QueryPlugin(*args)
    def UnloadPlugin(*args): return _cspace.iPluginManager_UnloadPlugin(*args)
    def RegisterPlugin(*args): return _cspace.iPluginManager_RegisterPlugin(*args)
    def GetPlugins(*args): return _cspace.iPluginManager_GetPlugins(*args)
    def Clear(*args): return _cspace.iPluginManager_Clear(*args)
    def QueryOptions(*args): return _cspace.iPluginManager_QueryOptions(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iPluginManager_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iPluginManager_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iPluginManager
    __del__ = lambda self : None;
iPluginManager_swigregister = _cspace.iPluginManager_swigregister
iPluginManager_swigregister(iPluginManager)
iPluginManager_scfGetVersion = _cspace.iPluginManager_scfGetVersion

csLoadPluginAlways = _cspace.csLoadPluginAlways
csComposeNoChar = _cspace.csComposeNoChar
csComposeNormalChar = _cspace.csComposeNormalChar
csComposeComposedChar = _cspace.csComposeComposedChar
csComposeUncomposeable = _cspace.csComposeUncomposeable
class iKeyComposer(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iKeyComposer, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iKeyComposer, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def HandleKey(*args): return _cspace.iKeyComposer_HandleKey(*args)
    def ResetState(*args): return _cspace.iKeyComposer_ResetState(*args)
    __swig_destroy__ = _cspace.delete_iKeyComposer
    __del__ = lambda self : None;
iKeyComposer_swigregister = _cspace.iKeyComposer_swigregister
iKeyComposer_swigregister(iKeyComposer)

class iKeyboardDriver(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iKeyboardDriver, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iKeyboardDriver, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Reset(*args): return _cspace.iKeyboardDriver_Reset(*args)
    def DoKey(*args): return _cspace.iKeyboardDriver_DoKey(*args)
    def GetModifierState(*args): return _cspace.iKeyboardDriver_GetModifierState(*args)
    def CreateKeyComposer(*args): return _cspace.iKeyboardDriver_CreateKeyComposer(*args)
    def SynthesizeCooked(*args): return _cspace.iKeyboardDriver_SynthesizeCooked(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iKeyboardDriver_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iKeyboardDriver_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iKeyboardDriver
    __del__ = lambda self : None;
    def GetKeyState(*args): return _cspace.iKeyboardDriver_GetKeyState(*args)
iKeyboardDriver_swigregister = _cspace.iKeyboardDriver_swigregister
iKeyboardDriver_swigregister(iKeyboardDriver)
iKeyboardDriver_scfGetVersion = _cspace.iKeyboardDriver_scfGetVersion

class iMouseDriver(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMouseDriver, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMouseDriver, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetDoubleClickTime(*args): return _cspace.iMouseDriver_SetDoubleClickTime(*args)
    def Reset(*args): return _cspace.iMouseDriver_Reset(*args)
    def GetLastX(*args): return _cspace.iMouseDriver_GetLastX(*args)
    def GetLastY(*args): return _cspace.iMouseDriver_GetLastY(*args)
    def GetLast(*args): return _cspace.iMouseDriver_GetLast(*args)
    def GetLastButton(*args): return _cspace.iMouseDriver_GetLastButton(*args)
    def DoButton(*args): return _cspace.iMouseDriver_DoButton(*args)
    def DoMotion(*args): return _cspace.iMouseDriver_DoMotion(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iMouseDriver_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iMouseDriver_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iMouseDriver
    __del__ = lambda self : None;
iMouseDriver_swigregister = _cspace.iMouseDriver_swigregister
iMouseDriver_swigregister(iMouseDriver)
iMouseDriver_scfGetVersion = _cspace.iMouseDriver_scfGetVersion

class iJoystickDriver(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iJoystickDriver, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iJoystickDriver, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Reset(*args): return _cspace.iJoystickDriver_Reset(*args)
    def GetLast(*args): return _cspace.iJoystickDriver_GetLast(*args)
    def GetLastButton(*args): return _cspace.iJoystickDriver_GetLastButton(*args)
    def DoButton(*args): return _cspace.iJoystickDriver_DoButton(*args)
    def DoMotion(*args): return _cspace.iJoystickDriver_DoMotion(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iJoystickDriver_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iJoystickDriver_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iJoystickDriver
    __del__ = lambda self : None;
iJoystickDriver_swigregister = _cspace.iJoystickDriver_swigregister
iJoystickDriver_swigregister(iJoystickDriver)
iJoystickDriver_scfGetVersion = _cspace.iJoystickDriver_scfGetVersion

class iConfigFile(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iConfigFile, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iConfigFile, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetFileName(*args): return _cspace.iConfigFile_GetFileName(*args)
    def GetVFS(*args): return _cspace.iConfigFile_GetVFS(*args)
    def SetFileName(*args): return _cspace.iConfigFile_SetFileName(*args)
    def Load(*args): return _cspace.iConfigFile_Load(*args)
    def Save(*args): return _cspace.iConfigFile_Save(*args)
    def Clear(*args): return _cspace.iConfigFile_Clear(*args)
    def Enumerate(*args): return _cspace.iConfigFile_Enumerate(*args)
    def KeyExists(*args): return _cspace.iConfigFile_KeyExists(*args)
    def SubsectionExists(*args): return _cspace.iConfigFile_SubsectionExists(*args)
    def GetInt(*args): return _cspace.iConfigFile_GetInt(*args)
    def GetFloat(*args): return _cspace.iConfigFile_GetFloat(*args)
    def GetStr(*args): return _cspace.iConfigFile_GetStr(*args)
    def GetBool(*args): return _cspace.iConfigFile_GetBool(*args)
    def GetTuple(*args): return _cspace.iConfigFile_GetTuple(*args)
    def GetComment(*args): return _cspace.iConfigFile_GetComment(*args)
    def SetStr(*args): return _cspace.iConfigFile_SetStr(*args)
    def SetInt(*args): return _cspace.iConfigFile_SetInt(*args)
    def SetFloat(*args): return _cspace.iConfigFile_SetFloat(*args)
    def SetBool(*args): return _cspace.iConfigFile_SetBool(*args)
    def SetTuple(*args): return _cspace.iConfigFile_SetTuple(*args)
    def SetComment(*args): return _cspace.iConfigFile_SetComment(*args)
    def DeleteKey(*args): return _cspace.iConfigFile_DeleteKey(*args)
    def GetEOFComment(*args): return _cspace.iConfigFile_GetEOFComment(*args)
    def SetEOFComment(*args): return _cspace.iConfigFile_SetEOFComment(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iConfigFile_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iConfigFile_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iConfigFile
    __del__ = lambda self : None;
iConfigFile_swigregister = _cspace.iConfigFile_swigregister
iConfigFile_swigregister(iConfigFile)
iConfigFile_scfGetVersion = _cspace.iConfigFile_scfGetVersion

class iConfigIterator(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iConfigIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iConfigIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetConfigFile(*args): return _cspace.iConfigIterator_GetConfigFile(*args)
    def GetSubsection(*args): return _cspace.iConfigIterator_GetSubsection(*args)
    def Rewind(*args): return _cspace.iConfigIterator_Rewind(*args)
    def Next(*args): return _cspace.iConfigIterator_Next(*args)
    def HasNext(*args): return _cspace.iConfigIterator_HasNext(*args)
    def GetKey(*args): return _cspace.iConfigIterator_GetKey(*args)
    def GetInt(*args): return _cspace.iConfigIterator_GetInt(*args)
    def GetFloat(*args): return _cspace.iConfigIterator_GetFloat(*args)
    def GetStr(*args): return _cspace.iConfigIterator_GetStr(*args)
    def GetBool(*args): return _cspace.iConfigIterator_GetBool(*args)
    def GetTuple(*args): return _cspace.iConfigIterator_GetTuple(*args)
    def GetComment(*args): return _cspace.iConfigIterator_GetComment(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iConfigIterator_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iConfigIterator_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iConfigIterator
    __del__ = lambda self : None;
iConfigIterator_swigregister = _cspace.iConfigIterator_swigregister
iConfigIterator_swigregister(iConfigIterator)
iConfigIterator_scfGetVersion = _cspace.iConfigIterator_scfGetVersion

class iConfigManager(iConfigFile):
    __swig_setmethods__ = {}
    for _s in [iConfigFile]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iConfigManager, name, value)
    __swig_getmethods__ = {}
    for _s in [iConfigFile]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iConfigManager, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    PriorityMin = _cspace.iConfigManager_PriorityMin
    PriorityVeryLow = _cspace.iConfigManager_PriorityVeryLow
    PriorityLow = _cspace.iConfigManager_PriorityLow
    PriorityMedium = _cspace.iConfigManager_PriorityMedium
    PriorityHigh = _cspace.iConfigManager_PriorityHigh
    PriorityVeryHigh = _cspace.iConfigManager_PriorityVeryHigh
    PriorityMax = _cspace.iConfigManager_PriorityMax
    ConfigPriorityPlugin = _cspace.iConfigManager_ConfigPriorityPlugin
    ConfigPriorityApplication = _cspace.iConfigManager_ConfigPriorityApplication
    ConfigPriorityUserGlobal = _cspace.iConfigManager_ConfigPriorityUserGlobal
    ConfigPriorityUserApp = _cspace.iConfigManager_ConfigPriorityUserApp
    ConfigPriorityCmdLine = _cspace.iConfigManager_ConfigPriorityCmdLine
    def AddDomain(*args): return _cspace.iConfigManager_AddDomain(*args)
    def RemoveDomain(*args): return _cspace.iConfigManager_RemoveDomain(*args)
    def LookupDomain(*args): return _cspace.iConfigManager_LookupDomain(*args)
    def SetDomainPriority(*args): return _cspace.iConfigManager_SetDomainPriority(*args)
    def GetDomainPriority(*args): return _cspace.iConfigManager_GetDomainPriority(*args)
    def SetDynamicDomain(*args): return _cspace.iConfigManager_SetDynamicDomain(*args)
    def GetDynamicDomain(*args): return _cspace.iConfigManager_GetDynamicDomain(*args)
    def SetDynamicDomainPriority(*args): return _cspace.iConfigManager_SetDynamicDomainPriority(*args)
    def GetDynamicDomainPriority(*args): return _cspace.iConfigManager_GetDynamicDomainPriority(*args)
    def FlushRemoved(*args): return _cspace.iConfigManager_FlushRemoved(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iConfigManager_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iConfigManager_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iConfigManager
    __del__ = lambda self : None;
iConfigManager_swigregister = _cspace.iConfigManager_swigregister
iConfigManager_swigregister(iConfigManager)
iConfigManager_scfGetVersion = _cspace.iConfigManager_scfGetVersion

class iStringArray(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iStringArray, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iStringArray, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetSize(*args): return _cspace.iStringArray_GetSize(*args)
    def Length(*args): return _cspace.iStringArray_Length(*args)
    def Push(*args): return _cspace.iStringArray_Push(*args)
    def Pop(*args): return _cspace.iStringArray_Pop(*args)
    def Get(*args): return _cspace.iStringArray_Get(*args)
    def Find(*args): return _cspace.iStringArray_Find(*args)
    def FindCaseInsensitive(*args): return _cspace.iStringArray_FindCaseInsensitive(*args)
    def FindSortedKey(*args): return _cspace.iStringArray_FindSortedKey(*args)
    def Contains(*args): return _cspace.iStringArray_Contains(*args)
    def Sort(*args): return _cspace.iStringArray_Sort(*args)
    def DeleteIndex(*args): return _cspace.iStringArray_DeleteIndex(*args)
    def Insert(*args): return _cspace.iStringArray_Insert(*args)
    def Empty(*args): return _cspace.iStringArray_Empty(*args)
    def DeleteAll(*args): return _cspace.iStringArray_DeleteAll(*args)
    def IsEmpty(*args): return _cspace.iStringArray_IsEmpty(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iStringArray_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iStringArray_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iStringArray
    __del__ = lambda self : None;
iStringArray_swigregister = _cspace.iStringArray_swigregister
iStringArray_swigregister(iStringArray)
iStringArray_scfGetVersion = _cspace.iStringArray_scfGetVersion

CS_NODE_DOCUMENT = _cspace.CS_NODE_DOCUMENT
CS_NODE_ELEMENT = _cspace.CS_NODE_ELEMENT
CS_NODE_COMMENT = _cspace.CS_NODE_COMMENT
CS_NODE_UNKNOWN = _cspace.CS_NODE_UNKNOWN
CS_NODE_TEXT = _cspace.CS_NODE_TEXT
CS_NODE_DECLARATION = _cspace.CS_NODE_DECLARATION
CS_CHANGEABLE_NEVER = _cspace.CS_CHANGEABLE_NEVER
CS_CHANGEABLE_NEWROOT = _cspace.CS_CHANGEABLE_NEWROOT
CS_CHANGEABLE_YES = _cspace.CS_CHANGEABLE_YES
class iDocumentAttributeIterator(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDocumentAttributeIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDocumentAttributeIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def HasNext(*args): return _cspace.iDocumentAttributeIterator_HasNext(*args)
    def Next(*args): return _cspace.iDocumentAttributeIterator_Next(*args)
    def __iter__(self):
        while self.HasNext():
            yield self.Next() 
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iDocumentAttributeIterator_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iDocumentAttributeIterator_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iDocumentAttributeIterator
    __del__ = lambda self : None;
iDocumentAttributeIterator_swigregister = _cspace.iDocumentAttributeIterator_swigregister
iDocumentAttributeIterator_swigregister(iDocumentAttributeIterator)
iDocumentAttributeIterator_scfGetVersion = _cspace.iDocumentAttributeIterator_scfGetVersion

class iDocumentAttribute(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDocumentAttribute, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDocumentAttribute, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetName(*args): return _cspace.iDocumentAttribute_GetName(*args)
    def GetValue(*args): return _cspace.iDocumentAttribute_GetValue(*args)
    def GetValueAsInt(*args): return _cspace.iDocumentAttribute_GetValueAsInt(*args)
    def GetValueAsFloat(*args): return _cspace.iDocumentAttribute_GetValueAsFloat(*args)
    def GetValueAsBool(*args): return _cspace.iDocumentAttribute_GetValueAsBool(*args)
    def SetName(*args): return _cspace.iDocumentAttribute_SetName(*args)
    def SetValue(*args): return _cspace.iDocumentAttribute_SetValue(*args)
    def SetValueAsInt(*args): return _cspace.iDocumentAttribute_SetValueAsInt(*args)
    def SetValueAsFloat(*args): return _cspace.iDocumentAttribute_SetValueAsFloat(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iDocumentAttribute_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iDocumentAttribute_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iDocumentAttribute
    __del__ = lambda self : None;
iDocumentAttribute_swigregister = _cspace.iDocumentAttribute_swigregister
iDocumentAttribute_swigregister(iDocumentAttribute)
iDocumentAttribute_scfGetVersion = _cspace.iDocumentAttribute_scfGetVersion

class iDocumentNodeIterator(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDocumentNodeIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDocumentNodeIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def HasNext(*args): return _cspace.iDocumentNodeIterator_HasNext(*args)
    def Next(*args): return _cspace.iDocumentNodeIterator_Next(*args)
    def GetNextPosition(*args): return _cspace.iDocumentNodeIterator_GetNextPosition(*args)
    def GetEndPosition(*args): return _cspace.iDocumentNodeIterator_GetEndPosition(*args)
    def __iter__(self):
        while self.HasNext():
            yield self.Next() 
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iDocumentNodeIterator_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iDocumentNodeIterator_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iDocumentNodeIterator
    __del__ = lambda self : None;
iDocumentNodeIterator_swigregister = _cspace.iDocumentNodeIterator_swigregister
iDocumentNodeIterator_swigregister(iDocumentNodeIterator)
iDocumentNodeIterator_scfGetVersion = _cspace.iDocumentNodeIterator_scfGetVersion

class iDocumentNode(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDocumentNode, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDocumentNode, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetType(*args): return _cspace.iDocumentNode_GetType(*args)
    def Equals(*args): return _cspace.iDocumentNode_Equals(*args)
    def GetValue(*args): return _cspace.iDocumentNode_GetValue(*args)
    def SetValue(*args): return _cspace.iDocumentNode_SetValue(*args)
    def SetValueAsInt(*args): return _cspace.iDocumentNode_SetValueAsInt(*args)
    def SetValueAsFloat(*args): return _cspace.iDocumentNode_SetValueAsFloat(*args)
    def GetParent(*args): return _cspace.iDocumentNode_GetParent(*args)
    def GetNodes(*args): return _cspace.iDocumentNode_GetNodes(*args)
    def GetNode(*args): return _cspace.iDocumentNode_GetNode(*args)
    def RemoveNode(*args): return _cspace.iDocumentNode_RemoveNode(*args)
    def RemoveNodes(*args): return _cspace.iDocumentNode_RemoveNodes(*args)
    def CreateNodeBefore(*args): return _cspace.iDocumentNode_CreateNodeBefore(*args)
    def GetContentsValue(*args): return _cspace.iDocumentNode_GetContentsValue(*args)
    def GetContentsValueAsInt(*args): return _cspace.iDocumentNode_GetContentsValueAsInt(*args)
    def GetContentsValueAsFloat(*args): return _cspace.iDocumentNode_GetContentsValueAsFloat(*args)
    def GetAttributes(*args): return _cspace.iDocumentNode_GetAttributes(*args)
    def GetAttribute(*args): return _cspace.iDocumentNode_GetAttribute(*args)
    def GetAttributeValue(*args): return _cspace.iDocumentNode_GetAttributeValue(*args)
    def GetAttributeValueAsInt(*args): return _cspace.iDocumentNode_GetAttributeValueAsInt(*args)
    def GetAttributeValueAsFloat(*args): return _cspace.iDocumentNode_GetAttributeValueAsFloat(*args)
    def GetAttributeValueAsBool(*args): return _cspace.iDocumentNode_GetAttributeValueAsBool(*args)
    def RemoveAttribute(*args): return _cspace.iDocumentNode_RemoveAttribute(*args)
    def RemoveAttributes(*args): return _cspace.iDocumentNode_RemoveAttributes(*args)
    def SetAttribute(*args): return _cspace.iDocumentNode_SetAttribute(*args)
    def SetAttributeAsInt(*args): return _cspace.iDocumentNode_SetAttributeAsInt(*args)
    def SetAttributeAsFloat(*args): return _cspace.iDocumentNode_SetAttributeAsFloat(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iDocumentNode_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iDocumentNode_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iDocumentNode
    __del__ = lambda self : None;
iDocumentNode_swigregister = _cspace.iDocumentNode_swigregister
iDocumentNode_swigregister(iDocumentNode)
iDocumentNode_scfGetVersion = _cspace.iDocumentNode_scfGetVersion

class iDocument(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDocument, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDocument, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Clear(*args): return _cspace.iDocument_Clear(*args)
    def CreateRoot(*args): return _cspace.iDocument_CreateRoot(*args)
    def GetRoot(*args): return _cspace.iDocument_GetRoot(*args)
    def Parse(*args): return _cspace.iDocument_Parse(*args)
    def Write(*args): return _cspace.iDocument_Write(*args)
    def Changeable(*args): return _cspace.iDocument_Changeable(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iDocument_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iDocument_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iDocument
    __del__ = lambda self : None;
iDocument_swigregister = _cspace.iDocument_swigregister
iDocument_swigregister(iDocument)
iDocument_scfGetVersion = _cspace.iDocument_scfGetVersion

class iDocumentSystem(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDocumentSystem, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDocumentSystem, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CreateDocument(*args): return _cspace.iDocumentSystem_CreateDocument(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iDocumentSystem_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iDocumentSystem_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iDocumentSystem
    __del__ = lambda self : None;
iDocumentSystem_swigregister = _cspace.iDocumentSystem_swigregister
iDocumentSystem_swigregister(iDocumentSystem)
iDocumentSystem_scfGetVersion = _cspace.iDocumentSystem_scfGetVersion

class scfConfigFile(iConfigFile):
    __swig_setmethods__ = {}
    for _s in [iConfigFile]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, scfConfigFile, name, value)
    __swig_getmethods__ = {}
    for _s in [iConfigFile]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, scfConfigFile, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def IncRef(*args): return _cspace.scfConfigFile_IncRef(*args)
    def DecRef(*args): return _cspace.scfConfigFile_DecRef(*args)
    def GetRefCount(*args): return _cspace.scfConfigFile_GetRefCount(*args)
    def QueryInterface(*args): return _cspace.scfConfigFile_QueryInterface(*args)
    def AddRefOwner(*args): return _cspace.scfConfigFile_AddRefOwner(*args)
    def RemoveRefOwner(*args): return _cspace.scfConfigFile_RemoveRefOwner(*args)
    def GetInterfaceMetadata(*args): return _cspace.scfConfigFile_GetInterfaceMetadata(*args)
scfConfigFile_swigregister = _cspace.scfConfigFile_swigregister
scfConfigFile_swigregister(scfConfigFile)

class csConfigFile(scfConfigFile):
    __swig_setmethods__ = {}
    for _s in [scfConfigFile]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csConfigFile, name, value)
    __swig_getmethods__ = {}
    for _s in [scfConfigFile]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csConfigFile, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csConfigFile(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csConfigFile
    __del__ = lambda self : None;
    def IsEmpty(*args): return _cspace.csConfigFile_IsEmpty(*args)
    def GetFileName(*args): return _cspace.csConfigFile_GetFileName(*args)
    def GetVFS(*args): return _cspace.csConfigFile_GetVFS(*args)
    def SetFileName(*args): return _cspace.csConfigFile_SetFileName(*args)
    def Load(*args): return _cspace.csConfigFile_Load(*args)
    def LoadFromBuffer(*args): return _cspace.csConfigFile_LoadFromBuffer(*args)
    def Save(*args): return _cspace.csConfigFile_Save(*args)
    def Clear(*args): return _cspace.csConfigFile_Clear(*args)
    def Enumerate(*args): return _cspace.csConfigFile_Enumerate(*args)
    def KeyExists(*args): return _cspace.csConfigFile_KeyExists(*args)
    def SubsectionExists(*args): return _cspace.csConfigFile_SubsectionExists(*args)
    def GetInt(*args): return _cspace.csConfigFile_GetInt(*args)
    def GetFloat(*args): return _cspace.csConfigFile_GetFloat(*args)
    def GetStr(*args): return _cspace.csConfigFile_GetStr(*args)
    def GetBool(*args): return _cspace.csConfigFile_GetBool(*args)
    def GetTuple(*args): return _cspace.csConfigFile_GetTuple(*args)
    def GetComment(*args): return _cspace.csConfigFile_GetComment(*args)
    def SetStr(*args): return _cspace.csConfigFile_SetStr(*args)
    def SetInt(*args): return _cspace.csConfigFile_SetInt(*args)
    def SetFloat(*args): return _cspace.csConfigFile_SetFloat(*args)
    def SetBool(*args): return _cspace.csConfigFile_SetBool(*args)
    def SetTuple(*args): return _cspace.csConfigFile_SetTuple(*args)
    def SetComment(*args): return _cspace.csConfigFile_SetComment(*args)
    def DeleteKey(*args): return _cspace.csConfigFile_DeleteKey(*args)
    def SetEOFComment(*args): return _cspace.csConfigFile_SetEOFComment(*args)
    def GetEOFComment(*args): return _cspace.csConfigFile_GetEOFComment(*args)
    def ParseCommandLine(*args): return _cspace.csConfigFile_ParseCommandLine(*args)
csConfigFile_swigregister = _cspace.csConfigFile_swigregister
csConfigFile_swigregister(csConfigFile)

class csRadixSorter(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csRadixSorter, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csRadixSorter, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csRadixSorter(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csRadixSorter
    __del__ = lambda self : None;
    def Sort(*args): return _cspace.csRadixSorter_Sort(*args)
    def GetRanks(*args): return _cspace.csRadixSorter_GetRanks(*args)
csRadixSorter_swigregister = _cspace.csRadixSorter_swigregister
csRadixSorter_swigregister(csRadixSorter)

class csTinyDocumentSystem(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csTinyDocumentSystem, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csTinyDocumentSystem, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csTinyDocumentSystem(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csTinyDocumentSystem
    __del__ = lambda self : None;
    def CreateDocument(*args): return _cspace.csTinyDocumentSystem_CreateDocument(*args)
csTinyDocumentSystem_swigregister = _cspace.csTinyDocumentSystem_swigregister
csTinyDocumentSystem_swigregister(csTinyDocumentSystem)

class iDataBuffer(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDataBuffer, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDataBuffer, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetSize(*args): return _cspace.iDataBuffer_GetSize(*args)
    def GetData(*args): return _cspace.iDataBuffer_GetData(*args)
    def asString(*args): return _cspace.iDataBuffer_asString(*args)
    def GetUint8(*args): return _cspace.iDataBuffer_GetUint8(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iDataBuffer_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iDataBuffer_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iDataBuffer
    __del__ = lambda self : None;
iDataBuffer_swigregister = _cspace.iDataBuffer_swigregister
iDataBuffer_swigregister(iDataBuffer)
iDataBuffer_scfGetVersion = _cspace.iDataBuffer_scfGetVersion

CS_WRITE_BASELINE = _cspace.CS_WRITE_BASELINE
CS_WRITE_NOANTIALIAS = _cspace.CS_WRITE_NOANTIALIAS
class csPixelCoord(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPixelCoord, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csPixelCoord, name)
    __repr__ = _swig_repr
    __swig_setmethods__["x"] = _cspace.csPixelCoord_x_set
    __swig_getmethods__["x"] = _cspace.csPixelCoord_x_get
    if _newclass:x = property(_cspace.csPixelCoord_x_get, _cspace.csPixelCoord_x_set)
    __swig_setmethods__["y"] = _cspace.csPixelCoord_y_set
    __swig_getmethods__["y"] = _cspace.csPixelCoord_y_get
    if _newclass:y = property(_cspace.csPixelCoord_y_get, _cspace.csPixelCoord_y_set)
    def __init__(self, *args): 
        this = _cspace.new_csPixelCoord(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csPixelCoord
    __del__ = lambda self : None;
csPixelCoord_swigregister = _cspace.csPixelCoord_swigregister
csPixelCoord_swigregister(csPixelCoord)

class csPixelFormat(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPixelFormat, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csPixelFormat, name)
    __repr__ = _swig_repr
    __swig_setmethods__["RedMask"] = _cspace.csPixelFormat_RedMask_set
    __swig_getmethods__["RedMask"] = _cspace.csPixelFormat_RedMask_get
    if _newclass:RedMask = property(_cspace.csPixelFormat_RedMask_get, _cspace.csPixelFormat_RedMask_set)
    __swig_setmethods__["GreenMask"] = _cspace.csPixelFormat_GreenMask_set
    __swig_getmethods__["GreenMask"] = _cspace.csPixelFormat_GreenMask_get
    if _newclass:GreenMask = property(_cspace.csPixelFormat_GreenMask_get, _cspace.csPixelFormat_GreenMask_set)
    __swig_setmethods__["BlueMask"] = _cspace.csPixelFormat_BlueMask_set
    __swig_getmethods__["BlueMask"] = _cspace.csPixelFormat_BlueMask_get
    if _newclass:BlueMask = property(_cspace.csPixelFormat_BlueMask_get, _cspace.csPixelFormat_BlueMask_set)
    __swig_setmethods__["AlphaMask"] = _cspace.csPixelFormat_AlphaMask_set
    __swig_getmethods__["AlphaMask"] = _cspace.csPixelFormat_AlphaMask_get
    if _newclass:AlphaMask = property(_cspace.csPixelFormat_AlphaMask_get, _cspace.csPixelFormat_AlphaMask_set)
    __swig_setmethods__["RedShift"] = _cspace.csPixelFormat_RedShift_set
    __swig_getmethods__["RedShift"] = _cspace.csPixelFormat_RedShift_get
    if _newclass:RedShift = property(_cspace.csPixelFormat_RedShift_get, _cspace.csPixelFormat_RedShift_set)
    __swig_setmethods__["GreenShift"] = _cspace.csPixelFormat_GreenShift_set
    __swig_getmethods__["GreenShift"] = _cspace.csPixelFormat_GreenShift_get
    if _newclass:GreenShift = property(_cspace.csPixelFormat_GreenShift_get, _cspace.csPixelFormat_GreenShift_set)
    __swig_setmethods__["BlueShift"] = _cspace.csPixelFormat_BlueShift_set
    __swig_getmethods__["BlueShift"] = _cspace.csPixelFormat_BlueShift_get
    if _newclass:BlueShift = property(_cspace.csPixelFormat_BlueShift_get, _cspace.csPixelFormat_BlueShift_set)
    __swig_setmethods__["AlphaShift"] = _cspace.csPixelFormat_AlphaShift_set
    __swig_getmethods__["AlphaShift"] = _cspace.csPixelFormat_AlphaShift_get
    if _newclass:AlphaShift = property(_cspace.csPixelFormat_AlphaShift_get, _cspace.csPixelFormat_AlphaShift_set)
    __swig_setmethods__["RedBits"] = _cspace.csPixelFormat_RedBits_set
    __swig_getmethods__["RedBits"] = _cspace.csPixelFormat_RedBits_get
    if _newclass:RedBits = property(_cspace.csPixelFormat_RedBits_get, _cspace.csPixelFormat_RedBits_set)
    __swig_setmethods__["GreenBits"] = _cspace.csPixelFormat_GreenBits_set
    __swig_getmethods__["GreenBits"] = _cspace.csPixelFormat_GreenBits_get
    if _newclass:GreenBits = property(_cspace.csPixelFormat_GreenBits_get, _cspace.csPixelFormat_GreenBits_set)
    __swig_setmethods__["BlueBits"] = _cspace.csPixelFormat_BlueBits_set
    __swig_getmethods__["BlueBits"] = _cspace.csPixelFormat_BlueBits_get
    if _newclass:BlueBits = property(_cspace.csPixelFormat_BlueBits_get, _cspace.csPixelFormat_BlueBits_set)
    __swig_setmethods__["AlphaBits"] = _cspace.csPixelFormat_AlphaBits_set
    __swig_getmethods__["AlphaBits"] = _cspace.csPixelFormat_AlphaBits_get
    if _newclass:AlphaBits = property(_cspace.csPixelFormat_AlphaBits_get, _cspace.csPixelFormat_AlphaBits_set)
    __swig_setmethods__["PalEntries"] = _cspace.csPixelFormat_PalEntries_set
    __swig_getmethods__["PalEntries"] = _cspace.csPixelFormat_PalEntries_get
    if _newclass:PalEntries = property(_cspace.csPixelFormat_PalEntries_get, _cspace.csPixelFormat_PalEntries_set)
    __swig_setmethods__["PixelBytes"] = _cspace.csPixelFormat_PixelBytes_set
    __swig_getmethods__["PixelBytes"] = _cspace.csPixelFormat_PixelBytes_get
    if _newclass:PixelBytes = property(_cspace.csPixelFormat_PixelBytes_get, _cspace.csPixelFormat_PixelBytes_set)
    def complete(*args): return _cspace.csPixelFormat_complete(*args)
    def __init__(self, *args): 
        this = _cspace.new_csPixelFormat(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csPixelFormat
    __del__ = lambda self : None;
csPixelFormat_swigregister = _cspace.csPixelFormat_swigregister
csPixelFormat_swigregister(csPixelFormat)

class csImageArea(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csImageArea, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csImageArea, name)
    __repr__ = _swig_repr
    __swig_setmethods__["x"] = _cspace.csImageArea_x_set
    __swig_getmethods__["x"] = _cspace.csImageArea_x_get
    if _newclass:x = property(_cspace.csImageArea_x_get, _cspace.csImageArea_x_set)
    __swig_setmethods__["y"] = _cspace.csImageArea_y_set
    __swig_getmethods__["y"] = _cspace.csImageArea_y_get
    if _newclass:y = property(_cspace.csImageArea_y_get, _cspace.csImageArea_y_set)
    __swig_setmethods__["w"] = _cspace.csImageArea_w_set
    __swig_getmethods__["w"] = _cspace.csImageArea_w_get
    if _newclass:w = property(_cspace.csImageArea_w_get, _cspace.csImageArea_w_set)
    __swig_setmethods__["h"] = _cspace.csImageArea_h_set
    __swig_getmethods__["h"] = _cspace.csImageArea_h_get
    if _newclass:h = property(_cspace.csImageArea_h_get, _cspace.csImageArea_h_set)
    __swig_setmethods__["data"] = _cspace.csImageArea_data_set
    __swig_getmethods__["data"] = _cspace.csImageArea_data_get
    if _newclass:data = property(_cspace.csImageArea_data_get, _cspace.csImageArea_data_set)
    def __init__(self, *args): 
        this = _cspace.new_csImageArea(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csImageArea
    __del__ = lambda self : None;
csImageArea_swigregister = _cspace.csImageArea_swigregister
csImageArea_swigregister(csImageArea)

class iOffscreenCanvasCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iOffscreenCanvasCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iOffscreenCanvasCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def FinishDraw(*args): return _cspace.iOffscreenCanvasCallback_FinishDraw(*args)
    def SetRGB(*args): return _cspace.iOffscreenCanvasCallback_SetRGB(*args)
    __swig_destroy__ = _cspace.delete_iOffscreenCanvasCallback
    __del__ = lambda self : None;
iOffscreenCanvasCallback_swigregister = _cspace.iOffscreenCanvasCallback_swigregister
iOffscreenCanvasCallback_swigregister(iOffscreenCanvasCallback)

class iGraphics2D(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iGraphics2D, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iGraphics2D, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Open(*args): return _cspace.iGraphics2D_Open(*args)
    def Close(*args): return _cspace.iGraphics2D_Close(*args)
    def GetWidth(*args): return _cspace.iGraphics2D_GetWidth(*args)
    def GetHeight(*args): return _cspace.iGraphics2D_GetHeight(*args)
    def GetPage(*args): return _cspace.iGraphics2D_GetPage(*args)
    def DoubleBuffer(*args): return _cspace.iGraphics2D_DoubleBuffer(*args)
    def GetDoubleBufferState(*args): return _cspace.iGraphics2D_GetDoubleBufferState(*args)
    def GetPixelFormat(*args): return _cspace.iGraphics2D_GetPixelFormat(*args)
    def GetPixelBytes(*args): return _cspace.iGraphics2D_GetPixelBytes(*args)
    def GetPalEntryCount(*args): return _cspace.iGraphics2D_GetPalEntryCount(*args)
    def GetPalette(*args): return _cspace.iGraphics2D_GetPalette(*args)
    def SetRGB(*args): return _cspace.iGraphics2D_SetRGB(*args)
    def FindRGB(*args): return _cspace.iGraphics2D_FindRGB(*args)
    def GetRGB(*args): return _cspace.iGraphics2D_GetRGB(*args)
    def GetRGBA(*args): return _cspace.iGraphics2D_GetRGBA(*args)
    def SetClipRect(*args): return _cspace.iGraphics2D_SetClipRect(*args)
    def GetClipRect(*args): return _cspace.iGraphics2D_GetClipRect(*args)
    def BeginDraw(*args): return _cspace.iGraphics2D_BeginDraw(*args)
    def FinishDraw(*args): return _cspace.iGraphics2D_FinishDraw(*args)
    def Print(*args): return _cspace.iGraphics2D_Print(*args)
    def Clear(*args): return _cspace.iGraphics2D_Clear(*args)
    def ClearAll(*args): return _cspace.iGraphics2D_ClearAll(*args)
    def DrawLine(*args): return _cspace.iGraphics2D_DrawLine(*args)
    def DrawBox(*args): return _cspace.iGraphics2D_DrawBox(*args)
    def ClipLine(*args): return _cspace.iGraphics2D_ClipLine(*args)
    def DrawPixel(*args): return _cspace.iGraphics2D_DrawPixel(*args)
    def DrawPixels(*args): return _cspace.iGraphics2D_DrawPixels(*args)
    def Blit(*args): return _cspace.iGraphics2D_Blit(*args)
    def GetPixelAt(*args): return _cspace.iGraphics2D_GetPixelAt(*args)
    def GetPixel(*args): return _cspace.iGraphics2D_GetPixel(*args)
    def SaveArea(*args): return _cspace.iGraphics2D_SaveArea(*args)
    def RestoreArea(*args): return _cspace.iGraphics2D_RestoreArea(*args)
    def FreeArea(*args): return _cspace.iGraphics2D_FreeArea(*args)
    def AllowResize(*args): return _cspace.iGraphics2D_AllowResize(*args)
    def Resize(*args): return _cspace.iGraphics2D_Resize(*args)
    def GetFontServer(*args): return _cspace.iGraphics2D_GetFontServer(*args)
    def PerformExtension(*args): return _cspace.iGraphics2D_PerformExtension(*args)
    def ScreenShot(*args): return _cspace.iGraphics2D_ScreenShot(*args)
    def GetNativeWindow(*args): return _cspace.iGraphics2D_GetNativeWindow(*args)
    def GetFullScreen(*args): return _cspace.iGraphics2D_GetFullScreen(*args)
    def SetFullScreen(*args): return _cspace.iGraphics2D_SetFullScreen(*args)
    def SetMousePosition(*args): return _cspace.iGraphics2D_SetMousePosition(*args)
    def SetMouseCursor(*args): return _cspace.iGraphics2D_SetMouseCursor(*args)
    def SetGamma(*args): return _cspace.iGraphics2D_SetGamma(*args)
    def GetGamma(*args): return _cspace.iGraphics2D_GetGamma(*args)
    def GetName(*args): return _cspace.iGraphics2D_GetName(*args)
    def CreateOffscreenCanvas(*args): return _cspace.iGraphics2D_CreateOffscreenCanvas(*args)
    def Write(*args): return _cspace.iGraphics2D_Write(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iGraphics2D_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iGraphics2D_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iGraphics2D
    __del__ = lambda self : None;
    def _PerformExtension(*args): return _cspace.iGraphics2D__PerformExtension(*args)
    def PerformExtension (self, command, *args):
      self._PerformExtension(self.__class__.__name__, command, args);

iGraphics2D_swigregister = _cspace.iGraphics2D_swigregister
iGraphics2D_swigregister(iGraphics2D)
iGraphics2D_scfGetVersion = _cspace.iGraphics2D_scfGetVersion

CSDRAW_2DGRAPHICS = _cspace.CSDRAW_2DGRAPHICS
CSDRAW_3DGRAPHICS = _cspace.CSDRAW_3DGRAPHICS
CSDRAW_CLEARZBUFFER = _cspace.CSDRAW_CLEARZBUFFER
CSDRAW_CLEARSCREEN = _cspace.CSDRAW_CLEARSCREEN
CS_CLIPPER_NONE = _cspace.CS_CLIPPER_NONE
CS_CLIPPER_OPTIONAL = _cspace.CS_CLIPPER_OPTIONAL
CS_CLIPPER_TOPLEVEL = _cspace.CS_CLIPPER_TOPLEVEL
CS_CLIPPER_REQUIRED = _cspace.CS_CLIPPER_REQUIRED
CS_CLIP_NOT = _cspace.CS_CLIP_NOT
CS_CLIP_NEEDED = _cspace.CS_CLIP_NEEDED
CS_ZBUF_NONE = _cspace.CS_ZBUF_NONE
CS_ZBUF_FILL = _cspace.CS_ZBUF_FILL
CS_ZBUF_TEST = _cspace.CS_ZBUF_TEST
CS_ZBUF_USE = _cspace.CS_ZBUF_USE
CS_ZBUF_EQUAL = _cspace.CS_ZBUF_EQUAL
CS_ZBUF_INVERT = _cspace.CS_ZBUF_INVERT
CS_ZBUF_MESH = _cspace.CS_ZBUF_MESH
CS_ZBUF_MESH2 = _cspace.CS_ZBUF_MESH2
CS_VATTRIB_SPECIFIC_FIRST = _cspace.CS_VATTRIB_SPECIFIC_FIRST
CS_VATTRIB_SPECIFIC_LAST = _cspace.CS_VATTRIB_SPECIFIC_LAST
CS_VATTRIB_GENERIC_FIRST = _cspace.CS_VATTRIB_GENERIC_FIRST
CS_VATTRIB_GENERIC_LAST = _cspace.CS_VATTRIB_GENERIC_LAST
CS_VATTRIB_UNUSED = _cspace.CS_VATTRIB_UNUSED
CS_VATTRIB_INVALID = _cspace.CS_VATTRIB_INVALID
CS_VATTRIB_POSITION = _cspace.CS_VATTRIB_POSITION
CS_VATTRIB_WEIGHT = _cspace.CS_VATTRIB_WEIGHT
CS_VATTRIB_NORMAL = _cspace.CS_VATTRIB_NORMAL
CS_VATTRIB_COLOR = _cspace.CS_VATTRIB_COLOR
CS_VATTRIB_PRIMARY_COLOR = _cspace.CS_VATTRIB_PRIMARY_COLOR
CS_VATTRIB_SECONDARY_COLOR = _cspace.CS_VATTRIB_SECONDARY_COLOR
CS_VATTRIB_FOGCOORD = _cspace.CS_VATTRIB_FOGCOORD
CS_VATTRIB_TEXCOORD = _cspace.CS_VATTRIB_TEXCOORD
CS_VATTRIB_TEXCOORD0 = _cspace.CS_VATTRIB_TEXCOORD0
CS_VATTRIB_TEXCOORD1 = _cspace.CS_VATTRIB_TEXCOORD1
CS_VATTRIB_TEXCOORD2 = _cspace.CS_VATTRIB_TEXCOORD2
CS_VATTRIB_TEXCOORD3 = _cspace.CS_VATTRIB_TEXCOORD3
CS_VATTRIB_TEXCOORD4 = _cspace.CS_VATTRIB_TEXCOORD4
CS_VATTRIB_TEXCOORD5 = _cspace.CS_VATTRIB_TEXCOORD5
CS_VATTRIB_TEXCOORD6 = _cspace.CS_VATTRIB_TEXCOORD6
CS_VATTRIB_TEXCOORD7 = _cspace.CS_VATTRIB_TEXCOORD7
CS_VATTRIB_0 = _cspace.CS_VATTRIB_0
CS_VATTRIB_1 = _cspace.CS_VATTRIB_1
CS_VATTRIB_2 = _cspace.CS_VATTRIB_2
CS_VATTRIB_3 = _cspace.CS_VATTRIB_3
CS_VATTRIB_4 = _cspace.CS_VATTRIB_4
CS_VATTRIB_5 = _cspace.CS_VATTRIB_5
CS_VATTRIB_6 = _cspace.CS_VATTRIB_6
CS_VATTRIB_7 = _cspace.CS_VATTRIB_7
CS_VATTRIB_8 = _cspace.CS_VATTRIB_8
CS_VATTRIB_9 = _cspace.CS_VATTRIB_9
CS_VATTRIB_10 = _cspace.CS_VATTRIB_10
CS_VATTRIB_11 = _cspace.CS_VATTRIB_11
CS_VATTRIB_12 = _cspace.CS_VATTRIB_12
CS_VATTRIB_13 = _cspace.CS_VATTRIB_13
CS_VATTRIB_14 = _cspace.CS_VATTRIB_14
CS_VATTRIB_15 = _cspace.CS_VATTRIB_15
CS_MIXMODE_TYPE_AUTO = _cspace.CS_MIXMODE_TYPE_AUTO
CS_MIXMODE_TYPE_BLENDOP = _cspace.CS_MIXMODE_TYPE_BLENDOP
CS_MIXMODE_FLAG_BLENDOP_ALPHA = _cspace.CS_MIXMODE_FLAG_BLENDOP_ALPHA
CS_MIXMODE_TYPE_MESH = _cspace.CS_MIXMODE_TYPE_MESH
CS_MIXMODE_TYPE_MASK = _cspace.CS_MIXMODE_TYPE_MASK
CS_MIXMODE_FACT_ZERO = _cspace.CS_MIXMODE_FACT_ZERO
CS_MIXMODE_FACT_ONE = _cspace.CS_MIXMODE_FACT_ONE
CS_MIXMODE_FACT_SRCCOLOR = _cspace.CS_MIXMODE_FACT_SRCCOLOR
CS_MIXMODE_FACT_SRCCOLOR_INV = _cspace.CS_MIXMODE_FACT_SRCCOLOR_INV
CS_MIXMODE_FACT_DSTCOLOR = _cspace.CS_MIXMODE_FACT_DSTCOLOR
CS_MIXMODE_FACT_DSTCOLOR_INV = _cspace.CS_MIXMODE_FACT_DSTCOLOR_INV
CS_MIXMODE_FACT_SRCALPHA = _cspace.CS_MIXMODE_FACT_SRCALPHA
CS_MIXMODE_FACT_SRCALPHA_INV = _cspace.CS_MIXMODE_FACT_SRCALPHA_INV
CS_MIXMODE_FACT_DSTALPHA = _cspace.CS_MIXMODE_FACT_DSTALPHA
CS_MIXMODE_FACT_DSTALPHA_INV = _cspace.CS_MIXMODE_FACT_DSTALPHA_INV
CS_MIXMODE_FACT_COUNT = _cspace.CS_MIXMODE_FACT_COUNT
CS_MIXMODE_FACT_MASK = _cspace.CS_MIXMODE_FACT_MASK
CS_MIXMODE_ALPHATEST_AUTO = _cspace.CS_MIXMODE_ALPHATEST_AUTO
CS_MIXMODE_ALPHATEST_ENABLE = _cspace.CS_MIXMODE_ALPHATEST_ENABLE
CS_MIXMODE_ALPHATEST_DISABLE = _cspace.CS_MIXMODE_ALPHATEST_DISABLE
CS_MIXMODE_ALPHATEST_MASK = _cspace.CS_MIXMODE_ALPHATEST_MASK
CS_FX_COPY = _cspace.CS_FX_COPY
CS_FX_MESH = _cspace.CS_FX_MESH
CS_FX_FLAT = _cspace.CS_FX_FLAT
CS_FX_MASK_ALPHA = _cspace.CS_FX_MASK_ALPHA
CS_FX_MASK_MIXMODE = _cspace.CS_FX_MASK_MIXMODE
class csAlphaMode(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csAlphaMode, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csAlphaMode, name)
    __repr__ = _swig_repr
    alphaNone = _cspace.csAlphaMode_alphaNone
    alphaBinary = _cspace.csAlphaMode_alphaBinary
    alphaSmooth = _cspace.csAlphaMode_alphaSmooth
    __swig_setmethods__["autoAlphaMode"] = _cspace.csAlphaMode_autoAlphaMode_set
    __swig_getmethods__["autoAlphaMode"] = _cspace.csAlphaMode_autoAlphaMode_get
    if _newclass:autoAlphaMode = property(_cspace.csAlphaMode_autoAlphaMode_get, _cspace.csAlphaMode_autoAlphaMode_set)
    def __init__(self, *args): 
        this = _cspace.new_csAlphaMode(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csAlphaMode
    __del__ = lambda self : None;
csAlphaMode_swigregister = _cspace.csAlphaMode_swigregister
csAlphaMode_swigregister(csAlphaMode)

CS_LIGHTPARAM_POSITION = _cspace.CS_LIGHTPARAM_POSITION
CS_LIGHTPARAM_DIFFUSE = _cspace.CS_LIGHTPARAM_DIFFUSE
CS_LIGHTPARAM_SPECULAR = _cspace.CS_LIGHTPARAM_SPECULAR
CS_LIGHTPARAM_ATTENUATION = _cspace.CS_LIGHTPARAM_ATTENUATION
CS_SHADOW_VOLUME_BEGIN = _cspace.CS_SHADOW_VOLUME_BEGIN
CS_SHADOW_VOLUME_PASS1 = _cspace.CS_SHADOW_VOLUME_PASS1
CS_SHADOW_VOLUME_PASS2 = _cspace.CS_SHADOW_VOLUME_PASS2
CS_SHADOW_VOLUME_FAIL1 = _cspace.CS_SHADOW_VOLUME_FAIL1
CS_SHADOW_VOLUME_FAIL2 = _cspace.CS_SHADOW_VOLUME_FAIL2
CS_SHADOW_VOLUME_USE = _cspace.CS_SHADOW_VOLUME_USE
CS_SHADOW_VOLUME_FINISH = _cspace.CS_SHADOW_VOLUME_FINISH
G3DRENDERSTATE_ZBUFFERMODE = _cspace.G3DRENDERSTATE_ZBUFFERMODE
G3DRENDERSTATE_DITHERENABLE = _cspace.G3DRENDERSTATE_DITHERENABLE
G3DRENDERSTATE_BILINEARMAPPINGENABLE = _cspace.G3DRENDERSTATE_BILINEARMAPPINGENABLE
G3DRENDERSTATE_TRILINEARMAPPINGENABLE = _cspace.G3DRENDERSTATE_TRILINEARMAPPINGENABLE
G3DRENDERSTATE_TRANSPARENCYENABLE = _cspace.G3DRENDERSTATE_TRANSPARENCYENABLE
G3DRENDERSTATE_MIPMAPENABLE = _cspace.G3DRENDERSTATE_MIPMAPENABLE
G3DRENDERSTATE_TEXTUREMAPPINGENABLE = _cspace.G3DRENDERSTATE_TEXTUREMAPPINGENABLE
G3DRENDERSTATE_LIGHTINGENABLE = _cspace.G3DRENDERSTATE_LIGHTINGENABLE
G3DRENDERSTATE_INTERLACINGENABLE = _cspace.G3DRENDERSTATE_INTERLACINGENABLE
G3DRENDERSTATE_MMXENABLE = _cspace.G3DRENDERSTATE_MMXENABLE
G3DRENDERSTATE_INTERPOLATIONSTEP = _cspace.G3DRENDERSTATE_INTERPOLATIONSTEP
G3DRENDERSTATE_MAXPOLYGONSTODRAW = _cspace.G3DRENDERSTATE_MAXPOLYGONSTODRAW
G3DRENDERSTATE_GOURAUDENABLE = _cspace.G3DRENDERSTATE_GOURAUDENABLE
G3DRENDERSTATE_EDGES = _cspace.G3DRENDERSTATE_EDGES
class csGraphics3DCaps(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csGraphics3DCaps, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csGraphics3DCaps, name)
    __repr__ = _swig_repr
    __swig_setmethods__["minTexHeight"] = _cspace.csGraphics3DCaps_minTexHeight_set
    __swig_getmethods__["minTexHeight"] = _cspace.csGraphics3DCaps_minTexHeight_get
    if _newclass:minTexHeight = property(_cspace.csGraphics3DCaps_minTexHeight_get, _cspace.csGraphics3DCaps_minTexHeight_set)
    __swig_setmethods__["minTexWidth"] = _cspace.csGraphics3DCaps_minTexWidth_set
    __swig_getmethods__["minTexWidth"] = _cspace.csGraphics3DCaps_minTexWidth_get
    if _newclass:minTexWidth = property(_cspace.csGraphics3DCaps_minTexWidth_get, _cspace.csGraphics3DCaps_minTexWidth_set)
    __swig_setmethods__["maxTexHeight"] = _cspace.csGraphics3DCaps_maxTexHeight_set
    __swig_getmethods__["maxTexHeight"] = _cspace.csGraphics3DCaps_maxTexHeight_get
    if _newclass:maxTexHeight = property(_cspace.csGraphics3DCaps_maxTexHeight_get, _cspace.csGraphics3DCaps_maxTexHeight_set)
    __swig_setmethods__["maxTexWidth"] = _cspace.csGraphics3DCaps_maxTexWidth_set
    __swig_getmethods__["maxTexWidth"] = _cspace.csGraphics3DCaps_maxTexWidth_get
    if _newclass:maxTexWidth = property(_cspace.csGraphics3DCaps_maxTexWidth_get, _cspace.csGraphics3DCaps_maxTexWidth_set)
    __swig_setmethods__["SupportsPointSprites"] = _cspace.csGraphics3DCaps_SupportsPointSprites_set
    __swig_getmethods__["SupportsPointSprites"] = _cspace.csGraphics3DCaps_SupportsPointSprites_get
    if _newclass:SupportsPointSprites = property(_cspace.csGraphics3DCaps_SupportsPointSprites_get, _cspace.csGraphics3DCaps_SupportsPointSprites_set)
    __swig_setmethods__["DestinationAlpha"] = _cspace.csGraphics3DCaps_DestinationAlpha_set
    __swig_getmethods__["DestinationAlpha"] = _cspace.csGraphics3DCaps_DestinationAlpha_get
    if _newclass:DestinationAlpha = property(_cspace.csGraphics3DCaps_DestinationAlpha_get, _cspace.csGraphics3DCaps_DestinationAlpha_set)
    __swig_setmethods__["StencilShadows"] = _cspace.csGraphics3DCaps_StencilShadows_set
    __swig_getmethods__["StencilShadows"] = _cspace.csGraphics3DCaps_StencilShadows_get
    if _newclass:StencilShadows = property(_cspace.csGraphics3DCaps_StencilShadows_get, _cspace.csGraphics3DCaps_StencilShadows_set)
    def __init__(self, *args): 
        this = _cspace.new_csGraphics3DCaps(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csGraphics3DCaps
    __del__ = lambda self : None;
csGraphics3DCaps_swigregister = _cspace.csGraphics3DCaps_swigregister
csGraphics3DCaps_swigregister(csGraphics3DCaps)

CS_MESHTYPE_TRIANGLES = _cspace.CS_MESHTYPE_TRIANGLES
CS_MESHTYPE_QUADS = _cspace.CS_MESHTYPE_QUADS
CS_MESHTYPE_TRIANGLESTRIP = _cspace.CS_MESHTYPE_TRIANGLESTRIP
CS_MESHTYPE_TRIANGLEFAN = _cspace.CS_MESHTYPE_TRIANGLEFAN
CS_MESHTYPE_POINTS = _cspace.CS_MESHTYPE_POINTS
CS_MESHTYPE_POINT_SPRITES = _cspace.CS_MESHTYPE_POINT_SPRITES
CS_MESHTYPE_LINES = _cspace.CS_MESHTYPE_LINES
CS_MESHTYPE_LINESTRIP = _cspace.CS_MESHTYPE_LINESTRIP
csSimpleMeshScreenspace = _cspace.csSimpleMeshScreenspace
CS_OPENPORTAL_ZFILL = _cspace.CS_OPENPORTAL_ZFILL
CS_OPENPORTAL_MIRROR = _cspace.CS_OPENPORTAL_MIRROR
CS_OPENPORTAL_FLOAT = _cspace.CS_OPENPORTAL_FLOAT
class csSimpleRenderMesh(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSimpleRenderMesh, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csSimpleRenderMesh, name)
    __repr__ = _swig_repr
    __swig_setmethods__["meshtype"] = _cspace.csSimpleRenderMesh_meshtype_set
    __swig_getmethods__["meshtype"] = _cspace.csSimpleRenderMesh_meshtype_get
    if _newclass:meshtype = property(_cspace.csSimpleRenderMesh_meshtype_get, _cspace.csSimpleRenderMesh_meshtype_set)
    __swig_setmethods__["indexCount"] = _cspace.csSimpleRenderMesh_indexCount_set
    __swig_getmethods__["indexCount"] = _cspace.csSimpleRenderMesh_indexCount_get
    if _newclass:indexCount = property(_cspace.csSimpleRenderMesh_indexCount_get, _cspace.csSimpleRenderMesh_indexCount_set)
    __swig_setmethods__["indices"] = _cspace.csSimpleRenderMesh_indices_set
    __swig_getmethods__["indices"] = _cspace.csSimpleRenderMesh_indices_get
    if _newclass:indices = property(_cspace.csSimpleRenderMesh_indices_get, _cspace.csSimpleRenderMesh_indices_set)
    __swig_setmethods__["vertexCount"] = _cspace.csSimpleRenderMesh_vertexCount_set
    __swig_getmethods__["vertexCount"] = _cspace.csSimpleRenderMesh_vertexCount_get
    if _newclass:vertexCount = property(_cspace.csSimpleRenderMesh_vertexCount_get, _cspace.csSimpleRenderMesh_vertexCount_set)
    __swig_setmethods__["vertices"] = _cspace.csSimpleRenderMesh_vertices_set
    __swig_getmethods__["vertices"] = _cspace.csSimpleRenderMesh_vertices_get
    if _newclass:vertices = property(_cspace.csSimpleRenderMesh_vertices_get, _cspace.csSimpleRenderMesh_vertices_set)
    __swig_setmethods__["texcoords"] = _cspace.csSimpleRenderMesh_texcoords_set
    __swig_getmethods__["texcoords"] = _cspace.csSimpleRenderMesh_texcoords_get
    if _newclass:texcoords = property(_cspace.csSimpleRenderMesh_texcoords_get, _cspace.csSimpleRenderMesh_texcoords_set)
    __swig_setmethods__["colors"] = _cspace.csSimpleRenderMesh_colors_set
    __swig_getmethods__["colors"] = _cspace.csSimpleRenderMesh_colors_get
    if _newclass:colors = property(_cspace.csSimpleRenderMesh_colors_get, _cspace.csSimpleRenderMesh_colors_set)
    __swig_setmethods__["texture"] = _cspace.csSimpleRenderMesh_texture_set
    __swig_getmethods__["texture"] = _cspace.csSimpleRenderMesh_texture_get
    if _newclass:texture = property(_cspace.csSimpleRenderMesh_texture_get, _cspace.csSimpleRenderMesh_texture_set)
    __swig_setmethods__["shader"] = _cspace.csSimpleRenderMesh_shader_set
    __swig_getmethods__["shader"] = _cspace.csSimpleRenderMesh_shader_get
    if _newclass:shader = property(_cspace.csSimpleRenderMesh_shader_get, _cspace.csSimpleRenderMesh_shader_set)
    __swig_setmethods__["dynDomain"] = _cspace.csSimpleRenderMesh_dynDomain_set
    __swig_getmethods__["dynDomain"] = _cspace.csSimpleRenderMesh_dynDomain_get
    if _newclass:dynDomain = property(_cspace.csSimpleRenderMesh_dynDomain_get, _cspace.csSimpleRenderMesh_dynDomain_set)
    __swig_setmethods__["alphaType"] = _cspace.csSimpleRenderMesh_alphaType_set
    __swig_getmethods__["alphaType"] = _cspace.csSimpleRenderMesh_alphaType_get
    if _newclass:alphaType = property(_cspace.csSimpleRenderMesh_alphaType_get, _cspace.csSimpleRenderMesh_alphaType_set)
    __swig_setmethods__["z_buf_mode"] = _cspace.csSimpleRenderMesh_z_buf_mode_set
    __swig_getmethods__["z_buf_mode"] = _cspace.csSimpleRenderMesh_z_buf_mode_get
    if _newclass:z_buf_mode = property(_cspace.csSimpleRenderMesh_z_buf_mode_get, _cspace.csSimpleRenderMesh_z_buf_mode_set)
    __swig_setmethods__["mixmode"] = _cspace.csSimpleRenderMesh_mixmode_set
    __swig_getmethods__["mixmode"] = _cspace.csSimpleRenderMesh_mixmode_get
    if _newclass:mixmode = property(_cspace.csSimpleRenderMesh_mixmode_get, _cspace.csSimpleRenderMesh_mixmode_set)
    __swig_setmethods__["object2world"] = _cspace.csSimpleRenderMesh_object2world_set
    __swig_getmethods__["object2world"] = _cspace.csSimpleRenderMesh_object2world_get
    if _newclass:object2world = property(_cspace.csSimpleRenderMesh_object2world_get, _cspace.csSimpleRenderMesh_object2world_set)
    def __init__(self, *args): 
        this = _cspace.new_csSimpleRenderMesh(*args)
        try: self.this.append(this)
        except: self.this = this
    def SetWithGenmeshFactory(*args): return _cspace.csSimpleRenderMesh_SetWithGenmeshFactory(*args)
    __swig_destroy__ = _cspace.delete_csSimpleRenderMesh
    __del__ = lambda self : None;
csSimpleRenderMesh_swigregister = _cspace.csSimpleRenderMesh_swigregister
csSimpleRenderMesh_swigregister(csSimpleRenderMesh)

class iGraphics3D(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iGraphics3D, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iGraphics3D, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Open(*args): return _cspace.iGraphics3D_Open(*args)
    def Close(*args): return _cspace.iGraphics3D_Close(*args)
    def GetDriver2D(*args): return _cspace.iGraphics3D_GetDriver2D(*args)
    def GetTextureManager(*args): return _cspace.iGraphics3D_GetTextureManager(*args)
    def SetDimensions(*args): return _cspace.iGraphics3D_SetDimensions(*args)
    def GetWidth(*args): return _cspace.iGraphics3D_GetWidth(*args)
    def GetHeight(*args): return _cspace.iGraphics3D_GetHeight(*args)
    def GetCaps(*args): return _cspace.iGraphics3D_GetCaps(*args)
    def SetPerspectiveCenter(*args): return _cspace.iGraphics3D_SetPerspectiveCenter(*args)
    def GetPerspectiveCenter(*args): return _cspace.iGraphics3D_GetPerspectiveCenter(*args)
    def SetPerspectiveAspect(*args): return _cspace.iGraphics3D_SetPerspectiveAspect(*args)
    def GetPerspectiveAspect(*args): return _cspace.iGraphics3D_GetPerspectiveAspect(*args)
    def SetRenderTarget(*args): return _cspace.iGraphics3D_SetRenderTarget(*args)
    def GetRenderTarget(*args): return _cspace.iGraphics3D_GetRenderTarget(*args)
    def BeginDraw(*args): return _cspace.iGraphics3D_BeginDraw(*args)
    def FinishDraw(*args): return _cspace.iGraphics3D_FinishDraw(*args)
    def Print(*args): return _cspace.iGraphics3D_Print(*args)
    def DrawMesh(*args): return _cspace.iGraphics3D_DrawMesh(*args)
    def DrawSimpleMesh(*args): return _cspace.iGraphics3D_DrawSimpleMesh(*args)
    def DrawPixmap(*args): return _cspace.iGraphics3D_DrawPixmap(*args)
    def DrawLine(*args): return _cspace.iGraphics3D_DrawLine(*args)
    def ActivateBuffers(*args): return _cspace.iGraphics3D_ActivateBuffers(*args)
    def DeactivateBuffers(*args): return _cspace.iGraphics3D_DeactivateBuffers(*args)
    def SetTextureState(*args): return _cspace.iGraphics3D_SetTextureState(*args)
    def SetClipper(*args): return _cspace.iGraphics3D_SetClipper(*args)
    def GetClipper(*args): return _cspace.iGraphics3D_GetClipper(*args)
    def GetClipType(*args): return _cspace.iGraphics3D_GetClipType(*args)
    def SetNearPlane(*args): return _cspace.iGraphics3D_SetNearPlane(*args)
    def ResetNearPlane(*args): return _cspace.iGraphics3D_ResetNearPlane(*args)
    def GetNearPlane(*args): return _cspace.iGraphics3D_GetNearPlane(*args)
    def HasNearPlane(*args): return _cspace.iGraphics3D_HasNearPlane(*args)
    def SetRenderState(*args): return _cspace.iGraphics3D_SetRenderState(*args)
    def GetRenderState(*args): return _cspace.iGraphics3D_GetRenderState(*args)
    def SetOption(*args): return _cspace.iGraphics3D_SetOption(*args)
    def SetWriteMask(*args): return _cspace.iGraphics3D_SetWriteMask(*args)
    def GetWriteMask(*args): return _cspace.iGraphics3D_GetWriteMask(*args)
    def SetZMode(*args): return _cspace.iGraphics3D_SetZMode(*args)
    def GetZMode(*args): return _cspace.iGraphics3D_GetZMode(*args)
    def EnableZOffset(*args): return _cspace.iGraphics3D_EnableZOffset(*args)
    def DisableZOffset(*args): return _cspace.iGraphics3D_DisableZOffset(*args)
    def SetShadowState(*args): return _cspace.iGraphics3D_SetShadowState(*args)
    def GetZBuffValue(*args): return _cspace.iGraphics3D_GetZBuffValue(*args)
    def OpenPortal(*args): return _cspace.iGraphics3D_OpenPortal(*args)
    def ClosePortal(*args): return _cspace.iGraphics3D_ClosePortal(*args)
    def CreateHalo(*args): return _cspace.iGraphics3D_CreateHalo(*args)
    def RemoveFromCache(*args): return _cspace.iGraphics3D_RemoveFromCache(*args)
    def SetWorldToCamera(*args): return _cspace.iGraphics3D_SetWorldToCamera(*args)
    def PerformExtension(*args): return _cspace.iGraphics3D_PerformExtension(*args)
    def GetWorldToCamera(*args): return _cspace.iGraphics3D_GetWorldToCamera(*args)
    def GetCurrentDrawFlags(*args): return _cspace.iGraphics3D_GetCurrentDrawFlags(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iGraphics3D_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iGraphics3D_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iGraphics3D
    __del__ = lambda self : None;
iGraphics3D_swigregister = _cspace.iGraphics3D_swigregister
iGraphics3D_swigregister(iGraphics3D)
iGraphics3D_scfGetVersion = _cspace.iGraphics3D_scfGetVersion

csmcNone = _cspace.csmcNone
csmcArrow = _cspace.csmcArrow
csmcLens = _cspace.csmcLens
csmcCross = _cspace.csmcCross
csmcPen = _cspace.csmcPen
csmcMove = _cspace.csmcMove
csmcSizeNWSE = _cspace.csmcSizeNWSE
csmcSizeNESW = _cspace.csmcSizeNESW
csmcSizeNS = _cspace.csmcSizeNS
csmcSizeEW = _cspace.csmcSizeEW
csmcStop = _cspace.csmcStop
csmcWait = _cspace.csmcWait
CS_ALERT_ERROR = _cspace.CS_ALERT_ERROR
CS_ALERT_WARNING = _cspace.CS_ALERT_WARNING
CS_ALERT_NOTE = _cspace.CS_ALERT_NOTE
class iNativeWindowManager(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iNativeWindowManager, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iNativeWindowManager, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Alert(*args): return _cspace.iNativeWindowManager_Alert(*args)
    __swig_destroy__ = _cspace.delete_iNativeWindowManager
    __del__ = lambda self : None;
iNativeWindowManager_swigregister = _cspace.iNativeWindowManager_swigregister
iNativeWindowManager_swigregister(iNativeWindowManager)

class iNativeWindow(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iNativeWindow, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iNativeWindow, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetTitle(*args): return _cspace.iNativeWindow_SetTitle(*args)
    __swig_destroy__ = _cspace.delete_iNativeWindow
    __del__ = lambda self : None;
iNativeWindow_swigregister = _cspace.iNativeWindow_swigregister
iNativeWindow_swigregister(iNativeWindow)

CSFONT_LARGE = _cspace.CSFONT_LARGE
CSFONT_ITALIC = _cspace.CSFONT_ITALIC
CSFONT_COURIER = _cspace.CSFONT_COURIER
CSFONT_SMALL = _cspace.CSFONT_SMALL
CS_FONT_DEFAULT_GLYPH = _cspace.CS_FONT_DEFAULT_GLYPH
class iFontDeleteNotify(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iFontDeleteNotify, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iFontDeleteNotify, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def BeforeDelete(*args): return _cspace.iFontDeleteNotify_BeforeDelete(*args)
    __swig_destroy__ = _cspace.delete_iFontDeleteNotify
    __del__ = lambda self : None;
iFontDeleteNotify_swigregister = _cspace.iFontDeleteNotify_swigregister
iFontDeleteNotify_swigregister(iFontDeleteNotify)

class csBitmapMetrics(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csBitmapMetrics, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csBitmapMetrics, name)
    __repr__ = _swig_repr
    __swig_setmethods__["width"] = _cspace.csBitmapMetrics_width_set
    __swig_getmethods__["width"] = _cspace.csBitmapMetrics_width_get
    if _newclass:width = property(_cspace.csBitmapMetrics_width_get, _cspace.csBitmapMetrics_width_set)
    __swig_setmethods__["height"] = _cspace.csBitmapMetrics_height_set
    __swig_getmethods__["height"] = _cspace.csBitmapMetrics_height_get
    if _newclass:height = property(_cspace.csBitmapMetrics_height_get, _cspace.csBitmapMetrics_height_set)
    __swig_setmethods__["left"] = _cspace.csBitmapMetrics_left_set
    __swig_getmethods__["left"] = _cspace.csBitmapMetrics_left_get
    if _newclass:left = property(_cspace.csBitmapMetrics_left_get, _cspace.csBitmapMetrics_left_set)
    __swig_setmethods__["top"] = _cspace.csBitmapMetrics_top_set
    __swig_getmethods__["top"] = _cspace.csBitmapMetrics_top_get
    if _newclass:top = property(_cspace.csBitmapMetrics_top_get, _cspace.csBitmapMetrics_top_set)
    def __init__(self, *args): 
        this = _cspace.new_csBitmapMetrics(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csBitmapMetrics
    __del__ = lambda self : None;
csBitmapMetrics_swigregister = _cspace.csBitmapMetrics_swigregister
csBitmapMetrics_swigregister(csBitmapMetrics)

class csGlyphMetrics(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csGlyphMetrics, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csGlyphMetrics, name)
    __repr__ = _swig_repr
    __swig_setmethods__["advance"] = _cspace.csGlyphMetrics_advance_set
    __swig_getmethods__["advance"] = _cspace.csGlyphMetrics_advance_get
    if _newclass:advance = property(_cspace.csGlyphMetrics_advance_get, _cspace.csGlyphMetrics_advance_set)
    def __init__(self, *args): 
        this = _cspace.new_csGlyphMetrics(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csGlyphMetrics
    __del__ = lambda self : None;
csGlyphMetrics_swigregister = _cspace.csGlyphMetrics_swigregister
csGlyphMetrics_swigregister(csGlyphMetrics)

class iFont(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iFont, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iFont, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def AddDeleteCallback(*args): return _cspace.iFont_AddDeleteCallback(*args)
    def RemoveDeleteCallback(*args): return _cspace.iFont_RemoveDeleteCallback(*args)
    def GetSize(*args): return _cspace.iFont_GetSize(*args)
    def GetMaxSize(*args): return _cspace.iFont_GetMaxSize(*args)
    def GetGlyphMetrics(*args): return _cspace.iFont_GetGlyphMetrics(*args)
    def GetGlyphBitmap(*args): return _cspace.iFont_GetGlyphBitmap(*args)
    def GetGlyphAlphaBitmap(*args): return _cspace.iFont_GetGlyphAlphaBitmap(*args)
    def GetDimensions(*args): return _cspace.iFont_GetDimensions(*args)
    def GetLength(*args): return _cspace.iFont_GetLength(*args)
    def GetDescent(*args): return _cspace.iFont_GetDescent(*args)
    def GetAscent(*args): return _cspace.iFont_GetAscent(*args)
    def HasGlyph(*args): return _cspace.iFont_HasGlyph(*args)
    def GetTextHeight(*args): return _cspace.iFont_GetTextHeight(*args)
    def GetUnderlinePosition(*args): return _cspace.iFont_GetUnderlinePosition(*args)
    def GetUnderlineThickness(*args): return _cspace.iFont_GetUnderlineThickness(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iFont_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iFont_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iFont
    __del__ = lambda self : None;
iFont_swigregister = _cspace.iFont_swigregister
iFont_swigregister(iFont)
iFont_scfGetVersion = _cspace.iFont_scfGetVersion

class iFontServer(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iFontServer, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iFontServer, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def LoadFont(*args): return _cspace.iFontServer_LoadFont(*args)
    def SetWarnOnError(*args): return _cspace.iFontServer_SetWarnOnError(*args)
    def GetWarnOnError(*args): return _cspace.iFontServer_GetWarnOnError(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iFontServer_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iFontServer_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iFontServer
    __del__ = lambda self : None;
iFontServer_swigregister = _cspace.iFontServer_swigregister
iFontServer_swigregister(iFontServer)
iFontServer_scfGetVersion = _cspace.iFontServer_scfGetVersion

class iHalo(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iHalo, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iHalo, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetWidth(*args): return _cspace.iHalo_GetWidth(*args)
    def GetHeight(*args): return _cspace.iHalo_GetHeight(*args)
    def SetColor(*args): return _cspace.iHalo_SetColor(*args)
    def GetColor(*args): return _cspace.iHalo_GetColor(*args)
    def Draw(*args): return _cspace.iHalo_Draw(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iHalo_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iHalo_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iHalo
    __del__ = lambda self : None;
iHalo_swigregister = _cspace.iHalo_swigregister
iHalo_swigregister(iHalo)
iHalo_scfGetVersion = _cspace.iHalo_scfGetVersion

class iShaderVarStack(csShaderVariableArray):
    __swig_setmethods__ = {}
    for _s in [csShaderVariableArray]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iShaderVarStack, name, value)
    __swig_getmethods__ = {}
    for _s in [csShaderVariableArray]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iShaderVarStack, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_iShaderVarStack
    __del__ = lambda self : None;
iShaderVarStack_swigregister = _cspace.iShaderVarStack_swigregister
iShaderVarStack_swigregister(iShaderVarStack)

class iShaderVariableContext(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iShaderVariableContext, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iShaderVariableContext, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def AddVariable(*args): return _cspace.iShaderVariableContext_AddVariable(*args)
    def GetVariable(*args): return _cspace.iShaderVariableContext_GetVariable(*args)
    def GetVariableAdd(*args): return _cspace.iShaderVariableContext_GetVariableAdd(*args)
    def GetShaderVariables(*args): return _cspace.iShaderVariableContext_GetShaderVariables(*args)
    def PushVariables(*args): return _cspace.iShaderVariableContext_PushVariables(*args)
    def IsEmpty(*args): return _cspace.iShaderVariableContext_IsEmpty(*args)
    def ReplaceVariable(*args): return _cspace.iShaderVariableContext_ReplaceVariable(*args)
    def Clear(*args): return _cspace.iShaderVariableContext_Clear(*args)
    def RemoveVariable(*args): return _cspace.iShaderVariableContext_RemoveVariable(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iShaderVariableContext_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iShaderVariableContext_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iShaderVariableContext
    __del__ = lambda self : None;
iShaderVariableContext_swigregister = _cspace.iShaderVariableContext_swigregister
iShaderVariableContext_swigregister(iShaderVariableContext)
csGetShaderVariableFromStack = _cspace.csGetShaderVariableFromStack
iShaderVariableContext_scfGetVersion = _cspace.iShaderVariableContext_scfGetVersion

TagNeutral = _cspace.TagNeutral
TagForbidden = _cspace.TagForbidden
TagRequired = _cspace.TagRequired
class iShaderManager(iShaderVariableContext):
    __swig_setmethods__ = {}
    for _s in [iShaderVariableContext]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iShaderManager, name, value)
    __swig_getmethods__ = {}
    for _s in [iShaderVariableContext]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iShaderManager, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def RegisterShader(*args): return _cspace.iShaderManager_RegisterShader(*args)
    def UnregisterShader(*args): return _cspace.iShaderManager_UnregisterShader(*args)
    def UnregisterShaders(*args): return _cspace.iShaderManager_UnregisterShaders(*args)
    def GetShader(*args): return _cspace.iShaderManager_GetShader(*args)
    def GetShaders(*args): return _cspace.iShaderManager_GetShaders(*args)
    def RegisterCompiler(*args): return _cspace.iShaderManager_RegisterCompiler(*args)
    def GetCompiler(*args): return _cspace.iShaderManager_GetCompiler(*args)
    def RegisterShaderVariableAccessor(*args): return _cspace.iShaderManager_RegisterShaderVariableAccessor(*args)
    def UnregisterShaderVariableAccessor(*args): return _cspace.iShaderManager_UnregisterShaderVariableAccessor(*args)
    def GetShaderVariableAccessor(*args): return _cspace.iShaderManager_GetShaderVariableAccessor(*args)
    def UnregisterShaderVariableAcessors(*args): return _cspace.iShaderManager_UnregisterShaderVariableAcessors(*args)
    def GetShaderVariableStack(*args): return _cspace.iShaderManager_GetShaderVariableStack(*args)
    def SetTagOptions(*args): return _cspace.iShaderManager_SetTagOptions(*args)
    def GetTagOptions(*args): return _cspace.iShaderManager_GetTagOptions(*args)
    def GetTags(*args): return _cspace.iShaderManager_GetTags(*args)
    def SetActiveLights(*args): return _cspace.iShaderManager_SetActiveLights(*args)
    def GetActiveLights(*args): return _cspace.iShaderManager_GetActiveLights(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iShaderManager_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iShaderManager_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iShaderManager
    __del__ = lambda self : None;
iShaderManager_swigregister = _cspace.iShaderManager_swigregister
iShaderManager_swigregister(iShaderManager)
iShaderManager_scfGetVersion = _cspace.iShaderManager_scfGetVersion

class csShaderMetadata(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csShaderMetadata, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csShaderMetadata, name)
    __repr__ = _swig_repr
    __swig_setmethods__["description"] = _cspace.csShaderMetadata_description_set
    __swig_getmethods__["description"] = _cspace.csShaderMetadata_description_get
    if _newclass:description = property(_cspace.csShaderMetadata_description_get, _cspace.csShaderMetadata_description_set)
    __swig_setmethods__["numberOfLights"] = _cspace.csShaderMetadata_numberOfLights_set
    __swig_getmethods__["numberOfLights"] = _cspace.csShaderMetadata_numberOfLights_get
    if _newclass:numberOfLights = property(_cspace.csShaderMetadata_numberOfLights_get, _cspace.csShaderMetadata_numberOfLights_set)
    def __init__(self, *args): 
        this = _cspace.new_csShaderMetadata(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csShaderMetadata
    __del__ = lambda self : None;
csShaderMetadata_swigregister = _cspace.csShaderMetadata_swigregister
csShaderMetadata_swigregister(csShaderMetadata)

class iShader(iShaderVariableContext):
    __swig_setmethods__ = {}
    for _s in [iShaderVariableContext]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iShader, name, value)
    __swig_getmethods__ = {}
    for _s in [iShaderVariableContext]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iShader, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iShader_QueryObject(*args)
    def GetFileName(*args): return _cspace.iShader_GetFileName(*args)
    def SetFileName(*args): return _cspace.iShader_SetFileName(*args)
    def GetTicket(*args): return _cspace.iShader_GetTicket(*args)
    def GetNumberOfPasses(*args): return _cspace.iShader_GetNumberOfPasses(*args)
    def ActivatePass(*args): return _cspace.iShader_ActivatePass(*args)
    def SetupPass(*args): return _cspace.iShader_SetupPass(*args)
    def TeardownPass(*args): return _cspace.iShader_TeardownPass(*args)
    def DeactivatePass(*args): return _cspace.iShader_DeactivatePass(*args)
    def GetMetadata(*args): return _cspace.iShader_GetMetadata(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iShader_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iShader_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iShader
    __del__ = lambda self : None;
iShader_swigregister = _cspace.iShader_swigregister
iShader_swigregister(iShader)
iShader_scfGetVersion = _cspace.iShader_scfGetVersion

class iShaderPriorityList(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iShaderPriorityList, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iShaderPriorityList, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetCount(*args): return _cspace.iShaderPriorityList_GetCount(*args)
    def GetPriority(*args): return _cspace.iShaderPriorityList_GetPriority(*args)
    __swig_destroy__ = _cspace.delete_iShaderPriorityList
    __del__ = lambda self : None;
iShaderPriorityList_swigregister = _cspace.iShaderPriorityList_swigregister
iShaderPriorityList_swigregister(iShaderPriorityList)

class iShaderCompiler(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iShaderCompiler, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iShaderCompiler, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetName(*args): return _cspace.iShaderCompiler_GetName(*args)
    def CompileShader(*args): return _cspace.iShaderCompiler_CompileShader(*args)
    def ValidateTemplate(*args): return _cspace.iShaderCompiler_ValidateTemplate(*args)
    def IsTemplateToCompiler(*args): return _cspace.iShaderCompiler_IsTemplateToCompiler(*args)
    def GetPriorities(*args): return _cspace.iShaderCompiler_GetPriorities(*args)
    __swig_destroy__ = _cspace.delete_iShaderCompiler
    __del__ = lambda self : None;
iShaderCompiler_swigregister = _cspace.iShaderCompiler_swigregister
iShaderCompiler_swigregister(iShaderCompiler)

class iTextureHandle(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iTextureHandle, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iTextureHandle, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetFlags(*args): return _cspace.iTextureHandle_GetFlags(*args)
    def SetKeyColor(*args): return _cspace.iTextureHandle_SetKeyColor(*args)
    def GetKeyColorStatus(*args): return _cspace.iTextureHandle_GetKeyColorStatus(*args)
    def GetKeyColor(*args): return _cspace.iTextureHandle_GetKeyColor(*args)
    CS_TEX_IMG_1D = _cspace.iTextureHandle_CS_TEX_IMG_1D
    CS_TEX_IMG_2D = _cspace.iTextureHandle_CS_TEX_IMG_2D
    CS_TEX_IMG_3D = _cspace.iTextureHandle_CS_TEX_IMG_3D
    CS_TEX_IMG_CUBEMAP = _cspace.iTextureHandle_CS_TEX_IMG_CUBEMAP
    CS_TEX_IMG_RECT = _cspace.iTextureHandle_CS_TEX_IMG_RECT
    CS_TEXTURE_CUBE_POS_X = _cspace.iTextureHandle_CS_TEXTURE_CUBE_POS_X
    CS_TEXTURE_CUBE_NEG_X = _cspace.iTextureHandle_CS_TEXTURE_CUBE_NEG_X
    CS_TEXTURE_CUBE_POS_Y = _cspace.iTextureHandle_CS_TEXTURE_CUBE_POS_Y
    CS_TEXTURE_CUBE_NEG_Y = _cspace.iTextureHandle_CS_TEXTURE_CUBE_NEG_Y
    CS_TEXTURE_CUBE_POS_Z = _cspace.iTextureHandle_CS_TEXTURE_CUBE_POS_Z
    CS_TEXTURE_CUBE_NEG_Z = _cspace.iTextureHandle_CS_TEXTURE_CUBE_NEG_Z
    def GetRendererDimensions(*args): return _cspace.iTextureHandle_GetRendererDimensions(*args)
    def GetOriginalDimensions(*args): return _cspace.iTextureHandle_GetOriginalDimensions(*args)
    def GetTextureTarget(*args): return _cspace.iTextureHandle_GetTextureTarget(*args)
    RGBA8888 = _cspace.iTextureHandle_RGBA8888
    BGRA8888 = _cspace.iTextureHandle_BGRA8888
    def Blit(*args): return _cspace.iTextureHandle_Blit(*args)
    def GetImageName(*args): return _cspace.iTextureHandle_GetImageName(*args)
    def GetPrivateObject(*args): return _cspace.iTextureHandle_GetPrivateObject(*args)
    def GetAlphaMap(*args): return _cspace.iTextureHandle_GetAlphaMap(*args)
    def GetAlphaType(*args): return _cspace.iTextureHandle_GetAlphaType(*args)
    def Precache(*args): return _cspace.iTextureHandle_Precache(*args)
    def SetTextureClass(*args): return _cspace.iTextureHandle_SetTextureClass(*args)
    def GetTextureClass(*args): return _cspace.iTextureHandle_GetTextureClass(*args)
    def SetAlphaType(*args): return _cspace.iTextureHandle_SetAlphaType(*args)
    texType1D = _cspace.iTextureHandle_texType1D
    texType2D = _cspace.iTextureHandle_texType2D
    texType3D = _cspace.iTextureHandle_texType3D
    texTypeCube = _cspace.iTextureHandle_texTypeCube
    texTypeRect = _cspace.iTextureHandle_texTypeRect
    def GetTextureType(*args): return _cspace.iTextureHandle_GetTextureType(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iTextureHandle_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iTextureHandle_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iTextureHandle
    __del__ = lambda self : None;
iTextureHandle_swigregister = _cspace.iTextureHandle_swigregister
iTextureHandle_swigregister(iTextureHandle)
iTextureHandle_scfGetVersion = _cspace.iTextureHandle_scfGetVersion

CS_TEXTURE_2D = _cspace.CS_TEXTURE_2D
CS_TEXTURE_3D = _cspace.CS_TEXTURE_3D
CS_TEXTURE_NOMIPMAPS = _cspace.CS_TEXTURE_NOMIPMAPS
CS_TEXTURE_CLAMP = _cspace.CS_TEXTURE_CLAMP
CS_TEXTURE_NOFILTER = _cspace.CS_TEXTURE_NOFILTER
CS_TEXTURE_NPOTS = _cspace.CS_TEXTURE_NPOTS
CS_TEXTURE_SCALE_UP = _cspace.CS_TEXTURE_SCALE_UP
CS_TEXTURE_SCALE_DOWN = _cspace.CS_TEXTURE_SCALE_DOWN
class iRendererLightmap(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iRendererLightmap, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iRendererLightmap, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetSLMCoords(*args): return _cspace.iRendererLightmap_GetSLMCoords(*args)
    def SetData(*args): return _cspace.iRendererLightmap_SetData(*args)
    def SetLightCellSize(*args): return _cspace.iRendererLightmap_SetLightCellSize(*args)
    __swig_destroy__ = _cspace.delete_iRendererLightmap
    __del__ = lambda self : None;
iRendererLightmap_swigregister = _cspace.iRendererLightmap_swigregister
iRendererLightmap_swigregister(iRendererLightmap)

class iSuperLightmap(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSuperLightmap, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSuperLightmap, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def RegisterLightmap(*args): return _cspace.iSuperLightmap_RegisterLightmap(*args)
    def Dump(*args): return _cspace.iSuperLightmap_Dump(*args)
    def GetTexture(*args): return _cspace.iSuperLightmap_GetTexture(*args)
    __swig_destroy__ = _cspace.delete_iSuperLightmap
    __del__ = lambda self : None;
iSuperLightmap_swigregister = _cspace.iSuperLightmap_swigregister
iSuperLightmap_swigregister(iSuperLightmap)

class iTextureManager(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iTextureManager, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iTextureManager, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def RegisterTexture(*args): return _cspace.iTextureManager_RegisterTexture(*args)
    def CreateTexture(*args): return _cspace.iTextureManager_CreateTexture(*args)
    def GetTextureFormat(*args): return _cspace.iTextureManager_GetTextureFormat(*args)
    def CreateSuperLightmap(*args): return _cspace.iTextureManager_CreateSuperLightmap(*args)
    def GetMaxTextureSize(*args): return _cspace.iTextureManager_GetMaxTextureSize(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iTextureManager_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iTextureManager_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iTextureManager
    __del__ = lambda self : None;
iTextureManager_swigregister = _cspace.iTextureManager_swigregister
iTextureManager_swigregister(iTextureManager)
iTextureManager_scfGetVersion = _cspace.iTextureManager_scfGetVersion

CS_MATERIAL_VARNAME_FLATCOLOR = _cspace.CS_MATERIAL_VARNAME_FLATCOLOR
CS_MATERIAL_TEXTURE_DIFFUSE = _cspace.CS_MATERIAL_TEXTURE_DIFFUSE
class iMaterial(iShaderVariableContext):
    __swig_setmethods__ = {}
    for _s in [iShaderVariableContext]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMaterial, name, value)
    __swig_getmethods__ = {}
    for _s in [iShaderVariableContext]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMaterial, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetShader(*args): return _cspace.iMaterial_SetShader(*args)
    def GetShader(*args): return _cspace.iMaterial_GetShader(*args)
    def GetShaders(*args): return _cspace.iMaterial_GetShaders(*args)
    def GetTexture(*args): return _cspace.iMaterial_GetTexture(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iMaterial_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iMaterial_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iMaterial
    __del__ = lambda self : None;
iMaterial_swigregister = _cspace.iMaterial_swigregister
iMaterial_swigregister(iMaterial)
iMaterial_scfGetVersion = _cspace.iMaterial_scfGetVersion

CS_IMGFMT_MASK = _cspace.CS_IMGFMT_MASK
CS_IMGFMT_NONE = _cspace.CS_IMGFMT_NONE
CS_IMGFMT_TRUECOLOR = _cspace.CS_IMGFMT_TRUECOLOR
CS_IMGFMT_PALETTED8 = _cspace.CS_IMGFMT_PALETTED8
CS_IMGFMT_ANY = _cspace.CS_IMGFMT_ANY
CS_IMGFMT_ALPHA = _cspace.CS_IMGFMT_ALPHA
CS_IMGFMT_INVALID = _cspace.CS_IMGFMT_INVALID
csimg2D = _cspace.csimg2D
csimg3D = _cspace.csimg3D
csimgCube = _cspace.csimgCube
class iImage(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iImage, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iImage, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetImageData(*args): return _cspace.iImage_GetImageData(*args)
    def GetWidth(*args): return _cspace.iImage_GetWidth(*args)
    def GetHeight(*args): return _cspace.iImage_GetHeight(*args)
    def GetDepth(*args): return _cspace.iImage_GetDepth(*args)
    def SetName(*args): return _cspace.iImage_SetName(*args)
    def GetName(*args): return _cspace.iImage_GetName(*args)
    def GetFormat(*args): return _cspace.iImage_GetFormat(*args)
    def GetPalette(*args): return _cspace.iImage_GetPalette(*args)
    def GetAlpha(*args): return _cspace.iImage_GetAlpha(*args)
    def HasKeyColor(*args): return _cspace.iImage_HasKeyColor(*args)
    def GetKeyColor(*args): return _cspace.iImage_GetKeyColor(*args)
    def HasMipmaps(*args): return _cspace.iImage_HasMipmaps(*args)
    def GetMipmap(*args): return _cspace.iImage_GetMipmap(*args)
    def GetRawFormat(*args): return _cspace.iImage_GetRawFormat(*args)
    def GetRawData(*args): return _cspace.iImage_GetRawData(*args)
    def GetImageType(*args): return _cspace.iImage_GetImageType(*args)
    def HasSubImages(*args): return _cspace.iImage_HasSubImages(*args)
    def GetSubImage(*args): return _cspace.iImage_GetSubImage(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iImage_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iImage_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iImage
    __del__ = lambda self : None;
iImage_swigregister = _cspace.iImage_swigregister
iImage_swigregister(iImage)
iImage_scfGetVersion = _cspace.iImage_scfGetVersion

class csImageBaseBase(iImage):
    __swig_setmethods__ = {}
    for _s in [iImage]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csImageBaseBase, name, value)
    __swig_getmethods__ = {}
    for _s in [iImage]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csImageBaseBase, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def IncRef(*args): return _cspace.csImageBaseBase_IncRef(*args)
    def DecRef(*args): return _cspace.csImageBaseBase_DecRef(*args)
    def GetRefCount(*args): return _cspace.csImageBaseBase_GetRefCount(*args)
    def QueryInterface(*args): return _cspace.csImageBaseBase_QueryInterface(*args)
    def AddRefOwner(*args): return _cspace.csImageBaseBase_AddRefOwner(*args)
    def RemoveRefOwner(*args): return _cspace.csImageBaseBase_RemoveRefOwner(*args)
    def GetInterfaceMetadata(*args): return _cspace.csImageBaseBase_GetInterfaceMetadata(*args)
csImageBaseBase_swigregister = _cspace.csImageBaseBase_swigregister
csImageBaseBase_swigregister(csImageBaseBase)

class csImageBase(csImageBaseBase):
    __swig_setmethods__ = {}
    for _s in [csImageBaseBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csImageBase, name, value)
    __swig_getmethods__ = {}
    for _s in [csImageBaseBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csImageBase, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_csImageBase
    __del__ = lambda self : None;
    def GetDepth(*args): return _cspace.csImageBase_GetDepth(*args)
    def SetName(*args): return _cspace.csImageBase_SetName(*args)
    def GetName(*args): return _cspace.csImageBase_GetName(*args)
    def GetPalette(*args): return _cspace.csImageBase_GetPalette(*args)
    def GetAlpha(*args): return _cspace.csImageBase_GetAlpha(*args)
    def HasKeyColor(*args): return _cspace.csImageBase_HasKeyColor(*args)
    def GetKeyColor(*args): return _cspace.csImageBase_GetKeyColor(*args)
    def HasMipmaps(*args): return _cspace.csImageBase_HasMipmaps(*args)
    def GetMipmap(*args): return _cspace.csImageBase_GetMipmap(*args)
    def GetRawFormat(*args): return _cspace.csImageBase_GetRawFormat(*args)
    def GetRawData(*args): return _cspace.csImageBase_GetRawData(*args)
    def GetImageType(*args): return _cspace.csImageBase_GetImageType(*args)
    def HasSubImages(*args): return _cspace.csImageBase_HasSubImages(*args)
    def GetSubImage(*args): return _cspace.csImageBase_GetSubImage(*args)
csImageBase_swigregister = _cspace.csImageBase_swigregister
csImageBase_swigregister(csImageBase)

class csImageMemoryBase(csImageBase):
    __swig_setmethods__ = {}
    for _s in [csImageBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csImageMemoryBase, name, value)
    __swig_getmethods__ = {}
    for _s in [csImageBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csImageMemoryBase, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def IncRef(*args): return _cspace.csImageMemoryBase_IncRef(*args)
    def DecRef(*args): return _cspace.csImageMemoryBase_DecRef(*args)
    def GetRefCount(*args): return _cspace.csImageMemoryBase_GetRefCount(*args)
    def QueryInterface(*args): return _cspace.csImageMemoryBase_QueryInterface(*args)
    def AddRefOwner(*args): return _cspace.csImageMemoryBase_AddRefOwner(*args)
    def RemoveRefOwner(*args): return _cspace.csImageMemoryBase_RemoveRefOwner(*args)
    def GetInterfaceMetadata(*args): return _cspace.csImageMemoryBase_GetInterfaceMetadata(*args)
csImageMemoryBase_swigregister = _cspace.csImageMemoryBase_swigregister
csImageMemoryBase_swigregister(csImageMemoryBase)

class csImageMemory(csImageMemoryBase):
    __swig_setmethods__ = {}
    for _s in [csImageMemoryBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csImageMemory, name, value)
    __swig_getmethods__ = {}
    for _s in [csImageMemoryBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csImageMemory, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csImageMemory(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csImageMemory
    __del__ = lambda self : None;
    def GetImagePtr(*args): return _cspace.csImageMemory_GetImagePtr(*args)
    def GetPalettePtr(*args): return _cspace.csImageMemory_GetPalettePtr(*args)
    def GetAlphaPtr(*args): return _cspace.csImageMemory_GetAlphaPtr(*args)
    def GetImageData(*args): return _cspace.csImageMemory_GetImageData(*args)
    def GetWidth(*args): return _cspace.csImageMemory_GetWidth(*args)
    def GetHeight(*args): return _cspace.csImageMemory_GetHeight(*args)
    def GetDepth(*args): return _cspace.csImageMemory_GetDepth(*args)
    def GetRawFormat(*args): return _cspace.csImageMemory_GetRawFormat(*args)
    def GetRawData(*args): return _cspace.csImageMemory_GetRawData(*args)
    def GetFormat(*args): return _cspace.csImageMemory_GetFormat(*args)
    def GetPalette(*args): return _cspace.csImageMemory_GetPalette(*args)
    def GetAlpha(*args): return _cspace.csImageMemory_GetAlpha(*args)
    def HasKeyColor(*args): return _cspace.csImageMemory_HasKeyColor(*args)
    def GetKeyColor(*args): return _cspace.csImageMemory_GetKeyColor(*args)
    def Clear(*args): return _cspace.csImageMemory_Clear(*args)
    def CheckAlpha(*args): return _cspace.csImageMemory_CheckAlpha(*args)
    def SetFormat(*args): return _cspace.csImageMemory_SetFormat(*args)
    def SetKeyColor(*args): return _cspace.csImageMemory_SetKeyColor(*args)
    def SetKeycolor(*args): return _cspace.csImageMemory_SetKeycolor(*args)
    def ClearKeyColor(*args): return _cspace.csImageMemory_ClearKeyColor(*args)
    def ClearKeycolor(*args): return _cspace.csImageMemory_ClearKeycolor(*args)
    def ApplyKeyColor(*args): return _cspace.csImageMemory_ApplyKeyColor(*args)
    def ApplyKeycolor(*args): return _cspace.csImageMemory_ApplyKeycolor(*args)
    def GetImageType(*args): return _cspace.csImageMemory_GetImageType(*args)
    def SetImageType(*args): return _cspace.csImageMemory_SetImageType(*args)
    def HasMipmaps(*args): return _cspace.csImageMemory_HasMipmaps(*args)
    def GetMipmap(*args): return _cspace.csImageMemory_GetMipmap(*args)
    def SetMipmap(*args): return _cspace.csImageMemory_SetMipmap(*args)
    def Copy(*args): return _cspace.csImageMemory_Copy(*args)
    def CopyScale(*args): return _cspace.csImageMemory_CopyScale(*args)
    def CopyTile(*args): return _cspace.csImageMemory_CopyTile(*args)
    def ConvertFromRGBA(*args): return _cspace.csImageMemory_ConvertFromRGBA(*args)
    def ConvertFromPal8(*args): return _cspace.csImageMemory_ConvertFromPal8(*args)
csImageMemory_swigregister = _cspace.csImageMemory_swigregister
csImageMemory_swigregister(csImageMemory)

class csImageIOFileFormatDescriptions(CustomAllocated):
    __swig_setmethods__ = {}
    for _s in [CustomAllocated]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csImageIOFileFormatDescriptions, name, value)
    __swig_getmethods__ = {}
    for _s in [CustomAllocated]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csImageIOFileFormatDescriptions, name)
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_csImageIOFileFormatDescriptions
    __del__ = lambda self : None;
    def __init__(self, *args): 
        this = _cspace.new_csImageIOFileFormatDescriptions(*args)
        try: self.this.append(this)
        except: self.this = this
    def GetSize(*args): return _cspace.csImageIOFileFormatDescriptions_GetSize(*args)
    def Get(*args): return _cspace.csImageIOFileFormatDescriptions_Get(*args)
    def Put(*args): return _cspace.csImageIOFileFormatDescriptions_Put(*args)
    def Push(*args): return _cspace.csImageIOFileFormatDescriptions_Push(*args)
    def Pop(*args): return _cspace.csImageIOFileFormatDescriptions_Pop(*args)
    def Top(*args): return _cspace.csImageIOFileFormatDescriptions_Top(*args)
    def Insert(*args): return _cspace.csImageIOFileFormatDescriptions_Insert(*args)
    def Contains(*args): return _cspace.csImageIOFileFormatDescriptions_Contains(*args)
    def Truncate(*args): return _cspace.csImageIOFileFormatDescriptions_Truncate(*args)
    def Empty(*args): return _cspace.csImageIOFileFormatDescriptions_Empty(*args)
    def IsEmpty(*args): return _cspace.csImageIOFileFormatDescriptions_IsEmpty(*args)
    def SetMinimalCapacity(*args): return _cspace.csImageIOFileFormatDescriptions_SetMinimalCapacity(*args)
    def DeleteIndex(*args): return _cspace.csImageIOFileFormatDescriptions_DeleteIndex(*args)
    def DeleteIndexFast(*args): return _cspace.csImageIOFileFormatDescriptions_DeleteIndexFast(*args)
    def DeleteRange(*args): return _cspace.csImageIOFileFormatDescriptions_DeleteRange(*args)
    def __eq__(*args): return _cspace.csImageIOFileFormatDescriptions___eq__(*args)
    def __ne__(*args): return _cspace.csImageIOFileFormatDescriptions___ne__(*args)
    def GetAllocator(*args): return _cspace.csImageIOFileFormatDescriptions_GetAllocator(*args)
csImageIOFileFormatDescriptions_swigregister = _cspace.csImageIOFileFormatDescriptions_swigregister
csImageIOFileFormatDescriptions_swigregister(csImageIOFileFormatDescriptions)

CS_IMAGEIO_LOAD = _cspace.CS_IMAGEIO_LOAD
CS_IMAGEIO_SAVE = _cspace.CS_IMAGEIO_SAVE
class csImageIOFileFormatDescription(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csImageIOFileFormatDescription, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csImageIOFileFormatDescription, name)
    __repr__ = _swig_repr
    __swig_getmethods__["mime"] = _cspace.csImageIOFileFormatDescription_mime_get
    if _newclass:mime = property(_cspace.csImageIOFileFormatDescription_mime_get)
    __swig_getmethods__["subtype"] = _cspace.csImageIOFileFormatDescription_subtype_get
    if _newclass:subtype = property(_cspace.csImageIOFileFormatDescription_subtype_get)
    __swig_setmethods__["cap"] = _cspace.csImageIOFileFormatDescription_cap_set
    __swig_getmethods__["cap"] = _cspace.csImageIOFileFormatDescription_cap_get
    if _newclass:cap = property(_cspace.csImageIOFileFormatDescription_cap_get, _cspace.csImageIOFileFormatDescription_cap_set)
    def __init__(self, *args): 
        this = _cspace.new_csImageIOFileFormatDescription(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csImageIOFileFormatDescription
    __del__ = lambda self : None;
csImageIOFileFormatDescription_swigregister = _cspace.csImageIOFileFormatDescription_swigregister
csImageIOFileFormatDescription_swigregister(csImageIOFileFormatDescription)

class iImageIO(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iImageIO, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iImageIO, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetDescription(*args): return _cspace.iImageIO_GetDescription(*args)
    def Load(*args): return _cspace.iImageIO_Load(*args)
    def SetDithering(*args): return _cspace.iImageIO_SetDithering(*args)
    def Save(*args): return _cspace.iImageIO_Save(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iImageIO_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iImageIO_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iImageIO
    __del__ = lambda self : None;
iImageIO_swigregister = _cspace.iImageIO_swigregister
iImageIO_swigregister(iImageIO)
iImageIO_scfGetVersion = _cspace.iImageIO_scfGetVersion

CS_REPORTER_SEVERITY_BUG = _cspace.CS_REPORTER_SEVERITY_BUG
CS_REPORTER_SEVERITY_ERROR = _cspace.CS_REPORTER_SEVERITY_ERROR
CS_REPORTER_SEVERITY_WARNING = _cspace.CS_REPORTER_SEVERITY_WARNING
CS_REPORTER_SEVERITY_NOTIFY = _cspace.CS_REPORTER_SEVERITY_NOTIFY
CS_REPORTER_SEVERITY_DEBUG = _cspace.CS_REPORTER_SEVERITY_DEBUG
class iReporterListener(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iReporterListener, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iReporterListener, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Report(*args): return _cspace.iReporterListener_Report(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iReporterListener_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iReporterListener_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iReporterListener
    __del__ = lambda self : None;
iReporterListener_swigregister = _cspace.iReporterListener_swigregister
iReporterListener_swigregister(iReporterListener)
iReporterListener_scfGetVersion = _cspace.iReporterListener_scfGetVersion

class iReporterIterator(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iReporterIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iReporterIterator, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def HasNext(*args): return _cspace.iReporterIterator_HasNext(*args)
    def Next(*args): return _cspace.iReporterIterator_Next(*args)
    def GetMessageSeverity(*args): return _cspace.iReporterIterator_GetMessageSeverity(*args)
    def GetMessageId(*args): return _cspace.iReporterIterator_GetMessageId(*args)
    def GetMessageDescription(*args): return _cspace.iReporterIterator_GetMessageDescription(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iReporterIterator_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iReporterIterator_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iReporterIterator
    __del__ = lambda self : None;
iReporterIterator_swigregister = _cspace.iReporterIterator_swigregister
iReporterIterator_swigregister(iReporterIterator)
iReporterIterator_scfGetVersion = _cspace.iReporterIterator_scfGetVersion

class iReporter(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iReporter, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iReporter, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Report(*args): return _cspace.iReporter_Report(*args)
    def Clear(*args): return _cspace.iReporter_Clear(*args)
    def GetMessageIterator(*args): return _cspace.iReporter_GetMessageIterator(*args)
    def AddReporterListener(*args): return _cspace.iReporter_AddReporterListener(*args)
    def RemoveReporterListener(*args): return _cspace.iReporter_RemoveReporterListener(*args)
    def FindReporterListener(*args): return _cspace.iReporter_FindReporterListener(*args)
    def ReportError(*args): return _cspace.iReporter_ReportError(*args)
    def ReportWarning(*args): return _cspace.iReporter_ReportWarning(*args)
    def ReportNotify(*args): return _cspace.iReporter_ReportNotify(*args)
    def ReportBug(*args): return _cspace.iReporter_ReportBug(*args)
    def ReportDebug(*args): return _cspace.iReporter_ReportDebug(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iReporter_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iReporter_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iReporter
    __del__ = lambda self : None;
iReporter_swigregister = _cspace.iReporter_swigregister
iReporter_swigregister(iReporter)
iReporter_scfGetVersion = _cspace.iReporter_scfGetVersion

class csReporterHelper(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csReporterHelper, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csReporterHelper, name)
    __repr__ = _swig_repr
    __swig_getmethods__["Report"] = lambda x: _cspace.csReporterHelper_Report
    if _newclass:Report = staticmethod(_cspace.csReporterHelper_Report)
    def __init__(self, *args): 
        this = _cspace.new_csReporterHelper(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csReporterHelper
    __del__ = lambda self : None;
csReporterHelper_swigregister = _cspace.csReporterHelper_swigregister
csReporterHelper_swigregister(csReporterHelper)
csReporterHelper_Report = _cspace.csReporterHelper_Report

csConPageUp = _cspace.csConPageUp
csConPageDown = _cspace.csConPageDown
csConVeryTop = _cspace.csConVeryTop
csConVeryBottom = _cspace.csConVeryBottom
csConNoCursor = _cspace.csConNoCursor
csConNormalCursor = _cspace.csConNormalCursor
csConInsertCursor = _cspace.csConInsertCursor
class iConsoleWatcher(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iConsoleWatcher, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iConsoleWatcher, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def ConsoleVisibilityChanged(*args): return _cspace.iConsoleWatcher_ConsoleVisibilityChanged(*args)
    __swig_destroy__ = _cspace.delete_iConsoleWatcher
    __del__ = lambda self : None;
iConsoleWatcher_swigregister = _cspace.iConsoleWatcher_swigregister
iConsoleWatcher_swigregister(iConsoleWatcher)

class iConsoleOutput(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iConsoleOutput, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iConsoleOutput, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def PutText(*args): return _cspace.iConsoleOutput_PutText(*args)
    def GetLine(*args): return _cspace.iConsoleOutput_GetLine(*args)
    def Draw2D(*args): return _cspace.iConsoleOutput_Draw2D(*args)
    def Draw3D(*args): return _cspace.iConsoleOutput_Draw3D(*args)
    def Clear(*args): return _cspace.iConsoleOutput_Clear(*args)
    def SetBufferSize(*args): return _cspace.iConsoleOutput_SetBufferSize(*args)
    def GetTransparency(*args): return _cspace.iConsoleOutput_GetTransparency(*args)
    def SetTransparency(*args): return _cspace.iConsoleOutput_SetTransparency(*args)
    def GetFont(*args): return _cspace.iConsoleOutput_GetFont(*args)
    def SetFont(*args): return _cspace.iConsoleOutput_SetFont(*args)
    def GetTopLine(*args): return _cspace.iConsoleOutput_GetTopLine(*args)
    def ScrollTo(*args): return _cspace.iConsoleOutput_ScrollTo(*args)
    def GetCursorStyle(*args): return _cspace.iConsoleOutput_GetCursorStyle(*args)
    def SetCursorStyle(*args): return _cspace.iConsoleOutput_SetCursorStyle(*args)
    def SetVisible(*args): return _cspace.iConsoleOutput_SetVisible(*args)
    def GetVisible(*args): return _cspace.iConsoleOutput_GetVisible(*args)
    def AutoUpdate(*args): return _cspace.iConsoleOutput_AutoUpdate(*args)
    def SetCursorPos(*args): return _cspace.iConsoleOutput_SetCursorPos(*args)
    def GetMaxLineWidth(*args): return _cspace.iConsoleOutput_GetMaxLineWidth(*args)
    def RegisterWatcher(*args): return _cspace.iConsoleOutput_RegisterWatcher(*args)
    def PerformExtension(*args): return _cspace.iConsoleOutput_PerformExtension(*args)
    __swig_destroy__ = _cspace.delete_iConsoleOutput
    __del__ = lambda self : None;
iConsoleOutput_swigregister = _cspace.iConsoleOutput_swigregister
iConsoleOutput_swigregister(iConsoleOutput)

class iStandardReporterListener(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iStandardReporterListener, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iStandardReporterListener, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetOutputConsole(*args): return _cspace.iStandardReporterListener_SetOutputConsole(*args)
    def SetNativeWindowManager(*args): return _cspace.iStandardReporterListener_SetNativeWindowManager(*args)
    def SetReporter(*args): return _cspace.iStandardReporterListener_SetReporter(*args)
    def SetDebugFile(*args): return _cspace.iStandardReporterListener_SetDebugFile(*args)
    def SetDefaults(*args): return _cspace.iStandardReporterListener_SetDefaults(*args)
    def SetMessageDestination(*args): return _cspace.iStandardReporterListener_SetMessageDestination(*args)
    def RemoveMessages(*args): return _cspace.iStandardReporterListener_RemoveMessages(*args)
    def ShowMessageID(*args): return _cspace.iStandardReporterListener_ShowMessageID(*args)
    def GetDebugFile(*args): return _cspace.iStandardReporterListener_GetDebugFile(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iStandardReporterListener_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iStandardReporterListener_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iStandardReporterListener
    __del__ = lambda self : None;
iStandardReporterListener_swigregister = _cspace.iStandardReporterListener_swigregister
iStandardReporterListener_swigregister(iStandardReporterListener)
iStandardReporterListener_scfGetVersion = _cspace.iStandardReporterListener_scfGetVersion

class iView(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iView, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iView, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetEngine(*args): return _cspace.iView_GetEngine(*args)
    def SetEngine(*args): return _cspace.iView_SetEngine(*args)
    def GetCamera(*args): return _cspace.iView_GetCamera(*args)
    def SetCamera(*args): return _cspace.iView_SetCamera(*args)
    def GetContext(*args): return _cspace.iView_GetContext(*args)
    def SetContext(*args): return _cspace.iView_SetContext(*args)
    def SetRectangle(*args): return _cspace.iView_SetRectangle(*args)
    def ClearView(*args): return _cspace.iView_ClearView(*args)
    def AddViewVertex(*args): return _cspace.iView_AddViewVertex(*args)
    def RestrictClipperToScreen(*args): return _cspace.iView_RestrictClipperToScreen(*args)
    def UpdateClipper(*args): return _cspace.iView_UpdateClipper(*args)
    def GetClipper(*args): return _cspace.iView_GetClipper(*args)
    def Draw(*args): return _cspace.iView_Draw(*args)
    def SetAutoResize(*args): return _cspace.iView_SetAutoResize(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iView_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iView_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iView
    __del__ = lambda self : None;
iView_swigregister = _cspace.iView_swigregister
iView_swigregister(iView)
iView_scfGetVersion = _cspace.iView_scfGetVersion

class csCollisionPair(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csCollisionPair, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csCollisionPair, name)
    __repr__ = _swig_repr
    __swig_setmethods__["a1"] = _cspace.csCollisionPair_a1_set
    __swig_getmethods__["a1"] = _cspace.csCollisionPair_a1_get
    if _newclass:a1 = property(_cspace.csCollisionPair_a1_get, _cspace.csCollisionPair_a1_set)
    __swig_setmethods__["b1"] = _cspace.csCollisionPair_b1_set
    __swig_getmethods__["b1"] = _cspace.csCollisionPair_b1_get
    if _newclass:b1 = property(_cspace.csCollisionPair_b1_get, _cspace.csCollisionPair_b1_set)
    __swig_setmethods__["c1"] = _cspace.csCollisionPair_c1_set
    __swig_getmethods__["c1"] = _cspace.csCollisionPair_c1_get
    if _newclass:c1 = property(_cspace.csCollisionPair_c1_get, _cspace.csCollisionPair_c1_set)
    __swig_setmethods__["a2"] = _cspace.csCollisionPair_a2_set
    __swig_getmethods__["a2"] = _cspace.csCollisionPair_a2_get
    if _newclass:a2 = property(_cspace.csCollisionPair_a2_get, _cspace.csCollisionPair_a2_set)
    __swig_setmethods__["b2"] = _cspace.csCollisionPair_b2_set
    __swig_getmethods__["b2"] = _cspace.csCollisionPair_b2_get
    if _newclass:b2 = property(_cspace.csCollisionPair_b2_get, _cspace.csCollisionPair_b2_set)
    __swig_setmethods__["c2"] = _cspace.csCollisionPair_c2_set
    __swig_getmethods__["c2"] = _cspace.csCollisionPair_c2_get
    if _newclass:c2 = property(_cspace.csCollisionPair_c2_get, _cspace.csCollisionPair_c2_set)
    def __eq__(*args): return _cspace.csCollisionPair___eq__(*args)
    def __init__(self, *args): 
        this = _cspace.new_csCollisionPair(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csCollisionPair
    __del__ = lambda self : None;
csCollisionPair_swigregister = _cspace.csCollisionPair_swigregister
csCollisionPair_swigregister(csCollisionPair)

class csIntersectingTriangle(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csIntersectingTriangle, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csIntersectingTriangle, name)
    __repr__ = _swig_repr
    __swig_setmethods__["a"] = _cspace.csIntersectingTriangle_a_set
    __swig_getmethods__["a"] = _cspace.csIntersectingTriangle_a_get
    if _newclass:a = property(_cspace.csIntersectingTriangle_a_get, _cspace.csIntersectingTriangle_a_set)
    __swig_setmethods__["b"] = _cspace.csIntersectingTriangle_b_set
    __swig_getmethods__["b"] = _cspace.csIntersectingTriangle_b_get
    if _newclass:b = property(_cspace.csIntersectingTriangle_b_get, _cspace.csIntersectingTriangle_b_set)
    __swig_setmethods__["c"] = _cspace.csIntersectingTriangle_c_set
    __swig_getmethods__["c"] = _cspace.csIntersectingTriangle_c_get
    if _newclass:c = property(_cspace.csIntersectingTriangle_c_get, _cspace.csIntersectingTriangle_c_set)
    def __init__(self, *args): 
        this = _cspace.new_csIntersectingTriangle(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csIntersectingTriangle
    __del__ = lambda self : None;
csIntersectingTriangle_swigregister = _cspace.csIntersectingTriangle_swigregister
csIntersectingTriangle_swigregister(csIntersectingTriangle)

CS_MESH_COLLIDER = _cspace.CS_MESH_COLLIDER
CS_TERRAFORMER_COLLIDER = _cspace.CS_TERRAFORMER_COLLIDER
CS_TERRAIN_COLLIDER = _cspace.CS_TERRAIN_COLLIDER
class iCollider(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iCollider, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iCollider, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetColliderType(*args): return _cspace.iCollider_GetColliderType(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iCollider_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iCollider_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iCollider
    __del__ = lambda self : None;
iCollider_swigregister = _cspace.iCollider_swigregister
iCollider_swigregister(iCollider)
iCollider_scfGetVersion = _cspace.iCollider_scfGetVersion

class iCollideSystem(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iCollideSystem, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iCollideSystem, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetTriangleDataID(*args): return _cspace.iCollideSystem_GetTriangleDataID(*args)
    def GetBaseDataID(*args): return _cspace.iCollideSystem_GetBaseDataID(*args)
    def CreateCollider(*args): return _cspace.iCollideSystem_CreateCollider(*args)
    def Collide(*args): return _cspace.iCollideSystem_Collide(*args)
    def GetCollisionPairs(*args): return _cspace.iCollideSystem_GetCollisionPairs(*args)
    def GetCollisionPairCount(*args): return _cspace.iCollideSystem_GetCollisionPairCount(*args)
    def ResetCollisionPairs(*args): return _cspace.iCollideSystem_ResetCollisionPairs(*args)
    def CollideRay(*args): return _cspace.iCollideSystem_CollideRay(*args)
    def CollideSegment(*args): return _cspace.iCollideSystem_CollideSegment(*args)
    def GetIntersectingTriangles(*args): return _cspace.iCollideSystem_GetIntersectingTriangles(*args)
    def SetOneHitOnly(*args): return _cspace.iCollideSystem_SetOneHitOnly(*args)
    def GetOneHitOnly(*args): return _cspace.iCollideSystem_GetOneHitOnly(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iCollideSystem_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iCollideSystem_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iCollideSystem
    __del__ = lambda self : None;
    def GetCollisionPairByIndex(*args): return _cspace.iCollideSystem_GetCollisionPairByIndex(*args)
    def GetCollisionPairs (self):
      num = self.GetCollisionPairCount()
      pairs = []
      for i in range(num):
        pairs.append(self.GetCollisionPairByIndex(i))
      return pairs

iCollideSystem_swigregister = _cspace.iCollideSystem_swigregister
iCollideSystem_swigregister(iCollideSystem)
iCollideSystem_scfGetVersion = _cspace.iCollideSystem_scfGetVersion

class iDynamicsStepCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDynamicsStepCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDynamicsStepCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Step(*args): return _cspace.iDynamicsStepCallback_Step(*args)
    __swig_destroy__ = _cspace.delete_iDynamicsStepCallback
    __del__ = lambda self : None;
iDynamicsStepCallback_swigregister = _cspace.iDynamicsStepCallback_swigregister
iDynamicsStepCallback_swigregister(iDynamicsStepCallback)

class iDynamics(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDynamics, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDynamics, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CreateSystem(*args): return _cspace.iDynamics_CreateSystem(*args)
    def RemoveSystem(*args): return _cspace.iDynamics_RemoveSystem(*args)
    def RemoveSystems(*args): return _cspace.iDynamics_RemoveSystems(*args)
    def FindSystem(*args): return _cspace.iDynamics_FindSystem(*args)
    def Step(*args): return _cspace.iDynamics_Step(*args)
    def AddStepCallback(*args): return _cspace.iDynamics_AddStepCallback(*args)
    def RemoveStepCallback(*args): return _cspace.iDynamics_RemoveStepCallback(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iDynamics_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iDynamics_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iDynamics
    __del__ = lambda self : None;
iDynamics_swigregister = _cspace.iDynamics_swigregister
iDynamics_swigregister(iDynamics)
iDynamics_scfGetVersion = _cspace.iDynamics_scfGetVersion

class iDynamicSystem(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDynamicSystem, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDynamicSystem, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iDynamicSystem_QueryObject(*args)
    def SetGravity(*args): return _cspace.iDynamicSystem_SetGravity(*args)
    def GetGravity(*args): return _cspace.iDynamicSystem_GetGravity(*args)
    def SetLinearDampener(*args): return _cspace.iDynamicSystem_SetLinearDampener(*args)
    def GetLinearDampener(*args): return _cspace.iDynamicSystem_GetLinearDampener(*args)
    def SetRollingDampener(*args): return _cspace.iDynamicSystem_SetRollingDampener(*args)
    def GetRollingDampener(*args): return _cspace.iDynamicSystem_GetRollingDampener(*args)
    def EnableAutoDisable(*args): return _cspace.iDynamicSystem_EnableAutoDisable(*args)
    def AutoDisableEnabled(*args): return _cspace.iDynamicSystem_AutoDisableEnabled(*args)
    def SetAutoDisableParams(*args): return _cspace.iDynamicSystem_SetAutoDisableParams(*args)
    def Step(*args): return _cspace.iDynamicSystem_Step(*args)
    def CreateBody(*args): return _cspace.iDynamicSystem_CreateBody(*args)
    def RemoveBody(*args): return _cspace.iDynamicSystem_RemoveBody(*args)
    def FindBody(*args): return _cspace.iDynamicSystem_FindBody(*args)
    def GetBody(*args): return _cspace.iDynamicSystem_GetBody(*args)
    def GetBodysCount(*args): return _cspace.iDynamicSystem_GetBodysCount(*args)
    def CreateGroup(*args): return _cspace.iDynamicSystem_CreateGroup(*args)
    def RemoveGroup(*args): return _cspace.iDynamicSystem_RemoveGroup(*args)
    def CreateJoint(*args): return _cspace.iDynamicSystem_CreateJoint(*args)
    def RemoveJoint(*args): return _cspace.iDynamicSystem_RemoveJoint(*args)
    def GetDefaultMoveCallback(*args): return _cspace.iDynamicSystem_GetDefaultMoveCallback(*args)
    def AttachColliderMesh(*args): return _cspace.iDynamicSystem_AttachColliderMesh(*args)
    def AttachColliderCylinder(*args): return _cspace.iDynamicSystem_AttachColliderCylinder(*args)
    def AttachColliderBox(*args): return _cspace.iDynamicSystem_AttachColliderBox(*args)
    def AttachColliderSphere(*args): return _cspace.iDynamicSystem_AttachColliderSphere(*args)
    def AttachColliderPlane(*args): return _cspace.iDynamicSystem_AttachColliderPlane(*args)
    def DestroyColliders(*args): return _cspace.iDynamicSystem_DestroyColliders(*args)
    def DestroyCollider(*args): return _cspace.iDynamicSystem_DestroyCollider(*args)
    def AttachCollider(*args): return _cspace.iDynamicSystem_AttachCollider(*args)
    def CreateCollider(*args): return _cspace.iDynamicSystem_CreateCollider(*args)
    def GetCollider(*args): return _cspace.iDynamicSystem_GetCollider(*args)
    def GetColliderCount(*args): return _cspace.iDynamicSystem_GetColliderCount(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iDynamicSystem_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iDynamicSystem_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iDynamicSystem
    __del__ = lambda self : None;
iDynamicSystem_swigregister = _cspace.iDynamicSystem_swigregister
iDynamicSystem_swigregister(iDynamicSystem)
iDynamicSystem_scfGetVersion = _cspace.iDynamicSystem_scfGetVersion

class iDynamicsMoveCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDynamicsMoveCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDynamicsMoveCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Execute(*args): return _cspace.iDynamicsMoveCallback_Execute(*args)
    __swig_destroy__ = _cspace.delete_iDynamicsMoveCallback
    __del__ = lambda self : None;
iDynamicsMoveCallback_swigregister = _cspace.iDynamicsMoveCallback_swigregister
iDynamicsMoveCallback_swigregister(iDynamicsMoveCallback)

class iDynamicsCollisionCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDynamicsCollisionCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDynamicsCollisionCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Execute(*args): return _cspace.iDynamicsCollisionCallback_Execute(*args)
    __swig_destroy__ = _cspace.delete_iDynamicsCollisionCallback
    __del__ = lambda self : None;
iDynamicsCollisionCallback_swigregister = _cspace.iDynamicsCollisionCallback_swigregister
iDynamicsCollisionCallback_swigregister(iDynamicsCollisionCallback)

class iBodyGroup(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iBodyGroup, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iBodyGroup, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def AddBody(*args): return _cspace.iBodyGroup_AddBody(*args)
    def RemoveBody(*args): return _cspace.iBodyGroup_RemoveBody(*args)
    def BodyInGroup(*args): return _cspace.iBodyGroup_BodyInGroup(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iBodyGroup_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iBodyGroup_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iBodyGroup
    __del__ = lambda self : None;
iBodyGroup_swigregister = _cspace.iBodyGroup_swigregister
iBodyGroup_swigregister(iBodyGroup)
iBodyGroup_scfGetVersion = _cspace.iBodyGroup_scfGetVersion

class iRigidBody(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iRigidBody, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iRigidBody, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iRigidBody_QueryObject(*args)
    def MakeStatic(*args): return _cspace.iRigidBody_MakeStatic(*args)
    def MakeDynamic(*args): return _cspace.iRigidBody_MakeDynamic(*args)
    def IsStatic(*args): return _cspace.iRigidBody_IsStatic(*args)
    def Disable(*args): return _cspace.iRigidBody_Disable(*args)
    def Enable(*args): return _cspace.iRigidBody_Enable(*args)
    def IsEnabled(*args): return _cspace.iRigidBody_IsEnabled(*args)
    def GetGroup(*args): return _cspace.iRigidBody_GetGroup(*args)
    def AttachColliderMesh(*args): return _cspace.iRigidBody_AttachColliderMesh(*args)
    def AttachColliderCylinder(*args): return _cspace.iRigidBody_AttachColliderCylinder(*args)
    def AttachColliderBox(*args): return _cspace.iRigidBody_AttachColliderBox(*args)
    def AttachColliderSphere(*args): return _cspace.iRigidBody_AttachColliderSphere(*args)
    def AttachColliderPlane(*args): return _cspace.iRigidBody_AttachColliderPlane(*args)
    def AttachCollider(*args): return _cspace.iRigidBody_AttachCollider(*args)
    def DestroyColliders(*args): return _cspace.iRigidBody_DestroyColliders(*args)
    def DestroyCollider(*args): return _cspace.iRigidBody_DestroyCollider(*args)
    def SetPosition(*args): return _cspace.iRigidBody_SetPosition(*args)
    def GetPosition(*args): return _cspace.iRigidBody_GetPosition(*args)
    def SetOrientation(*args): return _cspace.iRigidBody_SetOrientation(*args)
    def GetOrientation(*args): return _cspace.iRigidBody_GetOrientation(*args)
    def SetTransform(*args): return _cspace.iRigidBody_SetTransform(*args)
    def GetTransform(*args): return _cspace.iRigidBody_GetTransform(*args)
    def SetLinearVelocity(*args): return _cspace.iRigidBody_SetLinearVelocity(*args)
    def GetLinearVelocity(*args): return _cspace.iRigidBody_GetLinearVelocity(*args)
    def SetAngularVelocity(*args): return _cspace.iRigidBody_SetAngularVelocity(*args)
    def GetAngularVelocity(*args): return _cspace.iRigidBody_GetAngularVelocity(*args)
    def SetProperties(*args): return _cspace.iRigidBody_SetProperties(*args)
    def GetProperties(*args): return _cspace.iRigidBody_GetProperties(*args)
    def GetMass(*args): return _cspace.iRigidBody_GetMass(*args)
    def GetCenter(*args): return _cspace.iRigidBody_GetCenter(*args)
    def GetInertia(*args): return _cspace.iRigidBody_GetInertia(*args)
    def AdjustTotalMass(*args): return _cspace.iRigidBody_AdjustTotalMass(*args)
    def AddForce(*args): return _cspace.iRigidBody_AddForce(*args)
    def AddTorque(*args): return _cspace.iRigidBody_AddTorque(*args)
    def AddRelForce(*args): return _cspace.iRigidBody_AddRelForce(*args)
    def AddRelTorque(*args): return _cspace.iRigidBody_AddRelTorque(*args)
    def AddForceAtPos(*args): return _cspace.iRigidBody_AddForceAtPos(*args)
    def AddForceAtRelPos(*args): return _cspace.iRigidBody_AddForceAtRelPos(*args)
    def AddRelForceAtPos(*args): return _cspace.iRigidBody_AddRelForceAtPos(*args)
    def AddRelForceAtRelPos(*args): return _cspace.iRigidBody_AddRelForceAtRelPos(*args)
    def GetForce(*args): return _cspace.iRigidBody_GetForce(*args)
    def GetTorque(*args): return _cspace.iRigidBody_GetTorque(*args)
    def AttachMesh(*args): return _cspace.iRigidBody_AttachMesh(*args)
    def GetAttachedMesh(*args): return _cspace.iRigidBody_GetAttachedMesh(*args)
    def AttachLight(*args): return _cspace.iRigidBody_AttachLight(*args)
    def GetAttachedLight(*args): return _cspace.iRigidBody_GetAttachedLight(*args)
    def AttachCamera(*args): return _cspace.iRigidBody_AttachCamera(*args)
    def GetAttachedCamera(*args): return _cspace.iRigidBody_GetAttachedCamera(*args)
    def SetMoveCallback(*args): return _cspace.iRigidBody_SetMoveCallback(*args)
    def SetCollisionCallback(*args): return _cspace.iRigidBody_SetCollisionCallback(*args)
    def Collision(*args): return _cspace.iRigidBody_Collision(*args)
    def Update(*args): return _cspace.iRigidBody_Update(*args)
    def GetCollider(*args): return _cspace.iRigidBody_GetCollider(*args)
    def GetColliderCount(*args): return _cspace.iRigidBody_GetColliderCount(*args)
    __swig_destroy__ = _cspace.delete_iRigidBody
    __del__ = lambda self : None;
iRigidBody_swigregister = _cspace.iRigidBody_swigregister
iRigidBody_swigregister(iRigidBody)

NO_GEOMETRY = _cspace.NO_GEOMETRY
BOX_COLLIDER_GEOMETRY = _cspace.BOX_COLLIDER_GEOMETRY
PLANE_COLLIDER_GEOMETRY = _cspace.PLANE_COLLIDER_GEOMETRY
TRIMESH_COLLIDER_GEOMETRY = _cspace.TRIMESH_COLLIDER_GEOMETRY
CYLINDER_COLLIDER_GEOMETRY = _cspace.CYLINDER_COLLIDER_GEOMETRY
CAPSULE_COLLIDER_GEOMETRY = _cspace.CAPSULE_COLLIDER_GEOMETRY
SPHERE_COLLIDER_GEOMETRY = _cspace.SPHERE_COLLIDER_GEOMETRY
class iDynamicsColliderCollisionCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDynamicsColliderCollisionCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDynamicsColliderCollisionCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Execute(*args): return _cspace.iDynamicsColliderCollisionCallback_Execute(*args)
    __swig_destroy__ = _cspace.delete_iDynamicsColliderCollisionCallback
    __del__ = lambda self : None;
iDynamicsColliderCollisionCallback_swigregister = _cspace.iDynamicsColliderCollisionCallback_swigregister
iDynamicsColliderCollisionCallback_swigregister(iDynamicsColliderCollisionCallback)

class iDynamicsSystemCollider(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iDynamicsSystemCollider, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iDynamicsSystemCollider, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CreateSphereGeometry(*args): return _cspace.iDynamicsSystemCollider_CreateSphereGeometry(*args)
    def CreatePlaneGeometry(*args): return _cspace.iDynamicsSystemCollider_CreatePlaneGeometry(*args)
    def CreateMeshGeometry(*args): return _cspace.iDynamicsSystemCollider_CreateMeshGeometry(*args)
    def CreateBoxGeometry(*args): return _cspace.iDynamicsSystemCollider_CreateBoxGeometry(*args)
    def CreateCapsuleGeometry(*args): return _cspace.iDynamicsSystemCollider_CreateCapsuleGeometry(*args)
    def SetCollisionCallback(*args): return _cspace.iDynamicsSystemCollider_SetCollisionCallback(*args)
    def SetFriction(*args): return _cspace.iDynamicsSystemCollider_SetFriction(*args)
    def SetSoftness(*args): return _cspace.iDynamicsSystemCollider_SetSoftness(*args)
    def SetDensity(*args): return _cspace.iDynamicsSystemCollider_SetDensity(*args)
    def SetElasticity(*args): return _cspace.iDynamicsSystemCollider_SetElasticity(*args)
    def GetFriction(*args): return _cspace.iDynamicsSystemCollider_GetFriction(*args)
    def GetSoftness(*args): return _cspace.iDynamicsSystemCollider_GetSoftness(*args)
    def GetDensity(*args): return _cspace.iDynamicsSystemCollider_GetDensity(*args)
    def GetElasticity(*args): return _cspace.iDynamicsSystemCollider_GetElasticity(*args)
    def FillWithColliderGeometry(*args): return _cspace.iDynamicsSystemCollider_FillWithColliderGeometry(*args)
    def GetGeometryType(*args): return _cspace.iDynamicsSystemCollider_GetGeometryType(*args)
    def GetTransform(*args): return _cspace.iDynamicsSystemCollider_GetTransform(*args)
    def GetLocalTransform(*args): return _cspace.iDynamicsSystemCollider_GetLocalTransform(*args)
    def SetTransform(*args): return _cspace.iDynamicsSystemCollider_SetTransform(*args)
    def GetBoxGeometry(*args): return _cspace.iDynamicsSystemCollider_GetBoxGeometry(*args)
    def GetSphereGeometry(*args): return _cspace.iDynamicsSystemCollider_GetSphereGeometry(*args)
    def GetPlaneGeometry(*args): return _cspace.iDynamicsSystemCollider_GetPlaneGeometry(*args)
    def GetCylinderGeometry(*args): return _cspace.iDynamicsSystemCollider_GetCylinderGeometry(*args)
    def MakeStatic(*args): return _cspace.iDynamicsSystemCollider_MakeStatic(*args)
    def MakeDynamic(*args): return _cspace.iDynamicsSystemCollider_MakeDynamic(*args)
    def IsStatic(*args): return _cspace.iDynamicsSystemCollider_IsStatic(*args)
    __swig_destroy__ = _cspace.delete_iDynamicsSystemCollider
    __del__ = lambda self : None;
iDynamicsSystemCollider_swigregister = _cspace.iDynamicsSystemCollider_swigregister
iDynamicsSystemCollider_swigregister(iDynamicsSystemCollider)

class iJoint(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iJoint, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iJoint, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Attach(*args): return _cspace.iJoint_Attach(*args)
    def GetAttachedBody(*args): return _cspace.iJoint_GetAttachedBody(*args)
    def SetTransform(*args): return _cspace.iJoint_SetTransform(*args)
    def GetTransform(*args): return _cspace.iJoint_GetTransform(*args)
    def SetTransConstraints(*args): return _cspace.iJoint_SetTransConstraints(*args)
    def IsXTransConstrained(*args): return _cspace.iJoint_IsXTransConstrained(*args)
    def IsYTransConstrained(*args): return _cspace.iJoint_IsYTransConstrained(*args)
    def IsZTransConstrained(*args): return _cspace.iJoint_IsZTransConstrained(*args)
    def SetMinimumDistance(*args): return _cspace.iJoint_SetMinimumDistance(*args)
    def GetMinimumDistance(*args): return _cspace.iJoint_GetMinimumDistance(*args)
    def SetMaximumDistance(*args): return _cspace.iJoint_SetMaximumDistance(*args)
    def GetMaximumDistance(*args): return _cspace.iJoint_GetMaximumDistance(*args)
    def SetRotConstraints(*args): return _cspace.iJoint_SetRotConstraints(*args)
    def IsXRotConstrained(*args): return _cspace.iJoint_IsXRotConstrained(*args)
    def IsYRotConstrained(*args): return _cspace.iJoint_IsYRotConstrained(*args)
    def IsZRotConstrained(*args): return _cspace.iJoint_IsZRotConstrained(*args)
    def SetMinimumAngle(*args): return _cspace.iJoint_SetMinimumAngle(*args)
    def GetMinimumAngle(*args): return _cspace.iJoint_GetMinimumAngle(*args)
    def SetMaximumAngle(*args): return _cspace.iJoint_SetMaximumAngle(*args)
    def GetMaximumAngle(*args): return _cspace.iJoint_GetMaximumAngle(*args)
    def SetBounce(*args): return _cspace.iJoint_SetBounce(*args)
    def GetBounce(*args): return _cspace.iJoint_GetBounce(*args)
    def SetDesiredVelocity(*args): return _cspace.iJoint_SetDesiredVelocity(*args)
    def GetDesiredVelocity(*args): return _cspace.iJoint_GetDesiredVelocity(*args)
    def SetMaxForce(*args): return _cspace.iJoint_SetMaxForce(*args)
    def GetMaxForce(*args): return _cspace.iJoint_GetMaxForce(*args)
    def SetAngularConstraintAxis(*args): return _cspace.iJoint_SetAngularConstraintAxis(*args)
    def GetAngularConstraintAxis(*args): return _cspace.iJoint_GetAngularConstraintAxis(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iJoint_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iJoint_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iJoint
    __del__ = lambda self : None;
iJoint_swigregister = _cspace.iJoint_swigregister
iJoint_swigregister(iJoint)
iJoint_scfGetVersion = _cspace.iJoint_scfGetVersion

class iODEFrameUpdateCallback(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iODEFrameUpdateCallback, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iODEFrameUpdateCallback, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Execute(*args): return _cspace.iODEFrameUpdateCallback_Execute(*args)
    __swig_destroy__ = _cspace.delete_iODEFrameUpdateCallback
    __del__ = lambda self : None;
iODEFrameUpdateCallback_swigregister = _cspace.iODEFrameUpdateCallback_swigregister
iODEFrameUpdateCallback_swigregister(iODEFrameUpdateCallback)

class iODEDynamicState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iODEDynamicState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iODEDynamicState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetGlobalERP(*args): return _cspace.iODEDynamicState_SetGlobalERP(*args)
    def GlobalERP(*args): return _cspace.iODEDynamicState_GlobalERP(*args)
    def SetGlobalCFM(*args): return _cspace.iODEDynamicState_SetGlobalCFM(*args)
    def GlobalCFM(*args): return _cspace.iODEDynamicState_GlobalCFM(*args)
    def EnableStepFast(*args): return _cspace.iODEDynamicState_EnableStepFast(*args)
    def StepFastEnabled(*args): return _cspace.iODEDynamicState_StepFastEnabled(*args)
    def SetStepFastIterations(*args): return _cspace.iODEDynamicState_SetStepFastIterations(*args)
    def StepFastIterations(*args): return _cspace.iODEDynamicState_StepFastIterations(*args)
    def EnableQuickStep(*args): return _cspace.iODEDynamicState_EnableQuickStep(*args)
    def QuickStepEnabled(*args): return _cspace.iODEDynamicState_QuickStepEnabled(*args)
    def SetQuickStepIterations(*args): return _cspace.iODEDynamicState_SetQuickStepIterations(*args)
    def QuickStepIterations(*args): return _cspace.iODEDynamicState_QuickStepIterations(*args)
    def EnableFrameRate(*args): return _cspace.iODEDynamicState_EnableFrameRate(*args)
    def FrameRateEnabled(*args): return _cspace.iODEDynamicState_FrameRateEnabled(*args)
    def SetFrameRate(*args): return _cspace.iODEDynamicState_SetFrameRate(*args)
    def FrameRate(*args): return _cspace.iODEDynamicState_FrameRate(*args)
    def SetFrameLimit(*args): return _cspace.iODEDynamicState_SetFrameLimit(*args)
    def FrameLimit(*args): return _cspace.iODEDynamicState_FrameLimit(*args)
    def AddFrameUpdateCallback(*args): return _cspace.iODEDynamicState_AddFrameUpdateCallback(*args)
    def RemoveFrameUpdateCallback(*args): return _cspace.iODEDynamicState_RemoveFrameUpdateCallback(*args)
    def EnableEventProcessing(*args): return _cspace.iODEDynamicState_EnableEventProcessing(*args)
    def EventProcessingEnabled(*args): return _cspace.iODEDynamicState_EventProcessingEnabled(*args)
    def EnableFastObjects(*args): return _cspace.iODEDynamicState_EnableFastObjects(*args)
    def FastObjectsEnabled(*args): return _cspace.iODEDynamicState_FastObjectsEnabled(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iODEDynamicState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iODEDynamicState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iODEDynamicState
    __del__ = lambda self : None;
iODEDynamicState_swigregister = _cspace.iODEDynamicState_swigregister
iODEDynamicState_swigregister(iODEDynamicState)
iODEDynamicState_scfGetVersion = _cspace.iODEDynamicState_scfGetVersion

class iODEDynamicSystemState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iODEDynamicSystemState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iODEDynamicSystemState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetERP(*args): return _cspace.iODEDynamicSystemState_SetERP(*args)
    def ERP(*args): return _cspace.iODEDynamicSystemState_ERP(*args)
    def SetCFM(*args): return _cspace.iODEDynamicSystemState_SetCFM(*args)
    def CFM(*args): return _cspace.iODEDynamicSystemState_CFM(*args)
    def EnableStepFast(*args): return _cspace.iODEDynamicSystemState_EnableStepFast(*args)
    def StepFastEnabled(*args): return _cspace.iODEDynamicSystemState_StepFastEnabled(*args)
    def SetStepFastIterations(*args): return _cspace.iODEDynamicSystemState_SetStepFastIterations(*args)
    def StepFastIterations(*args): return _cspace.iODEDynamicSystemState_StepFastIterations(*args)
    def EnableQuickStep(*args): return _cspace.iODEDynamicSystemState_EnableQuickStep(*args)
    def QuickStepEnabled(*args): return _cspace.iODEDynamicSystemState_QuickStepEnabled(*args)
    def SetQuickStepIterations(*args): return _cspace.iODEDynamicSystemState_SetQuickStepIterations(*args)
    def QuickStepIterations(*args): return _cspace.iODEDynamicSystemState_QuickStepIterations(*args)
    def EnableAutoDisable(*args): return _cspace.iODEDynamicSystemState_EnableAutoDisable(*args)
    def AutoDisableEnabled(*args): return _cspace.iODEDynamicSystemState_AutoDisableEnabled(*args)
    def SetAutoDisableParams(*args): return _cspace.iODEDynamicSystemState_SetAutoDisableParams(*args)
    def EnableFrameRate(*args): return _cspace.iODEDynamicSystemState_EnableFrameRate(*args)
    def FrameRateEnabled(*args): return _cspace.iODEDynamicSystemState_FrameRateEnabled(*args)
    def SetFrameRate(*args): return _cspace.iODEDynamicSystemState_SetFrameRate(*args)
    def FrameRate(*args): return _cspace.iODEDynamicSystemState_FrameRate(*args)
    def SetFrameLimit(*args): return _cspace.iODEDynamicSystemState_SetFrameLimit(*args)
    def FrameLimit(*args): return _cspace.iODEDynamicSystemState_FrameLimit(*args)
    def AddFrameUpdateCallback(*args): return _cspace.iODEDynamicSystemState_AddFrameUpdateCallback(*args)
    def RemoveFrameUpdateCallback(*args): return _cspace.iODEDynamicSystemState_RemoveFrameUpdateCallback(*args)
    def EnableFastObjects(*args): return _cspace.iODEDynamicSystemState_EnableFastObjects(*args)
    def FastObjectsEnabled(*args): return _cspace.iODEDynamicSystemState_FastObjectsEnabled(*args)
    def CreateBallJoint(*args): return _cspace.iODEDynamicSystemState_CreateBallJoint(*args)
    def CreateHingeJoint(*args): return _cspace.iODEDynamicSystemState_CreateHingeJoint(*args)
    def CreateHinge2Joint(*args): return _cspace.iODEDynamicSystemState_CreateHinge2Joint(*args)
    def CreateAMotorJoint(*args): return _cspace.iODEDynamicSystemState_CreateAMotorJoint(*args)
    def CreateUniversalJoint(*args): return _cspace.iODEDynamicSystemState_CreateUniversalJoint(*args)
    def CreateSliderJoint(*args): return _cspace.iODEDynamicSystemState_CreateSliderJoint(*args)
    def RemoveJoint(*args): return _cspace.iODEDynamicSystemState_RemoveJoint(*args)
    def SetContactMaxCorrectingVel(*args): return _cspace.iODEDynamicSystemState_SetContactMaxCorrectingVel(*args)
    def GetContactMaxCorrectingVel(*args): return _cspace.iODEDynamicSystemState_GetContactMaxCorrectingVel(*args)
    def SetContactSurfaceLayer(*args): return _cspace.iODEDynamicSystemState_SetContactSurfaceLayer(*args)
    def GetContactSurfaceLayer(*args): return _cspace.iODEDynamicSystemState_GetContactSurfaceLayer(*args)
    def EnableOldInertia(*args): return _cspace.iODEDynamicSystemState_EnableOldInertia(*args)
    def IsOldInertiaEnabled(*args): return _cspace.iODEDynamicSystemState_IsOldInertiaEnabled(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iODEDynamicSystemState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iODEDynamicSystemState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iODEDynamicSystemState
    __del__ = lambda self : None;
iODEDynamicSystemState_swigregister = _cspace.iODEDynamicSystemState_swigregister
iODEDynamicSystemState_swigregister(iODEDynamicSystemState)
iODEDynamicSystemState_scfGetVersion = _cspace.iODEDynamicSystemState_scfGetVersion

CS_ODE_JOINT_TYPE_UNKNOWN = _cspace.CS_ODE_JOINT_TYPE_UNKNOWN
CS_ODE_JOINT_TYPE_BALL = _cspace.CS_ODE_JOINT_TYPE_BALL
CS_ODE_JOINT_TYPE_HINGE = _cspace.CS_ODE_JOINT_TYPE_HINGE
CS_ODE_JOINT_TYPE_SLIDER = _cspace.CS_ODE_JOINT_TYPE_SLIDER
CS_ODE_JOINT_TYPE_CONTACT = _cspace.CS_ODE_JOINT_TYPE_CONTACT
CS_ODE_JOINT_TYPE_UNIVERSAL = _cspace.CS_ODE_JOINT_TYPE_UNIVERSAL
CS_ODE_JOINT_TYPE_HINGE2 = _cspace.CS_ODE_JOINT_TYPE_HINGE2
CS_ODE_JOINT_TYPE_FIXED = _cspace.CS_ODE_JOINT_TYPE_FIXED
CS_ODE_JOINT_TYPE_AMOTOR = _cspace.CS_ODE_JOINT_TYPE_AMOTOR
class iODEJointState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iODEJointState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iODEJointState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetType(*args): return _cspace.iODEJointState_GetType(*args)
    def SetLoStop(*args): return _cspace.iODEJointState_SetLoStop(*args)
    def SetHiStop(*args): return _cspace.iODEJointState_SetHiStop(*args)
    def SetVel(*args): return _cspace.iODEJointState_SetVel(*args)
    def SetFMax(*args): return _cspace.iODEJointState_SetFMax(*args)
    def SetFudgeFactor(*args): return _cspace.iODEJointState_SetFudgeFactor(*args)
    def SetBounce(*args): return _cspace.iODEJointState_SetBounce(*args)
    def SetCFM(*args): return _cspace.iODEJointState_SetCFM(*args)
    def SetStopERP(*args): return _cspace.iODEJointState_SetStopERP(*args)
    def SetStopCFM(*args): return _cspace.iODEJointState_SetStopCFM(*args)
    def SetSuspensionERP(*args): return _cspace.iODEJointState_SetSuspensionERP(*args)
    def SetSuspensionCFM(*args): return _cspace.iODEJointState_SetSuspensionCFM(*args)
    def GetLoStop(*args): return _cspace.iODEJointState_GetLoStop(*args)
    def GetHiStop(*args): return _cspace.iODEJointState_GetHiStop(*args)
    def GetVel(*args): return _cspace.iODEJointState_GetVel(*args)
    def GetMaxForce(*args): return _cspace.iODEJointState_GetMaxForce(*args)
    def GetFudgeFactor(*args): return _cspace.iODEJointState_GetFudgeFactor(*args)
    def GetBounce(*args): return _cspace.iODEJointState_GetBounce(*args)
    def GetCFM(*args): return _cspace.iODEJointState_GetCFM(*args)
    def GetStopERP(*args): return _cspace.iODEJointState_GetStopERP(*args)
    def GetStopCFM(*args): return _cspace.iODEJointState_GetStopCFM(*args)
    def GetSuspensionERP(*args): return _cspace.iODEJointState_GetSuspensionERP(*args)
    def GetSuspensionCFM(*args): return _cspace.iODEJointState_GetSuspensionCFM(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iODEJointState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iODEJointState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iODEJointState
    __del__ = lambda self : None;
iODEJointState_swigregister = _cspace.iODEJointState_swigregister
iODEJointState_swigregister(iODEJointState)
iODEJointState_scfGetVersion = _cspace.iODEJointState_scfGetVersion

class iODEGeneralJointState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iODEGeneralJointState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iODEGeneralJointState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetLoStop(*args): return _cspace.iODEGeneralJointState_SetLoStop(*args)
    def SetHiStop(*args): return _cspace.iODEGeneralJointState_SetHiStop(*args)
    def SetVel(*args): return _cspace.iODEGeneralJointState_SetVel(*args)
    def SetFMax(*args): return _cspace.iODEGeneralJointState_SetFMax(*args)
    def SetFudgeFactor(*args): return _cspace.iODEGeneralJointState_SetFudgeFactor(*args)
    def SetBounce(*args): return _cspace.iODEGeneralJointState_SetBounce(*args)
    def SetCFM(*args): return _cspace.iODEGeneralJointState_SetCFM(*args)
    def SetStopERP(*args): return _cspace.iODEGeneralJointState_SetStopERP(*args)
    def SetStopCFM(*args): return _cspace.iODEGeneralJointState_SetStopCFM(*args)
    def SetSuspensionERP(*args): return _cspace.iODEGeneralJointState_SetSuspensionERP(*args)
    def SetSuspensionCFM(*args): return _cspace.iODEGeneralJointState_SetSuspensionCFM(*args)
    def GetLoStop(*args): return _cspace.iODEGeneralJointState_GetLoStop(*args)
    def GetHiStop(*args): return _cspace.iODEGeneralJointState_GetHiStop(*args)
    def GetVel(*args): return _cspace.iODEGeneralJointState_GetVel(*args)
    def GetFMax(*args): return _cspace.iODEGeneralJointState_GetFMax(*args)
    def GetFudgeFactor(*args): return _cspace.iODEGeneralJointState_GetFudgeFactor(*args)
    def GetBounce(*args): return _cspace.iODEGeneralJointState_GetBounce(*args)
    def GetCFM(*args): return _cspace.iODEGeneralJointState_GetCFM(*args)
    def GetStopERP(*args): return _cspace.iODEGeneralJointState_GetStopERP(*args)
    def GetStopCFM(*args): return _cspace.iODEGeneralJointState_GetStopCFM(*args)
    def GetSuspensionERP(*args): return _cspace.iODEGeneralJointState_GetSuspensionERP(*args)
    def GetSuspensionCFM(*args): return _cspace.iODEGeneralJointState_GetSuspensionCFM(*args)
    def Attach(*args): return _cspace.iODEGeneralJointState_Attach(*args)
    def GetAttachedBody(*args): return _cspace.iODEGeneralJointState_GetAttachedBody(*args)
    def GetFeedbackForce1(*args): return _cspace.iODEGeneralJointState_GetFeedbackForce1(*args)
    def GetFeedbackTorque1(*args): return _cspace.iODEGeneralJointState_GetFeedbackTorque1(*args)
    def GetFeedbackForce2(*args): return _cspace.iODEGeneralJointState_GetFeedbackForce2(*args)
    def GetFeedbackTorque2(*args): return _cspace.iODEGeneralJointState_GetFeedbackTorque2(*args)
    __swig_destroy__ = _cspace.delete_iODEGeneralJointState
    __del__ = lambda self : None;
iODEGeneralJointState_swigregister = _cspace.iODEGeneralJointState_swigregister
iODEGeneralJointState_swigregister(iODEGeneralJointState)

class iODESliderJoint(iODEGeneralJointState):
    __swig_setmethods__ = {}
    for _s in [iODEGeneralJointState]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iODESliderJoint, name, value)
    __swig_getmethods__ = {}
    for _s in [iODEGeneralJointState]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iODESliderJoint, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetSliderAxis(*args): return _cspace.iODESliderJoint_SetSliderAxis(*args)
    def GetSliderAxis(*args): return _cspace.iODESliderJoint_GetSliderAxis(*args)
    def GetSliderPosition(*args): return _cspace.iODESliderJoint_GetSliderPosition(*args)
    def GetSliderPositionRate(*args): return _cspace.iODESliderJoint_GetSliderPositionRate(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iODESliderJoint_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iODESliderJoint_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iODESliderJoint
    __del__ = lambda self : None;
iODESliderJoint_swigregister = _cspace.iODESliderJoint_swigregister
iODESliderJoint_swigregister(iODESliderJoint)
iODESliderJoint_scfGetVersion = _cspace.iODESliderJoint_scfGetVersion

class iODEUniversalJoint(iODEGeneralJointState):
    __swig_setmethods__ = {}
    for _s in [iODEGeneralJointState]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iODEUniversalJoint, name, value)
    __swig_getmethods__ = {}
    for _s in [iODEGeneralJointState]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iODEUniversalJoint, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetUniversalAnchor(*args): return _cspace.iODEUniversalJoint_SetUniversalAnchor(*args)
    def SetUniversalAxis1(*args): return _cspace.iODEUniversalJoint_SetUniversalAxis1(*args)
    def SetUniversalAxis2(*args): return _cspace.iODEUniversalJoint_SetUniversalAxis2(*args)
    def GetUniversalAnchor1(*args): return _cspace.iODEUniversalJoint_GetUniversalAnchor1(*args)
    def GetUniversalAnchor2(*args): return _cspace.iODEUniversalJoint_GetUniversalAnchor2(*args)
    def GetUniversalAxis1(*args): return _cspace.iODEUniversalJoint_GetUniversalAxis1(*args)
    def GetUniversalAxis2(*args): return _cspace.iODEUniversalJoint_GetUniversalAxis2(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iODEUniversalJoint_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iODEUniversalJoint_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iODEUniversalJoint
    __del__ = lambda self : None;
iODEUniversalJoint_swigregister = _cspace.iODEUniversalJoint_swigregister
iODEUniversalJoint_swigregister(iODEUniversalJoint)
iODEUniversalJoint_scfGetVersion = _cspace.iODEUniversalJoint_scfGetVersion

CS_ODE_AMOTOR_MODE_UNKNOWN = _cspace.CS_ODE_AMOTOR_MODE_UNKNOWN
CS_ODE_AMOTOR_MODE_USER = _cspace.CS_ODE_AMOTOR_MODE_USER
CS_ODE_AMOTOR_MODE_EULER = _cspace.CS_ODE_AMOTOR_MODE_EULER
CS_ODE_AMOTOR_MODE_LAST = _cspace.CS_ODE_AMOTOR_MODE_LAST
class iODEAMotorJoint(iODEGeneralJointState):
    __swig_setmethods__ = {}
    for _s in [iODEGeneralJointState]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iODEAMotorJoint, name, value)
    __swig_getmethods__ = {}
    for _s in [iODEGeneralJointState]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iODEAMotorJoint, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetAMotorMode(*args): return _cspace.iODEAMotorJoint_SetAMotorMode(*args)
    def GetAMotorMode(*args): return _cspace.iODEAMotorJoint_GetAMotorMode(*args)
    def SetAMotorNumAxes(*args): return _cspace.iODEAMotorJoint_SetAMotorNumAxes(*args)
    def GetAMotorNumAxes(*args): return _cspace.iODEAMotorJoint_GetAMotorNumAxes(*args)
    def SetAMotorAxis(*args): return _cspace.iODEAMotorJoint_SetAMotorAxis(*args)
    def GetAMotorAxis(*args): return _cspace.iODEAMotorJoint_GetAMotorAxis(*args)
    def GetAMotorAxisRelOrientation(*args): return _cspace.iODEAMotorJoint_GetAMotorAxisRelOrientation(*args)
    def SetAMotorAngle(*args): return _cspace.iODEAMotorJoint_SetAMotorAngle(*args)
    def GetAMotorAngle(*args): return _cspace.iODEAMotorJoint_GetAMotorAngle(*args)
    def GetAMotorAngleRate(*args): return _cspace.iODEAMotorJoint_GetAMotorAngleRate(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iODEAMotorJoint_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iODEAMotorJoint_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iODEAMotorJoint
    __del__ = lambda self : None;
iODEAMotorJoint_swigregister = _cspace.iODEAMotorJoint_swigregister
iODEAMotorJoint_swigregister(iODEAMotorJoint)
iODEAMotorJoint_scfGetVersion = _cspace.iODEAMotorJoint_scfGetVersion

class iODEHinge2Joint(iODEGeneralJointState):
    __swig_setmethods__ = {}
    for _s in [iODEGeneralJointState]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iODEHinge2Joint, name, value)
    __swig_getmethods__ = {}
    for _s in [iODEGeneralJointState]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iODEHinge2Joint, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetHingeAnchor(*args): return _cspace.iODEHinge2Joint_SetHingeAnchor(*args)
    def SetHingeAxis1(*args): return _cspace.iODEHinge2Joint_SetHingeAxis1(*args)
    def SetHingeAxis2(*args): return _cspace.iODEHinge2Joint_SetHingeAxis2(*args)
    def GetHingeAnchor1(*args): return _cspace.iODEHinge2Joint_GetHingeAnchor1(*args)
    def GetHingeAnchor2(*args): return _cspace.iODEHinge2Joint_GetHingeAnchor2(*args)
    def GetHingeAxis1(*args): return _cspace.iODEHinge2Joint_GetHingeAxis1(*args)
    def GetHingeAxis2(*args): return _cspace.iODEHinge2Joint_GetHingeAxis2(*args)
    def GetHingeAngle(*args): return _cspace.iODEHinge2Joint_GetHingeAngle(*args)
    def GetHingeAngleRate1(*args): return _cspace.iODEHinge2Joint_GetHingeAngleRate1(*args)
    def GetHingeAngleRate2(*args): return _cspace.iODEHinge2Joint_GetHingeAngleRate2(*args)
    def GetAnchorError(*args): return _cspace.iODEHinge2Joint_GetAnchorError(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iODEHinge2Joint_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iODEHinge2Joint_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iODEHinge2Joint
    __del__ = lambda self : None;
iODEHinge2Joint_swigregister = _cspace.iODEHinge2Joint_swigregister
iODEHinge2Joint_swigregister(iODEHinge2Joint)
iODEHinge2Joint_scfGetVersion = _cspace.iODEHinge2Joint_scfGetVersion

class iODEHingeJoint(iODEGeneralJointState):
    __swig_setmethods__ = {}
    for _s in [iODEGeneralJointState]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iODEHingeJoint, name, value)
    __swig_getmethods__ = {}
    for _s in [iODEGeneralJointState]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iODEHingeJoint, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetHingeAnchor(*args): return _cspace.iODEHingeJoint_SetHingeAnchor(*args)
    def SetHingeAxis(*args): return _cspace.iODEHingeJoint_SetHingeAxis(*args)
    def GetHingeAnchor1(*args): return _cspace.iODEHingeJoint_GetHingeAnchor1(*args)
    def GetHingeAnchor2(*args): return _cspace.iODEHingeJoint_GetHingeAnchor2(*args)
    def GetHingeAxis(*args): return _cspace.iODEHingeJoint_GetHingeAxis(*args)
    def GetHingeAngle(*args): return _cspace.iODEHingeJoint_GetHingeAngle(*args)
    def GetHingeAngleRate(*args): return _cspace.iODEHingeJoint_GetHingeAngleRate(*args)
    def GetAnchorError(*args): return _cspace.iODEHingeJoint_GetAnchorError(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iODEHingeJoint_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iODEHingeJoint_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iODEHingeJoint
    __del__ = lambda self : None;
iODEHingeJoint_swigregister = _cspace.iODEHingeJoint_swigregister
iODEHingeJoint_swigregister(iODEHingeJoint)
iODEHingeJoint_scfGetVersion = _cspace.iODEHingeJoint_scfGetVersion

class iODEBallJoint(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iODEBallJoint, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iODEBallJoint, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetBallAnchor(*args): return _cspace.iODEBallJoint_SetBallAnchor(*args)
    def GetBallAnchor1(*args): return _cspace.iODEBallJoint_GetBallAnchor1(*args)
    def GetBallAnchor2(*args): return _cspace.iODEBallJoint_GetBallAnchor2(*args)
    def GetAnchorError(*args): return _cspace.iODEBallJoint_GetAnchorError(*args)
    def Attach(*args): return _cspace.iODEBallJoint_Attach(*args)
    def GetAttachedBody(*args): return _cspace.iODEBallJoint_GetAttachedBody(*args)
    def GetFeedbackForce1(*args): return _cspace.iODEBallJoint_GetFeedbackForce1(*args)
    def GetFeedbackTorque1(*args): return _cspace.iODEBallJoint_GetFeedbackTorque1(*args)
    def GetFeedbackForce2(*args): return _cspace.iODEBallJoint_GetFeedbackForce2(*args)
    def GetFeedbackTorque2(*args): return _cspace.iODEBallJoint_GetFeedbackTorque2(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iODEBallJoint_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iODEBallJoint_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iODEBallJoint
    __del__ = lambda self : None;
iODEBallJoint_swigregister = _cspace.iODEBallJoint_swigregister
iODEBallJoint_swigregister(iODEBallJoint)
iODEBallJoint_scfGetVersion = _cspace.iODEBallJoint_scfGetVersion

CS_SEQUENCE_LIGHTCHANGE_NONE = _cspace.CS_SEQUENCE_LIGHTCHANGE_NONE
CS_SEQUENCE_LIGHTCHANGE_LESS = _cspace.CS_SEQUENCE_LIGHTCHANGE_LESS
CS_SEQUENCE_LIGHTCHANGE_GREATER = _cspace.CS_SEQUENCE_LIGHTCHANGE_GREATER
class iParameterESM(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iParameterESM, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iParameterESM, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetValue(*args): return _cspace.iParameterESM_GetValue(*args)
    def IsConstant(*args): return _cspace.iParameterESM_IsConstant(*args)
    __swig_destroy__ = _cspace.delete_iParameterESM
    __del__ = lambda self : None;
iParameterESM_swigregister = _cspace.iParameterESM_swigregister
iParameterESM_swigregister(iParameterESM)

class iEngineSequenceParameters(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iEngineSequenceParameters, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iEngineSequenceParameters, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetParameterCount(*args): return _cspace.iEngineSequenceParameters_GetParameterCount(*args)
    def GetParameter(*args): return _cspace.iEngineSequenceParameters_GetParameter(*args)
    def GetParameterIdx(*args): return _cspace.iEngineSequenceParameters_GetParameterIdx(*args)
    def GetParameterName(*args): return _cspace.iEngineSequenceParameters_GetParameterName(*args)
    def AddParameter(*args): return _cspace.iEngineSequenceParameters_AddParameter(*args)
    def SetParameter(*args): return _cspace.iEngineSequenceParameters_SetParameter(*args)
    def CreateParameterESM(*args): return _cspace.iEngineSequenceParameters_CreateParameterESM(*args)
    __swig_destroy__ = _cspace.delete_iEngineSequenceParameters
    __del__ = lambda self : None;
iEngineSequenceParameters_swigregister = _cspace.iEngineSequenceParameters_swigregister
iEngineSequenceParameters_swigregister(iEngineSequenceParameters)

class iSequenceWrapper(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSequenceWrapper, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSequenceWrapper, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iSequenceWrapper_QueryObject(*args)
    def GetSequence(*args): return _cspace.iSequenceWrapper_GetSequence(*args)
    def CreateBaseParameterBlock(*args): return _cspace.iSequenceWrapper_CreateBaseParameterBlock(*args)
    def GetBaseParameterBlock(*args): return _cspace.iSequenceWrapper_GetBaseParameterBlock(*args)
    def CreateParameterBlock(*args): return _cspace.iSequenceWrapper_CreateParameterBlock(*args)
    def AddOperationSetVariable(*args): return _cspace.iSequenceWrapper_AddOperationSetVariable(*args)
    def AddOperationSetMaterial(*args): return _cspace.iSequenceWrapper_AddOperationSetMaterial(*args)
    def AddOperationSetPolygonMaterial(*args): return _cspace.iSequenceWrapper_AddOperationSetPolygonMaterial(*args)
    def AddOperationSetLight(*args): return _cspace.iSequenceWrapper_AddOperationSetLight(*args)
    def AddOperationFadeLight(*args): return _cspace.iSequenceWrapper_AddOperationFadeLight(*args)
    def AddOperationSetAmbient(*args): return _cspace.iSequenceWrapper_AddOperationSetAmbient(*args)
    def AddOperationFadeAmbient(*args): return _cspace.iSequenceWrapper_AddOperationFadeAmbient(*args)
    def AddOperationRandomDelay(*args): return _cspace.iSequenceWrapper_AddOperationRandomDelay(*args)
    def AddOperationSetMeshColor(*args): return _cspace.iSequenceWrapper_AddOperationSetMeshColor(*args)
    def AddOperationFadeMeshColor(*args): return _cspace.iSequenceWrapper_AddOperationFadeMeshColor(*args)
    def AddOperationSetFog(*args): return _cspace.iSequenceWrapper_AddOperationSetFog(*args)
    def AddOperationFadeFog(*args): return _cspace.iSequenceWrapper_AddOperationFadeFog(*args)
    def AddOperationRotateDuration(*args): return _cspace.iSequenceWrapper_AddOperationRotateDuration(*args)
    def AddOperationMoveDuration(*args): return _cspace.iSequenceWrapper_AddOperationMoveDuration(*args)
    def AddOperationTriggerState(*args): return _cspace.iSequenceWrapper_AddOperationTriggerState(*args)
    def AddOperationCheckTrigger(*args): return _cspace.iSequenceWrapper_AddOperationCheckTrigger(*args)
    def AddOperationTestTrigger(*args): return _cspace.iSequenceWrapper_AddOperationTestTrigger(*args)
    __swig_destroy__ = _cspace.delete_iSequenceWrapper
    __del__ = lambda self : None;
iSequenceWrapper_swigregister = _cspace.iSequenceWrapper_swigregister
iSequenceWrapper_swigregister(iSequenceWrapper)

class iSequenceTrigger(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSequenceTrigger, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSequenceTrigger, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iSequenceTrigger_QueryObject(*args)
    def AddConditionInSector(*args): return _cspace.iSequenceTrigger_AddConditionInSector(*args)
    def AddConditionSectorVisible(*args): return _cspace.iSequenceTrigger_AddConditionSectorVisible(*args)
    def AddConditionMeshClick(*args): return _cspace.iSequenceTrigger_AddConditionMeshClick(*args)
    def AddConditionLightChange(*args): return _cspace.iSequenceTrigger_AddConditionLightChange(*args)
    def AddConditionManual(*args): return _cspace.iSequenceTrigger_AddConditionManual(*args)
    def SetEnabled(*args): return _cspace.iSequenceTrigger_SetEnabled(*args)
    def IsEnabled(*args): return _cspace.iSequenceTrigger_IsEnabled(*args)
    def ClearConditions(*args): return _cspace.iSequenceTrigger_ClearConditions(*args)
    def Trigger(*args): return _cspace.iSequenceTrigger_Trigger(*args)
    def SetParameters(*args): return _cspace.iSequenceTrigger_SetParameters(*args)
    def GetParameters(*args): return _cspace.iSequenceTrigger_GetParameters(*args)
    def FireSequence(*args): return _cspace.iSequenceTrigger_FireSequence(*args)
    def GetFiredSequence(*args): return _cspace.iSequenceTrigger_GetFiredSequence(*args)
    def TestConditions(*args): return _cspace.iSequenceTrigger_TestConditions(*args)
    def CheckState(*args): return _cspace.iSequenceTrigger_CheckState(*args)
    def ForceFire(*args): return _cspace.iSequenceTrigger_ForceFire(*args)
    __swig_destroy__ = _cspace.delete_iSequenceTrigger
    __del__ = lambda self : None;
iSequenceTrigger_swigregister = _cspace.iSequenceTrigger_swigregister
iSequenceTrigger_swigregister(iSequenceTrigger)

class iSequenceTimedOperation(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSequenceTimedOperation, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSequenceTimedOperation, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Do(*args): return _cspace.iSequenceTimedOperation_Do(*args)
    __swig_destroy__ = _cspace.delete_iSequenceTimedOperation
    __del__ = lambda self : None;
iSequenceTimedOperation_swigregister = _cspace.iSequenceTimedOperation_swigregister
iSequenceTimedOperation_swigregister(iSequenceTimedOperation)

class iEngineSequenceManager(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iEngineSequenceManager, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iEngineSequenceManager, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetSequenceManager(*args): return _cspace.iEngineSequenceManager_GetSequenceManager(*args)
    def CreateParameterESM(*args): return _cspace.iEngineSequenceManager_CreateParameterESM(*args)
    def CreateTrigger(*args): return _cspace.iEngineSequenceManager_CreateTrigger(*args)
    def RemoveTrigger(*args): return _cspace.iEngineSequenceManager_RemoveTrigger(*args)
    def RemoveTriggers(*args): return _cspace.iEngineSequenceManager_RemoveTriggers(*args)
    def GetTriggerCount(*args): return _cspace.iEngineSequenceManager_GetTriggerCount(*args)
    def GetTrigger(*args): return _cspace.iEngineSequenceManager_GetTrigger(*args)
    def FindTriggerByName(*args): return _cspace.iEngineSequenceManager_FindTriggerByName(*args)
    def FireTriggerByName(*args): return _cspace.iEngineSequenceManager_FireTriggerByName(*args)
    def CreateSequence(*args): return _cspace.iEngineSequenceManager_CreateSequence(*args)
    def RemoveSequence(*args): return _cspace.iEngineSequenceManager_RemoveSequence(*args)
    def RemoveSequences(*args): return _cspace.iEngineSequenceManager_RemoveSequences(*args)
    def GetSequenceCount(*args): return _cspace.iEngineSequenceManager_GetSequenceCount(*args)
    def GetSequence(*args): return _cspace.iEngineSequenceManager_GetSequence(*args)
    def FindSequenceByName(*args): return _cspace.iEngineSequenceManager_FindSequenceByName(*args)
    def RunSequenceByName(*args): return _cspace.iEngineSequenceManager_RunSequenceByName(*args)
    def FireTimedOperation(*args): return _cspace.iEngineSequenceManager_FireTimedOperation(*args)
    def DestroyTimedOperations(*args): return _cspace.iEngineSequenceManager_DestroyTimedOperations(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iEngineSequenceManager_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iEngineSequenceManager_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iEngineSequenceManager
    __del__ = lambda self : None;
iEngineSequenceManager_swigregister = _cspace.iEngineSequenceManager_swigregister
iEngineSequenceManager_swigregister(iEngineSequenceManager)
iEngineSequenceManager_scfGetVersion = _cspace.iEngineSequenceManager_scfGetVersion

class iMovieRecorder(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMovieRecorder, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMovieRecorder, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Start(*args): return _cspace.iMovieRecorder_Start(*args)
    def Stop(*args): return _cspace.iMovieRecorder_Stop(*args)
    def IsRecording(*args): return _cspace.iMovieRecorder_IsRecording(*args)
    def Pause(*args): return _cspace.iMovieRecorder_Pause(*args)
    def UnPause(*args): return _cspace.iMovieRecorder_UnPause(*args)
    def IsPaused(*args): return _cspace.iMovieRecorder_IsPaused(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iMovieRecorder_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iMovieRecorder_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iMovieRecorder
    __del__ = lambda self : None;
iMovieRecorder_swigregister = _cspace.iMovieRecorder_swigregister
iMovieRecorder_swigregister(iMovieRecorder)
iMovieRecorder_scfGetVersion = _cspace.iMovieRecorder_scfGetVersion

class iMapNode(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iMapNode, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iMapNode, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def QueryObject(*args): return _cspace.iMapNode_QueryObject(*args)
    def SetPosition(*args): return _cspace.iMapNode_SetPosition(*args)
    def GetPosition(*args): return _cspace.iMapNode_GetPosition(*args)
    def SetXVector(*args): return _cspace.iMapNode_SetXVector(*args)
    def GetXVector(*args): return _cspace.iMapNode_GetXVector(*args)
    def SetYVector(*args): return _cspace.iMapNode_SetYVector(*args)
    def GetYVector(*args): return _cspace.iMapNode_GetYVector(*args)
    def SetZVector(*args): return _cspace.iMapNode_SetZVector(*args)
    def GetZVector(*args): return _cspace.iMapNode_GetZVector(*args)
    def SetSector(*args): return _cspace.iMapNode_SetSector(*args)
    def GetSector(*args): return _cspace.iMapNode_GetSector(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iMapNode_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iMapNode_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iMapNode
    __del__ = lambda self : None;
iMapNode_swigregister = _cspace.iMapNode_swigregister
iMapNode_swigregister(iMapNode)
iMapNode_scfGetVersion = _cspace.iMapNode_scfGetVersion

class iScriptValue(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iScriptValue, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iScriptValue, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    tInt = _cspace.iScriptValue_tInt
    tFloat = _cspace.iScriptValue_tFloat
    tDouble = _cspace.iScriptValue_tDouble
    tString = _cspace.iScriptValue_tString
    tBool = _cspace.iScriptValue_tBool
    tObject = _cspace.iScriptValue_tObject
    def GetScript(*args): return _cspace.iScriptValue_GetScript(*args)
    def GetTypes(*args): return _cspace.iScriptValue_GetTypes(*args)
    def GetInt(*args): return _cspace.iScriptValue_GetInt(*args)
    def GetFloat(*args): return _cspace.iScriptValue_GetFloat(*args)
    def GetDouble(*args): return _cspace.iScriptValue_GetDouble(*args)
    def GetString(*args): return _cspace.iScriptValue_GetString(*args)
    def GetBool(*args): return _cspace.iScriptValue_GetBool(*args)
    def GetObject(*args): return _cspace.iScriptValue_GetObject(*args)
    __swig_destroy__ = _cspace.delete_iScriptValue
    __del__ = lambda self : None;
iScriptValue_swigregister = _cspace.iScriptValue_swigregister
iScriptValue_swigregister(iScriptValue)

class iScriptObject(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iScriptObject, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iScriptObject, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetScript(*args): return _cspace.iScriptObject_GetScript(*args)
    def GetClass(*args): return _cspace.iScriptObject_GetClass(*args)
    def IsA(*args): return _cspace.iScriptObject_IsA(*args)
    def IsType(*args): return _cspace.iScriptObject_IsType(*args)
    def GetPointer(*args): return _cspace.iScriptObject_GetPointer(*args)
    def SetPointer(*args): return _cspace.iScriptObject_SetPointer(*args)
    def IntCall(*args): return _cspace.iScriptObject_IntCall(*args)
    def FloatCall(*args): return _cspace.iScriptObject_FloatCall(*args)
    def DoubleCall(*args): return _cspace.iScriptObject_DoubleCall(*args)
    def Call(*args): return _cspace.iScriptObject_Call(*args)
    def ObjectCall(*args): return _cspace.iScriptObject_ObjectCall(*args)
    def SetInt(*args): return _cspace.iScriptObject_SetInt(*args)
    def SetFloat(*args): return _cspace.iScriptObject_SetFloat(*args)
    def SetDouble(*args): return _cspace.iScriptObject_SetDouble(*args)
    def SetString(*args): return _cspace.iScriptObject_SetString(*args)
    def Set(*args): return _cspace.iScriptObject_Set(*args)
    def SetTruth(*args): return _cspace.iScriptObject_SetTruth(*args)
    def GetFloat(*args): return _cspace.iScriptObject_GetFloat(*args)
    def Get(*args): return _cspace.iScriptObject_Get(*args)
    def GetTruth(*args): return _cspace.iScriptObject_GetTruth(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iScriptObject_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iScriptObject_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iScriptObject
    __del__ = lambda self : None;
iScriptObject_swigregister = _cspace.iScriptObject_swigregister
iScriptObject_swigregister(iScriptObject)
iScriptObject_scfGetVersion = _cspace.iScriptObject_scfGetVersion

class iScript(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iScript, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iScript, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def RunText(*args): return _cspace.iScript_RunText(*args)
    def LoadModule(*args): return _cspace.iScript_LoadModule(*args)
    def LoadModuleNative(*args): return _cspace.iScript_LoadModuleNative(*args)
    def RValue(*args): return _cspace.iScript_RValue(*args)
    def New(*args): return _cspace.iScript_New(*args)
    def Remove(*args): return _cspace.iScript_Remove(*args)
    def NewObject(*args): return _cspace.iScript_NewObject(*args)
    def IntCall(*args): return _cspace.iScript_IntCall(*args)
    def FloatCall(*args): return _cspace.iScript_FloatCall(*args)
    def DoubleCall(*args): return _cspace.iScript_DoubleCall(*args)
    def Call(*args): return _cspace.iScript_Call(*args)
    def ObjectCall(*args): return _cspace.iScript_ObjectCall(*args)
    def StoreInt(*args): return _cspace.iScript_StoreInt(*args)
    def StoreFloat(*args): return _cspace.iScript_StoreFloat(*args)
    def StoreDouble(*args): return _cspace.iScript_StoreDouble(*args)
    def StoreString(*args): return _cspace.iScript_StoreString(*args)
    def Store(*args): return _cspace.iScript_Store(*args)
    def SetTruth(*args): return _cspace.iScript_SetTruth(*args)
    def RetrieveFloat(*args): return _cspace.iScript_RetrieveFloat(*args)
    def Retrieve(*args): return _cspace.iScript_Retrieve(*args)
    def GetTruth(*args): return _cspace.iScript_GetTruth(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iScript_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iScript_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iScript
    __del__ = lambda self : None;
iScript_swigregister = _cspace.iScript_swigregister
iScript_swigregister(iScript)
iScript_scfGetVersion = _cspace.iScript_scfGetVersion

class iSimpleFormerState(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iSimpleFormerState, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iSimpleFormerState, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetHeightmap(*args): return _cspace.iSimpleFormerState_SetHeightmap(*args)
    def SetScale(*args): return _cspace.iSimpleFormerState_SetScale(*args)
    def SetOffset(*args): return _cspace.iSimpleFormerState_SetOffset(*args)
    def SetIntegerMap(*args): return _cspace.iSimpleFormerState_SetIntegerMap(*args)
    def SetFloatMap(*args): return _cspace.iSimpleFormerState_SetFloatMap(*args)
    def GetFloatMap(*args): return _cspace.iSimpleFormerState_GetFloatMap(*args)
    def SetMaterialScale(*args): return _cspace.iSimpleFormerState_SetMaterialScale(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iSimpleFormerState_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iSimpleFormerState_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iSimpleFormerState
    __del__ = lambda self : None;
iSimpleFormerState_swigregister = _cspace.iSimpleFormerState_swigregister
iSimpleFormerState_swigregister(iSimpleFormerState)
iSimpleFormerState_scfGetVersion = _cspace.iSimpleFormerState_scfGetVersion

class iTerraFormer(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iTerraFormer, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iTerraFormer, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetSampler(*args): return _cspace.iTerraFormer_GetSampler(*args)
    def GetIntegerMapSize(*args): return _cspace.iTerraFormer_GetIntegerMapSize(*args)
    def SampleFloat(*args): return _cspace.iTerraFormer_SampleFloat(*args)
    def SampleVector2(*args): return _cspace.iTerraFormer_SampleVector2(*args)
    def SampleVector3(*args): return _cspace.iTerraFormer_SampleVector3(*args)
    def SampleInteger(*args): return _cspace.iTerraFormer_SampleInteger(*args)
    def QueryObject(*args): return _cspace.iTerraFormer_QueryObject(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iTerraFormer_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iTerraFormer_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iTerraFormer
    __del__ = lambda self : None;
iTerraFormer_swigregister = _cspace.iTerraFormer_swigregister
iTerraFormer_swigregister(iTerraFormer)
iTerraFormer_scfGetVersion = _cspace.iTerraFormer_scfGetVersion

class iTerraSampler(iBase):
    __swig_setmethods__ = {}
    for _s in [iBase]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, iTerraSampler, name, value)
    __swig_getmethods__ = {}
    for _s in [iBase]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, iTerraSampler, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SampleFloat(*args): return _cspace.iTerraSampler_SampleFloat(*args)
    def SampleVector2(*args): return _cspace.iTerraSampler_SampleVector2(*args)
    def SampleVector3(*args): return _cspace.iTerraSampler_SampleVector3(*args)
    def SampleInteger(*args): return _cspace.iTerraSampler_SampleInteger(*args)
    def GetMaterialPalette(*args): return _cspace.iTerraSampler_GetMaterialPalette(*args)
    def GetRegion(*args): return _cspace.iTerraSampler_GetRegion(*args)
    def GetResolution(*args): return _cspace.iTerraSampler_GetResolution(*args)
    def GetVersion(*args): return _cspace.iTerraSampler_GetVersion(*args)
    def Cleanup(*args): return _cspace.iTerraSampler_Cleanup(*args)
    __swig_getmethods__["scfGetVersion"] = lambda x: _cspace.iTerraSampler_scfGetVersion
    if _newclass:scfGetVersion = staticmethod(_cspace.iTerraSampler_scfGetVersion)
    __swig_destroy__ = _cspace.delete_iTerraSampler
    __del__ = lambda self : None;
iTerraSampler_swigregister = _cspace.iTerraSampler_swigregister
iTerraSampler_swigregister(iTerraSampler)
iTerraSampler_scfGetVersion = _cspace.iTerraSampler_scfGetVersion

class pycsObject(iObject):
    __swig_setmethods__ = {}
    for _s in [iObject]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, pycsObject, name, value)
    __swig_getmethods__ = {}
    for _s in [iObject]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, pycsObject, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def IncRef(*args): return _cspace.pycsObject_IncRef(*args)
    def DecRef(*args): return _cspace.pycsObject_DecRef(*args)
    def GetRefCount(*args): return _cspace.pycsObject_GetRefCount(*args)
    def QueryInterface(*args): return _cspace.pycsObject_QueryInterface(*args)
    def AddRefOwner(*args): return _cspace.pycsObject_AddRefOwner(*args)
    def RemoveRefOwner(*args): return _cspace.pycsObject_RemoveRefOwner(*args)
    def GetInterfaceMetadata(*args): return _cspace.pycsObject_GetInterfaceMetadata(*args)
pycsObject_swigregister = _cspace.pycsObject_swigregister
pycsObject_swigregister(pycsObject)

class csObject(pycsObject):
    __swig_setmethods__ = {}
    for _s in [pycsObject]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csObject, name, value)
    __swig_getmethods__ = {}
    for _s in [pycsObject]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csObject, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csObject(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csObject
    __del__ = lambda self : None;
    def SetName(*args): return _cspace.csObject_SetName(*args)
    def GetName(*args): return _cspace.csObject_GetName(*args)
    def GetID(*args): return _cspace.csObject_GetID(*args)
    def SetObjectParent(*args): return _cspace.csObject_SetObjectParent(*args)
    def GetObjectParent(*args): return _cspace.csObject_GetObjectParent(*args)
    def ObjAdd(*args): return _cspace.csObject_ObjAdd(*args)
    def ObjRemove(*args): return _cspace.csObject_ObjRemove(*args)
    def ObjRemoveAll(*args): return _cspace.csObject_ObjRemoveAll(*args)
    def ObjAddChildren(*args): return _cspace.csObject_ObjAddChildren(*args)
    def GetChild(*args): return _cspace.csObject_GetChild(*args)
    def GetIterator(*args): return _cspace.csObject_GetIterator(*args)
    def AddNameChangeListener(*args): return _cspace.csObject_AddNameChangeListener(*args)
    def RemoveNameChangeListener(*args): return _cspace.csObject_RemoveNameChangeListener(*args)
    def ObjReleaseOld(*args): return _cspace.csObject_ObjReleaseOld(*args)
csObject_swigregister = _cspace.csObject_swigregister
csObject_swigregister(csObject)

class pycsColliderWrapper(csObject):
    __swig_setmethods__ = {}
    for _s in [csObject]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, pycsColliderWrapper, name, value)
    __swig_getmethods__ = {}
    for _s in [csObject]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, pycsColliderWrapper, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def IncRef(*args): return _cspace.pycsColliderWrapper_IncRef(*args)
    def DecRef(*args): return _cspace.pycsColliderWrapper_DecRef(*args)
    def GetRefCount(*args): return _cspace.pycsColliderWrapper_GetRefCount(*args)
    def QueryInterface(*args): return _cspace.pycsColliderWrapper_QueryInterface(*args)
    def AddRefOwner(*args): return _cspace.pycsColliderWrapper_AddRefOwner(*args)
    def RemoveRefOwner(*args): return _cspace.pycsColliderWrapper_RemoveRefOwner(*args)
    def GetInterfaceMetadata(*args): return _cspace.pycsColliderWrapper_GetInterfaceMetadata(*args)
pycsColliderWrapper_swigregister = _cspace.pycsColliderWrapper_swigregister
pycsColliderWrapper_swigregister(pycsColliderWrapper)

class csColliderWrapper(pycsColliderWrapper):
    __swig_setmethods__ = {}
    for _s in [pycsColliderWrapper]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csColliderWrapper, name, value)
    __swig_getmethods__ = {}
    for _s in [pycsColliderWrapper]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csColliderWrapper, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csColliderWrapper(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csColliderWrapper
    __del__ = lambda self : None;
    def GetCollider(*args): return _cspace.csColliderWrapper_GetCollider(*args)
    def GetCollideSystem(*args): return _cspace.csColliderWrapper_GetCollideSystem(*args)
    def Collide(*args): return _cspace.csColliderWrapper_Collide(*args)
    __swig_getmethods__["GetColliderWrapper"] = lambda x: _cspace.csColliderWrapper_GetColliderWrapper
    if _newclass:GetColliderWrapper = staticmethod(_cspace.csColliderWrapper_GetColliderWrapper)
    def UpdateCollider(*args): return _cspace.csColliderWrapper_UpdateCollider(*args)
csColliderWrapper_swigregister = _cspace.csColliderWrapper_swigregister
csColliderWrapper_swigregister(csColliderWrapper)
csColliderWrapper_GetColliderWrapper = _cspace.csColliderWrapper_GetColliderWrapper

class csTraceBeamResult(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csTraceBeamResult, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csTraceBeamResult, name)
    __repr__ = _swig_repr
    __swig_setmethods__["closest_tri"] = _cspace.csTraceBeamResult_closest_tri_set
    __swig_getmethods__["closest_tri"] = _cspace.csTraceBeamResult_closest_tri_get
    if _newclass:closest_tri = property(_cspace.csTraceBeamResult_closest_tri_get, _cspace.csTraceBeamResult_closest_tri_set)
    __swig_setmethods__["closest_isect"] = _cspace.csTraceBeamResult_closest_isect_set
    __swig_getmethods__["closest_isect"] = _cspace.csTraceBeamResult_closest_isect_get
    if _newclass:closest_isect = property(_cspace.csTraceBeamResult_closest_isect_get, _cspace.csTraceBeamResult_closest_isect_set)
    __swig_setmethods__["closest_mesh"] = _cspace.csTraceBeamResult_closest_mesh_set
    __swig_getmethods__["closest_mesh"] = _cspace.csTraceBeamResult_closest_mesh_get
    if _newclass:closest_mesh = property(_cspace.csTraceBeamResult_closest_mesh_get, _cspace.csTraceBeamResult_closest_mesh_set)
    __swig_setmethods__["sqdistance"] = _cspace.csTraceBeamResult_sqdistance_set
    __swig_getmethods__["sqdistance"] = _cspace.csTraceBeamResult_sqdistance_get
    if _newclass:sqdistance = property(_cspace.csTraceBeamResult_sqdistance_get, _cspace.csTraceBeamResult_sqdistance_set)
    __swig_setmethods__["end_sector"] = _cspace.csTraceBeamResult_end_sector_set
    __swig_getmethods__["end_sector"] = _cspace.csTraceBeamResult_end_sector_get
    if _newclass:end_sector = property(_cspace.csTraceBeamResult_end_sector_get, _cspace.csTraceBeamResult_end_sector_set)
    def __init__(self, *args): 
        this = _cspace.new_csTraceBeamResult(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csTraceBeamResult
    __del__ = lambda self : None;
csTraceBeamResult_swigregister = _cspace.csTraceBeamResult_swigregister
csTraceBeamResult_swigregister(csTraceBeamResult)

class csColliderHelper(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csColliderHelper, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csColliderHelper, name)
    __repr__ = _swig_repr
    __swig_getmethods__["InitializeCollisionWrapper"] = lambda x: _cspace.csColliderHelper_InitializeCollisionWrapper
    if _newclass:InitializeCollisionWrapper = staticmethod(_cspace.csColliderHelper_InitializeCollisionWrapper)
    __swig_getmethods__["InitializeCollisionWrappers"] = lambda x: _cspace.csColliderHelper_InitializeCollisionWrappers
    if _newclass:InitializeCollisionWrappers = staticmethod(_cspace.csColliderHelper_InitializeCollisionWrappers)
    __swig_getmethods__["CollideArray"] = lambda x: _cspace.csColliderHelper_CollideArray
    if _newclass:CollideArray = staticmethod(_cspace.csColliderHelper_CollideArray)
    __swig_getmethods__["CollidePath"] = lambda x: _cspace.csColliderHelper_CollidePath
    if _newclass:CollidePath = staticmethod(_cspace.csColliderHelper_CollidePath)
    __swig_getmethods__["TraceBeam"] = lambda x: _cspace.csColliderHelper_TraceBeam
    if _newclass:TraceBeam = staticmethod(_cspace.csColliderHelper_TraceBeam)
    def __init__(self, *args): 
        this = _cspace.new_csColliderHelper(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csColliderHelper
    __del__ = lambda self : None;
csColliderHelper_swigregister = _cspace.csColliderHelper_swigregister
csColliderHelper_swigregister(csColliderHelper)
csColliderHelper_InitializeCollisionWrapper = _cspace.csColliderHelper_InitializeCollisionWrapper
csColliderHelper_InitializeCollisionWrappers = _cspace.csColliderHelper_InitializeCollisionWrappers
csColliderHelper_CollideArray = _cspace.csColliderHelper_CollideArray
csColliderHelper_CollidePath = _cspace.csColliderHelper_CollidePath
csColliderHelper_TraceBeam = _cspace.csColliderHelper_TraceBeam

class csColliderActor(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csColliderActor, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csColliderActor, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csColliderActor(*args)
        try: self.this.append(this)
        except: self.this = this
    def SetCollideSystem(*args): return _cspace.csColliderActor_SetCollideSystem(*args)
    def SetEngine(*args): return _cspace.csColliderActor_SetEngine(*args)
    def InitializeColliders(*args): return _cspace.csColliderActor_InitializeColliders(*args)
    def SetCamera(*args): return _cspace.csColliderActor_SetCamera(*args)
    def SetGravity(*args): return _cspace.csColliderActor_SetGravity(*args)
    def GetGravity(*args): return _cspace.csColliderActor_GetGravity(*args)
    def IsOnGround(*args): return _cspace.csColliderActor_IsOnGround(*args)
    def SetOnGround(*args): return _cspace.csColliderActor_SetOnGround(*args)
    def HasCD(*args): return _cspace.csColliderActor_HasCD(*args)
    def SetCD(*args): return _cspace.csColliderActor_SetCD(*args)
    def CheckRevertMove(*args): return _cspace.csColliderActor_CheckRevertMove(*args)
    def EnableHitMeshes(*args): return _cspace.csColliderActor_EnableHitMeshes(*args)
    def CheckHitMeshes(*args): return _cspace.csColliderActor_CheckHitMeshes(*args)
    def GetHitMeshes(*args): return _cspace.csColliderActor_GetHitMeshes(*args)
    def Move(*args): return _cspace.csColliderActor_Move(*args)
    def GetRotation(*args): return _cspace.csColliderActor_GetRotation(*args)
    def SetRotation(*args): return _cspace.csColliderActor_SetRotation(*args)
    def AdjustForCollisions(*args): return _cspace.csColliderActor_AdjustForCollisions(*args)
    __swig_destroy__ = _cspace.delete_csColliderActor
    __del__ = lambda self : None;
csColliderActor_swigregister = _cspace.csColliderActor_swigregister
csColliderActor_swigregister(csColliderActor)

class csView(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csView, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csView, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csView(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csView
    __del__ = lambda self : None;
    def GetEngine(*args): return _cspace.csView_GetEngine(*args)
    def SetEngine(*args): return _cspace.csView_SetEngine(*args)
    def GetCamera(*args): return _cspace.csView_GetCamera(*args)
    def SetCamera(*args): return _cspace.csView_SetCamera(*args)
    def GetContext(*args): return _cspace.csView_GetContext(*args)
    def SetContext(*args): return _cspace.csView_SetContext(*args)
    def SetRectangle(*args): return _cspace.csView_SetRectangle(*args)
    def ClearView(*args): return _cspace.csView_ClearView(*args)
    def AddViewVertex(*args): return _cspace.csView_AddViewVertex(*args)
    def RestrictClipperToScreen(*args): return _cspace.csView_RestrictClipperToScreen(*args)
    def SetAutoResize(*args): return _cspace.csView_SetAutoResize(*args)
    def UpdateClipper(*args): return _cspace.csView_UpdateClipper(*args)
    def GetClipper(*args): return _cspace.csView_GetClipper(*args)
    def Draw(*args): return _cspace.csView_Draw(*args)
csView_swigregister = _cspace.csView_swigregister
csView_swigregister(csView)

csfxInterference = _cspace.csfxInterference
csfxFadeOut = _cspace.csfxFadeOut
csfxFadeTo = _cspace.csfxFadeTo
csfxFadeToColor = _cspace.csfxFadeToColor
csfxGreenScreen = _cspace.csfxGreenScreen
csfxRedScreen = _cspace.csfxRedScreen
csfxBlueScreen = _cspace.csfxBlueScreen
csfxWhiteOut = _cspace.csfxWhiteOut
csfxScreenDPFX = _cspace.csfxScreenDPFX
csfxScreenDPFXPartial = _cspace.csfxScreenDPFXPartial
class csPixmap(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPixmap, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csPixmap, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_csPixmap
    __del__ = lambda self : None;
    def Width(*args): return _cspace.csPixmap_Width(*args)
    def Height(*args): return _cspace.csPixmap_Height(*args)
    def Advance(*args): return _cspace.csPixmap_Advance(*args)
    def GetTextureHandle(*args): return _cspace.csPixmap_GetTextureHandle(*args)
    def DrawScaled(*args): return _cspace.csPixmap_DrawScaled(*args)
    def DrawScaledAlign(*args): return _cspace.csPixmap_DrawScaledAlign(*args)
    def Draw(*args): return _cspace.csPixmap_Draw(*args)
    def DrawAlign(*args): return _cspace.csPixmap_DrawAlign(*args)
    def DrawTiled(*args): return _cspace.csPixmap_DrawTiled(*args)
csPixmap_swigregister = _cspace.csPixmap_swigregister
csPixmap_swigregister(csPixmap)
csfxShadeVert = _cspace.csfxShadeVert

class csSimplePixmap(csPixmap):
    __swig_setmethods__ = {}
    for _s in [csPixmap]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csSimplePixmap, name, value)
    __swig_getmethods__ = {}
    for _s in [csPixmap]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csSimplePixmap, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csSimplePixmap(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csSimplePixmap
    __del__ = lambda self : None;
    def SetTextureHandle(*args): return _cspace.csSimplePixmap_SetTextureHandle(*args)
    def SetTextureRectangle(*args): return _cspace.csSimplePixmap_SetTextureRectangle(*args)
    def DrawScaled(*args): return _cspace.csSimplePixmap_DrawScaled(*args)
    def DrawTiled(*args): return _cspace.csSimplePixmap_DrawTiled(*args)
    def Width(*args): return _cspace.csSimplePixmap_Width(*args)
    def Height(*args): return _cspace.csSimplePixmap_Height(*args)
    def Advance(*args): return _cspace.csSimplePixmap_Advance(*args)
    def GetTextureHandle(*args): return _cspace.csSimplePixmap_GetTextureHandle(*args)
csSimplePixmap_swigregister = _cspace.csSimplePixmap_swigregister
csSimplePixmap_swigregister(csSimplePixmap)

class csShortestDistanceResult(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csShortestDistanceResult, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csShortestDistanceResult, name)
    __repr__ = _swig_repr
    __swig_setmethods__["sqdistance"] = _cspace.csShortestDistanceResult_sqdistance_set
    __swig_getmethods__["sqdistance"] = _cspace.csShortestDistanceResult_sqdistance_get
    if _newclass:sqdistance = property(_cspace.csShortestDistanceResult_sqdistance_get, _cspace.csShortestDistanceResult_sqdistance_set)
    __swig_setmethods__["direction"] = _cspace.csShortestDistanceResult_direction_set
    __swig_getmethods__["direction"] = _cspace.csShortestDistanceResult_direction_get
    if _newclass:direction = property(_cspace.csShortestDistanceResult_direction_get, _cspace.csShortestDistanceResult_direction_set)
    def __init__(self, *args): 
        this = _cspace.new_csShortestDistanceResult(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csShortestDistanceResult
    __del__ = lambda self : None;
csShortestDistanceResult_swigregister = _cspace.csShortestDistanceResult_swigregister
csShortestDistanceResult_swigregister(csShortestDistanceResult)

class csScreenTargetResult(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csScreenTargetResult, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csScreenTargetResult, name)
    __repr__ = _swig_repr
    __swig_setmethods__["mesh"] = _cspace.csScreenTargetResult_mesh_set
    __swig_getmethods__["mesh"] = _cspace.csScreenTargetResult_mesh_get
    if _newclass:mesh = property(_cspace.csScreenTargetResult_mesh_get, _cspace.csScreenTargetResult_mesh_set)
    __swig_setmethods__["isect"] = _cspace.csScreenTargetResult_isect_set
    __swig_getmethods__["isect"] = _cspace.csScreenTargetResult_isect_get
    if _newclass:isect = property(_cspace.csScreenTargetResult_isect_get, _cspace.csScreenTargetResult_isect_set)
    __swig_setmethods__["polygon_idx"] = _cspace.csScreenTargetResult_polygon_idx_set
    __swig_getmethods__["polygon_idx"] = _cspace.csScreenTargetResult_polygon_idx_get
    if _newclass:polygon_idx = property(_cspace.csScreenTargetResult_polygon_idx_get, _cspace.csScreenTargetResult_polygon_idx_set)
    def __init__(self, *args): 
        this = _cspace.new_csScreenTargetResult(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csScreenTargetResult
    __del__ = lambda self : None;
csScreenTargetResult_swigregister = _cspace.csScreenTargetResult_swigregister
csScreenTargetResult_swigregister(csScreenTargetResult)

class csEngineTools(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, csEngineTools, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, csEngineTools, name)
    __repr__ = _swig_repr
    __swig_getmethods__["FindShortestDistance"] = lambda x: _cspace.csEngineTools_FindShortestDistance
    if _newclass:FindShortestDistance = staticmethod(_cspace.csEngineTools_FindShortestDistance)
    __swig_getmethods__["FindScreenTarget"] = lambda x: _cspace.csEngineTools_FindScreenTarget
    if _newclass:FindScreenTarget = staticmethod(_cspace.csEngineTools_FindScreenTarget)
    def __init__(self, *args): 
        this = _cspace.new_csEngineTools(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csEngineTools
    __del__ = lambda self : None;
csEngineTools_swigregister = _cspace.csEngineTools_swigregister
csEngineTools_swigregister(csEngineTools)
csEngineTools_FindShortestDistance = _cspace.csEngineTools_FindShortestDistance
csEngineTools_FindScreenTarget = _cspace.csEngineTools_FindScreenTarget

CS_PEN_TA_TOP = _cspace.CS_PEN_TA_TOP
CS_PEN_TA_BOT = _cspace.CS_PEN_TA_BOT
CS_PEN_TA_LEFT = _cspace.CS_PEN_TA_LEFT
CS_PEN_TA_RIGHT = _cspace.CS_PEN_TA_RIGHT
CS_PEN_TA_CENTER = _cspace.CS_PEN_TA_CENTER
CS_PEN_FILL = _cspace.CS_PEN_FILL
CS_PEN_SWAPCOLORS = _cspace.CS_PEN_SWAPCOLORS
CS_PEN_TEXTURE_ONLY = _cspace.CS_PEN_TEXTURE_ONLY
CS_PEN_TEXTURE = _cspace.CS_PEN_TEXTURE
class iPen(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, iPen, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, iPen, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def SetFlag(*args): return _cspace.iPen_SetFlag(*args)
    def ClearFlag(*args): return _cspace.iPen_ClearFlag(*args)
    def SetMixMode(*args): return _cspace.iPen_SetMixMode(*args)
    def SetColor(*args): return _cspace.iPen_SetColor(*args)
    def SetTexture(*args): return _cspace.iPen_SetTexture(*args)
    def SwapColors(*args): return _cspace.iPen_SwapColors(*args)
    def SetPenWidth(*args): return _cspace.iPen_SetPenWidth(*args)
    def ClearTransform(*args): return _cspace.iPen_ClearTransform(*args)
    def PushTransform(*args): return _cspace.iPen_PushTransform(*args)
    def PopTransform(*args): return _cspace.iPen_PopTransform(*args)
    def SetOrigin(*args): return _cspace.iPen_SetOrigin(*args)
    def Translate(*args): return _cspace.iPen_Translate(*args)
    def DrawLine(*args): return _cspace.iPen_DrawLine(*args)
    def DrawPoint(*args): return _cspace.iPen_DrawPoint(*args)
    def DrawRect(*args): return _cspace.iPen_DrawRect(*args)
    def DrawMiteredRect(*args): return _cspace.iPen_DrawMiteredRect(*args)
    def DrawRoundedRect(*args): return _cspace.iPen_DrawRoundedRect(*args)
    def DrawArc(*args): return _cspace.iPen_DrawArc(*args)
    def DrawTriangle(*args): return _cspace.iPen_DrawTriangle(*args)
    def Write(*args): return _cspace.iPen_Write(*args)
    def WriteBoxed(*args): return _cspace.iPen_WriteBoxed(*args)
    def _Rotate(*args): return _cspace.iPen__Rotate(*args)
    def Rotate(self,a):
         return _cspace.iPen__Rotate(a)

iPen_swigregister = _cspace.iPen_swigregister
iPen_swigregister(iPen)

class csPen(iPen):
    __swig_setmethods__ = {}
    for _s in [iPen]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPen, name, value)
    __swig_getmethods__ = {}
    for _s in [iPen]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csPen, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csPen(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csPen
    __del__ = lambda self : None;
    def SetFlag(*args): return _cspace.csPen_SetFlag(*args)
    def ClearFlag(*args): return _cspace.csPen_ClearFlag(*args)
    def SetMixMode(*args): return _cspace.csPen_SetMixMode(*args)
    def SetColor(*args): return _cspace.csPen_SetColor(*args)
    def SetTexture(*args): return _cspace.csPen_SetTexture(*args)
    def SwapColors(*args): return _cspace.csPen_SwapColors(*args)
    def SetPenWidth(*args): return _cspace.csPen_SetPenWidth(*args)
    def ClearTransform(*args): return _cspace.csPen_ClearTransform(*args)
    def PushTransform(*args): return _cspace.csPen_PushTransform(*args)
    def PopTransform(*args): return _cspace.csPen_PopTransform(*args)
    def SetOrigin(*args): return _cspace.csPen_SetOrigin(*args)
    def Translate(*args): return _cspace.csPen_Translate(*args)
    def DrawLine(*args): return _cspace.csPen_DrawLine(*args)
    def DrawThickLine(*args): return _cspace.csPen_DrawThickLine(*args)
    def DrawPoint(*args): return _cspace.csPen_DrawPoint(*args)
    def DrawRect(*args): return _cspace.csPen_DrawRect(*args)
    def DrawMiteredRect(*args): return _cspace.csPen_DrawMiteredRect(*args)
    def DrawRoundedRect(*args): return _cspace.csPen_DrawRoundedRect(*args)
    def DrawArc(*args): return _cspace.csPen_DrawArc(*args)
    def DrawTriangle(*args): return _cspace.csPen_DrawTriangle(*args)
    def Write(*args): return _cspace.csPen_Write(*args)
    def WriteBoxed(*args): return _cspace.csPen_WriteBoxed(*args)
csPen_swigregister = _cspace.csPen_swigregister
csPen_swigregister(csPen)

class csMemoryPen(iPen):
    __swig_setmethods__ = {}
    for _s in [iPen]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csMemoryPen, name, value)
    __swig_getmethods__ = {}
    for _s in [iPen]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csMemoryPen, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csMemoryPen(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csMemoryPen
    __del__ = lambda self : None;
    def Clear(*args): return _cspace.csMemoryPen_Clear(*args)
    def Draw(*args): return _cspace.csMemoryPen_Draw(*args)
    def SetFlag(*args): return _cspace.csMemoryPen_SetFlag(*args)
    def ClearFlag(*args): return _cspace.csMemoryPen_ClearFlag(*args)
    def SetMixMode(*args): return _cspace.csMemoryPen_SetMixMode(*args)
    def SetColor(*args): return _cspace.csMemoryPen_SetColor(*args)
    def SetTexture(*args): return _cspace.csMemoryPen_SetTexture(*args)
    def SwapColors(*args): return _cspace.csMemoryPen_SwapColors(*args)
    def SetPenWidth(*args): return _cspace.csMemoryPen_SetPenWidth(*args)
    def ClearTransform(*args): return _cspace.csMemoryPen_ClearTransform(*args)
    def PushTransform(*args): return _cspace.csMemoryPen_PushTransform(*args)
    def PopTransform(*args): return _cspace.csMemoryPen_PopTransform(*args)
    def SetOrigin(*args): return _cspace.csMemoryPen_SetOrigin(*args)
    def Translate(*args): return _cspace.csMemoryPen_Translate(*args)
    def DrawLine(*args): return _cspace.csMemoryPen_DrawLine(*args)
    def DrawPoint(*args): return _cspace.csMemoryPen_DrawPoint(*args)
    def DrawRect(*args): return _cspace.csMemoryPen_DrawRect(*args)
    def DrawMiteredRect(*args): return _cspace.csMemoryPen_DrawMiteredRect(*args)
    def DrawRoundedRect(*args): return _cspace.csMemoryPen_DrawRoundedRect(*args)
    def DrawArc(*args): return _cspace.csMemoryPen_DrawArc(*args)
    def DrawTriangle(*args): return _cspace.csMemoryPen_DrawTriangle(*args)
    def Write(*args): return _cspace.csMemoryPen_Write(*args)
    def WriteBoxed(*args): return _cspace.csMemoryPen_WriteBoxed(*args)
csMemoryPen_swigregister = _cspace.csMemoryPen_swigregister
csMemoryPen_swigregister(csMemoryPen)

CSKEY_SHIFT_NUM = _cspace.CSKEY_SHIFT_NUM
CSKEY_CTRL_NUM = _cspace.CSKEY_CTRL_NUM
CSKEY_ALT_NUM = _cspace.CSKEY_ALT_NUM
CSKEY_SPECIAL = _cspace.CSKEY_SPECIAL
CSKEY_SPECIAL_NUM = _cspace.CSKEY_SPECIAL_NUM
CSKEY_MODIFIER = _cspace.CSKEY_MODIFIER
CS_IS_KEYBOARD_EVENT = _cspace.CS_IS_KEYBOARD_EVENT
CS_IS_MOUSE_EVENT = _cspace.CS_IS_MOUSE_EVENT
CS_IS_JOYSTICK_EVENT = _cspace.CS_IS_JOYSTICK_EVENT
CS_IS_INPUT_EVENT = _cspace.CS_IS_INPUT_EVENT
csevAllEvents = _cspace.csevAllEvents
csevFrame = _cspace.csevFrame
csevInput = _cspace.csevInput
csevQuit = _cspace.csevQuit
csevKeyboardEvent = _cspace.csevKeyboardEvent
csevKeyboardDown = _cspace.csevKeyboardDown
csevKeyboardUp = _cspace.csevKeyboardUp
csevMouseEvent = _cspace.csevMouseEvent
csevMouseButton = _cspace.csevMouseButton
csevMouseUp = _cspace.csevMouseUp
csevMouseDown = _cspace.csevMouseDown
csevMouseClick = _cspace.csevMouseClick
csevMouseDoubleClick = _cspace.csevMouseDoubleClick
csevMouseMove = _cspace.csevMouseMove
csevJoystickEvent = _cspace.csevJoystickEvent
CS_LOAD_PLUGIN_ALWAYS = _cspace.CS_LOAD_PLUGIN_ALWAYS
CS_FX_SETALPHA = _cspace.CS_FX_SETALPHA
CS_FX_SETALPHA_INT = _cspace.CS_FX_SETALPHA_INT
class TextureMapper(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TextureMapper, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TextureMapper, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _cspace.delete_TextureMapper
    __del__ = lambda self : None;
    def Map(*args): return _cspace.TextureMapper_Map(*args)
TextureMapper_swigregister = _cspace.TextureMapper_swigregister
TextureMapper_swigregister(TextureMapper)

class TableTextureMapper(TextureMapper):
    __swig_setmethods__ = {}
    for _s in [TextureMapper]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, TableTextureMapper, name, value)
    __swig_getmethods__ = {}
    for _s in [TextureMapper]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, TableTextureMapper, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_TableTextureMapper(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_TableTextureMapper
    __del__ = lambda self : None;
    def Map(*args): return _cspace.TableTextureMapper_Map(*args)
TableTextureMapper_swigregister = _cspace.TableTextureMapper_swigregister
TableTextureMapper_swigregister(TableTextureMapper)

class DensityTextureMapper(TextureMapper):
    __swig_setmethods__ = {}
    for _s in [TextureMapper]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, DensityTextureMapper, name, value)
    __swig_getmethods__ = {}
    for _s in [TextureMapper]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, DensityTextureMapper, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_DensityTextureMapper(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_DensityTextureMapper
    __del__ = lambda self : None;
    def Map(*args): return _cspace.DensityTextureMapper_Map(*args)
DensityTextureMapper_swigregister = _cspace.DensityTextureMapper_swigregister
DensityTextureMapper_swigregister(DensityTextureMapper)

class Primitives(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Primitives, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Primitives, name)
    __repr__ = _swig_repr
    __swig_setmethods__["boxTable"] = _cspace.Primitives_boxTable_set
    __swig_getmethods__["boxTable"] = _cspace.Primitives_boxTable_get
    if _newclass:boxTable = property(_cspace.Primitives_boxTable_get, _cspace.Primitives_boxTable_set)
    __swig_setmethods__["quadTable"] = _cspace.Primitives_quadTable_set
    __swig_getmethods__["quadTable"] = _cspace.Primitives_quadTable_get
    if _newclass:quadTable = property(_cspace.Primitives_quadTable_get, _cspace.Primitives_quadTable_set)
    CS_PRIMBOX_INSIDE = _cspace.Primitives_CS_PRIMBOX_INSIDE
    CS_PRIMBOX_SMOOTH = _cspace.Primitives_CS_PRIMBOX_SMOOTH
    __swig_getmethods__["GenerateBox"] = lambda x: _cspace.Primitives_GenerateBox
    if _newclass:GenerateBox = staticmethod(_cspace.Primitives_GenerateBox)
    __swig_getmethods__["GenerateQuad"] = lambda x: _cspace.Primitives_GenerateQuad
    if _newclass:GenerateQuad = staticmethod(_cspace.Primitives_GenerateQuad)
    __swig_getmethods__["GenerateTesselatedQuad"] = lambda x: _cspace.Primitives_GenerateTesselatedQuad
    if _newclass:GenerateTesselatedQuad = staticmethod(_cspace.Primitives_GenerateTesselatedQuad)
    __swig_getmethods__["GenerateCapsule"] = lambda x: _cspace.Primitives_GenerateCapsule
    if _newclass:GenerateCapsule = staticmethod(_cspace.Primitives_GenerateCapsule)
    __swig_getmethods__["GenerateSphere"] = lambda x: _cspace.Primitives_GenerateSphere
    if _newclass:GenerateSphere = staticmethod(_cspace.Primitives_GenerateSphere)
    def __init__(self, *args): 
        this = _cspace.new_Primitives(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_Primitives
    __del__ = lambda self : None;
Primitives_swigregister = _cspace.Primitives_swigregister
Primitives_swigregister(Primitives)
Primitives_GenerateBox = _cspace.Primitives_GenerateBox
Primitives_GenerateQuad = _cspace.Primitives_GenerateQuad
Primitives_GenerateTesselatedQuad = _cspace.Primitives_GenerateTesselatedQuad
Primitives_GenerateCapsule = _cspace.Primitives_GenerateCapsule
Primitives_GenerateSphere = _cspace.Primitives_GenerateSphere

class csPrimitives(Primitives):
    __swig_setmethods__ = {}
    for _s in [Primitives]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPrimitives, name, value)
    __swig_getmethods__ = {}
    for _s in [Primitives]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csPrimitives, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new_csPrimitives(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete_csPrimitives
    __del__ = lambda self : None;
csPrimitives_swigregister = _cspace.csPrimitives_swigregister
csPrimitives_swigregister(csPrimitives)

CS_QUERY_REGISTRY = _cspace.CS_QUERY_REGISTRY
CS_QUERY_REGISTRY_TAG_INTERFACE = _cspace.CS_QUERY_REGISTRY_TAG_INTERFACE
SCF_QUERY_INTERFACE = _cspace.SCF_QUERY_INTERFACE
SCF_QUERY_INTERFACE_SAFE = _cspace.SCF_QUERY_INTERFACE_SAFE
CS_QUERY_PLUGIN_CLASS = _cspace.CS_QUERY_PLUGIN_CLASS
CS_LOAD_PLUGIN = _cspace.CS_LOAD_PLUGIN
CS_GET_CHILD_OBJECT = _cspace.CS_GET_CHILD_OBJECT
CS_GET_NAMED_CHILD_OBJECT = _cspace.CS_GET_NAMED_CHILD_OBJECT
CS_GET_FIRST_NAMED_CHILD_OBJECT = _cspace.CS_GET_FIRST_NAMED_CHILD_OBJECT
class csPyEventHandlerParent(iEventHandler):
    __swig_setmethods__ = {}
    for _s in [iEventHandler]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, csPyEventHandlerParent, name, value)
    __swig_getmethods__ = {}
    for _s in [iEventHandler]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, csPyEventHandlerParent, name)
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def IncRef(*args): return _cspace.csPyEventHandlerParent_IncRef(*args)
    def DecRef(*args): return _cspace.csPyEventHandlerParent_DecRef(*args)
    def GetRefCount(*args): return _cspace.csPyEventHandlerParent_GetRefCount(*args)
    def QueryInterface(*args): return _cspace.csPyEventHandlerParent_QueryInterface(*args)
    def AddRefOwner(*args): return _cspace.csPyEventHandlerParent_AddRefOwner(*args)
    def RemoveRefOwner(*args): return _cspace.csPyEventHandlerParent_RemoveRefOwner(*args)
    def GetInterfaceMetadata(*args): return _cspace.csPyEventHandlerParent_GetInterfaceMetadata(*args)
csPyEventHandlerParent_swigregister = _cspace.csPyEventHandlerParent_swigregister
csPyEventHandlerParent_swigregister(csPyEventHandlerParent)

class _csPyEventHandler(csPyEventHandlerParent):
    __swig_setmethods__ = {}
    for _s in [csPyEventHandlerParent]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, _csPyEventHandler, name, value)
    __swig_getmethods__ = {}
    for _s in [csPyEventHandlerParent]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, _csPyEventHandler, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _cspace.new__csPyEventHandler(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _cspace.delete__csPyEventHandler
    __del__ = lambda self : None;
    def HandleEvent(*args): return _cspace._csPyEventHandler_HandleEvent(*args)
    __swig_getmethods__["StaticHandlerName"] = lambda x: _cspace._csPyEventHandler_StaticHandlerName
    if _newclass:StaticHandlerName = staticmethod(_cspace._csPyEventHandler_StaticHandlerName)
    __swig_getmethods__["StaticID"] = lambda x: _cspace._csPyEventHandler_StaticID
    if _newclass:StaticID = staticmethod(_cspace._csPyEventHandler_StaticID)
    def GenericName(*args): return _cspace._csPyEventHandler_GenericName(*args)
    def GenericID(*args): return _cspace._csPyEventHandler_GenericID(*args)
    def GenericPrec(*args): return _cspace._csPyEventHandler_GenericPrec(*args)
    def GenericSucc(*args): return _cspace._csPyEventHandler_GenericSucc(*args)
    def InstancePrec(*args): return _cspace._csPyEventHandler_InstancePrec(*args)
    def InstanceSucc(*args): return _cspace._csPyEventHandler_InstanceSucc(*args)
_csPyEventHandler_swigregister = _cspace._csPyEventHandler_swigregister
_csPyEventHandler_swigregister(_csPyEventHandler)
_csPyEventHandler_StaticHandlerName = _cspace._csPyEventHandler_StaticHandlerName
_csPyEventHandler_StaticID = _cspace._csPyEventHandler_StaticID

class csPyEventHandler (_csPyEventHandler):
  """Python version of iEventHandler implementation.
     This class can be used as base class for event handlers in Python.
     Call csPyEventHandler.__init__(self) in __init__ of derived class.
  """
  def __init__ (self):
    _csPyEventHandler.__init__(self, self)

class _EventHandlerFuncWrapper (csPyEventHandler):
  def __init__ (self, func):
    csPyEventHandler.__init__(self)
    self._func = func
    # Make sure a reference keeps to this wrapper instance.
    self._func._cs_event_handler_wrapper = self
  def HandleEvent (self, event):
    return self._func(event)

def _csInitializer_SetupEventHandler (reg, obj,
    eventids=None):
  """Replacement of C++ versions."""
  if callable(obj):
    # obj is a function
    hdlr = _EventHandlerFuncWrapper(obj)
    hdlr.thisown = 1
  else:
    # assume it is a iEventHandler
    hdlr = obj
  if eventids==None:
    eventids=[csevFrame(reg), csevInput(reg), csevKeyboardEvent(reg), \
              csevMouseEvent(reg), csevQuit(reg), CS_EVENTLIST_END]
  return csInitializer._SetupEventHandler(reg, hdlr, eventids)

csInitializer.SetupEventHandler = \
  staticmethod(_csInitializer_SetupEventHandler)


def csevCommandLineHelp(reg):
  csEventNameRegistry.GetID(reg, "crystalspace.application.commandlinehelp")
  
CS_EVENTLIST_END = csInvalidStringID

def _csInitializer_RequestPlugins (reg, plugins):
  """Replacement of C++ version."""
  def _get_tuple (x):
    if callable(x):
      return tuple(x())
    else:
      return tuple(x)
  requests = csPluginRequestArray()
  for cls, intf, ident, ver in map(
      lambda x: _get_tuple(x), plugins):
    requests.Push(csPluginRequest(
      csString(cls), csString(intf), ident, ver))
  return csInitializer._RequestPlugins(reg, requests)

csInitializer.RequestPlugins = staticmethod(_csInitializer_RequestPlugins)


csReport = csReporterHelper.Report

def CS_REQUEST_PLUGIN (name, intf):
  return (name, intf.__name__, cvar.iSCF_SCF.GetInterfaceID(intf.__name__),
    intf.scfGetVersion())

def CS_REQUEST_VFS ():
  return CS_REQUEST_PLUGIN("crystalspace.kernel.vfs", iVFS)

def CS_REQUEST_FONTSERVER ():
  return CS_REQUEST_PLUGIN("crystalspace.font.server.default", iFontServer)

def CS_REQUEST_IMAGELOADER ():
  return CS_REQUEST_PLUGIN("crystalspace.graphic.image.io.multiplexer",
    iImageIO)

def CS_REQUEST_NULL3D ():
  return CS_REQUEST_PLUGIN("crystalspace.graphics3d.null", iGraphics3D)

def CS_REQUEST_SOFTWARE3D ():
  return CS_REQUEST_PLUGIN("crystalspace.graphics3d.software", iGraphics3D)

def CS_REQUEST_OPENGL3D ():
  return CS_REQUEST_PLUGIN("crystalspace.graphics3d.opengl", iGraphics3D)

def CS_REQUEST_ENGINE ():
  return CS_REQUEST_PLUGIN("crystalspace.engine.3d", iEngine)

def CS_REQUEST_LEVELLOADER ():
  return CS_REQUEST_PLUGIN("crystalspace.level.loader", iLoader)

def CS_REQUEST_LEVELSAVER ():
  return CS_REQUEST_PLUGIN("crystalspace.level.saver", iSaver)

def CS_REQUEST_REPORTER ():
  return CS_REQUEST_PLUGIN("crystalspace.utilities.reporter", iReporter)

def CS_REQUEST_REPORTERLISTENER ():
  return CS_REQUEST_PLUGIN("crystalspace.utilities.stdrep",
    iStandardReporterListener)

def CS_REQUEST_CONSOLEOUT ():
  return CS_REQUEST_PLUGIN("crystalspace.console.output.standard",
    iConsoleOutput)


class CSMutableArrayHelper:
  def __init__(self, getFunc, lenFunc):
    self.getFunc = getFunc
    self.lenFunc = lenFunc

  def __len__(self):
    return self.lenFunc()

  def __getitem__(self, key):
    if type(key) != type(0):
      raise TypeError()
    arrlen = self.lenFunc()
    if key < 0 or key >= arrlen:
      raise IndexError('Length is ' + str(arrlen) + ', you asked for ' +
        str(key))
    return self.getFunc(key)
  def content_iterator(self):
    for idx in xrange(len(self)):
      yield self.__getitem__(idx)
  def __iter__(self): return self.content_iterator() 
  # We do not implement __setitem__ because the only legal action is to
  #  overwrite the object at the given location.  (The contents of the
  #  array are mutable, but the array is a single allocation of a single
  #  type.)  Callers should be using the contained objects' own
  #  __setitem__ or mutation methods.

  # We do not implement __delitem__ because we cannot delete items.

CS_VEC_FORWARD = csVector3(0,0,1)
CS_VEC_BACKWARD = csVector3(0,0,-1)
CS_VEC_RIGHT = csVector3(1,0,0)
CS_VEC_LEFT = csVector3(-1,0,0)
CS_VEC_UP = csVector3(0,1,0)
CS_VEC_DOWN = csVector3(0,-1,0)
CS_VEC_ROT_RIGHT = csVector3(0,1,0)
CS_VEC_ROT_LEFT = csVector3(0,-1,0)
CS_VEC_TILT_RIGHT = -csVector3(0,0,1)
CS_VEC_TILT_LEFT = -csVector3(0,0,-1) 
CS_VEC_TILT_UP = -csVector3(1,0,0)
CS_VEC_TILT_DOWN = -csVector3(-1,0,0)


CS_POLYRANGE_LAST = csPolygonRange (-1, -1)



