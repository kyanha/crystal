import bpy

from .util import *
from io_scene_cs.utilities import GetShaderSetName
from io_scene_cs.utilities import GetExportPath

# Property defining an UV texture's name for a material ('None' if not defined)
bpy.types.Material.uv_texture = bpy.props.StringProperty(default='None')


def GetName(name, dic):
  if name not in dic:
    dic[name] = 1
    return name
  dic[name] += 1
  return name + ' ' + str(dic[name])


def HasDiffuseTexture(self):
  """ Test if this material defines a diffuse texture
  """
  for slot in self.texture_slots:
    if slot and slot.texture and slot.texture.type =='IMAGE':
      if slot.use_map_color_diffuse:
        return True
  return False

bpy.types.Material.HasDiffuseTexture = HasDiffuseTexture


MAP = {'use_map_color_diffuse': 'tex diffuse',
       'use_map_normal': 'tex normal',
       'use_map_displacement': 'tex height',
       'use_map_specular': 'tex specular',}

def MaterialAsCS(self, func, depth=0, **kwargs):
  """ Write an xml decription of this material
  """
  if not self.b2cs.csMatRef:
    func(' '*depth +'<material name="%s">'%(self.uname))
    dic = {}
    for slot in self.texture_slots:
      if slot and slot.texture and slot.texture.type =='IMAGE':
        for type, name in MAP.items():
          if getattr(slot, type, False):
            func(' '*depth +'  <shadervar type="texture" name="%s">%s</shadervar>'%(GetName(name, dic), slot.texture.image.uname))

    func(' '*depth +'  <shadervar type="vector4" name="specular">%f, %f, %f, 1</shadervar>'% tuple(self.specular_color))
  
    haswater = False

    if self.b2cs.shaderset != 'DEFAULT':
      name = GetShaderSetName(self.b2cs.shaderset)
      if name == 'reflect_water_plane':
        haswater = True
      func(' '*depth +'  <shaderset>%s</shaderset>'%(name)) 

    '''
    for step in ['depthwrite', 'ambient', 'diffuse']:
      if getattr(self, step+'_step') != 'DEFAULT':
        name = GetShaderName(getattr(self, step+'_step'))
        if name == 'water_plane':
          haswater = True
          if step == 'ambient':
            step = 'base'     # Hacky@@@
        func(' '*depth +'  <shader type="%s">%s</shader>'%(step, name))   
    '''
    
    if haswater:
      func(' '*depth +'  <shadervar type="vector4" name="water fog color">%s</shadervar>'%(self.water.water_fog_color,))
      func(' '*depth +'  <shadervar type="vector4" name="water perturb scale">%s</shadervar>'%(self.water.water_perturb_scale,))
      func(' '*depth +'  <shadervar type="float" name="water fog density">%s</shadervar>'%(self.water.water_fog_density,))

    func(' '*depth +'</material>')

bpy.types.Material.AsCS = MaterialAsCS

'''
def MaterialGetShaders(self):
  shaders = {}
  for step in ['depthwrite', 'ambient', 'diffuse']:
    fil = getattr(self, step+'_step')
    if fil != 'DEFAULT' and fil != '*null':
      shaders[getattr(self, step+'_step')] = None
  return shaders
  
bpy.types.Material.GetShaders = MaterialGetShaders
'''

def MaterialDependencies(self):
  dependencies = EmptyDependencies()

  if not self.b2cs.csMatRef:    # skip materials replaced by references to CS materials
    for tex in self.textures:
      if tex.type=='IMAGE' and tex.image:
        dependencies['T'][tex.image.uname] = tex.image
  return dependencies
  
bpy.types.Material.GetDependencies = MaterialDependencies


#===== static method ExportMaterials ==============================

def ExportMaterials(func, depth, dependencies, use_imposter):
  """ Write an xml description of the materials/textures/shaders
      param dependencies: list of textures ('T' key) and 
            materials ('M' key)
      use_imposter: indicates if an imposter shader is defined
  """
  # Export textures 
  if len(dependencies['T'].keys()) > 0:
    func(' '*depth +"<textures>")
    for name, tex in dependencies['T'].items():
      tex.AsCS(func, depth+2)
      tex.save_export(GetExportPath ())
    func(' '*depth +"</textures>")

  # Export materials
  shaders = {}
  if len(dependencies['M'].keys()) > 0 or len(dependencies['TM'].keys()) > 0:
    func(' '*depth +"<materials>")
    for name, mat in dependencies['M'].items():
      mat.AsCS(func, depth+2)
      #shaders.update(mat.GetShaders())
    # Export missing materials
    for name, image in dependencies['TM'].items():
      func(' '*depth +'  <material name="%s">'%(name))      
      func(' '*depth +'    <shadervar type="texture" name="tex diffuse">%s</shadervar>'%(name))
      func(' '*depth +'  </material>')      
    func(' '*depth +"</materials>")

  # Export shaders
  if use_imposter:
    func(' '*depth +"<shaders>")
    func(' '*depth +"  <shader><file>/shader/lighting/lighting_imposter.xml</file></shader>")
    func(' '*depth +"</shaders>")
  else:
    if len(shaders) > 0:
      func(' '*depth +"<shaders>")
      for shader in shaders:
        func(' '*depth +"  <shader><file>"+shader+"</file></shader>")
      func(' '*depth +"</shaders>")
