import bpy
from mathutils import *

from .util import *
from .transform import *
from .renderbuffer import *
from .morphtarget import *
from .material import *


class Hierarchy:
  OBJECTS = {}
  def __init__(self, anObject, anEmpty):
    self.object = anObject
    self.empty = anEmpty
    #uname = anEmpty.uname if anEmpty else anObject.uname
    uname = anObject.uname
    if uname in Hierarchy.OBJECTS:
      self.id = Hierarchy.OBJECTS[uname]
    else:
      self.id = len(Hierarchy.OBJECTS)
      Hierarchy.OBJECTS[uname] = self.id

  def _Get(obj):
    return obj, [Hierarchy._Get(child) for child in obj.children]

  def Get(self):
    return Hierarchy._Get(self.object)

  @property
  def uname(self):
    return "hier" + str(self.id)

  def AsCSRef(self, func, depth=0, dirName='factories/'):
    if self.object.type == 'ARMATURE' and self.object.data.bones:
      func(' '*depth +'<library>%s%s_rig</library>'%(dirName,self.object.name))
    func(' '*depth +'<library>%s%s</library>'%(dirName,self.object.name))

  def GetDependencies(self):
    dependencies = EmptyDependencies()

    subMeshess = {}
    objects = self.Get()
    def Gets(objs):
      for ob, children in objs:
        if ob.data not in subMeshess:
          subMeshess[ob.data] = ob.data.GetSubMeshes()
        Gets(children)

    Gets([objects])

    for subs in subMeshess.values():
      for sub in subs:
        MergeDependencies(dependencies, sub.GetDependencies())

    return dependencies


