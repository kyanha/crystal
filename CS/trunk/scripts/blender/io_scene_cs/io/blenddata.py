import bpy
from io_scene_cs.utilities import GetPreferences

def GetExportableScenes(context):
  scenes = [bpy.context.scene] if GetPreferences().exportOnlyCurrentScene else [scene for scene in bpy.data.scenes if scene.b2cs.export]
  return scenes

bpy.types.BlendData.exportable_scenes = property(GetExportableScenes)
