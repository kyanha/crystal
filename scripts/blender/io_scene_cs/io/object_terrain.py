import bpy
import mathutils
from .util import *
from .transform import *

import math

from io_scene_cs.xml import XML, NoNewLineWriter

from io_scene_cs.utilities import GetExportPath


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
    print('>>>WriteCSTerrain '+self.uname)
    heightmap = exportHeightMap(self)
    print('@>>>WriteCSTerrain '+self.uname)

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


#==================================================================
def exportHeightMap(object):
  import time
  mesh = object.data
  init_time = time.time()
  # find bounding box
  bbox = object.bound_box
  min = list(bbox[0])
  max = list(bbox[0])
  for vert in bbox:
    for a in range(3):
      if vert[a] < min[a]:
        min[a] = vert[a]
      elif vert[a] > max[a]:
        max[a] = vert[a]
  size = object.dimensions
  location = object.location
  for a in range(3):
    min[a] = (min[a]-location[a])/size[a]
    max[a] = (max[a]-location[a])/size[a]
    
  resolution = 512
  
  mesh.update(calc_edges=False, calc_tessface=True)

  pixels = [0]*((resolution+1) * (resolution+1) * 4)
  for face in mesh.tessfaces:
    distributeFace(resolution, pixels, mesh,face,min,max)
    #break
  
  path = 'textures/'+object.uname+'-heighmap.png'
  print(' '+path)
  
  image_object = bpy.data.images.new(name=object.uname+'-hm', width=resolution+1, height=resolution+1, alpha=False, float_buffer=False)
  image_object.pixels = pixels
  image_object.update()  
  image_object.filepath_raw = GetExportPath()+path
  image_object.save()
  bpy.data.images.remove(image_object)
  del image_object
  
  return path

#==================================================================
def getTriBounds(tri):
  min = [tri[0].co[0],tri[0].co[1],tri[0].co[2]]
  max = list(min)
  for j in range(1,3):
    vertex = tri[j] # skip the first vertex
    for i in range(0,3):
      if vertex.co[i] < min[i]:
        min[i] = vertex.co[i]
      elif vertex.co[i] > max[i]:
        max[i] = vertex.co[i]
  return (min,max)
#==================================================================
def cross_product2(v, w):
	return (v[0]*w[1]) - (v[1]*w[0])
  
def pointInTri(p,tri,min,max):
  a = tri[0].co
  b = tri[1].co
  c = tri[2].co
  s1 = cross_product2(c-a,(p[0]-a[0],p[1]-a[1]))
  s2 = cross_product2(a-b,(p[0]-b[0],p[1]-b[1]))
  if (s1*s2 < 0): # fast skip if its already outside
    return False
  s3 = cross_product2(b-c,(p[0]-c[0],p[1]-c[1]))
  if (s2*s3 < 0):
    return False
  return True
  
def getPointTriIntersection(point,tri):
  ab = (tri[1].co - tri[0].co)
  ac = (tri[2].co - tri[0].co)
  
  #PLANE = CrossVecs(ab,ac)
  ab = mathutils.Vector(ab)
  ac = mathutils.Vector(ac)
  PLANE = ab.cross(ac)
  
  #D = -DotVecs(PLANE,tri[0].co)
  D = - PLANE.dot(tri[0].co)
  z = -(point[0]*PLANE[0]+(point[1]*PLANE[1]+D))/PLANE[2]
  return z
  
def makeTri(mesh,face,skip):
  tri = ()
  for i in range(0,4):
    if (not i == skip):
      tri = tri + (mesh.vertices[face.vertices[i]],)
  return tri
#==================================================================

def distributeTri(terrain_resolution, new_pixels, tri, dx, dy,min,max,trinormals):
  def hputpixel(coords, color):
    x, y = coords
    print('    %f - %f '%(x, y))
    offset = (x + (y * (terrain_resolution+1))) * 4
    new_pixels[offset] = color[0]
    new_pixels[offset+1] = color[1]
    new_pixels[offset+2] = color[2]
    new_pixels[offset+3] = 1
  
  box = getTriBounds(tri)
  print('min', min, dy)
  print('getTriBounds', box)
  box_x1 = ((box[0][0]-min[0])/dx)
  box_y1 = ((box[0][1]-min[1])/dy)
  box_x2 = ((box[1][0]-min[0])/dx) + 1
  box_y2 = ((box[1][1]-min[1])/dy) + 1
  print('box_y1', box_y1)
  boundsz = max[2]-min[2]
  # check to prevent divide by 0
  if boundsz == 0:
    boundsz = 1.0/255.0
  if (box_x1 < 0):
    box_x1 = 0
  if (box_y1 < 0):
    box_y1 = 0
  if (box_x2 > terrain_resolution+1):
    box_x2 = terrain_resolution+1
  if (box_y2 > terrain_resolution+1):
    box_y2 = terrain_resolution+1
  f=122.5
  fmax = 16777215.0
  tfactor = fmax/boundsz
  print ('range %f - %f '%(box_y1, box_y2))
  for y in range(int(box_y1),int(box_y2)):
    for x in range(int(box_x1),int(box_x2)):
      point = (x*dx + min[0], y*dy + min[1])
      if pointInTri(point,tri,min,max):
        val_z = getPointTriIntersection(point,tri)
        h = (val_z-min[2])*(tfactor)
        r = int(h)>>16
        postr = h-(r*65536.0)
        g = int(postr)>>8
        b = int(postr-(g*256.0))
        hputpixel((x,terrain_resolution-y),(r,g,b))
#==================================================================
def distributeFace(terrain_resolution, new_pixels, mesh,face,min,max):
  spanx = max[0] - min[0]
  spany = max[1] - min[1]
  stepx = spanx / terrain_resolution
  stepy = spany / terrain_resolution
  
  print('%f - %f    %f - %f'%(spanx, spany, stepx, stepy))
  
  if (len(face.vertices) ==4):
    tri1 = makeTri(mesh, face,3)
    distributeTri(terrain_resolution, new_pixels, tri1,stepx,stepy,min,max,face.normal)
    tri2 = makeTri(mesh, face,1)
    distributeTri(terrain_resolution, new_pixels, tri2,stepx,stepy,min,max,face.normal)
  else:
    tri1 = makeTri(mesh, face,3)
    distributeTri(terrain_resolution, new_pixels, tri1,stepx,stepy,min,max,face.normal)