#======== Genmesh and Animesh ===========================================================

  def WriteCSLibHeader(self, func, animesh=False):
    """ Write an xml header for a CS library
        param animesh: indicates if the library decribes an animesh
    """
    func('<?xml version="1.0" encoding="UTF-8"?>')
    func("<library xmlns=\"http://crystalspace3d.org/xml/library\">")

    if animesh:
      func('  <plugins>')
      func('    <plugin name="animesh">crystalspace.mesh.loader.animesh</plugin>')
      func('    <plugin name="animeshfact">crystalspace.mesh.loader.factory.animesh</plugin>')
      func('    <plugin name="skeletonfact">crystalspace.mesh.loader.factory.skeleton</plugin>')
      func('  </plugins>')      


  def WriteCSAnimeshHeader(self, func, depth, skelRef=True):
    """ Write an xml header for a CS animesh factory
        param skelRef: indicates if a skeleton library should be
              specified for this mesh
    """
    # Reference skeleton as a CS library
    if skelRef and self.object.type == 'ARMATURE':
      func(" "*depth + "<library>factories/%s_rig</library>"%(self.object.name))

    # Animesh factory header
    func(" "*depth + "<meshfact name=\"%s\">"%(self.object.name))
    func(" "*depth + "  <plugin>crystalspace.mesh.loader.factory.animesh</plugin>")

    # Render priority and Z-mode properties
    if self.object.type == 'ARMATURE':
      for child in self.object.children:
        if child.parent_type!='BONE':
          if child.data.priority != 'object':
            func(' '*depth + '  <priority>%s</priority>'%(child.data.priority))
          if child.data.zbuf_mode != 'zuse':
            func(' '*depth + '  <%s/>'%(child.data.zbuf_mode))
    else:      
      if self.object.data.priority != 'object':
        func(' '*depth + '  <priority>%s</priority>'%(self.object.data.priority))
      if self.object.data.zbuf_mode != 'zuse':
        func(' '*depth + '  <%s/>'%(self.object.data.zbuf_mode))
      

  def AsCSLib(self, path='', animesh=False, **kwargs):
    """ Export this Blender mesh as a CS library file entitled '<mesh name>' in the 
        '<path>/factories' folder; it is exported as a general mesh or an animated 
        mesh depending of the 'animesh' parameter; if an armature is defined for an 
        animesh, it is exported as a library file entitled '<mesh name>_rig' in the
        'factories' folder
    """

    def Write(fi):
      def write(data):
        fi.write(data+'\n')
      return write

    # Export mesh
    fa = open(Join(path, 'factories/', self.object.name), 'w')
    self.WriteCSLibHeader(Write(fa), animesh)
    objectDeps = self.object.GetDependencies()
    use_imposter = not animesh and self.object.data.use_imposter
    ExportMaterials(Write(fa), 2, path, objectDeps, use_imposter)
    if animesh:
      self.WriteCSAnimeshHeader(Write(fa), 2)
    self.WriteCSMeshBuffers(Write(fa), 2, path, animesh, dontClose=False)
    fa.close()

    # Export skeleton and animations
    if self.object.type == 'ARMATURE' and self.object.data.bones:
      skel = self.object
      print('Exporting skeleton and animations:', Join(path, 'factories/', '%s_rig'%(skel.name)))
      fb = open(Join(path, 'factories/', '%s_rig'%(skel.name)), 'w')
      skel.data.AsCSSkelAnim(Write(fb), 2, skel, dontClose=False)
      fb.close()


  def WriteCSMeshBuffers(self, func, depth=0, path='', animesh=False, dontClose=False, **kwargs):
    """ Write an xml decription of a Blender object and its children of type 'MESH':
        - animesh=False: mesh and children without armature neither morph targets 
          are exported as imbricated Crystal Space general mesh factories
        - animesh=True: mesh and children with armature and/or morph targets 
          are exported as a Crystal Space animated mesh factory
    """

    # Build CS mapping buffers and submeshes for this object
    print("Building CS mapping buffers...")
    meshData = []
    subMeshess = []
    mappingBuffers = []
    mappingVertices = []
    scales = []
    objects = self.Get()

    def Gets(objs, indexV):
      total = 0
      for ob, children in objs:
        numCSVertices = 0
        if ob.type == 'MESH':
          # Socket objects must be exported as general meshes
          if animesh and ob.parent_type=='BONE':
            continue
          indexObject = find(lambda obCpy: obCpy.data == ob.data, meshData)
          if indexObject == None:
            # Get object's scale
            scale = ob.GetScale()
            scales.append(scale)
            # Get a deep copy of the object
            if ((animesh and ob.parent and ob.parent.type == 'ARMATURE') or \
                (not animesh and ob.parent and ob.parent.type == 'MESH'))   \
               and ob.parent_type != 'BONE':
              # If the mesh is child of another object (i.e. an armature in case of 
              # animesh export and a mesh in case of genmesh export), transform the 
              # copied object to its world position
              obCpy = ob.GetTransformedCopy(ob.relative_matrix)
            else:
              obCpy = ob.GetTransformedCopy()
            meshData.append(obCpy)
            # Generate mapping buffers
            mapVert, mapBuf = ob.data.GetCSMappingBuffer()
            numCSVertices = len(mapVert)
            # Generate submeshes
            subMeshess.append(ob.data.GetSubMeshes(ob.name,mapBuf,indexV))
            mappingBuffers.append(mapBuf)
            mappingVertices.append(mapVert)
            print('number of CS vertices for mesh "%s" = %s'%(ob.name,numCSVertices))
            if animesh:
              indexV += numCSVertices
        total += numCSVertices + Gets(children, indexV)
      return total

    totalVertices = Gets([objects], 0)
    print('--> Total number of CS vertices = %s'%(totalVertices))

    # Export meshes as imbricated CS general mesh factories
    import copy
    if not animesh:
      def Export(objs, d):
        for ob, children in objs:
          indexObject = find(lambda obCpy: obCpy.name[:-4] == ob.name[:len(obCpy.name[:-4])], meshData)
          lib = "from library '%s'"%(bpy.path.abspath(ob.library.filepath)) if ob.library else ''
          print('EXPORT factory "%s" %s'%(ob.name, lib))
          args = copy.deepcopy(kwargs)
          args['meshData'] = [meshData[indexObject]]
          args['subMeshes'] = subMeshess[indexObject]
          args['mappingBuffers'] = [mappingBuffers[indexObject]]
          args['mappingVertices'] = [mappingVertices[indexObject]]
          args['scales'] = [scales[indexObject]]
          args['dontClose'] = True
          ob.AsCSGenmeshLib(func, d, **args)
          Export(children, d+2)
          func(" "*d + "</meshfact>")

      Export([objects], depth)

    # Export meshes as a CS animated mesh factory
    else:  
      args = copy.deepcopy(kwargs)
      args['meshData'] = meshData
      args['subMeshess'] = subMeshess
      args['mappingBuffers'] = mappingBuffers
      args['mappingVertices'] = mappingVertices
      args['scales'] = scales
      self.AsCSAnimeshLib(func, depth, totalVertices, dontClose, **args)

    if not dontClose:
      func("</library>")

    # Delete working copies of the exported objects
    for obCpy in meshData:
      bpy.data.objects.remove(obCpy)


