# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.
package cspace;
require Exporter;
require DynaLoader;
@ISA = qw(Exporter DynaLoader);
package cspacec;
bootstrap cspace;
package cspace;
@EXPORT = qw( );

# ---------- BASE METHODS -------------

package cspace;

sub TIEHASH {
    my ($classname,$obj) = @_;
    return bless $obj, $classname;
}

sub CLEAR { }

sub FIRSTKEY { }

sub NEXTKEY { }

sub FETCH {
    my ($self,$field) = @_;
    my $member_func = "swig_${field}_get";
    $self->$member_func();
}

sub STORE {
    my ($self,$field,$newval) = @_;
    my $member_func = "swig_${field}_set";
    $self->$member_func($newval);
}

sub this {
    my $ptr = shift;
    return tied(%$ptr);
}


# ------- FUNCTION WRAPPERS --------

package cspace;

*scfCompatibleVersion = *cspacec::scfCompatibleVersion;
*scfRegisterStaticClass = *cspacec::scfRegisterStaticClass;
*scfRegisterStaticClasses = *cspacec::scfRegisterStaticClasses;
*scfRegisterStaticFactoryFunc = *cspacec::scfRegisterStaticFactoryFunc;
*__mult_ass__ = *cspacec::__mult_ass__;
*__divide_ass__ = *cspacec::__divide_ass__;
*__subtr__ = *cspacec::__subtr__;
*__div__ = *cspacec::__div__;
*__add__ = *cspacec::__add__;
*__mult__ = *cspacec::__mult__;
*__eq__ = *cspacec::__eq__;
*__ne__ = *cspacec::__ne__;
*__gt__ = *cspacec::__gt__;
*csDefaultRunLoop = *cspacec::csDefaultRunLoop;
*csPlatformStartup = *cspacec::csPlatformStartup;
*csPlatformShutdown = *cspacec::csPlatformShutdown;
*csPrintf = *cspacec::csPrintf;
*csFPrintf = *cspacec::csFPrintf;
*csPrintfErr = *cspacec::csPrintfErr;
*csGetTicks = *cspacec::csGetTicks;
*csGetMicroTicks = *cspacec::csGetMicroTicks;
*csSleep = *cspacec::csSleep;
*csGetUsername = *cspacec::csGetUsername;
*csGetPlatformConfigPath = *cspacec::csGetPlatformConfigPath;
*csQueryRegistryTag = *cspacec::csQueryRegistryTag;
*RegisterWeakListener = *cspacec::RegisterWeakListener;
*RemoveWeakListener = *cspacec::RemoveWeakListener;
*csHashCompute = *cspacec::csHashCompute;
*csevMouse = *cspacec::csevMouse;
*csevMouseOp = *cspacec::csevMouseOp;
*csevJoystick = *cspacec::csevJoystick;
*csevJoystickOp = *cspacec::csevJoystickOp;
*csevCanvasOp = *cspacec::csevCanvasOp;
*csevPreProcess = *cspacec::csevPreProcess;
*csevProcess = *cspacec::csevProcess;
*csevPostProcess = *cspacec::csevPostProcess;
*csevFinalProcess = *cspacec::csevFinalProcess;
*csLoadPluginAlways = *cspacec::csLoadPluginAlways;
*csGetShaderVariableFromStack = *cspacec::csGetShaderVariableFromStack;
*csfxInterference = *cspacec::csfxInterference;
*csfxFadeOut = *cspacec::csfxFadeOut;
*csfxFadeTo = *cspacec::csfxFadeTo;
*csfxFadeToColor = *cspacec::csfxFadeToColor;
*csfxGreenScreen = *cspacec::csfxGreenScreen;
*csfxRedScreen = *cspacec::csfxRedScreen;
*csfxBlueScreen = *cspacec::csfxBlueScreen;
*csfxWhiteOut = *cspacec::csfxWhiteOut;
*csfxShadeVert = *cspacec::csfxShadeVert;
*csfxScreenDPFX = *cspacec::csfxScreenDPFX;
*csfxScreenDPFXPartial = *cspacec::csfxScreenDPFXPartial;
*CSKEY_SHIFT_NUM = *cspacec::CSKEY_SHIFT_NUM;
*CSKEY_SPECIAL = *cspacec::CSKEY_SPECIAL;
*CSKEY_SPECIAL_NUM = *cspacec::CSKEY_SPECIAL_NUM;
*CSKEY_MODIFIER = *cspacec::CSKEY_MODIFIER;
*CS_IS_KEYBOARD_EVENT = *cspacec::CS_IS_KEYBOARD_EVENT;
*CS_IS_MOUSE_EVENT = *cspacec::CS_IS_MOUSE_EVENT;
*CS_IS_JOYSTICK_EVENT = *cspacec::CS_IS_JOYSTICK_EVENT;
*CS_IS_INPUT_EVENT = *cspacec::CS_IS_INPUT_EVENT;
*csevAllEvents = *cspacec::csevAllEvents;
*csevFrame = *cspacec::csevFrame;
*csevInput = *cspacec::csevInput;
*csevQuit = *cspacec::csevQuit;
*csevKeyboardEvent = *cspacec::csevKeyboardEvent;
*csevKeyboardDown = *cspacec::csevKeyboardDown;
*csevKeyboardUp = *cspacec::csevKeyboardUp;
*csevMouseEvent = *cspacec::csevMouseEvent;
*csevMouseButton = *cspacec::csevMouseButton;
*csevMouseUp = *cspacec::csevMouseUp;
*csevMouseDown = *cspacec::csevMouseDown;
*csevMouseClick = *cspacec::csevMouseClick;
*csevMouseDoubleClick = *cspacec::csevMouseDoubleClick;
*csevMouseMove = *cspacec::csevMouseMove;
*csevJoystickEvent = *cspacec::csevJoystickEvent;
*CS_LOAD_PLUGIN_ALWAYS = *cspacec::CS_LOAD_PLUGIN_ALWAYS;
*CS_FX_SETALPHA = *cspacec::CS_FX_SETALPHA;
*CS_FX_SETALPHA_INT = *cspacec::CS_FX_SETALPHA_INT;
*CS_QUERY_REGISTRY = *cspacec::CS_QUERY_REGISTRY;
*CS_QUERY_REGISTRY_TAG_INTERFACE = *cspacec::CS_QUERY_REGISTRY_TAG_INTERFACE;
*SCF_QUERY_INTERFACE = *cspacec::SCF_QUERY_INTERFACE;
*SCF_QUERY_INTERFACE_SAFE = *cspacec::SCF_QUERY_INTERFACE_SAFE;
*CS_QUERY_PLUGIN_CLASS = *cspacec::CS_QUERY_PLUGIN_CLASS;
*CS_LOAD_PLUGIN = *cspacec::CS_LOAD_PLUGIN;
*CS_GET_CHILD_OBJECT = *cspacec::CS_GET_CHILD_OBJECT;
*CS_GET_NAMED_CHILD_OBJECT = *cspacec::CS_GET_NAMED_CHILD_OBJECT;
*CS_GET_FIRST_NAMED_CHILD_OBJECT = *cspacec::CS_GET_FIRST_NAMED_CHILD_OBJECT;
*AUTOLOAD = *cspacec::AUTOLOAD;
*csInitializer_SetupEventHandler = *cspacec::csInitializer_SetupEventHandler;
*csInitializer_RequestPlugins = *cspacec::csInitializer_RequestPlugins;
*CS_REQUEST_PLUGIN = *cspacec::CS_REQUEST_PLUGIN;
*CS_REQUEST_VFS = *cspacec::CS_REQUEST_VFS;
*CS_REQUEST_FONTSERVER = *cspacec::CS_REQUEST_FONTSERVER;
*CS_REQUEST_IMAGELOADER = *cspacec::CS_REQUEST_IMAGELOADER;
*CS_REQUEST_NULL3D = *cspacec::CS_REQUEST_NULL3D;
*CS_REQUEST_SOFTWARE3D = *cspacec::CS_REQUEST_SOFTWARE3D;
*CS_REQUEST_OPENGL3D = *cspacec::CS_REQUEST_OPENGL3D;
*CS_REQUEST_ENGINE = *cspacec::CS_REQUEST_ENGINE;
*CS_REQUEST_LEVELLOADER = *cspacec::CS_REQUEST_LEVELLOADER;
*CS_REQUEST_LEVELSAVER = *cspacec::CS_REQUEST_LEVELSAVER;
*CS_REQUEST_REPORTER = *cspacec::CS_REQUEST_REPORTER;
*CS_REQUEST_REPORTERLISTENER = *cspacec::CS_REQUEST_REPORTERLISTENER;
*CS_REQUEST_CONSOLEOUT = *cspacec::CS_REQUEST_CONSOLEOUT;
*iCollideSystem_GetCollisionPairs = *cspacec::iCollideSystem_GetCollisionPairs;
*CS_VEC_FORWARD = *cspacec::CS_VEC_FORWARD;
*CS_VEC_BACKWARD = *cspacec::CS_VEC_BACKWARD;
*CS_VEC_RIGHT = *cspacec::CS_VEC_RIGHT;
*CS_VEC_LEFT = *cspacec::CS_VEC_LEFT;
*CS_VEC_UP = *cspacec::CS_VEC_UP;
*CS_VEC_DOWN = *cspacec::CS_VEC_DOWN;
*CS_VEC_ROT_RIGHT = *cspacec::CS_VEC_ROT_RIGHT;
*CS_VEC_ROT_LEFT = *cspacec::CS_VEC_ROT_LEFT;
*CS_VEC_TILT_RIGHT = *cspacec::CS_VEC_TILT_RIGHT;
*CS_VEC_TILT_LEFT = *cspacec::CS_VEC_TILT_LEFT;
*CS_VEC_TILT_UP = *cspacec::CS_VEC_TILT_UP;
*CS_VEC_TILT_DOWN = *cspacec::CS_VEC_TILT_DOWN;

############# Class : cspace::csWrapPtr ##############

package cspace::csWrapPtr;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_Ref_get = *cspacec::csWrapPtr_Ref_get;
*swig_Ref_set = *cspacec::csWrapPtr_Ref_set;
*swig_Type_get = *cspacec::csWrapPtr_Type_get;
*swig_Type_set = *cspacec::csWrapPtr_Type_set;
*swig_Version_get = *cspacec::csWrapPtr_Version_get;
*swig_Version_set = *cspacec::csWrapPtr_Version_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csWrapPtr(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csWrapPtr($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csArrayThresholdVariable ##############

package cspace::csArrayThresholdVariable;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csArrayThresholdVariable(@_);
    bless $self, $pkg if defined($self);
}

*GetThreshold = *cspacec::csArrayThresholdVariable_GetThreshold;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csArrayThresholdVariable($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csArrayThresholdVariableCapacityLinear ##############

package cspace::csArrayThresholdVariableCapacityLinear;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csArrayThresholdVariable cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csArrayThresholdVariableCapacityLinear(@_);
    bless $self, $pkg if defined($self);
}

*IsCapacityExcessive = *cspacec::csArrayThresholdVariableCapacityLinear_IsCapacityExcessive;
*GetCapacity = *cspacec::csArrayThresholdVariableCapacityLinear_GetCapacity;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csArrayThresholdVariableCapacityLinear($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csArrayCapacityDefault ##############

package cspace::csArrayCapacityDefault;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csArrayThresholdVariableCapacityLinear cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csArrayCapacityDefault(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csArrayCapacityDefault($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iBase ##############

package cspace::iBase;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
*IncRef = *cspacec::iBase_IncRef;
*DecRef = *cspacec::iBase_DecRef;
*GetRefCount = *cspacec::iBase_GetRefCount;
*QueryInterface = *cspacec::iBase_QueryInterface;
*AddRefOwner = *cspacec::iBase_AddRefOwner;
*RemoveRefOwner = *cspacec::iBase_RemoveRefOwner;
*scfGetVersion = *cspacec::iBase_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSCF ##############

package cspace::iSCF;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SCF = *cspacec::iSCF_SCF;
*RegisterClasses = *cspacec::iSCF_RegisterClasses;
*ClassRegistered = *cspacec::iSCF_ClassRegistered;
*CreateInstance = *cspacec::iSCF_CreateInstance;
*GetClassDescription = *cspacec::iSCF_GetClassDescription;
*GetClassDependencies = *cspacec::iSCF_GetClassDependencies;
*GetPluginMetadata = *cspacec::iSCF_GetPluginMetadata;
*UnloadUnusedModules = *cspacec::iSCF_UnloadUnusedModules;
*RegisterClass = *cspacec::iSCF_RegisterClass;
*RegisterFactoryFunc = *cspacec::iSCF_RegisterFactoryFunc;
*UnregisterClass = *cspacec::iSCF_UnregisterClass;
*GetInterfaceName = *cspacec::iSCF_GetInterfaceName;
*GetInterfaceID = *cspacec::iSCF_GetInterfaceID;
*Finish = *cspacec::iSCF_Finish;
*QueryClassList = *cspacec::iSCF_QueryClassList;
*ScanPluginsPath = *cspacec::iSCF_ScanPluginsPath;
*RegisterPlugin = *cspacec::iSCF_RegisterPlugin;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSCF($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSCF_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iFactory ##############

package cspace::iFactory;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*CreateInstance = *cspacec::iFactory_CreateInstance;
*TryUnload = *cspacec::iFactory_TryUnload;
*QueryDescription = *cspacec::iFactory_QueryDescription;
*QueryDependencies = *cspacec::iFactory_QueryDependencies;
*QueryClassID = *cspacec::iFactory_QueryClassID;
*QueryModuleName = *cspacec::iFactory_QueryModuleName;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iFactory($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iFactory_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDebugHelper ##############

package cspace::iDebugHelper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetSupportedTests = *cspacec::iDebugHelper_GetSupportedTests;
*UnitTest = *cspacec::iDebugHelper_UnitTest;
*StateTest = *cspacec::iDebugHelper_StateTest;
*Benchmark = *cspacec::iDebugHelper_Benchmark;
*Dump = *cspacec::iDebugHelper_Dump;
*DebugCommand = *cspacec::iDebugHelper_DebugCommand;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDebugHelper($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iDebugHelper_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iCommandLineParser ##############

package cspace::iCommandLineParser;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Initialize = *cspacec::iCommandLineParser_Initialize;
*Reset = *cspacec::iCommandLineParser_Reset;
*GetName = *cspacec::iCommandLineParser_GetName;
*AddOption = *cspacec::iCommandLineParser_AddOption;
*AddName = *cspacec::iCommandLineParser_AddName;
*ReplaceOption = *cspacec::iCommandLineParser_ReplaceOption;
*ReplaceName = *cspacec::iCommandLineParser_ReplaceName;
*GetBoolOption = *cspacec::iCommandLineParser_GetBoolOption;
*GetResourceDir = *cspacec::iCommandLineParser_GetResourceDir;
*GetAppDir = *cspacec::iCommandLineParser_GetAppDir;
*GetAppPath = *cspacec::iCommandLineParser_GetAppPath;
*GetOptionName = *cspacec::iCommandLineParser_GetOptionName;
*GetOption = *cspacec::iCommandLineParser_GetOption;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iCommandLineParser($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iCommandLineParser_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csColor ##############

package cspace::csColor;
use overload
    "-" => sub { $_[0]->__sub__($_[1])},
    "*" => sub { $_[0]->__mul__($_[1])},
    "!=" => sub { $_[0]->__ne__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "+" => sub { $_[0]->__add__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_red_get = *cspacec::csColor_red_get;
*swig_red_set = *cspacec::csColor_red_set;
*swig_green_get = *cspacec::csColor_green_get;
*swig_green_set = *cspacec::csColor_green_set;
*swig_blue_get = *cspacec::csColor_blue_get;
*swig_blue_set = *cspacec::csColor_blue_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csColor(@_);
    bless $self, $pkg if defined($self);
}

*Set = *cspacec::csColor_Set;
*Clamp = *cspacec::csColor_Clamp;
*ClampDown = *cspacec::csColor_ClampDown;
*IsBlack = *cspacec::csColor_IsBlack;
*__copy__ = *cspacec::csColor___copy__;
*__add_ass__ = *cspacec::csColor___add_ass__;
*__subtr_ass__ = *cspacec::csColor___subtr_ass__;
*__mult_ass__ = *cspacec::csColor___mult_ass__;
*__mul__ = *cspacec::csColor___mul__;
*__eq__ = *cspacec::csColor___eq__;
*__ne__ = *cspacec::csColor___ne__;
*Add = *cspacec::csColor_Add;
*Subtract = *cspacec::csColor_Subtract;
*__add__ = *cspacec::csColor___add__;
*__sub__ = *cspacec::csColor___sub__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csColor($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csColor4 ##############

package cspace::csColor4;
use overload
    "!=" => sub { $_[0]->__ne__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csColor cspace );
%OWNER = ();
%ITERATORS = ();
*swig_alpha_get = *cspacec::csColor4_alpha_get;
*swig_alpha_set = *cspacec::csColor4_alpha_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csColor4(@_);
    bless $self, $pkg if defined($self);
}

*Set = *cspacec::csColor4_Set;
*__copy__ = *cspacec::csColor4___copy__;
*__mult_ass__ = *cspacec::csColor4___mult_ass__;
*__add_ass__ = *cspacec::csColor4___add_ass__;
*__subtr_ass__ = *cspacec::csColor4___subtr_ass__;
*__eq__ = *cspacec::csColor4___eq__;
*__ne__ = *cspacec::csColor4___ne__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csColor4($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csCommandLineHelper ##############

package cspace::csCommandLineHelper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*Help = *cspacec::csCommandLineHelper_Help;
*CheckHelp = *cspacec::csCommandLineHelper_CheckHelp;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csCommandLineHelper(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csCommandLineHelper($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csStringSet ##############

package cspace::csStringSet;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csStringSet(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csStringSet($self);
        delete $OWNER{$self};
    }
}

*__copy__ = *cspacec::csStringSet___copy__;
*Request = *cspacec::csStringSet_Request;
*Contains = *cspacec::csStringSet_Contains;
*Delete = *cspacec::csStringSet_Delete;
*Empty = *cspacec::csStringSet_Empty;
*Clear = *cspacec::csStringSet_Clear;
*GetSize = *cspacec::csStringSet_GetSize;
*IsEmpty = *cspacec::csStringSet_IsEmpty;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csStringIDSet ##############

package cspace::csStringIDSet;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csStringIDSet(@_);
    bless $self, $pkg if defined($self);
}

*Add = *cspacec::csStringIDSet_Add;
*AddNoTest = *cspacec::csStringIDSet_AddNoTest;
*Contains = *cspacec::csStringIDSet_Contains;
*In = *cspacec::csStringIDSet_In;
*DeleteAll = *cspacec::csStringIDSet_DeleteAll;
*Empty = *cspacec::csStringIDSet_Empty;
*Delete = *cspacec::csStringIDSet_Delete;
*GetSize = *cspacec::csStringIDSet_GetSize;
*IsEmpty = *cspacec::csStringIDSet_IsEmpty;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csStringIDSet($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iString ##############

package cspace::iString;
use overload
    "!=" => sub { $_[0]->__ne__($_[1])},
    "+" => sub { $_[0]->__add__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetData = *cspacec::iString_GetData;
*DeleteAt = *cspacec::iString_DeleteAt;
*StartsWith = *cspacec::iString_StartsWith;
*__ne__ = *cspacec::iString___ne__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iString($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iString_scfGetVersion;
*__seq__ = *cspacec::iString___seq__;
*__string__ = *cspacec::iString___string__;
*__sv__ = *cspacec::iString___sv__;
*__av__ = *cspacec::iString___av__;
*length = *cspacec::iString_length;
*__add__ = *cspacec::iString___add__;
*__concat__ = *cspacec::iString___concat__;
*__concat_ass__ = *cspacec::iString___concat_ass__;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csStringParent ##############

package cspace::csStringParent;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csStringParent(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csStringParent($self);
        delete $OWNER{$self};
    }
}

*__copy__ = *cspacec::csStringParent___copy__;
*GetData = *cspacec::csStringParent_GetData;
*GetCapacity = *cspacec::csStringParent_GetCapacity;
*ShrinkBestFit = *cspacec::csStringParent_ShrinkBestFit;
*Free = *cspacec::csStringParent_Free;
*Detach = *cspacec::csStringParent_Detach;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csString ##############

package cspace::csString;
use overload
    "==" => sub { $_[0]->__eq__($_[1])},
    "+" => sub { $_[0]->__add__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csStringParent cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csString(@_);
    bless $self, $pkg if defined($self);
}

*__eq__ = *cspacec::csString___eq__;
*__seq__ = *cspacec::csString___seq__;
*__string__ = *cspacec::csString___string__;
*__sv__ = *cspacec::csString___sv__;
*__av__ = *cspacec::csString___av__;
*length = *cspacec::csString_length;
*__add__ = *cspacec::csString___add__;
*__concat__ = *cspacec::csString___concat__;
*__concat_ass__ = *cspacec::csString___concat_ass__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csString($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csRefCount ##############

package cspace::csRefCount;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csRefCount(@_);
    bless $self, $pkg if defined($self);
}

*IncRef = *cspacec::csRefCount_IncRef;
*DecRef = *cspacec::csRefCount_DecRef;
*GetRefCount = *cspacec::csRefCount_GetRefCount;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csVector2 ##############

package cspace::csVector2;
use overload
    "-" => sub { $_[0]->__sub__($_[1])},
    "neg" => sub { $_[0]->__neg__()},
    "*" => sub { $_[0]->__mul__($_[1])},
    "/" => sub { $_[0]->__div__($_[1])},
    "!=" => sub { $_[0]->__ne__($_[1])},
    "<" => sub { $_[0]->__lt__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "+" => sub { $_[0]->__add__($_[1])},
    ">" => sub { $_[0]->__gt__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_x_get = *cspacec::csVector2_x_get;
*swig_x_set = *cspacec::csVector2_x_set;
*swig_y_get = *cspacec::csVector2_y_get;
*swig_y_set = *cspacec::csVector2_y_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csVector2(@_);
    bless $self, $pkg if defined($self);
}

*Description = *cspacec::csVector2_Description;
*Set = *cspacec::csVector2_Set;
*Get = *cspacec::csVector2_Get;
*Norm = *cspacec::csVector2_Norm;
*SquaredNorm = *cspacec::csVector2_SquaredNorm;
*Rotate = *cspacec::csVector2_Rotate;
*IsLeft = *cspacec::csVector2_IsLeft;
*__add_ass__ = *cspacec::csVector2___add_ass__;
*__subtr_ass__ = *cspacec::csVector2___subtr_ass__;
*__mult_ass__ = *cspacec::csVector2___mult_ass__;
*__divide_ass__ = *cspacec::csVector2___divide_ass__;
*__neg__ = *cspacec::csVector2___neg__;
*__add__ = *cspacec::csVector2___add__;
*__sub__ = *cspacec::csVector2___sub__;
*__mul__ = *cspacec::csVector2___mul__;
*__div__ = *cspacec::csVector2___div__;
*__eq__ = *cspacec::csVector2___eq__;
*__ne__ = *cspacec::csVector2___ne__;
*__lt__ = *cspacec::csVector2___lt__;
*__gt__ = *cspacec::csVector2___gt__;
*__abs__ = *cspacec::csVector2___abs__;
*__av__ = *cspacec::csVector2___av__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csVector2($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csVector3 ##############

package cspace::csVector3;
use overload
    "-" => sub { $_[0]->__sub__($_[1])},
    "neg" => sub { $_[0]->__neg__()},
    "*" => sub { $_[0]->__mul__($_[1])},
    "/" => sub { $_[0]->__div__($_[1])},
    "!=" => sub { $_[0]->__ne__($_[1])},
    "<" => sub { $_[0]->__lt__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "+" => sub { $_[0]->__add__($_[1])},
    ">" => sub { $_[0]->__gt__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_x_get = *cspacec::csVector3_x_get;
*swig_x_set = *cspacec::csVector3_x_set;
*swig_y_get = *cspacec::csVector3_y_get;
*swig_y_set = *cspacec::csVector3_y_set;
*swig_z_get = *cspacec::csVector3_z_get;
*swig_z_set = *cspacec::csVector3_z_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csVector3(@_);
    bless $self, $pkg if defined($self);
}

*Description = *cspacec::csVector3_Description;
*Cross = *cspacec::csVector3_Cross;
*__add_ass__ = *cspacec::csVector3___add_ass__;
*__subtr_ass__ = *cspacec::csVector3___subtr_ass__;
*__neg__ = *cspacec::csVector3___neg__;
*Set = *cspacec::csVector3_Set;
*Get = *cspacec::csVector3_Get;
*Norm = *cspacec::csVector3_Norm;
*SquaredNorm = *cspacec::csVector3_SquaredNorm;
*Unit = *cspacec::csVector3_Unit;
*Normalize = *cspacec::csVector3_Normalize;
*IsZero = *cspacec::csVector3_IsZero;
*UnitAxisClamped = *cspacec::csVector3_UnitAxisClamped;
*DominantAxis = *cspacec::csVector3_DominantAxis;
*__add__ = *cspacec::csVector3___add__;
*__sub__ = *cspacec::csVector3___sub__;
*__mul__ = *cspacec::csVector3___mul__;
*__eq__ = *cspacec::csVector3___eq__;
*__ne__ = *cspacec::csVector3___ne__;
*__lt__ = *cspacec::csVector3___lt__;
*__gt__ = *cspacec::csVector3___gt__;
*__mult_ass__ = *cspacec::csVector3___mult_ass__;
*__divide_ass__ = *cspacec::csVector3___divide_ass__;
*__div__ = *cspacec::csVector3___div__;
*project = *cspacec::csVector3_project;
*__abs__ = *cspacec::csVector3___abs__;
*__av__ = *cspacec::csVector3___av__;
*__bool__ = *cspacec::csVector3___bool__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csVector3($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csVector4Float ##############

package cspace::csVector4Float;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_x_get = *cspacec::csVector4Float_x_get;
*swig_x_set = *cspacec::csVector4Float_x_set;
*swig_y_get = *cspacec::csVector4Float_y_get;
*swig_y_set = *cspacec::csVector4Float_y_set;
*swig_z_get = *cspacec::csVector4Float_z_get;
*swig_z_set = *cspacec::csVector4Float_z_set;
*swig_w_get = *cspacec::csVector4Float_w_get;
*swig_w_set = *cspacec::csVector4Float_w_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csVector4Float(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csVector4Float($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csVector4 ##############

package cspace::csVector4;
use overload
    "-" => sub { $_[0]->__sub__($_[1])},
    "*" => sub { $_[0]->__mul__($_[1])},
    "/" => sub { $_[0]->__div__($_[1])},
    "!=" => sub { $_[0]->__ne__($_[1])},
    "<" => sub { $_[0]->__lt__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "+" => sub { $_[0]->__add__($_[1])},
    ">" => sub { $_[0]->__gt__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csVector4Float cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csVector4(@_);
    bless $self, $pkg if defined($self);
}

*__copy__ = *cspacec::csVector4___copy__;
*__add__ = *cspacec::csVector4___add__;
*__sub__ = *cspacec::csVector4___sub__;
*__mul__ = *cspacec::csVector4___mul__;
*__div__ = *cspacec::csVector4___div__;
*__eq__ = *cspacec::csVector4___eq__;
*__ne__ = *cspacec::csVector4___ne__;
*__lt__ = *cspacec::csVector4___lt__;
*__gt__ = *cspacec::csVector4___gt__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csVector4($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csMatrix2 ##############

package cspace::csMatrix2;
use overload
    "neg" => sub { $_[0]->__neg__()},
    "+" => sub { $_[0]->__add__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_m11_get = *cspacec::csMatrix2_m11_get;
*swig_m11_set = *cspacec::csMatrix2_m11_set;
*swig_m12_get = *cspacec::csMatrix2_m12_get;
*swig_m12_set = *cspacec::csMatrix2_m12_set;
*swig_m21_get = *cspacec::csMatrix2_m21_get;
*swig_m21_set = *cspacec::csMatrix2_m21_set;
*swig_m22_get = *cspacec::csMatrix2_m22_get;
*swig_m22_set = *cspacec::csMatrix2_m22_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csMatrix2(@_);
    bless $self, $pkg if defined($self);
}

*Row1 = *cspacec::csMatrix2_Row1;
*Row2 = *cspacec::csMatrix2_Row2;
*Col1 = *cspacec::csMatrix2_Col1;
*Col2 = *cspacec::csMatrix2_Col2;
*Set = *cspacec::csMatrix2_Set;
*__add_ass__ = *cspacec::csMatrix2___add_ass__;
*__subtr_ass__ = *cspacec::csMatrix2___subtr_ass__;
*__mult_ass__ = *cspacec::csMatrix2___mult_ass__;
*__divide_ass__ = *cspacec::csMatrix2___divide_ass__;
*__add__ = *cspacec::csMatrix2___add__;
*__neg__ = *cspacec::csMatrix2___neg__;
*Transpose = *cspacec::csMatrix2_Transpose;
*GetTranspose = *cspacec::csMatrix2_GetTranspose;
*GetInverse = *cspacec::csMatrix2_GetInverse;
*Invert = *cspacec::csMatrix2_Invert;
*Determinant = *cspacec::csMatrix2_Determinant;
*Identity = *cspacec::csMatrix2_Identity;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csMatrix2($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csMatrix3 ##############

package cspace::csMatrix3;
use overload
    "-" => sub { $_[0]->__sub__($_[1])},
    "neg" => sub { $_[0]->__neg__()},
    "/" => sub { $_[0]->__div__($_[1])},
    "*" => sub { $_[0]->__mul__($_[1])},
    "!=" => sub { $_[0]->__ne__($_[1])},
    "<" => sub { $_[0]->__lt__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "+" => sub { $_[0]->__add__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_m11_get = *cspacec::csMatrix3_m11_get;
*swig_m11_set = *cspacec::csMatrix3_m11_set;
*swig_m12_get = *cspacec::csMatrix3_m12_get;
*swig_m12_set = *cspacec::csMatrix3_m12_set;
*swig_m13_get = *cspacec::csMatrix3_m13_get;
*swig_m13_set = *cspacec::csMatrix3_m13_set;
*swig_m21_get = *cspacec::csMatrix3_m21_get;
*swig_m21_set = *cspacec::csMatrix3_m21_set;
*swig_m22_get = *cspacec::csMatrix3_m22_get;
*swig_m22_set = *cspacec::csMatrix3_m22_set;
*swig_m23_get = *cspacec::csMatrix3_m23_get;
*swig_m23_set = *cspacec::csMatrix3_m23_set;
*swig_m31_get = *cspacec::csMatrix3_m31_get;
*swig_m31_set = *cspacec::csMatrix3_m31_set;
*swig_m32_get = *cspacec::csMatrix3_m32_get;
*swig_m32_set = *cspacec::csMatrix3_m32_set;
*swig_m33_get = *cspacec::csMatrix3_m33_get;
*swig_m33_set = *cspacec::csMatrix3_m33_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csMatrix3(@_);
    bless $self, $pkg if defined($self);
}

*Row1 = *cspacec::csMatrix3_Row1;
*Row2 = *cspacec::csMatrix3_Row2;
*Row3 = *cspacec::csMatrix3_Row3;
*Col1 = *cspacec::csMatrix3_Col1;
*Col2 = *cspacec::csMatrix3_Col2;
*Col3 = *cspacec::csMatrix3_Col3;
*Set = *cspacec::csMatrix3_Set;
*__copy__ = *cspacec::csMatrix3___copy__;
*__add_ass__ = *cspacec::csMatrix3___add_ass__;
*__subtr_ass__ = *cspacec::csMatrix3___subtr_ass__;
*__divide_ass__ = *cspacec::csMatrix3___divide_ass__;
*__neg__ = *cspacec::csMatrix3___neg__;
*Transpose = *cspacec::csMatrix3_Transpose;
*GetTranspose = *cspacec::csMatrix3_GetTranspose;
*GetInverse = *cspacec::csMatrix3_GetInverse;
*Invert = *cspacec::csMatrix3_Invert;
*Determinant = *cspacec::csMatrix3_Determinant;
*Identity = *cspacec::csMatrix3_Identity;
*IsIdentity = *cspacec::csMatrix3_IsIdentity;
*__add__ = *cspacec::csMatrix3___add__;
*__sub__ = *cspacec::csMatrix3___sub__;
*__div__ = *cspacec::csMatrix3___div__;
*__eq__ = *cspacec::csMatrix3___eq__;
*__ne__ = *cspacec::csMatrix3___ne__;
*__lt__ = *cspacec::csMatrix3___lt__;
*__mul__ = *cspacec::csMatrix3___mul__;
*__mult_ass__ = *cspacec::csMatrix3___mult_ass__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csMatrix3($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csXRotMatrix3 ##############

package cspace::csXRotMatrix3;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csMatrix3 cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csXRotMatrix3(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csXRotMatrix3($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csYRotMatrix3 ##############

package cspace::csYRotMatrix3;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csMatrix3 cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csYRotMatrix3(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csYRotMatrix3($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csZRotMatrix3 ##############

package cspace::csZRotMatrix3;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csMatrix3 cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csZRotMatrix3(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csZRotMatrix3($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csXScaleMatrix3 ##############

package cspace::csXScaleMatrix3;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csMatrix3 cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csXScaleMatrix3(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csXScaleMatrix3($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csYScaleMatrix3 ##############

package cspace::csYScaleMatrix3;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csMatrix3 cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csYScaleMatrix3(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csYScaleMatrix3($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csZScaleMatrix3 ##############

package cspace::csZScaleMatrix3;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csMatrix3 cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csZScaleMatrix3(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csZScaleMatrix3($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csTransform ##############

package cspace::csTransform;
use overload
    "*" => sub { $_[0]->__mul__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csTransform($self);
        delete $OWNER{$self};
    }
}

sub new {
    my $pkg = shift;
    my $self = cspacec::new_csTransform(@_);
    bless $self, $pkg if defined($self);
}

*Identity = *cspacec::csTransform_Identity;
*IsIdentity = *cspacec::csTransform_IsIdentity;
*GetO2T = *cspacec::csTransform_GetO2T;
*GetO2TTranslation = *cspacec::csTransform_GetO2TTranslation;
*GetOrigin = *cspacec::csTransform_GetOrigin;
*SetO2T = *cspacec::csTransform_SetO2T;
*SetO2TTranslation = *cspacec::csTransform_SetO2TTranslation;
*SetOrigin = *cspacec::csTransform_SetOrigin;
*Translate = *cspacec::csTransform_Translate;
*Other2ThisRelative = *cspacec::csTransform_Other2ThisRelative;
*Other2This = *cspacec::csTransform_Other2This;
*GetReflect = *cspacec::csTransform_GetReflect;
*GetFront = *cspacec::csTransform_GetFront;
*GetUp = *cspacec::csTransform_GetUp;
*GetRight = *cspacec::csTransform_GetRight;
*__mul__ = *cspacec::csTransform___mul__;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csReversibleTransform ##############

package cspace::csReversibleTransform;
use overload
    "*" => sub { $_[0]->__mul__($_[1])},
    "/" => sub { $_[0]->__div__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csTransform cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csReversibleTransform(@_);
    bless $self, $pkg if defined($self);
}

*GetT2O = *cspacec::csReversibleTransform_GetT2O;
*GetT2OTranslation = *cspacec::csReversibleTransform_GetT2OTranslation;
*GetInverse = *cspacec::csReversibleTransform_GetInverse;
*SetO2T = *cspacec::csReversibleTransform_SetO2T;
*SetT2O = *cspacec::csReversibleTransform_SetT2O;
*This2OtherRelative = *cspacec::csReversibleTransform_This2OtherRelative;
*This2Other = *cspacec::csReversibleTransform_This2Other;
*RotateOther = *cspacec::csReversibleTransform_RotateOther;
*RotateThis = *cspacec::csReversibleTransform_RotateThis;
*LookAt = *cspacec::csReversibleTransform_LookAt;
*__mult_ass__ = *cspacec::csReversibleTransform___mult_ass__;
*__mul__ = *cspacec::csReversibleTransform___mul__;
*__divide_ass__ = *cspacec::csReversibleTransform___divide_ass__;
*__div__ = *cspacec::csReversibleTransform___div__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csReversibleTransform($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csOrthoTransform ##############

package cspace::csOrthoTransform;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csReversibleTransform cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csOrthoTransform(@_);
    bless $self, $pkg if defined($self);
}

*SetO2T = *cspacec::csOrthoTransform_SetO2T;
*SetT2O = *cspacec::csOrthoTransform_SetT2O;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csOrthoTransform($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSphere ##############

package cspace::csSphere;
use overload
    "/" => sub { $_[0]->__div__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csSphere(@_);
    bless $self, $pkg if defined($self);
}

*GetCenter = *cspacec::csSphere_GetCenter;
*SetCenter = *cspacec::csSphere_SetCenter;
*GetRadius = *cspacec::csSphere_GetRadius;
*SetRadius = *cspacec::csSphere_SetRadius;
*Union = *cspacec::csSphere_Union;
*__add_ass__ = *cspacec::csSphere___add_ass__;
*TestIntersect = *cspacec::csSphere_TestIntersect;
*__mult_ass__ = *cspacec::csSphere___mult_ass__;
*__div__ = *cspacec::csSphere___div__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSphere($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csEllipsoid ##############

package cspace::csEllipsoid;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csEllipsoid(@_);
    bless $self, $pkg if defined($self);
}

*GetCenter = *cspacec::csEllipsoid_GetCenter;
*SetCenter = *cspacec::csEllipsoid_SetCenter;
*GetRadius = *cspacec::csEllipsoid_GetRadius;
*SetRadius = *cspacec::csEllipsoid_SetRadius;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csEllipsoid($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPlane2 ##############

package cspace::csPlane2;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_norm_get = *cspacec::csPlane2_norm_get;
*swig_norm_set = *cspacec::csPlane2_norm_set;
*swig_CC_get = *cspacec::csPlane2_CC_get;
*swig_CC_set = *cspacec::csPlane2_CC_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPlane2(@_);
    bless $self, $pkg if defined($self);
}

*Normal = *cspacec::csPlane2_Normal;
*GetNormal = *cspacec::csPlane2_GetNormal;
*A = *cspacec::csPlane2_A;
*B = *cspacec::csPlane2_B;
*C = *cspacec::csPlane2_C;
*Set = *cspacec::csPlane2_Set;
*Classify = *cspacec::csPlane2_Classify;
*Distance = *cspacec::csPlane2_Distance;
*SquaredDistance = *cspacec::csPlane2_SquaredDistance;
*Invert = *cspacec::csPlane2_Invert;
*Normalize = *cspacec::csPlane2_Normalize;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPlane2($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPlane3 ##############

package cspace::csPlane3;
use overload
    "/" => sub { $_[0]->__div__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_norm_get = *cspacec::csPlane3_norm_get;
*swig_norm_set = *cspacec::csPlane3_norm_set;
*swig_DD_get = *cspacec::csPlane3_DD_get;
*swig_DD_set = *cspacec::csPlane3_DD_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPlane3(@_);
    bless $self, $pkg if defined($self);
}

*Normal = *cspacec::csPlane3_Normal;
*A = *cspacec::csPlane3_A;
*B = *cspacec::csPlane3_B;
*C = *cspacec::csPlane3_C;
*D = *cspacec::csPlane3_D;
*GetNormal = *cspacec::csPlane3_GetNormal;
*Set = *cspacec::csPlane3_Set;
*SetOrigin = *cspacec::csPlane3_SetOrigin;
*Classify = *cspacec::csPlane3_Classify;
*Distance = *cspacec::csPlane3_Distance;
*Invert = *cspacec::csPlane3_Invert;
*Normalize = *cspacec::csPlane3_Normalize;
*FindPoint = *cspacec::csPlane3_FindPoint;
*ClipPolygon = *cspacec::csPlane3_ClipPolygon;
*Description = *cspacec::csPlane3_Description;
*__mult_ass__ = *cspacec::csPlane3___mult_ass__;
*__divide_ass__ = *cspacec::csPlane3___divide_ass__;
*__div__ = *cspacec::csPlane3___div__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPlane3($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csMath2 ##############

package cspace::csMath2;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*WhichSide2D = *cspacec::csMath2_WhichSide2D;
*InPoly2D = *cspacec::csMath2_InPoly2D;
*Area2 = *cspacec::csMath2_Area2;
*Right = *cspacec::csMath2_Right;
*Left = *cspacec::csMath2_Left;
*Visible = *cspacec::csMath2_Visible;
*PlanesEqual = *cspacec::csMath2_PlanesEqual;
*PlanesClose = *cspacec::csMath2_PlanesClose;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csMath2(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csMath2($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csIntersect2 ##############

package cspace::csIntersect2;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*PlanePolygon = *cspacec::csIntersect2_PlanePolygon;
*SegmentSegment = *cspacec::csIntersect2_SegmentSegment;
*SegmentLine = *cspacec::csIntersect2_SegmentLine;
*LineLine = *cspacec::csIntersect2_LineLine;
*SegmentPlane = *cspacec::csIntersect2_SegmentPlane;
*SegmentPlaneNoTest = *cspacec::csIntersect2_SegmentPlaneNoTest;
*PlanePlane = *cspacec::csIntersect2_PlanePlane;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csIntersect2(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csIntersect2($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPoly2D ##############

package cspace::csPoly2D;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPoly2D(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPoly2D($self);
        delete $OWNER{$self};
    }
}

*__copy__ = *cspacec::csPoly2D___copy__;
*MakeEmpty = *cspacec::csPoly2D_MakeEmpty;
*GetVertexCount = *cspacec::csPoly2D_GetVertexCount;
*GetVertices = *cspacec::csPoly2D_GetVertices;
*GetVertex = *cspacec::csPoly2D_GetVertex;
*GetFirst = *cspacec::csPoly2D_GetFirst;
*GetLast = *cspacec::csPoly2D_GetLast;
*In = *cspacec::csPoly2D_In;
*MakeRoom = *cspacec::csPoly2D_MakeRoom;
*SetVertexCount = *cspacec::csPoly2D_SetVertexCount;
*AddVertex = *cspacec::csPoly2D_AddVertex;
*SetVertices = *cspacec::csPoly2D_SetVertices;
*ClipAgainst = *cspacec::csPoly2D_ClipAgainst;
*Intersect = *cspacec::csPoly2D_Intersect;
*ClipPlane = *cspacec::csPoly2D_ClipPlane;
*ExtendConvex = *cspacec::csPoly2D_ExtendConvex;
*GetSignedArea = *cspacec::csPoly2D_GetSignedArea;
*Random = *cspacec::csPoly2D_Random;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPoly2DFactory ##############

package cspace::csPoly2DFactory;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPoly2DFactory($self);
        delete $OWNER{$self};
    }
}

*Create = *cspacec::csPoly2DFactory_Create;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPoly2DFactory(@_);
    bless $self, $pkg if defined($self);
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csMath3 ##############

package cspace::csMath3;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*WhichSide3D = *cspacec::csMath3_WhichSide3D;
*Visible = *cspacec::csMath3_Visible;
*Between = *cspacec::csMath3_Between;
*SetMinMax = *cspacec::csMath3_SetMinMax;
*DoubleArea3 = *cspacec::csMath3_DoubleArea3;
*Direction3 = *cspacec::csMath3_Direction3;
*CalcNormal = *cspacec::csMath3_CalcNormal;
*CalcPlane = *cspacec::csMath3_CalcPlane;
*PlanesEqual = *cspacec::csMath3_PlanesEqual;
*PlanesClose = *cspacec::csMath3_PlanesClose;
*OuterPlanes = *cspacec::csMath3_OuterPlanes;
*FindObserverSides = *cspacec::csMath3_FindObserverSides;
*SpherePosition = *cspacec::csMath3_SpherePosition;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csMath3(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csMath3($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSquaredDist ##############

package cspace::csSquaredDist;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*PointPoint = *cspacec::csSquaredDist_PointPoint;
*PointLine = *cspacec::csSquaredDist_PointLine;
*PointPlane = *cspacec::csSquaredDist_PointPlane;
*PointPoly = *cspacec::csSquaredDist_PointPoly;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csSquaredDist(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSquaredDist($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csIntersect3 ##############

package cspace::csIntersect3;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*PlanePolygon = *cspacec::csIntersect3_PlanePolygon;
*SegmentFrustum = *cspacec::csIntersect3_SegmentFrustum;
*SegmentTriangle = *cspacec::csIntersect3_SegmentTriangle;
*SegmentPolygon = *cspacec::csIntersect3_SegmentPolygon;
*SegmentPlanes = *cspacec::csIntersect3_SegmentPlanes;
*SegmentPlane = *cspacec::csIntersect3_SegmentPlane;
*ThreePlanes = *cspacec::csIntersect3_ThreePlanes;
*PlaneXPlane = *cspacec::csIntersect3_PlaneXPlane;
*PlaneYPlane = *cspacec::csIntersect3_PlaneYPlane;
*PlaneZPlane = *cspacec::csIntersect3_PlaneZPlane;
*PlaneAxisPlane = *cspacec::csIntersect3_PlaneAxisPlane;
*SegmentZ0Plane = *cspacec::csIntersect3_SegmentZ0Plane;
*SegmentXPlane = *cspacec::csIntersect3_SegmentXPlane;
*SegmentYPlane = *cspacec::csIntersect3_SegmentYPlane;
*SegmentZPlane = *cspacec::csIntersect3_SegmentZPlane;
*SegmentAxisPlane = *cspacec::csIntersect3_SegmentAxisPlane;
*SegmentXFrustum = *cspacec::csIntersect3_SegmentXFrustum;
*SegmentYFrustum = *cspacec::csIntersect3_SegmentYFrustum;
*BoxSegment = *cspacec::csIntersect3_BoxSegment;
*BoxFrustum = *cspacec::csIntersect3_BoxFrustum;
*BoxSphere = *cspacec::csIntersect3_BoxSphere;
*BoxPlane = *cspacec::csIntersect3_BoxPlane;
*BoxTriangle = *cspacec::csIntersect3_BoxTriangle;
*BoxBox = *cspacec::csIntersect3_BoxBox;
*FrustumFrustum = *cspacec::csIntersect3_FrustumFrustum;
*TriangleTriangle = *cspacec::csIntersect3_TriangleTriangle;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csIntersect3(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csIntersect3($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csGeomDebugHelper ##############

package cspace::csGeomDebugHelper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csGeomDebugHelper(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csGeomDebugHelper($self);
        delete $OWNER{$self};
    }
}

*GetSupportedTests = *cspacec::csGeomDebugHelper_GetSupportedTests;
*UnitTest = *cspacec::csGeomDebugHelper_UnitTest;
*StateTest = *cspacec::csGeomDebugHelper_StateTest;
*Benchmark = *cspacec::csGeomDebugHelper_Benchmark;
*Dump = *cspacec::csGeomDebugHelper_Dump;
*DebugCommand = *cspacec::csGeomDebugHelper_DebugCommand;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPoly3D ##############

package cspace::csPoly3D;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPoly3D(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPoly3D($self);
        delete $OWNER{$self};
    }
}

*MakeEmpty = *cspacec::csPoly3D_MakeEmpty;
*GetVertexCount = *cspacec::csPoly3D_GetVertexCount;
*GetVertices = *cspacec::csPoly3D_GetVertices;
*GetVertex = *cspacec::csPoly3D_GetVertex;
*GetFirst = *cspacec::csPoly3D_GetFirst;
*GetLast = *cspacec::csPoly3D_GetLast;
*In = *cspacec::csPoly3D_In;
*MakeRoom = *cspacec::csPoly3D_MakeRoom;
*SetVertexCount = *cspacec::csPoly3D_SetVertexCount;
*AddVertex = *cspacec::csPoly3D_AddVertex;
*SetVertices = *cspacec::csPoly3D_SetVertices;
*ProjectXPlane = *cspacec::csPoly3D_ProjectXPlane;
*ProjectYPlane = *cspacec::csPoly3D_ProjectYPlane;
*ProjectZPlane = *cspacec::csPoly3D_ProjectZPlane;
*ProjectAxisPlane = *cspacec::csPoly3D_ProjectAxisPlane;
*Classify = *cspacec::csPoly3D_Classify;
*ClassifyX = *cspacec::csPoly3D_ClassifyX;
*ClassifyY = *cspacec::csPoly3D_ClassifyY;
*ClassifyZ = *cspacec::csPoly3D_ClassifyZ;
*ClassifyAxis = *cspacec::csPoly3D_ClassifyAxis;
*IsAxisAligned = *cspacec::csPoly3D_IsAxisAligned;
*ComputeMainNormalAxis = *cspacec::csPoly3D_ComputeMainNormalAxis;
*CutToPlane = *cspacec::csPoly3D_CutToPlane;
*SplitWithPlane = *cspacec::csPoly3D_SplitWithPlane;
*SplitWithPlaneX = *cspacec::csPoly3D_SplitWithPlaneX;
*SplitWithPlaneY = *cspacec::csPoly3D_SplitWithPlaneY;
*SplitWithPlaneZ = *cspacec::csPoly3D_SplitWithPlaneZ;
*ComputeNormal = *cspacec::csPoly3D_ComputeNormal;
*ComputePlane = *cspacec::csPoly3D_ComputePlane;
*GetArea = *cspacec::csPoly3D_GetArea;
*GetCenter = *cspacec::csPoly3D_GetCenter;
*InSphere = *cspacec::csPoly3D_InSphere;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csCompressVertex ##############

package cspace::csCompressVertex;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_orig_idx_get = *cspacec::csCompressVertex_orig_idx_get;
*swig_orig_idx_set = *cspacec::csCompressVertex_orig_idx_set;
*swig_x_get = *cspacec::csCompressVertex_x_get;
*swig_x_set = *cspacec::csCompressVertex_x_set;
*swig_y_get = *cspacec::csCompressVertex_y_get;
*swig_y_set = *cspacec::csCompressVertex_y_set;
*swig_z_get = *cspacec::csCompressVertex_z_get;
*swig_z_set = *cspacec::csCompressVertex_z_set;
*swig_new_idx_get = *cspacec::csCompressVertex_new_idx_get;
*swig_new_idx_set = *cspacec::csCompressVertex_new_idx_set;
*swig_used_get = *cspacec::csCompressVertex_used_get;
*swig_used_set = *cspacec::csCompressVertex_used_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csCompressVertex(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csCompressVertex($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csVector3Array ##############

package cspace::csVector3Array;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csPoly3D cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csVector3Array(@_);
    bless $self, $pkg if defined($self);
}

*AddVertexSmart = *cspacec::csVector3Array_AddVertexSmart;
*CompressVertices = *cspacec::csVector3Array_CompressVertices;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csVector3Array($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::TriangleInt ##############

package cspace::TriangleInt;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_a_get = *cspacec::TriangleInt_a_get;
*swig_a_set = *cspacec::TriangleInt_a_set;
*swig_b_get = *cspacec::TriangleInt_b_get;
*swig_b_set = *cspacec::TriangleInt_b_set;
*swig_c_get = *cspacec::TriangleInt_c_get;
*swig_c_set = *cspacec::TriangleInt_c_set;
*Set = *cspacec::TriangleInt_Set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_TriangleInt(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_TriangleInt($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csTriangle ##############

package cspace::csTriangle;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::TriangleInt cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csTriangle(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csTriangle($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csRect ##############

package cspace::csRect;
use overload
    "!=" => sub { $_[0]->__ne__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_xmin_get = *cspacec::csRect_xmin_get;
*swig_xmin_set = *cspacec::csRect_xmin_set;
*swig_ymin_get = *cspacec::csRect_ymin_get;
*swig_ymin_set = *cspacec::csRect_ymin_set;
*swig_xmax_get = *cspacec::csRect_xmax_get;
*swig_xmax_set = *cspacec::csRect_xmax_set;
*swig_ymax_get = *cspacec::csRect_ymax_get;
*swig_ymax_set = *cspacec::csRect_ymax_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csRect(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csRect($self);
        delete $OWNER{$self};
    }
}

*Intersect = *cspacec::csRect_Intersect;
*Intersects = *cspacec::csRect_Intersects;
*Union = *cspacec::csRect_Union;
*Exclude = *cspacec::csRect_Exclude;
*Subtract = *cspacec::csRect_Subtract;
*IsEmpty = *cspacec::csRect_IsEmpty;
*MakeEmpty = *cspacec::csRect_MakeEmpty;
*Set = *cspacec::csRect_Set;
*SetPos = *cspacec::csRect_SetPos;
*SetSize = *cspacec::csRect_SetSize;
*Move = *cspacec::csRect_Move;
*Width = *cspacec::csRect_Width;
*Height = *cspacec::csRect_Height;
*Contains = *cspacec::csRect_Contains;
*ContainsRel = *cspacec::csRect_ContainsRel;
*Equal = *cspacec::csRect_Equal;
*Normalize = *cspacec::csRect_Normalize;
*Area = *cspacec::csRect_Area;
*AddAdjacent = *cspacec::csRect_AddAdjacent;
*__eq__ = *cspacec::csRect___eq__;
*__ne__ = *cspacec::csRect___ne__;
*Extend = *cspacec::csRect_Extend;
*Join = *cspacec::csRect_Join;
*Outset = *cspacec::csRect_Outset;
*Inset = *cspacec::csRect_Inset;
*ClipLineGeneral = *cspacec::csRect_ClipLineGeneral;
*ClipLine = *cspacec::csRect_ClipLine;
*ClipLineSafe = *cspacec::csRect_ClipLineSafe;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csRectRegion ##############

package cspace::csRectRegion;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csRectRegion(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csRectRegion($self);
        delete $OWNER{$self};
    }
}

*Include = *cspacec::csRectRegion_Include;
*Exclude = *cspacec::csRectRegion_Exclude;
*ClipTo = *cspacec::csRectRegion_ClipTo;
*Count = *cspacec::csRectRegion_Count;
*RectAt = *cspacec::csRectRegion_RectAt;
*MakeEmpty = *cspacec::csRectRegion_MakeEmpty;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csQuaternion ##############

package cspace::csQuaternion;
use overload
    "-" => sub { $_[0]->__sub__($_[1])},
    "*" => sub { $_[0]->__mul__($_[1])},
    "+" => sub { $_[0]->__add__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csQuaternion(@_);
    bless $self, $pkg if defined($self);
}

*Set = *cspacec::csQuaternion_Set;
*SetIdentity = *cspacec::csQuaternion_SetIdentity;
*__add_ass__ = *cspacec::csQuaternion___add_ass__;
*__subtr_ass__ = *cspacec::csQuaternion___subtr_ass__;
*__mult_ass__ = *cspacec::csQuaternion___mult_ass__;
*GetConjugate = *cspacec::csQuaternion_GetConjugate;
*Conjugate = *cspacec::csQuaternion_Conjugate;
*Dot = *cspacec::csQuaternion_Dot;
*SquaredNorm = *cspacec::csQuaternion_SquaredNorm;
*Norm = *cspacec::csQuaternion_Norm;
*Unit = *cspacec::csQuaternion_Unit;
*Rotate = *cspacec::csQuaternion_Rotate;
*SetAxisAngle = *cspacec::csQuaternion_SetAxisAngle;
*GetAxisAngle = *cspacec::csQuaternion_GetAxisAngle;
*SetEulerAngles = *cspacec::csQuaternion_SetEulerAngles;
*GetEulerAngles = *cspacec::csQuaternion_GetEulerAngles;
*SetMatrix = *cspacec::csQuaternion_SetMatrix;
*GetMatrix = *cspacec::csQuaternion_GetMatrix;
*NLerp = *cspacec::csQuaternion_NLerp;
*SLerp = *cspacec::csQuaternion_SLerp;
*Log = *cspacec::csQuaternion_Log;
*Exp = *cspacec::csQuaternion_Exp;
*Squad = *cspacec::csQuaternion_Squad;
*swig_v_get = *cspacec::csQuaternion_v_get;
*swig_v_set = *cspacec::csQuaternion_v_set;
*swig_w_get = *cspacec::csQuaternion_w_get;
*swig_w_set = *cspacec::csQuaternion_w_set;
*__add__ = *cspacec::csQuaternion___add__;
*__sub__ = *cspacec::csQuaternion___sub__;
*__mul__ = *cspacec::csQuaternion___mul__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csQuaternion($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSpline ##############

package cspace::csSpline;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSpline($self);
        delete $OWNER{$self};
    }
}

*GetDimensionCount = *cspacec::csSpline_GetDimensionCount;
*GetPointCount = *cspacec::csSpline_GetPointCount;
*InsertPoint = *cspacec::csSpline_InsertPoint;
*RemovePoint = *cspacec::csSpline_RemovePoint;
*SetTimeValues = *cspacec::csSpline_SetTimeValues;
*SetTimeValue = *cspacec::csSpline_SetTimeValue;
*GetTimeValues = *cspacec::csSpline_GetTimeValues;
*GetTimeValue = *cspacec::csSpline_GetTimeValue;
*SetDimensionValues = *cspacec::csSpline_SetDimensionValues;
*SetDimensionValue = *cspacec::csSpline_SetDimensionValue;
*GetDimensionValues = *cspacec::csSpline_GetDimensionValues;
*GetDimensionValue = *cspacec::csSpline_GetDimensionValue;
*SetIndexValues = *cspacec::csSpline_SetIndexValues;
*GetIndexValues = *cspacec::csSpline_GetIndexValues;
*Calculate = *cspacec::csSpline_Calculate;
*GetCurrentIndex = *cspacec::csSpline_GetCurrentIndex;
*GetInterpolatedDimension = *cspacec::csSpline_GetInterpolatedDimension;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csCubicSpline ##############

package cspace::csCubicSpline;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csSpline cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csCubicSpline(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csCubicSpline($self);
        delete $OWNER{$self};
    }
}

*Calculate = *cspacec::csCubicSpline_Calculate;
*GetInterpolatedDimension = *cspacec::csCubicSpline_GetInterpolatedDimension;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csBSpline ##############

package cspace::csBSpline;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csSpline cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csBSpline(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csBSpline($self);
        delete $OWNER{$self};
    }
}

*Calculate = *cspacec::csBSpline_Calculate;
*GetInterpolatedDimension = *cspacec::csBSpline_GetInterpolatedDimension;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csCatmullRomSpline ##############

package cspace::csCatmullRomSpline;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csBSpline cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csCatmullRomSpline(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csCatmullRomSpline($self);
        delete $OWNER{$self};
    }
}

*Clone = *cspacec::csCatmullRomSpline_Clone;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csBox2 ##############

package cspace::csBox2;
use overload
    "*" => sub { $_[0]->__mul__($_[1])},
    "!=" => sub { $_[0]->__ne__($_[1])},
    "<" => sub { $_[0]->__lt__($_[1])},
    ">" => sub { $_[0]->__gt__($_[1])},
    "+" => sub { $_[0]->__add__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*MinX = *cspacec::csBox2_MinX;
*MinY = *cspacec::csBox2_MinY;
*MaxX = *cspacec::csBox2_MaxX;
*MaxY = *cspacec::csBox2_MaxY;
*Min = *cspacec::csBox2_Min;
*Max = *cspacec::csBox2_Max;
*Area = *cspacec::csBox2_Area;
*GetCorner = *cspacec::csBox2_GetCorner;
*GetCenter = *cspacec::csBox2_GetCenter;
*SetCenter = *cspacec::csBox2_SetCenter;
*SetSize = *cspacec::csBox2_SetSize;
*GetEdgeInfo = *cspacec::csBox2_GetEdgeInfo;
*GetEdge = *cspacec::csBox2_GetEdge;
*Intersect = *cspacec::csBox2_Intersect;
*In = *cspacec::csBox2_In;
*Overlap = *cspacec::csBox2_Overlap;
*Contains = *cspacec::csBox2_Contains;
*Empty = *cspacec::csBox2_Empty;
*SquaredOriginDist = *cspacec::csBox2_SquaredOriginDist;
*SquaredOriginMaxDist = *cspacec::csBox2_SquaredOriginMaxDist;
*SquaredPosDist = *cspacec::csBox2_SquaredPosDist;
*SquaredPosMaxDist = *cspacec::csBox2_SquaredPosMaxDist;
*StartBoundingBox = *cspacec::csBox2_StartBoundingBox;
*AddBoundingVertex = *cspacec::csBox2_AddBoundingVertex;
*AddBoundingVertexSmart = *cspacec::csBox2_AddBoundingVertexSmart;
*AddBoundingVertexTest = *cspacec::csBox2_AddBoundingVertexTest;
*AddBoundingVertexSmartTest = *cspacec::csBox2_AddBoundingVertexSmartTest;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csBox2(@_);
    bless $self, $pkg if defined($self);
}

*Set = *cspacec::csBox2_Set;
*SetMin = *cspacec::csBox2_SetMin;
*SetMax = *cspacec::csBox2_SetMax;
*Description = *cspacec::csBox2_Description;
*__add_ass__ = *cspacec::csBox2___add_ass__;
*__mult_ass__ = *cspacec::csBox2___mult_ass__;
*TestIntersect = *cspacec::csBox2_TestIntersect;
*__mul__ = *cspacec::csBox2___mul__;
*__ne__ = *cspacec::csBox2___ne__;
*__gt__ = *cspacec::csBox2___gt__;
*__add__ = *cspacec::csBox2___add__;
*__lt__ = *cspacec::csBox2___lt__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csBox2($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csBox3 ##############

package cspace::csBox3;
use overload
    "*" => sub { $_[0]->__mul__($_[1])},
    "!=" => sub { $_[0]->__ne__($_[1])},
    "<" => sub { $_[0]->__lt__($_[1])},
    ">" => sub { $_[0]->__gt__($_[1])},
    "+" => sub { $_[0]->__add__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*MinX = *cspacec::csBox3_MinX;
*MinY = *cspacec::csBox3_MinY;
*MinZ = *cspacec::csBox3_MinZ;
*MaxX = *cspacec::csBox3_MaxX;
*MaxY = *cspacec::csBox3_MaxY;
*MaxZ = *cspacec::csBox3_MaxZ;
*Min = *cspacec::csBox3_Min;
*Max = *cspacec::csBox3_Max;
*Volume = *cspacec::csBox3_Volume;
*Area = *cspacec::csBox3_Area;
*GetCorner = *cspacec::csBox3_GetCorner;
*GetEdgeInfo = *cspacec::csBox3_GetEdgeInfo;
*GetFaceEdges = *cspacec::csBox3_GetFaceEdges;
*GetCenter = *cspacec::csBox3_GetCenter;
*SetCenter = *cspacec::csBox3_SetCenter;
*SetSize = *cspacec::csBox3_SetSize;
*GetSize = *cspacec::csBox3_GetSize;
*GetSide = *cspacec::csBox3_GetSide;
*GetAxisPlane = *cspacec::csBox3_GetAxisPlane;
*GetVisibleSides = *cspacec::csBox3_GetVisibleSides;
*OtherSide = *cspacec::csBox3_OtherSide;
*GetEdge = *cspacec::csBox3_GetEdge;
*In = *cspacec::csBox3_In;
*Overlap = *cspacec::csBox3_Overlap;
*Contains = *cspacec::csBox3_Contains;
*Empty = *cspacec::csBox3_Empty;
*StartBoundingBox = *cspacec::csBox3_StartBoundingBox;
*AddBoundingVertex = *cspacec::csBox3_AddBoundingVertex;
*AddBoundingVertexSmart = *cspacec::csBox3_AddBoundingVertexSmart;
*AddBoundingVertexTest = *cspacec::csBox3_AddBoundingVertexTest;
*AddBoundingVertexSmartTest = *cspacec::csBox3_AddBoundingVertexSmartTest;
*AddBoundingBox = *cspacec::csBox3_AddBoundingBox;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csBox3(@_);
    bless $self, $pkg if defined($self);
}

*Set = *cspacec::csBox3_Set;
*SetMin = *cspacec::csBox3_SetMin;
*SetMax = *cspacec::csBox3_SetMax;
*GetMin = *cspacec::csBox3_GetMin;
*GetMax = *cspacec::csBox3_GetMax;
*Description = *cspacec::csBox3_Description;
*Split = *cspacec::csBox3_Split;
*TestSplit = *cspacec::csBox3_TestSplit;
*AdjacentX = *cspacec::csBox3_AdjacentX;
*AdjacentY = *cspacec::csBox3_AdjacentY;
*AdjacentZ = *cspacec::csBox3_AdjacentZ;
*Adjacent = *cspacec::csBox3_Adjacent;
*CalculatePointSegment = *cspacec::csBox3_CalculatePointSegment;
*GetConvexOutline = *cspacec::csBox3_GetConvexOutline;
*Between = *cspacec::csBox3_Between;
*ManhattanDistance = *cspacec::csBox3_ManhattanDistance;
*SquaredOriginDist = *cspacec::csBox3_SquaredOriginDist;
*SquaredOriginMaxDist = *cspacec::csBox3_SquaredOriginMaxDist;
*SquaredPosDist = *cspacec::csBox3_SquaredPosDist;
*SquaredPosMaxDist = *cspacec::csBox3_SquaredPosMaxDist;
*ProjectBox = *cspacec::csBox3_ProjectBox;
*ProjectOutline = *cspacec::csBox3_ProjectOutline;
*ProjectBoxAndOutline = *cspacec::csBox3_ProjectBoxAndOutline;
*__add_ass__ = *cspacec::csBox3___add_ass__;
*__mult_ass__ = *cspacec::csBox3___mult_ass__;
*TestIntersect = *cspacec::csBox3_TestIntersect;
*__mul__ = *cspacec::csBox3___mul__;
*__ne__ = *cspacec::csBox3___ne__;
*__gt__ = *cspacec::csBox3___gt__;
*__add__ = *cspacec::csBox3___add__;
*__lt__ = *cspacec::csBox3___lt__;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csBox3($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSegment2 ##############

package cspace::csSegment2;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csSegment2(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSegment2($self);
        delete $OWNER{$self};
    }
}

*Set = *cspacec::csSegment2_Set;
*SetStart = *cspacec::csSegment2_SetStart;
*SetEnd = *cspacec::csSegment2_SetEnd;
*Start = *cspacec::csSegment2_Start;
*End = *cspacec::csSegment2_End;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSegment3 ##############

package cspace::csSegment3;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csSegment3(@_);
    bless $self, $pkg if defined($self);
}

*Set = *cspacec::csSegment3_Set;
*SetStart = *cspacec::csSegment3_SetStart;
*SetEnd = *cspacec::csSegment3_SetEnd;
*Start = *cspacec::csSegment3_Start;
*End = *cspacec::csSegment3_End;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSegment3($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csRGBcolor ##############

package cspace::csRGBcolor;
use overload
    "!=" => sub { $_[0]->__ne__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "+" => sub { $_[0]->__add__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_red_get = *cspacec::csRGBcolor_red_get;
*swig_red_set = *cspacec::csRGBcolor_red_set;
*swig_green_get = *cspacec::csRGBcolor_green_get;
*swig_green_set = *cspacec::csRGBcolor_green_set;
*swig_blue_get = *cspacec::csRGBcolor_blue_get;
*swig_blue_set = *cspacec::csRGBcolor_blue_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csRGBcolor(@_);
    bless $self, $pkg if defined($self);
}

*Set = *cspacec::csRGBcolor_Set;
*__eq__ = *cspacec::csRGBcolor___eq__;
*__ne__ = *cspacec::csRGBcolor___ne__;
*__add__ = *cspacec::csRGBcolor___add__;
*UnsafeAdd = *cspacec::csRGBcolor_UnsafeAdd;
*SafeAdd = *cspacec::csRGBcolor_SafeAdd;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csRGBcolor($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csRGBpixel ##############

package cspace::csRGBpixel;
use overload
    "!=" => sub { $_[0]->__ne__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_red_get = *cspacec::csRGBpixel_red_get;
*swig_red_set = *cspacec::csRGBpixel_red_set;
*swig_green_get = *cspacec::csRGBpixel_green_get;
*swig_green_set = *cspacec::csRGBpixel_green_set;
*swig_blue_get = *cspacec::csRGBpixel_blue_get;
*swig_blue_set = *cspacec::csRGBpixel_blue_set;
*swig_alpha_get = *cspacec::csRGBpixel_alpha_get;
*swig_alpha_set = *cspacec::csRGBpixel_alpha_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csRGBpixel(@_);
    bless $self, $pkg if defined($self);
}

*__eq__ = *cspacec::csRGBpixel___eq__;
*__ne__ = *cspacec::csRGBpixel___ne__;
*asRGBcolor = *cspacec::csRGBpixel_asRGBcolor;
*eq = *cspacec::csRGBpixel_eq;
*Intensity = *cspacec::csRGBpixel_Intensity;
*Luminance = *cspacec::csRGBpixel_Luminance;
*Set = *cspacec::csRGBpixel_Set;
*__add_ass__ = *cspacec::csRGBpixel___add_ass__;
*UnsafeAdd = *cspacec::csRGBpixel_UnsafeAdd;
*SafeAdd = *cspacec::csRGBpixel_SafeAdd;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csRGBpixel($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iShaderVariableAccessor ##############

package cspace::iShaderVariableAccessor;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*PreGetValue = *cspacec::iShaderVariableAccessor_PreGetValue;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iShaderVariableAccessor($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csShaderVariable ##############

package cspace::csShaderVariable;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csRefCount cspace );
%OWNER = ();
%ITERATORS = ();
*UNKNOWN = *cspacec::csShaderVariable_UNKNOWN;
*INT = *cspacec::csShaderVariable_INT;
*FLOAT = *cspacec::csShaderVariable_FLOAT;
*TEXTURE = *cspacec::csShaderVariable_TEXTURE;
*RENDERBUFFER = *cspacec::csShaderVariable_RENDERBUFFER;
*VECTOR2 = *cspacec::csShaderVariable_VECTOR2;
*VECTOR3 = *cspacec::csShaderVariable_VECTOR3;
*VECTOR4 = *cspacec::csShaderVariable_VECTOR4;
*MATRIX = *cspacec::csShaderVariable_MATRIX;
*TRANSFORM = *cspacec::csShaderVariable_TRANSFORM;
*ARRAY = *cspacec::csShaderVariable_ARRAY;
*COLOR = *cspacec::csShaderVariable_COLOR;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csShaderVariable(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csShaderVariable($self);
        delete $OWNER{$self};
    }
}

*__copy__ = *cspacec::csShaderVariable___copy__;
*GetType = *cspacec::csShaderVariable_GetType;
*SetType = *cspacec::csShaderVariable_SetType;
*SetAccessor = *cspacec::csShaderVariable_SetAccessor;
*SetName = *cspacec::csShaderVariable_SetName;
*GetName = *cspacec::csShaderVariable_GetName;
*GetValue = *cspacec::csShaderVariable_GetValue;
*SetValue = *cspacec::csShaderVariable_SetValue;
*AddVariableToArray = *cspacec::csShaderVariable_AddVariableToArray;
*RemoveFromArray = *cspacec::csShaderVariable_RemoveFromArray;
*SetArraySize = *cspacec::csShaderVariable_SetArraySize;
*GetArraySize = *cspacec::csShaderVariable_GetArraySize;
*GetArrayElement = *cspacec::csShaderVariable_GetArrayElement;
*SetArrayElement = *cspacec::csShaderVariable_SetArrayElement;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csShaderVariableArrayReadOnly ##############

package cspace::csShaderVariableArrayReadOnly;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetSize = *cspacec::csShaderVariableArrayReadOnly_GetSize;
*Get = *cspacec::csShaderVariableArrayReadOnly_Get;
*Top = *cspacec::csShaderVariableArrayReadOnly_Top;
*Find = *cspacec::csShaderVariableArrayReadOnly_Find;
*GetIndex = *cspacec::csShaderVariableArrayReadOnly_GetIndex;
*IsEmpty = *cspacec::csShaderVariableArrayReadOnly_IsEmpty;
*GetAll = *cspacec::csShaderVariableArrayReadOnly_GetAll;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csShaderVariableArrayReadOnly($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csShaderVariableArrayChangeElements ##############

package cspace::csShaderVariableArrayChangeElements;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csShaderVariableArrayReadOnly cspace );
%OWNER = ();
%ITERATORS = ();
*Get = *cspacec::csShaderVariableArrayChangeElements_Get;
*Top = *cspacec::csShaderVariableArrayChangeElements_Top;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csShaderVariableArrayChangeElements($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csShaderVariableArray ##############

package cspace::csShaderVariableArray;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csShaderVariableArrayChangeElements cspace );
%OWNER = ();
%ITERATORS = ();
*SetSize = *cspacec::csShaderVariableArray_SetSize;
*GetExtend = *cspacec::csShaderVariableArray_GetExtend;
*Put = *cspacec::csShaderVariableArray_Put;
*Push = *cspacec::csShaderVariableArray_Push;
*PushSmart = *cspacec::csShaderVariableArray_PushSmart;
*Pop = *cspacec::csShaderVariableArray_Pop;
*Insert = *cspacec::csShaderVariableArray_Insert;
*DeleteAll = *cspacec::csShaderVariableArray_DeleteAll;
*Truncate = *cspacec::csShaderVariableArray_Truncate;
*Empty = *cspacec::csShaderVariableArray_Empty;
*DeleteIndex = *cspacec::csShaderVariableArray_DeleteIndex;
*DeleteIndexFast = *cspacec::csShaderVariableArray_DeleteIndexFast;
*Delete = *cspacec::csShaderVariableArray_Delete;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csShaderVariableArray($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPluginRequest ##############

package cspace::csPluginRequest;
use overload
    "!=" => sub { $_[0]->__ne__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPluginRequest(@_);
    bless $self, $pkg if defined($self);
}

*__copy__ = *cspacec::csPluginRequest___copy__;
*__eq__ = *cspacec::csPluginRequest___eq__;
*__ne__ = *cspacec::csPluginRequest___ne__;
*GetClassName = *cspacec::csPluginRequest_GetClassName;
*GetInterfaceName = *cspacec::csPluginRequest_GetInterfaceName;
*GetInterfaceID = *cspacec::csPluginRequest_GetInterfaceID;
*GetInterfaceVersion = *cspacec::csPluginRequest_GetInterfaceVersion;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPluginRequest($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csInitializer ##############

package cspace::csInitializer;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*CreateEnvironment = *cspacec::csInitializer_CreateEnvironment;
*InitializeSCF = *cspacec::csInitializer_InitializeSCF;
*CreateObjectRegistry = *cspacec::csInitializer_CreateObjectRegistry;
*CreatePluginManager = *cspacec::csInitializer_CreatePluginManager;
*CreateEventQueue = *cspacec::csInitializer_CreateEventQueue;
*CreateVirtualClock = *cspacec::csInitializer_CreateVirtualClock;
*CreateCommandLineParser = *cspacec::csInitializer_CreateCommandLineParser;
*CreateVerbosityManager = *cspacec::csInitializer_CreateVerbosityManager;
*CreateConfigManager = *cspacec::csInitializer_CreateConfigManager;
*CreateInputDrivers = *cspacec::csInitializer_CreateInputDrivers;
*CreateStringSet = *cspacec::csInitializer_CreateStringSet;
*SetupConfigManager = *cspacec::csInitializer_SetupConfigManager;
*SetupVFS = *cspacec::csInitializer_SetupVFS;
*_RequestPlugins = *cspacec::csInitializer__RequestPlugins;
*OpenApplication = *cspacec::csInitializer_OpenApplication;
*CloseApplication = *cspacec::csInitializer_CloseApplication;
*_SetupEventHandler = *cspacec::csInitializer__SetupEventHandler;
*DestroyApplication = *cspacec::csInitializer_DestroyApplication;
*GetDefaultAppID = *cspacec::csInitializer_GetDefaultAppID;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csInitializer(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csInitializer($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPluginRequestArray ##############

package cspace::csPluginRequestArray;
use overload
    "!=" => sub { $_[0]->__ne__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPluginRequestArray($self);
        delete $OWNER{$self};
    }
}

sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPluginRequestArray(@_);
    bless $self, $pkg if defined($self);
}

*GetSize = *cspacec::csPluginRequestArray_GetSize;
*Get = *cspacec::csPluginRequestArray_Get;
*Push = *cspacec::csPluginRequestArray_Push;
*Pop = *cspacec::csPluginRequestArray_Pop;
*Top = *cspacec::csPluginRequestArray_Top;
*Insert = *cspacec::csPluginRequestArray_Insert;
*Contains = *cspacec::csPluginRequestArray_Contains;
*Truncate = *cspacec::csPluginRequestArray_Truncate;
*Empty = *cspacec::csPluginRequestArray_Empty;
*IsEmpty = *cspacec::csPluginRequestArray_IsEmpty;
*SetMinimalCapacity = *cspacec::csPluginRequestArray_SetMinimalCapacity;
*DeleteIndex = *cspacec::csPluginRequestArray_DeleteIndex;
*DeleteIndexFast = *cspacec::csPluginRequestArray_DeleteIndexFast;
*DeleteRange = *cspacec::csPluginRequestArray_DeleteRange;
*__eq__ = *cspacec::csPluginRequestArray___eq__;
*__ne__ = *cspacec::csPluginRequestArray___ne__;
*GetAllocator = *cspacec::csPluginRequestArray_GetAllocator;
*Length = *cspacec::csPluginRequestArray_Length;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csVertexStatus ##############

package cspace::csVertexStatus;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_Type_get = *cspacec::csVertexStatus_Type_get;
*swig_Type_set = *cspacec::csVertexStatus_Type_set;
*swig_Vertex_get = *cspacec::csVertexStatus_Vertex_get;
*swig_Vertex_set = *cspacec::csVertexStatus_Vertex_set;
*swig_Pos_get = *cspacec::csVertexStatus_Pos_get;
*swig_Pos_set = *cspacec::csVertexStatus_Pos_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csVertexStatus(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csVertexStatus($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iClipper2D ##############

package cspace::iClipper2D;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Clip = *cspacec::iClipper2D_Clip;
*ClipInPlace = *cspacec::iClipper2D_ClipInPlace;
*ClassifyBox = *cspacec::iClipper2D_ClassifyBox;
*IsInside = *cspacec::iClipper2D_IsInside;
*GetVertexCount = *cspacec::iClipper2D_GetVertexCount;
*GetClipPoly = *cspacec::iClipper2D_GetClipPoly;
*clipperPoly = *cspacec::iClipper2D_clipperPoly;
*clipperBox = *cspacec::iClipper2D_clipperBox;
*GetClipperType = *cspacec::iClipper2D_GetClipperType;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iClipper2D($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDecal ##############

package cspace::iDecal;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_iDecal(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDecal($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDecalTemplate ##############

package cspace::iDecalTemplate;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetTimeToLive = *cspacec::iDecalTemplate_GetTimeToLive;
*GetMaterialWrapper = *cspacec::iDecalTemplate_GetMaterialWrapper;
*GetRenderPriority = *cspacec::iDecalTemplate_GetRenderPriority;
*GetZBufMode = *cspacec::iDecalTemplate_GetZBufMode;
*GetPolygonNormalThreshold = *cspacec::iDecalTemplate_GetPolygonNormalThreshold;
*GetDecalOffset = *cspacec::iDecalTemplate_GetDecalOffset;
*HasTopClipping = *cspacec::iDecalTemplate_HasTopClipping;
*GetTopClippingScale = *cspacec::iDecalTemplate_GetTopClippingScale;
*HasBottomClipping = *cspacec::iDecalTemplate_HasBottomClipping;
*GetBottomClippingScale = *cspacec::iDecalTemplate_GetBottomClippingScale;
*GetMinTexCoord = *cspacec::iDecalTemplate_GetMinTexCoord;
*GetMainColor = *cspacec::iDecalTemplate_GetMainColor;
*GetTopColor = *cspacec::iDecalTemplate_GetTopColor;
*GetBottomColor = *cspacec::iDecalTemplate_GetBottomColor;
*GetMaxTexCoord = *cspacec::iDecalTemplate_GetMaxTexCoord;
*GetMixMode = *cspacec::iDecalTemplate_GetMixMode;
*GetPerpendicularFaceThreshold = *cspacec::iDecalTemplate_GetPerpendicularFaceThreshold;
*GetPerpendicularFaceOffset = *cspacec::iDecalTemplate_GetPerpendicularFaceOffset;
*SetTimeToLive = *cspacec::iDecalTemplate_SetTimeToLive;
*SetMaterialWrapper = *cspacec::iDecalTemplate_SetMaterialWrapper;
*SetRenderPriority = *cspacec::iDecalTemplate_SetRenderPriority;
*SetZBufMode = *cspacec::iDecalTemplate_SetZBufMode;
*SetPolygonNormalThreshold = *cspacec::iDecalTemplate_SetPolygonNormalThreshold;
*SetDecalOffset = *cspacec::iDecalTemplate_SetDecalOffset;
*SetTopClipping = *cspacec::iDecalTemplate_SetTopClipping;
*SetBottomClipping = *cspacec::iDecalTemplate_SetBottomClipping;
*SetTexCoords = *cspacec::iDecalTemplate_SetTexCoords;
*SetMixMode = *cspacec::iDecalTemplate_SetMixMode;
*SetPerpendicularFaceThreshold = *cspacec::iDecalTemplate_SetPerpendicularFaceThreshold;
*SetPerpendicularFaceOffset = *cspacec::iDecalTemplate_SetPerpendicularFaceOffset;
*SetMainColor = *cspacec::iDecalTemplate_SetMainColor;
*SetTopColor = *cspacec::iDecalTemplate_SetTopColor;
*SetBottomColor = *cspacec::iDecalTemplate_SetBottomColor;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDecalTemplate($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iDecalTemplate_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDecalBuilder ##############

package cspace::iDecalBuilder;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDecalBuilder($self);
        delete $OWNER{$self};
    }
}

*AddStaticPoly = *cspacec::iDecalBuilder_AddStaticPoly;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDecalManager ##############

package cspace::iDecalManager;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*CreateDecal = *cspacec::iDecalManager_CreateDecal;
*CreateDecalTemplate = *cspacec::iDecalManager_CreateDecalTemplate;
*DeleteDecal = *cspacec::iDecalManager_DeleteDecal;
*GetDecalCount = *cspacec::iDecalManager_GetDecalCount;
*GetDecal = *cspacec::iDecalManager_GetDecal;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDecalManager($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iDecalManager_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iObjectModelListener ##############

package cspace::iObjectModelListener;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*ObjectModelChanged = *cspacec::iObjectModelListener_ObjectModelChanged;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iObjectModelListener($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iObjectModelListener_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iObjectModel ##############

package cspace::iObjectModel;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetShapeNumber = *cspacec::iObjectModel_GetShapeNumber;
*GetTriangleData = *cspacec::iObjectModel_GetTriangleData;
*SetTriangleData = *cspacec::iObjectModel_SetTriangleData;
*IsTriangleDataSet = *cspacec::iObjectModel_IsTriangleDataSet;
*ResetTriangleData = *cspacec::iObjectModel_ResetTriangleData;
*GetPolygonMeshBase = *cspacec::iObjectModel_GetPolygonMeshBase;
*GetPolygonMeshColldet = *cspacec::iObjectModel_GetPolygonMeshColldet;
*GetTerraFormerColldet = *cspacec::iObjectModel_GetTerraFormerColldet;
*GetTerrainColldet = *cspacec::iObjectModel_GetTerrainColldet;
*SetPolygonMeshColldet = *cspacec::iObjectModel_SetPolygonMeshColldet;
*GetPolygonMeshViscull = *cspacec::iObjectModel_GetPolygonMeshViscull;
*SetPolygonMeshViscull = *cspacec::iObjectModel_SetPolygonMeshViscull;
*GetPolygonMeshShadows = *cspacec::iObjectModel_GetPolygonMeshShadows;
*SetPolygonMeshShadows = *cspacec::iObjectModel_SetPolygonMeshShadows;
*CreateLowerDetailPolygonMesh = *cspacec::iObjectModel_CreateLowerDetailPolygonMesh;
*GetObjectBoundingBox = *cspacec::iObjectModel_GetObjectBoundingBox;
*SetObjectBoundingBox = *cspacec::iObjectModel_SetObjectBoundingBox;
*GetRadius = *cspacec::iObjectModel_GetRadius;
*AddListener = *cspacec::iObjectModel_AddListener;
*RemoveListener = *cspacec::iObjectModel_RemoveListener;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iObjectModel($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iObjectModel_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iPath ##############

package cspace::iPath;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Length = *cspacec::iPath_Length;
*CalculateAtTime = *cspacec::iPath_CalculateAtTime;
*GetCurrentIndex = *cspacec::iPath_GetCurrentIndex;
*SetPositionVectors = *cspacec::iPath_SetPositionVectors;
*SetUpVectors = *cspacec::iPath_SetUpVectors;
*SetForwardVectors = *cspacec::iPath_SetForwardVectors;
*SetPositionVector = *cspacec::iPath_SetPositionVector;
*SetUpVector = *cspacec::iPath_SetUpVector;
*SetForwardVector = *cspacec::iPath_SetForwardVector;
*GetPositionVector = *cspacec::iPath_GetPositionVector;
*GetUpVector = *cspacec::iPath_GetUpVector;
*GetForwardVector = *cspacec::iPath_GetForwardVector;
*GetTime = *cspacec::iPath_GetTime;
*SetTime = *cspacec::iPath_SetTime;
*GetInterpolatedPosition = *cspacec::iPath_GetInterpolatedPosition;
*GetInterpolatedUp = *cspacec::iPath_GetInterpolatedUp;
*GetInterpolatedForward = *cspacec::iPath_GetInterpolatedForward;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iPath($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iPath_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::scfPath ##############

package cspace::scfPath;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iPath cspace );
%OWNER = ();
*IncRef = *cspacec::scfPath_IncRef;
*DecRef = *cspacec::scfPath_DecRef;
*GetRefCount = *cspacec::scfPath_GetRefCount;
*QueryInterface = *cspacec::scfPath_QueryInterface;
*AddRefOwner = *cspacec::scfPath_AddRefOwner;
*RemoveRefOwner = *cspacec::scfPath_RemoveRefOwner;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csMeshedPolygon ##############

package cspace::csMeshedPolygon;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_num_vertices_get = *cspacec::csMeshedPolygon_num_vertices_get;
*swig_num_vertices_set = *cspacec::csMeshedPolygon_num_vertices_set;
*swig_vertices_get = *cspacec::csMeshedPolygon_vertices_get;
*swig_vertices_set = *cspacec::csMeshedPolygon_vertices_set;
*GetVertexByIndex = *cspacec::csMeshedPolygon_GetVertexByIndex;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csMeshedPolygon(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csMeshedPolygon($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iPolygonMesh ##############

package cspace::iPolygonMesh;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetVertexCount = *cspacec::iPolygonMesh_GetVertexCount;
*GetPolygonCount = *cspacec::iPolygonMesh_GetPolygonCount;
*GetTriangleCount = *cspacec::iPolygonMesh_GetTriangleCount;
*Lock = *cspacec::iPolygonMesh_Lock;
*Unlock = *cspacec::iPolygonMesh_Unlock;
*GetFlags = *cspacec::iPolygonMesh_GetFlags;
*GetChangeNumber = *cspacec::iPolygonMesh_GetChangeNumber;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iPolygonMesh($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iPolygonMesh_scfGetVersion;
*GetVertexByIndex = *cspacec::iPolygonMesh_GetVertexByIndex;
*GetPolygonByIndex = *cspacec::iPolygonMesh_GetPolygonByIndex;
*GetTriangleByIndex = *cspacec::iPolygonMesh_GetTriangleByIndex;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPath ##############

package cspace::csPath;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::scfPath cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPath(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPath($self);
        delete $OWNER{$self};
    }
}

*Length = *cspacec::csPath_Length;
*CalculateAtTime = *cspacec::csPath_CalculateAtTime;
*GetCurrentIndex = *cspacec::csPath_GetCurrentIndex;
*GetTime = *cspacec::csPath_GetTime;
*SetTime = *cspacec::csPath_SetTime;
*SetTimes = *cspacec::csPath_SetTimes;
*GetTimes = *cspacec::csPath_GetTimes;
*SetPositionVectors = *cspacec::csPath_SetPositionVectors;
*SetUpVectors = *cspacec::csPath_SetUpVectors;
*SetForwardVectors = *cspacec::csPath_SetForwardVectors;
*SetPositionVector = *cspacec::csPath_SetPositionVector;
*SetUpVector = *cspacec::csPath_SetUpVector;
*SetForwardVector = *cspacec::csPath_SetForwardVector;
*GetPositionVector = *cspacec::csPath_GetPositionVector;
*GetUpVector = *cspacec::csPath_GetUpVector;
*GetForwardVector = *cspacec::csPath_GetForwardVector;
*GetInterpolatedPosition = *cspacec::csPath_GetInterpolatedPosition;
*GetInterpolatedUp = *cspacec::csPath_GetInterpolatedUp;
*GetInterpolatedForward = *cspacec::csPath_GetInterpolatedForward;
*GetDimensionValues = *cspacec::csPath_GetDimensionValues;
*GetDimensionValue = *cspacec::csPath_GetDimensionValue;
*InsertPoint = *cspacec::csPath_InsertPoint;
*RemovePoint = *cspacec::csPath_RemovePoint;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::pycsPolygonMesh ##############

package cspace::pycsPolygonMesh;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iPolygonMesh cspace );
%OWNER = ();
*IncRef = *cspacec::pycsPolygonMesh_IncRef;
*DecRef = *cspacec::pycsPolygonMesh_DecRef;
*GetRefCount = *cspacec::pycsPolygonMesh_GetRefCount;
*QueryInterface = *cspacec::pycsPolygonMesh_QueryInterface;
*AddRefOwner = *cspacec::pycsPolygonMesh_AddRefOwner;
*RemoveRefOwner = *cspacec::pycsPolygonMesh_RemoveRefOwner;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::pycsPolygonMeshBox ##############

package cspace::pycsPolygonMeshBox;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iPolygonMesh cspace );
%OWNER = ();
*IncRef = *cspacec::pycsPolygonMeshBox_IncRef;
*DecRef = *cspacec::pycsPolygonMeshBox_DecRef;
*GetRefCount = *cspacec::pycsPolygonMeshBox_GetRefCount;
*QueryInterface = *cspacec::pycsPolygonMeshBox_QueryInterface;
*AddRefOwner = *cspacec::pycsPolygonMeshBox_AddRefOwner;
*RemoveRefOwner = *cspacec::pycsPolygonMeshBox_RemoveRefOwner;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPolygonMesh ##############

package cspace::csPolygonMesh;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::pycsPolygonMesh cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPolygonMesh(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPolygonMesh($self);
        delete $OWNER{$self};
    }
}

*SetVertices = *cspacec::csPolygonMesh_SetVertices;
*SetPolygons = *cspacec::csPolygonMesh_SetPolygons;
*SetPolygonIndices = *cspacec::csPolygonMesh_SetPolygonIndices;
*SetPolygonIndexCount = *cspacec::csPolygonMesh_SetPolygonIndexCount;
*GetPolygonIndices = *cspacec::csPolygonMesh_GetPolygonIndices;
*SetVertexCount = *cspacec::csPolygonMesh_SetVertexCount;
*SetPolygonCount = *cspacec::csPolygonMesh_SetPolygonCount;
*ShapeChanged = *cspacec::csPolygonMesh_ShapeChanged;
*GetVertexCount = *cspacec::csPolygonMesh_GetVertexCount;
*GetPolygonCount = *cspacec::csPolygonMesh_GetPolygonCount;
*GetTriangleCount = *cspacec::csPolygonMesh_GetTriangleCount;
*Lock = *cspacec::csPolygonMesh_Lock;
*Unlock = *cspacec::csPolygonMesh_Unlock;
*GetFlags = *cspacec::csPolygonMesh_GetFlags;
*GetChangeNumber = *cspacec::csPolygonMesh_GetChangeNumber;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPolygonMeshBox ##############

package cspace::csPolygonMeshBox;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::pycsPolygonMeshBox cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPolygonMeshBox(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPolygonMeshBox($self);
        delete $OWNER{$self};
    }
}

*SetBox = *cspacec::csPolygonMeshBox_SetBox;
*GetVertexCount = *cspacec::csPolygonMeshBox_GetVertexCount;
*GetPolygonCount = *cspacec::csPolygonMeshBox_GetPolygonCount;
*GetTriangleCount = *cspacec::csPolygonMeshBox_GetTriangleCount;
*Lock = *cspacec::csPolygonMeshBox_Lock;
*Unlock = *cspacec::csPolygonMeshBox_Unlock;
*GetFlags = *cspacec::csPolygonMeshBox_GetFlags;
*GetChangeNumber = *cspacec::csPolygonMeshBox_GetChangeNumber;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csIntArray ##############

package cspace::csIntArray;
use overload
    "!=" => sub { $_[0]->__ne__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csIntArray($self);
        delete $OWNER{$self};
    }
}

sub new {
    my $pkg = shift;
    my $self = cspacec::new_csIntArray(@_);
    bless $self, $pkg if defined($self);
}

*GetSize = *cspacec::csIntArray_GetSize;
*Get = *cspacec::csIntArray_Get;
*Put = *cspacec::csIntArray_Put;
*Push = *cspacec::csIntArray_Push;
*Pop = *cspacec::csIntArray_Pop;
*Top = *cspacec::csIntArray_Top;
*Insert = *cspacec::csIntArray_Insert;
*Contains = *cspacec::csIntArray_Contains;
*Truncate = *cspacec::csIntArray_Truncate;
*Empty = *cspacec::csIntArray_Empty;
*IsEmpty = *cspacec::csIntArray_IsEmpty;
*SetMinimalCapacity = *cspacec::csIntArray_SetMinimalCapacity;
*DeleteIndex = *cspacec::csIntArray_DeleteIndex;
*DeleteIndexFast = *cspacec::csIntArray_DeleteIndexFast;
*DeleteRange = *cspacec::csIntArray_DeleteRange;
*__eq__ = *cspacec::csIntArray___eq__;
*__ne__ = *cspacec::csIntArray___ne__;
*GetAllocator = *cspacec::csIntArray_GetAllocator;
*Length = *cspacec::csIntArray_Length;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csIntArrayArray ##############

package cspace::csIntArrayArray;
use overload
    "!=" => sub { $_[0]->__ne__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csIntArrayArray($self);
        delete $OWNER{$self};
    }
}

sub new {
    my $pkg = shift;
    my $self = cspacec::new_csIntArrayArray(@_);
    bless $self, $pkg if defined($self);
}

*GetSize = *cspacec::csIntArrayArray_GetSize;
*Get = *cspacec::csIntArrayArray_Get;
*Put = *cspacec::csIntArrayArray_Put;
*Push = *cspacec::csIntArrayArray_Push;
*Pop = *cspacec::csIntArrayArray_Pop;
*Top = *cspacec::csIntArrayArray_Top;
*Insert = *cspacec::csIntArrayArray_Insert;
*Truncate = *cspacec::csIntArrayArray_Truncate;
*Empty = *cspacec::csIntArrayArray_Empty;
*IsEmpty = *cspacec::csIntArrayArray_IsEmpty;
*SetMinimalCapacity = *cspacec::csIntArrayArray_SetMinimalCapacity;
*DeleteIndex = *cspacec::csIntArrayArray_DeleteIndex;
*DeleteIndexFast = *cspacec::csIntArrayArray_DeleteIndexFast;
*DeleteRange = *cspacec::csIntArrayArray_DeleteRange;
*__eq__ = *cspacec::csIntArrayArray___eq__;
*__ne__ = *cspacec::csIntArrayArray___ne__;
*GetAllocator = *cspacec::csIntArrayArray_GetAllocator;
*Length = *cspacec::csIntArrayArray_Length;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPolygonMeshEdge ##############

package cspace::csPolygonMeshEdge;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_vt1_get = *cspacec::csPolygonMeshEdge_vt1_get;
*swig_vt1_set = *cspacec::csPolygonMeshEdge_vt1_set;
*swig_vt2_get = *cspacec::csPolygonMeshEdge_vt2_get;
*swig_vt2_set = *cspacec::csPolygonMeshEdge_vt2_set;
*swig_poly1_get = *cspacec::csPolygonMeshEdge_poly1_get;
*swig_poly1_set = *cspacec::csPolygonMeshEdge_poly1_set;
*swig_poly2_get = *cspacec::csPolygonMeshEdge_poly2_get;
*swig_poly2_set = *cspacec::csPolygonMeshEdge_poly2_set;
*swig_active_get = *cspacec::csPolygonMeshEdge_active_get;
*swig_active_set = *cspacec::csPolygonMeshEdge_active_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPolygonMeshEdge(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPolygonMeshEdge($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csTriangleMinMax ##############

package cspace::csTriangleMinMax;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csTriangle cspace );
%OWNER = ();
%ITERATORS = ();
*swig_minx_get = *cspacec::csTriangleMinMax_minx_get;
*swig_minx_set = *cspacec::csTriangleMinMax_minx_set;
*swig_maxx_get = *cspacec::csTriangleMinMax_maxx_get;
*swig_maxx_set = *cspacec::csTriangleMinMax_maxx_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csTriangleMinMax(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csTriangleMinMax($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPolygonMeshTools ##############

package cspace::csPolygonMeshTools;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*CalculateNormals = *cspacec::csPolygonMeshTools_CalculateNormals;
*CalculatePlanes = *cspacec::csPolygonMeshTools_CalculatePlanes;
*CalculateEdges = *cspacec::csPolygonMeshTools_CalculateEdges;
*CheckActiveEdges = *cspacec::csPolygonMeshTools_CheckActiveEdges;
*CalculateOutline = *cspacec::csPolygonMeshTools_CalculateOutline;
*IsMeshClosed = *cspacec::csPolygonMeshTools_IsMeshClosed;
*IsMeshConvex = *cspacec::csPolygonMeshTools_IsMeshConvex;
*CloseMesh = *cspacec::csPolygonMeshTools_CloseMesh;
*Triangulate = *cspacec::csPolygonMeshTools_Triangulate;
*Polygonize = *cspacec::csPolygonMeshTools_Polygonize;
*SortTrianglesX = *cspacec::csPolygonMeshTools_SortTrianglesX;
*PointInClosedMesh = *cspacec::csPolygonMeshTools_PointInClosedMesh;
*LineInClosedMesh = *cspacec::csPolygonMeshTools_LineInClosedMesh;
*BoxInClosedMesh = *cspacec::csPolygonMeshTools_BoxInClosedMesh;
*CalculateVertexConnections = *cspacec::csPolygonMeshTools_CalculateVertexConnections;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPolygonMeshTools(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPolygonMeshTools($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iFrustumViewUserdata ##############

package cspace::iFrustumViewUserdata;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iFrustumViewUserdata($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iFrustumViewUserdata_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csFrustumContext ##############

package cspace::csFrustumContext;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csFrustumContext(@_);
    bless $self, $pkg if defined($self);
}

*__copy__ = *cspacec::csFrustumContext___copy__;
*GetShadows = *cspacec::csFrustumContext_GetShadows;
*SetShadows = *cspacec::csFrustumContext_SetShadows;
*SetNewShadows = *cspacec::csFrustumContext_SetNewShadows;
*IsShared = *cspacec::csFrustumContext_IsShared;
*SetLightFrustum = *cspacec::csFrustumContext_SetLightFrustum;
*SetNewLightFrustum = *cspacec::csFrustumContext_SetNewLightFrustum;
*GetLightFrustum = *cspacec::csFrustumContext_GetLightFrustum;
*SetMirrored = *cspacec::csFrustumContext_SetMirrored;
*IsMirrored = *cspacec::csFrustumContext_IsMirrored;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csFrustumContext($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iFrustumView ##############

package cspace::iFrustumView;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetFrustumContext = *cspacec::iFrustumView_GetFrustumContext;
*CreateFrustumContext = *cspacec::iFrustumView_CreateFrustumContext;
*CopyFrustumContext = *cspacec::iFrustumView_CopyFrustumContext;
*SetFrustumContext = *cspacec::iFrustumView_SetFrustumContext;
*RestoreFrustumContext = *cspacec::iFrustumView_RestoreFrustumContext;
*SetObjectFunction = *cspacec::iFrustumView_SetObjectFunction;
*CallObjectFunction = *cspacec::iFrustumView_CallObjectFunction;
*GetRadius = *cspacec::iFrustumView_GetRadius;
*GetSquaredRadius = *cspacec::iFrustumView_GetSquaredRadius;
*CheckShadowMask = *cspacec::iFrustumView_CheckShadowMask;
*CheckProcessMask = *cspacec::iFrustumView_CheckProcessMask;
*StartNewShadowBlock = *cspacec::iFrustumView_StartNewShadowBlock;
*SetUserdata = *cspacec::iFrustumView_SetUserdata;
*GetUserdata = *cspacec::iFrustumView_GetUserdata;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iFrustumView($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iFrustumView_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iLightCallback ##############

package cspace::iLightCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*OnColorChange = *cspacec::iLightCallback_OnColorChange;
*OnPositionChange = *cspacec::iLightCallback_OnPositionChange;
*OnSectorChange = *cspacec::iLightCallback_OnSectorChange;
*OnRadiusChange = *cspacec::iLightCallback_OnRadiusChange;
*OnDestroy = *cspacec::iLightCallback_OnDestroy;
*OnAttenuationChange = *cspacec::iLightCallback_OnAttenuationChange;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iLightCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iLight ##############

package cspace::iLight;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetLightID = *cspacec::iLight_GetLightID;
*QueryObject = *cspacec::iLight_QueryObject;
*GetDynamicType = *cspacec::iLight_GetDynamicType;
*GetCenter = *cspacec::iLight_GetCenter;
*GetFullCenter = *cspacec::iLight_GetFullCenter;
*SetCenter = *cspacec::iLight_SetCenter;
*GetSector = *cspacec::iLight_GetSector;
*GetMovable = *cspacec::iLight_GetMovable;
*QuerySceneNode = *cspacec::iLight_QuerySceneNode;
*GetColor = *cspacec::iLight_GetColor;
*SetColor = *cspacec::iLight_SetColor;
*GetSpecularColor = *cspacec::iLight_GetSpecularColor;
*SetSpecularColor = *cspacec::iLight_SetSpecularColor;
*GetType = *cspacec::iLight_GetType;
*SetType = *cspacec::iLight_SetType;
*GetAttenuationMode = *cspacec::iLight_GetAttenuationMode;
*SetAttenuationMode = *cspacec::iLight_SetAttenuationMode;
*SetAttenuationConstants = *cspacec::iLight_SetAttenuationConstants;
*GetAttenuationConstants = *cspacec::iLight_GetAttenuationConstants;
*GetCutoffDistance = *cspacec::iLight_GetCutoffDistance;
*SetCutoffDistance = *cspacec::iLight_SetCutoffDistance;
*GetDirectionalCutoffRadius = *cspacec::iLight_GetDirectionalCutoffRadius;
*SetDirectionalCutoffRadius = *cspacec::iLight_SetDirectionalCutoffRadius;
*SetSpotLightFalloff = *cspacec::iLight_SetSpotLightFalloff;
*GetSpotLightFalloff = *cspacec::iLight_GetSpotLightFalloff;
*CreateCrossHalo = *cspacec::iLight_CreateCrossHalo;
*CreateNovaHalo = *cspacec::iLight_CreateNovaHalo;
*CreateFlareHalo = *cspacec::iLight_CreateFlareHalo;
*GetHalo = *cspacec::iLight_GetHalo;
*GetBrightnessAtDistance = *cspacec::iLight_GetBrightnessAtDistance;
*GetFlags = *cspacec::iLight_GetFlags;
*SetLightCallback = *cspacec::iLight_SetLightCallback;
*RemoveLightCallback = *cspacec::iLight_RemoveLightCallback;
*GetLightCallbackCount = *cspacec::iLight_GetLightCallbackCount;
*GetLightCallback = *cspacec::iLight_GetLightCallback;
*GetLightNumber = *cspacec::iLight_GetLightNumber;
*AddAffectedLightingInfo = *cspacec::iLight_AddAffectedLightingInfo;
*RemoveAffectedLightingInfo = *cspacec::iLight_RemoveAffectedLightingInfo;
*Setup = *cspacec::iLight_Setup;
*GetSVContext = *cspacec::iLight_GetSVContext;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iLight($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iLight_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iLightList ##############

package cspace::iLightList;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetCount = *cspacec::iLightList_GetCount;
*Get = *cspacec::iLightList_Get;
*Add = *cspacec::iLightList_Add;
*Remove = *cspacec::iLightList_Remove;
*RemoveAll = *cspacec::iLightList_RemoveAll;
*Find = *cspacec::iLightList_Find;
*FindByName = *cspacec::iLightList_FindByName;
*FindByID = *cspacec::iLightList_FindByID;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iLightList($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iLightList_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iLightingProcessData ##############

package cspace::iLightingProcessData;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*FinalizeLighting = *cspacec::iLightingProcessData_FinalizeLighting;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iLightingProcessData($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iLightingProcessInfo ##############

package cspace::iLightingProcessInfo;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iFrustumViewUserdata cspace );
%OWNER = ();
%ITERATORS = ();
*GetLight = *cspacec::iLightingProcessInfo_GetLight;
*IsDynamic = *cspacec::iLightingProcessInfo_IsDynamic;
*SetColor = *cspacec::iLightingProcessInfo_SetColor;
*GetColor = *cspacec::iLightingProcessInfo_GetColor;
*AttachUserdata = *cspacec::iLightingProcessInfo_AttachUserdata;
*QueryUserdata = *cspacec::iLightingProcessInfo_QueryUserdata;
*FinalizeLighting = *cspacec::iLightingProcessInfo_FinalizeLighting;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iLightingProcessInfo($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iLightIterator ##############

package cspace::iLightIterator;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*HasNext = *cspacec::iLightIterator_HasNext;
*Next = *cspacec::iLightIterator_Next;
*GetLastSector = *cspacec::iLightIterator_GetLastSector;
*Reset = *cspacec::iLightIterator_Reset;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iLightIterator($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csFog ##############

package cspace::csFog;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_density_get = *cspacec::csFog_density_get;
*swig_density_set = *cspacec::csFog_density_set;
*swig_color_get = *cspacec::csFog_color_get;
*swig_color_set = *cspacec::csFog_color_set;
*swig_start_get = *cspacec::csFog_start_get;
*swig_start_set = *cspacec::csFog_start_set;
*swig_end_get = *cspacec::csFog_end_get;
*swig_end_set = *cspacec::csFog_end_set;
*swig_mode_get = *cspacec::csFog_mode_get;
*swig_mode_set = *cspacec::csFog_mode_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csFog(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csFog($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSectorCallback ##############

package cspace::iSectorCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Traverse = *cspacec::iSectorCallback_Traverse;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSectorCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSectorMeshCallback ##############

package cspace::iSectorMeshCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*NewMesh = *cspacec::iSectorMeshCallback_NewMesh;
*RemoveMesh = *cspacec::iSectorMeshCallback_RemoveMesh;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSectorMeshCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iLightVisibleCallback ##############

package cspace::iLightVisibleCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*LightVisible = *cspacec::iLightVisibleCallback_LightVisible;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iLightVisibleCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSectorHitBeamResult ##############

package cspace::csSectorHitBeamResult;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_mesh_get = *cspacec::csSectorHitBeamResult_mesh_get;
*swig_mesh_set = *cspacec::csSectorHitBeamResult_mesh_set;
*swig_isect_get = *cspacec::csSectorHitBeamResult_isect_get;
*swig_isect_set = *cspacec::csSectorHitBeamResult_isect_set;
*swig_polygon_idx_get = *cspacec::csSectorHitBeamResult_polygon_idx_get;
*swig_polygon_idx_set = *cspacec::csSectorHitBeamResult_polygon_idx_set;
*swig_final_sector_get = *cspacec::csSectorHitBeamResult_final_sector_get;
*swig_final_sector_set = *cspacec::csSectorHitBeamResult_final_sector_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csSectorHitBeamResult(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSectorHitBeamResult($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSector ##############

package cspace::iSector;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iSector_QueryObject;
*GetMeshes = *cspacec::iSector_GetMeshes;
*GetVisibleMeshes = *cspacec::iSector_GetVisibleMeshes;
*GetPortalMeshes = *cspacec::iSector_GetPortalMeshes;
*UnlinkObjects = *cspacec::iSector_UnlinkObjects;
*AddSectorMeshCallback = *cspacec::iSector_AddSectorMeshCallback;
*RemoveSectorMeshCallback = *cspacec::iSector_RemoveSectorMeshCallback;
*Draw = *cspacec::iSector_Draw;
*PrepareDraw = *cspacec::iSector_PrepareDraw;
*GetRecLevel = *cspacec::iSector_GetRecLevel;
*IncRecLevel = *cspacec::iSector_IncRecLevel;
*DecRecLevel = *cspacec::iSector_DecRecLevel;
*SetRenderLoop = *cspacec::iSector_SetRenderLoop;
*GetRenderLoop = *cspacec::iSector_GetRenderLoop;
*CreateMeshGenerator = *cspacec::iSector_CreateMeshGenerator;
*GetMeshGeneratorCount = *cspacec::iSector_GetMeshGeneratorCount;
*GetMeshGenerator = *cspacec::iSector_GetMeshGenerator;
*GetMeshGeneratorByName = *cspacec::iSector_GetMeshGeneratorByName;
*RemoveMeshGenerator = *cspacec::iSector_RemoveMeshGenerator;
*RemoveMeshGenerators = *cspacec::iSector_RemoveMeshGenerators;
*HasFog = *cspacec::iSector_HasFog;
*GetFog = *cspacec::iSector_GetFog;
*SetFog = *cspacec::iSector_SetFog;
*DisableFog = *cspacec::iSector_DisableFog;
*GetLights = *cspacec::iSector_GetLights;
*ShineLights = *cspacec::iSector_ShineLights;
*SetDynamicAmbientLight = *cspacec::iSector_SetDynamicAmbientLight;
*GetDynamicAmbientLight = *cspacec::iSector_GetDynamicAmbientLight;
*GetDynamicAmbientVersion = *cspacec::iSector_GetDynamicAmbientVersion;
*CalculateSectorBBox = *cspacec::iSector_CalculateSectorBBox;
*SetVisibilityCullerPlugin = *cspacec::iSector_SetVisibilityCullerPlugin;
*GetVisibilityCuller = *cspacec::iSector_GetVisibilityCuller;
*CheckFrustum = *cspacec::iSector_CheckFrustum;
*HitBeamPortals = *cspacec::iSector_HitBeamPortals;
*HitBeam = *cspacec::iSector_HitBeam;
*FollowSegment = *cspacec::iSector_FollowSegment;
*SetSectorCallback = *cspacec::iSector_SetSectorCallback;
*RemoveSectorCallback = *cspacec::iSector_RemoveSectorCallback;
*GetSectorCallbackCount = *cspacec::iSector_GetSectorCallbackCount;
*GetSectorCallback = *cspacec::iSector_GetSectorCallback;
*SetLightCulling = *cspacec::iSector_SetLightCulling;
*IsLightCullingEnabled = *cspacec::iSector_IsLightCullingEnabled;
*AddLightVisibleCallback = *cspacec::iSector_AddLightVisibleCallback;
*RemoveLightVisibleCallback = *cspacec::iSector_RemoveLightVisibleCallback;
*GetSVContext = *cspacec::iSector_GetSVContext;
*PrecacheDraw = *cspacec::iSector_PrecacheDraw;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSector($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSector_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSectorList ##############

package cspace::iSectorList;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetCount = *cspacec::iSectorList_GetCount;
*Get = *cspacec::iSectorList_Get;
*Add = *cspacec::iSectorList_Add;
*Remove = *cspacec::iSectorList_Remove;
*RemoveAll = *cspacec::iSectorList_RemoveAll;
*Find = *cspacec::iSectorList_Find;
*FindByName = *cspacec::iSectorList_FindByName;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSectorList($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSectorList_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSectorIterator ##############

package cspace::iSectorIterator;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*HasNext = *cspacec::iSectorIterator_HasNext;
*Next = *cspacec::iSectorIterator_Next;
*GetLastPosition = *cspacec::iSectorIterator_GetLastPosition;
*Reset = *cspacec::iSectorIterator_Reset;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSectorIterator($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iEngineFrameCallback ##############

package cspace::iEngineFrameCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*StartFrame = *cspacec::iEngineFrameCallback_StartFrame;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iEngineFrameCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iEngineSectorCallback ##############

package cspace::iEngineSectorCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*NewSector = *cspacec::iEngineSectorCallback_NewSector;
*RemoveSector = *cspacec::iEngineSectorCallback_RemoveSector;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iEngineSectorCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iEngine ##############

package cspace::iEngine;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iEngine_QueryObject;
*Prepare = *cspacec::iEngine_Prepare;
*PrepareTextures = *cspacec::iEngine_PrepareTextures;
*PrepareMeshes = *cspacec::iEngine_PrepareMeshes;
*ForceRelight = *cspacec::iEngine_ForceRelight;
*ShineLights = *cspacec::iEngine_ShineLights;
*SetLightingCacheMode = *cspacec::iEngine_SetLightingCacheMode;
*GetLightingCacheMode = *cspacec::iEngine_GetLightingCacheMode;
*SetCacheManager = *cspacec::iEngine_SetCacheManager;
*SetVFSCacheManager = *cspacec::iEngine_SetVFSCacheManager;
*GetCacheManager = *cspacec::iEngine_GetCacheManager;
*SetMaxLightmapSize = *cspacec::iEngine_SetMaxLightmapSize;
*GetMaxLightmapSize = *cspacec::iEngine_GetMaxLightmapSize;
*GetDefaultMaxLightmapSize = *cspacec::iEngine_GetDefaultMaxLightmapSize;
*GetMaxLightmapAspectRatio = *cspacec::iEngine_GetMaxLightmapAspectRatio;
*RegisterRenderPriority = *cspacec::iEngine_RegisterRenderPriority;
*GetRenderPriority = *cspacec::iEngine_GetRenderPriority;
*GetRenderPrioritySorting = *cspacec::iEngine_GetRenderPrioritySorting;
*GetSkyRenderPriority = *cspacec::iEngine_GetSkyRenderPriority;
*GetPortalRenderPriority = *cspacec::iEngine_GetPortalRenderPriority;
*GetWallRenderPriority = *cspacec::iEngine_GetWallRenderPriority;
*GetObjectRenderPriority = *cspacec::iEngine_GetObjectRenderPriority;
*GetAlphaRenderPriority = *cspacec::iEngine_GetAlphaRenderPriority;
*ClearRenderPriorities = *cspacec::iEngine_ClearRenderPriorities;
*GetRenderPriorityCount = *cspacec::iEngine_GetRenderPriorityCount;
*GetRenderPriorityName = *cspacec::iEngine_GetRenderPriorityName;
*CreateBaseMaterial = *cspacec::iEngine_CreateBaseMaterial;
*CreateMaterial = *cspacec::iEngine_CreateMaterial;
*GetMaterialList = *cspacec::iEngine_GetMaterialList;
*FindMaterial = *cspacec::iEngine_FindMaterial;
*CreateTexture = *cspacec::iEngine_CreateTexture;
*CreateBlackTexture = *cspacec::iEngine_CreateBlackTexture;
*GetTextureFormat = *cspacec::iEngine_GetTextureFormat;
*GetTextureList = *cspacec::iEngine_GetTextureList;
*FindTexture = *cspacec::iEngine_FindTexture;
*CreateLight = *cspacec::iEngine_CreateLight;
*FindLight = *cspacec::iEngine_FindLight;
*FindLightID = *cspacec::iEngine_FindLightID;
*GetLightIterator = *cspacec::iEngine_GetLightIterator;
*RemoveLight = *cspacec::iEngine_RemoveLight;
*SetAmbientLight = *cspacec::iEngine_SetAmbientLight;
*GetAmbientLight = *cspacec::iEngine_GetAmbientLight;
*GetDefaultAmbientLight = *cspacec::iEngine_GetDefaultAmbientLight;
*GetNearbyLights = *cspacec::iEngine_GetNearbyLights;
*CreateSector = *cspacec::iEngine_CreateSector;
*GetSectors = *cspacec::iEngine_GetSectors;
*FindSector = *cspacec::iEngine_FindSector;
*GetNearbySectors = *cspacec::iEngine_GetNearbySectors;
*AddEngineFrameCallback = *cspacec::iEngine_AddEngineFrameCallback;
*RemoveEngineFrameCallback = *cspacec::iEngine_RemoveEngineFrameCallback;
*AddEngineSectorCallback = *cspacec::iEngine_AddEngineSectorCallback;
*RemoveEngineSectorCallback = *cspacec::iEngine_RemoveEngineSectorCallback;
*CreateMeshWrapper = *cspacec::iEngine_CreateMeshWrapper;
*CreateSectorWallsMesh = *cspacec::iEngine_CreateSectorWallsMesh;
*CreateThingMesh = *cspacec::iEngine_CreateThingMesh;
*LoadMeshWrapper = *cspacec::iEngine_LoadMeshWrapper;
*AddMeshAndChildren = *cspacec::iEngine_AddMeshAndChildren;
*GetNearbyMeshes = *cspacec::iEngine_GetNearbyMeshes;
*GetMeshes = *cspacec::iEngine_GetMeshes;
*FindMeshObject = *cspacec::iEngine_FindMeshObject;
*WantToDie = *cspacec::iEngine_WantToDie;
*CreateMeshFactory = *cspacec::iEngine_CreateMeshFactory;
*LoadMeshFactory = *cspacec::iEngine_LoadMeshFactory;
*FindMeshFactory = *cspacec::iEngine_FindMeshFactory;
*GetMeshFactories = *cspacec::iEngine_GetMeshFactories;
*CreateRegion = *cspacec::iEngine_CreateRegion;
*GetRegions = *cspacec::iEngine_GetRegions;
*CreateCamera = *cspacec::iEngine_CreateCamera;
*FindCameraPosition = *cspacec::iEngine_FindCameraPosition;
*GetCameraPositions = *cspacec::iEngine_GetCameraPositions;
*CreatePortal = *cspacec::iEngine_CreatePortal;
*CreatePortalContainer = *cspacec::iEngine_CreatePortalContainer;
*SetClearZBuf = *cspacec::iEngine_SetClearZBuf;
*GetClearZBuf = *cspacec::iEngine_GetClearZBuf;
*GetDefaultClearZBuf = *cspacec::iEngine_GetDefaultClearZBuf;
*SetClearScreen = *cspacec::iEngine_SetClearScreen;
*GetClearScreen = *cspacec::iEngine_GetClearScreen;
*GetDefaultClearScreen = *cspacec::iEngine_GetDefaultClearScreen;
*GetBeginDrawFlags = *cspacec::iEngine_GetBeginDrawFlags;
*GetTopLevelClipper = *cspacec::iEngine_GetTopLevelClipper;
*PrecacheDraw = *cspacec::iEngine_PrecacheDraw;
*Draw = *cspacec::iEngine_Draw;
*SetContext = *cspacec::iEngine_SetContext;
*GetContext = *cspacec::iEngine_GetContext;
*GetRenderLoopManager = *cspacec::iEngine_GetRenderLoopManager;
*GetCurrentDefaultRenderloop = *cspacec::iEngine_GetCurrentDefaultRenderloop;
*SetCurrentDefaultRenderloop = *cspacec::iEngine_SetCurrentDefaultRenderloop;
*GetCurrentFrameNumber = *cspacec::iEngine_GetCurrentFrameNumber;
*SetSaveableFlag = *cspacec::iEngine_SetSaveableFlag;
*GetSaveableFlag = *cspacec::iEngine_GetSaveableFlag;
*CreateLoaderContext = *cspacec::iEngine_CreateLoaderContext;
*GetNearbyObjects = *cspacec::iEngine_GetNearbyObjects;
*GetVisibleObjects = *cspacec::iEngine_GetVisibleObjects;
*GetVisibleMeshes = *cspacec::iEngine_GetVisibleMeshes;
*CreateFrustumView = *cspacec::iEngine_CreateFrustumView;
*CreateObjectWatcher = *cspacec::iEngine_CreateObjectWatcher;
*GetVariableList = *cspacec::iEngine_GetVariableList;
*RemoveObject = *cspacec::iEngine_RemoveObject;
*DelayedRemoveObject = *cspacec::iEngine_DelayedRemoveObject;
*RemoveDelayedRemoves = *cspacec::iEngine_RemoveDelayedRemoves;
*DeleteAll = *cspacec::iEngine_DeleteAll;
*ResetWorldSpecificSettings = *cspacec::iEngine_ResetWorldSpecificSettings;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iEngine($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iEngine_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iCameraSectorListener ##############

package cspace::iCameraSectorListener;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*NewSector = *cspacec::iCameraSectorListener_NewSector;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iCameraSectorListener($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iCamera ##############

package cspace::iCamera;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Clone = *cspacec::iCamera_Clone;
*QuerySceneNode = *cspacec::iCamera_QuerySceneNode;
*GetFOV = *cspacec::iCamera_GetFOV;
*GetInvFOV = *cspacec::iCamera_GetInvFOV;
*GetFOVAngle = *cspacec::iCamera_GetFOVAngle;
*SetFOV = *cspacec::iCamera_SetFOV;
*SetFOVAngle = *cspacec::iCamera_SetFOVAngle;
*GetShiftX = *cspacec::iCamera_GetShiftX;
*GetShiftY = *cspacec::iCamera_GetShiftY;
*SetPerspectiveCenter = *cspacec::iCamera_SetPerspectiveCenter;
*GetTransform = *cspacec::iCamera_GetTransform;
*SetTransform = *cspacec::iCamera_SetTransform;
*MoveWorld = *cspacec::iCamera_MoveWorld;
*Move = *cspacec::iCamera_Move;
*MoveWorldUnrestricted = *cspacec::iCamera_MoveWorldUnrestricted;
*MoveUnrestricted = *cspacec::iCamera_MoveUnrestricted;
*GetSector = *cspacec::iCamera_GetSector;
*SetSector = *cspacec::iCamera_SetSector;
*Correct = *cspacec::iCamera_Correct;
*IsMirrored = *cspacec::iCamera_IsMirrored;
*SetMirrored = *cspacec::iCamera_SetMirrored;
*GetFarPlane = *cspacec::iCamera_GetFarPlane;
*SetFarPlane = *cspacec::iCamera_SetFarPlane;
*GetCameraNumber = *cspacec::iCamera_GetCameraNumber;
*Perspective = *cspacec::iCamera_Perspective;
*InvPerspective = *cspacec::iCamera_InvPerspective;
*OnlyPortals = *cspacec::iCamera_OnlyPortals;
*GetOnlyPortals = *cspacec::iCamera_GetOnlyPortals;
*AddCameraSectorListener = *cspacec::iCamera_AddCameraSectorListener;
*RemoveCameraSectorListener = *cspacec::iCamera_RemoveCameraSectorListener;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iCamera($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iCamera_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iCameraPosition ##############

package cspace::iCameraPosition;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iCameraPosition_QueryObject;
*Clone = *cspacec::iCameraPosition_Clone;
*GetSector = *cspacec::iCameraPosition_GetSector;
*SetSector = *cspacec::iCameraPosition_SetSector;
*GetPosition = *cspacec::iCameraPosition_GetPosition;
*SetPosition = *cspacec::iCameraPosition_SetPosition;
*GetUpwardVector = *cspacec::iCameraPosition_GetUpwardVector;
*SetUpwardVector = *cspacec::iCameraPosition_SetUpwardVector;
*GetForwardVector = *cspacec::iCameraPosition_GetForwardVector;
*SetForwardVector = *cspacec::iCameraPosition_SetForwardVector;
*Set = *cspacec::iCameraPosition_Set;
*Load = *cspacec::iCameraPosition_Load;
*SetFarPlane = *cspacec::iCameraPosition_SetFarPlane;
*ClearFarPlane = *cspacec::iCameraPosition_ClearFarPlane;
*GetFarPlane = *cspacec::iCameraPosition_GetFarPlane;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iCameraPosition($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iCameraPosition_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iCameraPositionList ##############

package cspace::iCameraPositionList;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*NewCameraPosition = *cspacec::iCameraPositionList_NewCameraPosition;
*GetCount = *cspacec::iCameraPositionList_GetCount;
*Get = *cspacec::iCameraPositionList_Get;
*Add = *cspacec::iCameraPositionList_Add;
*Remove = *cspacec::iCameraPositionList_Remove;
*RemoveAll = *cspacec::iCameraPositionList_RemoveAll;
*Find = *cspacec::iCameraPositionList_Find;
*FindByName = *cspacec::iCameraPositionList_FindByName;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iCameraPositionList($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iTextureCallback ##############

package cspace::iTextureCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*UseTexture = *cspacec::iTextureCallback_UseTexture;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iTextureCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iTextureWrapper ##############

package cspace::iTextureWrapper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iTextureWrapper_QueryObject;
*Clone = *cspacec::iTextureWrapper_Clone;
*SetImageFile = *cspacec::iTextureWrapper_SetImageFile;
*GetImageFile = *cspacec::iTextureWrapper_GetImageFile;
*SetTextureHandle = *cspacec::iTextureWrapper_SetTextureHandle;
*GetTextureHandle = *cspacec::iTextureWrapper_GetTextureHandle;
*SetKeyColor = *cspacec::iTextureWrapper_SetKeyColor;
*GetKeyColor = *cspacec::iTextureWrapper_GetKeyColor;
*SetFlags = *cspacec::iTextureWrapper_SetFlags;
*GetFlags = *cspacec::iTextureWrapper_GetFlags;
*Register = *cspacec::iTextureWrapper_Register;
*SetUseCallback = *cspacec::iTextureWrapper_SetUseCallback;
*GetUseCallback = *cspacec::iTextureWrapper_GetUseCallback;
*Visit = *cspacec::iTextureWrapper_Visit;
*IsVisitRequired = *cspacec::iTextureWrapper_IsVisitRequired;
*SetKeepImage = *cspacec::iTextureWrapper_SetKeepImage;
*KeepImage = *cspacec::iTextureWrapper_KeepImage;
*SetTextureClass = *cspacec::iTextureWrapper_SetTextureClass;
*GetTextureClass = *cspacec::iTextureWrapper_GetTextureClass;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iTextureWrapper($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iTextureWrapper_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iTextureList ##############

package cspace::iTextureList;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*NewTexture = *cspacec::iTextureList_NewTexture;
*GetCount = *cspacec::iTextureList_GetCount;
*Get = *cspacec::iTextureList_Get;
*Add = *cspacec::iTextureList_Add;
*Remove = *cspacec::iTextureList_Remove;
*RemoveAll = *cspacec::iTextureList_RemoveAll;
*Find = *cspacec::iTextureList_Find;
*FindByName = *cspacec::iTextureList_FindByName;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iTextureList($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iTextureList_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMaterialWrapper ##############

package cspace::iMaterialWrapper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iMaterialWrapper_QueryObject;
*SetMaterial = *cspacec::iMaterialWrapper_SetMaterial;
*GetMaterial = *cspacec::iMaterialWrapper_GetMaterial;
*Visit = *cspacec::iMaterialWrapper_Visit;
*IsVisitRequired = *cspacec::iMaterialWrapper_IsVisitRequired;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMaterialWrapper($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iMaterialWrapper_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMaterialEngine ##############

package cspace::iMaterialEngine;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetTextureWrapper = *cspacec::iMaterialEngine_GetTextureWrapper;
*Visit = *cspacec::iMaterialEngine_Visit;
*IsVisitRequired = *cspacec::iMaterialEngine_IsVisitRequired;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMaterialEngine($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMaterialList ##############

package cspace::iMaterialList;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*NewMaterial = *cspacec::iMaterialList_NewMaterial;
*GetCount = *cspacec::iMaterialList_GetCount;
*Get = *cspacec::iMaterialList_Get;
*Add = *cspacec::iMaterialList_Add;
*Remove = *cspacec::iMaterialList_Remove;
*RemoveAll = *cspacec::iMaterialList_RemoveAll;
*Find = *cspacec::iMaterialList_Find;
*FindByName = *cspacec::iMaterialList_FindByName;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMaterialList($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSceneNodeArrayReadOnly ##############

package cspace::iSceneNodeArrayReadOnly;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetSize = *cspacec::iSceneNodeArrayReadOnly_GetSize;
*Get = *cspacec::iSceneNodeArrayReadOnly_Get;
*Top = *cspacec::iSceneNodeArrayReadOnly_Top;
*Find = *cspacec::iSceneNodeArrayReadOnly_Find;
*GetIndex = *cspacec::iSceneNodeArrayReadOnly_GetIndex;
*IsEmpty = *cspacec::iSceneNodeArrayReadOnly_IsEmpty;
*GetAll = *cspacec::iSceneNodeArrayReadOnly_GetAll;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSceneNodeArrayReadOnly($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSceneNodeArray ##############

package cspace::iSceneNodeArray;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iSceneNodeArrayReadOnly cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSceneNodeArray($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSceneNodeArray_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSceneNode ##############

package cspace::iSceneNode;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetMovable = *cspacec::iSceneNode_GetMovable;
*QueryMesh = *cspacec::iSceneNode_QueryMesh;
*QueryLight = *cspacec::iSceneNode_QueryLight;
*QueryCamera = *cspacec::iSceneNode_QueryCamera;
*SetParent = *cspacec::iSceneNode_SetParent;
*GetParent = *cspacec::iSceneNode_GetParent;
*GetChildren = *cspacec::iSceneNode_GetChildren;
*GetChildrenArray = *cspacec::iSceneNode_GetChildrenArray;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSceneNode($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSceneNode_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMeshDrawCallback ##############

package cspace::iMeshDrawCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*BeforeDrawing = *cspacec::iMeshDrawCallback_BeforeDrawing;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMeshDrawCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csHitBeamResult ##############

package cspace::csHitBeamResult;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_isect_get = *cspacec::csHitBeamResult_isect_get;
*swig_isect_set = *cspacec::csHitBeamResult_isect_set;
*swig_r_get = *cspacec::csHitBeamResult_r_get;
*swig_r_set = *cspacec::csHitBeamResult_r_set;
*swig_polygon_idx_get = *cspacec::csHitBeamResult_polygon_idx_get;
*swig_polygon_idx_set = *cspacec::csHitBeamResult_polygon_idx_set;
*swig_material_get = *cspacec::csHitBeamResult_material_get;
*swig_material_set = *cspacec::csHitBeamResult_material_set;
*swig_facehit_get = *cspacec::csHitBeamResult_facehit_get;
*swig_facehit_set = *cspacec::csHitBeamResult_facehit_set;
*swig_hit_get = *cspacec::csHitBeamResult_hit_get;
*swig_hit_set = *cspacec::csHitBeamResult_hit_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csHitBeamResult(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csHitBeamResult($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csScreenBoxResult ##############

package cspace::csScreenBoxResult;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_sbox_get = *cspacec::csScreenBoxResult_sbox_get;
*swig_sbox_set = *cspacec::csScreenBoxResult_sbox_set;
*swig_cbox_get = *cspacec::csScreenBoxResult_cbox_get;
*swig_cbox_set = *cspacec::csScreenBoxResult_cbox_set;
*swig_distance_get = *cspacec::csScreenBoxResult_distance_get;
*swig_distance_set = *cspacec::csScreenBoxResult_distance_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csScreenBoxResult(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csScreenBoxResult($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMeshWrapper ##############

package cspace::iMeshWrapper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iMeshWrapper_QueryObject;
*GetMeshObject = *cspacec::iMeshWrapper_GetMeshObject;
*SetMeshObject = *cspacec::iMeshWrapper_SetMeshObject;
*GetPortalContainer = *cspacec::iMeshWrapper_GetPortalContainer;
*GetLightingInfo = *cspacec::iMeshWrapper_GetLightingInfo;
*GetShadowReceiver = *cspacec::iMeshWrapper_GetShadowReceiver;
*GetShadowCaster = *cspacec::iMeshWrapper_GetShadowCaster;
*GetFactory = *cspacec::iMeshWrapper_GetFactory;
*SetFactory = *cspacec::iMeshWrapper_SetFactory;
*SetLightingUpdate = *cspacec::iMeshWrapper_SetLightingUpdate;
*GetMovable = *cspacec::iMeshWrapper_GetMovable;
*QuerySceneNode = *cspacec::iMeshWrapper_QuerySceneNode;
*FindChildByName = *cspacec::iMeshWrapper_FindChildByName;
*PlaceMesh = *cspacec::iMeshWrapper_PlaceMesh;
*HitBeamBBox = *cspacec::iMeshWrapper_HitBeamBBox;
*HitBeamOutline = *cspacec::iMeshWrapper_HitBeamOutline;
*HitBeamObject = *cspacec::iMeshWrapper_HitBeamObject;
*HitBeam = *cspacec::iMeshWrapper_HitBeam;
*SetDrawCallback = *cspacec::iMeshWrapper_SetDrawCallback;
*RemoveDrawCallback = *cspacec::iMeshWrapper_RemoveDrawCallback;
*GetDrawCallbackCount = *cspacec::iMeshWrapper_GetDrawCallbackCount;
*GetDrawCallback = *cspacec::iMeshWrapper_GetDrawCallback;
*SetRenderPriority = *cspacec::iMeshWrapper_SetRenderPriority;
*GetRenderPriority = *cspacec::iMeshWrapper_GetRenderPriority;
*SetRenderPriorityRecursive = *cspacec::iMeshWrapper_SetRenderPriorityRecursive;
*GetFlags = *cspacec::iMeshWrapper_GetFlags;
*SetFlagsRecursive = *cspacec::iMeshWrapper_SetFlagsRecursive;
*SetZBufMode = *cspacec::iMeshWrapper_SetZBufMode;
*GetZBufMode = *cspacec::iMeshWrapper_GetZBufMode;
*SetZBufModeRecursive = *cspacec::iMeshWrapper_SetZBufModeRecursive;
*HardTransform = *cspacec::iMeshWrapper_HardTransform;
*GetWorldBoundingBox = *cspacec::iMeshWrapper_GetWorldBoundingBox;
*GetTransformedBoundingBox = *cspacec::iMeshWrapper_GetTransformedBoundingBox;
*GetScreenBoundingBox = *cspacec::iMeshWrapper_GetScreenBoundingBox;
*GetRadius = *cspacec::iMeshWrapper_GetRadius;
*ResetMinMaxRenderDistance = *cspacec::iMeshWrapper_ResetMinMaxRenderDistance;
*SetMinimumRenderDistance = *cspacec::iMeshWrapper_SetMinimumRenderDistance;
*GetMinimumRenderDistance = *cspacec::iMeshWrapper_GetMinimumRenderDistance;
*SetMaximumRenderDistance = *cspacec::iMeshWrapper_SetMaximumRenderDistance;
*GetMaximumRenderDistance = *cspacec::iMeshWrapper_GetMaximumRenderDistance;
*SetMinimumRenderDistanceVar = *cspacec::iMeshWrapper_SetMinimumRenderDistanceVar;
*GetMinimumRenderDistanceVar = *cspacec::iMeshWrapper_GetMinimumRenderDistanceVar;
*SetMaximumRenderDistanceVar = *cspacec::iMeshWrapper_SetMaximumRenderDistanceVar;
*GetMaximumRenderDistanceVar = *cspacec::iMeshWrapper_GetMaximumRenderDistanceVar;
*CreateStaticLOD = *cspacec::iMeshWrapper_CreateStaticLOD;
*DestroyStaticLOD = *cspacec::iMeshWrapper_DestroyStaticLOD;
*GetStaticLOD = *cspacec::iMeshWrapper_GetStaticLOD;
*AddMeshToStaticLOD = *cspacec::iMeshWrapper_AddMeshToStaticLOD;
*RemoveMeshFromStaticLOD = *cspacec::iMeshWrapper_RemoveMeshFromStaticLOD;
*GetSVContext = *cspacec::iMeshWrapper_GetSVContext;
*AddExtraRenderMesh = *cspacec::iMeshWrapper_AddExtraRenderMesh;
*GetExtraRenderMeshPriority = *cspacec::iMeshWrapper_GetExtraRenderMeshPriority;
*GetExtraRenderMeshZBufMode = *cspacec::iMeshWrapper_GetExtraRenderMeshZBufMode;
*RemoveExtraRenderMesh = *cspacec::iMeshWrapper_RemoveExtraRenderMesh;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMeshWrapper($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iMeshWrapper_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMeshFactoryWrapper ##############

package cspace::iMeshFactoryWrapper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iMeshFactoryWrapper_QueryObject;
*GetMeshObjectFactory = *cspacec::iMeshFactoryWrapper_GetMeshObjectFactory;
*SetMeshObjectFactory = *cspacec::iMeshFactoryWrapper_SetMeshObjectFactory;
*HardTransform = *cspacec::iMeshFactoryWrapper_HardTransform;
*CreateMeshWrapper = *cspacec::iMeshFactoryWrapper_CreateMeshWrapper;
*GetFlags = *cspacec::iMeshFactoryWrapper_GetFlags;
*GetParentContainer = *cspacec::iMeshFactoryWrapper_GetParentContainer;
*SetParentContainer = *cspacec::iMeshFactoryWrapper_SetParentContainer;
*GetChildren = *cspacec::iMeshFactoryWrapper_GetChildren;
*GetTransform = *cspacec::iMeshFactoryWrapper_GetTransform;
*SetTransform = *cspacec::iMeshFactoryWrapper_SetTransform;
*CreateStaticLOD = *cspacec::iMeshFactoryWrapper_CreateStaticLOD;
*DestroyStaticLOD = *cspacec::iMeshFactoryWrapper_DestroyStaticLOD;
*SetStaticLOD = *cspacec::iMeshFactoryWrapper_SetStaticLOD;
*GetStaticLOD = *cspacec::iMeshFactoryWrapper_GetStaticLOD;
*AddFactoryToStaticLOD = *cspacec::iMeshFactoryWrapper_AddFactoryToStaticLOD;
*RemoveFactoryFromStaticLOD = *cspacec::iMeshFactoryWrapper_RemoveFactoryFromStaticLOD;
*SetZBufMode = *cspacec::iMeshFactoryWrapper_SetZBufMode;
*GetZBufMode = *cspacec::iMeshFactoryWrapper_GetZBufMode;
*SetZBufModeRecursive = *cspacec::iMeshFactoryWrapper_SetZBufModeRecursive;
*SetRenderPriority = *cspacec::iMeshFactoryWrapper_SetRenderPriority;
*GetRenderPriority = *cspacec::iMeshFactoryWrapper_GetRenderPriority;
*SetRenderPriorityRecursive = *cspacec::iMeshFactoryWrapper_SetRenderPriorityRecursive;
*GetSVContext = *cspacec::iMeshFactoryWrapper_GetSVContext;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMeshFactoryWrapper($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iMeshFactoryWrapper_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMeshList ##############

package cspace::iMeshList;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetCount = *cspacec::iMeshList_GetCount;
*Get = *cspacec::iMeshList_Get;
*Add = *cspacec::iMeshList_Add;
*Remove = *cspacec::iMeshList_Remove;
*RemoveAll = *cspacec::iMeshList_RemoveAll;
*Find = *cspacec::iMeshList_Find;
*FindByName = *cspacec::iMeshList_FindByName;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMeshList($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMeshFactoryList ##############

package cspace::iMeshFactoryList;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetCount = *cspacec::iMeshFactoryList_GetCount;
*Get = *cspacec::iMeshFactoryList_Get;
*Add = *cspacec::iMeshFactoryList_Add;
*Remove = *cspacec::iMeshFactoryList_Remove;
*RemoveAll = *cspacec::iMeshFactoryList_RemoveAll;
*Find = *cspacec::iMeshFactoryList_Find;
*FindByName = *cspacec::iMeshFactoryList_FindByName;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMeshFactoryList($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMeshWrapperIterator ##############

package cspace::iMeshWrapperIterator;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Next = *cspacec::iMeshWrapperIterator_Next;
*Reset = *cspacec::iMeshWrapperIterator_Reset;
*HasNext = *cspacec::iMeshWrapperIterator_HasNext;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMeshWrapperIterator($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iMeshWrapperIterator_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMovableListener ##############

package cspace::iMovableListener;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*MovableChanged = *cspacec::iMovableListener_MovableChanged;
*MovableDestroyed = *cspacec::iMovableListener_MovableDestroyed;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMovableListener($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iMovableListener_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMovable ##############

package cspace::iMovable;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetSceneNode = *cspacec::iMovable_GetSceneNode;
*SetSector = *cspacec::iMovable_SetSector;
*ClearSectors = *cspacec::iMovable_ClearSectors;
*GetSectors = *cspacec::iMovable_GetSectors;
*InSector = *cspacec::iMovable_InSector;
*SetPosition = *cspacec::iMovable_SetPosition;
*GetPosition = *cspacec::iMovable_GetPosition;
*GetFullPosition = *cspacec::iMovable_GetFullPosition;
*SetTransform = *cspacec::iMovable_SetTransform;
*GetTransform = *cspacec::iMovable_GetTransform;
*GetFullTransform = *cspacec::iMovable_GetFullTransform;
*MovePosition = *cspacec::iMovable_MovePosition;
*Transform = *cspacec::iMovable_Transform;
*AddListener = *cspacec::iMovable_AddListener;
*RemoveListener = *cspacec::iMovable_RemoveListener;
*UpdateMove = *cspacec::iMovable_UpdateMove;
*GetUpdateNumber = *cspacec::iMovable_GetUpdateNumber;
*IsTransformIdentity = *cspacec::iMovable_IsTransformIdentity;
*IsFullTransformIdentity = *cspacec::iMovable_IsFullTransformIdentity;
*TransformIdentity = *cspacec::iMovable_TransformIdentity;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMovable($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iMovable_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iRegion ##############

package cspace::iRegion;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iRegion_QueryObject;
*Add = *cspacec::iRegion_Add;
*Remove = *cspacec::iRegion_Remove;
*Clear = *cspacec::iRegion_Clear;
*DeleteAll = *cspacec::iRegion_DeleteAll;
*PrepareTextures = *cspacec::iRegion_PrepareTextures;
*ShineLights = *cspacec::iRegion_ShineLights;
*Prepare = *cspacec::iRegion_Prepare;
*FindSector = *cspacec::iRegion_FindSector;
*FindMeshObject = *cspacec::iRegion_FindMeshObject;
*FindMeshFactory = *cspacec::iRegion_FindMeshFactory;
*FindTexture = *cspacec::iRegion_FindTexture;
*FindMaterial = *cspacec::iRegion_FindMaterial;
*FindCameraPosition = *cspacec::iRegion_FindCameraPosition;
*IsInRegion = *cspacec::iRegion_IsInRegion;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iRegion($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iRegionList ##############

package cspace::iRegionList;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetCount = *cspacec::iRegionList_GetCount;
*Get = *cspacec::iRegionList_Get;
*Add = *cspacec::iRegionList_Add;
*Remove = *cspacec::iRegionList_Remove;
*RemoveAll = *cspacec::iRegionList_RemoveAll;
*Find = *cspacec::iRegionList_Find;
*FindByName = *cspacec::iRegionList_FindByName;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iRegionList($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iVisibilityObjectIterator ##############

package cspace::iVisibilityObjectIterator;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*HasNext = *cspacec::iVisibilityObjectIterator_HasNext;
*Next = *cspacec::iVisibilityObjectIterator_Next;
*Reset = *cspacec::iVisibilityObjectIterator_Reset;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iVisibilityObjectIterator($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iVisibilityCullerListener ##############

package cspace::iVisibilityCullerListener;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*ObjectVisible = *cspacec::iVisibilityCullerListener_ObjectVisible;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iVisibilityCullerListener($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iVisibilityCuller ##############

package cspace::iVisibilityCuller;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Setup = *cspacec::iVisibilityCuller_Setup;
*RegisterVisObject = *cspacec::iVisibilityCuller_RegisterVisObject;
*UnregisterVisObject = *cspacec::iVisibilityCuller_UnregisterVisObject;
*PrecacheCulling = *cspacec::iVisibilityCuller_PrecacheCulling;
*VisTest = *cspacec::iVisibilityCuller_VisTest;
*IntersectSegmentSloppy = *cspacec::iVisibilityCuller_IntersectSegmentSloppy;
*IntersectSegment = *cspacec::iVisibilityCuller_IntersectSegment;
*CastShadows = *cspacec::iVisibilityCuller_CastShadows;
*ParseCullerParameters = *cspacec::iVisibilityCuller_ParseCullerParameters;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iVisibilityCuller($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iVisibilityCuller_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iVisibilityObject ##############

package cspace::iVisibilityObject;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetMovable = *cspacec::iVisibilityObject_GetMovable;
*GetMeshWrapper = *cspacec::iVisibilityObject_GetMeshWrapper;
*GetObjectModel = *cspacec::iVisibilityObject_GetObjectModel;
*GetCullerFlags = *cspacec::iVisibilityObject_GetCullerFlags;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iVisibilityObject($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iPortalCallback ##############

package cspace::iPortalCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Traverse = *cspacec::iPortalCallback_Traverse;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iPortalCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iPortal ##############

package cspace::iPortal;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetName = *cspacec::iPortal_SetName;
*GetName = *cspacec::iPortal_GetName;
*GetSector = *cspacec::iPortal_GetSector;
*GetVertices = *cspacec::iPortal_GetVertices;
*GetWorldVertices = *cspacec::iPortal_GetWorldVertices;
*GetVertexIndices = *cspacec::iPortal_GetVertexIndices;
*GetVertexIndicesCount = *cspacec::iPortal_GetVertexIndicesCount;
*GetObjectPlane = *cspacec::iPortal_GetObjectPlane;
*GetWorldPlane = *cspacec::iPortal_GetWorldPlane;
*ComputeCameraPlane = *cspacec::iPortal_ComputeCameraPlane;
*PointOnPolygon = *cspacec::iPortal_PointOnPolygon;
*SetSector = *cspacec::iPortal_SetSector;
*GetFlags = *cspacec::iPortal_GetFlags;
*SetMaximumSectorVisit = *cspacec::iPortal_SetMaximumSectorVisit;
*GetMaximumSectorVisit = *cspacec::iPortal_GetMaximumSectorVisit;
*SetPortalCallback = *cspacec::iPortal_SetPortalCallback;
*RemovePortalCallback = *cspacec::iPortal_RemovePortalCallback;
*GetPortalCallbackCount = *cspacec::iPortal_GetPortalCallbackCount;
*GetPortalCallback = *cspacec::iPortal_GetPortalCallback;
*SetMissingSectorCallback = *cspacec::iPortal_SetMissingSectorCallback;
*RemoveMissingSectorCallback = *cspacec::iPortal_RemoveMissingSectorCallback;
*GetMissingSectorCallbackCount = *cspacec::iPortal_GetMissingSectorCallbackCount;
*GetMissingSectorCallback = *cspacec::iPortal_GetMissingSectorCallback;
*GetTextureFilter = *cspacec::iPortal_GetTextureFilter;
*SetFilter = *cspacec::iPortal_SetFilter;
*GetColorFilter = *cspacec::iPortal_GetColorFilter;
*SetWarp = *cspacec::iPortal_SetWarp;
*SetMirror = *cspacec::iPortal_SetMirror;
*GetWarp = *cspacec::iPortal_GetWarp;
*HardTransform = *cspacec::iPortal_HardTransform;
*ObjectToWorld = *cspacec::iPortal_ObjectToWorld;
*Warp = *cspacec::iPortal_Warp;
*WarpSpace = *cspacec::iPortal_WarpSpace;
*CompleteSector = *cspacec::iPortal_CompleteSector;
*CheckFrustum = *cspacec::iPortal_CheckFrustum;
*HitBeamPortals = *cspacec::iPortal_HitBeamPortals;
*GetVerticesCount = *cspacec::iPortal_GetVerticesCount;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iPortal($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iPortal_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iPortalContainer ##############

package cspace::iPortalContainer;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetPortalCount = *cspacec::iPortalContainer_GetPortalCount;
*GetPortal = *cspacec::iPortalContainer_GetPortal;
*CreatePortal = *cspacec::iPortalContainer_CreatePortal;
*RemovePortal = *cspacec::iPortalContainer_RemovePortal;
*Draw = *cspacec::iPortalContainer_Draw;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iPortalContainer($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iPortalContainer_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iGeneralMeshSubMesh ##############

package cspace::iGeneralMeshSubMesh;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetIndices = *cspacec::iGeneralMeshSubMesh_GetIndices;
*GetMaterial = *cspacec::iGeneralMeshSubMesh_GetMaterial;
*GetName = *cspacec::iGeneralMeshSubMesh_GetName;
*GetMixmode = *cspacec::iGeneralMeshSubMesh_GetMixmode;
*SetMaterial = *cspacec::iGeneralMeshSubMesh_SetMaterial;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iGeneralMeshSubMesh($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iGeneralMeshCommonState ##############

package cspace::iGeneralMeshCommonState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetLighting = *cspacec::iGeneralMeshCommonState_SetLighting;
*IsLighting = *cspacec::iGeneralMeshCommonState_IsLighting;
*SetManualColors = *cspacec::iGeneralMeshCommonState_SetManualColors;
*IsManualColors = *cspacec::iGeneralMeshCommonState_IsManualColors;
*SetShadowCasting = *cspacec::iGeneralMeshCommonState_SetShadowCasting;
*IsShadowCasting = *cspacec::iGeneralMeshCommonState_IsShadowCasting;
*SetShadowReceiving = *cspacec::iGeneralMeshCommonState_SetShadowReceiving;
*IsShadowReceiving = *cspacec::iGeneralMeshCommonState_IsShadowReceiving;
*AddRenderBuffer = *cspacec::iGeneralMeshCommonState_AddRenderBuffer;
*RemoveRenderBuffer = *cspacec::iGeneralMeshCommonState_RemoveRenderBuffer;
*GetRenderBufferCount = *cspacec::iGeneralMeshCommonState_GetRenderBufferCount;
*GetRenderBuffer = *cspacec::iGeneralMeshCommonState_GetRenderBuffer;
*GetRenderBufferName = *cspacec::iGeneralMeshCommonState_GetRenderBufferName;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iGeneralMeshCommonState($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iGeneralMeshState ##############

package cspace::iGeneralMeshState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iGeneralMeshCommonState cspace );
%OWNER = ();
%ITERATORS = ();
*SetAnimationControl = *cspacec::iGeneralMeshState_SetAnimationControl;
*GetAnimationControl = *cspacec::iGeneralMeshState_GetAnimationControl;
*FindSubMesh = *cspacec::iGeneralMeshState_FindSubMesh;
*AddSubMesh = *cspacec::iGeneralMeshState_AddSubMesh;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iGeneralMeshState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iGeneralMeshState_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iGeneralFactoryState ##############

package cspace::iGeneralFactoryState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iGeneralMeshCommonState cspace );
%OWNER = ();
%ITERATORS = ();
*SetColor = *cspacec::iGeneralFactoryState_SetColor;
*GetColor = *cspacec::iGeneralFactoryState_GetColor;
*AddVertex = *cspacec::iGeneralFactoryState_AddVertex;
*SetVertexCount = *cspacec::iGeneralFactoryState_SetVertexCount;
*GetVertexCount = *cspacec::iGeneralFactoryState_GetVertexCount;
*AddTriangle = *cspacec::iGeneralFactoryState_AddTriangle;
*SetTriangleCount = *cspacec::iGeneralFactoryState_SetTriangleCount;
*GetTriangleCount = *cspacec::iGeneralFactoryState_GetTriangleCount;
*Invalidate = *cspacec::iGeneralFactoryState_Invalidate;
*CalculateNormals = *cspacec::iGeneralFactoryState_CalculateNormals;
*Compress = *cspacec::iGeneralFactoryState_Compress;
*GenerateBox = *cspacec::iGeneralFactoryState_GenerateBox;
*GenerateSphere = *cspacec::iGeneralFactoryState_GenerateSphere;
*SetBack2Front = *cspacec::iGeneralFactoryState_SetBack2Front;
*IsAutoNormals = *cspacec::iGeneralFactoryState_IsAutoNormals;
*IsBack2Front = *cspacec::iGeneralFactoryState_IsBack2Front;
*SetAnimationControlFactory = *cspacec::iGeneralFactoryState_SetAnimationControlFactory;
*GetAnimationControlFactory = *cspacec::iGeneralFactoryState_GetAnimationControlFactory;
*ClearSubMeshes = *cspacec::iGeneralFactoryState_ClearSubMeshes;
*AddSubMesh = *cspacec::iGeneralFactoryState_AddSubMesh;
*FindSubMesh = *cspacec::iGeneralFactoryState_FindSubMesh;
*DeleteSubMesh = *cspacec::iGeneralFactoryState_DeleteSubMesh;
*GetSubMeshCount = *cspacec::iGeneralFactoryState_GetSubMeshCount;
*GetSubMesh = *cspacec::iGeneralFactoryState_GetSubMesh;
*DisableAutoNormals = *cspacec::iGeneralFactoryState_DisableAutoNormals;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iGeneralFactoryState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iGeneralFactoryState_scfGetVersion;
*GetVertexByIndex = *cspacec::iGeneralFactoryState_GetVertexByIndex;
*GetTexelByIndex = *cspacec::iGeneralFactoryState_GetTexelByIndex;
*GetNormalByIndex = *cspacec::iGeneralFactoryState_GetNormalByIndex;
*GetTriangleByIndex = *cspacec::iGeneralFactoryState_GetTriangleByIndex;
*GetColorByIndex = *cspacec::iGeneralFactoryState_GetColorByIndex;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iGenMeshAnimationControl ##############

package cspace::iGenMeshAnimationControl;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*AnimatesVertices = *cspacec::iGenMeshAnimationControl_AnimatesVertices;
*AnimatesTexels = *cspacec::iGenMeshAnimationControl_AnimatesTexels;
*AnimatesNormals = *cspacec::iGenMeshAnimationControl_AnimatesNormals;
*AnimatesColors = *cspacec::iGenMeshAnimationControl_AnimatesColors;
*Update = *cspacec::iGenMeshAnimationControl_Update;
*UpdateVertices = *cspacec::iGenMeshAnimationControl_UpdateVertices;
*UpdateTexels = *cspacec::iGenMeshAnimationControl_UpdateTexels;
*UpdateNormals = *cspacec::iGenMeshAnimationControl_UpdateNormals;
*UpdateColors = *cspacec::iGenMeshAnimationControl_UpdateColors;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iGenMeshAnimationControl($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iGenMeshAnimationControlFactory ##############

package cspace::iGenMeshAnimationControlFactory;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*CreateAnimationControl = *cspacec::iGenMeshAnimationControlFactory_CreateAnimationControl;
*Load = *cspacec::iGenMeshAnimationControlFactory_Load;
*Save = *cspacec::iGenMeshAnimationControlFactory_Save;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iGenMeshAnimationControlFactory($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iGenMeshAnimationControlType ##############

package cspace::iGenMeshAnimationControlType;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*CreateAnimationControlFactory = *cspacec::iGenMeshAnimationControlType_CreateAnimationControlFactory;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iGenMeshAnimationControlType($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeletonBone ##############

package cspace::iSkeletonBone;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetName = *cspacec::iSkeletonBone_GetName;
*SetName = *cspacec::iSkeletonBone_SetName;
*GetTransform = *cspacec::iSkeletonBone_GetTransform;
*SetTransform = *cspacec::iSkeletonBone_SetTransform;
*GetFullTransform = *cspacec::iSkeletonBone_GetFullTransform;
*SetParent = *cspacec::iSkeletonBone_SetParent;
*GetParent = *cspacec::iSkeletonBone_GetParent;
*GetChildrenCount = *cspacec::iSkeletonBone_GetChildrenCount;
*GetChild = *cspacec::iSkeletonBone_GetChild;
*FindChild = *cspacec::iSkeletonBone_FindChild;
*FindChildIndex = *cspacec::iSkeletonBone_FindChildIndex;
*SetSkinBox = *cspacec::iSkeletonBone_SetSkinBox;
*GetSkinBox = *cspacec::iSkeletonBone_GetSkinBox;
*SetUpdateCallback = *cspacec::iSkeletonBone_SetUpdateCallback;
*GetUpdateCallback = *cspacec::iSkeletonBone_GetUpdateCallback;
*GetFactory = *cspacec::iSkeletonBone_GetFactory;
*SetTransformMode = *cspacec::iSkeletonBone_SetTransformMode;
*GetTransformMode = *cspacec::iSkeletonBone_GetTransformMode;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeletonBone($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSkeletonBone_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeletonBoneUpdateCallback ##############

package cspace::iSkeletonBoneUpdateCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*UpdateTransform = *cspacec::iSkeletonBoneUpdateCallback_UpdateTransform;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeletonBoneUpdateCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeletonAnimationKeyFrame ##############

package cspace::iSkeletonAnimationKeyFrame;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetName = *cspacec::iSkeletonAnimationKeyFrame_GetName;
*SetName = *cspacec::iSkeletonAnimationKeyFrame_SetName;
*GetDuration = *cspacec::iSkeletonAnimationKeyFrame_GetDuration;
*SetDuration = *cspacec::iSkeletonAnimationKeyFrame_SetDuration;
*GetTransformsCount = *cspacec::iSkeletonAnimationKeyFrame_GetTransformsCount;
*AddTransform = *cspacec::iSkeletonAnimationKeyFrame_AddTransform;
*GetTransform = *cspacec::iSkeletonAnimationKeyFrame_GetTransform;
*SetTransform = *cspacec::iSkeletonAnimationKeyFrame_SetTransform;
*GetKeyFrameData = *cspacec::iSkeletonAnimationKeyFrame_GetKeyFrameData;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeletonAnimationKeyFrame($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeletonAnimation ##############

package cspace::iSkeletonAnimation;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetName = *cspacec::iSkeletonAnimation_GetName;
*SetName = *cspacec::iSkeletonAnimation_SetName;
*GetTime = *cspacec::iSkeletonAnimation_GetTime;
*SetTime = *cspacec::iSkeletonAnimation_SetTime;
*GetSpeed = *cspacec::iSkeletonAnimation_GetSpeed;
*SetSpeed = *cspacec::iSkeletonAnimation_SetSpeed;
*SetFactor = *cspacec::iSkeletonAnimation_SetFactor;
*GetFactor = *cspacec::iSkeletonAnimation_GetFactor;
*SetLoop = *cspacec::iSkeletonAnimation_SetLoop;
*GetLoop = *cspacec::iSkeletonAnimation_GetLoop;
*CreateFrame = *cspacec::iSkeletonAnimation_CreateFrame;
*GetFramesCount = *cspacec::iSkeletonAnimation_GetFramesCount;
*GetFrame = *cspacec::iSkeletonAnimation_GetFrame;
*FindFrameIndex = *cspacec::iSkeletonAnimation_FindFrameIndex;
*RemoveFrame = *cspacec::iSkeletonAnimation_RemoveFrame;
*RemoveAllFrames = *cspacec::iSkeletonAnimation_RemoveAllFrames;
*RecalcSpline = *cspacec::iSkeletonAnimation_RecalcSpline;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeletonAnimation($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeletonAnimationCallback ##############

package cspace::iSkeletonAnimationCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Execute = *cspacec::iSkeletonAnimationCallback_Execute;
*OnFinish = *cspacec::iSkeletonAnimationCallback_OnFinish;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeletonAnimationCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeletonUpdateCallback ##############

package cspace::iSkeletonUpdateCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Execute = *cspacec::iSkeletonUpdateCallback_Execute;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeletonUpdateCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeletonAnimationInstance ##############

package cspace::iSkeletonAnimationInstance;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetSpeed = *cspacec::iSkeletonAnimationInstance_GetSpeed;
*SetSpeed = *cspacec::iSkeletonAnimationInstance_SetSpeed;
*SetFactor = *cspacec::iSkeletonAnimationInstance_SetFactor;
*GetFactor = *cspacec::iSkeletonAnimationInstance_GetFactor;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeletonAnimationInstance($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeleton ##############

package cspace::iSkeleton;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetName = *cspacec::iSkeleton_GetName;
*SetName = *cspacec::iSkeleton_SetName;
*GetBonesCount = *cspacec::iSkeleton_GetBonesCount;
*GetBone = *cspacec::iSkeleton_GetBone;
*FindBone = *cspacec::iSkeleton_FindBone;
*FindBoneIndex = *cspacec::iSkeleton_FindBoneIndex;
*Execute = *cspacec::iSkeleton_Execute;
*Append = *cspacec::iSkeleton_Append;
*Play = *cspacec::iSkeleton_Play;
*ClearPendingAnimations = *cspacec::iSkeleton_ClearPendingAnimations;
*ClearPendingScripts = *cspacec::iSkeleton_ClearPendingScripts;
*GetAnimationsCount = *cspacec::iSkeleton_GetAnimationsCount;
*GetScriptsCount = *cspacec::iSkeleton_GetScriptsCount;
*GetAnimation = *cspacec::iSkeleton_GetAnimation;
*GetScript = *cspacec::iSkeleton_GetScript;
*FindAnimation = *cspacec::iSkeleton_FindAnimation;
*FindScript = *cspacec::iSkeleton_FindScript;
*FindSocket = *cspacec::iSkeleton_FindSocket;
*StopAll = *cspacec::iSkeleton_StopAll;
*Stop = *cspacec::iSkeleton_Stop;
*GetFactory = *cspacec::iSkeleton_GetFactory;
*SetScriptCallback = *cspacec::iSkeleton_SetScriptCallback;
*SetAnimationCallback = *cspacec::iSkeleton_SetAnimationCallback;
*AddUpdateCallback = *cspacec::iSkeleton_AddUpdateCallback;
*GetUpdateCallbacksCount = *cspacec::iSkeleton_GetUpdateCallbacksCount;
*GetUpdateCallback = *cspacec::iSkeleton_GetUpdateCallback;
*RemoveUpdateCallback = *cspacec::iSkeleton_RemoveUpdateCallback;
*UpdateAnimation = *cspacec::iSkeleton_UpdateAnimation;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeleton($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSkeleton_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeletonSocket ##############

package cspace::iSkeletonSocket;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetName = *cspacec::iSkeletonSocket_GetName;
*SetName = *cspacec::iSkeletonSocket_SetName;
*GetTransform = *cspacec::iSkeletonSocket_GetTransform;
*SetTransform = *cspacec::iSkeletonSocket_SetTransform;
*GetFullTransform = *cspacec::iSkeletonSocket_GetFullTransform;
*SetBone = *cspacec::iSkeletonSocket_SetBone;
*GetBone = *cspacec::iSkeletonSocket_GetBone;
*SetSceneNode = *cspacec::iSkeletonSocket_SetSceneNode;
*GetSceneNode = *cspacec::iSkeletonSocket_GetSceneNode;
*GetFactory = *cspacec::iSkeletonSocket_GetFactory;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeletonSocket($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSkeletonSocket_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeletonBoneRagdollInfo ##############

package cspace::iSkeletonBoneRagdollInfo;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetEnabled = *cspacec::iSkeletonBoneRagdollInfo_SetEnabled;
*GetEnabled = *cspacec::iSkeletonBoneRagdollInfo_GetEnabled;
*SetAttachToParent = *cspacec::iSkeletonBoneRagdollInfo_SetAttachToParent;
*GetAttachToParent = *cspacec::iSkeletonBoneRagdollInfo_GetAttachToParent;
*SetGeomName = *cspacec::iSkeletonBoneRagdollInfo_SetGeomName;
*GetGeomName = *cspacec::iSkeletonBoneRagdollInfo_GetGeomName;
*SetGeomType = *cspacec::iSkeletonBoneRagdollInfo_SetGeomType;
*GetGeomType = *cspacec::iSkeletonBoneRagdollInfo_GetGeomType;
*SetGeomDimensions = *cspacec::iSkeletonBoneRagdollInfo_SetGeomDimensions;
*GetGeomDimensions = *cspacec::iSkeletonBoneRagdollInfo_GetGeomDimensions;
*SetFriction = *cspacec::iSkeletonBoneRagdollInfo_SetFriction;
*GetFriction = *cspacec::iSkeletonBoneRagdollInfo_GetFriction;
*SetElasticity = *cspacec::iSkeletonBoneRagdollInfo_SetElasticity;
*GetElasticity = *cspacec::iSkeletonBoneRagdollInfo_GetElasticity;
*SetSoftness = *cspacec::iSkeletonBoneRagdollInfo_SetSoftness;
*GetSoftness = *cspacec::iSkeletonBoneRagdollInfo_GetSoftness;
*SetSlip = *cspacec::iSkeletonBoneRagdollInfo_SetSlip;
*GetSlip = *cspacec::iSkeletonBoneRagdollInfo_GetSlip;
*SetBodyName = *cspacec::iSkeletonBoneRagdollInfo_SetBodyName;
*GetBodyName = *cspacec::iSkeletonBoneRagdollInfo_GetBodyName;
*SetBodyMass = *cspacec::iSkeletonBoneRagdollInfo_SetBodyMass;
*GetBodyMass = *cspacec::iSkeletonBoneRagdollInfo_GetBodyMass;
*SetBodyGravmode = *cspacec::iSkeletonBoneRagdollInfo_SetBodyGravmode;
*GetBodyGravmode = *cspacec::iSkeletonBoneRagdollInfo_GetBodyGravmode;
*SetJointName = *cspacec::iSkeletonBoneRagdollInfo_SetJointName;
*GetJointName = *cspacec::iSkeletonBoneRagdollInfo_GetJointName;
*SetJointMinRotContraints = *cspacec::iSkeletonBoneRagdollInfo_SetJointMinRotContraints;
*GetJointMinRotContraints = *cspacec::iSkeletonBoneRagdollInfo_GetJointMinRotContraints;
*SetJointMaxRotContraints = *cspacec::iSkeletonBoneRagdollInfo_SetJointMaxRotContraints;
*GetJointMaxRotContraints = *cspacec::iSkeletonBoneRagdollInfo_GetJointMaxRotContraints;
*SetJointMinTransContraints = *cspacec::iSkeletonBoneRagdollInfo_SetJointMinTransContraints;
*GetJointMinTransContraints = *cspacec::iSkeletonBoneRagdollInfo_GetJointMinTransContraints;
*SetJointMaxTransContraints = *cspacec::iSkeletonBoneRagdollInfo_SetJointMaxTransContraints;
*GetJointMaxTransContraints = *cspacec::iSkeletonBoneRagdollInfo_GetJointMaxTransContraints;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeletonBoneRagdollInfo($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSkeletonBoneRagdollInfo_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeletonBoneFactory ##############

package cspace::iSkeletonBoneFactory;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetName = *cspacec::iSkeletonBoneFactory_GetName;
*SetName = *cspacec::iSkeletonBoneFactory_SetName;
*GetTransform = *cspacec::iSkeletonBoneFactory_GetTransform;
*SetTransform = *cspacec::iSkeletonBoneFactory_SetTransform;
*GetFullTransform = *cspacec::iSkeletonBoneFactory_GetFullTransform;
*SetParent = *cspacec::iSkeletonBoneFactory_SetParent;
*GetParent = *cspacec::iSkeletonBoneFactory_GetParent;
*GetChildrenCount = *cspacec::iSkeletonBoneFactory_GetChildrenCount;
*GetChild = *cspacec::iSkeletonBoneFactory_GetChild;
*FindChild = *cspacec::iSkeletonBoneFactory_FindChild;
*FindChildIndex = *cspacec::iSkeletonBoneFactory_FindChildIndex;
*SetSkinBox = *cspacec::iSkeletonBoneFactory_SetSkinBox;
*GetSkinBox = *cspacec::iSkeletonBoneFactory_GetSkinBox;
*GetRagdollInfo = *cspacec::iSkeletonBoneFactory_GetRagdollInfo;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeletonBoneFactory($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSkeletonBoneFactory_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeletonSocketFactory ##############

package cspace::iSkeletonSocketFactory;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetName = *cspacec::iSkeletonSocketFactory_GetName;
*SetName = *cspacec::iSkeletonSocketFactory_SetName;
*GetTransform = *cspacec::iSkeletonSocketFactory_GetTransform;
*SetTransform = *cspacec::iSkeletonSocketFactory_SetTransform;
*GetFullTransform = *cspacec::iSkeletonSocketFactory_GetFullTransform;
*SetBone = *cspacec::iSkeletonSocketFactory_SetBone;
*GetBone = *cspacec::iSkeletonSocketFactory_GetBone;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeletonSocketFactory($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSkeletonSocketFactory_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeletonFactory ##############

package cspace::iSkeletonFactory;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetName = *cspacec::iSkeletonFactory_GetName;
*SetName = *cspacec::iSkeletonFactory_SetName;
*CreateBone = *cspacec::iSkeletonFactory_CreateBone;
*CreateAnimation = *cspacec::iSkeletonFactory_CreateAnimation;
*CreateScript = *cspacec::iSkeletonFactory_CreateScript;
*FindAnimation = *cspacec::iSkeletonFactory_FindAnimation;
*GetAnimationsCount = *cspacec::iSkeletonFactory_GetAnimationsCount;
*GetAnimation = *cspacec::iSkeletonFactory_GetAnimation;
*FindScript = *cspacec::iSkeletonFactory_FindScript;
*FindBone = *cspacec::iSkeletonFactory_FindBone;
*FindBoneIndex = *cspacec::iSkeletonFactory_FindBoneIndex;
*GetBonesCount = *cspacec::iSkeletonFactory_GetBonesCount;
*GetBone = *cspacec::iSkeletonFactory_GetBone;
*GetGraveyard = *cspacec::iSkeletonFactory_GetGraveyard;
*CreateSocket = *cspacec::iSkeletonFactory_CreateSocket;
*FindSocket = *cspacec::iSkeletonFactory_FindSocket;
*GetSocket = *cspacec::iSkeletonFactory_GetSocket;
*RemoveSocket = *cspacec::iSkeletonFactory_RemoveSocket;
*GetSocketsCount = *cspacec::iSkeletonFactory_GetSocketsCount;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeletonFactory($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSkeletonFactory_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSkeletonGraveyard ##############

package cspace::iSkeletonGraveyard;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetFactoriesCount = *cspacec::iSkeletonGraveyard_GetFactoriesCount;
*CreateFactory = *cspacec::iSkeletonGraveyard_CreateFactory;
*LoadFactory = *cspacec::iSkeletonGraveyard_LoadFactory;
*FindFactory = *cspacec::iSkeletonGraveyard_FindFactory;
*CreateSkeleton = *cspacec::iSkeletonGraveyard_CreateSkeleton;
*SetManualUpdates = *cspacec::iSkeletonGraveyard_SetManualUpdates;
*Update = *cspacec::iSkeletonGraveyard_Update;
*AddSkeleton = *cspacec::iSkeletonGraveyard_AddSkeleton;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSkeletonGraveyard($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSkeletonGraveyard_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iGenMeshSkeletonControlState ##############

package cspace::iGenMeshSkeletonControlState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetAnimatedVertices = *cspacec::iGenMeshSkeletonControlState_GetAnimatedVertices;
*GetAnimatedVerticesCount = *cspacec::iGenMeshSkeletonControlState_GetAnimatedVerticesCount;
*GetAnimatedFaceNormals = *cspacec::iGenMeshSkeletonControlState_GetAnimatedFaceNormals;
*GetAnimatedFaceNormalsCount = *cspacec::iGenMeshSkeletonControlState_GetAnimatedFaceNormalsCount;
*GetAnimatedVertNormals = *cspacec::iGenMeshSkeletonControlState_GetAnimatedVertNormals;
*GetAnimatedVertNormalsCount = *cspacec::iGenMeshSkeletonControlState_GetAnimatedVertNormalsCount;
*GetSkeleton = *cspacec::iGenMeshSkeletonControlState_GetSkeleton;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iGenMeshSkeletonControlState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iGenMeshSkeletonControlState_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSprite2DVertexArrayReadOnly ##############

package cspace::csSprite2DVertexArrayReadOnly;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetSize = *cspacec::csSprite2DVertexArrayReadOnly_GetSize;
*Get = *cspacec::csSprite2DVertexArrayReadOnly_Get;
*Top = *cspacec::csSprite2DVertexArrayReadOnly_Top;
*Find = *cspacec::csSprite2DVertexArrayReadOnly_Find;
*GetIndex = *cspacec::csSprite2DVertexArrayReadOnly_GetIndex;
*IsEmpty = *cspacec::csSprite2DVertexArrayReadOnly_IsEmpty;
*GetAll = *cspacec::csSprite2DVertexArrayReadOnly_GetAll;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSprite2DVertexArrayReadOnly($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSprite2DVertexArrayChangeElements ##############

package cspace::csSprite2DVertexArrayChangeElements;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csSprite2DVertexArrayReadOnly cspace );
%OWNER = ();
%ITERATORS = ();
*Get = *cspacec::csSprite2DVertexArrayChangeElements_Get;
*Top = *cspacec::csSprite2DVertexArrayChangeElements_Top;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSprite2DVertexArrayChangeElements($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSprite2DVertexArrayChangeAll ##############

package cspace::csSprite2DVertexArrayChangeAll;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csSprite2DVertexArrayChangeElements cspace );
%OWNER = ();
%ITERATORS = ();
*SetSize = *cspacec::csSprite2DVertexArrayChangeAll_SetSize;
*GetExtend = *cspacec::csSprite2DVertexArrayChangeAll_GetExtend;
*Put = *cspacec::csSprite2DVertexArrayChangeAll_Put;
*Push = *cspacec::csSprite2DVertexArrayChangeAll_Push;
*PushSmart = *cspacec::csSprite2DVertexArrayChangeAll_PushSmart;
*Pop = *cspacec::csSprite2DVertexArrayChangeAll_Pop;
*Insert = *cspacec::csSprite2DVertexArrayChangeAll_Insert;
*DeleteAll = *cspacec::csSprite2DVertexArrayChangeAll_DeleteAll;
*Truncate = *cspacec::csSprite2DVertexArrayChangeAll_Truncate;
*Empty = *cspacec::csSprite2DVertexArrayChangeAll_Empty;
*DeleteIndex = *cspacec::csSprite2DVertexArrayChangeAll_DeleteIndex;
*DeleteIndexFast = *cspacec::csSprite2DVertexArrayChangeAll_DeleteIndexFast;
*Delete = *cspacec::csSprite2DVertexArrayChangeAll_Delete;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSprite2DVertexArrayChangeAll($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSprite2DVertex ##############

package cspace::csSprite2DVertex;
use overload
    "==" => sub { $_[0]->__eq__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_pos_get = *cspacec::csSprite2DVertex_pos_get;
*swig_pos_set = *cspacec::csSprite2DVertex_pos_set;
*swig_color_init_get = *cspacec::csSprite2DVertex_color_init_get;
*swig_color_init_set = *cspacec::csSprite2DVertex_color_init_set;
*swig_color_get = *cspacec::csSprite2DVertex_color_get;
*swig_color_set = *cspacec::csSprite2DVertex_color_set;
*swig_u_get = *cspacec::csSprite2DVertex_u_get;
*swig_u_set = *cspacec::csSprite2DVertex_u_set;
*swig_v_get = *cspacec::csSprite2DVertex_v_get;
*swig_v_set = *cspacec::csSprite2DVertex_v_set;
*__eq__ = *cspacec::csSprite2DVertex___eq__;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csSprite2DVertex(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSprite2DVertex($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iColoredVertices ##############

package cspace::iColoredVertices;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csSprite2DVertexArrayChangeAll cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iColoredVertices($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSprite2DUVAnimationFrame ##############

package cspace::iSprite2DUVAnimationFrame;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetName = *cspacec::iSprite2DUVAnimationFrame_SetName;
*GetName = *cspacec::iSprite2DUVAnimationFrame_GetName;
*GetUVCoo = *cspacec::iSprite2DUVAnimationFrame_GetUVCoo;
*GetUVCount = *cspacec::iSprite2DUVAnimationFrame_GetUVCount;
*SetUV = *cspacec::iSprite2DUVAnimationFrame_SetUV;
*SetFrameData = *cspacec::iSprite2DUVAnimationFrame_SetFrameData;
*RemoveUV = *cspacec::iSprite2DUVAnimationFrame_RemoveUV;
*GetDuration = *cspacec::iSprite2DUVAnimationFrame_GetDuration;
*SetDuration = *cspacec::iSprite2DUVAnimationFrame_SetDuration;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSprite2DUVAnimationFrame($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSprite2DUVAnimation ##############

package cspace::iSprite2DUVAnimation;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetName = *cspacec::iSprite2DUVAnimation_SetName;
*GetName = *cspacec::iSprite2DUVAnimation_GetName;
*GetFrameCount = *cspacec::iSprite2DUVAnimation_GetFrameCount;
*GetFrame = *cspacec::iSprite2DUVAnimation_GetFrame;
*CreateFrame = *cspacec::iSprite2DUVAnimation_CreateFrame;
*MoveFrame = *cspacec::iSprite2DUVAnimation_MoveFrame;
*RemoveFrame = *cspacec::iSprite2DUVAnimation_RemoveFrame;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSprite2DUVAnimation($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSprite2DFactoryState ##############

package cspace::iSprite2DFactoryState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetLighting = *cspacec::iSprite2DFactoryState_SetLighting;
*HasLighting = *cspacec::iSprite2DFactoryState_HasLighting;
*GetUVAnimationCount = *cspacec::iSprite2DFactoryState_GetUVAnimationCount;
*CreateUVAnimation = *cspacec::iSprite2DFactoryState_CreateUVAnimation;
*RemoveUVAnimation = *cspacec::iSprite2DFactoryState_RemoveUVAnimation;
*GetUVAnimation = *cspacec::iSprite2DFactoryState_GetUVAnimation;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSprite2DFactoryState($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSprite2DState ##############

package cspace::iSprite2DState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iSprite2DFactoryState cspace );
%OWNER = ();
%ITERATORS = ();
*CreateRegularVertices = *cspacec::iSprite2DState_CreateRegularVertices;
*SetUVAnimation = *cspacec::iSprite2DState_SetUVAnimation;
*GetUVAnimation = *cspacec::iSprite2DState_GetUVAnimation;
*StopUVAnimation = *cspacec::iSprite2DState_StopUVAnimation;
*PlayUVAnimation = *cspacec::iSprite2DState_PlayUVAnimation;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSprite2DState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSprite2DState_scfGetVersion;
*GetVertexByIndex = *cspacec::iSprite2DState_GetVertexByIndex;
*GetVertexCount = *cspacec::iSprite2DState_GetVertexCount;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSpriteFrame ##############

package cspace::iSpriteFrame;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetName = *cspacec::iSpriteFrame_SetName;
*GetName = *cspacec::iSpriteFrame_GetName;
*GetAnmIndex = *cspacec::iSpriteFrame_GetAnmIndex;
*GetTexIndex = *cspacec::iSpriteFrame_GetTexIndex;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSpriteFrame($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSpriteAction ##############

package cspace::iSpriteAction;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetName = *cspacec::iSpriteAction_SetName;
*GetName = *cspacec::iSpriteAction_GetName;
*GetFrameCount = *cspacec::iSpriteAction_GetFrameCount;
*GetFrame = *cspacec::iSpriteAction_GetFrame;
*GetNextFrame = *cspacec::iSpriteAction_GetNextFrame;
*GetFrameDelay = *cspacec::iSpriteAction_GetFrameDelay;
*GetFrameDisplacement = *cspacec::iSpriteAction_GetFrameDisplacement;
*AddFrame = *cspacec::iSpriteAction_AddFrame;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSpriteAction($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSpriteSocket ##############

package cspace::iSpriteSocket;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetName = *cspacec::iSpriteSocket_SetName;
*GetName = *cspacec::iSpriteSocket_GetName;
*SetMeshWrapper = *cspacec::iSpriteSocket_SetMeshWrapper;
*GetMeshWrapper = *cspacec::iSpriteSocket_GetMeshWrapper;
*SetTriangleIndex = *cspacec::iSpriteSocket_SetTriangleIndex;
*GetTriangleIndex = *cspacec::iSpriteSocket_GetTriangleIndex;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSpriteSocket($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSprite3DFactoryState ##############

package cspace::iSprite3DFactoryState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*AddVertices = *cspacec::iSprite3DFactoryState_AddVertices;
*GetVertexCount = *cspacec::iSprite3DFactoryState_GetVertexCount;
*GetVertex = *cspacec::iSprite3DFactoryState_GetVertex;
*SetVertex = *cspacec::iSprite3DFactoryState_SetVertex;
*GetVertices = *cspacec::iSprite3DFactoryState_GetVertices;
*SetVertices = *cspacec::iSprite3DFactoryState_SetVertices;
*GetTexel = *cspacec::iSprite3DFactoryState_GetTexel;
*SetTexel = *cspacec::iSprite3DFactoryState_SetTexel;
*GetTexels = *cspacec::iSprite3DFactoryState_GetTexels;
*SetTexels = *cspacec::iSprite3DFactoryState_SetTexels;
*GetNormal = *cspacec::iSprite3DFactoryState_GetNormal;
*SetNormal = *cspacec::iSprite3DFactoryState_SetNormal;
*GetNormals = *cspacec::iSprite3DFactoryState_GetNormals;
*SetNormals = *cspacec::iSprite3DFactoryState_SetNormals;
*AddTriangle = *cspacec::iSprite3DFactoryState_AddTriangle;
*GetTriangle = *cspacec::iSprite3DFactoryState_GetTriangle;
*GetTriangles = *cspacec::iSprite3DFactoryState_GetTriangles;
*GetTriangleCount = *cspacec::iSprite3DFactoryState_GetTriangleCount;
*SetTriangleCount = *cspacec::iSprite3DFactoryState_SetTriangleCount;
*SetTriangles = *cspacec::iSprite3DFactoryState_SetTriangles;
*AddFrame = *cspacec::iSprite3DFactoryState_AddFrame;
*FindFrame = *cspacec::iSprite3DFactoryState_FindFrame;
*GetFrameCount = *cspacec::iSprite3DFactoryState_GetFrameCount;
*GetFrame = *cspacec::iSprite3DFactoryState_GetFrame;
*AddAction = *cspacec::iSprite3DFactoryState_AddAction;
*FindAction = *cspacec::iSprite3DFactoryState_FindAction;
*GetFirstAction = *cspacec::iSprite3DFactoryState_GetFirstAction;
*GetActionCount = *cspacec::iSprite3DFactoryState_GetActionCount;
*GetAction = *cspacec::iSprite3DFactoryState_GetAction;
*AddSocket = *cspacec::iSprite3DFactoryState_AddSocket;
*FindSocket = *cspacec::iSprite3DFactoryState_FindSocket;
*GetSocketCount = *cspacec::iSprite3DFactoryState_GetSocketCount;
*GetSocket = *cspacec::iSprite3DFactoryState_GetSocket;
*EnableTweening = *cspacec::iSprite3DFactoryState_EnableTweening;
*IsTweeningEnabled = *cspacec::iSprite3DFactoryState_IsTweeningEnabled;
*SetLightingQuality = *cspacec::iSprite3DFactoryState_SetLightingQuality;
*GetLightingQuality = *cspacec::iSprite3DFactoryState_GetLightingQuality;
*SetLightingQualityConfig = *cspacec::iSprite3DFactoryState_SetLightingQualityConfig;
*GetLightingQualityConfig = *cspacec::iSprite3DFactoryState_GetLightingQualityConfig;
*SetLodLevelConfig = *cspacec::iSprite3DFactoryState_SetLodLevelConfig;
*GetLodLevelConfig = *cspacec::iSprite3DFactoryState_GetLodLevelConfig;
*MergeNormals = *cspacec::iSprite3DFactoryState_MergeNormals;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSprite3DFactoryState($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSprite3DState ##############

package cspace::iSprite3DState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetMixMode = *cspacec::iSprite3DState_SetMixMode;
*GetMixMode = *cspacec::iSprite3DState_GetMixMode;
*SetLighting = *cspacec::iSprite3DState_SetLighting;
*IsLighting = *cspacec::iSprite3DState_IsLighting;
*SetFrame = *cspacec::iSprite3DState_SetFrame;
*GetCurFrame = *cspacec::iSprite3DState_GetCurFrame;
*GetFrameCount = *cspacec::iSprite3DState_GetFrameCount;
*SetAction = *cspacec::iSprite3DState_SetAction;
*SetReverseAction = *cspacec::iSprite3DState_SetReverseAction;
*SetSingleStepAction = *cspacec::iSprite3DState_SetSingleStepAction;
*SetOverrideAction = *cspacec::iSprite3DState_SetOverrideAction;
*PropagateAction = *cspacec::iSprite3DState_PropagateAction;
*GetCurAction = *cspacec::iSprite3DState_GetCurAction;
*GetReverseAction = *cspacec::iSprite3DState_GetReverseAction;
*EnableTweening = *cspacec::iSprite3DState_EnableTweening;
*IsTweeningEnabled = *cspacec::iSprite3DState_IsTweeningEnabled;
*UnsetTexture = *cspacec::iSprite3DState_UnsetTexture;
*GetLightingQuality = *cspacec::iSprite3DState_GetLightingQuality;
*SetLocalLightingQuality = *cspacec::iSprite3DState_SetLocalLightingQuality;
*SetLightingQualityConfig = *cspacec::iSprite3DState_SetLightingQualityConfig;
*GetLightingQualityConfig = *cspacec::iSprite3DState_GetLightingQualityConfig;
*SetLodLevelConfig = *cspacec::iSprite3DState_SetLodLevelConfig;
*GetLodLevelConfig = *cspacec::iSprite3DState_GetLodLevelConfig;
*IsLodEnabled = *cspacec::iSprite3DState_IsLodEnabled;
*FindSocket = *cspacec::iSprite3DState_FindSocket;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSprite3DState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSprite3DState_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSpriteCal3DSocket ##############

package cspace::iSpriteCal3DSocket;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iSpriteSocket cspace );
%OWNER = ();
%ITERATORS = ();
*SetSubmeshIndex = *cspacec::iSpriteCal3DSocket_SetSubmeshIndex;
*GetSubmeshIndex = *cspacec::iSpriteCal3DSocket_GetSubmeshIndex;
*SetMeshIndex = *cspacec::iSpriteCal3DSocket_SetMeshIndex;
*GetMeshIndex = *cspacec::iSpriteCal3DSocket_GetMeshIndex;
*SetTransform = *cspacec::iSpriteCal3DSocket_SetTransform;
*GetTransform = *cspacec::iSpriteCal3DSocket_GetTransform;
*GetSecondaryCount = *cspacec::iSpriteCal3DSocket_GetSecondaryCount;
*GetSecondaryMesh = *cspacec::iSpriteCal3DSocket_GetSecondaryMesh;
*GetSecondaryTransform = *cspacec::iSpriteCal3DSocket_GetSecondaryTransform;
*SetSecondaryTransform = *cspacec::iSpriteCal3DSocket_SetSecondaryTransform;
*AttachSecondary = *cspacec::iSpriteCal3DSocket_AttachSecondary;
*DetachSecondary = *cspacec::iSpriteCal3DSocket_DetachSecondary;
*FindSecondary = *cspacec::iSpriteCal3DSocket_FindSecondary;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSpriteCal3DSocket($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSpriteCal3DFactoryState ##############

package cspace::iSpriteCal3DFactoryState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Create = *cspacec::iSpriteCal3DFactoryState_Create;
*ReportLastError = *cspacec::iSpriteCal3DFactoryState_ReportLastError;
*SetLoadFlags = *cspacec::iSpriteCal3DFactoryState_SetLoadFlags;
*SetBasePath = *cspacec::iSpriteCal3DFactoryState_SetBasePath;
*LoadCoreSkeleton = *cspacec::iSpriteCal3DFactoryState_LoadCoreSkeleton;
*RescaleFactory = *cspacec::iSpriteCal3DFactoryState_RescaleFactory;
*LoadCoreAnimation = *cspacec::iSpriteCal3DFactoryState_LoadCoreAnimation;
*LoadCoreMesh = *cspacec::iSpriteCal3DFactoryState_LoadCoreMesh;
*LoadCoreMorphTarget = *cspacec::iSpriteCal3DFactoryState_LoadCoreMorphTarget;
*AddMorphAnimation = *cspacec::iSpriteCal3DFactoryState_AddMorphAnimation;
*AddMorphTarget = *cspacec::iSpriteCal3DFactoryState_AddMorphTarget;
*AddCoreMaterial = *cspacec::iSpriteCal3DFactoryState_AddCoreMaterial;
*CalculateAllBoneBoundingBoxes = *cspacec::iSpriteCal3DFactoryState_CalculateAllBoneBoundingBoxes;
*BindMaterials = *cspacec::iSpriteCal3DFactoryState_BindMaterials;
*GetMeshCount = *cspacec::iSpriteCal3DFactoryState_GetMeshCount;
*GetMorphAnimationCount = *cspacec::iSpriteCal3DFactoryState_GetMorphAnimationCount;
*GetMorphTargetCount = *cspacec::iSpriteCal3DFactoryState_GetMorphTargetCount;
*GetMeshName = *cspacec::iSpriteCal3DFactoryState_GetMeshName;
*FindMeshName = *cspacec::iSpriteCal3DFactoryState_FindMeshName;
*GetDefaultMaterial = *cspacec::iSpriteCal3DFactoryState_GetDefaultMaterial;
*GetMorphAnimationName = *cspacec::iSpriteCal3DFactoryState_GetMorphAnimationName;
*FindMorphAnimationName = *cspacec::iSpriteCal3DFactoryState_FindMorphAnimationName;
*IsMeshDefault = *cspacec::iSpriteCal3DFactoryState_IsMeshDefault;
*AddSocket = *cspacec::iSpriteCal3DFactoryState_AddSocket;
*FindSocket = *cspacec::iSpriteCal3DFactoryState_FindSocket;
*GetSocketCount = *cspacec::iSpriteCal3DFactoryState_GetSocketCount;
*GetSocket = *cspacec::iSpriteCal3DFactoryState_GetSocket;
*GetCal3DCoreModel = *cspacec::iSpriteCal3DFactoryState_GetCal3DCoreModel;
*RegisterAnimCallback = *cspacec::iSpriteCal3DFactoryState_RegisterAnimCallback;
*RemoveAnimCallback = *cspacec::iSpriteCal3DFactoryState_RemoveAnimCallback;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSpriteCal3DFactoryState($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iAnimTimeUpdateHandler ##############

package cspace::iAnimTimeUpdateHandler;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*UpdatePosition = *cspacec::iAnimTimeUpdateHandler_UpdatePosition;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iAnimTimeUpdateHandler($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSpriteCal3DActiveAnim ##############

package cspace::csSpriteCal3DActiveAnim;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_index_get = *cspacec::csSpriteCal3DActiveAnim_index_get;
*swig_index_set = *cspacec::csSpriteCal3DActiveAnim_index_set;
*swig_weight_get = *cspacec::csSpriteCal3DActiveAnim_weight_get;
*swig_weight_set = *cspacec::csSpriteCal3DActiveAnim_weight_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csSpriteCal3DActiveAnim(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSpriteCal3DActiveAnim($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSpriteCal3DState ##############

package cspace::iSpriteCal3DState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*C3D_ANIM_TYPE_NONE = *cspacec::iSpriteCal3DState_C3D_ANIM_TYPE_NONE;
*C3D_ANIM_TYPE_IDLE = *cspacec::iSpriteCal3DState_C3D_ANIM_TYPE_IDLE;
*C3D_ANIM_TYPE_TRAVEL = *cspacec::iSpriteCal3DState_C3D_ANIM_TYPE_TRAVEL;
*C3D_ANIM_TYPE_CYCLE = *cspacec::iSpriteCal3DState_C3D_ANIM_TYPE_CYCLE;
*C3D_ANIM_TYPE_STYLE_CYCLE = *cspacec::iSpriteCal3DState_C3D_ANIM_TYPE_STYLE_CYCLE;
*C3D_ANIM_TYPE_ACTION = *cspacec::iSpriteCal3DState_C3D_ANIM_TYPE_ACTION;
*GetAnimCount = *cspacec::iSpriteCal3DState_GetAnimCount;
*GetAnimName = *cspacec::iSpriteCal3DState_GetAnimName;
*GetAnimType = *cspacec::iSpriteCal3DState_GetAnimType;
*FindAnim = *cspacec::iSpriteCal3DState_FindAnim;
*ClearAllAnims = *cspacec::iSpriteCal3DState_ClearAllAnims;
*SetAnimCycle = *cspacec::iSpriteCal3DState_SetAnimCycle;
*AddAnimCycle = *cspacec::iSpriteCal3DState_AddAnimCycle;
*ClearAnimCycle = *cspacec::iSpriteCal3DState_ClearAnimCycle;
*GetActiveAnimCount = *cspacec::iSpriteCal3DState_GetActiveAnimCount;
*GetActiveAnims = *cspacec::iSpriteCal3DState_GetActiveAnims;
*SetActiveAnims = *cspacec::iSpriteCal3DState_SetActiveAnims;
*SetAnimAction = *cspacec::iSpriteCal3DState_SetAnimAction;
*SetVelocity = *cspacec::iSpriteCal3DState_SetVelocity;
*SetDefaultIdleAnim = *cspacec::iSpriteCal3DState_SetDefaultIdleAnim;
*SetLOD = *cspacec::iSpriteCal3DState_SetLOD;
*AttachCoreMesh = *cspacec::iSpriteCal3DState_AttachCoreMesh;
*DetachCoreMesh = *cspacec::iSpriteCal3DState_DetachCoreMesh;
*BlendMorphTarget = *cspacec::iSpriteCal3DState_BlendMorphTarget;
*ClearMorphTarget = *cspacec::iSpriteCal3DState_ClearMorphTarget;
*FindSocket = *cspacec::iSpriteCal3DState_FindSocket;
*SetMaterial = *cspacec::iSpriteCal3DState_SetMaterial;
*SetTimeFactor = *cspacec::iSpriteCal3DState_SetTimeFactor;
*GetTimeFactor = *cspacec::iSpriteCal3DState_GetTimeFactor;
*GetAnimationTime = *cspacec::iSpriteCal3DState_GetAnimationTime;
*GetAnimationDuration = *cspacec::iSpriteCal3DState_GetAnimationDuration;
*SetAnimationTime = *cspacec::iSpriteCal3DState_SetAnimationTime;
*SetAnimTimeUpdateHandler = *cspacec::iSpriteCal3DState_SetAnimTimeUpdateHandler;
*SetUserData = *cspacec::iSpriteCal3DState_SetUserData;
*GetCoreMeshShaderVarContext = *cspacec::iSpriteCal3DState_GetCoreMeshShaderVarContext;
*GetSkeleton = *cspacec::iSpriteCal3DState_GetSkeleton;
*GetCal3DModel = *cspacec::iSpriteCal3DState_GetCal3DModel;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSpriteCal3DState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSpriteCal3DState_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMeshObjectDrawCallback ##############

package cspace::iMeshObjectDrawCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*BeforeDrawing = *cspacec::iMeshObjectDrawCallback_BeforeDrawing;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMeshObjectDrawCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMeshObject ##############

package cspace::iMeshObject;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetFactory = *cspacec::iMeshObject_GetFactory;
*GetFlags = *cspacec::iMeshObject_GetFlags;
*Clone = *cspacec::iMeshObject_Clone;
*GetRenderMeshes = *cspacec::iMeshObject_GetRenderMeshes;
*SetVisibleCallback = *cspacec::iMeshObject_SetVisibleCallback;
*GetVisibleCallback = *cspacec::iMeshObject_GetVisibleCallback;
*NextFrame = *cspacec::iMeshObject_NextFrame;
*HardTransform = *cspacec::iMeshObject_HardTransform;
*SupportsHardTransform = *cspacec::iMeshObject_SupportsHardTransform;
*HitBeamOutline = *cspacec::iMeshObject_HitBeamOutline;
*HitBeamObject = *cspacec::iMeshObject_HitBeamObject;
*SetMeshWrapper = *cspacec::iMeshObject_SetMeshWrapper;
*GetMeshWrapper = *cspacec::iMeshObject_GetMeshWrapper;
*GetObjectModel = *cspacec::iMeshObject_GetObjectModel;
*SetColor = *cspacec::iMeshObject_SetColor;
*GetColor = *cspacec::iMeshObject_GetColor;
*SetMaterialWrapper = *cspacec::iMeshObject_SetMaterialWrapper;
*GetMaterialWrapper = *cspacec::iMeshObject_GetMaterialWrapper;
*SetMixMode = *cspacec::iMeshObject_SetMixMode;
*GetMixMode = *cspacec::iMeshObject_GetMixMode;
*InvalidateMaterialHandles = *cspacec::iMeshObject_InvalidateMaterialHandles;
*PositionChild = *cspacec::iMeshObject_PositionChild;
*BuildDecal = *cspacec::iMeshObject_BuildDecal;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMeshObject($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iMeshObject_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMeshObjectFactory ##############

package cspace::iMeshObjectFactory;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetFlags = *cspacec::iMeshObjectFactory_GetFlags;
*NewInstance = *cspacec::iMeshObjectFactory_NewInstance;
*Clone = *cspacec::iMeshObjectFactory_Clone;
*HardTransform = *cspacec::iMeshObjectFactory_HardTransform;
*SupportsHardTransform = *cspacec::iMeshObjectFactory_SupportsHardTransform;
*SetMeshFactoryWrapper = *cspacec::iMeshObjectFactory_SetMeshFactoryWrapper;
*GetMeshFactoryWrapper = *cspacec::iMeshObjectFactory_GetMeshFactoryWrapper;
*GetMeshObjectType = *cspacec::iMeshObjectFactory_GetMeshObjectType;
*GetObjectModel = *cspacec::iMeshObjectFactory_GetObjectModel;
*SetMaterialWrapper = *cspacec::iMeshObjectFactory_SetMaterialWrapper;
*GetMaterialWrapper = *cspacec::iMeshObjectFactory_GetMaterialWrapper;
*SetMixMode = *cspacec::iMeshObjectFactory_SetMixMode;
*GetMixMode = *cspacec::iMeshObjectFactory_GetMixMode;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMeshObjectFactory($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iMeshObjectFactory_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMeshObjectType ##############

package cspace::iMeshObjectType;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*NewFactory = *cspacec::iMeshObjectType_NewFactory;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMeshObjectType($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iMeshObjectType_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPolygonRange ##############

package cspace::csPolygonRange;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_start_get = *cspacec::csPolygonRange_start_get;
*swig_start_set = *cspacec::csPolygonRange_start_set;
*swig_end_get = *cspacec::csPolygonRange_end_get;
*swig_end_set = *cspacec::csPolygonRange_end_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPolygonRange(@_);
    bless $self, $pkg if defined($self);
}

*Set = *cspacec::csPolygonRange_Set;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPolygonRange($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iPolygonHandle ##############

package cspace::iPolygonHandle;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetThingFactoryState = *cspacec::iPolygonHandle_GetThingFactoryState;
*GetMeshObjectFactory = *cspacec::iPolygonHandle_GetMeshObjectFactory;
*GetThingState = *cspacec::iPolygonHandle_GetThingState;
*GetMeshObject = *cspacec::iPolygonHandle_GetMeshObject;
*GetIndex = *cspacec::iPolygonHandle_GetIndex;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iPolygonHandle($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iThingFactoryState ##############

package cspace::iThingFactoryState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*CompressVertices = *cspacec::iThingFactoryState_CompressVertices;
*GetPolygonCount = *cspacec::iThingFactoryState_GetPolygonCount;
*RemovePolygon = *cspacec::iThingFactoryState_RemovePolygon;
*RemovePolygons = *cspacec::iThingFactoryState_RemovePolygons;
*FindPolygonByName = *cspacec::iThingFactoryState_FindPolygonByName;
*AddEmptyPolygon = *cspacec::iThingFactoryState_AddEmptyPolygon;
*AddTriangle = *cspacec::iThingFactoryState_AddTriangle;
*AddQuad = *cspacec::iThingFactoryState_AddQuad;
*AddPolygon = *cspacec::iThingFactoryState_AddPolygon;
*AddOutsideBox = *cspacec::iThingFactoryState_AddOutsideBox;
*AddInsideBox = *cspacec::iThingFactoryState_AddInsideBox;
*SetPolygonName = *cspacec::iThingFactoryState_SetPolygonName;
*GetPolygonName = *cspacec::iThingFactoryState_GetPolygonName;
*CreatePolygonHandle = *cspacec::iThingFactoryState_CreatePolygonHandle;
*SetPolygonMaterial = *cspacec::iThingFactoryState_SetPolygonMaterial;
*GetPolygonMaterial = *cspacec::iThingFactoryState_GetPolygonMaterial;
*AddPolygonVertex = *cspacec::iThingFactoryState_AddPolygonVertex;
*SetPolygonVertexIndices = *cspacec::iThingFactoryState_SetPolygonVertexIndices;
*GetPolygonVertexCount = *cspacec::iThingFactoryState_GetPolygonVertexCount;
*GetPolygonVertex = *cspacec::iThingFactoryState_GetPolygonVertex;
*GetPolygonVertexIndices = *cspacec::iThingFactoryState_GetPolygonVertexIndices;
*SetPolygonTextureMapping = *cspacec::iThingFactoryState_SetPolygonTextureMapping;
*GetPolygonTextureMapping = *cspacec::iThingFactoryState_GetPolygonTextureMapping;
*SetPolygonTextureMappingEnabled = *cspacec::iThingFactoryState_SetPolygonTextureMappingEnabled;
*IsPolygonTextureMappingEnabled = *cspacec::iThingFactoryState_IsPolygonTextureMappingEnabled;
*SetPolygonFlags = *cspacec::iThingFactoryState_SetPolygonFlags;
*ResetPolygonFlags = *cspacec::iThingFactoryState_ResetPolygonFlags;
*GetPolygonFlags = *cspacec::iThingFactoryState_GetPolygonFlags;
*GetPolygonObjectPlane = *cspacec::iThingFactoryState_GetPolygonObjectPlane;
*IsPolygonTransparent = *cspacec::iThingFactoryState_IsPolygonTransparent;
*PointOnPolygon = *cspacec::iThingFactoryState_PointOnPolygon;
*GetVertexCount = *cspacec::iThingFactoryState_GetVertexCount;
*GetVertex = *cspacec::iThingFactoryState_GetVertex;
*GetVertices = *cspacec::iThingFactoryState_GetVertices;
*CreateVertex = *cspacec::iThingFactoryState_CreateVertex;
*SetVertex = *cspacec::iThingFactoryState_SetVertex;
*DeleteVertex = *cspacec::iThingFactoryState_DeleteVertex;
*DeleteVertices = *cspacec::iThingFactoryState_DeleteVertices;
*SetSmoothingFlag = *cspacec::iThingFactoryState_SetSmoothingFlag;
*GetSmoothingFlag = *cspacec::iThingFactoryState_GetSmoothingFlag;
*GetNormals = *cspacec::iThingFactoryState_GetNormals;
*GetCosinusFactor = *cspacec::iThingFactoryState_GetCosinusFactor;
*SetCosinusFactor = *cspacec::iThingFactoryState_SetCosinusFactor;
*AddPolygonRenderBuffer = *cspacec::iThingFactoryState_AddPolygonRenderBuffer;
*GetLightmapLayout = *cspacec::iThingFactoryState_GetLightmapLayout;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iThingFactoryState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iThingFactoryState_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iThingState ##############

package cspace::iThingState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetVertexW = *cspacec::iThingState_GetVertexW;
*GetVerticesW = *cspacec::iThingState_GetVerticesW;
*GetMovingOption = *cspacec::iThingState_GetMovingOption;
*SetMovingOption = *cspacec::iThingState_SetMovingOption;
*Prepare = *cspacec::iThingState_Prepare;
*Unprepare = *cspacec::iThingState_Unprepare;
*ReplaceMaterial = *cspacec::iThingState_ReplaceMaterial;
*ClearReplacedMaterials = *cspacec::iThingState_ClearReplacedMaterials;
*SetMixMode = *cspacec::iThingState_SetMixMode;
*GetMixMode = *cspacec::iThingState_GetMixMode;
*CreatePolygonHandle = *cspacec::iThingState_CreatePolygonHandle;
*GetPolygonWorldPlane = *cspacec::iThingState_GetPolygonWorldPlane;
*GetPolygonLightmap = *cspacec::iThingState_GetPolygonLightmap;
*GetPolygonPDLight = *cspacec::iThingState_GetPolygonPDLight;
*GetReplacedMaterial = *cspacec::iThingState_GetReplacedMaterial;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iThingState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iThingState_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iThingEnvironment ##############

package cspace::iThingEnvironment;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Clear = *cspacec::iThingEnvironment_Clear;
*GetLightmapCellSize = *cspacec::iThingEnvironment_GetLightmapCellSize;
*SetLightmapCellSize = *cspacec::iThingEnvironment_SetLightmapCellSize;
*GetDefaultLightmapCellSize = *cspacec::iThingEnvironment_GetDefaultLightmapCellSize;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iThingEnvironment($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csCharArrayArray ##############

package cspace::csCharArrayArray;
use overload
    "!=" => sub { $_[0]->__ne__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csCharArrayArray($self);
        delete $OWNER{$self};
    }
}

sub new {
    my $pkg = shift;
    my $self = cspacec::new_csCharArrayArray(@_);
    bless $self, $pkg if defined($self);
}

*GetSize = *cspacec::csCharArrayArray_GetSize;
*Get = *cspacec::csCharArrayArray_Get;
*Put = *cspacec::csCharArrayArray_Put;
*Push = *cspacec::csCharArrayArray_Push;
*Pop = *cspacec::csCharArrayArray_Pop;
*Top = *cspacec::csCharArrayArray_Top;
*Insert = *cspacec::csCharArrayArray_Insert;
*Contains = *cspacec::csCharArrayArray_Contains;
*Truncate = *cspacec::csCharArrayArray_Truncate;
*Empty = *cspacec::csCharArrayArray_Empty;
*IsEmpty = *cspacec::csCharArrayArray_IsEmpty;
*SetMinimalCapacity = *cspacec::csCharArrayArray_SetMinimalCapacity;
*DeleteIndex = *cspacec::csCharArrayArray_DeleteIndex;
*DeleteIndexFast = *cspacec::csCharArrayArray_DeleteIndexFast;
*DeleteRange = *cspacec::csCharArrayArray_DeleteRange;
*__eq__ = *cspacec::csCharArrayArray___eq__;
*__ne__ = *cspacec::csCharArrayArray___ne__;
*GetAllocator = *cspacec::csCharArrayArray_GetAllocator;
*Length = *cspacec::csCharArrayArray_Length;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iTerrainObjectState ##############

package cspace::iTerrainObjectState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetMaterialPalette = *cspacec::iTerrainObjectState_SetMaterialPalette;
*GetMaterialPalette = *cspacec::iTerrainObjectState_GetMaterialPalette;
*SetMaterialMap = *cspacec::iTerrainObjectState_SetMaterialMap;
*SetMaterialAlphaMaps = *cspacec::iTerrainObjectState_SetMaterialAlphaMaps;
*SetLODValue = *cspacec::iTerrainObjectState_SetLODValue;
*GetLODValue = *cspacec::iTerrainObjectState_GetLODValue;
*SaveState = *cspacec::iTerrainObjectState_SaveState;
*RestoreState = *cspacec::iTerrainObjectState_RestoreState;
*CollisionDetect = *cspacec::iTerrainObjectState_CollisionDetect;
*SetStaticLighting = *cspacec::iTerrainObjectState_SetStaticLighting;
*GetStaticLighting = *cspacec::iTerrainObjectState_GetStaticLighting;
*SetCastShadows = *cspacec::iTerrainObjectState_SetCastShadows;
*GetCastShadows = *cspacec::iTerrainObjectState_GetCastShadows;
*SetMaterialMapFile = *cspacec::iTerrainObjectState_SetMaterialMapFile;
*GetMaterialMapFile = *cspacec::iTerrainObjectState_GetMaterialMapFile;
*SetTopNeighbor = *cspacec::iTerrainObjectState_SetTopNeighbor;
*SetRightNeighbor = *cspacec::iTerrainObjectState_SetRightNeighbor;
*SetLeftNeighbor = *cspacec::iTerrainObjectState_SetLeftNeighbor;
*SetBottomNeighbor = *cspacec::iTerrainObjectState_SetBottomNeighbor;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iTerrainObjectState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iTerrainObjectState_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iTerrainFactoryState ##############

package cspace::iTerrainFactoryState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetTerraFormer = *cspacec::iTerrainFactoryState_SetTerraFormer;
*GetTerraFormer = *cspacec::iTerrainFactoryState_GetTerraFormer;
*SetSamplerRegion = *cspacec::iTerrainFactoryState_SetSamplerRegion;
*GetSamplerRegion = *cspacec::iTerrainFactoryState_GetSamplerRegion;
*SaveState = *cspacec::iTerrainFactoryState_SaveState;
*RestoreState = *cspacec::iTerrainFactoryState_RestoreState;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iTerrainFactoryState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iTerrainFactoryState_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csParticle ##############

package cspace::csParticle;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_position_get = *cspacec::csParticle_position_get;
*swig_position_set = *cspacec::csParticle_position_set;
*swig_mass_get = *cspacec::csParticle_mass_get;
*swig_mass_set = *cspacec::csParticle_mass_set;
*swig_orientation_get = *cspacec::csParticle_orientation_get;
*swig_orientation_set = *cspacec::csParticle_orientation_set;
*swig_linearVelocity_get = *cspacec::csParticle_linearVelocity_get;
*swig_linearVelocity_set = *cspacec::csParticle_linearVelocity_set;
*swig_timeToLive_get = *cspacec::csParticle_timeToLive_get;
*swig_timeToLive_set = *cspacec::csParticle_timeToLive_set;
*swig_angularVelocity_get = *cspacec::csParticle_angularVelocity_get;
*swig_angularVelocity_set = *cspacec::csParticle_angularVelocity_set;
*swig_pad_get = *cspacec::csParticle_pad_get;
*swig_pad_set = *cspacec::csParticle_pad_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csParticle(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csParticle($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csParticleAux ##############

package cspace::csParticleAux;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_color_get = *cspacec::csParticleAux_color_get;
*swig_color_set = *cspacec::csParticleAux_color_set;
*swig_particleSize_get = *cspacec::csParticleAux_particleSize_get;
*swig_particleSize_set = *cspacec::csParticleAux_particleSize_set;
*swig_pad_get = *cspacec::csParticleAux_pad_get;
*swig_pad_set = *cspacec::csParticleAux_pad_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csParticleAux(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csParticleAux($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csParticleBuffer ##############

package cspace::csParticleBuffer;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_particleData_get = *cspacec::csParticleBuffer_particleData_get;
*swig_particleData_set = *cspacec::csParticleBuffer_particleData_set;
*swig_particleAuxData_get = *cspacec::csParticleBuffer_particleAuxData_get;
*swig_particleAuxData_set = *cspacec::csParticleBuffer_particleAuxData_set;
*swig_particleCount_get = *cspacec::csParticleBuffer_particleCount_get;
*swig_particleCount_set = *cspacec::csParticleBuffer_particleCount_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csParticleBuffer(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csParticleBuffer($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleEmitter ##############

package cspace::iParticleEmitter;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetEnabled = *cspacec::iParticleEmitter_SetEnabled;
*GetEnabled = *cspacec::iParticleEmitter_GetEnabled;
*SetStartTime = *cspacec::iParticleEmitter_SetStartTime;
*GetStartTime = *cspacec::iParticleEmitter_GetStartTime;
*SetDuration = *cspacec::iParticleEmitter_SetDuration;
*GetDuration = *cspacec::iParticleEmitter_GetDuration;
*SetEmissionRate = *cspacec::iParticleEmitter_SetEmissionRate;
*GetEmissionRate = *cspacec::iParticleEmitter_GetEmissionRate;
*SetInitialTTL = *cspacec::iParticleEmitter_SetInitialTTL;
*GetInitialTTL = *cspacec::iParticleEmitter_GetInitialTTL;
*SetInitialMass = *cspacec::iParticleEmitter_SetInitialMass;
*GetInitialMass = *cspacec::iParticleEmitter_GetInitialMass;
*Clone = *cspacec::iParticleEmitter_Clone;
*ParticlesToEmit = *cspacec::iParticleEmitter_ParticlesToEmit;
*EmitParticles = *cspacec::iParticleEmitter_EmitParticles;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleEmitter($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleEmitter_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleEffector ##############

package cspace::iParticleEffector;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Clone = *cspacec::iParticleEffector_Clone;
*EffectParticles = *cspacec::iParticleEffector_EffectParticles;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleEffector($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleEffector_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleSystemBase ##############

package cspace::iParticleSystemBase;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetParticleRenderOrientation = *cspacec::iParticleSystemBase_SetParticleRenderOrientation;
*GetParticleRenderOrientation = *cspacec::iParticleSystemBase_GetParticleRenderOrientation;
*SetRotationMode = *cspacec::iParticleSystemBase_SetRotationMode;
*GetRotationMode = *cspacec::iParticleSystemBase_GetRotationMode;
*SetSortMode = *cspacec::iParticleSystemBase_SetSortMode;
*GetSortMode = *cspacec::iParticleSystemBase_GetSortMode;
*SetIntegrationMode = *cspacec::iParticleSystemBase_SetIntegrationMode;
*GetIntegrationMode = *cspacec::iParticleSystemBase_GetIntegrationMode;
*SetCommonDirection = *cspacec::iParticleSystemBase_SetCommonDirection;
*GetCommonDirection = *cspacec::iParticleSystemBase_GetCommonDirection;
*SetTransformMode = *cspacec::iParticleSystemBase_SetTransformMode;
*GetTransformMode = *cspacec::iParticleSystemBase_GetTransformMode;
*SetUseIndividualSize = *cspacec::iParticleSystemBase_SetUseIndividualSize;
*GetUseIndividualSize = *cspacec::iParticleSystemBase_GetUseIndividualSize;
*SetParticleSize = *cspacec::iParticleSystemBase_SetParticleSize;
*GetParticleSize = *cspacec::iParticleSystemBase_GetParticleSize;
*SetMinBoundingBox = *cspacec::iParticleSystemBase_SetMinBoundingBox;
*GetMinBoundingBox = *cspacec::iParticleSystemBase_GetMinBoundingBox;
*AddEmitter = *cspacec::iParticleSystemBase_AddEmitter;
*GetEmitter = *cspacec::iParticleSystemBase_GetEmitter;
*RemoveEmitter = *cspacec::iParticleSystemBase_RemoveEmitter;
*GetEmitterCount = *cspacec::iParticleSystemBase_GetEmitterCount;
*AddEffector = *cspacec::iParticleSystemBase_AddEffector;
*GetEffector = *cspacec::iParticleSystemBase_GetEffector;
*RemoveEffector = *cspacec::iParticleSystemBase_RemoveEffector;
*GetEffectorCount = *cspacec::iParticleSystemBase_GetEffectorCount;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleSystemBase($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleSystemBase_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleSystemFactory ##############

package cspace::iParticleSystemFactory;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iParticleSystemBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetDeepCreation = *cspacec::iParticleSystemFactory_SetDeepCreation;
*GetDeepCreation = *cspacec::iParticleSystemFactory_GetDeepCreation;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleSystemFactory($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleSystemFactory_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleSystem ##############

package cspace::iParticleSystem;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iParticleSystemBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetParticleCount = *cspacec::iParticleSystem_GetParticleCount;
*GetParticle = *cspacec::iParticleSystem_GetParticle;
*GetParticleAux = *cspacec::iParticleSystem_GetParticleAux;
*LockForExternalControl = *cspacec::iParticleSystem_LockForExternalControl;
*Advance = *cspacec::iParticleSystem_Advance;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleSystem($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleSystem_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleBuiltinEmitterBase ##############

package cspace::iParticleBuiltinEmitterBase;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iParticleEmitter cspace );
%OWNER = ();
%ITERATORS = ();
*SetPosition = *cspacec::iParticleBuiltinEmitterBase_SetPosition;
*GetPosition = *cspacec::iParticleBuiltinEmitterBase_GetPosition;
*SetParticlePlacement = *cspacec::iParticleBuiltinEmitterBase_SetParticlePlacement;
*GetParticlePlacement = *cspacec::iParticleBuiltinEmitterBase_GetParticlePlacement;
*SetUniformVelocity = *cspacec::iParticleBuiltinEmitterBase_SetUniformVelocity;
*GetUniformVelocity = *cspacec::iParticleBuiltinEmitterBase_GetUniformVelocity;
*SetInitialVelocity = *cspacec::iParticleBuiltinEmitterBase_SetInitialVelocity;
*GetInitialVelocity = *cspacec::iParticleBuiltinEmitterBase_GetInitialVelocity;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleBuiltinEmitterBase($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleBuiltinEmitterBase_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleBuiltinEmitterSphere ##############

package cspace::iParticleBuiltinEmitterSphere;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iParticleBuiltinEmitterBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetRadius = *cspacec::iParticleBuiltinEmitterSphere_SetRadius;
*GetRadius = *cspacec::iParticleBuiltinEmitterSphere_GetRadius;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleBuiltinEmitterSphere($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleBuiltinEmitterSphere_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleBuiltinEmitterCone ##############

package cspace::iParticleBuiltinEmitterCone;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iParticleBuiltinEmitterBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetExtent = *cspacec::iParticleBuiltinEmitterCone_SetExtent;
*GetExtent = *cspacec::iParticleBuiltinEmitterCone_GetExtent;
*SetConeAngle = *cspacec::iParticleBuiltinEmitterCone_SetConeAngle;
*GetConeAngle = *cspacec::iParticleBuiltinEmitterCone_GetConeAngle;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleBuiltinEmitterCone($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleBuiltinEmitterCone_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleBuiltinEmitterBox ##############

package cspace::iParticleBuiltinEmitterBox;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iParticleBuiltinEmitterBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetBox = *cspacec::iParticleBuiltinEmitterBox_SetBox;
*GetBox = *cspacec::iParticleBuiltinEmitterBox_GetBox;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleBuiltinEmitterBox($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleBuiltinEmitterBox_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleBuiltinEmitterCylinder ##############

package cspace::iParticleBuiltinEmitterCylinder;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iParticleBuiltinEmitterBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetRadius = *cspacec::iParticleBuiltinEmitterCylinder_SetRadius;
*GetRadius = *cspacec::iParticleBuiltinEmitterCylinder_GetRadius;
*SetExtent = *cspacec::iParticleBuiltinEmitterCylinder_SetExtent;
*GetExtent = *cspacec::iParticleBuiltinEmitterCylinder_GetExtent;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleBuiltinEmitterCylinder($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleBuiltinEmitterCylinder_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleBuiltinEmitterFactory ##############

package cspace::iParticleBuiltinEmitterFactory;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*CreateSphere = *cspacec::iParticleBuiltinEmitterFactory_CreateSphere;
*CreateCone = *cspacec::iParticleBuiltinEmitterFactory_CreateCone;
*CreateBox = *cspacec::iParticleBuiltinEmitterFactory_CreateBox;
*CreateCylinder = *cspacec::iParticleBuiltinEmitterFactory_CreateCylinder;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleBuiltinEmitterFactory($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleBuiltinEmitterFactory_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleBuiltinEffectorForce ##############

package cspace::iParticleBuiltinEffectorForce;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iParticleEffector cspace );
%OWNER = ();
%ITERATORS = ();
*SetAcceleration = *cspacec::iParticleBuiltinEffectorForce_SetAcceleration;
*GetAcceleration = *cspacec::iParticleBuiltinEffectorForce_GetAcceleration;
*SetForce = *cspacec::iParticleBuiltinEffectorForce_SetForce;
*GetForce = *cspacec::iParticleBuiltinEffectorForce_GetForce;
*SetRandomAcceleration = *cspacec::iParticleBuiltinEffectorForce_SetRandomAcceleration;
*GetRandomAcceleration = *cspacec::iParticleBuiltinEffectorForce_GetRandomAcceleration;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleBuiltinEffectorForce($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleBuiltinEffectorForce_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleBuiltinEffectorLinColor ##############

package cspace::iParticleBuiltinEffectorLinColor;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iParticleEffector cspace );
%OWNER = ();
%ITERATORS = ();
*AddColor = *cspacec::iParticleBuiltinEffectorLinColor_AddColor;
*SetColor = *cspacec::iParticleBuiltinEffectorLinColor_SetColor;
*GetColor = *cspacec::iParticleBuiltinEffectorLinColor_GetColor;
*GetColorCount = *cspacec::iParticleBuiltinEffectorLinColor_GetColorCount;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleBuiltinEffectorLinColor($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleBuiltinEffectorLinColor_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleBuiltinEffectorVelocityField ##############

package cspace::iParticleBuiltinEffectorVelocityField;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iParticleEffector cspace );
%OWNER = ();
%ITERATORS = ();
*SetType = *cspacec::iParticleBuiltinEffectorVelocityField_SetType;
*GetType = *cspacec::iParticleBuiltinEffectorVelocityField_GetType;
*SetFParameter = *cspacec::iParticleBuiltinEffectorVelocityField_SetFParameter;
*GetFParameter = *cspacec::iParticleBuiltinEffectorVelocityField_GetFParameter;
*GetFParameterCount = *cspacec::iParticleBuiltinEffectorVelocityField_GetFParameterCount;
*SetVParameter = *cspacec::iParticleBuiltinEffectorVelocityField_SetVParameter;
*GetVParameter = *cspacec::iParticleBuiltinEffectorVelocityField_GetVParameter;
*GetVParameterCount = *cspacec::iParticleBuiltinEffectorVelocityField_GetVParameterCount;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleBuiltinEffectorVelocityField($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParticleBuiltinEffectorFactory ##############

package cspace::iParticleBuiltinEffectorFactory;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*CreateForce = *cspacec::iParticleBuiltinEffectorFactory_CreateForce;
*CreateLinColor = *cspacec::iParticleBuiltinEffectorFactory_CreateLinColor;
*CreateVelocityField = *cspacec::iParticleBuiltinEffectorFactory_CreateVelocityField;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParticleBuiltinEffectorFactory($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iParticleBuiltinEffectorFactory_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iLoaderStatus ##############

package cspace::iLoaderStatus;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*IsReady = *cspacec::iLoaderStatus_IsReady;
*IsError = *cspacec::iLoaderStatus_IsError;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iLoaderStatus($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMissingLoaderData ##############

package cspace::iMissingLoaderData;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*MissingMaterial = *cspacec::iMissingLoaderData_MissingMaterial;
*MissingTexture = *cspacec::iMissingLoaderData_MissingTexture;
*MissingShader = *cspacec::iMissingLoaderData_MissingShader;
*MissingFactory = *cspacec::iMissingLoaderData_MissingFactory;
*MissingMesh = *cspacec::iMissingLoaderData_MissingMesh;
*MissingSector = *cspacec::iMissingLoaderData_MissingSector;
*MissingLight = *cspacec::iMissingLoaderData_MissingLight;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMissingLoaderData($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iLoader ##############

package cspace::iLoader;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*LoadImage = *cspacec::iLoader_LoadImage;
*LoadTexture = *cspacec::iLoader_LoadTexture;
*LoadSoundSysData = *cspacec::iLoader_LoadSoundSysData;
*LoadSoundStream = *cspacec::iLoader_LoadSoundStream;
*LoadSoundWrapper = *cspacec::iLoader_LoadSoundWrapper;
*ThreadedLoadMapFile = *cspacec::iLoader_ThreadedLoadMapFile;
*LoadMapFile = *cspacec::iLoader_LoadMapFile;
*LoadMap = *cspacec::iLoader_LoadMap;
*LoadLibraryFile = *cspacec::iLoader_LoadLibraryFile;
*LoadLibrary = *cspacec::iLoader_LoadLibrary;
*LoadMeshObjectFactory = *cspacec::iLoader_LoadMeshObjectFactory;
*LoadMeshObject = *cspacec::iLoader_LoadMeshObject;
*Load = *cspacec::iLoader_Load;
*LoadShader = *cspacec::iLoader_LoadShader;
*SetAutoRegions = *cspacec::iLoader_SetAutoRegions;
*GetAutoRegions = *cspacec::iLoader_GetAutoRegions;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iLoader($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iLoader_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iLoaderPlugin ##############

package cspace::iLoaderPlugin;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Parse = *cspacec::iLoaderPlugin_Parse;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iLoaderPlugin($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iLoaderPlugin_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iBinaryLoaderPlugin ##############

package cspace::iBinaryLoaderPlugin;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Parse = *cspacec::iBinaryLoaderPlugin_Parse;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iBinaryLoaderPlugin($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iBinaryLoaderPlugin_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSaver ##############

package cspace::iSaver;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SaveMapFile = *cspacec::iSaver_SaveMapFile;
*SaveAllRegions = *cspacec::iSaver_SaveAllRegions;
*SaveRegionFile = *cspacec::iSaver_SaveRegionFile;
*SaveRegion = *cspacec::iSaver_SaveRegion;
*SavePortal = *cspacec::iSaver_SavePortal;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSaver($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysData ##############

package cspace::iSndSysData;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetFormat = *cspacec::iSndSysData_GetFormat;
*GetFrameCount = *cspacec::iSndSysData_GetFrameCount;
*GetDataSize = *cspacec::iSndSysData_GetDataSize;
*CreateStream = *cspacec::iSndSysData_CreateStream;
*SetDescription = *cspacec::iSndSysData_SetDescription;
*GetDescription = *cspacec::iSndSysData_GetDescription;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysData($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSndSysData_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysSoftwareFilter3DProperties ##############

package cspace::iSndSysSoftwareFilter3DProperties;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_clean_buffer_get = *cspacec::iSndSysSoftwareFilter3DProperties_clean_buffer_get;
*swig_clean_buffer_set = *cspacec::iSndSysSoftwareFilter3DProperties_clean_buffer_set;
*swig_work_buffer_get = *cspacec::iSndSysSoftwareFilter3DProperties_work_buffer_get;
*swig_work_buffer_set = *cspacec::iSndSysSoftwareFilter3DProperties_work_buffer_set;
*swig_buffer_samples_get = *cspacec::iSndSysSoftwareFilter3DProperties_buffer_samples_get;
*swig_buffer_samples_set = *cspacec::iSndSysSoftwareFilter3DProperties_buffer_samples_set;
*swig_source_parameters_get = *cspacec::iSndSysSoftwareFilter3DProperties_source_parameters_get;
*swig_source_parameters_set = *cspacec::iSndSysSoftwareFilter3DProperties_source_parameters_set;
*swig_listener_parameters_get = *cspacec::iSndSysSoftwareFilter3DProperties_listener_parameters_get;
*swig_listener_parameters_set = *cspacec::iSndSysSoftwareFilter3DProperties_listener_parameters_set;
*swig_sound_format_get = *cspacec::iSndSysSoftwareFilter3DProperties_sound_format_get;
*swig_sound_format_set = *cspacec::iSndSysSoftwareFilter3DProperties_sound_format_set;
*swig_closest_speaker_distance_get = *cspacec::iSndSysSoftwareFilter3DProperties_closest_speaker_distance_get;
*swig_closest_speaker_distance_set = *cspacec::iSndSysSoftwareFilter3DProperties_closest_speaker_distance_set;
*swig_speaker_distance_get = *cspacec::iSndSysSoftwareFilter3DProperties_speaker_distance_get;
*swig_speaker_distance_set = *cspacec::iSndSysSoftwareFilter3DProperties_speaker_distance_set;
*swig_speaker_direction_cos_get = *cspacec::iSndSysSoftwareFilter3DProperties_speaker_direction_cos_get;
*swig_speaker_direction_cos_set = *cspacec::iSndSysSoftwareFilter3DProperties_speaker_direction_cos_set;
*swig_channel_get = *cspacec::iSndSysSoftwareFilter3DProperties_channel_get;
*swig_channel_set = *cspacec::iSndSysSoftwareFilter3DProperties_channel_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_iSndSysSoftwareFilter3DProperties(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysSoftwareFilter3DProperties($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysSoftwareFilter3D ##############

package cspace::iSndSysSoftwareFilter3D;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Apply = *cspacec::iSndSysSoftwareFilter3D_Apply;
*AddSubFilter = *cspacec::iSndSysSoftwareFilter3D_AddSubFilter;
*GetSubFilter = *cspacec::iSndSysSoftwareFilter3D_GetSubFilter;
*GetPtr = *cspacec::iSndSysSoftwareFilter3D_GetPtr;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysSoftwareFilter3D($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSndSysSoftwareFilter3D_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysSoftwareOutputFilter ##############

package cspace::iSndSysSoftwareOutputFilter;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*FormatNotify = *cspacec::iSndSysSoftwareOutputFilter_FormatNotify;
*DeliverData = *cspacec::iSndSysSoftwareOutputFilter_DeliverData;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysSoftwareOutputFilter($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysListener ##############

package cspace::iSndSysListener;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetDirection = *cspacec::iSndSysListener_SetDirection;
*SetPosition = *cspacec::iSndSysListener_SetPosition;
*SetDistanceFactor = *cspacec::iSndSysListener_SetDistanceFactor;
*SetRollOffFactor = *cspacec::iSndSysListener_SetRollOffFactor;
*GetDirection = *cspacec::iSndSysListener_GetDirection;
*GetPosition = *cspacec::iSndSysListener_GetPosition;
*GetDistanceFactor = *cspacec::iSndSysListener_GetDistanceFactor;
*GetRollOffFactor = *cspacec::iSndSysListener_GetRollOffFactor;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysListener($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSndSysListener_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysLoader ##############

package cspace::iSndSysLoader;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*LoadSound = *cspacec::iSndSysLoader_LoadSound;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysLoader($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSndSysLoader_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysWrapper ##############

package cspace::iSndSysWrapper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iSndSysWrapper_QueryObject;
*GetStream = *cspacec::iSndSysWrapper_GetStream;
*SetStream = *cspacec::iSndSysWrapper_SetStream;
*GetData = *cspacec::iSndSysWrapper_GetData;
*SetData = *cspacec::iSndSysWrapper_SetData;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysWrapper($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSndSysWrapper_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysManager ##############

package cspace::iSndSysManager;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*CreateSound = *cspacec::iSndSysManager_CreateSound;
*RemoveSound = *cspacec::iSndSysManager_RemoveSound;
*RemoveSounds = *cspacec::iSndSysManager_RemoveSounds;
*GetSoundCount = *cspacec::iSndSysManager_GetSoundCount;
*GetSound = *cspacec::iSndSysManager_GetSound;
*FindSoundByName = *cspacec::iSndSysManager_FindSoundByName;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysManager($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSndSysManager_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysSource ##############

package cspace::iSndSysSource;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetVolume = *cspacec::iSndSysSource_SetVolume;
*GetVolume = *cspacec::iSndSysSource_GetVolume;
*GetStream = *cspacec::iSndSysSource_GetStream;
*AddOutputFilter = *cspacec::iSndSysSource_AddOutputFilter;
*RemoveOutputFilter = *cspacec::iSndSysSource_RemoveOutputFilter;
*GetPtr = *cspacec::iSndSysSource_GetPtr;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysSource($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSndSysSource_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysSourceSoftware ##############

package cspace::iSndSysSourceSoftware;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iSndSysSource cspace );
%OWNER = ();
%ITERATORS = ();
*MergeIntoBuffer = *cspacec::iSndSysSourceSoftware_MergeIntoBuffer;
*ProcessOutputFilters = *cspacec::iSndSysSourceSoftware_ProcessOutputFilters;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysSourceSoftware($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysSourceSoftware3D ##############

package cspace::iSndSysSourceSoftware3D;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iSndSysSourceSoftware cspace );
%OWNER = ();
%ITERATORS = ();
*SetPosition = *cspacec::iSndSysSourceSoftware3D_SetPosition;
*GetPosition = *cspacec::iSndSysSourceSoftware3D_GetPosition;
*SetDirection = *cspacec::iSndSysSourceSoftware3D_SetDirection;
*GetDirection = *cspacec::iSndSysSourceSoftware3D_GetDirection;
*SetDirectionalRadiation = *cspacec::iSndSysSourceSoftware3D_SetDirectionalRadiation;
*GetDirectionalRadiation = *cspacec::iSndSysSourceSoftware3D_GetDirectionalRadiation;
*SetMinimumDistance = *cspacec::iSndSysSourceSoftware3D_SetMinimumDistance;
*SetMaximumDistance = *cspacec::iSndSysSourceSoftware3D_SetMaximumDistance;
*GetMinimumDistance = *cspacec::iSndSysSourceSoftware3D_GetMinimumDistance;
*GetMaximumDistance = *cspacec::iSndSysSourceSoftware3D_GetMaximumDistance;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysSourceSoftware3D($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSndSysSoundFormat ##############

package cspace::csSndSysSoundFormat;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_Freq_get = *cspacec::csSndSysSoundFormat_Freq_get;
*swig_Freq_set = *cspacec::csSndSysSoundFormat_Freq_set;
*swig_Bits_get = *cspacec::csSndSysSoundFormat_Bits_get;
*swig_Bits_set = *cspacec::csSndSysSoundFormat_Bits_set;
*swig_Channels_get = *cspacec::csSndSysSoundFormat_Channels_get;
*swig_Channels_set = *cspacec::csSndSysSoundFormat_Channels_set;
*swig_Flags_get = *cspacec::csSndSysSoundFormat_Flags_get;
*swig_Flags_set = *cspacec::csSndSysSoundFormat_Flags_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csSndSysSoundFormat(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSndSysSoundFormat($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysStream ##############

package cspace::iSndSysStream;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetDescription = *cspacec::iSndSysStream_GetDescription;
*GetRenderedFormat = *cspacec::iSndSysStream_GetRenderedFormat;
*Get3dMode = *cspacec::iSndSysStream_Get3dMode;
*GetFrameCount = *cspacec::iSndSysStream_GetFrameCount;
*GetPosition = *cspacec::iSndSysStream_GetPosition;
*ResetPosition = *cspacec::iSndSysStream_ResetPosition;
*SetPosition = *cspacec::iSndSysStream_SetPosition;
*Pause = *cspacec::iSndSysStream_Pause;
*Unpause = *cspacec::iSndSysStream_Unpause;
*GetPauseState = *cspacec::iSndSysStream_GetPauseState;
*SetLoopState = *cspacec::iSndSysStream_SetLoopState;
*GetLoopState = *cspacec::iSndSysStream_GetLoopState;
*SetPlayRatePercent = *cspacec::iSndSysStream_SetPlayRatePercent;
*GetPlayRatePercent = *cspacec::iSndSysStream_GetPlayRatePercent;
*SetAutoUnregister = *cspacec::iSndSysStream_SetAutoUnregister;
*GetAutoUnregister = *cspacec::iSndSysStream_GetAutoUnregister;
*GetAutoUnregisterRequested = *cspacec::iSndSysStream_GetAutoUnregisterRequested;
*AdvancePosition = *cspacec::iSndSysStream_AdvancePosition;
*GetDataPointers = *cspacec::iSndSysStream_GetDataPointers;
*InitializeSourcePositionMarker = *cspacec::iSndSysStream_InitializeSourcePositionMarker;
*ProcessNotifications = *cspacec::iSndSysStream_ProcessNotifications;
*RegisterCallback = *cspacec::iSndSysStream_RegisterCallback;
*UnregisterCallback = *cspacec::iSndSysStream_UnregisterCallback;
*RegisterFrameNotification = *cspacec::iSndSysStream_RegisterFrameNotification;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysStream($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSndSysStream_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysStreamCallback ##############

package cspace::iSndSysStreamCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*StreamLoopNotification = *cspacec::iSndSysStreamCallback_StreamLoopNotification;
*StreamPauseNotification = *cspacec::iSndSysStreamCallback_StreamPauseNotification;
*StreamUnpauseNotification = *cspacec::iSndSysStreamCallback_StreamUnpauseNotification;
*StreamFrameNotification = *cspacec::iSndSysStreamCallback_StreamFrameNotification;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysStreamCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysRenderer ##############

package cspace::iSndSysRenderer;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetVolume = *cspacec::iSndSysRenderer_SetVolume;
*GetVolume = *cspacec::iSndSysRenderer_GetVolume;
*CreateStream = *cspacec::iSndSysRenderer_CreateStream;
*CreateSource = *cspacec::iSndSysRenderer_CreateSource;
*RemoveStream = *cspacec::iSndSysRenderer_RemoveStream;
*RemoveSource = *cspacec::iSndSysRenderer_RemoveSource;
*GetListener = *cspacec::iSndSysRenderer_GetListener;
*RegisterCallback = *cspacec::iSndSysRenderer_RegisterCallback;
*UnregisterCallback = *cspacec::iSndSysRenderer_UnregisterCallback;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysRenderer($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSndSysRenderer_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysRendererCallback ##############

package cspace::iSndSysRendererCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*StreamAddNotification = *cspacec::iSndSysRendererCallback_StreamAddNotification;
*StreamRemoveNotification = *cspacec::iSndSysRendererCallback_StreamRemoveNotification;
*SourceAddNotification = *cspacec::iSndSysRendererCallback_SourceAddNotification;
*SourceRemoveNotification = *cspacec::iSndSysRendererCallback_SourceRemoveNotification;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysRendererCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysRendererSoftware ##############

package cspace::iSndSysRendererSoftware;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*AddOutputFilter = *cspacec::iSndSysRendererSoftware_AddOutputFilter;
*RemoveOutputFilter = *cspacec::iSndSysRendererSoftware_RemoveOutputFilter;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysRendererSoftware($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSndSysSoftwareDriver ##############

package cspace::iSndSysSoftwareDriver;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Open = *cspacec::iSndSysSoftwareDriver_Open;
*Close = *cspacec::iSndSysSoftwareDriver_Close;
*StartThread = *cspacec::iSndSysSoftwareDriver_StartThread;
*StopThread = *cspacec::iSndSysSoftwareDriver_StopThread;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSndSysSoftwareDriver($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSndSysSoftwareDriver_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iComponent ##############

package cspace::iComponent;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Initialize = *cspacec::iComponent_Initialize;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iComponent($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iComponent_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iCacheManager ##############

package cspace::iCacheManager;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetReadOnly = *cspacec::iCacheManager_SetReadOnly;
*IsReadOnly = *cspacec::iCacheManager_IsReadOnly;
*SetCurrentType = *cspacec::iCacheManager_SetCurrentType;
*GetCurrentType = *cspacec::iCacheManager_GetCurrentType;
*SetCurrentScope = *cspacec::iCacheManager_SetCurrentScope;
*GetCurrentScope = *cspacec::iCacheManager_GetCurrentScope;
*CacheData = *cspacec::iCacheManager_CacheData;
*ReadCache = *cspacec::iCacheManager_ReadCache;
*ClearCache = *cspacec::iCacheManager_ClearCache;
*Flush = *cspacec::iCacheManager_Flush;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iCacheManager($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iCacheManager_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csFileTime ##############

package cspace::csFileTime;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_sec_get = *cspacec::csFileTime_sec_get;
*swig_sec_set = *cspacec::csFileTime_sec_set;
*swig_min_get = *cspacec::csFileTime_min_get;
*swig_min_set = *cspacec::csFileTime_min_set;
*swig_hour_get = *cspacec::csFileTime_hour_get;
*swig_hour_set = *cspacec::csFileTime_hour_set;
*swig_day_get = *cspacec::csFileTime_day_get;
*swig_day_set = *cspacec::csFileTime_day_set;
*swig_mon_get = *cspacec::csFileTime_mon_get;
*swig_mon_set = *cspacec::csFileTime_mon_set;
*swig_year_get = *cspacec::csFileTime_year_get;
*swig_year_set = *cspacec::csFileTime_year_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csFileTime(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csFileTime($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iFile ##############

package cspace::iFile;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetName = *cspacec::iFile_GetName;
*GetSize = *cspacec::iFile_GetSize;
*GetStatus = *cspacec::iFile_GetStatus;
*Read = *cspacec::iFile_Read;
*Write = *cspacec::iFile_Write;
*Flush = *cspacec::iFile_Flush;
*AtEOF = *cspacec::iFile_AtEOF;
*GetPos = *cspacec::iFile_GetPos;
*SetPos = *cspacec::iFile_SetPos;
*GetAllData = *cspacec::iFile_GetAllData;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iFile($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iFile_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iVFS ##############

package cspace::iVFS;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*ChDir = *cspacec::iVFS_ChDir;
*GetCwd = *cspacec::iVFS_GetCwd;
*PushDir = *cspacec::iVFS_PushDir;
*PopDir = *cspacec::iVFS_PopDir;
*ExpandPath = *cspacec::iVFS_ExpandPath;
*Exists = *cspacec::iVFS_Exists;
*FindFiles = *cspacec::iVFS_FindFiles;
*Open = *cspacec::iVFS_Open;
*ReadFile = *cspacec::iVFS_ReadFile;
*WriteFile = *cspacec::iVFS_WriteFile;
*DeleteFile = *cspacec::iVFS_DeleteFile;
*Sync = *cspacec::iVFS_Sync;
*SymbolicLink = *cspacec::iVFS_SymbolicLink;
*Mount = *cspacec::iVFS_Mount;
*Unmount = *cspacec::iVFS_Unmount;
*MountRoot = *cspacec::iVFS_MountRoot;
*SaveMounts = *cspacec::iVFS_SaveMounts;
*LoadMountsFromFile = *cspacec::iVFS_LoadMountsFromFile;
*ChDirAuto = *cspacec::iVFS_ChDirAuto;
*GetFileTime = *cspacec::iVFS_GetFileTime;
*SetFileTime = *cspacec::iVFS_SetFileTime;
*GetFileSize = *cspacec::iVFS_GetFileSize;
*GetRealPath = *cspacec::iVFS_GetRealPath;
*GetMounts = *cspacec::iVFS_GetMounts;
*GetRealMountPaths = *cspacec::iVFS_GetRealMountPaths;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iVFS($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iVFS_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iObjectNameChangeListener ##############

package cspace::iObjectNameChangeListener;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*NameChanged = *cspacec::iObjectNameChangeListener_NameChanged;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iObjectNameChangeListener($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iObject ##############

package cspace::iObject;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetName = *cspacec::iObject_SetName;
*GetName = *cspacec::iObject_GetName;
*GetID = *cspacec::iObject_GetID;
*SetObjectParent = *cspacec::iObject_SetObjectParent;
*GetObjectParent = *cspacec::iObject_GetObjectParent;
*ObjAdd = *cspacec::iObject_ObjAdd;
*ObjRemove = *cspacec::iObject_ObjRemove;
*ObjRemoveAll = *cspacec::iObject_ObjRemoveAll;
*ObjAddChildren = *cspacec::iObject_ObjAddChildren;
*GetChild = *cspacec::iObject_GetChild;
*GetIterator = *cspacec::iObject_GetIterator;
*ObjReleaseOld = *cspacec::iObject_ObjReleaseOld;
*AddNameChangeListener = *cspacec::iObject_AddNameChangeListener;
*RemoveNameChangeListener = *cspacec::iObject_RemoveNameChangeListener;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iObject($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iObject_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iObjectIterator ##############

package cspace::iObjectIterator;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Next = *cspacec::iObjectIterator_Next;
*Reset = *cspacec::iObjectIterator_Reset;
*GetParentObj = *cspacec::iObjectIterator_GetParentObj;
*HasNext = *cspacec::iObjectIterator_HasNext;
*FindName = *cspacec::iObjectIterator_FindName;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iObjectIterator($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iStringSet ##############

package cspace::iStringSet;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Request = *cspacec::iStringSet_Request;
*Contains = *cspacec::iStringSet_Contains;
*Delete = *cspacec::iStringSet_Delete;
*Empty = *cspacec::iStringSet_Empty;
*Clear = *cspacec::iStringSet_Clear;
*GetSize = *cspacec::iStringSet_GetSize;
*IsEmpty = *cspacec::iStringSet_IsEmpty;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iStringSet($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iStringSet_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iObjectRegistry ##############

package cspace::iObjectRegistry;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Clear = *cspacec::iObjectRegistry_Clear;
*Register = *cspacec::iObjectRegistry_Register;
*Unregister = *cspacec::iObjectRegistry_Unregister;
*Get = *cspacec::iObjectRegistry_Get;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iObjectRegistry($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iObjectRegistry_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iObjectRegistryIterator ##############

package cspace::iObjectRegistryIterator;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Reset = *cspacec::iObjectRegistryIterator_Reset;
*GetCurrentTag = *cspacec::iObjectRegistryIterator_GetCurrentTag;
*HasNext = *cspacec::iObjectRegistryIterator_HasNext;
*Next = *cspacec::iObjectRegistryIterator_Next;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iObjectRegistryIterator($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iVirtualClock ##############

package cspace::iVirtualClock;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Advance = *cspacec::iVirtualClock_Advance;
*Suspend = *cspacec::iVirtualClock_Suspend;
*Resume = *cspacec::iVirtualClock_Resume;
*GetElapsedTicks = *cspacec::iVirtualClock_GetElapsedTicks;
*GetCurrentTicks = *cspacec::iVirtualClock_GetCurrentTicks;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iVirtualClock($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iVirtualClock_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iEventAttributeIterator ##############

package cspace::iEventAttributeIterator;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*HasNext = *cspacec::iEventAttributeIterator_HasNext;
*Next = *cspacec::iEventAttributeIterator_Next;
*Reset = *cspacec::iEventAttributeIterator_Reset;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iEventAttributeIterator($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csKeyEventData ##############

package cspace::csKeyEventData;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_eventType_get = *cspacec::csKeyEventData_eventType_get;
*swig_eventType_set = *cspacec::csKeyEventData_eventType_set;
*swig_codeRaw_get = *cspacec::csKeyEventData_codeRaw_get;
*swig_codeRaw_set = *cspacec::csKeyEventData_codeRaw_set;
*swig_codeCooked_get = *cspacec::csKeyEventData_codeCooked_get;
*swig_codeCooked_set = *cspacec::csKeyEventData_codeCooked_set;
*swig_modifiers_get = *cspacec::csKeyEventData_modifiers_get;
*swig_modifiers_set = *cspacec::csKeyEventData_modifiers_set;
*swig_autoRepeat_get = *cspacec::csKeyEventData_autoRepeat_get;
*swig_autoRepeat_set = *cspacec::csKeyEventData_autoRepeat_set;
*swig_charType_get = *cspacec::csKeyEventData_charType_get;
*swig_charType_set = *cspacec::csKeyEventData_charType_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csKeyEventData(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csKeyEventData($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csMouseEventData ##############

package cspace::csMouseEventData;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_x_get = *cspacec::csMouseEventData_x_get;
*swig_x_set = *cspacec::csMouseEventData_x_set;
*swig_y_get = *cspacec::csMouseEventData_y_get;
*swig_y_set = *cspacec::csMouseEventData_y_set;
*swig_axes_get = *cspacec::csMouseEventData_axes_get;
*swig_axes_set = *cspacec::csMouseEventData_axes_set;
*swig_numAxes_get = *cspacec::csMouseEventData_numAxes_get;
*swig_numAxes_set = *cspacec::csMouseEventData_numAxes_set;
*swig_Button_get = *cspacec::csMouseEventData_Button_get;
*swig_Button_set = *cspacec::csMouseEventData_Button_set;
*swig_Modifiers_get = *cspacec::csMouseEventData_Modifiers_get;
*swig_Modifiers_set = *cspacec::csMouseEventData_Modifiers_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csMouseEventData(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csMouseEventData($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csJoystickEventData ##############

package cspace::csJoystickEventData;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_number_get = *cspacec::csJoystickEventData_number_get;
*swig_number_set = *cspacec::csJoystickEventData_number_set;
*swig_axes_get = *cspacec::csJoystickEventData_axes_get;
*swig_axes_set = *cspacec::csJoystickEventData_axes_set;
*swig_numAxes_get = *cspacec::csJoystickEventData_numAxes_get;
*swig_numAxes_set = *cspacec::csJoystickEventData_numAxes_set;
*swig_axesChanged_get = *cspacec::csJoystickEventData_axesChanged_get;
*swig_axesChanged_set = *cspacec::csJoystickEventData_axesChanged_set;
*swig_Button_get = *cspacec::csJoystickEventData_Button_get;
*swig_Button_set = *cspacec::csJoystickEventData_Button_set;
*swig_Modifiers_get = *cspacec::csJoystickEventData_Modifiers_get;
*swig_Modifiers_set = *cspacec::csJoystickEventData_Modifiers_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csJoystickEventData(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csJoystickEventData($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csCommandEventData ##############

package cspace::csCommandEventData;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_Code_get = *cspacec::csCommandEventData_Code_get;
*swig_Code_set = *cspacec::csCommandEventData_Code_set;
*swig_Info_get = *cspacec::csCommandEventData_Info_get;
*swig_Info_set = *cspacec::csCommandEventData_Info_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csCommandEventData(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csCommandEventData($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iEvent ##############

package cspace::iEvent;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*swig_Name_get = *cspacec::iEvent_Name_get;
*swig_Name_set = *cspacec::iEvent_Name_set;
*GetName = *cspacec::iEvent_GetName;
*swig_Time_get = *cspacec::iEvent_Time_get;
*swig_Time_set = *cspacec::iEvent_Time_set;
*swig_Broadcast_get = *cspacec::iEvent_Broadcast_get;
*swig_Broadcast_set = *cspacec::iEvent_Broadcast_set;
*AddInt8 = *cspacec::iEvent_AddInt8;
*AddUInt8 = *cspacec::iEvent_AddUInt8;
*AddInt16 = *cspacec::iEvent_AddInt16;
*AddUInt16 = *cspacec::iEvent_AddUInt16;
*AddInt32 = *cspacec::iEvent_AddInt32;
*AddUInt32 = *cspacec::iEvent_AddUInt32;
*AddFloat = *cspacec::iEvent_AddFloat;
*AddDouble = *cspacec::iEvent_AddDouble;
*AddBool = *cspacec::iEvent_AddBool;
*Add = *cspacec::iEvent_Add;
*RetrieveInt8 = *cspacec::iEvent_RetrieveInt8;
*RetrieveUInt8 = *cspacec::iEvent_RetrieveUInt8;
*RetrieveInt16 = *cspacec::iEvent_RetrieveInt16;
*RetrieveUInt16 = *cspacec::iEvent_RetrieveUInt16;
*RetrieveInt32 = *cspacec::iEvent_RetrieveInt32;
*RetrieveUInt32 = *cspacec::iEvent_RetrieveUInt32;
*RetrieveFloat = *cspacec::iEvent_RetrieveFloat;
*RetrieveDouble = *cspacec::iEvent_RetrieveDouble;
*RetrieveBool = *cspacec::iEvent_RetrieveBool;
*Retrieve = *cspacec::iEvent_Retrieve;
*AttributeExists = *cspacec::iEvent_AttributeExists;
*GetAttributeType = *cspacec::iEvent_GetAttributeType;
*Remove = *cspacec::iEvent_Remove;
*RemoveAll = *cspacec::iEvent_RemoveAll;
*GetAttributeIterator = *cspacec::iEvent_GetAttributeIterator;
*RetrieveString = *cspacec::iEvent_RetrieveString;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iEvent($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iEvent_scfGetVersion;
*swig_Mouse_get = *cspacec::iEvent_Mouse_get;
*swig_Mouse_set = *cspacec::iEvent_Mouse_set;
*swig_Joystick_get = *cspacec::iEvent_Joystick_get;
*swig_Joystick_set = *cspacec::iEvent_Joystick_set;
*swig_Command_get = *cspacec::iEvent_Command_get;
*swig_Command_set = *cspacec::iEvent_Command_set;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iEventPlug ##############

package cspace::iEventPlug;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetPotentiallyConflictingEvents = *cspacec::iEventPlug_GetPotentiallyConflictingEvents;
*QueryEventPriority = *cspacec::iEventPlug_QueryEventPriority;
*EnableEvents = *cspacec::iEventPlug_EnableEvents;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iEventPlug($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iEventOutlet ##############

package cspace::iEventOutlet;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*CreateEvent = *cspacec::iEventOutlet_CreateEvent;
*Post = *cspacec::iEventOutlet_Post;
*Key = *cspacec::iEventOutlet_Key;
*Mouse = *cspacec::iEventOutlet_Mouse;
*Joystick = *cspacec::iEventOutlet_Joystick;
*Broadcast = *cspacec::iEventOutlet_Broadcast;
*ImmediateBroadcast = *cspacec::iEventOutlet_ImmediateBroadcast;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iEventOutlet($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iEventCord ##############

package cspace::iEventCord;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Insert = *cspacec::iEventCord_Insert;
*Remove = *cspacec::iEventCord_Remove;
*GetPass = *cspacec::iEventCord_GetPass;
*SetPass = *cspacec::iEventCord_SetPass;
*GetName = *cspacec::iEventCord_GetName;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iEventCord($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csKeyEventHelper ##############

package cspace::csKeyEventHelper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*GetRawCode = *cspacec::csKeyEventHelper_GetRawCode;
*GetCookedCode = *cspacec::csKeyEventHelper_GetCookedCode;
*GetEventType = *cspacec::csKeyEventHelper_GetEventType;
*GetAutoRepeat = *cspacec::csKeyEventHelper_GetAutoRepeat;
*GetCharacterType = *cspacec::csKeyEventHelper_GetCharacterType;
*GetEventData = *cspacec::csKeyEventHelper_GetEventData;
*GetModifiersBits = *cspacec::csKeyEventHelper_GetModifiersBits;
*GetModifiers = *cspacec::csKeyEventHelper_GetModifiers;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csKeyEventHelper(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csKeyEventHelper($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csMouseEventHelper ##############

package cspace::csMouseEventHelper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*NewEvent = *cspacec::csMouseEventHelper_NewEvent;
*GetEventType = *cspacec::csMouseEventHelper_GetEventType;
*GetNumber = *cspacec::csMouseEventHelper_GetNumber;
*GetX = *cspacec::csMouseEventHelper_GetX;
*GetY = *cspacec::csMouseEventHelper_GetY;
*GetAxis = *cspacec::csMouseEventHelper_GetAxis;
*GetNumAxes = *cspacec::csMouseEventHelper_GetNumAxes;
*GetButton = *cspacec::csMouseEventHelper_GetButton;
*GetButtonState = *cspacec::csMouseEventHelper_GetButtonState;
*GetButtonMask = *cspacec::csMouseEventHelper_GetButtonMask;
*GetModifiers = *cspacec::csMouseEventHelper_GetModifiers;
*GetEventData = *cspacec::csMouseEventHelper_GetEventData;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csMouseEventHelper(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csMouseEventHelper($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csJoystickEventHelper ##############

package cspace::csJoystickEventHelper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*NewEvent = *cspacec::csJoystickEventHelper_NewEvent;
*GetNumber = *cspacec::csJoystickEventHelper_GetNumber;
*GetAxis = *cspacec::csJoystickEventHelper_GetAxis;
*GetNumAxes = *cspacec::csJoystickEventHelper_GetNumAxes;
*GetButton = *cspacec::csJoystickEventHelper_GetButton;
*GetButtonState = *cspacec::csJoystickEventHelper_GetButtonState;
*GetButtonMask = *cspacec::csJoystickEventHelper_GetButtonMask;
*GetModifiers = *cspacec::csJoystickEventHelper_GetModifiers;
*GetEventData = *cspacec::csJoystickEventHelper_GetEventData;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csJoystickEventHelper(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csJoystickEventHelper($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csInputEventHelper ##############

package cspace::csInputEventHelper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*GetButton = *cspacec::csInputEventHelper_GetButton;
*GetButtonState = *cspacec::csInputEventHelper_GetButtonState;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csInputEventHelper(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csInputEventHelper($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csCommandEventHelper ##############

package cspace::csCommandEventHelper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*NewEvent = *cspacec::csCommandEventHelper_NewEvent;
*GetCode = *cspacec::csCommandEventHelper_GetCode;
*GetInfo = *cspacec::csCommandEventHelper_GetInfo;
*GetEventData = *cspacec::csCommandEventHelper_GetEventData;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csCommandEventHelper(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csCommandEventHelper($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csKeyModifiers ##############

package cspace::csKeyModifiers;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_modifiers_get = *cspacec::csKeyModifiers_modifiers_get;
*swig_modifiers_set = *cspacec::csKeyModifiers_modifiers_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csKeyModifiers(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csKeyModifiers($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iEventQueue ##############

package cspace::iEventQueue;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Process = *cspacec::iEventQueue_Process;
*Dispatch = *cspacec::iEventQueue_Dispatch;
*Subscribe = *cspacec::iEventQueue_Subscribe;
*Unsubscribe = *cspacec::iEventQueue_Unsubscribe;
*RegisterListener = *cspacec::iEventQueue_RegisterListener;
*RemoveListener = *cspacec::iEventQueue_RemoveListener;
*CreateEventOutlet = *cspacec::iEventQueue_CreateEventOutlet;
*GetEventOutlet = *cspacec::iEventQueue_GetEventOutlet;
*GetEventCord = *cspacec::iEventQueue_GetEventCord;
*CreateEvent = *cspacec::iEventQueue_CreateEvent;
*CreateBroadcastEvent = *cspacec::iEventQueue_CreateBroadcastEvent;
*Post = *cspacec::iEventQueue_Post;
*Get = *cspacec::iEventQueue_Get;
*Clear = *cspacec::iEventQueue_Clear;
*IsEmpty = *cspacec::iEventQueue_IsEmpty;
*RemoveAllListeners = *cspacec::iEventQueue_RemoveAllListeners;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iEventQueue($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iEventQueue_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csStrKey ##############

package cspace::csStrKey;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csStrKey(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csStrKey($self);
        delete $OWNER{$self};
    }
}

*__copy__ = *cspacec::csStrKey___copy__;
*GetHash = *cspacec::csStrKey_GetHash;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iEventNameRegistry ##############

package cspace::iEventNameRegistry;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetID = *cspacec::iEventNameRegistry_GetID;
*GetString = *cspacec::iEventNameRegistry_GetString;
*GetParentID = *cspacec::iEventNameRegistry_GetParentID;
*IsImmediateChildOf = *cspacec::iEventNameRegistry_IsImmediateChildOf;
*IsKindOf = *cspacec::iEventNameRegistry_IsKindOf;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iEventNameRegistry($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iEventNameRegistry_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csEventNameRegistry ##############

package cspace::csEventNameRegistry;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csEventNameRegistry($self);
        delete $OWNER{$self};
    }
}

*GetString = *cspacec::csEventNameRegistry_GetString;
*GetParentID = *cspacec::csEventNameRegistry_GetParentID;
*IsImmediateChildOf = *cspacec::csEventNameRegistry_IsImmediateChildOf;
*GetRegistry = *cspacec::csEventNameRegistry_GetRegistry;
*GetID = *cspacec::csEventNameRegistry_GetID;
*IsKindOf = *cspacec::csEventNameRegistry_IsKindOf;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iEventHandler ##############

package cspace::iEventHandler;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*HandleEvent = *cspacec::iEventHandler_HandleEvent;
*GenericName = *cspacec::iEventHandler_GenericName;
*GenericID = *cspacec::iEventHandler_GenericID;
*GenericPrec = *cspacec::iEventHandler_GenericPrec;
*GenericSucc = *cspacec::iEventHandler_GenericSucc;
*InstancePrec = *cspacec::iEventHandler_InstancePrec;
*InstanceSucc = *cspacec::iEventHandler_InstanceSucc;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iEventHandler($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iEventHandler_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iPluginIterator ##############

package cspace::iPluginIterator;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*HasNext = *cspacec::iPluginIterator_HasNext;
*Next = *cspacec::iPluginIterator_Next;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iPluginIterator($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iPluginManager ##############

package cspace::iPluginManager;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*LoadPlugin = *cspacec::iPluginManager_LoadPlugin;
*QueryPlugin = *cspacec::iPluginManager_QueryPlugin;
*UnloadPlugin = *cspacec::iPluginManager_UnloadPlugin;
*RegisterPlugin = *cspacec::iPluginManager_RegisterPlugin;
*GetPlugins = *cspacec::iPluginManager_GetPlugins;
*Clear = *cspacec::iPluginManager_Clear;
*QueryOptions = *cspacec::iPluginManager_QueryOptions;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iPluginManager($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iPluginManager_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iKeyComposer ##############

package cspace::iKeyComposer;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*HandleKey = *cspacec::iKeyComposer_HandleKey;
*ResetState = *cspacec::iKeyComposer_ResetState;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iKeyComposer($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iKeyboardDriver ##############

package cspace::iKeyboardDriver;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Reset = *cspacec::iKeyboardDriver_Reset;
*DoKey = *cspacec::iKeyboardDriver_DoKey;
*GetModifierState = *cspacec::iKeyboardDriver_GetModifierState;
*CreateKeyComposer = *cspacec::iKeyboardDriver_CreateKeyComposer;
*SynthesizeCooked = *cspacec::iKeyboardDriver_SynthesizeCooked;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iKeyboardDriver($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iKeyboardDriver_scfGetVersion;
*GetKeyState = *cspacec::iKeyboardDriver_GetKeyState;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMouseDriver ##############

package cspace::iMouseDriver;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetDoubleClickTime = *cspacec::iMouseDriver_SetDoubleClickTime;
*Reset = *cspacec::iMouseDriver_Reset;
*GetLastX = *cspacec::iMouseDriver_GetLastX;
*GetLastY = *cspacec::iMouseDriver_GetLastY;
*GetLast = *cspacec::iMouseDriver_GetLast;
*GetLastButton = *cspacec::iMouseDriver_GetLastButton;
*DoButton = *cspacec::iMouseDriver_DoButton;
*DoMotion = *cspacec::iMouseDriver_DoMotion;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMouseDriver($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iMouseDriver_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iJoystickDriver ##############

package cspace::iJoystickDriver;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Reset = *cspacec::iJoystickDriver_Reset;
*GetLastX = *cspacec::iJoystickDriver_GetLastX;
*GetLastY = *cspacec::iJoystickDriver_GetLastY;
*GetLast = *cspacec::iJoystickDriver_GetLast;
*GetLastButton = *cspacec::iJoystickDriver_GetLastButton;
*DoButton = *cspacec::iJoystickDriver_DoButton;
*DoMotion = *cspacec::iJoystickDriver_DoMotion;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iJoystickDriver($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iJoystickDriver_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iConfigFile ##############

package cspace::iConfigFile;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetFileName = *cspacec::iConfigFile_GetFileName;
*GetVFS = *cspacec::iConfigFile_GetVFS;
*SetFileName = *cspacec::iConfigFile_SetFileName;
*Load = *cspacec::iConfigFile_Load;
*Save = *cspacec::iConfigFile_Save;
*Clear = *cspacec::iConfigFile_Clear;
*Enumerate = *cspacec::iConfigFile_Enumerate;
*KeyExists = *cspacec::iConfigFile_KeyExists;
*SubsectionExists = *cspacec::iConfigFile_SubsectionExists;
*GetInt = *cspacec::iConfigFile_GetInt;
*GetFloat = *cspacec::iConfigFile_GetFloat;
*GetStr = *cspacec::iConfigFile_GetStr;
*GetBool = *cspacec::iConfigFile_GetBool;
*GetTuple = *cspacec::iConfigFile_GetTuple;
*GetComment = *cspacec::iConfigFile_GetComment;
*SetStr = *cspacec::iConfigFile_SetStr;
*SetInt = *cspacec::iConfigFile_SetInt;
*SetFloat = *cspacec::iConfigFile_SetFloat;
*SetBool = *cspacec::iConfigFile_SetBool;
*SetTuple = *cspacec::iConfigFile_SetTuple;
*SetComment = *cspacec::iConfigFile_SetComment;
*DeleteKey = *cspacec::iConfigFile_DeleteKey;
*GetEOFComment = *cspacec::iConfigFile_GetEOFComment;
*SetEOFComment = *cspacec::iConfigFile_SetEOFComment;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iConfigFile($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iConfigFile_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iConfigIterator ##############

package cspace::iConfigIterator;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetConfigFile = *cspacec::iConfigIterator_GetConfigFile;
*GetSubsection = *cspacec::iConfigIterator_GetSubsection;
*Rewind = *cspacec::iConfigIterator_Rewind;
*Next = *cspacec::iConfigIterator_Next;
*HasNext = *cspacec::iConfigIterator_HasNext;
*GetKey = *cspacec::iConfigIterator_GetKey;
*GetInt = *cspacec::iConfigIterator_GetInt;
*GetFloat = *cspacec::iConfigIterator_GetFloat;
*GetStr = *cspacec::iConfigIterator_GetStr;
*GetBool = *cspacec::iConfigIterator_GetBool;
*GetTuple = *cspacec::iConfigIterator_GetTuple;
*GetComment = *cspacec::iConfigIterator_GetComment;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iConfigIterator($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iConfigIterator_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iConfigManager ##############

package cspace::iConfigManager;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iConfigFile cspace );
%OWNER = ();
%ITERATORS = ();
*PriorityMin = *cspacec::iConfigManager_PriorityMin;
*PriorityVeryLow = *cspacec::iConfigManager_PriorityVeryLow;
*PriorityLow = *cspacec::iConfigManager_PriorityLow;
*PriorityMedium = *cspacec::iConfigManager_PriorityMedium;
*PriorityHigh = *cspacec::iConfigManager_PriorityHigh;
*PriorityVeryHigh = *cspacec::iConfigManager_PriorityVeryHigh;
*PriorityMax = *cspacec::iConfigManager_PriorityMax;
*ConfigPriorityPlugin = *cspacec::iConfigManager_ConfigPriorityPlugin;
*ConfigPriorityApplication = *cspacec::iConfigManager_ConfigPriorityApplication;
*ConfigPriorityUserGlobal = *cspacec::iConfigManager_ConfigPriorityUserGlobal;
*ConfigPriorityUserApp = *cspacec::iConfigManager_ConfigPriorityUserApp;
*ConfigPriorityCmdLine = *cspacec::iConfigManager_ConfigPriorityCmdLine;
*AddDomain = *cspacec::iConfigManager_AddDomain;
*RemoveDomain = *cspacec::iConfigManager_RemoveDomain;
*LookupDomain = *cspacec::iConfigManager_LookupDomain;
*SetDomainPriority = *cspacec::iConfigManager_SetDomainPriority;
*GetDomainPriority = *cspacec::iConfigManager_GetDomainPriority;
*SetDynamicDomain = *cspacec::iConfigManager_SetDynamicDomain;
*GetDynamicDomain = *cspacec::iConfigManager_GetDynamicDomain;
*SetDynamicDomainPriority = *cspacec::iConfigManager_SetDynamicDomainPriority;
*GetDynamicDomainPriority = *cspacec::iConfigManager_GetDynamicDomainPriority;
*FlushRemoved = *cspacec::iConfigManager_FlushRemoved;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iConfigManager($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iConfigManager_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iStringArray ##############

package cspace::iStringArray;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetSize = *cspacec::iStringArray_GetSize;
*Length = *cspacec::iStringArray_Length;
*Push = *cspacec::iStringArray_Push;
*Pop = *cspacec::iStringArray_Pop;
*Get = *cspacec::iStringArray_Get;
*Find = *cspacec::iStringArray_Find;
*FindCaseInsensitive = *cspacec::iStringArray_FindCaseInsensitive;
*FindSortedKey = *cspacec::iStringArray_FindSortedKey;
*Contains = *cspacec::iStringArray_Contains;
*Sort = *cspacec::iStringArray_Sort;
*DeleteIndex = *cspacec::iStringArray_DeleteIndex;
*Insert = *cspacec::iStringArray_Insert;
*Empty = *cspacec::iStringArray_Empty;
*DeleteAll = *cspacec::iStringArray_DeleteAll;
*IsEmpty = *cspacec::iStringArray_IsEmpty;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iStringArray($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iStringArray_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDocumentAttributeIterator ##############

package cspace::iDocumentAttributeIterator;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*HasNext = *cspacec::iDocumentAttributeIterator_HasNext;
*Next = *cspacec::iDocumentAttributeIterator_Next;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDocumentAttributeIterator($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iDocumentAttributeIterator_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDocumentAttribute ##############

package cspace::iDocumentAttribute;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetName = *cspacec::iDocumentAttribute_GetName;
*GetValue = *cspacec::iDocumentAttribute_GetValue;
*GetValueAsInt = *cspacec::iDocumentAttribute_GetValueAsInt;
*GetValueAsFloat = *cspacec::iDocumentAttribute_GetValueAsFloat;
*GetValueAsBool = *cspacec::iDocumentAttribute_GetValueAsBool;
*SetName = *cspacec::iDocumentAttribute_SetName;
*SetValue = *cspacec::iDocumentAttribute_SetValue;
*SetValueAsInt = *cspacec::iDocumentAttribute_SetValueAsInt;
*SetValueAsFloat = *cspacec::iDocumentAttribute_SetValueAsFloat;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDocumentAttribute($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iDocumentAttribute_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDocumentNodeIterator ##############

package cspace::iDocumentNodeIterator;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*HasNext = *cspacec::iDocumentNodeIterator_HasNext;
*Next = *cspacec::iDocumentNodeIterator_Next;
*GetNextPosition = *cspacec::iDocumentNodeIterator_GetNextPosition;
*GetEndPosition = *cspacec::iDocumentNodeIterator_GetEndPosition;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDocumentNodeIterator($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iDocumentNodeIterator_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDocumentNode ##############

package cspace::iDocumentNode;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetType = *cspacec::iDocumentNode_GetType;
*Equals = *cspacec::iDocumentNode_Equals;
*GetValue = *cspacec::iDocumentNode_GetValue;
*SetValue = *cspacec::iDocumentNode_SetValue;
*SetValueAsInt = *cspacec::iDocumentNode_SetValueAsInt;
*SetValueAsFloat = *cspacec::iDocumentNode_SetValueAsFloat;
*GetParent = *cspacec::iDocumentNode_GetParent;
*GetNodes = *cspacec::iDocumentNode_GetNodes;
*GetNode = *cspacec::iDocumentNode_GetNode;
*RemoveNode = *cspacec::iDocumentNode_RemoveNode;
*RemoveNodes = *cspacec::iDocumentNode_RemoveNodes;
*CreateNodeBefore = *cspacec::iDocumentNode_CreateNodeBefore;
*GetContentsValue = *cspacec::iDocumentNode_GetContentsValue;
*GetContentsValueAsInt = *cspacec::iDocumentNode_GetContentsValueAsInt;
*GetContentsValueAsFloat = *cspacec::iDocumentNode_GetContentsValueAsFloat;
*GetAttributes = *cspacec::iDocumentNode_GetAttributes;
*GetAttribute = *cspacec::iDocumentNode_GetAttribute;
*GetAttributeValue = *cspacec::iDocumentNode_GetAttributeValue;
*GetAttributeValueAsInt = *cspacec::iDocumentNode_GetAttributeValueAsInt;
*GetAttributeValueAsFloat = *cspacec::iDocumentNode_GetAttributeValueAsFloat;
*GetAttributeValueAsBool = *cspacec::iDocumentNode_GetAttributeValueAsBool;
*RemoveAttribute = *cspacec::iDocumentNode_RemoveAttribute;
*RemoveAttributes = *cspacec::iDocumentNode_RemoveAttributes;
*SetAttribute = *cspacec::iDocumentNode_SetAttribute;
*SetAttributeAsInt = *cspacec::iDocumentNode_SetAttributeAsInt;
*SetAttributeAsFloat = *cspacec::iDocumentNode_SetAttributeAsFloat;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDocumentNode($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iDocumentNode_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDocument ##############

package cspace::iDocument;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Clear = *cspacec::iDocument_Clear;
*CreateRoot = *cspacec::iDocument_CreateRoot;
*GetRoot = *cspacec::iDocument_GetRoot;
*Parse = *cspacec::iDocument_Parse;
*Write = *cspacec::iDocument_Write;
*Changeable = *cspacec::iDocument_Changeable;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDocument($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iDocument_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDocumentSystem ##############

package cspace::iDocumentSystem;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*CreateDocument = *cspacec::iDocumentSystem_CreateDocument;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDocumentSystem($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iDocumentSystem_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::scfConfigFile ##############

package cspace::scfConfigFile;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iConfigFile cspace );
%OWNER = ();
*IncRef = *cspacec::scfConfigFile_IncRef;
*DecRef = *cspacec::scfConfigFile_DecRef;
*GetRefCount = *cspacec::scfConfigFile_GetRefCount;
*QueryInterface = *cspacec::scfConfigFile_QueryInterface;
*AddRefOwner = *cspacec::scfConfigFile_AddRefOwner;
*RemoveRefOwner = *cspacec::scfConfigFile_RemoveRefOwner;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csConfigFile ##############

package cspace::csConfigFile;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::scfConfigFile cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csConfigFile(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csConfigFile($self);
        delete $OWNER{$self};
    }
}

*IsEmpty = *cspacec::csConfigFile_IsEmpty;
*GetFileName = *cspacec::csConfigFile_GetFileName;
*GetVFS = *cspacec::csConfigFile_GetVFS;
*SetFileName = *cspacec::csConfigFile_SetFileName;
*Load = *cspacec::csConfigFile_Load;
*LoadFromBuffer = *cspacec::csConfigFile_LoadFromBuffer;
*Save = *cspacec::csConfigFile_Save;
*Clear = *cspacec::csConfigFile_Clear;
*Enumerate = *cspacec::csConfigFile_Enumerate;
*KeyExists = *cspacec::csConfigFile_KeyExists;
*SubsectionExists = *cspacec::csConfigFile_SubsectionExists;
*GetInt = *cspacec::csConfigFile_GetInt;
*GetFloat = *cspacec::csConfigFile_GetFloat;
*GetStr = *cspacec::csConfigFile_GetStr;
*GetBool = *cspacec::csConfigFile_GetBool;
*GetTuple = *cspacec::csConfigFile_GetTuple;
*GetComment = *cspacec::csConfigFile_GetComment;
*SetStr = *cspacec::csConfigFile_SetStr;
*SetInt = *cspacec::csConfigFile_SetInt;
*SetFloat = *cspacec::csConfigFile_SetFloat;
*SetBool = *cspacec::csConfigFile_SetBool;
*SetTuple = *cspacec::csConfigFile_SetTuple;
*SetComment = *cspacec::csConfigFile_SetComment;
*DeleteKey = *cspacec::csConfigFile_DeleteKey;
*SetEOFComment = *cspacec::csConfigFile_SetEOFComment;
*GetEOFComment = *cspacec::csConfigFile_GetEOFComment;
*ParseCommandLine = *cspacec::csConfigFile_ParseCommandLine;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csRadixSorter ##############

package cspace::csRadixSorter;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csRadixSorter(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csRadixSorter($self);
        delete $OWNER{$self};
    }
}

*Sort = *cspacec::csRadixSorter_Sort;
*GetRanks = *cspacec::csRadixSorter_GetRanks;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csTinyDocumentSystem ##############

package cspace::csTinyDocumentSystem;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csTinyDocumentSystem(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csTinyDocumentSystem($self);
        delete $OWNER{$self};
    }
}

*CreateDocument = *cspacec::csTinyDocumentSystem_CreateDocument;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDataBuffer ##############

package cspace::iDataBuffer;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetSize = *cspacec::iDataBuffer_GetSize;
*GetData = *cspacec::iDataBuffer_GetData;
*__ref__ = *cspacec::iDataBuffer___ref__;
*GetUint8 = *cspacec::iDataBuffer_GetUint8;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDataBuffer($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iDataBuffer_scfGetVersion;
*__string__ = *cspacec::iDataBuffer___string__;
*__sv__ = *cspacec::iDataBuffer___sv__;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPixelCoord ##############

package cspace::csPixelCoord;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_x_get = *cspacec::csPixelCoord_x_get;
*swig_x_set = *cspacec::csPixelCoord_x_set;
*swig_y_get = *cspacec::csPixelCoord_y_get;
*swig_y_set = *cspacec::csPixelCoord_y_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPixelCoord(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPixelCoord($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPixelFormat ##############

package cspace::csPixelFormat;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_RedMask_get = *cspacec::csPixelFormat_RedMask_get;
*swig_RedMask_set = *cspacec::csPixelFormat_RedMask_set;
*swig_GreenMask_get = *cspacec::csPixelFormat_GreenMask_get;
*swig_GreenMask_set = *cspacec::csPixelFormat_GreenMask_set;
*swig_BlueMask_get = *cspacec::csPixelFormat_BlueMask_get;
*swig_BlueMask_set = *cspacec::csPixelFormat_BlueMask_set;
*swig_AlphaMask_get = *cspacec::csPixelFormat_AlphaMask_get;
*swig_AlphaMask_set = *cspacec::csPixelFormat_AlphaMask_set;
*swig_RedShift_get = *cspacec::csPixelFormat_RedShift_get;
*swig_RedShift_set = *cspacec::csPixelFormat_RedShift_set;
*swig_GreenShift_get = *cspacec::csPixelFormat_GreenShift_get;
*swig_GreenShift_set = *cspacec::csPixelFormat_GreenShift_set;
*swig_BlueShift_get = *cspacec::csPixelFormat_BlueShift_get;
*swig_BlueShift_set = *cspacec::csPixelFormat_BlueShift_set;
*swig_AlphaShift_get = *cspacec::csPixelFormat_AlphaShift_get;
*swig_AlphaShift_set = *cspacec::csPixelFormat_AlphaShift_set;
*swig_RedBits_get = *cspacec::csPixelFormat_RedBits_get;
*swig_RedBits_set = *cspacec::csPixelFormat_RedBits_set;
*swig_GreenBits_get = *cspacec::csPixelFormat_GreenBits_get;
*swig_GreenBits_set = *cspacec::csPixelFormat_GreenBits_set;
*swig_BlueBits_get = *cspacec::csPixelFormat_BlueBits_get;
*swig_BlueBits_set = *cspacec::csPixelFormat_BlueBits_set;
*swig_AlphaBits_get = *cspacec::csPixelFormat_AlphaBits_get;
*swig_AlphaBits_set = *cspacec::csPixelFormat_AlphaBits_set;
*swig_PalEntries_get = *cspacec::csPixelFormat_PalEntries_get;
*swig_PalEntries_set = *cspacec::csPixelFormat_PalEntries_set;
*swig_PixelBytes_get = *cspacec::csPixelFormat_PixelBytes_get;
*swig_PixelBytes_set = *cspacec::csPixelFormat_PixelBytes_set;
*complete = *cspacec::csPixelFormat_complete;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPixelFormat(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPixelFormat($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csImageArea ##############

package cspace::csImageArea;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_x_get = *cspacec::csImageArea_x_get;
*swig_x_set = *cspacec::csImageArea_x_set;
*swig_y_get = *cspacec::csImageArea_y_get;
*swig_y_set = *cspacec::csImageArea_y_set;
*swig_w_get = *cspacec::csImageArea_w_get;
*swig_w_set = *cspacec::csImageArea_w_set;
*swig_h_get = *cspacec::csImageArea_h_get;
*swig_h_set = *cspacec::csImageArea_h_set;
*swig_data_get = *cspacec::csImageArea_data_get;
*swig_data_set = *cspacec::csImageArea_data_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csImageArea(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csImageArea($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iOffscreenCanvasCallback ##############

package cspace::iOffscreenCanvasCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*FinishDraw = *cspacec::iOffscreenCanvasCallback_FinishDraw;
*SetRGB = *cspacec::iOffscreenCanvasCallback_SetRGB;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iOffscreenCanvasCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iGraphics2D ##############

package cspace::iGraphics2D;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Open = *cspacec::iGraphics2D_Open;
*Close = *cspacec::iGraphics2D_Close;
*GetWidth = *cspacec::iGraphics2D_GetWidth;
*GetHeight = *cspacec::iGraphics2D_GetHeight;
*GetPage = *cspacec::iGraphics2D_GetPage;
*DoubleBuffer = *cspacec::iGraphics2D_DoubleBuffer;
*GetDoubleBufferState = *cspacec::iGraphics2D_GetDoubleBufferState;
*GetPixelFormat = *cspacec::iGraphics2D_GetPixelFormat;
*GetPixelBytes = *cspacec::iGraphics2D_GetPixelBytes;
*GetPalEntryCount = *cspacec::iGraphics2D_GetPalEntryCount;
*GetPalette = *cspacec::iGraphics2D_GetPalette;
*SetRGB = *cspacec::iGraphics2D_SetRGB;
*FindRGB = *cspacec::iGraphics2D_FindRGB;
*GetRGB = *cspacec::iGraphics2D_GetRGB;
*GetRGBA = *cspacec::iGraphics2D_GetRGBA;
*SetClipRect = *cspacec::iGraphics2D_SetClipRect;
*GetClipRect = *cspacec::iGraphics2D_GetClipRect;
*BeginDraw = *cspacec::iGraphics2D_BeginDraw;
*FinishDraw = *cspacec::iGraphics2D_FinishDraw;
*Print = *cspacec::iGraphics2D_Print;
*Clear = *cspacec::iGraphics2D_Clear;
*ClearAll = *cspacec::iGraphics2D_ClearAll;
*DrawLine = *cspacec::iGraphics2D_DrawLine;
*DrawBox = *cspacec::iGraphics2D_DrawBox;
*ClipLine = *cspacec::iGraphics2D_ClipLine;
*DrawPixel = *cspacec::iGraphics2D_DrawPixel;
*DrawPixels = *cspacec::iGraphics2D_DrawPixels;
*Blit = *cspacec::iGraphics2D_Blit;
*GetPixelAt = *cspacec::iGraphics2D_GetPixelAt;
*GetPixel = *cspacec::iGraphics2D_GetPixel;
*SaveArea = *cspacec::iGraphics2D_SaveArea;
*RestoreArea = *cspacec::iGraphics2D_RestoreArea;
*FreeArea = *cspacec::iGraphics2D_FreeArea;
*WriteBaseline = *cspacec::iGraphics2D_WriteBaseline;
*AllowResize = *cspacec::iGraphics2D_AllowResize;
*Resize = *cspacec::iGraphics2D_Resize;
*GetFontServer = *cspacec::iGraphics2D_GetFontServer;
*PerformExtension = *cspacec::iGraphics2D_PerformExtension;
*ScreenShot = *cspacec::iGraphics2D_ScreenShot;
*GetNativeWindow = *cspacec::iGraphics2D_GetNativeWindow;
*GetFullScreen = *cspacec::iGraphics2D_GetFullScreen;
*SetFullScreen = *cspacec::iGraphics2D_SetFullScreen;
*SetMousePosition = *cspacec::iGraphics2D_SetMousePosition;
*SetMouseCursor = *cspacec::iGraphics2D_SetMouseCursor;
*SetGamma = *cspacec::iGraphics2D_SetGamma;
*GetGamma = *cspacec::iGraphics2D_GetGamma;
*GetName = *cspacec::iGraphics2D_GetName;
*CreateOffscreenCanvas = *cspacec::iGraphics2D_CreateOffscreenCanvas;
*Write = *cspacec::iGraphics2D_Write;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iGraphics2D($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iGraphics2D_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csAlphaMode ##############

package cspace::csAlphaMode;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*alphaNone = *cspacec::csAlphaMode_alphaNone;
*alphaBinary = *cspacec::csAlphaMode_alphaBinary;
*alphaSmooth = *cspacec::csAlphaMode_alphaSmooth;
*swig_autoAlphaMode_get = *cspacec::csAlphaMode_autoAlphaMode_get;
*swig_autoAlphaMode_set = *cspacec::csAlphaMode_autoAlphaMode_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csAlphaMode(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csAlphaMode($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csGraphics3DCaps ##############

package cspace::csGraphics3DCaps;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_minTexHeight_get = *cspacec::csGraphics3DCaps_minTexHeight_get;
*swig_minTexHeight_set = *cspacec::csGraphics3DCaps_minTexHeight_set;
*swig_minTexWidth_get = *cspacec::csGraphics3DCaps_minTexWidth_get;
*swig_minTexWidth_set = *cspacec::csGraphics3DCaps_minTexWidth_set;
*swig_maxTexHeight_get = *cspacec::csGraphics3DCaps_maxTexHeight_get;
*swig_maxTexHeight_set = *cspacec::csGraphics3DCaps_maxTexHeight_set;
*swig_maxTexWidth_get = *cspacec::csGraphics3DCaps_maxTexWidth_get;
*swig_maxTexWidth_set = *cspacec::csGraphics3DCaps_maxTexWidth_set;
*swig_SupportsPointSprites_get = *cspacec::csGraphics3DCaps_SupportsPointSprites_get;
*swig_SupportsPointSprites_set = *cspacec::csGraphics3DCaps_SupportsPointSprites_set;
*swig_DestinationAlpha_get = *cspacec::csGraphics3DCaps_DestinationAlpha_get;
*swig_DestinationAlpha_set = *cspacec::csGraphics3DCaps_DestinationAlpha_set;
*swig_StencilShadows_get = *cspacec::csGraphics3DCaps_StencilShadows_get;
*swig_StencilShadows_set = *cspacec::csGraphics3DCaps_StencilShadows_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csGraphics3DCaps(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csGraphics3DCaps($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSimpleRenderMesh ##############

package cspace::csSimpleRenderMesh;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_meshtype_get = *cspacec::csSimpleRenderMesh_meshtype_get;
*swig_meshtype_set = *cspacec::csSimpleRenderMesh_meshtype_set;
*swig_indexCount_get = *cspacec::csSimpleRenderMesh_indexCount_get;
*swig_indexCount_set = *cspacec::csSimpleRenderMesh_indexCount_set;
*swig_indices_get = *cspacec::csSimpleRenderMesh_indices_get;
*swig_indices_set = *cspacec::csSimpleRenderMesh_indices_set;
*swig_vertexCount_get = *cspacec::csSimpleRenderMesh_vertexCount_get;
*swig_vertexCount_set = *cspacec::csSimpleRenderMesh_vertexCount_set;
*swig_vertices_get = *cspacec::csSimpleRenderMesh_vertices_get;
*swig_vertices_set = *cspacec::csSimpleRenderMesh_vertices_set;
*swig_texcoords_get = *cspacec::csSimpleRenderMesh_texcoords_get;
*swig_texcoords_set = *cspacec::csSimpleRenderMesh_texcoords_set;
*swig_colors_get = *cspacec::csSimpleRenderMesh_colors_get;
*swig_colors_set = *cspacec::csSimpleRenderMesh_colors_set;
*swig_texture_get = *cspacec::csSimpleRenderMesh_texture_get;
*swig_texture_set = *cspacec::csSimpleRenderMesh_texture_set;
*swig_shader_get = *cspacec::csSimpleRenderMesh_shader_get;
*swig_shader_set = *cspacec::csSimpleRenderMesh_shader_set;
*swig_dynDomain_get = *cspacec::csSimpleRenderMesh_dynDomain_get;
*swig_dynDomain_set = *cspacec::csSimpleRenderMesh_dynDomain_set;
*swig_alphaType_get = *cspacec::csSimpleRenderMesh_alphaType_get;
*swig_alphaType_set = *cspacec::csSimpleRenderMesh_alphaType_set;
*swig_z_buf_mode_get = *cspacec::csSimpleRenderMesh_z_buf_mode_get;
*swig_z_buf_mode_set = *cspacec::csSimpleRenderMesh_z_buf_mode_set;
*swig_mixmode_get = *cspacec::csSimpleRenderMesh_mixmode_get;
*swig_mixmode_set = *cspacec::csSimpleRenderMesh_mixmode_set;
*swig_object2world_get = *cspacec::csSimpleRenderMesh_object2world_get;
*swig_object2world_set = *cspacec::csSimpleRenderMesh_object2world_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csSimpleRenderMesh(@_);
    bless $self, $pkg if defined($self);
}

*SetWithGenmeshFactory = *cspacec::csSimpleRenderMesh_SetWithGenmeshFactory;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSimpleRenderMesh($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iGraphics3D ##############

package cspace::iGraphics3D;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Open = *cspacec::iGraphics3D_Open;
*Close = *cspacec::iGraphics3D_Close;
*GetDriver2D = *cspacec::iGraphics3D_GetDriver2D;
*GetTextureManager = *cspacec::iGraphics3D_GetTextureManager;
*SetDimensions = *cspacec::iGraphics3D_SetDimensions;
*GetWidth = *cspacec::iGraphics3D_GetWidth;
*GetHeight = *cspacec::iGraphics3D_GetHeight;
*GetCaps = *cspacec::iGraphics3D_GetCaps;
*SetPerspectiveCenter = *cspacec::iGraphics3D_SetPerspectiveCenter;
*GetPerspectiveCenter = *cspacec::iGraphics3D_GetPerspectiveCenter;
*SetPerspectiveAspect = *cspacec::iGraphics3D_SetPerspectiveAspect;
*GetPerspectiveAspect = *cspacec::iGraphics3D_GetPerspectiveAspect;
*SetRenderTarget = *cspacec::iGraphics3D_SetRenderTarget;
*GetRenderTarget = *cspacec::iGraphics3D_GetRenderTarget;
*BeginDraw = *cspacec::iGraphics3D_BeginDraw;
*FinishDraw = *cspacec::iGraphics3D_FinishDraw;
*Print = *cspacec::iGraphics3D_Print;
*DrawMesh = *cspacec::iGraphics3D_DrawMesh;
*DrawSimpleMesh = *cspacec::iGraphics3D_DrawSimpleMesh;
*DrawPixmap = *cspacec::iGraphics3D_DrawPixmap;
*DrawLine = *cspacec::iGraphics3D_DrawLine;
*ActivateBuffers = *cspacec::iGraphics3D_ActivateBuffers;
*DeactivateBuffers = *cspacec::iGraphics3D_DeactivateBuffers;
*SetTextureState = *cspacec::iGraphics3D_SetTextureState;
*SetClipper = *cspacec::iGraphics3D_SetClipper;
*GetClipper = *cspacec::iGraphics3D_GetClipper;
*GetClipType = *cspacec::iGraphics3D_GetClipType;
*SetNearPlane = *cspacec::iGraphics3D_SetNearPlane;
*ResetNearPlane = *cspacec::iGraphics3D_ResetNearPlane;
*GetNearPlane = *cspacec::iGraphics3D_GetNearPlane;
*HasNearPlane = *cspacec::iGraphics3D_HasNearPlane;
*SetRenderState = *cspacec::iGraphics3D_SetRenderState;
*GetRenderState = *cspacec::iGraphics3D_GetRenderState;
*SetOption = *cspacec::iGraphics3D_SetOption;
*SetWriteMask = *cspacec::iGraphics3D_SetWriteMask;
*GetWriteMask = *cspacec::iGraphics3D_GetWriteMask;
*SetZMode = *cspacec::iGraphics3D_SetZMode;
*GetZMode = *cspacec::iGraphics3D_GetZMode;
*EnableZOffset = *cspacec::iGraphics3D_EnableZOffset;
*DisableZOffset = *cspacec::iGraphics3D_DisableZOffset;
*SetShadowState = *cspacec::iGraphics3D_SetShadowState;
*GetZBuffValue = *cspacec::iGraphics3D_GetZBuffValue;
*OpenPortal = *cspacec::iGraphics3D_OpenPortal;
*ClosePortal = *cspacec::iGraphics3D_ClosePortal;
*CreateHalo = *cspacec::iGraphics3D_CreateHalo;
*RemoveFromCache = *cspacec::iGraphics3D_RemoveFromCache;
*SetWorldToCamera = *cspacec::iGraphics3D_SetWorldToCamera;
*PerformExtension = *cspacec::iGraphics3D_PerformExtension;
*GetWorldToCamera = *cspacec::iGraphics3D_GetWorldToCamera;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iGraphics3D($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iGraphics3D_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iNativeWindowManager ##############

package cspace::iNativeWindowManager;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Alert = *cspacec::iNativeWindowManager_Alert;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iNativeWindowManager($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iNativeWindow ##############

package cspace::iNativeWindow;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetTitle = *cspacec::iNativeWindow_SetTitle;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iNativeWindow($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iFontDeleteNotify ##############

package cspace::iFontDeleteNotify;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*BeforeDelete = *cspacec::iFontDeleteNotify_BeforeDelete;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iFontDeleteNotify($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csBitmapMetrics ##############

package cspace::csBitmapMetrics;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_width_get = *cspacec::csBitmapMetrics_width_get;
*swig_width_set = *cspacec::csBitmapMetrics_width_set;
*swig_height_get = *cspacec::csBitmapMetrics_height_get;
*swig_height_set = *cspacec::csBitmapMetrics_height_set;
*swig_left_get = *cspacec::csBitmapMetrics_left_get;
*swig_left_set = *cspacec::csBitmapMetrics_left_set;
*swig_top_get = *cspacec::csBitmapMetrics_top_get;
*swig_top_set = *cspacec::csBitmapMetrics_top_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csBitmapMetrics(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csBitmapMetrics($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csGlyphMetrics ##############

package cspace::csGlyphMetrics;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_advance_get = *cspacec::csGlyphMetrics_advance_get;
*swig_advance_set = *cspacec::csGlyphMetrics_advance_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csGlyphMetrics(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csGlyphMetrics($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iFont ##############

package cspace::iFont;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*AddDeleteCallback = *cspacec::iFont_AddDeleteCallback;
*RemoveDeleteCallback = *cspacec::iFont_RemoveDeleteCallback;
*GetSize = *cspacec::iFont_GetSize;
*GetMaxSize = *cspacec::iFont_GetMaxSize;
*GetGlyphMetrics = *cspacec::iFont_GetGlyphMetrics;
*GetGlyphBitmap = *cspacec::iFont_GetGlyphBitmap;
*GetGlyphAlphaBitmap = *cspacec::iFont_GetGlyphAlphaBitmap;
*GetDimensions = *cspacec::iFont_GetDimensions;
*GetLength = *cspacec::iFont_GetLength;
*GetDescent = *cspacec::iFont_GetDescent;
*GetAscent = *cspacec::iFont_GetAscent;
*HasGlyph = *cspacec::iFont_HasGlyph;
*GetTextHeight = *cspacec::iFont_GetTextHeight;
*GetUnderlinePosition = *cspacec::iFont_GetUnderlinePosition;
*GetUnderlineThickness = *cspacec::iFont_GetUnderlineThickness;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iFont($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iFont_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iFontServer ##############

package cspace::iFontServer;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*LoadFont = *cspacec::iFontServer_LoadFont;
*SetWarnOnError = *cspacec::iFontServer_SetWarnOnError;
*GetWarnOnError = *cspacec::iFontServer_GetWarnOnError;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iFontServer($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iFontServer_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iHalo ##############

package cspace::iHalo;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetWidth = *cspacec::iHalo_GetWidth;
*GetHeight = *cspacec::iHalo_GetHeight;
*SetColor = *cspacec::iHalo_SetColor;
*GetColor = *cspacec::iHalo_GetColor;
*Draw = *cspacec::iHalo_Draw;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iHalo($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iHalo_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iShaderVarStack ##############

package cspace::iShaderVarStack;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csShaderVariableArray cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iShaderVarStack($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iShaderVariableContext ##############

package cspace::iShaderVariableContext;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*AddVariable = *cspacec::iShaderVariableContext_AddVariable;
*GetVariable = *cspacec::iShaderVariableContext_GetVariable;
*GetVariableAdd = *cspacec::iShaderVariableContext_GetVariableAdd;
*GetShaderVariables = *cspacec::iShaderVariableContext_GetShaderVariables;
*PushVariables = *cspacec::iShaderVariableContext_PushVariables;
*IsEmpty = *cspacec::iShaderVariableContext_IsEmpty;
*ReplaceVariable = *cspacec::iShaderVariableContext_ReplaceVariable;
*Clear = *cspacec::iShaderVariableContext_Clear;
*RemoveVariable = *cspacec::iShaderVariableContext_RemoveVariable;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iShaderVariableContext($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iShaderVariableContext_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iShaderManager ##############

package cspace::iShaderManager;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iShaderVariableContext cspace );
%OWNER = ();
%ITERATORS = ();
*RegisterShader = *cspacec::iShaderManager_RegisterShader;
*UnregisterShader = *cspacec::iShaderManager_UnregisterShader;
*UnregisterShaders = *cspacec::iShaderManager_UnregisterShaders;
*GetShader = *cspacec::iShaderManager_GetShader;
*GetShaders = *cspacec::iShaderManager_GetShaders;
*RegisterCompiler = *cspacec::iShaderManager_RegisterCompiler;
*GetCompiler = *cspacec::iShaderManager_GetCompiler;
*RegisterShaderVariableAccessor = *cspacec::iShaderManager_RegisterShaderVariableAccessor;
*UnregisterShaderVariableAccessor = *cspacec::iShaderManager_UnregisterShaderVariableAccessor;
*GetShaderVariableAccessor = *cspacec::iShaderManager_GetShaderVariableAccessor;
*UnregisterShaderVariableAcessors = *cspacec::iShaderManager_UnregisterShaderVariableAcessors;
*GetShaderVariableStack = *cspacec::iShaderManager_GetShaderVariableStack;
*SetTagOptions = *cspacec::iShaderManager_SetTagOptions;
*GetTagOptions = *cspacec::iShaderManager_GetTagOptions;
*GetTags = *cspacec::iShaderManager_GetTags;
*SetActiveLights = *cspacec::iShaderManager_SetActiveLights;
*GetActiveLights = *cspacec::iShaderManager_GetActiveLights;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iShaderManager($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iShaderManager_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csShaderMetadata ##############

package cspace::csShaderMetadata;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_description_get = *cspacec::csShaderMetadata_description_get;
*swig_description_set = *cspacec::csShaderMetadata_description_set;
*swig_numberOfLights_get = *cspacec::csShaderMetadata_numberOfLights_get;
*swig_numberOfLights_set = *cspacec::csShaderMetadata_numberOfLights_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csShaderMetadata(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csShaderMetadata($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iShader ##############

package cspace::iShader;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iShaderVariableContext cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iShader_QueryObject;
*GetFileName = *cspacec::iShader_GetFileName;
*SetFileName = *cspacec::iShader_SetFileName;
*GetTicket = *cspacec::iShader_GetTicket;
*GetNumberOfPasses = *cspacec::iShader_GetNumberOfPasses;
*ActivatePass = *cspacec::iShader_ActivatePass;
*SetupPass = *cspacec::iShader_SetupPass;
*TeardownPass = *cspacec::iShader_TeardownPass;
*DeactivatePass = *cspacec::iShader_DeactivatePass;
*GetMetadata = *cspacec::iShader_GetMetadata;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iShader($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iShader_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iShaderPriorityList ##############

package cspace::iShaderPriorityList;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetCount = *cspacec::iShaderPriorityList_GetCount;
*GetPriority = *cspacec::iShaderPriorityList_GetPriority;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iShaderPriorityList($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iShaderCompiler ##############

package cspace::iShaderCompiler;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetName = *cspacec::iShaderCompiler_GetName;
*CompileShader = *cspacec::iShaderCompiler_CompileShader;
*ValidateTemplate = *cspacec::iShaderCompiler_ValidateTemplate;
*IsTemplateToCompiler = *cspacec::iShaderCompiler_IsTemplateToCompiler;
*GetPriorities = *cspacec::iShaderCompiler_GetPriorities;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iShaderCompiler($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iTextureHandle ##############

package cspace::iTextureHandle;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetFlags = *cspacec::iTextureHandle_GetFlags;
*SetKeyColor = *cspacec::iTextureHandle_SetKeyColor;
*GetKeyColorStatus = *cspacec::iTextureHandle_GetKeyColorStatus;
*GetKeyColor = *cspacec::iTextureHandle_GetKeyColor;
*CS_TEX_IMG_1D = *cspacec::iTextureHandle_CS_TEX_IMG_1D;
*CS_TEX_IMG_2D = *cspacec::iTextureHandle_CS_TEX_IMG_2D;
*CS_TEX_IMG_3D = *cspacec::iTextureHandle_CS_TEX_IMG_3D;
*CS_TEX_IMG_CUBEMAP = *cspacec::iTextureHandle_CS_TEX_IMG_CUBEMAP;
*CS_TEX_IMG_RECT = *cspacec::iTextureHandle_CS_TEX_IMG_RECT;
*CS_TEXTURE_CUBE_POS_X = *cspacec::iTextureHandle_CS_TEXTURE_CUBE_POS_X;
*CS_TEXTURE_CUBE_NEG_X = *cspacec::iTextureHandle_CS_TEXTURE_CUBE_NEG_X;
*CS_TEXTURE_CUBE_POS_Y = *cspacec::iTextureHandle_CS_TEXTURE_CUBE_POS_Y;
*CS_TEXTURE_CUBE_NEG_Y = *cspacec::iTextureHandle_CS_TEXTURE_CUBE_NEG_Y;
*CS_TEXTURE_CUBE_POS_Z = *cspacec::iTextureHandle_CS_TEXTURE_CUBE_POS_Z;
*CS_TEXTURE_CUBE_NEG_Z = *cspacec::iTextureHandle_CS_TEXTURE_CUBE_NEG_Z;
*GetRendererDimensions = *cspacec::iTextureHandle_GetRendererDimensions;
*GetOriginalDimensions = *cspacec::iTextureHandle_GetOriginalDimensions;
*GetTextureTarget = *cspacec::iTextureHandle_GetTextureTarget;
*RGBA8888 = *cspacec::iTextureHandle_RGBA8888;
*BGRA8888 = *cspacec::iTextureHandle_BGRA8888;
*Blit = *cspacec::iTextureHandle_Blit;
*GetImageName = *cspacec::iTextureHandle_GetImageName;
*GetPrivateObject = *cspacec::iTextureHandle_GetPrivateObject;
*GetAlphaMap = *cspacec::iTextureHandle_GetAlphaMap;
*GetAlphaType = *cspacec::iTextureHandle_GetAlphaType;
*Precache = *cspacec::iTextureHandle_Precache;
*SetTextureClass = *cspacec::iTextureHandle_SetTextureClass;
*GetTextureClass = *cspacec::iTextureHandle_GetTextureClass;
*SetAlphaType = *cspacec::iTextureHandle_SetAlphaType;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iTextureHandle($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iTextureHandle_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iRendererLightmap ##############

package cspace::iRendererLightmap;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetSLMCoords = *cspacec::iRendererLightmap_GetSLMCoords;
*SetData = *cspacec::iRendererLightmap_SetData;
*SetLightCellSize = *cspacec::iRendererLightmap_SetLightCellSize;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iRendererLightmap($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSuperLightmap ##############

package cspace::iSuperLightmap;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*RegisterLightmap = *cspacec::iSuperLightmap_RegisterLightmap;
*Dump = *cspacec::iSuperLightmap_Dump;
*GetTexture = *cspacec::iSuperLightmap_GetTexture;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSuperLightmap($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iTextureManager ##############

package cspace::iTextureManager;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*RegisterTexture = *cspacec::iTextureManager_RegisterTexture;
*CreateTexture = *cspacec::iTextureManager_CreateTexture;
*GetTextureFormat = *cspacec::iTextureManager_GetTextureFormat;
*CreateSuperLightmap = *cspacec::iTextureManager_CreateSuperLightmap;
*GetMaxTextureSize = *cspacec::iTextureManager_GetMaxTextureSize;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iTextureManager($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iTextureManager_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMaterial ##############

package cspace::iMaterial;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iShaderVariableContext cspace );
%OWNER = ();
%ITERATORS = ();
*SetShader = *cspacec::iMaterial_SetShader;
*GetShader = *cspacec::iMaterial_GetShader;
*GetShaders = *cspacec::iMaterial_GetShaders;
*GetTexture = *cspacec::iMaterial_GetTexture;
*GetFlatColor = *cspacec::iMaterial_GetFlatColor;
*SetFlatColor = *cspacec::iMaterial_SetFlatColor;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMaterial($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iMaterial_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iImage ##############

package cspace::iImage;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetImageData = *cspacec::iImage_GetImageData;
*GetWidth = *cspacec::iImage_GetWidth;
*GetHeight = *cspacec::iImage_GetHeight;
*GetDepth = *cspacec::iImage_GetDepth;
*SetName = *cspacec::iImage_SetName;
*GetName = *cspacec::iImage_GetName;
*GetFormat = *cspacec::iImage_GetFormat;
*GetPalette = *cspacec::iImage_GetPalette;
*GetAlpha = *cspacec::iImage_GetAlpha;
*HasKeyColor = *cspacec::iImage_HasKeyColor;
*GetKeyColor = *cspacec::iImage_GetKeyColor;
*HasMipmaps = *cspacec::iImage_HasMipmaps;
*GetMipmap = *cspacec::iImage_GetMipmap;
*GetRawFormat = *cspacec::iImage_GetRawFormat;
*GetRawData = *cspacec::iImage_GetRawData;
*GetImageType = *cspacec::iImage_GetImageType;
*HasSubImages = *cspacec::iImage_HasSubImages;
*GetSubImage = *cspacec::iImage_GetSubImage;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iImage($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iImage_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csImageBaseBase ##############

package cspace::csImageBaseBase;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iImage cspace );
%OWNER = ();
*IncRef = *cspacec::csImageBaseBase_IncRef;
*DecRef = *cspacec::csImageBaseBase_DecRef;
*GetRefCount = *cspacec::csImageBaseBase_GetRefCount;
*QueryInterface = *cspacec::csImageBaseBase_QueryInterface;
*AddRefOwner = *cspacec::csImageBaseBase_AddRefOwner;
*RemoveRefOwner = *cspacec::csImageBaseBase_RemoveRefOwner;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csImageBase ##############

package cspace::csImageBase;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csImageBaseBase cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csImageBase($self);
        delete $OWNER{$self};
    }
}

*GetDepth = *cspacec::csImageBase_GetDepth;
*SetName = *cspacec::csImageBase_SetName;
*GetName = *cspacec::csImageBase_GetName;
*GetPalette = *cspacec::csImageBase_GetPalette;
*GetAlpha = *cspacec::csImageBase_GetAlpha;
*HasKeyColor = *cspacec::csImageBase_HasKeyColor;
*GetKeyColor = *cspacec::csImageBase_GetKeyColor;
*HasMipmaps = *cspacec::csImageBase_HasMipmaps;
*GetMipmap = *cspacec::csImageBase_GetMipmap;
*GetRawFormat = *cspacec::csImageBase_GetRawFormat;
*GetRawData = *cspacec::csImageBase_GetRawData;
*GetImageType = *cspacec::csImageBase_GetImageType;
*HasSubImages = *cspacec::csImageBase_HasSubImages;
*GetSubImage = *cspacec::csImageBase_GetSubImage;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csImageMemoryBase ##############

package cspace::csImageMemoryBase;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csImageBase cspace );
%OWNER = ();
*IncRef = *cspacec::csImageMemoryBase_IncRef;
*DecRef = *cspacec::csImageMemoryBase_DecRef;
*GetRefCount = *cspacec::csImageMemoryBase_GetRefCount;
*QueryInterface = *cspacec::csImageMemoryBase_QueryInterface;
*AddRefOwner = *cspacec::csImageMemoryBase_AddRefOwner;
*RemoveRefOwner = *cspacec::csImageMemoryBase_RemoveRefOwner;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csImageMemory ##############

package cspace::csImageMemory;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csImageMemoryBase cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csImageMemory(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csImageMemory($self);
        delete $OWNER{$self};
    }
}

*GetImagePtr = *cspacec::csImageMemory_GetImagePtr;
*GetPalettePtr = *cspacec::csImageMemory_GetPalettePtr;
*GetAlphaPtr = *cspacec::csImageMemory_GetAlphaPtr;
*GetImageData = *cspacec::csImageMemory_GetImageData;
*GetWidth = *cspacec::csImageMemory_GetWidth;
*GetHeight = *cspacec::csImageMemory_GetHeight;
*GetDepth = *cspacec::csImageMemory_GetDepth;
*GetRawFormat = *cspacec::csImageMemory_GetRawFormat;
*GetRawData = *cspacec::csImageMemory_GetRawData;
*GetFormat = *cspacec::csImageMemory_GetFormat;
*GetPalette = *cspacec::csImageMemory_GetPalette;
*GetAlpha = *cspacec::csImageMemory_GetAlpha;
*HasKeyColor = *cspacec::csImageMemory_HasKeyColor;
*GetKeyColor = *cspacec::csImageMemory_GetKeyColor;
*Clear = *cspacec::csImageMemory_Clear;
*CheckAlpha = *cspacec::csImageMemory_CheckAlpha;
*SetFormat = *cspacec::csImageMemory_SetFormat;
*SetKeyColor = *cspacec::csImageMemory_SetKeyColor;
*SetKeycolor = *cspacec::csImageMemory_SetKeycolor;
*ClearKeyColor = *cspacec::csImageMemory_ClearKeyColor;
*ClearKeycolor = *cspacec::csImageMemory_ClearKeycolor;
*ApplyKeyColor = *cspacec::csImageMemory_ApplyKeyColor;
*ApplyKeycolor = *cspacec::csImageMemory_ApplyKeycolor;
*GetImageType = *cspacec::csImageMemory_GetImageType;
*SetImageType = *cspacec::csImageMemory_SetImageType;
*HasMipmaps = *cspacec::csImageMemory_HasMipmaps;
*GetMipmap = *cspacec::csImageMemory_GetMipmap;
*SetMipmap = *cspacec::csImageMemory_SetMipmap;
*Copy = *cspacec::csImageMemory_Copy;
*CopyScale = *cspacec::csImageMemory_CopyScale;
*CopyTile = *cspacec::csImageMemory_CopyTile;
*ConvertFromRGBA = *cspacec::csImageMemory_ConvertFromRGBA;
*ConvertFromPal8 = *cspacec::csImageMemory_ConvertFromPal8;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csImageIOFileFormatDescriptions ##############

package cspace::csImageIOFileFormatDescriptions;
use overload
    "!=" => sub { $_[0]->__ne__($_[1])},
    "==" => sub { $_[0]->__eq__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csImageIOFileFormatDescriptions($self);
        delete $OWNER{$self};
    }
}

sub new {
    my $pkg = shift;
    my $self = cspacec::new_csImageIOFileFormatDescriptions(@_);
    bless $self, $pkg if defined($self);
}

*GetSize = *cspacec::csImageIOFileFormatDescriptions_GetSize;
*Get = *cspacec::csImageIOFileFormatDescriptions_Get;
*Put = *cspacec::csImageIOFileFormatDescriptions_Put;
*Push = *cspacec::csImageIOFileFormatDescriptions_Push;
*Pop = *cspacec::csImageIOFileFormatDescriptions_Pop;
*Top = *cspacec::csImageIOFileFormatDescriptions_Top;
*Insert = *cspacec::csImageIOFileFormatDescriptions_Insert;
*Contains = *cspacec::csImageIOFileFormatDescriptions_Contains;
*Truncate = *cspacec::csImageIOFileFormatDescriptions_Truncate;
*Empty = *cspacec::csImageIOFileFormatDescriptions_Empty;
*IsEmpty = *cspacec::csImageIOFileFormatDescriptions_IsEmpty;
*SetMinimalCapacity = *cspacec::csImageIOFileFormatDescriptions_SetMinimalCapacity;
*DeleteIndex = *cspacec::csImageIOFileFormatDescriptions_DeleteIndex;
*DeleteIndexFast = *cspacec::csImageIOFileFormatDescriptions_DeleteIndexFast;
*DeleteRange = *cspacec::csImageIOFileFormatDescriptions_DeleteRange;
*__eq__ = *cspacec::csImageIOFileFormatDescriptions___eq__;
*__ne__ = *cspacec::csImageIOFileFormatDescriptions___ne__;
*GetAllocator = *cspacec::csImageIOFileFormatDescriptions_GetAllocator;
*Length = *cspacec::csImageIOFileFormatDescriptions_Length;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csImageIOFileFormatDescription ##############

package cspace::csImageIOFileFormatDescription;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_mime_get = *cspacec::csImageIOFileFormatDescription_mime_get;
*swig_mime_set = *cspacec::csImageIOFileFormatDescription_mime_set;
*swig_subtype_get = *cspacec::csImageIOFileFormatDescription_subtype_get;
*swig_subtype_set = *cspacec::csImageIOFileFormatDescription_subtype_set;
*swig_cap_get = *cspacec::csImageIOFileFormatDescription_cap_get;
*swig_cap_set = *cspacec::csImageIOFileFormatDescription_cap_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csImageIOFileFormatDescription(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csImageIOFileFormatDescription($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iImageIO ##############

package cspace::iImageIO;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetDescription = *cspacec::iImageIO_GetDescription;
*Load = *cspacec::iImageIO_Load;
*SetDithering = *cspacec::iImageIO_SetDithering;
*Save = *cspacec::iImageIO_Save;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iImageIO($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iImageIO_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iReporterListener ##############

package cspace::iReporterListener;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Report = *cspacec::iReporterListener_Report;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iReporterListener($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iReporterListener_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iReporterIterator ##############

package cspace::iReporterIterator;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*HasNext = *cspacec::iReporterIterator_HasNext;
*Next = *cspacec::iReporterIterator_Next;
*GetMessageSeverity = *cspacec::iReporterIterator_GetMessageSeverity;
*GetMessageId = *cspacec::iReporterIterator_GetMessageId;
*GetMessageDescription = *cspacec::iReporterIterator_GetMessageDescription;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iReporterIterator($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iReporterIterator_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iReporter ##############

package cspace::iReporter;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Report = *cspacec::iReporter_Report;
*Clear = *cspacec::iReporter_Clear;
*GetMessageIterator = *cspacec::iReporter_GetMessageIterator;
*AddReporterListener = *cspacec::iReporter_AddReporterListener;
*RemoveReporterListener = *cspacec::iReporter_RemoveReporterListener;
*FindReporterListener = *cspacec::iReporter_FindReporterListener;
*ReportError = *cspacec::iReporter_ReportError;
*ReportWarning = *cspacec::iReporter_ReportWarning;
*ReportNotify = *cspacec::iReporter_ReportNotify;
*ReportBug = *cspacec::iReporter_ReportBug;
*ReportDebug = *cspacec::iReporter_ReportDebug;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iReporter($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iReporter_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csReporterHelper ##############

package cspace::csReporterHelper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*Report = *cspacec::csReporterHelper_Report;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csReporterHelper(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csReporterHelper($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iConsoleWatcher ##############

package cspace::iConsoleWatcher;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*ConsoleVisibilityChanged = *cspacec::iConsoleWatcher_ConsoleVisibilityChanged;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iConsoleWatcher($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iConsoleOutput ##############

package cspace::iConsoleOutput;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*PutText = *cspacec::iConsoleOutput_PutText;
*GetLine = *cspacec::iConsoleOutput_GetLine;
*Draw2D = *cspacec::iConsoleOutput_Draw2D;
*Draw3D = *cspacec::iConsoleOutput_Draw3D;
*Clear = *cspacec::iConsoleOutput_Clear;
*SetBufferSize = *cspacec::iConsoleOutput_SetBufferSize;
*GetTransparency = *cspacec::iConsoleOutput_GetTransparency;
*SetTransparency = *cspacec::iConsoleOutput_SetTransparency;
*GetFont = *cspacec::iConsoleOutput_GetFont;
*SetFont = *cspacec::iConsoleOutput_SetFont;
*GetTopLine = *cspacec::iConsoleOutput_GetTopLine;
*ScrollTo = *cspacec::iConsoleOutput_ScrollTo;
*GetCursorStyle = *cspacec::iConsoleOutput_GetCursorStyle;
*SetCursorStyle = *cspacec::iConsoleOutput_SetCursorStyle;
*SetVisible = *cspacec::iConsoleOutput_SetVisible;
*GetVisible = *cspacec::iConsoleOutput_GetVisible;
*AutoUpdate = *cspacec::iConsoleOutput_AutoUpdate;
*SetCursorPos = *cspacec::iConsoleOutput_SetCursorPos;
*GetMaxLineWidth = *cspacec::iConsoleOutput_GetMaxLineWidth;
*RegisterWatcher = *cspacec::iConsoleOutput_RegisterWatcher;
*PerformExtension = *cspacec::iConsoleOutput_PerformExtension;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iConsoleOutput($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iStandardReporterListener ##############

package cspace::iStandardReporterListener;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetOutputConsole = *cspacec::iStandardReporterListener_SetOutputConsole;
*SetNativeWindowManager = *cspacec::iStandardReporterListener_SetNativeWindowManager;
*SetReporter = *cspacec::iStandardReporterListener_SetReporter;
*SetDebugFile = *cspacec::iStandardReporterListener_SetDebugFile;
*SetDefaults = *cspacec::iStandardReporterListener_SetDefaults;
*SetMessageDestination = *cspacec::iStandardReporterListener_SetMessageDestination;
*RemoveMessages = *cspacec::iStandardReporterListener_RemoveMessages;
*ShowMessageID = *cspacec::iStandardReporterListener_ShowMessageID;
*GetDebugFile = *cspacec::iStandardReporterListener_GetDebugFile;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iStandardReporterListener($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iStandardReporterListener_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iView ##############

package cspace::iView;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetEngine = *cspacec::iView_GetEngine;
*SetEngine = *cspacec::iView_SetEngine;
*GetCamera = *cspacec::iView_GetCamera;
*SetCamera = *cspacec::iView_SetCamera;
*GetContext = *cspacec::iView_GetContext;
*SetContext = *cspacec::iView_SetContext;
*SetRectangle = *cspacec::iView_SetRectangle;
*ClearView = *cspacec::iView_ClearView;
*AddViewVertex = *cspacec::iView_AddViewVertex;
*RestrictClipperToScreen = *cspacec::iView_RestrictClipperToScreen;
*UpdateClipper = *cspacec::iView_UpdateClipper;
*GetClipper = *cspacec::iView_GetClipper;
*Draw = *cspacec::iView_Draw;
*SetAutoResize = *cspacec::iView_SetAutoResize;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iView($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iView_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csCollisionPair ##############

package cspace::csCollisionPair;
use overload
    "==" => sub { $_[0]->__eq__($_[1])},
    "fallback" => 1;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_a1_get = *cspacec::csCollisionPair_a1_get;
*swig_a1_set = *cspacec::csCollisionPair_a1_set;
*swig_b1_get = *cspacec::csCollisionPair_b1_get;
*swig_b1_set = *cspacec::csCollisionPair_b1_set;
*swig_c1_get = *cspacec::csCollisionPair_c1_get;
*swig_c1_set = *cspacec::csCollisionPair_c1_set;
*swig_a2_get = *cspacec::csCollisionPair_a2_get;
*swig_a2_set = *cspacec::csCollisionPair_a2_set;
*swig_b2_get = *cspacec::csCollisionPair_b2_get;
*swig_b2_set = *cspacec::csCollisionPair_b2_set;
*swig_c2_get = *cspacec::csCollisionPair_c2_get;
*swig_c2_set = *cspacec::csCollisionPair_c2_set;
*__eq__ = *cspacec::csCollisionPair___eq__;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csCollisionPair(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csCollisionPair($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csIntersectingTriangle ##############

package cspace::csIntersectingTriangle;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_a_get = *cspacec::csIntersectingTriangle_a_get;
*swig_a_set = *cspacec::csIntersectingTriangle_a_set;
*swig_b_get = *cspacec::csIntersectingTriangle_b_get;
*swig_b_set = *cspacec::csIntersectingTriangle_b_set;
*swig_c_get = *cspacec::csIntersectingTriangle_c_get;
*swig_c_set = *cspacec::csIntersectingTriangle_c_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csIntersectingTriangle(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csIntersectingTriangle($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iCollider ##############

package cspace::iCollider;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetColliderType = *cspacec::iCollider_GetColliderType;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iCollider($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iCollider_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iCollideSystem ##############

package cspace::iCollideSystem;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetTriangleDataID = *cspacec::iCollideSystem_GetTriangleDataID;
*CreateCollider = *cspacec::iCollideSystem_CreateCollider;
*Collide = *cspacec::iCollideSystem_Collide;
*GetCollisionPairs = *cspacec::iCollideSystem_GetCollisionPairs;
*GetCollisionPairCount = *cspacec::iCollideSystem_GetCollisionPairCount;
*ResetCollisionPairs = *cspacec::iCollideSystem_ResetCollisionPairs;
*CollideRay = *cspacec::iCollideSystem_CollideRay;
*CollideSegment = *cspacec::iCollideSystem_CollideSegment;
*GetIntersectingTriangles = *cspacec::iCollideSystem_GetIntersectingTriangles;
*SetOneHitOnly = *cspacec::iCollideSystem_SetOneHitOnly;
*GetOneHitOnly = *cspacec::iCollideSystem_GetOneHitOnly;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iCollideSystem($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iCollideSystem_scfGetVersion;
*GetCollisionPairByIndex = *cspacec::iCollideSystem_GetCollisionPairByIndex;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDynamicsStepCallback ##############

package cspace::iDynamicsStepCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Step = *cspacec::iDynamicsStepCallback_Step;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDynamicsStepCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDynamics ##############

package cspace::iDynamics;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*CreateSystem = *cspacec::iDynamics_CreateSystem;
*RemoveSystem = *cspacec::iDynamics_RemoveSystem;
*RemoveSystems = *cspacec::iDynamics_RemoveSystems;
*FindSystem = *cspacec::iDynamics_FindSystem;
*Step = *cspacec::iDynamics_Step;
*AddStepCallback = *cspacec::iDynamics_AddStepCallback;
*RemoveStepCallback = *cspacec::iDynamics_RemoveStepCallback;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDynamics($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iDynamics_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDynamicSystem ##############

package cspace::iDynamicSystem;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iDynamicSystem_QueryObject;
*SetGravity = *cspacec::iDynamicSystem_SetGravity;
*GetGravity = *cspacec::iDynamicSystem_GetGravity;
*SetLinearDampener = *cspacec::iDynamicSystem_SetLinearDampener;
*GetLinearDampener = *cspacec::iDynamicSystem_GetLinearDampener;
*SetRollingDampener = *cspacec::iDynamicSystem_SetRollingDampener;
*GetRollingDampener = *cspacec::iDynamicSystem_GetRollingDampener;
*EnableAutoDisable = *cspacec::iDynamicSystem_EnableAutoDisable;
*AutoDisableEnabled = *cspacec::iDynamicSystem_AutoDisableEnabled;
*SetAutoDisableParams = *cspacec::iDynamicSystem_SetAutoDisableParams;
*Step = *cspacec::iDynamicSystem_Step;
*CreateBody = *cspacec::iDynamicSystem_CreateBody;
*RemoveBody = *cspacec::iDynamicSystem_RemoveBody;
*FindBody = *cspacec::iDynamicSystem_FindBody;
*GetBody = *cspacec::iDynamicSystem_GetBody;
*GetBodysCount = *cspacec::iDynamicSystem_GetBodysCount;
*CreateGroup = *cspacec::iDynamicSystem_CreateGroup;
*RemoveGroup = *cspacec::iDynamicSystem_RemoveGroup;
*CreateJoint = *cspacec::iDynamicSystem_CreateJoint;
*RemoveJoint = *cspacec::iDynamicSystem_RemoveJoint;
*GetDefaultMoveCallback = *cspacec::iDynamicSystem_GetDefaultMoveCallback;
*AttachColliderMesh = *cspacec::iDynamicSystem_AttachColliderMesh;
*AttachColliderCylinder = *cspacec::iDynamicSystem_AttachColliderCylinder;
*AttachColliderBox = *cspacec::iDynamicSystem_AttachColliderBox;
*AttachColliderSphere = *cspacec::iDynamicSystem_AttachColliderSphere;
*AttachColliderPlane = *cspacec::iDynamicSystem_AttachColliderPlane;
*DestroyColliders = *cspacec::iDynamicSystem_DestroyColliders;
*DestroyCollider = *cspacec::iDynamicSystem_DestroyCollider;
*AttachCollider = *cspacec::iDynamicSystem_AttachCollider;
*CreateCollider = *cspacec::iDynamicSystem_CreateCollider;
*GetCollider = *cspacec::iDynamicSystem_GetCollider;
*GetColliderCount = *cspacec::iDynamicSystem_GetColliderCount;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDynamicSystem($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iDynamicSystem_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDynamicsMoveCallback ##############

package cspace::iDynamicsMoveCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Execute = *cspacec::iDynamicsMoveCallback_Execute;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDynamicsMoveCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDynamicsCollisionCallback ##############

package cspace::iDynamicsCollisionCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Execute = *cspacec::iDynamicsCollisionCallback_Execute;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDynamicsCollisionCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iBodyGroup ##############

package cspace::iBodyGroup;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*AddBody = *cspacec::iBodyGroup_AddBody;
*RemoveBody = *cspacec::iBodyGroup_RemoveBody;
*BodyInGroup = *cspacec::iBodyGroup_BodyInGroup;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iBodyGroup($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iBodyGroup_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iRigidBody ##############

package cspace::iRigidBody;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iRigidBody_QueryObject;
*MakeStatic = *cspacec::iRigidBody_MakeStatic;
*MakeDynamic = *cspacec::iRigidBody_MakeDynamic;
*IsStatic = *cspacec::iRigidBody_IsStatic;
*Disable = *cspacec::iRigidBody_Disable;
*Enable = *cspacec::iRigidBody_Enable;
*IsEnabled = *cspacec::iRigidBody_IsEnabled;
*GetGroup = *cspacec::iRigidBody_GetGroup;
*AttachColliderMesh = *cspacec::iRigidBody_AttachColliderMesh;
*AttachColliderCylinder = *cspacec::iRigidBody_AttachColliderCylinder;
*AttachColliderBox = *cspacec::iRigidBody_AttachColliderBox;
*AttachColliderSphere = *cspacec::iRigidBody_AttachColliderSphere;
*AttachColliderPlane = *cspacec::iRigidBody_AttachColliderPlane;
*AttachCollider = *cspacec::iRigidBody_AttachCollider;
*DestroyColliders = *cspacec::iRigidBody_DestroyColliders;
*DestroyCollider = *cspacec::iRigidBody_DestroyCollider;
*SetPosition = *cspacec::iRigidBody_SetPosition;
*GetPosition = *cspacec::iRigidBody_GetPosition;
*SetOrientation = *cspacec::iRigidBody_SetOrientation;
*GetOrientation = *cspacec::iRigidBody_GetOrientation;
*SetTransform = *cspacec::iRigidBody_SetTransform;
*GetTransform = *cspacec::iRigidBody_GetTransform;
*SetLinearVelocity = *cspacec::iRigidBody_SetLinearVelocity;
*GetLinearVelocity = *cspacec::iRigidBody_GetLinearVelocity;
*SetAngularVelocity = *cspacec::iRigidBody_SetAngularVelocity;
*GetAngularVelocity = *cspacec::iRigidBody_GetAngularVelocity;
*SetProperties = *cspacec::iRigidBody_SetProperties;
*GetProperties = *cspacec::iRigidBody_GetProperties;
*GetMass = *cspacec::iRigidBody_GetMass;
*GetCenter = *cspacec::iRigidBody_GetCenter;
*GetInertia = *cspacec::iRigidBody_GetInertia;
*AdjustTotalMass = *cspacec::iRigidBody_AdjustTotalMass;
*AddForce = *cspacec::iRigidBody_AddForce;
*AddTorque = *cspacec::iRigidBody_AddTorque;
*AddRelForce = *cspacec::iRigidBody_AddRelForce;
*AddRelTorque = *cspacec::iRigidBody_AddRelTorque;
*AddForceAtPos = *cspacec::iRigidBody_AddForceAtPos;
*AddForceAtRelPos = *cspacec::iRigidBody_AddForceAtRelPos;
*AddRelForceAtPos = *cspacec::iRigidBody_AddRelForceAtPos;
*AddRelForceAtRelPos = *cspacec::iRigidBody_AddRelForceAtRelPos;
*GetForce = *cspacec::iRigidBody_GetForce;
*GetTorque = *cspacec::iRigidBody_GetTorque;
*AttachMesh = *cspacec::iRigidBody_AttachMesh;
*GetAttachedMesh = *cspacec::iRigidBody_GetAttachedMesh;
*AttachLight = *cspacec::iRigidBody_AttachLight;
*GetAttachedLight = *cspacec::iRigidBody_GetAttachedLight;
*AttachCamera = *cspacec::iRigidBody_AttachCamera;
*GetAttachedCamera = *cspacec::iRigidBody_GetAttachedCamera;
*SetMoveCallback = *cspacec::iRigidBody_SetMoveCallback;
*SetCollisionCallback = *cspacec::iRigidBody_SetCollisionCallback;
*Collision = *cspacec::iRigidBody_Collision;
*Update = *cspacec::iRigidBody_Update;
*GetCollider = *cspacec::iRigidBody_GetCollider;
*GetColliderCount = *cspacec::iRigidBody_GetColliderCount;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iRigidBody($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDynamicsColliderCollisionCallback ##############

package cspace::iDynamicsColliderCollisionCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Execute = *cspacec::iDynamicsColliderCollisionCallback_Execute;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDynamicsColliderCollisionCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iDynamicsSystemCollider ##############

package cspace::iDynamicsSystemCollider;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*CreateSphereGeometry = *cspacec::iDynamicsSystemCollider_CreateSphereGeometry;
*CreatePlaneGeometry = *cspacec::iDynamicsSystemCollider_CreatePlaneGeometry;
*CreateMeshGeometry = *cspacec::iDynamicsSystemCollider_CreateMeshGeometry;
*CreateBoxGeometry = *cspacec::iDynamicsSystemCollider_CreateBoxGeometry;
*CreateCCylinderGeometry = *cspacec::iDynamicsSystemCollider_CreateCCylinderGeometry;
*SetCollisionCallback = *cspacec::iDynamicsSystemCollider_SetCollisionCallback;
*SetFriction = *cspacec::iDynamicsSystemCollider_SetFriction;
*SetSoftness = *cspacec::iDynamicsSystemCollider_SetSoftness;
*SetDensity = *cspacec::iDynamicsSystemCollider_SetDensity;
*SetElasticity = *cspacec::iDynamicsSystemCollider_SetElasticity;
*GetFriction = *cspacec::iDynamicsSystemCollider_GetFriction;
*GetSoftness = *cspacec::iDynamicsSystemCollider_GetSoftness;
*GetDensity = *cspacec::iDynamicsSystemCollider_GetDensity;
*GetElasticity = *cspacec::iDynamicsSystemCollider_GetElasticity;
*FillWithColliderGeometry = *cspacec::iDynamicsSystemCollider_FillWithColliderGeometry;
*GetGeometryType = *cspacec::iDynamicsSystemCollider_GetGeometryType;
*GetTransform = *cspacec::iDynamicsSystemCollider_GetTransform;
*GetLocalTransform = *cspacec::iDynamicsSystemCollider_GetLocalTransform;
*SetTransform = *cspacec::iDynamicsSystemCollider_SetTransform;
*GetBoxGeometry = *cspacec::iDynamicsSystemCollider_GetBoxGeometry;
*GetSphereGeometry = *cspacec::iDynamicsSystemCollider_GetSphereGeometry;
*GetPlaneGeometry = *cspacec::iDynamicsSystemCollider_GetPlaneGeometry;
*GetCylinderGeometry = *cspacec::iDynamicsSystemCollider_GetCylinderGeometry;
*MakeStatic = *cspacec::iDynamicsSystemCollider_MakeStatic;
*MakeDynamic = *cspacec::iDynamicsSystemCollider_MakeDynamic;
*IsStatic = *cspacec::iDynamicsSystemCollider_IsStatic;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iDynamicsSystemCollider($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iJoint ##############

package cspace::iJoint;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Attach = *cspacec::iJoint_Attach;
*GetAttachedBody = *cspacec::iJoint_GetAttachedBody;
*SetTransform = *cspacec::iJoint_SetTransform;
*GetTransform = *cspacec::iJoint_GetTransform;
*SetTransConstraints = *cspacec::iJoint_SetTransConstraints;
*IsXTransConstrained = *cspacec::iJoint_IsXTransConstrained;
*IsYTransConstrained = *cspacec::iJoint_IsYTransConstrained;
*IsZTransConstrained = *cspacec::iJoint_IsZTransConstrained;
*SetMinimumDistance = *cspacec::iJoint_SetMinimumDistance;
*GetMinimumDistance = *cspacec::iJoint_GetMinimumDistance;
*SetMaximumDistance = *cspacec::iJoint_SetMaximumDistance;
*GetMaximumDistance = *cspacec::iJoint_GetMaximumDistance;
*SetRotConstraints = *cspacec::iJoint_SetRotConstraints;
*IsXRotConstrained = *cspacec::iJoint_IsXRotConstrained;
*IsYRotConstrained = *cspacec::iJoint_IsYRotConstrained;
*IsZRotConstrained = *cspacec::iJoint_IsZRotConstrained;
*SetMinimumAngle = *cspacec::iJoint_SetMinimumAngle;
*GetMinimumAngle = *cspacec::iJoint_GetMinimumAngle;
*SetMaximumAngle = *cspacec::iJoint_SetMaximumAngle;
*GetMaximumAngle = *cspacec::iJoint_GetMaximumAngle;
*SetBounce = *cspacec::iJoint_SetBounce;
*GetBounce = *cspacec::iJoint_GetBounce;
*SetDesiredVelocity = *cspacec::iJoint_SetDesiredVelocity;
*GetDesiredVelocity = *cspacec::iJoint_GetDesiredVelocity;
*SetMaxForce = *cspacec::iJoint_SetMaxForce;
*GetMaxForce = *cspacec::iJoint_GetMaxForce;
*SetAngularConstraintAxis = *cspacec::iJoint_SetAngularConstraintAxis;
*GetAngularConstraintAxis = *cspacec::iJoint_GetAngularConstraintAxis;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iJoint($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iJoint_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iODEFrameUpdateCallback ##############

package cspace::iODEFrameUpdateCallback;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Execute = *cspacec::iODEFrameUpdateCallback_Execute;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iODEFrameUpdateCallback($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iODEDynamicState ##############

package cspace::iODEDynamicState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetGlobalERP = *cspacec::iODEDynamicState_SetGlobalERP;
*GlobalERP = *cspacec::iODEDynamicState_GlobalERP;
*SetGlobalCFM = *cspacec::iODEDynamicState_SetGlobalCFM;
*GlobalCFM = *cspacec::iODEDynamicState_GlobalCFM;
*EnableStepFast = *cspacec::iODEDynamicState_EnableStepFast;
*StepFastEnabled = *cspacec::iODEDynamicState_StepFastEnabled;
*SetStepFastIterations = *cspacec::iODEDynamicState_SetStepFastIterations;
*StepFastIterations = *cspacec::iODEDynamicState_StepFastIterations;
*EnableQuickStep = *cspacec::iODEDynamicState_EnableQuickStep;
*QuickStepEnabled = *cspacec::iODEDynamicState_QuickStepEnabled;
*SetQuickStepIterations = *cspacec::iODEDynamicState_SetQuickStepIterations;
*QuickStepIterations = *cspacec::iODEDynamicState_QuickStepIterations;
*EnableFrameRate = *cspacec::iODEDynamicState_EnableFrameRate;
*FrameRateEnabled = *cspacec::iODEDynamicState_FrameRateEnabled;
*SetFrameRate = *cspacec::iODEDynamicState_SetFrameRate;
*FrameRate = *cspacec::iODEDynamicState_FrameRate;
*SetFrameLimit = *cspacec::iODEDynamicState_SetFrameLimit;
*FrameLimit = *cspacec::iODEDynamicState_FrameLimit;
*AddFrameUpdateCallback = *cspacec::iODEDynamicState_AddFrameUpdateCallback;
*RemoveFrameUpdateCallback = *cspacec::iODEDynamicState_RemoveFrameUpdateCallback;
*EnableEventProcessing = *cspacec::iODEDynamicState_EnableEventProcessing;
*EventProcessingEnabled = *cspacec::iODEDynamicState_EventProcessingEnabled;
*EnableFastObjects = *cspacec::iODEDynamicState_EnableFastObjects;
*FastObjectsEnabled = *cspacec::iODEDynamicState_FastObjectsEnabled;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iODEDynamicState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iODEDynamicState_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iODEDynamicSystemState ##############

package cspace::iODEDynamicSystemState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetERP = *cspacec::iODEDynamicSystemState_SetERP;
*ERP = *cspacec::iODEDynamicSystemState_ERP;
*SetCFM = *cspacec::iODEDynamicSystemState_SetCFM;
*CFM = *cspacec::iODEDynamicSystemState_CFM;
*EnableStepFast = *cspacec::iODEDynamicSystemState_EnableStepFast;
*StepFastEnabled = *cspacec::iODEDynamicSystemState_StepFastEnabled;
*SetStepFastIterations = *cspacec::iODEDynamicSystemState_SetStepFastIterations;
*StepFastIterations = *cspacec::iODEDynamicSystemState_StepFastIterations;
*EnableQuickStep = *cspacec::iODEDynamicSystemState_EnableQuickStep;
*QuickStepEnabled = *cspacec::iODEDynamicSystemState_QuickStepEnabled;
*SetQuickStepIterations = *cspacec::iODEDynamicSystemState_SetQuickStepIterations;
*QuickStepIterations = *cspacec::iODEDynamicSystemState_QuickStepIterations;
*EnableAutoDisable = *cspacec::iODEDynamicSystemState_EnableAutoDisable;
*AutoDisableEnabled = *cspacec::iODEDynamicSystemState_AutoDisableEnabled;
*SetAutoDisableParams = *cspacec::iODEDynamicSystemState_SetAutoDisableParams;
*EnableFrameRate = *cspacec::iODEDynamicSystemState_EnableFrameRate;
*FrameRateEnabled = *cspacec::iODEDynamicSystemState_FrameRateEnabled;
*SetFrameRate = *cspacec::iODEDynamicSystemState_SetFrameRate;
*FrameRate = *cspacec::iODEDynamicSystemState_FrameRate;
*SetFrameLimit = *cspacec::iODEDynamicSystemState_SetFrameLimit;
*FrameLimit = *cspacec::iODEDynamicSystemState_FrameLimit;
*AddFrameUpdateCallback = *cspacec::iODEDynamicSystemState_AddFrameUpdateCallback;
*RemoveFrameUpdateCallback = *cspacec::iODEDynamicSystemState_RemoveFrameUpdateCallback;
*EnableFastObjects = *cspacec::iODEDynamicSystemState_EnableFastObjects;
*FastObjectsEnabled = *cspacec::iODEDynamicSystemState_FastObjectsEnabled;
*CreateBallJoint = *cspacec::iODEDynamicSystemState_CreateBallJoint;
*CreateHingeJoint = *cspacec::iODEDynamicSystemState_CreateHingeJoint;
*CreateHinge2Joint = *cspacec::iODEDynamicSystemState_CreateHinge2Joint;
*CreateAMotorJoint = *cspacec::iODEDynamicSystemState_CreateAMotorJoint;
*CreateUniversalJoint = *cspacec::iODEDynamicSystemState_CreateUniversalJoint;
*CreateSliderJoint = *cspacec::iODEDynamicSystemState_CreateSliderJoint;
*RemoveJoint = *cspacec::iODEDynamicSystemState_RemoveJoint;
*SetContactMaxCorrectingVel = *cspacec::iODEDynamicSystemState_SetContactMaxCorrectingVel;
*GetContactMaxCorrectingVel = *cspacec::iODEDynamicSystemState_GetContactMaxCorrectingVel;
*SetContactSurfaceLayer = *cspacec::iODEDynamicSystemState_SetContactSurfaceLayer;
*GetContactSurfaceLayer = *cspacec::iODEDynamicSystemState_GetContactSurfaceLayer;
*EnableOldInertia = *cspacec::iODEDynamicSystemState_EnableOldInertia;
*IsOldInertiaEnabled = *cspacec::iODEDynamicSystemState_IsOldInertiaEnabled;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iODEDynamicSystemState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iODEDynamicSystemState_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iODEJointState ##############

package cspace::iODEJointState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetType = *cspacec::iODEJointState_GetType;
*SetLoStop = *cspacec::iODEJointState_SetLoStop;
*SetHiStop = *cspacec::iODEJointState_SetHiStop;
*SetVel = *cspacec::iODEJointState_SetVel;
*SetFMax = *cspacec::iODEJointState_SetFMax;
*SetFudgeFactor = *cspacec::iODEJointState_SetFudgeFactor;
*SetBounce = *cspacec::iODEJointState_SetBounce;
*SetCFM = *cspacec::iODEJointState_SetCFM;
*SetStopERP = *cspacec::iODEJointState_SetStopERP;
*SetStopCFM = *cspacec::iODEJointState_SetStopCFM;
*SetSuspensionERP = *cspacec::iODEJointState_SetSuspensionERP;
*SetSuspensionCFM = *cspacec::iODEJointState_SetSuspensionCFM;
*GetLoStop = *cspacec::iODEJointState_GetLoStop;
*GetHiStop = *cspacec::iODEJointState_GetHiStop;
*GetVel = *cspacec::iODEJointState_GetVel;
*GetMaxForce = *cspacec::iODEJointState_GetMaxForce;
*GetFudgeFactor = *cspacec::iODEJointState_GetFudgeFactor;
*GetBounce = *cspacec::iODEJointState_GetBounce;
*GetCFM = *cspacec::iODEJointState_GetCFM;
*GetStopERP = *cspacec::iODEJointState_GetStopERP;
*GetStopCFM = *cspacec::iODEJointState_GetStopCFM;
*GetSuspensionERP = *cspacec::iODEJointState_GetSuspensionERP;
*GetSuspensionCFM = *cspacec::iODEJointState_GetSuspensionCFM;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iODEJointState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iODEJointState_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iODEGeneralJointState ##############

package cspace::iODEGeneralJointState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetLoStop = *cspacec::iODEGeneralJointState_SetLoStop;
*SetHiStop = *cspacec::iODEGeneralJointState_SetHiStop;
*SetVel = *cspacec::iODEGeneralJointState_SetVel;
*SetFMax = *cspacec::iODEGeneralJointState_SetFMax;
*SetFudgeFactor = *cspacec::iODEGeneralJointState_SetFudgeFactor;
*SetBounce = *cspacec::iODEGeneralJointState_SetBounce;
*SetCFM = *cspacec::iODEGeneralJointState_SetCFM;
*SetStopERP = *cspacec::iODEGeneralJointState_SetStopERP;
*SetStopCFM = *cspacec::iODEGeneralJointState_SetStopCFM;
*SetSuspensionERP = *cspacec::iODEGeneralJointState_SetSuspensionERP;
*SetSuspensionCFM = *cspacec::iODEGeneralJointState_SetSuspensionCFM;
*GetLoStop = *cspacec::iODEGeneralJointState_GetLoStop;
*GetHiStop = *cspacec::iODEGeneralJointState_GetHiStop;
*GetVel = *cspacec::iODEGeneralJointState_GetVel;
*GetFMax = *cspacec::iODEGeneralJointState_GetFMax;
*GetFudgeFactor = *cspacec::iODEGeneralJointState_GetFudgeFactor;
*GetBounce = *cspacec::iODEGeneralJointState_GetBounce;
*GetCFM = *cspacec::iODEGeneralJointState_GetCFM;
*GetStopERP = *cspacec::iODEGeneralJointState_GetStopERP;
*GetStopCFM = *cspacec::iODEGeneralJointState_GetStopCFM;
*GetSuspensionERP = *cspacec::iODEGeneralJointState_GetSuspensionERP;
*GetSuspensionCFM = *cspacec::iODEGeneralJointState_GetSuspensionCFM;
*Attach = *cspacec::iODEGeneralJointState_Attach;
*GetAttachedBody = *cspacec::iODEGeneralJointState_GetAttachedBody;
*GetFeedbackForce1 = *cspacec::iODEGeneralJointState_GetFeedbackForce1;
*GetFeedbackTorque1 = *cspacec::iODEGeneralJointState_GetFeedbackTorque1;
*GetFeedbackForce2 = *cspacec::iODEGeneralJointState_GetFeedbackForce2;
*GetFeedbackTorque2 = *cspacec::iODEGeneralJointState_GetFeedbackTorque2;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iODEGeneralJointState($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iODESliderJoint ##############

package cspace::iODESliderJoint;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iODEGeneralJointState cspace );
%OWNER = ();
%ITERATORS = ();
*SetSliderAxis = *cspacec::iODESliderJoint_SetSliderAxis;
*GetSliderAxis = *cspacec::iODESliderJoint_GetSliderAxis;
*GetSliderPosition = *cspacec::iODESliderJoint_GetSliderPosition;
*GetSliderPositionRate = *cspacec::iODESliderJoint_GetSliderPositionRate;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iODESliderJoint($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iODESliderJoint_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iODEUniversalJoint ##############

package cspace::iODEUniversalJoint;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iODEGeneralJointState cspace );
%OWNER = ();
%ITERATORS = ();
*SetUniversalAnchor = *cspacec::iODEUniversalJoint_SetUniversalAnchor;
*SetUniversalAxis1 = *cspacec::iODEUniversalJoint_SetUniversalAxis1;
*SetUniversalAxis2 = *cspacec::iODEUniversalJoint_SetUniversalAxis2;
*GetUniversalAnchor1 = *cspacec::iODEUniversalJoint_GetUniversalAnchor1;
*GetUniversalAnchor2 = *cspacec::iODEUniversalJoint_GetUniversalAnchor2;
*GetUniversalAxis1 = *cspacec::iODEUniversalJoint_GetUniversalAxis1;
*GetUniversalAxis2 = *cspacec::iODEUniversalJoint_GetUniversalAxis2;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iODEUniversalJoint($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iODEUniversalJoint_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iODEAMotorJoint ##############

package cspace::iODEAMotorJoint;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iODEGeneralJointState cspace );
%OWNER = ();
%ITERATORS = ();
*SetAMotorMode = *cspacec::iODEAMotorJoint_SetAMotorMode;
*GetAMotorMode = *cspacec::iODEAMotorJoint_GetAMotorMode;
*SetAMotorNumAxes = *cspacec::iODEAMotorJoint_SetAMotorNumAxes;
*GetAMotorNumAxes = *cspacec::iODEAMotorJoint_GetAMotorNumAxes;
*SetAMotorAxis = *cspacec::iODEAMotorJoint_SetAMotorAxis;
*GetAMotorAxis = *cspacec::iODEAMotorJoint_GetAMotorAxis;
*GetAMotorAxisRelOrientation = *cspacec::iODEAMotorJoint_GetAMotorAxisRelOrientation;
*SetAMotorAngle = *cspacec::iODEAMotorJoint_SetAMotorAngle;
*GetAMotorAngle = *cspacec::iODEAMotorJoint_GetAMotorAngle;
*GetAMotorAngleRate = *cspacec::iODEAMotorJoint_GetAMotorAngleRate;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iODEAMotorJoint($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iODEAMotorJoint_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iODEHinge2Joint ##############

package cspace::iODEHinge2Joint;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iODEGeneralJointState cspace );
%OWNER = ();
%ITERATORS = ();
*SetHingeAnchor = *cspacec::iODEHinge2Joint_SetHingeAnchor;
*SetHingeAxis1 = *cspacec::iODEHinge2Joint_SetHingeAxis1;
*SetHingeAxis2 = *cspacec::iODEHinge2Joint_SetHingeAxis2;
*GetHingeAnchor1 = *cspacec::iODEHinge2Joint_GetHingeAnchor1;
*GetHingeAnchor2 = *cspacec::iODEHinge2Joint_GetHingeAnchor2;
*GetHingeAxis1 = *cspacec::iODEHinge2Joint_GetHingeAxis1;
*GetHingeAxis2 = *cspacec::iODEHinge2Joint_GetHingeAxis2;
*GetHingeAngle = *cspacec::iODEHinge2Joint_GetHingeAngle;
*GetHingeAngleRate1 = *cspacec::iODEHinge2Joint_GetHingeAngleRate1;
*GetHingeAngleRate2 = *cspacec::iODEHinge2Joint_GetHingeAngleRate2;
*GetAnchorError = *cspacec::iODEHinge2Joint_GetAnchorError;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iODEHinge2Joint($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iODEHinge2Joint_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iODEHingeJoint ##############

package cspace::iODEHingeJoint;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iODEGeneralJointState cspace );
%OWNER = ();
%ITERATORS = ();
*SetHingeAnchor = *cspacec::iODEHingeJoint_SetHingeAnchor;
*SetHingeAxis = *cspacec::iODEHingeJoint_SetHingeAxis;
*GetHingeAnchor1 = *cspacec::iODEHingeJoint_GetHingeAnchor1;
*GetHingeAnchor2 = *cspacec::iODEHingeJoint_GetHingeAnchor2;
*GetHingeAxis = *cspacec::iODEHingeJoint_GetHingeAxis;
*GetHingeAngle = *cspacec::iODEHingeJoint_GetHingeAngle;
*GetHingeAngleRate = *cspacec::iODEHingeJoint_GetHingeAngleRate;
*GetAnchorError = *cspacec::iODEHingeJoint_GetAnchorError;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iODEHingeJoint($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iODEHingeJoint_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iODEBallJoint ##############

package cspace::iODEBallJoint;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetBallAnchor = *cspacec::iODEBallJoint_SetBallAnchor;
*GetBallAnchor1 = *cspacec::iODEBallJoint_GetBallAnchor1;
*GetBallAnchor2 = *cspacec::iODEBallJoint_GetBallAnchor2;
*GetAnchorError = *cspacec::iODEBallJoint_GetAnchorError;
*Attach = *cspacec::iODEBallJoint_Attach;
*GetAttachedBody = *cspacec::iODEBallJoint_GetAttachedBody;
*GetFeedbackForce1 = *cspacec::iODEBallJoint_GetFeedbackForce1;
*GetFeedbackTorque1 = *cspacec::iODEBallJoint_GetFeedbackTorque1;
*GetFeedbackForce2 = *cspacec::iODEBallJoint_GetFeedbackForce2;
*GetFeedbackTorque2 = *cspacec::iODEBallJoint_GetFeedbackTorque2;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iODEBallJoint($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iODEBallJoint_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iParameterESM ##############

package cspace::iParameterESM;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetValue = *cspacec::iParameterESM_GetValue;
*IsConstant = *cspacec::iParameterESM_IsConstant;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iParameterESM($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iEngineSequenceParameters ##############

package cspace::iEngineSequenceParameters;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetParameterCount = *cspacec::iEngineSequenceParameters_GetParameterCount;
*GetParameter = *cspacec::iEngineSequenceParameters_GetParameter;
*GetParameterIdx = *cspacec::iEngineSequenceParameters_GetParameterIdx;
*GetParameterName = *cspacec::iEngineSequenceParameters_GetParameterName;
*AddParameter = *cspacec::iEngineSequenceParameters_AddParameter;
*SetParameter = *cspacec::iEngineSequenceParameters_SetParameter;
*CreateParameterESM = *cspacec::iEngineSequenceParameters_CreateParameterESM;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iEngineSequenceParameters($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSequenceWrapper ##############

package cspace::iSequenceWrapper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iSequenceWrapper_QueryObject;
*GetSequence = *cspacec::iSequenceWrapper_GetSequence;
*CreateBaseParameterBlock = *cspacec::iSequenceWrapper_CreateBaseParameterBlock;
*GetBaseParameterBlock = *cspacec::iSequenceWrapper_GetBaseParameterBlock;
*CreateParameterBlock = *cspacec::iSequenceWrapper_CreateParameterBlock;
*AddOperationSetVariable = *cspacec::iSequenceWrapper_AddOperationSetVariable;
*AddOperationSetMaterial = *cspacec::iSequenceWrapper_AddOperationSetMaterial;
*AddOperationSetPolygonMaterial = *cspacec::iSequenceWrapper_AddOperationSetPolygonMaterial;
*AddOperationSetLight = *cspacec::iSequenceWrapper_AddOperationSetLight;
*AddOperationFadeLight = *cspacec::iSequenceWrapper_AddOperationFadeLight;
*AddOperationSetAmbient = *cspacec::iSequenceWrapper_AddOperationSetAmbient;
*AddOperationFadeAmbient = *cspacec::iSequenceWrapper_AddOperationFadeAmbient;
*AddOperationRandomDelay = *cspacec::iSequenceWrapper_AddOperationRandomDelay;
*AddOperationSetMeshColor = *cspacec::iSequenceWrapper_AddOperationSetMeshColor;
*AddOperationFadeMeshColor = *cspacec::iSequenceWrapper_AddOperationFadeMeshColor;
*AddOperationSetFog = *cspacec::iSequenceWrapper_AddOperationSetFog;
*AddOperationFadeFog = *cspacec::iSequenceWrapper_AddOperationFadeFog;
*AddOperationRotateDuration = *cspacec::iSequenceWrapper_AddOperationRotateDuration;
*AddOperationMoveDuration = *cspacec::iSequenceWrapper_AddOperationMoveDuration;
*AddOperationTriggerState = *cspacec::iSequenceWrapper_AddOperationTriggerState;
*AddOperationCheckTrigger = *cspacec::iSequenceWrapper_AddOperationCheckTrigger;
*AddOperationTestTrigger = *cspacec::iSequenceWrapper_AddOperationTestTrigger;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSequenceWrapper($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSequenceTrigger ##############

package cspace::iSequenceTrigger;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iSequenceTrigger_QueryObject;
*AddConditionInSector = *cspacec::iSequenceTrigger_AddConditionInSector;
*AddConditionSectorVisible = *cspacec::iSequenceTrigger_AddConditionSectorVisible;
*AddConditionMeshClick = *cspacec::iSequenceTrigger_AddConditionMeshClick;
*AddConditionLightChange = *cspacec::iSequenceTrigger_AddConditionLightChange;
*AddConditionManual = *cspacec::iSequenceTrigger_AddConditionManual;
*SetEnabled = *cspacec::iSequenceTrigger_SetEnabled;
*IsEnabled = *cspacec::iSequenceTrigger_IsEnabled;
*ClearConditions = *cspacec::iSequenceTrigger_ClearConditions;
*Trigger = *cspacec::iSequenceTrigger_Trigger;
*SetParameters = *cspacec::iSequenceTrigger_SetParameters;
*GetParameters = *cspacec::iSequenceTrigger_GetParameters;
*FireSequence = *cspacec::iSequenceTrigger_FireSequence;
*GetFiredSequence = *cspacec::iSequenceTrigger_GetFiredSequence;
*TestConditions = *cspacec::iSequenceTrigger_TestConditions;
*CheckState = *cspacec::iSequenceTrigger_CheckState;
*ForceFire = *cspacec::iSequenceTrigger_ForceFire;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSequenceTrigger($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSequenceTimedOperation ##############

package cspace::iSequenceTimedOperation;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Do = *cspacec::iSequenceTimedOperation_Do;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSequenceTimedOperation($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iEngineSequenceManager ##############

package cspace::iEngineSequenceManager;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetSequenceManager = *cspacec::iEngineSequenceManager_GetSequenceManager;
*CreateParameterESM = *cspacec::iEngineSequenceManager_CreateParameterESM;
*CreateTrigger = *cspacec::iEngineSequenceManager_CreateTrigger;
*RemoveTrigger = *cspacec::iEngineSequenceManager_RemoveTrigger;
*RemoveTriggers = *cspacec::iEngineSequenceManager_RemoveTriggers;
*GetTriggerCount = *cspacec::iEngineSequenceManager_GetTriggerCount;
*GetTrigger = *cspacec::iEngineSequenceManager_GetTrigger;
*FindTriggerByName = *cspacec::iEngineSequenceManager_FindTriggerByName;
*FireTriggerByName = *cspacec::iEngineSequenceManager_FireTriggerByName;
*CreateSequence = *cspacec::iEngineSequenceManager_CreateSequence;
*RemoveSequence = *cspacec::iEngineSequenceManager_RemoveSequence;
*RemoveSequences = *cspacec::iEngineSequenceManager_RemoveSequences;
*GetSequenceCount = *cspacec::iEngineSequenceManager_GetSequenceCount;
*GetSequence = *cspacec::iEngineSequenceManager_GetSequence;
*FindSequenceByName = *cspacec::iEngineSequenceManager_FindSequenceByName;
*RunSequenceByName = *cspacec::iEngineSequenceManager_RunSequenceByName;
*FireTimedOperation = *cspacec::iEngineSequenceManager_FireTimedOperation;
*DestroyTimedOperations = *cspacec::iEngineSequenceManager_DestroyTimedOperations;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iEngineSequenceManager($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iEngineSequenceManager_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMovieRecorder ##############

package cspace::iMovieRecorder;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Start = *cspacec::iMovieRecorder_Start;
*Stop = *cspacec::iMovieRecorder_Stop;
*IsRecording = *cspacec::iMovieRecorder_IsRecording;
*Pause = *cspacec::iMovieRecorder_Pause;
*UnPause = *cspacec::iMovieRecorder_UnPause;
*IsPaused = *cspacec::iMovieRecorder_IsPaused;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMovieRecorder($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iMovieRecorder_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iMapNode ##############

package cspace::iMapNode;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*QueryObject = *cspacec::iMapNode_QueryObject;
*SetPosition = *cspacec::iMapNode_SetPosition;
*GetPosition = *cspacec::iMapNode_GetPosition;
*SetXVector = *cspacec::iMapNode_SetXVector;
*GetXVector = *cspacec::iMapNode_GetXVector;
*SetYVector = *cspacec::iMapNode_SetYVector;
*GetYVector = *cspacec::iMapNode_GetYVector;
*SetZVector = *cspacec::iMapNode_SetZVector;
*GetZVector = *cspacec::iMapNode_GetZVector;
*SetSector = *cspacec::iMapNode_SetSector;
*GetSector = *cspacec::iMapNode_GetSector;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iMapNode($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iMapNode_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iScriptObject ##############

package cspace::iScriptObject;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*IsType = *cspacec::iScriptObject_IsType;
*GetPointer = *cspacec::iScriptObject_GetPointer;
*SetPointer = *cspacec::iScriptObject_SetPointer;
*IntCall = *cspacec::iScriptObject_IntCall;
*FloatCall = *cspacec::iScriptObject_FloatCall;
*DoubleCall = *cspacec::iScriptObject_DoubleCall;
*Call = *cspacec::iScriptObject_Call;
*ObjectCall = *cspacec::iScriptObject_ObjectCall;
*SetInt = *cspacec::iScriptObject_SetInt;
*SetFloat = *cspacec::iScriptObject_SetFloat;
*SetDouble = *cspacec::iScriptObject_SetDouble;
*SetString = *cspacec::iScriptObject_SetString;
*Set = *cspacec::iScriptObject_Set;
*SetTruth = *cspacec::iScriptObject_SetTruth;
*Get = *cspacec::iScriptObject_Get;
*GetTruth = *cspacec::iScriptObject_GetTruth;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iScriptObject($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iScriptObject_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iScript ##############

package cspace::iScript;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*Initialize = *cspacec::iScript_Initialize;
*RunText = *cspacec::iScript_RunText;
*LoadModule = *cspacec::iScript_LoadModule;
*IntCall = *cspacec::iScript_IntCall;
*FloatCall = *cspacec::iScript_FloatCall;
*DoubleCall = *cspacec::iScript_DoubleCall;
*Call = *cspacec::iScript_Call;
*ObjectCall = *cspacec::iScript_ObjectCall;
*NewObject = *cspacec::iScript_NewObject;
*StoreInt = *cspacec::iScript_StoreInt;
*StoreFloat = *cspacec::iScript_StoreFloat;
*StoreDouble = *cspacec::iScript_StoreDouble;
*StoreString = *cspacec::iScript_StoreString;
*Store = *cspacec::iScript_Store;
*SetTruth = *cspacec::iScript_SetTruth;
*Retrieve = *cspacec::iScript_Retrieve;
*GetTruth = *cspacec::iScript_GetTruth;
*Remove = *cspacec::iScript_Remove;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iScript($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iScript_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iSimpleFormerState ##############

package cspace::iSimpleFormerState;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SetHeightmap = *cspacec::iSimpleFormerState_SetHeightmap;
*SetScale = *cspacec::iSimpleFormerState_SetScale;
*SetOffset = *cspacec::iSimpleFormerState_SetOffset;
*SetIntegerMap = *cspacec::iSimpleFormerState_SetIntegerMap;
*SetFloatMap = *cspacec::iSimpleFormerState_SetFloatMap;
*GetFloatMap = *cspacec::iSimpleFormerState_GetFloatMap;
*SetMaterialScale = *cspacec::iSimpleFormerState_SetMaterialScale;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iSimpleFormerState($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iSimpleFormerState_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iTerraFormer ##############

package cspace::iTerraFormer;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*GetSampler = *cspacec::iTerraFormer_GetSampler;
*GetIntegerMapSize = *cspacec::iTerraFormer_GetIntegerMapSize;
*SampleFloat = *cspacec::iTerraFormer_SampleFloat;
*SampleVector2 = *cspacec::iTerraFormer_SampleVector2;
*SampleVector3 = *cspacec::iTerraFormer_SampleVector3;
*SampleInteger = *cspacec::iTerraFormer_SampleInteger;
*QueryObject = *cspacec::iTerraFormer_QueryObject;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iTerraFormer($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iTerraFormer_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iTerraSampler ##############

package cspace::iTerraSampler;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iBase cspace );
%OWNER = ();
%ITERATORS = ();
*SampleFloat = *cspacec::iTerraSampler_SampleFloat;
*SampleVector2 = *cspacec::iTerraSampler_SampleVector2;
*SampleVector3 = *cspacec::iTerraSampler_SampleVector3;
*SampleInteger = *cspacec::iTerraSampler_SampleInteger;
*GetMaterialPalette = *cspacec::iTerraSampler_GetMaterialPalette;
*GetRegion = *cspacec::iTerraSampler_GetRegion;
*GetResolution = *cspacec::iTerraSampler_GetResolution;
*GetVersion = *cspacec::iTerraSampler_GetVersion;
*Cleanup = *cspacec::iTerraSampler_Cleanup;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_iTerraSampler($self);
        delete $OWNER{$self};
    }
}

*scfGetVersion = *cspacec::iTerraSampler_scfGetVersion;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::pycsObject ##############

package cspace::pycsObject;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iObject cspace );
%OWNER = ();
*IncRef = *cspacec::pycsObject_IncRef;
*DecRef = *cspacec::pycsObject_DecRef;
*GetRefCount = *cspacec::pycsObject_GetRefCount;
*QueryInterface = *cspacec::pycsObject_QueryInterface;
*AddRefOwner = *cspacec::pycsObject_AddRefOwner;
*RemoveRefOwner = *cspacec::pycsObject_RemoveRefOwner;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csObject ##############

package cspace::csObject;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::pycsObject cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csObject(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csObject($self);
        delete $OWNER{$self};
    }
}

*SetName = *cspacec::csObject_SetName;
*GetName = *cspacec::csObject_GetName;
*GetID = *cspacec::csObject_GetID;
*SetObjectParent = *cspacec::csObject_SetObjectParent;
*GetObjectParent = *cspacec::csObject_GetObjectParent;
*ObjAdd = *cspacec::csObject_ObjAdd;
*ObjRemove = *cspacec::csObject_ObjRemove;
*ObjRemoveAll = *cspacec::csObject_ObjRemoveAll;
*ObjAddChildren = *cspacec::csObject_ObjAddChildren;
*GetChild = *cspacec::csObject_GetChild;
*GetIterator = *cspacec::csObject_GetIterator;
*AddNameChangeListener = *cspacec::csObject_AddNameChangeListener;
*RemoveNameChangeListener = *cspacec::csObject_RemoveNameChangeListener;
*ObjReleaseOld = *cspacec::csObject_ObjReleaseOld;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::pycsColliderWrapper ##############

package cspace::pycsColliderWrapper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csObject cspace );
%OWNER = ();
*IncRef = *cspacec::pycsColliderWrapper_IncRef;
*DecRef = *cspacec::pycsColliderWrapper_DecRef;
*GetRefCount = *cspacec::pycsColliderWrapper_GetRefCount;
*QueryInterface = *cspacec::pycsColliderWrapper_QueryInterface;
*AddRefOwner = *cspacec::pycsColliderWrapper_AddRefOwner;
*RemoveRefOwner = *cspacec::pycsColliderWrapper_RemoveRefOwner;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csColliderWrapper ##############

package cspace::csColliderWrapper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::pycsColliderWrapper cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csColliderWrapper(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csColliderWrapper($self);
        delete $OWNER{$self};
    }
}

*GetCollider = *cspacec::csColliderWrapper_GetCollider;
*GetCollideSystem = *cspacec::csColliderWrapper_GetCollideSystem;
*Collide = *cspacec::csColliderWrapper_Collide;
*GetColliderWrapper = *cspacec::csColliderWrapper_GetColliderWrapper;
*UpdateCollider = *cspacec::csColliderWrapper_UpdateCollider;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csTraceBeamResult ##############

package cspace::csTraceBeamResult;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_closest_tri_get = *cspacec::csTraceBeamResult_closest_tri_get;
*swig_closest_tri_set = *cspacec::csTraceBeamResult_closest_tri_set;
*swig_closest_isect_get = *cspacec::csTraceBeamResult_closest_isect_get;
*swig_closest_isect_set = *cspacec::csTraceBeamResult_closest_isect_set;
*swig_closest_mesh_get = *cspacec::csTraceBeamResult_closest_mesh_get;
*swig_closest_mesh_set = *cspacec::csTraceBeamResult_closest_mesh_set;
*swig_sqdistance_get = *cspacec::csTraceBeamResult_sqdistance_get;
*swig_sqdistance_set = *cspacec::csTraceBeamResult_sqdistance_set;
*swig_end_sector_get = *cspacec::csTraceBeamResult_end_sector_get;
*swig_end_sector_set = *cspacec::csTraceBeamResult_end_sector_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csTraceBeamResult(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csTraceBeamResult($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csColliderHelper ##############

package cspace::csColliderHelper;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*InitializeCollisionWrapper = *cspacec::csColliderHelper_InitializeCollisionWrapper;
*InitializeCollisionWrappers = *cspacec::csColliderHelper_InitializeCollisionWrappers;
*CollideArray = *cspacec::csColliderHelper_CollideArray;
*CollidePath = *cspacec::csColliderHelper_CollidePath;
*TraceBeam = *cspacec::csColliderHelper_TraceBeam;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csColliderHelper(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csColliderHelper($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csColliderActor ##############

package cspace::csColliderActor;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csColliderActor(@_);
    bless $self, $pkg if defined($self);
}

*SetCollideSystem = *cspacec::csColliderActor_SetCollideSystem;
*SetEngine = *cspacec::csColliderActor_SetEngine;
*InitializeColliders = *cspacec::csColliderActor_InitializeColliders;
*SetCamera = *cspacec::csColliderActor_SetCamera;
*SetGravity = *cspacec::csColliderActor_SetGravity;
*GetGravity = *cspacec::csColliderActor_GetGravity;
*IsOnGround = *cspacec::csColliderActor_IsOnGround;
*SetOnGround = *cspacec::csColliderActor_SetOnGround;
*HasCD = *cspacec::csColliderActor_HasCD;
*SetCD = *cspacec::csColliderActor_SetCD;
*CheckRevertMove = *cspacec::csColliderActor_CheckRevertMove;
*EnableHitMeshes = *cspacec::csColliderActor_EnableHitMeshes;
*CheckHitMeshes = *cspacec::csColliderActor_CheckHitMeshes;
*GetHitMeshes = *cspacec::csColliderActor_GetHitMeshes;
*Move = *cspacec::csColliderActor_Move;
*GetRotation = *cspacec::csColliderActor_GetRotation;
*SetRotation = *cspacec::csColliderActor_SetRotation;
*AdjustForCollisions = *cspacec::csColliderActor_AdjustForCollisions;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csColliderActor($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csView ##############

package cspace::csView;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csView(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csView($self);
        delete $OWNER{$self};
    }
}

*GetEngine = *cspacec::csView_GetEngine;
*SetEngine = *cspacec::csView_SetEngine;
*GetCamera = *cspacec::csView_GetCamera;
*SetCamera = *cspacec::csView_SetCamera;
*GetContext = *cspacec::csView_GetContext;
*SetContext = *cspacec::csView_SetContext;
*SetRectangle = *cspacec::csView_SetRectangle;
*ClearView = *cspacec::csView_ClearView;
*AddViewVertex = *cspacec::csView_AddViewVertex;
*RestrictClipperToScreen = *cspacec::csView_RestrictClipperToScreen;
*SetAutoResize = *cspacec::csView_SetAutoResize;
*UpdateClipper = *cspacec::csView_UpdateClipper;
*GetClipper = *cspacec::csView_GetClipper;
*Draw = *cspacec::csView_Draw;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPixmap ##############

package cspace::csPixmap;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPixmap($self);
        delete $OWNER{$self};
    }
}

*Width = *cspacec::csPixmap_Width;
*Height = *cspacec::csPixmap_Height;
*Advance = *cspacec::csPixmap_Advance;
*GetTextureHandle = *cspacec::csPixmap_GetTextureHandle;
*DrawScaled = *cspacec::csPixmap_DrawScaled;
*DrawScaledAlign = *cspacec::csPixmap_DrawScaledAlign;
*Draw = *cspacec::csPixmap_Draw;
*DrawAlign = *cspacec::csPixmap_DrawAlign;
*DrawTiled = *cspacec::csPixmap_DrawTiled;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csSimplePixmap ##############

package cspace::csSimplePixmap;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::csPixmap cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csSimplePixmap(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csSimplePixmap($self);
        delete $OWNER{$self};
    }
}

*SetTextureHandle = *cspacec::csSimplePixmap_SetTextureHandle;
*SetTextureRectangle = *cspacec::csSimplePixmap_SetTextureRectangle;
*DrawScaled = *cspacec::csSimplePixmap_DrawScaled;
*DrawTiled = *cspacec::csSimplePixmap_DrawTiled;
*Width = *cspacec::csSimplePixmap_Width;
*Height = *cspacec::csSimplePixmap_Height;
*Advance = *cspacec::csSimplePixmap_Advance;
*GetTextureHandle = *cspacec::csSimplePixmap_GetTextureHandle;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csShortestDistanceResult ##############

package cspace::csShortestDistanceResult;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_sqdistance_get = *cspacec::csShortestDistanceResult_sqdistance_get;
*swig_sqdistance_set = *cspacec::csShortestDistanceResult_sqdistance_set;
*swig_direction_get = *cspacec::csShortestDistanceResult_direction_get;
*swig_direction_set = *cspacec::csShortestDistanceResult_direction_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csShortestDistanceResult(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csShortestDistanceResult($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csScreenTargetResult ##############

package cspace::csScreenTargetResult;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*swig_mesh_get = *cspacec::csScreenTargetResult_mesh_get;
*swig_mesh_set = *cspacec::csScreenTargetResult_mesh_set;
*swig_isect_get = *cspacec::csScreenTargetResult_isect_get;
*swig_isect_set = *cspacec::csScreenTargetResult_isect_set;
*swig_polygon_idx_get = *cspacec::csScreenTargetResult_polygon_idx_get;
*swig_polygon_idx_set = *cspacec::csScreenTargetResult_polygon_idx_set;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csScreenTargetResult(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csScreenTargetResult($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csEngineTools ##############

package cspace::csEngineTools;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*FindShortestDistance = *cspacec::csEngineTools_FindShortestDistance;
*FindScreenTarget = *cspacec::csEngineTools_FindScreenTarget;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csEngineTools(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csEngineTools($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::iPen ##############

package cspace::iPen;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
*SetFlag = *cspacec::iPen_SetFlag;
*ClearFlag = *cspacec::iPen_ClearFlag;
*SetMixMode = *cspacec::iPen_SetMixMode;
*SetColor = *cspacec::iPen_SetColor;
*SetTexture = *cspacec::iPen_SetTexture;
*SwapColors = *cspacec::iPen_SwapColors;
*SetPenWidth = *cspacec::iPen_SetPenWidth;
*ClearTransform = *cspacec::iPen_ClearTransform;
*PushTransform = *cspacec::iPen_PushTransform;
*PopTransform = *cspacec::iPen_PopTransform;
*SetOrigin = *cspacec::iPen_SetOrigin;
*Translate = *cspacec::iPen_Translate;
*Rotate = *cspacec::iPen_Rotate;
*DrawLine = *cspacec::iPen_DrawLine;
*DrawPoint = *cspacec::iPen_DrawPoint;
*DrawRect = *cspacec::iPen_DrawRect;
*DrawMiteredRect = *cspacec::iPen_DrawMiteredRect;
*DrawRoundedRect = *cspacec::iPen_DrawRoundedRect;
*DrawArc = *cspacec::iPen_DrawArc;
*DrawTriangle = *cspacec::iPen_DrawTriangle;
*Write = *cspacec::iPen_Write;
*WriteBoxed = *cspacec::iPen_WriteBoxed;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPen ##############

package cspace::csPen;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iPen cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPen(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPen($self);
        delete $OWNER{$self};
    }
}

*SetFlag = *cspacec::csPen_SetFlag;
*ClearFlag = *cspacec::csPen_ClearFlag;
*SetMixMode = *cspacec::csPen_SetMixMode;
*SetColor = *cspacec::csPen_SetColor;
*SetTexture = *cspacec::csPen_SetTexture;
*SwapColors = *cspacec::csPen_SwapColors;
*SetPenWidth = *cspacec::csPen_SetPenWidth;
*ClearTransform = *cspacec::csPen_ClearTransform;
*PushTransform = *cspacec::csPen_PushTransform;
*PopTransform = *cspacec::csPen_PopTransform;
*SetOrigin = *cspacec::csPen_SetOrigin;
*Translate = *cspacec::csPen_Translate;
*Rotate = *cspacec::csPen_Rotate;
*DrawLine = *cspacec::csPen_DrawLine;
*DrawThickLine = *cspacec::csPen_DrawThickLine;
*DrawPoint = *cspacec::csPen_DrawPoint;
*DrawRect = *cspacec::csPen_DrawRect;
*DrawMiteredRect = *cspacec::csPen_DrawMiteredRect;
*DrawRoundedRect = *cspacec::csPen_DrawRoundedRect;
*DrawArc = *cspacec::csPen_DrawArc;
*DrawTriangle = *cspacec::csPen_DrawTriangle;
*Write = *cspacec::csPen_Write;
*WriteBoxed = *cspacec::csPen_WriteBoxed;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csMemoryPen ##############

package cspace::csMemoryPen;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace::iPen cspace );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csMemoryPen(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csMemoryPen($self);
        delete $OWNER{$self};
    }
}

*Clear = *cspacec::csMemoryPen_Clear;
*Draw = *cspacec::csMemoryPen_Draw;
*SetFlag = *cspacec::csMemoryPen_SetFlag;
*ClearFlag = *cspacec::csMemoryPen_ClearFlag;
*SetMixMode = *cspacec::csMemoryPen_SetMixMode;
*SetColor = *cspacec::csMemoryPen_SetColor;
*SetTexture = *cspacec::csMemoryPen_SetTexture;
*SwapColors = *cspacec::csMemoryPen_SwapColors;
*SetPenWidth = *cspacec::csMemoryPen_SetPenWidth;
*ClearTransform = *cspacec::csMemoryPen_ClearTransform;
*PushTransform = *cspacec::csMemoryPen_PushTransform;
*PopTransform = *cspacec::csMemoryPen_PopTransform;
*SetOrigin = *cspacec::csMemoryPen_SetOrigin;
*Translate = *cspacec::csMemoryPen_Translate;
*Rotate = *cspacec::csMemoryPen_Rotate;
*DrawLine = *cspacec::csMemoryPen_DrawLine;
*DrawPoint = *cspacec::csMemoryPen_DrawPoint;
*DrawRect = *cspacec::csMemoryPen_DrawRect;
*DrawMiteredRect = *cspacec::csMemoryPen_DrawMiteredRect;
*DrawRoundedRect = *cspacec::csMemoryPen_DrawRoundedRect;
*DrawArc = *cspacec::csMemoryPen_DrawArc;
*DrawTriangle = *cspacec::csMemoryPen_DrawTriangle;
*Write = *cspacec::csMemoryPen_Write;
*WriteBoxed = *cspacec::csMemoryPen_WriteBoxed;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : cspace::csPrimitives ##############

package cspace::csPrimitives;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( cspace );
%OWNER = ();
%ITERATORS = ();
*GenerateBox = *cspacec::csPrimitives_GenerateBox;
*GenerateQuad = *cspacec::csPrimitives_GenerateQuad;
*GenerateSphere = *cspacec::csPrimitives_GenerateSphere;
sub new {
    my $pkg = shift;
    my $self = cspacec::new_csPrimitives(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        cspacec::delete_csPrimitives($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


# ------- CONSTANT STUBS -------

package cspace;

sub CS_WCHAR_T_SIZE () { $cspacec::CS_WCHAR_T_SIZE }
sub SCF_STATIC_CLASS_CONTEXT () { $cspacec::SCF_STATIC_CLASS_CONTEXT }
sub SCF_VERBOSE_NONE () { $cspacec::SCF_VERBOSE_NONE }
sub SCF_VERBOSE_PLUGIN_SCAN () { $cspacec::SCF_VERBOSE_PLUGIN_SCAN }
sub SCF_VERBOSE_PLUGIN_LOAD () { $cspacec::SCF_VERBOSE_PLUGIN_LOAD }
sub SCF_VERBOSE_PLUGIN_REGISTER () { $cspacec::SCF_VERBOSE_PLUGIN_REGISTER }
sub SCF_VERBOSE_CLASS_REGISTER () { $cspacec::SCF_VERBOSE_CLASS_REGISTER }
sub SCF_VERBOSE_ALL () { $cspacec::SCF_VERBOSE_ALL }
sub CS_DBGHELP_UNITTEST () { $cspacec::CS_DBGHELP_UNITTEST }
sub CS_DBGHELP_BENCHMARK () { $cspacec::CS_DBGHELP_BENCHMARK }
sub CS_DBGHELP_TXTDUMP () { $cspacec::CS_DBGHELP_TXTDUMP }
sub CS_DBGHELP_GFXDUMP () { $cspacec::CS_DBGHELP_GFXDUMP }
sub CS_DBGHELP_STATETEST () { $cspacec::CS_DBGHELP_STATETEST }
sub CS_AXIS_NONE () { $cspacec::CS_AXIS_NONE }
sub CS_AXIS_X () { $cspacec::CS_AXIS_X }
sub CS_AXIS_Y () { $cspacec::CS_AXIS_Y }
sub CS_AXIS_Z () { $cspacec::CS_AXIS_Z }
sub CS_AXIS_W () { $cspacec::CS_AXIS_W }
sub CS_POLY_IN () { $cspacec::CS_POLY_IN }
sub CS_POLY_ON () { $cspacec::CS_POLY_ON }
sub CS_POLY_OUT () { $cspacec::CS_POLY_OUT }
sub CS_POL_SAME_PLANE () { $cspacec::CS_POL_SAME_PLANE }
sub CS_POL_FRONT () { $cspacec::CS_POL_FRONT }
sub CS_POL_BACK () { $cspacec::CS_POL_BACK }
sub CS_POL_SPLIT_NEEDED () { $cspacec::CS_POL_SPLIT_NEEDED }
sub CS_BOUNDINGBOX_MAXVALUE () { $cspacec::CS_BOUNDINGBOX_MAXVALUE }
sub CS_BOX_CORNER_xy () { $cspacec::CS_BOX_CORNER_xy }
sub CS_BOX_CORNER_xY () { $cspacec::CS_BOX_CORNER_xY }
sub CS_BOX_CORNER_Xy () { $cspacec::CS_BOX_CORNER_Xy }
sub CS_BOX_CORNER_XY () { $cspacec::CS_BOX_CORNER_XY }
sub CS_BOX_CENTER2 () { $cspacec::CS_BOX_CENTER2 }
sub CS_BOX_EDGE_xy_Xy () { $cspacec::CS_BOX_EDGE_xy_Xy }
sub CS_BOX_EDGE_Xy_xy () { $cspacec::CS_BOX_EDGE_Xy_xy }
sub CS_BOX_EDGE_Xy_XY () { $cspacec::CS_BOX_EDGE_Xy_XY }
sub CS_BOX_EDGE_XY_Xy () { $cspacec::CS_BOX_EDGE_XY_Xy }
sub CS_BOX_EDGE_XY_xY () { $cspacec::CS_BOX_EDGE_XY_xY }
sub CS_BOX_EDGE_xY_XY () { $cspacec::CS_BOX_EDGE_xY_XY }
sub CS_BOX_EDGE_xY_xy () { $cspacec::CS_BOX_EDGE_xY_xy }
sub CS_BOX_EDGE_xy_xY () { $cspacec::CS_BOX_EDGE_xy_xY }
sub CS_BOX_CORNER_xyz () { $cspacec::CS_BOX_CORNER_xyz }
sub CS_BOX_CORNER_xyZ () { $cspacec::CS_BOX_CORNER_xyZ }
sub CS_BOX_CORNER_xYz () { $cspacec::CS_BOX_CORNER_xYz }
sub CS_BOX_CORNER_xYZ () { $cspacec::CS_BOX_CORNER_xYZ }
sub CS_BOX_CORNER_Xyz () { $cspacec::CS_BOX_CORNER_Xyz }
sub CS_BOX_CORNER_XyZ () { $cspacec::CS_BOX_CORNER_XyZ }
sub CS_BOX_CORNER_XYz () { $cspacec::CS_BOX_CORNER_XYz }
sub CS_BOX_CORNER_XYZ () { $cspacec::CS_BOX_CORNER_XYZ }
sub CS_BOX_CENTER3 () { $cspacec::CS_BOX_CENTER3 }
sub CS_BOX_SIDE_x () { $cspacec::CS_BOX_SIDE_x }
sub CS_BOX_SIDE_X () { $cspacec::CS_BOX_SIDE_X }
sub CS_BOX_SIDE_y () { $cspacec::CS_BOX_SIDE_y }
sub CS_BOX_SIDE_Y () { $cspacec::CS_BOX_SIDE_Y }
sub CS_BOX_SIDE_z () { $cspacec::CS_BOX_SIDE_z }
sub CS_BOX_SIDE_Z () { $cspacec::CS_BOX_SIDE_Z }
sub CS_BOX_INSIDE () { $cspacec::CS_BOX_INSIDE }
sub CS_BOX_EDGE_Xyz_xyz () { $cspacec::CS_BOX_EDGE_Xyz_xyz }
sub CS_BOX_EDGE_xyz_Xyz () { $cspacec::CS_BOX_EDGE_xyz_Xyz }
sub CS_BOX_EDGE_xyz_xYz () { $cspacec::CS_BOX_EDGE_xyz_xYz }
sub CS_BOX_EDGE_xYz_xyz () { $cspacec::CS_BOX_EDGE_xYz_xyz }
sub CS_BOX_EDGE_xYz_XYz () { $cspacec::CS_BOX_EDGE_xYz_XYz }
sub CS_BOX_EDGE_XYz_xYz () { $cspacec::CS_BOX_EDGE_XYz_xYz }
sub CS_BOX_EDGE_XYz_Xyz () { $cspacec::CS_BOX_EDGE_XYz_Xyz }
sub CS_BOX_EDGE_Xyz_XYz () { $cspacec::CS_BOX_EDGE_Xyz_XYz }
sub CS_BOX_EDGE_Xyz_XyZ () { $cspacec::CS_BOX_EDGE_Xyz_XyZ }
sub CS_BOX_EDGE_XyZ_Xyz () { $cspacec::CS_BOX_EDGE_XyZ_Xyz }
sub CS_BOX_EDGE_XyZ_XYZ () { $cspacec::CS_BOX_EDGE_XyZ_XYZ }
sub CS_BOX_EDGE_XYZ_XyZ () { $cspacec::CS_BOX_EDGE_XYZ_XyZ }
sub CS_BOX_EDGE_XYZ_XYz () { $cspacec::CS_BOX_EDGE_XYZ_XYz }
sub CS_BOX_EDGE_XYz_XYZ () { $cspacec::CS_BOX_EDGE_XYz_XYZ }
sub CS_BOX_EDGE_XYZ_xYZ () { $cspacec::CS_BOX_EDGE_XYZ_xYZ }
sub CS_BOX_EDGE_xYZ_XYZ () { $cspacec::CS_BOX_EDGE_xYZ_XYZ }
sub CS_BOX_EDGE_xYZ_xYz () { $cspacec::CS_BOX_EDGE_xYZ_xYz }
sub CS_BOX_EDGE_xYz_xYZ () { $cspacec::CS_BOX_EDGE_xYz_xYZ }
sub CS_BOX_EDGE_xYZ_xyZ () { $cspacec::CS_BOX_EDGE_xYZ_xyZ }
sub CS_BOX_EDGE_xyZ_xYZ () { $cspacec::CS_BOX_EDGE_xyZ_xYZ }
sub CS_BOX_EDGE_xyZ_xyz () { $cspacec::CS_BOX_EDGE_xyZ_xyz }
sub CS_BOX_EDGE_xyz_xyZ () { $cspacec::CS_BOX_EDGE_xyz_xyZ }
sub CS_BOX_EDGE_xyZ_XyZ () { $cspacec::CS_BOX_EDGE_xyZ_XyZ }
sub CS_BOX_EDGE_XyZ_xyZ () { $cspacec::CS_BOX_EDGE_XyZ_xyZ }
sub R_COEF () { $cspacec::R_COEF }
sub G_COEF () { $cspacec::G_COEF }
sub B_COEF () { $cspacec::B_COEF }
sub R_COEF_SQ () { $cspacec::R_COEF_SQ }
sub G_COEF_SQ () { $cspacec::G_COEF_SQ }
sub B_COEF_SQ () { $cspacec::B_COEF_SQ }
sub MAX_OUTPUT_VERTICES () { $cspacec::MAX_OUTPUT_VERTICES }
sub CS_CLIP_OUTSIDE () { $cspacec::CS_CLIP_OUTSIDE }
sub CS_CLIP_CLIPPED () { $cspacec::CS_CLIP_CLIPPED }
sub CS_CLIP_INSIDE () { $cspacec::CS_CLIP_INSIDE }
sub CS_VERTEX_ORIGINAL () { $cspacec::CS_VERTEX_ORIGINAL }
sub CS_VERTEX_ONEDGE () { $cspacec::CS_VERTEX_ONEDGE }
sub CS_VERTEX_INSIDE () { $cspacec::CS_VERTEX_INSIDE }
sub CS_POLYMESH_CLOSED () { $cspacec::CS_POLYMESH_CLOSED }
sub CS_POLYMESH_NOTCLOSED () { $cspacec::CS_POLYMESH_NOTCLOSED }
sub CS_POLYMESH_CONVEX () { $cspacec::CS_POLYMESH_CONVEX }
sub CS_POLYMESH_NOTCONVEX () { $cspacec::CS_POLYMESH_NOTCONVEX }
sub CS_POLYMESH_DEFORMABLE () { $cspacec::CS_POLYMESH_DEFORMABLE }
sub CS_POLYMESH_TRIANGLEMESH () { $cspacec::CS_POLYMESH_TRIANGLEMESH }
sub CS_LIGHT_ACTIVEHALO () { $cspacec::CS_LIGHT_ACTIVEHALO }
sub CS_LIGHT_DYNAMICTYPE_STATIC () { $cspacec::CS_LIGHT_DYNAMICTYPE_STATIC }
sub CS_LIGHT_DYNAMICTYPE_PSEUDO () { $cspacec::CS_LIGHT_DYNAMICTYPE_PSEUDO }
sub CS_LIGHT_DYNAMICTYPE_DYNAMIC () { $cspacec::CS_LIGHT_DYNAMICTYPE_DYNAMIC }
sub CS_DEFAULT_LIGHT_LEVEL () { $cspacec::CS_DEFAULT_LIGHT_LEVEL }
sub CS_NORMAL_LIGHT_LEVEL () { $cspacec::CS_NORMAL_LIGHT_LEVEL }
sub CS_ATTN_NONE () { $cspacec::CS_ATTN_NONE }
sub CS_ATTN_LINEAR () { $cspacec::CS_ATTN_LINEAR }
sub CS_ATTN_INVERSE () { $cspacec::CS_ATTN_INVERSE }
sub CS_ATTN_REALISTIC () { $cspacec::CS_ATTN_REALISTIC }
sub CS_ATTN_CLQ () { $cspacec::CS_ATTN_CLQ }
sub CS_LIGHT_POINTLIGHT () { $cspacec::CS_LIGHT_POINTLIGHT }
sub CS_LIGHT_DIRECTIONAL () { $cspacec::CS_LIGHT_DIRECTIONAL }
sub CS_LIGHT_SPOTLIGHT () { $cspacec::CS_LIGHT_SPOTLIGHT }
sub CS_FOG_MODE_NONE () { $cspacec::CS_FOG_MODE_NONE }
sub CS_FOG_MODE_LINEAR () { $cspacec::CS_FOG_MODE_LINEAR }
sub CS_FOG_MODE_EXP () { $cspacec::CS_FOG_MODE_EXP }
sub CS_FOG_MODE_EXP2 () { $cspacec::CS_FOG_MODE_EXP2 }
sub CS_FOG_MODE_CRYSTALSPACE () { $cspacec::CS_FOG_MODE_CRYSTALSPACE }
sub CS_ENGINE_CACHE_READ () { $cspacec::CS_ENGINE_CACHE_READ }
sub CS_ENGINE_CACHE_WRITE () { $cspacec::CS_ENGINE_CACHE_WRITE }
sub CS_ENGINE_CACHE_NOUPDATE () { $cspacec::CS_ENGINE_CACHE_NOUPDATE }
sub CS_RENDPRI_SORT_NONE () { $cspacec::CS_RENDPRI_SORT_NONE }
sub CS_RENDPRI_SORT_BACK2FRONT () { $cspacec::CS_RENDPRI_SORT_BACK2FRONT }
sub CS_RENDPRI_SORT_FRONT2BACK () { $cspacec::CS_RENDPRI_SORT_FRONT2BACK }
sub CS_ENTITY_DETAIL () { $cspacec::CS_ENTITY_DETAIL }
sub CS_ENTITY_CAMERA () { $cspacec::CS_ENTITY_CAMERA }
sub CS_ENTITY_INVISIBLEMESH () { $cspacec::CS_ENTITY_INVISIBLEMESH }
sub CS_ENTITY_NOSHADOWS () { $cspacec::CS_ENTITY_NOSHADOWS }
sub CS_ENTITY_NOLIGHTING () { $cspacec::CS_ENTITY_NOLIGHTING }
sub CS_ENTITY_NOHITBEAM () { $cspacec::CS_ENTITY_NOHITBEAM }
sub CS_ENTITY_NOCLIP () { $cspacec::CS_ENTITY_NOCLIP }
sub CS_LIGHTINGUPDATE_SORTRELEVANCE () { $cspacec::CS_LIGHTINGUPDATE_SORTRELEVANCE }
sub CS_LIGHTINGUPDATE_ALWAYSUPDATE () { $cspacec::CS_LIGHTINGUPDATE_ALWAYSUPDATE }
sub CS_CULLER_HINT_GOODOCCLUDER () { $cspacec::CS_CULLER_HINT_GOODOCCLUDER }
sub CS_CULLER_HINT_BADOCCLUDER () { $cspacec::CS_CULLER_HINT_BADOCCLUDER }
sub CS_PORTAL_CLIPDEST () { $cspacec::CS_PORTAL_CLIPDEST }
sub CS_PORTAL_CLIPSTRADDLING () { $cspacec::CS_PORTAL_CLIPSTRADDLING }
sub CS_PORTAL_ZFILL () { $cspacec::CS_PORTAL_ZFILL }
sub CS_PORTAL_WARP () { $cspacec::CS_PORTAL_WARP }
sub CS_PORTAL_MIRROR () { $cspacec::CS_PORTAL_MIRROR }
sub CS_PORTAL_STATICDEST () { $cspacec::CS_PORTAL_STATICDEST }
sub CS_PORTAL_FLOAT () { $cspacec::CS_PORTAL_FLOAT }
sub CS_PORTAL_COLLDET () { $cspacec::CS_PORTAL_COLLDET }
sub CS_PORTAL_VISCULL () { $cspacec::CS_PORTAL_VISCULL }
sub CS_BTT_NONE () { $cspacec::CS_BTT_NONE }
sub CS_BTT_SCRIPT () { $cspacec::CS_BTT_SCRIPT }
sub CS_BTT_RIGID_BODY () { $cspacec::CS_BTT_RIGID_BODY }
sub CS_BGT_NONE () { $cspacec::CS_BGT_NONE }
sub CS_BGT_BOX () { $cspacec::CS_BGT_BOX }
sub CS_BGT_SPHERE () { $cspacec::CS_BGT_SPHERE }
sub CS_BGT_CYLINDER () { $cspacec::CS_BGT_CYLINDER }
sub CS_SPR_LIGHTING_HQ () { $cspacec::CS_SPR_LIGHTING_HQ }
sub CS_SPR_LIGHTING_LQ () { $cspacec::CS_SPR_LIGHTING_LQ }
sub CS_SPR_LIGHTING_FAST () { $cspacec::CS_SPR_LIGHTING_FAST }
sub CS_SPR_LIGHTING_RANDOM () { $cspacec::CS_SPR_LIGHTING_RANDOM }
sub CS_SPR_LIGHT_GLOBAL () { $cspacec::CS_SPR_LIGHT_GLOBAL }
sub CS_SPR_LIGHT_TEMPLATE () { $cspacec::CS_SPR_LIGHT_TEMPLATE }
sub CS_SPR_LIGHT_LOCAL () { $cspacec::CS_SPR_LIGHT_LOCAL }
sub CS_SPR_LOD_GLOBAL () { $cspacec::CS_SPR_LOD_GLOBAL }
sub CS_SPR_LOD_TEMPLATE () { $cspacec::CS_SPR_LOD_TEMPLATE }
sub CS_SPR_LOD_LOCAL () { $cspacec::CS_SPR_LOD_LOCAL }
sub CS_MESH_STATICPOS () { $cspacec::CS_MESH_STATICPOS }
sub CS_MESH_STATICSHAPE () { $cspacec::CS_MESH_STATICSHAPE }
sub CS_FACTORY_STATICSHAPE () { $cspacec::CS_FACTORY_STATICSHAPE }
sub CS_POLY_LIGHTING () { $cspacec::CS_POLY_LIGHTING }
sub CS_POLY_COLLDET () { $cspacec::CS_POLY_COLLDET }
sub CS_POLY_VISCULL () { $cspacec::CS_POLY_VISCULL }
sub CS_POLYINDEX_LAST () { $cspacec::CS_POLYINDEX_LAST }
sub CS_THING_NOCOMPRESS () { $cspacec::CS_THING_NOCOMPRESS }
sub CS_THING_MOVE_NEVER () { $cspacec::CS_THING_MOVE_NEVER }
sub CS_THING_MOVE_OCCASIONAL () { $cspacec::CS_THING_MOVE_OCCASIONAL }
sub CS_PARTICLE_SORT_NONE () { $cspacec::CS_PARTICLE_SORT_NONE }
sub CS_PARTICLE_SORT_DISTANCE () { $cspacec::CS_PARTICLE_SORT_DISTANCE }
sub CS_PARTICLE_SORT_DOT () { $cspacec::CS_PARTICLE_SORT_DOT }
sub CS_PARTICLE_CAMERAFACE () { $cspacec::CS_PARTICLE_CAMERAFACE }
sub CS_PARTICLE_CAMERAFACE_APPROX () { $cspacec::CS_PARTICLE_CAMERAFACE_APPROX }
sub CS_PARTICLE_ORIENT_COMMON () { $cspacec::CS_PARTICLE_ORIENT_COMMON }
sub CS_PARTICLE_ORIENT_COMMON_APPROX () { $cspacec::CS_PARTICLE_ORIENT_COMMON_APPROX }
sub CS_PARTICLE_ORIENT_VELOCITY () { $cspacec::CS_PARTICLE_ORIENT_VELOCITY }
sub CS_PARTICLE_ORIENT_SELF () { $cspacec::CS_PARTICLE_ORIENT_SELF }
sub CS_PARTICLE_ORIENT_SELF_FORWARD () { $cspacec::CS_PARTICLE_ORIENT_SELF_FORWARD }
sub CS_PARTICLE_ROTATE_NONE () { $cspacec::CS_PARTICLE_ROTATE_NONE }
sub CS_PARTICLE_ROTATE_TEXCOORD () { $cspacec::CS_PARTICLE_ROTATE_TEXCOORD }
sub CS_PARTICLE_ROTATE_VERTICES () { $cspacec::CS_PARTICLE_ROTATE_VERTICES }
sub CS_PARTICLE_INTEGRATE_NONE () { $cspacec::CS_PARTICLE_INTEGRATE_NONE }
sub CS_PARTICLE_INTEGRATE_LINEAR () { $cspacec::CS_PARTICLE_INTEGRATE_LINEAR }
sub CS_PARTICLE_INTEGRATE_BOTH () { $cspacec::CS_PARTICLE_INTEGRATE_BOTH }
sub CS_PARTICLE_LOCAL_MODE () { $cspacec::CS_PARTICLE_LOCAL_MODE }
sub CS_PARTICLE_LOCAL_EMITTER () { $cspacec::CS_PARTICLE_LOCAL_EMITTER }
sub CS_PARTICLE_WORLD_MODE () { $cspacec::CS_PARTICLE_WORLD_MODE }
sub CS_PARTICLE_BUILTIN_CENTER () { $cspacec::CS_PARTICLE_BUILTIN_CENTER }
sub CS_PARTICLE_BUILTIN_VOLUME () { $cspacec::CS_PARTICLE_BUILTIN_VOLUME }
sub CS_PARTICLE_BUILTIN_SURFACE () { $cspacec::CS_PARTICLE_BUILTIN_SURFACE }
sub CS_PARTICLE_BUILTIN_SPIRAL () { $cspacec::CS_PARTICLE_BUILTIN_SPIRAL }
sub CS_PARTICLE_BUILTIN_RADIALPOINT () { $cspacec::CS_PARTICLE_BUILTIN_RADIALPOINT }
sub CS_SNDSYS_DATA_UNKNOWN_SIZE () { $cspacec::CS_SNDSYS_DATA_UNKNOWN_SIZE }
sub SS_FILTER_LOC_RENDEROUT () { $cspacec::SS_FILTER_LOC_RENDEROUT }
sub SS_FILTER_LOC_SOURCEOUT () { $cspacec::SS_FILTER_LOC_SOURCEOUT }
sub SS_FILTER_LOC_SOURCEIN () { $cspacec::SS_FILTER_LOC_SOURCEIN }
sub CS_SNDSYS_SOURCE_DISTANCE_INFINITE () { $cspacec::CS_SNDSYS_SOURCE_DISTANCE_INFINITE }
sub CSSNDSYS_SAMPLE_LITTLE_ENDIAN () { $cspacec::CSSNDSYS_SAMPLE_LITTLE_ENDIAN }
sub CSSNDSYS_SAMPLE_BIG_ENDIAN () { $cspacec::CSSNDSYS_SAMPLE_BIG_ENDIAN }
sub CSSNDSYS_SAMPLE_ENDIAN_MASK () { $cspacec::CSSNDSYS_SAMPLE_ENDIAN_MASK }
sub CS_SNDSYS_STREAM_PAUSED () { $cspacec::CS_SNDSYS_STREAM_PAUSED }
sub CS_SNDSYS_STREAM_UNPAUSED () { $cspacec::CS_SNDSYS_STREAM_UNPAUSED }
sub CS_SNDSYS_STREAM_DONTLOOP () { $cspacec::CS_SNDSYS_STREAM_DONTLOOP }
sub CS_SNDSYS_STREAM_LOOP () { $cspacec::CS_SNDSYS_STREAM_LOOP }
sub CS_SND3D_DISABLE () { $cspacec::CS_SND3D_DISABLE }
sub CS_SND3D_RELATIVE () { $cspacec::CS_SND3D_RELATIVE }
sub CS_SND3D_ABSOLUTE () { $cspacec::CS_SND3D_ABSOLUTE }
sub CS_SNDSYS_SOURCE_STOPPED () { $cspacec::CS_SNDSYS_SOURCE_STOPPED }
sub CS_SNDSYS_SOURCE_PLAYING () { $cspacec::CS_SNDSYS_SOURCE_PLAYING }
sub VFS_PATH_DIVIDER () { $cspacec::VFS_PATH_DIVIDER }
sub VFS_PATH_SEPARATOR () { $cspacec::VFS_PATH_SEPARATOR }
sub VFS_MAX_PATH_LEN () { $cspacec::VFS_MAX_PATH_LEN }
sub VFS_FILE_MODE () { $cspacec::VFS_FILE_MODE }
sub VFS_FILE_READ () { $cspacec::VFS_FILE_READ }
sub VFS_FILE_WRITE () { $cspacec::VFS_FILE_WRITE }
sub VFS_FILE_APPEND () { $cspacec::VFS_FILE_APPEND }
sub VFS_FILE_UNCOMPRESSED () { $cspacec::VFS_FILE_UNCOMPRESSED }
sub VFS_STATUS_OK () { $cspacec::VFS_STATUS_OK }
sub VFS_STATUS_OTHER () { $cspacec::VFS_STATUS_OTHER }
sub VFS_STATUS_NOSPACE () { $cspacec::VFS_STATUS_NOSPACE }
sub VFS_STATUS_RESOURCES () { $cspacec::VFS_STATUS_RESOURCES }
sub VFS_STATUS_ACCESSDENIED () { $cspacec::VFS_STATUS_ACCESSDENIED }
sub VFS_STATUS_IOERROR () { $cspacec::VFS_STATUS_IOERROR }
sub CS_MAX_MOUSE_COUNT () { $cspacec::CS_MAX_MOUSE_COUNT }
sub CS_MAX_MOUSE_AXES () { $cspacec::CS_MAX_MOUSE_AXES }
sub CS_MAX_MOUSE_BUTTONS () { $cspacec::CS_MAX_MOUSE_BUTTONS }
sub CS_MAX_JOYSTICK_COUNT () { $cspacec::CS_MAX_JOYSTICK_COUNT }
sub CS_MAX_JOYSTICK_BUTTONS () { $cspacec::CS_MAX_JOYSTICK_BUTTONS }
sub CS_MAX_JOYSTICK_AXES () { $cspacec::CS_MAX_JOYSTICK_AXES }
sub csEventErrNone () { $cspacec::csEventErrNone }
sub csEventErrLossy () { $cspacec::csEventErrLossy }
sub csEventErrNotFound () { $cspacec::csEventErrNotFound }
sub csEventErrMismatchInt () { $cspacec::csEventErrMismatchInt }
sub csEventErrMismatchUInt () { $cspacec::csEventErrMismatchUInt }
sub csEventErrMismatchFloat () { $cspacec::csEventErrMismatchFloat }
sub csEventErrMismatchBuffer () { $cspacec::csEventErrMismatchBuffer }
sub csEventErrMismatchEvent () { $cspacec::csEventErrMismatchEvent }
sub csEventErrMismatchIBase () { $cspacec::csEventErrMismatchIBase }
sub csEventErrUhOhUnknown () { $cspacec::csEventErrUhOhUnknown }
sub csEventAttrUnknown () { $cspacec::csEventAttrUnknown }
sub csEventAttrInt () { $cspacec::csEventAttrInt }
sub csEventAttrUInt () { $cspacec::csEventAttrUInt }
sub csEventAttrFloat () { $cspacec::csEventAttrFloat }
sub csEventAttrDatabuffer () { $cspacec::csEventAttrDatabuffer }
sub csEventAttrEvent () { $cspacec::csEventAttrEvent }
sub csEventAttriBase () { $cspacec::csEventAttriBase }
sub csKeyEventTypeUp () { $cspacec::csKeyEventTypeUp }
sub csKeyEventTypeDown () { $cspacec::csKeyEventTypeDown }
sub csMouseEventTypeMove () { $cspacec::csMouseEventTypeMove }
sub csMouseEventTypeUp () { $cspacec::csMouseEventTypeUp }
sub csMouseEventTypeDown () { $cspacec::csMouseEventTypeDown }
sub csMouseEventTypeClick () { $cspacec::csMouseEventTypeClick }
sub csMouseEventTypeDoubleClick () { $cspacec::csMouseEventTypeDoubleClick }
sub csmbNone () { $cspacec::csmbNone }
sub csmbLeft () { $cspacec::csmbLeft }
sub csmbRight () { $cspacec::csmbRight }
sub csmbMiddle () { $cspacec::csmbMiddle }
sub csmbWheelUp () { $cspacec::csmbWheelUp }
sub csmbWheelDown () { $cspacec::csmbWheelDown }
sub csmbExtra1 () { $cspacec::csmbExtra1 }
sub csmbExtra2 () { $cspacec::csmbExtra2 }
sub csKeyModifierTypeShift () { $cspacec::csKeyModifierTypeShift }
sub csKeyModifierTypeCtrl () { $cspacec::csKeyModifierTypeCtrl }
sub csKeyModifierTypeAlt () { $cspacec::csKeyModifierTypeAlt }
sub csKeyModifierTypeCapsLock () { $cspacec::csKeyModifierTypeCapsLock }
sub csKeyModifierTypeNumLock () { $cspacec::csKeyModifierTypeNumLock }
sub csKeyModifierTypeScrollLock () { $cspacec::csKeyModifierTypeScrollLock }
sub csKeyModifierTypeLast () { $cspacec::csKeyModifierTypeLast }
sub csKeyModifierNumLeft () { $cspacec::csKeyModifierNumLeft }
sub csKeyModifierNumRight () { $cspacec::csKeyModifierNumRight }
sub csKeyModifierNumAny () { $cspacec::csKeyModifierNumAny }
sub CSKEY_ESC () { $cspacec::CSKEY_ESC }
sub CSKEY_ENTER () { $cspacec::CSKEY_ENTER }
sub CSKEY_TAB () { $cspacec::CSKEY_TAB }
sub CSKEY_BACKSPACE () { $cspacec::CSKEY_BACKSPACE }
sub CSKEY_SPACE () { $cspacec::CSKEY_SPACE }
sub CSKEY_SPECIAL_FIRST () { $cspacec::CSKEY_SPECIAL_FIRST }
sub CSKEY_SPECIAL_LAST () { $cspacec::CSKEY_SPECIAL_LAST }
sub CSKEY_UP () { $cspacec::CSKEY_UP }
sub CSKEY_DOWN () { $cspacec::CSKEY_DOWN }
sub CSKEY_LEFT () { $cspacec::CSKEY_LEFT }
sub CSKEY_RIGHT () { $cspacec::CSKEY_RIGHT }
sub CSKEY_PGUP () { $cspacec::CSKEY_PGUP }
sub CSKEY_PGDN () { $cspacec::CSKEY_PGDN }
sub CSKEY_HOME () { $cspacec::CSKEY_HOME }
sub CSKEY_END () { $cspacec::CSKEY_END }
sub CSKEY_INS () { $cspacec::CSKEY_INS }
sub CSKEY_DEL () { $cspacec::CSKEY_DEL }
sub CSKEY_CONTEXT () { $cspacec::CSKEY_CONTEXT }
sub CSKEY_PRINTSCREEN () { $cspacec::CSKEY_PRINTSCREEN }
sub CSKEY_PAUSE () { $cspacec::CSKEY_PAUSE }
sub CSKEY_F1 () { $cspacec::CSKEY_F1 }
sub CSKEY_F2 () { $cspacec::CSKEY_F2 }
sub CSKEY_F3 () { $cspacec::CSKEY_F3 }
sub CSKEY_F4 () { $cspacec::CSKEY_F4 }
sub CSKEY_F5 () { $cspacec::CSKEY_F5 }
sub CSKEY_F6 () { $cspacec::CSKEY_F6 }
sub CSKEY_F7 () { $cspacec::CSKEY_F7 }
sub CSKEY_F8 () { $cspacec::CSKEY_F8 }
sub CSKEY_F9 () { $cspacec::CSKEY_F9 }
sub CSKEY_F10 () { $cspacec::CSKEY_F10 }
sub CSKEY_F11 () { $cspacec::CSKEY_F11 }
sub CSKEY_F12 () { $cspacec::CSKEY_F12 }
sub CSKEY_MODIFIER_FIRST () { $cspacec::CSKEY_MODIFIER_FIRST }
sub CSKEY_MODIFIER_LAST () { $cspacec::CSKEY_MODIFIER_LAST }
sub CSKEY_MODIFIERTYPE_SHIFT () { $cspacec::CSKEY_MODIFIERTYPE_SHIFT }
sub CSKEY_PAD_FLAG () { $cspacec::CSKEY_PAD_FLAG }
sub csKeyCharTypeNormal () { $cspacec::csKeyCharTypeNormal }
sub csKeyCharTypeDead () { $cspacec::csKeyCharTypeDead }
sub CSEVTYPE_Keyboard () { $cspacec::CSEVTYPE_Keyboard }
sub CSEVTYPE_Mouse () { $cspacec::CSEVTYPE_Mouse }
sub CSEVTYPE_Joystick () { $cspacec::CSEVTYPE_Joystick }
sub csComposeNoChar () { $cspacec::csComposeNoChar }
sub csComposeNormalChar () { $cspacec::csComposeNormalChar }
sub csComposeComposedChar () { $cspacec::csComposeComposedChar }
sub csComposeUncomposeable () { $cspacec::csComposeUncomposeable }
sub CS_NODE_DOCUMENT () { $cspacec::CS_NODE_DOCUMENT }
sub CS_NODE_ELEMENT () { $cspacec::CS_NODE_ELEMENT }
sub CS_NODE_COMMENT () { $cspacec::CS_NODE_COMMENT }
sub CS_NODE_UNKNOWN () { $cspacec::CS_NODE_UNKNOWN }
sub CS_NODE_TEXT () { $cspacec::CS_NODE_TEXT }
sub CS_NODE_DECLARATION () { $cspacec::CS_NODE_DECLARATION }
sub CS_CHANGEABLE_NEVER () { $cspacec::CS_CHANGEABLE_NEVER }
sub CS_CHANGEABLE_NEWROOT () { $cspacec::CS_CHANGEABLE_NEWROOT }
sub CS_CHANGEABLE_YES () { $cspacec::CS_CHANGEABLE_YES }
sub CS_WRITE_BASELINE () { $cspacec::CS_WRITE_BASELINE }
sub CS_WRITE_NOANTIALIAS () { $cspacec::CS_WRITE_NOANTIALIAS }
sub CSDRAW_2DGRAPHICS () { $cspacec::CSDRAW_2DGRAPHICS }
sub CSDRAW_3DGRAPHICS () { $cspacec::CSDRAW_3DGRAPHICS }
sub CSDRAW_CLEARZBUFFER () { $cspacec::CSDRAW_CLEARZBUFFER }
sub CSDRAW_CLEARSCREEN () { $cspacec::CSDRAW_CLEARSCREEN }
sub CS_CLIPPER_NONE () { $cspacec::CS_CLIPPER_NONE }
sub CS_CLIPPER_OPTIONAL () { $cspacec::CS_CLIPPER_OPTIONAL }
sub CS_CLIPPER_TOPLEVEL () { $cspacec::CS_CLIPPER_TOPLEVEL }
sub CS_CLIPPER_REQUIRED () { $cspacec::CS_CLIPPER_REQUIRED }
sub CS_CLIP_NOT () { $cspacec::CS_CLIP_NOT }
sub CS_CLIP_NEEDED () { $cspacec::CS_CLIP_NEEDED }
sub CS_ZBUF_NONE () { $cspacec::CS_ZBUF_NONE }
sub CS_ZBUF_FILL () { $cspacec::CS_ZBUF_FILL }
sub CS_ZBUF_TEST () { $cspacec::CS_ZBUF_TEST }
sub CS_ZBUF_USE () { $cspacec::CS_ZBUF_USE }
sub CS_ZBUF_EQUAL () { $cspacec::CS_ZBUF_EQUAL }
sub CS_ZBUF_INVERT () { $cspacec::CS_ZBUF_INVERT }
sub CS_ZBUF_MESH () { $cspacec::CS_ZBUF_MESH }
sub CS_ZBUF_MESH2 () { $cspacec::CS_ZBUF_MESH2 }
sub CS_VATTRIB_SPECIFIC_FIRST () { $cspacec::CS_VATTRIB_SPECIFIC_FIRST }
sub CS_VATTRIB_SPECIFIC_LAST () { $cspacec::CS_VATTRIB_SPECIFIC_LAST }
sub CS_VATTRIB_GENERIC_FIRST () { $cspacec::CS_VATTRIB_GENERIC_FIRST }
sub CS_VATTRIB_GENERIC_LAST () { $cspacec::CS_VATTRIB_GENERIC_LAST }
sub CS_VATTRIB_UNUSED () { $cspacec::CS_VATTRIB_UNUSED }
sub CS_VATTRIB_INVALID () { $cspacec::CS_VATTRIB_INVALID }
sub CS_VATTRIB_POSITION () { $cspacec::CS_VATTRIB_POSITION }
sub CS_VATTRIB_WEIGHT () { $cspacec::CS_VATTRIB_WEIGHT }
sub CS_VATTRIB_NORMAL () { $cspacec::CS_VATTRIB_NORMAL }
sub CS_VATTRIB_COLOR () { $cspacec::CS_VATTRIB_COLOR }
sub CS_VATTRIB_PRIMARY_COLOR () { $cspacec::CS_VATTRIB_PRIMARY_COLOR }
sub CS_VATTRIB_SECONDARY_COLOR () { $cspacec::CS_VATTRIB_SECONDARY_COLOR }
sub CS_VATTRIB_FOGCOORD () { $cspacec::CS_VATTRIB_FOGCOORD }
sub CS_VATTRIB_TEXCOORD () { $cspacec::CS_VATTRIB_TEXCOORD }
sub CS_VATTRIB_TEXCOORD0 () { $cspacec::CS_VATTRIB_TEXCOORD0 }
sub CS_VATTRIB_TEXCOORD1 () { $cspacec::CS_VATTRIB_TEXCOORD1 }
sub CS_VATTRIB_TEXCOORD2 () { $cspacec::CS_VATTRIB_TEXCOORD2 }
sub CS_VATTRIB_TEXCOORD3 () { $cspacec::CS_VATTRIB_TEXCOORD3 }
sub CS_VATTRIB_TEXCOORD4 () { $cspacec::CS_VATTRIB_TEXCOORD4 }
sub CS_VATTRIB_TEXCOORD5 () { $cspacec::CS_VATTRIB_TEXCOORD5 }
sub CS_VATTRIB_TEXCOORD6 () { $cspacec::CS_VATTRIB_TEXCOORD6 }
sub CS_VATTRIB_TEXCOORD7 () { $cspacec::CS_VATTRIB_TEXCOORD7 }
sub CS_VATTRIB_0 () { $cspacec::CS_VATTRIB_0 }
sub CS_VATTRIB_1 () { $cspacec::CS_VATTRIB_1 }
sub CS_VATTRIB_2 () { $cspacec::CS_VATTRIB_2 }
sub CS_VATTRIB_3 () { $cspacec::CS_VATTRIB_3 }
sub CS_VATTRIB_4 () { $cspacec::CS_VATTRIB_4 }
sub CS_VATTRIB_5 () { $cspacec::CS_VATTRIB_5 }
sub CS_VATTRIB_6 () { $cspacec::CS_VATTRIB_6 }
sub CS_VATTRIB_7 () { $cspacec::CS_VATTRIB_7 }
sub CS_VATTRIB_8 () { $cspacec::CS_VATTRIB_8 }
sub CS_VATTRIB_9 () { $cspacec::CS_VATTRIB_9 }
sub CS_VATTRIB_10 () { $cspacec::CS_VATTRIB_10 }
sub CS_VATTRIB_11 () { $cspacec::CS_VATTRIB_11 }
sub CS_VATTRIB_12 () { $cspacec::CS_VATTRIB_12 }
sub CS_VATTRIB_13 () { $cspacec::CS_VATTRIB_13 }
sub CS_VATTRIB_14 () { $cspacec::CS_VATTRIB_14 }
sub CS_VATTRIB_15 () { $cspacec::CS_VATTRIB_15 }
sub CS_MIXMODE_TYPE_AUTO () { $cspacec::CS_MIXMODE_TYPE_AUTO }
sub CS_MIXMODE_TYPE_BLENDOP () { $cspacec::CS_MIXMODE_TYPE_BLENDOP }
sub CS_MIXMODE_TYPE_MESH () { $cspacec::CS_MIXMODE_TYPE_MESH }
sub CS_MIXMODE_TYPE_MASK () { $cspacec::CS_MIXMODE_TYPE_MASK }
sub CS_MIXMODE_FACT_ZERO () { $cspacec::CS_MIXMODE_FACT_ZERO }
sub CS_MIXMODE_FACT_ONE () { $cspacec::CS_MIXMODE_FACT_ONE }
sub CS_MIXMODE_FACT_SRCCOLOR () { $cspacec::CS_MIXMODE_FACT_SRCCOLOR }
sub CS_MIXMODE_FACT_SRCCOLOR_INV () { $cspacec::CS_MIXMODE_FACT_SRCCOLOR_INV }
sub CS_MIXMODE_FACT_DSTCOLOR () { $cspacec::CS_MIXMODE_FACT_DSTCOLOR }
sub CS_MIXMODE_FACT_DSTCOLOR_INV () { $cspacec::CS_MIXMODE_FACT_DSTCOLOR_INV }
sub CS_MIXMODE_FACT_SRCALPHA () { $cspacec::CS_MIXMODE_FACT_SRCALPHA }
sub CS_MIXMODE_FACT_SRCALPHA_INV () { $cspacec::CS_MIXMODE_FACT_SRCALPHA_INV }
sub CS_MIXMODE_FACT_DSTALPHA () { $cspacec::CS_MIXMODE_FACT_DSTALPHA }
sub CS_MIXMODE_FACT_DSTALPHA_INV () { $cspacec::CS_MIXMODE_FACT_DSTALPHA_INV }
sub CS_MIXMODE_FACT_COUNT () { $cspacec::CS_MIXMODE_FACT_COUNT }
sub CS_MIXMODE_FACT_MASK () { $cspacec::CS_MIXMODE_FACT_MASK }
sub CS_MIXMODE_ALPHATEST_AUTO () { $cspacec::CS_MIXMODE_ALPHATEST_AUTO }
sub CS_MIXMODE_ALPHATEST_ENABLE () { $cspacec::CS_MIXMODE_ALPHATEST_ENABLE }
sub CS_MIXMODE_ALPHATEST_DISABLE () { $cspacec::CS_MIXMODE_ALPHATEST_DISABLE }
sub CS_MIXMODE_ALPHATEST_MASK () { $cspacec::CS_MIXMODE_ALPHATEST_MASK }
sub CS_FX_COPY () { $cspacec::CS_FX_COPY }
sub CS_FX_MESH () { $cspacec::CS_FX_MESH }
sub CS_FX_FLAT () { $cspacec::CS_FX_FLAT }
sub CS_FX_MASK_ALPHA () { $cspacec::CS_FX_MASK_ALPHA }
sub CS_FX_MASK_MIXMODE () { $cspacec::CS_FX_MASK_MIXMODE }
sub CS_LIGHTPARAM_POSITION () { $cspacec::CS_LIGHTPARAM_POSITION }
sub CS_LIGHTPARAM_DIFFUSE () { $cspacec::CS_LIGHTPARAM_DIFFUSE }
sub CS_LIGHTPARAM_SPECULAR () { $cspacec::CS_LIGHTPARAM_SPECULAR }
sub CS_LIGHTPARAM_ATTENUATION () { $cspacec::CS_LIGHTPARAM_ATTENUATION }
sub CS_SHADOW_VOLUME_BEGIN () { $cspacec::CS_SHADOW_VOLUME_BEGIN }
sub CS_SHADOW_VOLUME_PASS1 () { $cspacec::CS_SHADOW_VOLUME_PASS1 }
sub CS_SHADOW_VOLUME_PASS2 () { $cspacec::CS_SHADOW_VOLUME_PASS2 }
sub CS_SHADOW_VOLUME_FAIL1 () { $cspacec::CS_SHADOW_VOLUME_FAIL1 }
sub CS_SHADOW_VOLUME_FAIL2 () { $cspacec::CS_SHADOW_VOLUME_FAIL2 }
sub CS_SHADOW_VOLUME_USE () { $cspacec::CS_SHADOW_VOLUME_USE }
sub CS_SHADOW_VOLUME_FINISH () { $cspacec::CS_SHADOW_VOLUME_FINISH }
sub G3DRENDERSTATE_ZBUFFERMODE () { $cspacec::G3DRENDERSTATE_ZBUFFERMODE }
sub G3DRENDERSTATE_DITHERENABLE () { $cspacec::G3DRENDERSTATE_DITHERENABLE }
sub G3DRENDERSTATE_BILINEARMAPPINGENABLE () { $cspacec::G3DRENDERSTATE_BILINEARMAPPINGENABLE }
sub G3DRENDERSTATE_TRILINEARMAPPINGENABLE () { $cspacec::G3DRENDERSTATE_TRILINEARMAPPINGENABLE }
sub G3DRENDERSTATE_TRANSPARENCYENABLE () { $cspacec::G3DRENDERSTATE_TRANSPARENCYENABLE }
sub G3DRENDERSTATE_MIPMAPENABLE () { $cspacec::G3DRENDERSTATE_MIPMAPENABLE }
sub G3DRENDERSTATE_TEXTUREMAPPINGENABLE () { $cspacec::G3DRENDERSTATE_TEXTUREMAPPINGENABLE }
sub G3DRENDERSTATE_LIGHTINGENABLE () { $cspacec::G3DRENDERSTATE_LIGHTINGENABLE }
sub G3DRENDERSTATE_INTERLACINGENABLE () { $cspacec::G3DRENDERSTATE_INTERLACINGENABLE }
sub G3DRENDERSTATE_MMXENABLE () { $cspacec::G3DRENDERSTATE_MMXENABLE }
sub G3DRENDERSTATE_INTERPOLATIONSTEP () { $cspacec::G3DRENDERSTATE_INTERPOLATIONSTEP }
sub G3DRENDERSTATE_MAXPOLYGONSTODRAW () { $cspacec::G3DRENDERSTATE_MAXPOLYGONSTODRAW }
sub G3DRENDERSTATE_GOURAUDENABLE () { $cspacec::G3DRENDERSTATE_GOURAUDENABLE }
sub G3DRENDERSTATE_EDGES () { $cspacec::G3DRENDERSTATE_EDGES }
sub CS_MESHTYPE_TRIANGLES () { $cspacec::CS_MESHTYPE_TRIANGLES }
sub CS_MESHTYPE_QUADS () { $cspacec::CS_MESHTYPE_QUADS }
sub CS_MESHTYPE_TRIANGLESTRIP () { $cspacec::CS_MESHTYPE_TRIANGLESTRIP }
sub CS_MESHTYPE_TRIANGLEFAN () { $cspacec::CS_MESHTYPE_TRIANGLEFAN }
sub CS_MESHTYPE_POINTS () { $cspacec::CS_MESHTYPE_POINTS }
sub CS_MESHTYPE_POINT_SPRITES () { $cspacec::CS_MESHTYPE_POINT_SPRITES }
sub CS_MESHTYPE_LINES () { $cspacec::CS_MESHTYPE_LINES }
sub CS_MESHTYPE_LINESTRIP () { $cspacec::CS_MESHTYPE_LINESTRIP }
sub csSimpleMeshScreenspace () { $cspacec::csSimpleMeshScreenspace }
sub CS_OPENPORTAL_ZFILL () { $cspacec::CS_OPENPORTAL_ZFILL }
sub CS_OPENPORTAL_MIRROR () { $cspacec::CS_OPENPORTAL_MIRROR }
sub CS_OPENPORTAL_FLOAT () { $cspacec::CS_OPENPORTAL_FLOAT }
sub csmcNone () { $cspacec::csmcNone }
sub csmcArrow () { $cspacec::csmcArrow }
sub csmcLens () { $cspacec::csmcLens }
sub csmcCross () { $cspacec::csmcCross }
sub csmcPen () { $cspacec::csmcPen }
sub csmcMove () { $cspacec::csmcMove }
sub csmcSizeNWSE () { $cspacec::csmcSizeNWSE }
sub csmcSizeNESW () { $cspacec::csmcSizeNESW }
sub csmcSizeNS () { $cspacec::csmcSizeNS }
sub csmcSizeEW () { $cspacec::csmcSizeEW }
sub csmcStop () { $cspacec::csmcStop }
sub csmcWait () { $cspacec::csmcWait }
sub CS_ALERT_ERROR () { $cspacec::CS_ALERT_ERROR }
sub CS_ALERT_WARNING () { $cspacec::CS_ALERT_WARNING }
sub CS_ALERT_NOTE () { $cspacec::CS_ALERT_NOTE }
sub CSFONT_LARGE () { $cspacec::CSFONT_LARGE }
sub CSFONT_ITALIC () { $cspacec::CSFONT_ITALIC }
sub CSFONT_COURIER () { $cspacec::CSFONT_COURIER }
sub CSFONT_SMALL () { $cspacec::CSFONT_SMALL }
sub CS_FONT_DEFAULT_GLYPH () { $cspacec::CS_FONT_DEFAULT_GLYPH }
sub TagNeutral () { $cspacec::TagNeutral }
sub TagForbidden () { $cspacec::TagForbidden }
sub TagRequired () { $cspacec::TagRequired }
sub CS_TEXTURE_2D () { $cspacec::CS_TEXTURE_2D }
sub CS_TEXTURE_3D () { $cspacec::CS_TEXTURE_3D }
sub CS_TEXTURE_NOMIPMAPS () { $cspacec::CS_TEXTURE_NOMIPMAPS }
sub CS_TEXTURE_CLAMP () { $cspacec::CS_TEXTURE_CLAMP }
sub CS_TEXTURE_NOFILTER () { $cspacec::CS_TEXTURE_NOFILTER }
sub CS_TEXTURE_NPOTS () { $cspacec::CS_TEXTURE_NPOTS }
sub CS_TEXTURE_SCALE_UP () { $cspacec::CS_TEXTURE_SCALE_UP }
sub CS_TEXTURE_SCALE_DOWN () { $cspacec::CS_TEXTURE_SCALE_DOWN }
sub CS_MATERIAL_VARNAME_FLATCOLOR () { $cspacec::CS_MATERIAL_VARNAME_FLATCOLOR }
sub CS_MATERIAL_TEXTURE_DIFFUSE () { $cspacec::CS_MATERIAL_TEXTURE_DIFFUSE }
sub CS_IMGFMT_MASK () { $cspacec::CS_IMGFMT_MASK }
sub CS_IMGFMT_NONE () { $cspacec::CS_IMGFMT_NONE }
sub CS_IMGFMT_TRUECOLOR () { $cspacec::CS_IMGFMT_TRUECOLOR }
sub CS_IMGFMT_PALETTED8 () { $cspacec::CS_IMGFMT_PALETTED8 }
sub CS_IMGFMT_ANY () { $cspacec::CS_IMGFMT_ANY }
sub CS_IMGFMT_ALPHA () { $cspacec::CS_IMGFMT_ALPHA }
sub CS_IMGFMT_INVALID () { $cspacec::CS_IMGFMT_INVALID }
sub csimg2D () { $cspacec::csimg2D }
sub csimg3D () { $cspacec::csimg3D }
sub csimgCube () { $cspacec::csimgCube }
sub CS_IMAGEIO_LOAD () { $cspacec::CS_IMAGEIO_LOAD }
sub CS_IMAGEIO_SAVE () { $cspacec::CS_IMAGEIO_SAVE }
sub CS_REPORTER_SEVERITY_BUG () { $cspacec::CS_REPORTER_SEVERITY_BUG }
sub CS_REPORTER_SEVERITY_ERROR () { $cspacec::CS_REPORTER_SEVERITY_ERROR }
sub CS_REPORTER_SEVERITY_WARNING () { $cspacec::CS_REPORTER_SEVERITY_WARNING }
sub CS_REPORTER_SEVERITY_NOTIFY () { $cspacec::CS_REPORTER_SEVERITY_NOTIFY }
sub CS_REPORTER_SEVERITY_DEBUG () { $cspacec::CS_REPORTER_SEVERITY_DEBUG }
sub csConPageUp () { $cspacec::csConPageUp }
sub csConPageDown () { $cspacec::csConPageDown }
sub csConVeryTop () { $cspacec::csConVeryTop }
sub csConVeryBottom () { $cspacec::csConVeryBottom }
sub csConNoCursor () { $cspacec::csConNoCursor }
sub csConNormalCursor () { $cspacec::csConNormalCursor }
sub csConInsertCursor () { $cspacec::csConInsertCursor }
sub CS_MESH_COLLIDER () { $cspacec::CS_MESH_COLLIDER }
sub CS_TERRAFORMER_COLLIDER () { $cspacec::CS_TERRAFORMER_COLLIDER }
sub CS_TERRAIN_COLLIDER () { $cspacec::CS_TERRAIN_COLLIDER }
sub NO_GEOMETRY () { $cspacec::NO_GEOMETRY }
sub BOX_COLLIDER_GEOMETRY () { $cspacec::BOX_COLLIDER_GEOMETRY }
sub PLANE_COLLIDER_GEOMETRY () { $cspacec::PLANE_COLLIDER_GEOMETRY }
sub TRIMESH_COLLIDER_GEOMETRY () { $cspacec::TRIMESH_COLLIDER_GEOMETRY }
sub CYLINDER_COLLIDER_GEOMETRY () { $cspacec::CYLINDER_COLLIDER_GEOMETRY }
sub SPHERE_COLLIDER_GEOMETRY () { $cspacec::SPHERE_COLLIDER_GEOMETRY }
sub CS_ODE_JOINT_TYPE_UNKNOWN () { $cspacec::CS_ODE_JOINT_TYPE_UNKNOWN }
sub CS_ODE_JOINT_TYPE_BALL () { $cspacec::CS_ODE_JOINT_TYPE_BALL }
sub CS_ODE_JOINT_TYPE_HINGE () { $cspacec::CS_ODE_JOINT_TYPE_HINGE }
sub CS_ODE_JOINT_TYPE_SLIDER () { $cspacec::CS_ODE_JOINT_TYPE_SLIDER }
sub CS_ODE_JOINT_TYPE_CONTACT () { $cspacec::CS_ODE_JOINT_TYPE_CONTACT }
sub CS_ODE_JOINT_TYPE_UNIVERSAL () { $cspacec::CS_ODE_JOINT_TYPE_UNIVERSAL }
sub CS_ODE_JOINT_TYPE_HINGE2 () { $cspacec::CS_ODE_JOINT_TYPE_HINGE2 }
sub CS_ODE_JOINT_TYPE_FIXED () { $cspacec::CS_ODE_JOINT_TYPE_FIXED }
sub CS_ODE_JOINT_TYPE_AMOTOR () { $cspacec::CS_ODE_JOINT_TYPE_AMOTOR }
sub CS_ODE_AMOTOR_MODE_UNKNOWN () { $cspacec::CS_ODE_AMOTOR_MODE_UNKNOWN }
sub CS_ODE_AMOTOR_MODE_USER () { $cspacec::CS_ODE_AMOTOR_MODE_USER }
sub CS_ODE_AMOTOR_MODE_EULER () { $cspacec::CS_ODE_AMOTOR_MODE_EULER }
sub CS_ODE_AMOTOR_MODE_LAST () { $cspacec::CS_ODE_AMOTOR_MODE_LAST }
sub CS_SEQUENCE_LIGHTCHANGE_NONE () { $cspacec::CS_SEQUENCE_LIGHTCHANGE_NONE }
sub CS_SEQUENCE_LIGHTCHANGE_LESS () { $cspacec::CS_SEQUENCE_LIGHTCHANGE_LESS }
sub CS_SEQUENCE_LIGHTCHANGE_GREATER () { $cspacec::CS_SEQUENCE_LIGHTCHANGE_GREATER }
sub CS_PEN_TA_TOP () { $cspacec::CS_PEN_TA_TOP }
sub CS_PEN_TA_BOT () { $cspacec::CS_PEN_TA_BOT }
sub CS_PEN_TA_LEFT () { $cspacec::CS_PEN_TA_LEFT }
sub CS_PEN_TA_RIGHT () { $cspacec::CS_PEN_TA_RIGHT }
sub CS_PEN_TA_CENTER () { $cspacec::CS_PEN_TA_CENTER }
sub CS_PEN_FILL () { $cspacec::CS_PEN_FILL }
sub CS_PEN_SWAPCOLORS () { $cspacec::CS_PEN_SWAPCOLORS }
sub CS_PEN_TEXTURE_ONLY () { $cspacec::CS_PEN_TEXTURE_ONLY }
sub CS_PEN_TEXTURE () { $cspacec::CS_PEN_TEXTURE }

# ------- VARIABLE STUBS --------

package cspace;

*csArrayItemNotFound = *cspacec::csArrayItemNotFound;

my %__iSCF_SCF_hash;
tie %__iSCF_SCF_hash,"cspace::iSCF", $cspacec::iSCF_SCF;
$iSCF_SCF= \%__iSCF_SCF_hash;
bless $iSCF_SCF, cspace::iSCF;
*CS_SNDSYS_STREAM_UNKNOWN_LENGTH = *cspacec::CS_SNDSYS_STREAM_UNKNOWN_LENGTH;
*csInvalidStringID = *cspacec::csInvalidStringID;
1;
