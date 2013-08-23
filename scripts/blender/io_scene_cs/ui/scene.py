import bpy

from io_scene_cs.utilities import rnaType, B2CS, BoolProperty
from io_scene_cs.utilities import FloatProperty

class csSectorPanel():
  bl_space_type = "PROPERTIES"
  bl_region_type = "WINDOW"
  bl_context = "scene"
  b2cs_context = "data"
  bl_label = ""

  @classmethod
  def poll(cls, context):
    return True


@rnaType
class SCENE_PT_csSector(csSectorPanel, bpy.types.Panel):
  bl_label = "Crystal Space Sector"

  def draw(self, context):
    layout = self.layout
    
    ob = bpy.context.scene

    split = layout.split()
    col1 = split.column(align=True)
    box1 = col1.box()

    row = box1.row()
    row.prop(ob, "export")
    
    if B2CS.properties.exportOnlyCurrentScene:
      row = box1.row()
      row.label(text="exportOnlyCurrentScene enabled so this won't have any effect.")




BoolProperty(['Scene'], 
     attr="export", 
     name="Export this scene", 
     description="Whether to export this scene",
     default=True)
