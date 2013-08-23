import os
import bpy

try:
  from bpy.types import AddonPreferences
except:
  AddonPreferences = None


def GetExportPath ():

  ve = bpy.app.version
  if (ve[0] > 2) or (ve[0] == 2 and (ve[1] >= 66 or (ve[1] == 65 and ve[2] >= 5))):
    if AddonPreferences != None and "io_scene_cs" in bpy.context.user_preferences.addons:
      return bpy.context.user_preferences.addons["io_scene_cs"].preferences.exportpath

  return B2CS.properties.exportPath


def rnaType(rna_type):
    if bpy: bpy.utils.register_class(rna_type)
    return rna_type

def rnaOperator(rna_op):
    #if bpy: bpy.types.register(rna_op)
    return rna_op
  
def Property(typ, types, **kwargs):
  attr = kwargs['attr']
  del kwargs['attr']
  kwargs['options'] = {'HIDDEN'}
  #print('I: Added property %s of type %s to %s.'%(attr, typ, str(types)))
  for klass in types:
    p = getattr(bpy.props, typ)
    t = getattr(bpy.types, klass)
    setattr(t, attr, p(**kwargs))


def BoolProperty(types, **kwargs):
  Property('BoolProperty', types, **kwargs)   

def StringProperty(types, **kwargs):
  Property('StringProperty', types, **kwargs)  

def EnumProperty(types, **kwargs):
  Property('EnumProperty', types, **kwargs)  

def FloatProperty(types, **kwargs):
  Property('FloatProperty', types, **kwargs)  

def CollectionProperty(types, **kwargs):
  Property('CollectionProperty', types, **kwargs)  
        
class B2CS:
  def B2CSGetProperties(self):
    if "io_scene_cs.settings" not in bpy.data.texts:
      text = bpy.data.texts.new("io_scene_cs.settings")
    else:
      text = bpy.data.texts["io_scene_cs.settings"]
    return text
    
  def BoolProperty(self, **kwargs):
    BoolProperty(['Text'], **kwargs)
    
  def StringProperty(self, **kwargs):
    StringProperty(['Text'], **kwargs)

  def CollectionProperty(self, **kwargs):
    CollectionProperty(['Text'], **kwargs)
    
  properties = property(B2CSGetProperties) 

B2CS = B2CS()
         
          
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