#======== Animesh ==============================================================

  def AsCSAnimeshLib(self, func, depth=0, totalVertices=0, dontClose=False, **kwargs):
    """ Write an xml description of this mesh, including children meshes,
        as a CS animated mesh factory (i.e. material, render buffers, submeshes, 
        bone influences, morph targets, sockets)
    """

    # Recover submeshes from kwargs
    subMeshess = []
    if 'subMeshess' in kwargs:
      subMeshess = kwargs['subMeshess']

    # Export the animesh factory
    func(" "*depth + "  <params>")

    # Take the first found material as default object material
    if self.object.type == 'ARMATURE':
      # Armature object
      foundMaterial = False
      for child in self.object.children:
        if child.type == 'MESH' and len(child.data.uv_textures) != 0:
          if child.data.GetMaterial(0):
            mat = child.data.GetMaterial(0).uname
            func(" "*depth + "    <material>%s</material>"%(str(mat)))
            foundMaterial = True
            break
      if not foundMaterial:
        func(" "*depth + "    <material>None</material>")
        print('ERROR: armature object "%s" has no child with texture coordinates'%(self.object.name))
    else:
      # Mesh object
      if len(self.object.data.uv_textures) != 0 and self.object.data.GetMaterial(0):
        mat = self.object.data.GetMaterial(0).uname
        func(" "*depth + "    <material>%s</material>"%(str(mat)))
      else:
        func(" "*depth + "    <material>None</material>")
        print('ERROR: mesh object "%s" has no texture coordinates'%(self.object.name))

    # Export object's render buffers
    print('EXPORT factory "%s"' % (self.object.name))
    for buf in GetRenderBuffers(**kwargs):
      buf.AsCS(func, depth+4, True)

    # Export bone influences
    if self.object.type == 'ARMATURE':
      # Specify skeleton name
      func(' '*depth + '    <skeleton>%s_rig</skeleton>'%(self.object.name))

      func(' '*depth + '    <boneinfluences>')
      for influences in self.object.data.GetBoneInfluences(**kwargs):
        func(' '*depth + '      <bi bone="%s" weight="%.4f"/>'%(influences[0],influences[1]))
      func(' '*depth + '    </boneinfluences>')

    # Export object's submeshes
    for submeshes in subMeshess:
      for sub in submeshes:
        sub.AsCS(func, depth+4, True)

    # Export morph targets
    for mt in GetMorphTargets(totalVertices, **kwargs):
      mt.AsCS(func, depth+4)

    # Find sockets
    if self.object.type == 'ARMATURE':
      socketList = {}
      for ob in bpy.data.objects:
        if ob.parent==self.object and ob.parent_type=='BONE' and ob.parent_bone:
          if ob.parent_bone not in socketList.keys():
            socketList[ob.parent_bone] = []
          socketList[ob.parent_bone].append(ob)

      # Export sockets
      bones = self.object.data.bones
      for boneName in socketList:
        for socketObject in socketList[boneName]:
          bone = bones[boneName]
          bone.AsCSSocket(func, depth+4, socketObject, self.object)

    func(' '*depth + '    <tangents automatic="true"/>')
    func(" "*depth + "  </params>")

    if not dontClose:
      func(" "*depth + "</meshfact>")


#======== Genmesh ==============================================================

