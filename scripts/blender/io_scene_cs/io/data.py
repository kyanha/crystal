import bpy
from sys import float_info
from mathutils import *

from .util import *
from .transform import *
from .renderbuffer import *
from .submesh import *

from io_scene_cs.utilities import B2CS


def GetMaterial(self, index):
  if index < len(self.materials):
    return self.materials[index]
  return None

bpy.types.Mesh.GetMaterial = GetMaterial


def GetSubMeshesRaw(self, name, indexV, indexGroups, mappingBuffer = []):
  """ Compute the CS submeshes of this Blender mesh by remapping
      Blender vertices to CS vertices and triangulating the faces
  """
  firstIndex = indexV
  tface = self.uv_textures.active.data if self.uv_textures.active else None
  # For each face composing this mesh
  for index, face in enumerate(self.faces):
    im = tface[index].image if tface else None
    # Identify the submesh
    triplet = (name, self.GetMaterial(face.material_index), im)
    if triplet not in indexGroups:
      indexGroups[triplet] = []
    # Triangulate
    indices = [v for v in face.vertices]
    tt = [2,1,0] if len(indices)==3 else [2, 1, 0, 3, 2, 0]
    for i in tt:
      # Add vertices composing the triangle to submesh
      if len(mappingBuffer) == 0:
        indexGroups[triplet].append(face.vertices[i])
        indexV += 1
      else:
        for mapV in mappingBuffer[face.vertices[i]]:
          if mapV.face == index and mapV.vertex == i:
            indexGroups[triplet].append(mapV.csVertex + firstIndex)
            indexV += 1
            break

  return indexV, indexGroups

bpy.types.Mesh.GetSubMeshesRaw = GetSubMeshesRaw


def GetSubMeshes(self, name = '', mappingBuffer = [], numVertices = 0):
  """ Create the CS submeshes of this Blender mesh
  """
  indexV = numVertices
  indexGroups = {}
  indexV, indexGroups = self.GetSubMeshesRaw(name, indexV, indexGroups, mappingBuffer)
  return [SubMesh(group[0], group[1], group[2], indexGroups[group]) for group in indexGroups]

bpy.types.Mesh.GetSubMeshes = GetSubMeshes


def SubmeshesLackMaterial(subMeshes):
  for sub in subMeshes:
    if not sub.material:
      return True
  return False


def MeshAsCSRef(self, func, depth=0, dirName='factories/'):
  func(' '*depth +'<library>%s%s</library>'%(dirName,self.uname))

bpy.types.Mesh.AsCSRef = MeshAsCSRef


#======== Genmesh ==============================================================

def AsCSGenmeshLib(self, func, depth=0, **kwargs):
  """ Write an xml description of this mesh as a CS general mesh factory
  """

  func(' '*depth + '<meshfact name=\"%s\">'%(self.uname))
  if self.use_imposter:
    func(' '*depth + '  <imposter range="100.0" tolerance="0.4" camera_tolerance="0.4" shader="lighting_imposter"/>')
  func(' '*depth + '  <plugin>crystalspace.mesh.loader.factory.genmesh</plugin>')
  func(' '*depth + '  <params>')

  # Recover submeshes from kwargs
  if 'subMeshes' in kwargs:
    subMeshes = kwargs['subMeshes']

  if SubmeshesLackMaterial(subMeshes):
    #This mesh has a submesh without a material
    if self.GetMaterial(0):
      mat = self.GetMaterial(0).uname
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

bpy.types.Mesh.AsCSGenmeshLib = AsCSGenmeshLib


#======= Mapping buffer ===========================================

class CSMappingVertex:
  def __init__(self, indexFace, indexVertex, indexCSVertex):
    self.face = indexFace
    self.vertex = indexVertex
    self.csVertex = indexCSVertex


