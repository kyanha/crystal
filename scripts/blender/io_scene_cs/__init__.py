__all__ = ["ui", "oi", "utilities"]

bl_info = {
    "name": "Export Crystal Space 3D format",
    "description": "Export meshes, scenes and animations",
    "author": "The Crystal Space team",
    "version": (2, 2),
    "blender": (2, 59, 0),
    "api": 55057,
    "location": "The main panel is in 'Properties > Render'",
    "warning": "",
    "wiki_url": "http://www.crystalspace3d.org/docs/online/manual/Blender.html",
    "tracker_url": "http://crystalspace3d.org/trac/CS/report",
    "category": "Import-Export"}

if "bpy" in locals():
  import imp
  imp.reload(utilities)
  imp.reload(ui)
  imp.reload(io)

else:
  import os
  import bpy
  from bpy.types import Operator
  from bpy.props import StringProperty, IntProperty, BoolProperty

  from . import utilities
  from . import ui
  from . import io

try:
  from bpy.types import AddonPreferences
except:
  AddonPreferences = None

if AddonPreferences != None:

  class ExportCSPreferences(AddonPreferences):
    bl_idname = __name__

    default_path = os.environ.get("TEMP")
    if not default_path:
      if os.name == 'nt':
        default_path = "c:/tmp/"
      else:
        default_path = "/tmp/"
    elif not default_path.endswith(os.sep):
      default_path += os.sep

    exportpath = StringProperty(
            name = "Default Export Path",
            default = default_path,
            subtype ='FILE_PATH',
            )

    def draw(self, context):
        layout = self.layout
        layout.label(text="This is a preferences view for our addon")
        layout.prop(self, "exportpath")


class ExportCS(Operator):
    bl_idname = "export.cs"
    bl_label = "Export CS"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        """
        user_preferences = context.user_preferences
        addon_prefs = user_preferences.addons[__name__].preferences
        info = ("Path: %s" % addon_prefs.exportpath)

        self.report({'INFO'}, info)
        print(info)
        """

        return {'FINISHED'}


def menu_func(self, context):
    self.layout.operator(ExportCS.bl_idname, text="CrystalSpace 3D") 

# Registration
def register():
    bpy.utils.register_module(__name__)
    #bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
    bpy.utils.unregister_module(__name__)
    #bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
    register()
