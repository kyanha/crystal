import bpy
import mathutils
from .util import *


def ObjectHasMergingGroup(self):
  for group in self.users_group:
    if group.doMerge:
      return group
  return None

bpy.types.Object.hasMergingGroup = ObjectHasMergingGroup


def SceneDependencies(self):
  dependencies = EmptyDependencies()

  for ob in self.objects:
    if not ob.IsExportable():
      continue
    merge = False
    for group in ob.users_group:
      if group.doMerge:
        dependencies['G'][group.uname] = group
        MergeDependencies(dependencies, group.GetDependencies())
        merge = True
    if not merge:
      MergeDependencies(dependencies, ob.GetDependencies())
      
  return dependencies

bpy.types.Scene.GetDependencies = SceneDependencies


def SceneAsCS(self, func, depth=0):
  """ Write an xml decription of this scene: it is exported as a CS sector
      with a reference to all mesh, armature, group, lamp and portal objects
      contained in this scene (no reference to cameras and socket objects)
  """
  print("- sector '%s'"%self.uname)
  func(' '*depth +'<sector name="%s">'%(self.uname))
  
  # Export physics
  self.ScenePhysicsAsCS(func, depth+1)
  
  # Export portals
  objects = self.PortalsAsCS(func, depth)

  # Export lamps and objects (as instancies of factories)
  for ob in [o for o in self.objects \
               if o.type!='CAMERA' and o.parent_type!='BONE' and (o.parent is None or o.parent.type!='LAMP')]:
    print("- '%s' (%s)"%(ob.uname, ob.parent.type if ob.parent else None))
    if ob.IsExportable():
      if ob not in objects:
        group = ob.hasMergingGroup()
        if group:
          group.AsCS(func, depth+2)
          objects.extend([object for object in group.objects])
        else:
          ob.AsCS(func, depth+2)
          objects.append(ob)
    
  func(' '*depth +'</sector>')

bpy.types.Scene.AsCS = SceneAsCS


def ScenePhysicsAsCS(self, func, depth=0):
  
  engine = None
  if self.game_settings.physics_engine=='BULLET':
    engine = 'crystalspace.physics.bullet'
      
  if engine:
    gravity = -1 * self.game_settings.physics_gravity
    
    func(' '*depth +'<addon plugin="crystalspace.physics.loader">')
    func(' '*depth +'  <collisionsector>')
    func(' '*depth +'    <gravity x="0" y="%f" z="0" />'%(gravity))
    func(' '*depth +'    <damping linear="0.1" angular="0.1" />')  #TODO
    func(' '*depth +'    <autodisabling linear="0.8" angular="1.0" time="0" />')  #TODO
    func(' '*depth +'  </collisionsector>')
    func(' '*depth +'</addon>')
  

bpy.types.Scene.ScenePhysicsAsCS = ScenePhysicsAsCS



