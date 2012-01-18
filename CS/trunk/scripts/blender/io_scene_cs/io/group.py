import bpy
import mathutils

from .util import *
from .transform import *
from .submesh import *

from io_scene_cs.utilities import B2CS


def GroupAsCS(self, func, depth=0, **kwargs):
  func(' '*depth +'<meshobj name="%s">'%(self.name))
  func(' '*depth +'  <plugin>crystalspace.mesh.loader.genmesh</plugin>')
  func(' '*depth +'  <params>')
  func(' '*depth +'    <factory>%s</factory>'%(self.uname))
  func(' '*depth +'  </params>')
  #func(' '*depth +'  <move>')
  # Flip Y and Z axis.
  #func(' '*depth +'    <v x="%f" z="%f" y="%f" />'% tuple(self.relative_location))
  #func(' '*depth +'  </move>')
  
  if 'transform' in kwargs:
    MatrixAsCS(kwargs['transform'], func, depth+2)
  else:
    func(' '*depth +'  <move><v x="0" z="0" y="0" /></move>')
  
  func(' '*depth +'</meshobj>')
  
bpy.types.Group.AsCS = GroupAsCS


def GroupallObjects(self, matrix=None):
  objects = []
  for ob in self.objects:
    if ob.type == 'EMPTY':
      if ob.dupli_type=='GROUP' and ob.dupli_group:
        objects.extend(ob.dupli_group.allObjects(ob.relative_matrix))
    else:
      objects.append((matrix, ob))
  return objects
  
bpy.types.Group.allObjects = GroupallObjects


