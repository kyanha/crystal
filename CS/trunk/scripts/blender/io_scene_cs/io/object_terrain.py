import bpy
from .util import *
from .transform import *

import math

from io_scene_cs.xml import XML, NoNewLineWriter


def grayScalePixel(pixel):
  newRed = pixel[0]*0.299
  newGreen = pixel[1]*0.587
  newBlue = pixel[2]* 0.114
  luminance = newRed+newGreen+newBlue
  return luminance


class TerrainAlpaMap(object):
  def __init__(self, material, i):
    self.material = material
    self.i = i
  @property  
  def uname(self):
    return self.material.uname+'_alphamap_'+str(self.i)
    
  def AsCS(self, func, depth=0, **kwargs):
    path = kwargs.get('path', 'textures/')
    xml = XML(NoNewLineWriter(func), depth)
    xml.texture({'name': self.uname})\
      .file(path+self.uname+'.png')
      
  def save_export(self, export_path):
    '''
    Merges the given stencilmaps into an CS-compatible RGBA-alphamap.
    In blender the first texture is the base texture onto which the other 
    textures are blended, so Red is always 1.
    '''
    stencils = []
    for i, tex in enumerate(self.material.textures):
      slot = self.material.texture_slots[i]
      if slot.use_stencil:
        stencils.append((slot, tex))
        
    if self.i > 1:
      offset = self.i*4
      stencils = stencils[offset:offset+4]
    else:# If this is the first alphamap, only allow up to 3 colors
      stencils = stencils[:3]
      
    #TODO: this expects all stencil textures to be the same size!   
    sizes = list(map(lambda x: list(x[1].image.size), stencils))
    print('TerrainAlpaMap: sizes', sizes)
    size = sizes[0]
    for s in sizes:
      if size != s:
        raise Exception('stencilmaps not of same size')
      
    #Get all pixels
    stencils = [list(tex.image.pixels) for s, tex in stencils]
    
    # Make new pixel array and start copying the data
    new_pixels = [0]*(size[0] * size[1] * 4)
    for i in range(size[0] * size[1]):  
        offset = i * 4
        for i, pixels in enumerate(stencils):
          if self.i > 1:
            new_pixels[offset+i] = grayScalePixel(pixels[offset:offset+4])
          else: # If this is the first alphamap, start with red == 1
            new_pixels[offset] = 1.0
            new_pixels[offset+i+1] = grayScalePixel(pixels[offset:offset+4])
    
    
    image_object = bpy.data.images.new(name=self.uname, width=size[0], height=size[1], alpha=True)
    image_object.pixels = new_pixels
    image_object.update()
    image_object.filepath_raw = export_path+'textures/'+self.uname+'.png'
    image_object.save()
    bpy.data.images.remove(image_object)
    del image_object


class TerrainMaterial(object):
  def __init__(self, material):
    self.material = material
  def AsCS(self, func, depth=0, **kwargs):
    xml = XML(NoNewLineWriter(func), depth)

    xml.material({'name': self.material.uname})\
      .shader({'type': 'base'}, 'terrain_multisplat', close=True)\
      .shadervar({'name': 'texture scale', 'type': 'vector2'}, '32,32', close=True)\
      
    nr = 1
    stencils = []
    for i, tex in enumerate(self.material.textures):
      slot = self.material.texture_slots[i]
      if not slot.use_stencil:
        xml.shadervar({'name': 'tex diffuse '+str(nr), 'type': 'texture'}, tex.image.uname, close=True)
        nr += 1
      else:
        stencils.append((slot, tex))
    
    nr = 1    
    for i in range( math.ceil((len(stencils)+1)/4) ):
      map = TerrainAlpaMap(self.material, i)
      xml.shadervar({'name': 'tex alpha map '+str(nr), 'type': 'texture'}, map.uname, close=True)
      nr += 1

    
  def GetDependencies(self):
    dependencies = EmptyDependencies()
    dependencies['S']['terrain_multisplat'] = '/shader/terrain/multisplat.xml'
    
    stencils = []
    for i, tex in enumerate(self.material.textures):
      slot = self.material.texture_slots[i]
      if not slot.use_stencil:
        dependencies['T'][tex.image.uname] = tex.image
      else:
        stencils.append((slot, tex))
    
    for i in range( math.ceil((len(stencils)+1)/4) ):
      map = TerrainAlpaMap(self.material, i)
      dependencies['T'][map.uname] = map
      
    return dependencies
    
  def __getattr__(self, name):
    return getattr(self.material, name)


def GetTerrainMaterialDependencies(self):
  dependencies = EmptyDependencies()
  if self.type == 'MESH':
    mat = self.materials[0] #Should only have one material, TODO handle 'oblivious' users
    tm = TerrainMaterial(mat)
    dependencies['M'][mat.uname] = tm
    MergeDependencies(dependencies, tm.GetDependencies())
  return dependencies
  
bpy.types.Object.GetTerrainMaterialDependencies = GetTerrainMaterialDependencies


def WriteCSTerrain (self, func, depth, path):
    xml = XML(NoNewLineWriter(func), depth)
    mat = self.materials[0] #Should only have one material, TODO handle 'oblivious' users
    
    #TODO: export the actual heightmap
    heightmap = '/lev/terrain/heightmap_257x257.png'

    xml.meshfact({'name': self.data.uname})\
        .plugin('crystalspace.mesh.loader.factory.terrain2').close()\
        .params()\
            .renderer('crystalspace.mesh.object.terrain2.bruteblockrenderer').close()\
            .collider('crystalspace.mesh.object.terrain2.collider').close()\
            .feeder('crystalspace.mesh.object.terrain2.simpledatafeeder', close=True)\
            .cells()\
                .celldefault()\
                    .gridsize({'width': 257, 'height': 257}, close=True)\
                    .size({'x': 256, 'y': 32, 'z': 256}, close=True)\
                .close()\
                \
                .cell()\
                    .name(0, close=True)\
                    .position({'x': -127, 'y': -127}, close=True)\
                    .basematerial(mat.uname, close=True)\
                    .feederproperties()\
                        .param(heightmap, {'name': 'heightmap source'}, close=True)\
                    .close()\
                .close()\
                \
            .close()\
        .close()\
    .close()
    
    func("</library>")

bpy.types.Object.WriteCSTerrain = WriteCSTerrain


def TerrainAsCS(self, func, depth, **kwargs):
  xml = XML(NoNewLineWriter(func), depth)
  
  name = self.uname
  if 'name' in kwargs:
    name = kwargs['name']+':'+name
    
  matrix = self.relative_matrix
  if 'transform' in kwargs:
    matrix =  kwargs['transform'] * matrix
    
  xml.meshobj({'name': name})\
    .plugin('crystalspace.mesh.loader.terrain2').close()\
    .params()\
      .factory(self.data.uname).close()\
    .close()
    
  MatrixAsCS(matrix, func, depth+2)
  
  xml.close()

bpy.types.Object.TerrainAsCS = TerrainAsCS