def PortalsAsCS(self, func, depth=0):
  """ Export portals defined in the current scene and
      return them as a list of objects
  """
  epsilon = 1e-7
  portals = []
  for ob in [o for o in self.objects if o.type=='MESH' and o.portal.enabled]:
    portals.append(ob)
    # Check if the portal is visible
    if ob.hide:
      continue

    # Check if the portal defines vertices
    if len(ob.data.vertices) == 0:
      print("\nWARNING: portal '%s' not exported: this mesh has no vertex"%(ob.name))
      continue

    # Find the destination scene
    source = ob
    destScName = source.portal.portalDestScene
    scenes = [s.name for s in bpy.data.scenes]
    if destScName == '' or not destScName in scenes:
      print("WARNING: portal '%s' not exported: target scene '%s' " \
            "is not a valid scene name"%(source.name,destScName))
      continue
    destScene = bpy.data.scenes[destScName]

    # Find the destination mesh object
    destObjName = source.portal.portalDestObject
    objects = [o.name for o in destScene.objects]
    if destObjName == '' or not destObjName in objects:
      print("WARNING: portal '%s' not exported: target mesh '%s' " \
            "is not a valid object name"%(source.name,destObjName))
      continue
    destObject = destScene.objects[destObjName]

    # Export the portal
    func(' '*depth +'  <portals name="%s">'%(ob.uname))
    func(' '*depth +'    <portal>')

    # Write the position of portal (i.e. the coordinates of its vertices)
    for v in source.data.vertices:
      vPos = source.relative_matrix * v.co
      func(' '*depth +'      <v x="%f" z="%f" y="%f" />'%(tuple(vPos)))

    # Write the name of the destination sector
    func(' '*depth +'      <sector>%s</sector>'%(destScene.uname))

    # Evaluate the warping transform
    # (a space warping portal changes the camera transform)
    currPos, currMat, scale = DecomposeMatrix(source.matrix_world)
    destPos, destMat, scale = DecomposeMatrix(destObject.matrix_world)
    totMat = destMat * currMat.inverted()
    destPos_after_rotate = destPos.to_4d() * totMat
    dpos = []
    # Vector added to the camera transform after the warping matrix is applied
    for i in range(len(currPos)):
      dpos.append(-(destPos_after_rotate[i]-currPos[i]))
    if (abs(dpos[0])>epsilon or abs(dpos[1])>epsilon or abs(dpos[2])>epsilon):
      func(' '*depth +'      <ww x="%f" y="%f" z="%f" />'%(dpos[0],dpos[2],dpos[1]))
    # Warping matrix
    itotMat = totMat.inverted()
    euler = itotMat.to_euler()
    quat = itotMat.to_quaternion()
    if abs(quat.angle)>epsilon:
      func(' '*depth +'      <matrix>')
      # Order 'YZX' is important!
      if abs(quat.axis[2])>epsilon:
        func(' '*depth +'        <roty>%s</roty>'%(euler[2]))
      if abs(quat.axis[1])>epsilon:
        func(' '*depth +'        <rotz>%s</rotz>'%(-euler[1]))
      if abs(quat.axis[0])>epsilon:
        func(' '*depth +'        <rotx>%s</rotx>'%(-euler[0]))
      func(' '*depth +'      </matrix>')

    # Set default portal flags
    func(' '*depth +'      <zfill />')
    func(' '*depth +'      <float />')
    func(' '*depth +'    </portal>')
    func(' '*depth +'    <priority>portal</priority>')
    func(' '*depth +'  </portals>')

  return portals

bpy.types.Scene.PortalsAsCS = PortalsAsCS


def GetCameras(self):
  """ Get a list of all visible cameras of this scene
  """
  cameras = {}
  for ob in self.objects:
    if ob.type == 'CAMERA' and not ob.hide:
      cameras[ob.uname] = {'scene': self, 'camera': ob}
  return cameras

bpy.types.Scene.GetCameras = GetCameras


def CameraAsCS (self, func, depth, camera=None):
  """ Export camera as a CS start location of current scene;
      if no camera is defined, set a default start position
  """
  if camera == None:
    # Set a default camera if none was found in current scene
    func(' '*depth +'<start name="Camera">')
    func(' '*depth +'  <sector>%s</sector>'%(self.uname))
    func(' '*depth +'  <position x="0" y="0" z="0" />')
    func(' '*depth +'  <up x="0" y="1" z="0" />')
    func(' '*depth +'  <forward x="0" y="0" z="1" />')
    func(' '*depth +'</start>')
  else:
    func(' '*depth +'<start name="%s">'%(camera.uname))
    func(' '*depth +'  <sector>%s</sector>'%(self.uname))
    # Flip Y and Z axis.
    func(' '*depth +'  <position x="%f" z="%f" y="%f" />'%tuple(camera.location))
    loc, rot, scale = DecomposeMatrix(camera.matrix_world)
    func(' '*depth +'  <up x="%f" z="%f" y="%f" />'%tuple(rot * mathutils.Vector((0.0, 1.0, 0.0))))
    func(' '*depth +'  <forward x="%f" z="%f" y="%f" />'%tuple(rot * mathutils.Vector((0.0, 0.0, -1.0))))
    func(' '*depth +'</start>')

bpy.types.Scene.CameraAsCS = CameraAsCS


#===== static method ExportCameras ==============================

def ExportCameras (func, depth, cameras):
  """ Export cameras sorted by names; each camera is described 
      as a CS start location for the scene it belongs to
      param cameras: list of cameras and their associated scene
  """
  keylist = cameras.keys()
  for camName in sorted(keylist, key=str.lower):
    cam = cameras[camName]
    cam['scene'].CameraAsCS(func, depth, cam['camera'])