def AsCSGenmeshLib(self, func, depth=0, **kwargs):
  """ Write an xml description of this mesh as a CS general mesh factory
      (i.e. header, material, render buffers and submeshes)
  """

  # Write genmesh header
  func(' '*depth + '<meshfact name=\"%s\">'%(self.name))
  func(' '*depth + '  <plugin>crystalspace.mesh.loader.factory.genmesh</plugin>')
  if self.data.use_imposter:
    func(' '*depth + '  <imposter range="100.0" tolerance="0.4" camera_tolerance="0.4" shader="lighting_imposter"/>')
  if self.data.priority != 'object':
    func(' '*depth + '  <priority>%s</priority>'%(self.data.priority))
  if self.data.zbuf_mode != 'zuse':
    func(' '*depth + '  <%s/>'%(self.data.zbuf_mode))
  func(' '*depth + '  <params>')

  # Recover submeshes from kwargs
  if 'subMeshes' in kwargs:
    subMeshes = kwargs['subMeshes']

  # Write mesh's material
  def SubmeshesLackMaterial(subMeshes):
    for sub in subMeshes:
      if not sub.material:
        return True
    return False

  if SubmeshesLackMaterial(subMeshes):
    #This mesh has a submesh without a material
    if self.data.GetMaterial(0):
      mat = self.data.GetMaterial(0).uname
    else:
      mat = None
    print('WARNING: Factory "%s" has no material'%(self.name))
    func(' '*depth + '    <material>%s</material>'%(str(mat)))

  # Export mesh's render buffers
  for buf in GetRenderBuffers(**kwargs):
    buf.AsCS(func, depth+4, False)

  # Export mesh's submeshes
  for sub in subMeshes:
    sub.AsCS(func, depth+4, False)

  func(' '*depth + '  </params>')

  # Don't close 'meshfact' flag if another mesh object is defined as 
  # an imbricated genmesh factory of this factory
  if not ('dontClose' in kwargs and kwargs['dontClose']):
    func(' '*depth + '</meshfact>')

bpy.types.Object.AsCSGenmeshLib = AsCSGenmeshLib


#======== Object ====================================================================

def ObjectAsCS(self, func, depth=0, **kwargs):
  """ Write a reference to this object (as part of a sector in the 'world' file); 
      only mesh, armature, group and lamp objects are described
  """
  name = self.uname
  if 'name' in kwargs:
    name = kwargs['name']+':'+name

  if self.type == 'MESH':
    func(' '*depth +'<meshref name="%s_object">'%(self.name))
    func(' '*depth +'  <factory>%s</factory>'%(self.name))
    matrix = self.relative_matrix
    if 'transform' in kwargs:
      matrix =  matrix * kwargs['transform']
    MatrixAsCS(matrix, func, depth+2)
    func(' '*depth +'</meshref>')

  elif self.type == 'ARMATURE':
    func(' '*depth +'<meshref name="%s_object">'%(name))
    func(' '*depth +'  <factory>%s</factory>'%(name))
    matrix = self.relative_matrix
    if 'transform' in kwargs:
      matrix =  matrix * kwargs['transform']
    MatrixAsCS(matrix, func, depth+2)
    func(' '*depth +'</meshref>')

  elif self.type == 'LAMP':
    func(' '*depth +'<light name="%s">'%(name))
    # Flip Y and Z axis.
    func(' '*depth +'  <center x="%f" z="%f" y="%f" />'% tuple(self.relative_matrix.to_translation()))
    func(' '*depth +'  <color red="%f" green="%f" blue="%f" />'% tuple(self.data.color))
    func(' '*depth +'  <radius brightness="%f">%f</radius>'%(self.data.energy, self.data.distance))
    func(' '*depth +'  <attenuation>linear</attenuation>')
    func(' '*depth +'</light>')

  elif self.type == 'EMPTY':
    if self.dupli_type=='GROUP' and self.dupli_group:
      if self.dupli_group.doMerge:
        self.dupli_group.AsCS(func, depth, transform=self.relative_matrix)
      else:
        for ob in self.dupli_group.objects:
          if not ob.parent:
            ob.AsCS(func, depth, transform=self.relative_matrix, name=self.uname)

    #Handle children: translate to top level.
    for obj in self.children:
      obj.AsCS(func, depth, transform=self.relative_matrix, name=self.uname)

  elif self.type != 'CAMERA':
    print('\nWARNING: Object "%s" of type "%s" is not supported!'%(self.name, self.type))

bpy.types.Object.AsCS = ObjectAsCS