def GetCSMappingBuffer (self):
  """ Generate mapping buffers between Blender and CS vertices.
      Return mapVert, list defining the Blender vertex corresponding
      to each CS vertex, and mapBuf, list defining a list of CSMappingVertex
      for each Blender vertex (i.e. the face, the index of this vertex in the face
      and the corresponding CS vertex)
  """
  # Determine the UV texture: we only use the active UV texture or the first one
  if self.uv_textures.active:
    tface = self.uv_textures.active.data
  elif len(self.uv_textures) != 0:
    tface = self.uv_textures[0].data
  else:
    print("WARNING: mesh",self.name,"has no UV texture")
    tface = None

  # TODO: double sided faces

  # Mesh without texture coordinates
  if tface == None:
    mapBuf = []
    mapVert = []
    for indexV, vertex in enumerate(self.vertices):
      indexBuf = []
      for indexF, face in enumerate(self.faces):
        tt = [2,1,0] if len(face.vertices)==3 else [3, 2, 1, 0]
        for i in tt:
          indexBuf.append(CSMappingVertex(indexF,i,indexV))
      mapVert.append(indexV)
      mapBuf.append(indexBuf)
    return mapVert, mapBuf

  # Mesh with texture coordinates
  epsilon = 1e-9
  mapBuf = []
  mapVert = []
  csIndex = 0
  for vertex in self.vertices:
    mapBuf.append([])

  for indexF, face in enumerate(self.faces):
    tt = [2,1,0] if len(face.vertices)==3 else [3, 2, 1, 0]
    for i in tt:
      if len(mapBuf[face.vertices[i]]) == 0:
        # Blender vertex is not defined in CS
        # => create a new CS vertex
        mapBuf[face.vertices[i]].append(CSMappingVertex(indexF,i,csIndex))
        mapVert.append(face.vertices[i])
        csIndex += 1
      else:
        vertexFound = False
        uv = tface[indexF].uv[i]
        for mappedI, mappedV in enumerate(mapBuf[face.vertices[i]]):
          if (abs(uv[0] - tface[mappedV.face].uv[mappedV.vertex][0]) < epsilon) and \
             (abs(uv[1] - tface[mappedV.face].uv[mappedV.vertex][1]) < epsilon):
            # Blender vertex is defined in CS with the same (u,v) coordinates
            # => add a reference to the corresponding CS vertex
            vertexFound = True
            mapBuf[face.vertices[i]].append(CSMappingVertex(indexF,i,mappedV.csVertex))
            break
        if not vertexFound:
          # Blender vertex is defined in CS but with different (u,v) coordinates
          # => create a new CS vertex
          mapBuf[face.vertices[i]].append(CSMappingVertex(indexF,i,csIndex))
          mapVert.append(face.vertices[i])
          csIndex += 1

  return mapVert, mapBuf

bpy.types.Mesh.GetCSMappingBuffer = GetCSMappingBuffer


#======= Armature ===========================================

def GetBoneNames (bones, boneNames={}, index=-1):
  ''' Return a list of bone names with their corresponding order index
      param bones: list of bones      
  '''
  def GetChildBoneNames (children, boneNames, index):
    for child in children:
      index += 1
      boneNames[child.name] = index
      boneNames, index = GetChildBoneNames(child.children, boneNames, index)
    return boneNames, index
    
  for bone in bones:
    if not bone.parent:
      index += 1
      boneNames[bone.name] = index
      boneNames, index = GetChildBoneNames(bone.children, boneNames, index)
  return boneNames
  

