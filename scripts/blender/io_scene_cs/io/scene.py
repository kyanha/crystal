import bpy

from .util import *


def ObjectHasMergingGroup(self):
  for group in self.users_group:
    if group.doMerge:
      return group
  return None

bpy.types.Object.hasMergingGroup = ObjectHasMergingGroup


def SceneAsCS(self, func, depth=0):
  func(' '*depth +'<sector name="%s">'%(self.uname))
    
  objects = []   
  for ob in [o for o in self.objects if o.type != 'CAMERA' and not o.parent]:
    if ob not in objects:
      group = ob.hasMergingGroup()
      if group:
        group.AsCS(func, depth+2)
        objects.extend([object for object in group.objects])
      else:
        ob.AsCS(func, depth+2)
    
  func(' '*depth +'</sector>')

  # Export cameras
  cameras = []
  for ob in self.objects:
    if ob.type == 'CAMERA':
      cameras.append(ob)
  self.CamerasAsCS(func, depth, cameras)

bpy.types.Scene.AsCS = SceneAsCS


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


# Export cameras of current scene as cs start locations
def CamerasAsCS (self, func, depth, cameras):
  if not cameras:
    # Set a default camera if none was found in current scene
    func(' '*depth +'<start name="Camera">')
    func(' '*depth +'  <sector>%s</sector>'%(self.uname))
    func(' '*depth +'  <position x="0.0" z="0.0" y="0.0" />')
    func(' '*depth +'  <up y="1" x="0" z="0" />')
    func(' '*depth +'</start>')
  else:
    # Export cameras sorted by names
    cameras.sort(key=lambda cam: cam.uname)
    for camera in cameras:
      func(' '*depth +'<start name="%s">'%(camera.uname))
      # Flip Y and Z axis.
      func(' '*depth +'  <sector>%s</sector>'%(self.uname))
      func(' '*depth +'  <position x="%f" z="%f" y="%f" />'%tuple(camera.location))
      func(' '*depth +'  <up y="1" x="0" z="0" />')
      func(' '*depth +'</start>')

bpy.types.Scene.CamerasAsCS = CamerasAsCS