def GroupAsCSFactory(self, func, depth=0):
  """ Export a group of Blender objects as a Crystal Space general mesh:
      objects are exported as instances or submeshes of the general mesh
  """

  func('<?xml version="1.0" encoding="UTF-8"?>')
  func("<library xmlns=\"http://www.crystalspace3d.org/docs/online/manual\">")
  
  # Get mapping buffers and submeshes for the objects composing the group
  meshData = []
  subMeshess = []
  mappingBuffers = []
  mappingVertices = []
  indexV = 0

  for m, ob in self.allObjects():
    numCSVertices = 0
    indexObject = find(lambda obCpy: obCpy.name[:-4] == ob.name[:len(obCpy.name[:-4])], meshData)
    if indexObject == None:
      # Export group objects as instances of the general mesh
      if self.groupedInstances:
        # Get a deep copy of the object
        obCpy = ob.GetTransformedCopy()
      # Export group objects as submeshes of the general mesh
      else:
        # Get the world transformation of this object
        matrix = ob.matrix_world
        if m:
          matrix = matrix * m
        # Get a deep copy of this object, transformed to its world position
        obCpy = ob.GetTransformedCopy(matrix)
      meshData.append(obCpy)
      # Generate mapping buffers
      mapVert, mapBuf = ob.data.GetCSMappingBuffer()
      numCSVertices = len(mapVert)
      # Generate submeshes
      subMeshess.append(ob.data.GetSubMeshes(ob.name,mapBuf,indexV))
      mappingBuffers.append(mapBuf)
      mappingVertices.append(mapVert)
      print('number of CS vertices for mesh "%s" = %s'%(ob.data.name,numCSVertices))
      indexV += numCSVertices

  # Export textures
  func("  <textures>")
  textures = {}
  for submeshes in subMeshess:
    for sub in submeshes:
      for name, tex in sub.GetDependencies()['T'].items():
        textures[name] = tex      
  for name, tex in textures.items():
    tex.AsCS(func, depth+2, path='textures/')
    tex.save_export(B2CS.properties.exportPath)
  func("  </textures>")
  
  # Export shaders
  func("  <shaders>")
  #func("    <shader><file>/shader/lighting/lighting_imposter.xml</file></shader>")
  func("    <shader><file>/shader/lighting/lighting_default_instance.xml</file></shader>")
  func("    <shader><file>/shader/lighting/lighting_default_instance_binalpha.xml</file></shader>")
  func("    <shader><file>/shader/foliage/tree_leaves.xml</file></shader>")
  func("  </shaders>")
  
  shaders = {}
  if self.groupedInstances:
    shaders = {'depthwrite':'*null', 'base':'lighting_default_instance', 'diffuse':'lighting_default_instance'}
  
  # Export materials
  mat = True
  materials = {}
  func("  <materials>")
  for submeshes in subMeshess:
    for sub in submeshes:
      if sub.material and sub.material.name not in materials:
        materials[sub.material.name] = sub.material
        sub.material.AsCS(func, depth+2, shaders=shaders)
      else:
        mat = None
  func("  </materials>")
    
  # Export the group of objects as a general mesh factory
  func("  <meshfact name=\"%s\">"%(self.uname))
  if self.groupedInstances:
    # EXPORT OBJECTS AS INSTANCES OF THE GENERAL MESH
    print("The objects composing group '%s' are exported as instances of a general mesh"%(self.uname))
    func('    <instances>')
    func('      <meshfact name=\"%s-instance\">'%(self.uname))
    #func('     <imposter range="10.0" tolerance="45.0" camera_tolerance="45.0" shader="lighting_imposter"/>')
    func('        <plugin>crystalspace.mesh.loader.factory.genmesh</plugin>')
    func('        <params>')

    # Export first object of the group as basic general mesh
    m, ob in self.allObjects()[0]

    # Export render buffers
    indexObject = find(lambda obCpy: obCpy.name[:-4] == ob.name[:len(obCpy.name[:-4])], meshData)
    args = {}
    args['meshData'] = [meshData[indexObject]]
    args['mappingBuffers'] = [mappingBuffers[indexObject]]
    args['mappingVertices'] = [mappingVertices[indexObject]]
    for buf in GetRenderBuffers(**args):
      buf.AsCS(func, depth+8)   

    # Export submeshes
    for sub in ob.data.GetSubMeshes(ob.name,mappingBuffers[indexObject]):
      sub.AsCS(func, depth+8)

    func("        </params>")    
    func("      </meshfact>")

    # Export objects of the group as instances of the basic general mesh
    min_corner = mathutils.Vector((0.0, 0.0, 0.0))
    max_corner = mathutils.Vector((0.0, 0.0, 0.0))
    for m, ob in self.allObjects():
      func("      <instance>")
      matrix = ob.matrix_world
      if m: 
        matrix = matrix * m
      MatrixAsCS(matrix, func, depth+4, True)
      func("      </instance>")
      
      for corner in ob.bound_box:
        corner =  matrix * mathutils.Vector(corner)
        for i in range(3):
          if corner[i] < min_corner[i]:
            min_corner[i] = corner[i]
          elif corner[i] > max_corner[i]:
            max_corner[i] = corner[i]      
      
    func("    </instances>")

    # Export group's bounding box
    func('    <bbox>')
    func('      <v x="%s" y="%s" z="%s" />'%(min_corner[0], min_corner[2], min_corner[1]))
    func('      <v x="%s" y="%s" z="%s" />'%(max_corner[0], max_corner[2], max_corner[1]))
    func('    </bbox>')

    func("  </meshfact>")

  else:
    # EXPORT OBJECTS AS SUBMESHES OF THE GENERAL MESH
    print("The objects composing group '%s' are exported as submeshes of a general mesh"%(self.uname))
    func("    <plugin>crystalspace.mesh.loader.factory.genmesh</plugin>")
    func("    <params>")
    
    # There is a submesh without a material
    if not mat:
      for m, ob in self.allObjects():
        if ob.data.GetMaterial(0):
          mat = ob.data.GetMaterial(0).uname
          break        
      if not mat:
        print('WARNING: Factory "%s" has no material!'%(self.name))
      func("      <material>%s</material>"%(str(mat)))   
  
    # Export the render buffers of all objects composing the group
    args = {}
    args['meshData'] = meshData
    args['mappingBuffers'] = mappingBuffers
    args['mappingVertices'] = mappingVertices
    for buf in GetRenderBuffers(**args):
      buf.AsCS(func, depth+4)
    
    # Export the submeshes composing the group's objects
    for submeshes in subMeshess:
      for sub in submeshes:
        sub.AsCS(func, 6)
  
    func("    </params>")    
    func("  </meshfact>")
    
  func("</library>")
  
  # Delete working copies of objects
  for obCpy in meshData:
    bpy.data.objects.remove(obCpy)

bpy.types.Group.AsCSFactory = GroupAsCSFactory


def GroupAsCSRef(self, func, depth=0, dirName='factories/'):
  func(' '*depth +'<library>%s%s</library>'%(dirName,self.uname))

bpy.types.Group.AsCSRef = GroupAsCSRef


def GroupDependencies(self):
  dependencies = EmptyDependencies()
  """
  for ob in self.objects:
    for mat in ob.materials:
      dependencies['M'][mat.uname] = mat
      MergeDependencies(dependencies, mat.GetDependencies())
  """
  return dependencies
  
bpy.types.Group.GetDependencies = GroupDependencies