def AsCSSkeletonAnimations (self, func, depth, armatureObject):
  """ Export Blender skeleton and associated animations
  """
  func('<library>')
  func('  <addon plugin="crystalspace.skeletalanimation.loader">')

  # EXPORT ANIMATIONS
  boneNames = GetBoneNames(self.bones)
  armatureName = armatureObject.name

  # Create scene for baking faster 
  # and link a copy of the armature to this new scene
  armatureObjectCpy = armatureObject.copy()
  bakingScene = bpy.data.scenes.new('bakingScene')
  bakingScene.objects.link(armatureObjectCpy)
  initialPose = armatureObjectCpy.data.pose_position
  armatureObjectCpy.data.pose_position = 'POSE'
  bakingScene.update()

  # Actions
  exportedActions = []
  actions = []
  if bpy.data.actions:
    actions = bpy.data.actions

  for action in actions:
    armatureObjectCpy.animation_data.action = action
    boneKeyframes = {}
    lostBones = []
    cyclic_action = False

    # Curves
    for curve in action.fcurves:
      if not curve.is_valid:
        print("Error: Curve is not valid", curve.data_path, curve.array_index)
        continue
        
      dp = curve.data_path        # (p.e. "pose.bones['BoneName'].location")
      if dp[:10] == "pose.bones":
        dpSplit = dp[10:].split('[')[1].split(']')
        boneName = dpSplit[0][1:-1:]
        op = dpSplit[1][1:]
          
        if boneName not in boneNames:
          if boneName not in lostBones:
            lostBones.append(boneName)
          continue

        if op != 'location' and op != 'rotation_quaternion':
          continue
        # TODO: export 'scale' operations

        # Keyframes
        if boneName not in boneKeyframes:
          boneKeyframes[boneName] = []
        for keyframe in curve.keyframe_points:
          if keyframe.co[0] not in boneKeyframes[boneName]:
            boneKeyframes[boneName].append(keyframe.co[0])

    if len(lostBones):
      print("WARNING: Action '%s' not exported: the following bones are not part of the armature"%(action.name),lostBones)
      continue

    if len(boneKeyframes) == 0:
      print("WARNING: Action '%s' not exported: no keyframe defined"%(action.name))
      continue

    # Verify if the action defines armature movement
    displacementFound = False
    for bone in boneKeyframes.keys():
      if len(boneKeyframes[bone]) > 1:
        displacementFound = True
        break
    if not displacementFound:
      print("INFO: Action '%s' defines a pose (no armature movement)"%(action.name))

    # Write animation packet
    exportedActions.append(action)
    if len(exportedActions) == 1:
      func('  <animationpacket name="%s_packet">'%(armatureName))
    func('    <animation name="%s">'%(action.name))

    fps = 24.0
    for boneName in boneKeyframes.keys():
      # Write bone index
      boneIndex = boneNames[boneName]
      func('      <channel bone="%s">'%(boneIndex))
      frames = boneKeyframes[boneName]
      for time in sorted(frames):
        # Write time and bone position/rotation for each frame
        bakingScene.frame_set(int(round(time)))
        bone = armatureObjectCpy.data.bones[boneName]
        pos, quat = bone.GetBoneCurrentMatrix(armatureObjectCpy)
        scale = armatureObjectCpy.scale
        csTime = time/fps
        func(' '*8 + '<key time="%g" x="%s" y="%s" z="%s" qx="%s" qy="%s" qz="%s" qw="%s" />' % \
             (csTime, scale[0]*pos[0], scale[2]*pos[2], scale[1]*pos[1], quat.x, quat.z, quat.y, -quat.w))
      func('      </channel>')
    func('    </animation>')

  if len(exportedActions) != 0:
    # Write animations tree
    func('    <node type="fsm" name="root">')
    for action in exportedActions:
      func('      <state name="%s">'%(action.name))
      func('        <node type="animation" animation="%s"/>'%(action.name))
      func('      </state>')
    func('    </node>')
    func('  </animationpacket>')
    print('INFO: %s animation(s) exported for skeleton "skel%s_rig"'%(len(exportedActions),armatureName))

  # Unlink the armature copy and delete baking scene
  armatureObjectCpy.data.pose_position = initialPose
  bakingScene.objects.unlink(armatureObjectCpy)
  bpy.data.objects.remove(armatureObjectCpy)
  bpy.data.scenes.remove(bakingScene)

  # EXPORT SKELETON
  func('  <skeleton name="skel%s_rig">'%(armatureName))
  for bone in self.bones:
    if not bone.parent:
      bone.AsCS(func, depth+2, armatureObject.matrix_world)
  if len(exportedActions):
    func('    <animationpacket>%s_packet</animationpacket>'%(armatureName))
  func('  </skeleton>')
  print('INFO: %s bone(s) exported for skeleton "skel%s_rig"'%(len(boneNames),armatureName))

  func('  </addon>')
  func('</library>')

