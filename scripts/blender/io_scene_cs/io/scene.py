import bpy

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
      with a reference to all objects of types mesh, armature, group and lamp
      contained in this scene
  """
  func(' '*depth +'<sector name="%s">'%(self.uname))
    
  objects = []   
  for ob in [o for o in self.objects if o.type != 'CAMERA']:
    if not ob.parent or (ob.parent and ob.parent_type == 'BONE'):
      if ob.type=='MESH' and (len(ob.data.vertices)==0 or len(ob.data.faces)==0):
        continue
      if ob not in objects:
        group = ob.hasMergingGroup()
        if group:
          group.AsCS(func, depth+2)
          objects.extend([object for object in group.objects])
        else:
          ob.AsCS(func, depth+2)
    
  func(' '*depth +'</sector>')

bpy.types.Scene.AsCS = SceneAsCS


def GetCameras(self):
  """ Get a list of all cameras of this scene
  """
  cameras = {}
  for ob in self.objects:
    if ob.type == 'CAMERA':
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
    func(' '*depth +'  <position x="0.0" z="0.0" y="0.0" />')
    func(' '*depth +'  <up y="1" x="0" z="0" />')
    func(' '*depth +'</start>')
  else:
    func(' '*depth +'<start name="%s">'%(camera.uname))
    func(' '*depth +'  <sector>%s</sector>'%(self.uname))
    # Flip Y and Z axis.
    func(' '*depth +'  <position x="%f" z="%f" y="%f" />'%tuple(camera.location))
    func(' '*depth +'  <up y="1" x="0" z="0" />')
    func(' '*depth +'</start>')

bpy.types.Scene.CameraAsCS = CameraAsCS


#===== static method ExportCameras ==============================

def ExportCameras (func, depth, cameras):
  """ Export cameras sorted by names; each camera is described 
      as a CS start location for the scene it belongs to
      param cameras: list of cameras and their associated scene
  """
  keylist = cameras.keys()
  for camName in sorted(keylist):
    cam = cameras[camName]
    cam['scene'].CameraAsCS(func, depth, cam['camera'])
