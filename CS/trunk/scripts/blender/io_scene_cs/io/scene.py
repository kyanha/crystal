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
