import os
import bpy

try:
  from bpy.types import AddonPreferences
except:
  AddonPreferences = None
 

def rnaType(rna_type):
  '''
  Decorator to register types in blender
  '''
  if bpy: bpy.utils.register_class(rna_type)
  return rna_type

  
def settings(type, attribute="b2cs"):
  '''
  Decorator for PropertyGroup settings class
  Automatically (un)registers in blender
  '''
  def decorator(klass):
    PROPERTYGROUPS.append((klass, type, attribute))
    return klass
  return decorator


def prepend_draw(type):
  '''
  Decorator for prepend drawing custom layouts on panel
  Automatically (un)registers in blender
  '''
  def decorator(func):
    PREPEND_DRAWS.append((func, type))
    return func
  return decorator



PROPERTYGROUPS = []
PREPEND_DRAWS = []
  
def _register():
  for klass, type, attribute in PROPERTYGROUPS:
    print('INFO: Registering PropertyGroup %s for bpy.types.%s.%s'%(klass.__name__, type, attribute))
    #bpy.utils.register_class(klass)
    t = getattr(bpy.types, type)
    p = bpy.props.PointerProperty(type=klass)
    setattr(t, attribute, p)
    
  for func, type in PREPEND_DRAWS:
    t = getattr(bpy.types, type)
    t.prepend(func)
    

def _unregister():
  for klass, type, attribute in PROPERTYGROUPS:
    print('INFO: Unregistering PropertyGroup %s for bpy.types.%s.%s'%(klass.__name__, type, attribute))
    #bpy.utils.unregister_class(klass)
    t = getattr(bpy.types, type)
    delattr(t, attribute)
    
  for func, type in PREPEND_DRAWS:
    t = getattr(bpy.types, type)
    t.remove(func)


def GetPreferences (): 
  if "io_scene_cs.settings" not in bpy.data.texts:
    text = bpy.data.texts.new("io_scene_cs.settings")
  else:
    text = bpy.data.texts["io_scene_cs.settings"]
  return text.preferences
  #Addon properties don't seem to be saved?
  #return bpy.context.user_preferences.addons[__package__].preferences


def GetDefaultPath ():   
  try:
    default_path = GetPreferences().exportPath
  except:
    default_path = os.environ.get("TEMP")
    if not default_path:
      if os.name == 'nt':
        default_path = "c:/tmp/"
      else:
        default_path = "/tmp/"
    elif not default_path.endswith(os.sep):
      default_path += os.sep
  return default_path

def GetExportPath ():
  return GetPreferences().exportPath
  

def WalkTestPath():
  if os.name == 'nt':
    walktest = "walktest.exe"
  else:
    walktest = "walktest"
    
  crystal_env = os.environ.get("CRYSTAL")
  if not crystal_env:
    print("Warning: CRYSTAL environment variable not set!")
    return None
  
  # Look either in the 'CRYSTAL' and 'CRYSTAL/bin' paths
  path = os.path.join(crystal_env, walktest).replace('\\', '/')
  if os.path.exists(path):
    return path
  return os.path.join(crystal_env, "bin", walktest).replace('\\', '/')

            
def HasCrystalSpace():
  path = WalkTestPath()    
  return path and os.path.exists(path)


SHADERSETS = (
  ("DEFAULT", "Default", "Default"),
  ("binalpha", "binalpha", ""),
  ("instanced", "instanced", ""),
  ("instanced_binalpha", "instanced_binalpha", ""),
  ("foliage_grass_fullbright", "foliage_grass_fullbright", ""),
  ("fullbright", "fullbright", ""),
  ("multisplat", "multisplat", ""),
  ("water_plane", "water_plane", ""),
)

def GetShaderSetName(f):
  i = [l[0] for l in SHADERSETS].index(f)
  if i >=0:
    return SHADERSETS[i][1]
  return None

RENDERPRIORITIES = [
  ('init','init',''),('sky','sky',''),('sky2','sky2',''),
  ('portal','portal',''),('wall','wall',''),('wall2','wall2',''),('object','object',''),
  ('object2','object2',''),('transp','transp',''),('alpha','alpha',''),('final','final','')
]

ZBUFFERMODES= [
  ('znone','Z-None',"Don't test or write"),
  ('zfill','Z-Fill',"Write unconditionally"),
  ('ztest','Z-Test',"Test only"),
  ('zuse','Z-Use',"Test, write if successful")
]