def ObjectDependencies(self, empty=None):
  """ Get the dependencies of this object and its children, i.e. textures, 
      materials, armatures, meshes and groups associated with it
  """
  dependencies = EmptyDependencies()  

  if self.type == 'ARMATURE':
    # Object with skeleton ==> 'A' type (animesh)
    if self.children:
      hier = Hierarchy(self, empty)
      print('ObjectDependencies: ', hier.uname, '-', self.name)
      dependencies['A'][hier.uname] = hier
      for child in self.children:
        if child.parent_type != 'BONE':
          MergeDependencies(dependencies, child.GetDependencies())

  elif self.type == 'MESH':
    def IsAnimeshSubmesh(ob):
      ''' Test if ob is child of an armature
          (and not linked by a socket to a bone)
      '''
      if ob.parent_type=='BONE' and ob.parent_bone:
        return False
      if ob.parent:
        return IsAnimeshSubmesh(ob.parent)
      return ob.type=='ARMATURE'

    # Mesh without any vertex or face and submesh of animesh
    # will not be exported as individual mesh factory
    if not IsAnimeshSubmesh(self) \
          and len(self.data.vertices)!=0 and len(self.data.faces)!=0:
      if self.data.shape_keys:
        # Mesh with morph targets ==> 'A' type (animesh)
        hier = Hierarchy(self, empty)
        print('ObjectDependencies: ', hier.uname, '-', self.name)
        dependencies['A'][hier.uname] = hier      
      else:
        # Mesh without skeleton neither morph target 
        # or child of a bone (attached by socket) ==> 'F' type (genmesh)
        hier = Hierarchy(self, empty)
        print('ObjectDependencies: ', hier.uname, '-', self.name)
        dependencies['F'][hier.uname] = hier

    # Material of the mesh ==> 'M' type
    # Associated textures  ==> 'T' type
    MergeDependencies(dependencies, self.GetMaterialDependencies())

  elif self.type == 'EMPTY':
    if self.dupli_type=='GROUP' and self.dupli_group:
      # Group of objects ==> 'G' type
      if self.dupli_group.doMerge:
        dependencies['G'][self.dupli_group.uname] = self.dupli_group
        MergeDependencies(dependencies, self.dupli_group.GetDependencies())
      else:
        for ob in [ob for ob in self.dupli_group.objects if not ob.parent]:
          MergeDependencies(dependencies, ob.GetDependencies(self))

  return dependencies

bpy.types.Object.GetDependencies = ObjectDependencies


def GetMaterialDeps(self):
  """ Get materials and textures associated with this object
  """
  dependencies = EmptyDependencies()
  foundDiffuseTexture = False
  for mat in self.materials:
    # Material of the mesh ==> 'M' type
    # and associated textures ==> 'T' type
    dependencies['M'][mat.uname] = mat
    MergeDependencies(dependencies, mat.GetDependencies())
    foundDiffuseTexture = foundDiffuseTexture or mat.HasDiffuseTexture() 
  # Search for a diffuse texture if none is defined among the materials
  if not foundDiffuseTexture:
    if self.data and self.data.uv_textures and self.data.uv_textures.active:
      # UV texture of the mesh ==> 'T' type
      for index, facedata in enumerate(self.data.uv_textures.active.data):
        if facedata.image and facedata.image.uname not in dependencies['T'].keys():
          dependencies['T'][facedata.image.uname] = facedata.image
          self.data.uv_texture = facedata.image.uname
          material = self.data.GetMaterial(self.data.faces[index].material_index)
          if material:
            material.uv_texture = facedata.image.uname
  return dependencies

bpy.types.Object.GetMaterialDeps = GetMaterialDeps


def GetMaterialDependencies(self):
  """ Get materials and textures associated with this object
      and its children
  """
  dependencies = self.GetMaterialDeps()
  for child in self.children:
    MergeDependencies(dependencies, child.GetMaterialDeps())
  return dependencies

bpy.types.Object.GetMaterialDependencies = GetMaterialDependencies


def HasImposter(self):
  """ Indicates if this object uses an imposter
  """
  if self.type == 'MESH':
    if self.data and self.data.use_imposter:
      return True
  elif self.type == 'EMPTY':
    if self.dupli_type=='GROUP' and self.dupli_group:
      if self.dupli_group.HasImposter():
        return True
  return False

bpy.types.Object.HasImposter = HasImposter


def GetScale(self):
  """ Get object scale
  """
  if self.parent and self.parent.type == 'ARMATURE' and self.parent_type != 'BONE':
    scale = self.parent.scale
  else:
    scale = self.scale

  return scale

bpy.types.Object.GetScale = GetScale


def GetTransformedCopy(self, matrix = None):
  """ Get a deep copy of this object, transformed by matrix
  """
  # Make a copy of the current object
  obCpy = self.copy()
  # Make a deep copy of the object data block
  obCpy.data = self.data.copy()
  # Transform the copied object
  if matrix:
    obCpy.data.transform(matrix)

  return obCpy

bpy.types.Object.GetTransformedCopy = GetTransformedCopy