bpy.types.Armature.AsCSSkelAnim = AsCSSkeletonAnimations


def GetBoneInfluences (self, objects, **kwargs):
  """ Generate a list of bone influences (4 per vertex) for each vertex of
      the mesh animated by this armature.
      param objects: list of the objects composing the scene
      param kwargs: mapping buffers of this armature
  """
  # Recover mapping buffers from kwargs
  if 'meshData' in kwargs:
    meshData = kwargs['meshData']
  if 'mappingVertices' in kwargs:
    mappingVertices = kwargs['mappingVertices']

  # Get bone influences per vertex (max 4)
  boneNames = GetBoneNames(self.bones)
  epsilon = 1e-8
  influences = []
  lostInfluences = False
  undefinedGroups = False
  lostGroups = []
  for index, ob in enumerate(meshData):
    # For each mesh depending of the armature
    for indexV in mappingVertices[index]:
      # For each vertex of the mesh
      weights = []
      for groupEl in ob.data.vertices[indexV].groups:
        # For each vertex group
        if groupEl.weight > epsilon:
          # Get the bone and its influence (weight) on the vertex
          groupIndex = find(lambda gr: gr.index==groupEl.group, ob.vertex_groups)
          if groupIndex == None:
            undefinedGroups = True
            continue
          group = ob.vertex_groups[groupIndex]
          if group.name not in boneNames:
            if group.name not in lostGroups:
              lostGroups.append(group.name)
            continue
          boneIndex = boneNames[group.name]
          weights.append([boneIndex, groupEl.weight])

          # TODO: treat all armatures, not only the root one

      # Sort the bone influences of the vertex (descending order)
      weights.sort(key=lambda el: el[1],reverse=True)

      # Add the 4 largest influences of the vertex
      countBones = 0
      sumWeights = 0
      for influence in weights:
        if countBones < 4:
          countBones += 1
          sumWeights += influence[1]
        else:
          lostInfluences = True
          break

      # Normalize the bone influences to 1
      for i in range(countBones):
        if sumWeights > epsilon:
          weights[i][1] = weights[i][1] / sumWeights
        influences.append(weights[i])

      # Fill vertex influences with null values if less then 4 non null
      # influences have been found for this vertex
      if countBones < 4:
        for i in range(4-countBones):
          influences.append([0, 0.0])

  if undefinedGroups:
    print("ERROR: undefined vertex group index; corresponding bone influences have been lost!")
  if len(lostGroups):
    print("WARNING: vertex groups defined on unknown bones; corresponding bone influences have been lost!",lostGroups)
  if lostInfluences:
    print("WARNING: some bone influences have been lost (max=4/vertex)")

  return influences

bpy.types.Armature.GetBoneInfluences = GetBoneInfluences


#======= Bone ===========================================

def GetBoneMatrix (self, worldTransform):
  ''' Return translation and rotation (quaternion) parts of the local bone transformation
      corresponding to the rest pose
  '''
  if not self.parent:
    loc, rot, scale = DecomposeMatrix(worldTransform)
    bone_matrix = self.matrix_local * rot
  else:
    parent_imatrix = Matrix(self.parent.matrix_local).inverted()
    bone_matrix = parent_imatrix * self.matrix_local

  return (bone_matrix.to_translation(), self.matrix.to_quaternion())

bpy.types.Bone.GetBoneMatrix = GetBoneMatrix


