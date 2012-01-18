import bpy

from .util import *

from io_scene_cs.utilities import GetShaderName

def GetName(name, dic):
  if name not in dic:
    dic[name] = 1
    return name
  dic[name] += 1
  return name + ' ' + str(dic[name])

MAP = {'use_map_color_diffuse': 'tex diffuse',
       'use_map_normal': 'tex normal',
       'use_map_displacement': 'tex height',
       'use_map_specular': 'tex specular',}

def MaterialAsCS(self, func, depth=0, **kwargs):
  func(' '*depth +'<material name="%s">'%(self.uname))
  dic = {}
  foundDiffuseTexture = False
  for slot in self.texture_slots:
    if slot and slot.texture and slot.texture.type =='IMAGE':
      for type, name in MAP.items():
        if getattr(slot, type, False):
          func(' '*depth +'  <shadervar type="texture" name="%s">%s</shadervar>'%(GetName(name, dic), slot.texture.image.uname))
          if type == 'use_map_color_diffuse':
            foundDiffuseTexture = True

  if not foundDiffuseTexture and 'image' in kwargs:
    image = kwargs['image']
    func(' '*depth +'  <shadervar type="texture" name="tex diffuse">%s</shadervar>'%(image.uname))
  '''
  else:
    print("WARNING: no diffuse texture defined for material", self.name)
  '''

  func(' '*depth +'  <shadervar type="vector4" name="specular">%f, %f, %f, 1</shadervar>'% tuple(self.specular_color))
  
  '''
  if 'shaders' in kwargs:
    for step, shader in kwargs['shaders'].items():
      func(' '*depth +' <shader type="%s">%s</shader>'%(step, shader))
  '''
  
  for step in ['depthwrite', 'ambient', 'diffuse']:
    if getattr(self, step+'_step') != 'DEFAULT':
      name = GetShaderName(getattr(self, step+'_step'))
      func(' '*depth +'  <shader type="%s">%s</shader>'%(step, name))
    
    
  func(' '*depth +'</material>')

bpy.types.Material.AsCS = MaterialAsCS


def MaterialGetShaders(self):
  shaders = {}
  for step in ['depthwrite', 'ambient', 'diffuse']:
    fil = getattr(self, step+'_step')
    if fil != 'DEFAULT' and fil != '*null':
      shaders[getattr(self, step+'_step')] = None
  return shaders
  
bpy.types.Material.GetShaders = MaterialGetShaders


def MaterialDependencies(self):
  dependencies = EmptyDependencies()
  for tex in self.textures:
    if tex.type=='IMAGE' and tex.image:
      dependencies['T'][tex.image.uname] = tex.image
  return dependencies
  
bpy.types.Material.GetDependencies = MaterialDependencies