def GetBoneCurrentMatrix (self, armatureObject):
  ''' Return translation and rotation (quaternion) parts of the local bone transformation
      corresponding to the current pose of the armature
  '''
  poseBones = armatureObject.pose.bones
  poseBone = poseBones[self.name]

  if not self.parent:
    loc, rot, scale = DecomposeMatrix(poseBone.matrix_channel)
    bone_matrix = poseBone.matrix * rot
  else:
    poseBoneMatrix = poseBone.matrix
    poseParent = poseBones[self.parent.name]
    poseParentMatrix = poseParent.matrix
    parent_imatrix = Matrix(poseParentMatrix).inverted()
    bone_matrix = parent_imatrix * poseBoneMatrix

  return (bone_matrix.to_translation(), bone_matrix.to_quaternion())

bpy.types.Bone.GetBoneCurrentMatrix = GetBoneCurrentMatrix


def AsCSBone (self, func, depth, worldTransform):
  """ Write an xml description (name and local transformation) of this bone
  """
  func(' '*depth + '<bone name="%s">'%(self.name))

  # Transform matrix
  pos, quat = self.GetBoneMatrix(worldTransform)
  scale = worldTransform.to_scale()
  func(' '*depth + '  <transform x="%s" y="%s" z="%s" qx="%s" qy="%s" qz="%s" qw="%s" />'% \
          (scale[0]*pos[0], scale[2]*pos[2], scale[1]*pos[1], quat.x, quat.z, quat.y, -quat.w))
  for childBone in self.children:
    childBone.AsCS(func, depth+2, worldTransform)

  func(' '*depth + '</bone>')

bpy.types.Bone.AsCS = AsCSBone


def GetSocketMatrix (self, socketObject, armatureObject):
  """ Get the relative transformation (location and rotation) of the socketObject
      attached to this bone, according to the current pose position of the armature
  """
  # Save current armature state (rest or pose position)
  # and set the 'pose mode'
  initialPose = armatureObject.data.pose_position
  armatureObject.data.pose_position = 'POSE'

  # Compute socket matrix for current armature pose
  poseBones = armatureObject.pose.bones
  poseBone = poseBones[self.name]
  bone_iMatrix = poseBone.matrix.inverted()
  skel_iMatrix = armatureObject.matrix_world.inverted()
  socketMatrix = bone_iMatrix * skel_iMatrix * socketObject.matrix_world

  # Restore initial armature state
  armatureObject.data.pose_position = initialPose

  # Decompose socket matrix
  loc, rot, scale = DecomposeMatrix(socketMatrix)
  return (loc, rot)

bpy.types.Bone.GetSocketMatrix = GetSocketMatrix


def AsCSSocket(self, func, depth, socketObject, armature):
  """ Write an xml description of this socket (i.e. name of the attached object, 
      name of the bone and relative transformation matrix), using the current pose
      position of the armature
  """
  print("Export socket: object '%s' attached to bone '%s'"%(socketObject.name,self.name))

  # Get local transformation of the socket
  pos, rot = self.GetSocketMatrix(socketObject, armature)
  rot = rot.to_euler()

  # Get armature's scale
  scale = armature.scale

  # Write socket description
  func(' '*depth + '<socket name="%s" bone="%s">'%(socketObject.name, self.name))  
  func(' '*depth + '  <transform>')
  func(' '*depth + '    <vector x="%s" y="%s" z="%s" />' % (scale[0]*pos[0], scale[2]*pos[2], scale[1]*pos[1]))
  func(' '*depth + '    <matrix>')
  if rot[0]:
    func(' '*depth + '      <rotx>%s</rotx>'%(rot[0]))
  if rot[1]:
    func(' '*depth + '      <rotz>%s</rotz>'%(rot[1]))
  if rot[2]:
    func(' '*depth + '      <roty>%s</roty>'%(-rot[2]))
  func(' '*depth + '    </matrix>')
  func(' '*depth + '  </transform>')
  func(' '*depth + '</socket>')

bpy.types.Bone.AsCSSocket = AsCSSocket
