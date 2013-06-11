import bpy

from io_scene_cs.utilities import rnaType, B2CS, BoolProperty

    
class csGroupPanel():
  bl_space_type = "PROPERTIES"
  bl_region_type = "WINDOW"
  bl_context = "object"
  b2cs_context = "object"
  bl_label = ""

  @classmethod
  def poll(cls, context): 
    ob = bpy.context.active_object
    r = False
    if (ob and ob.type == 'MESH') or (ob and ob.type == 'EMPTY'):
      for group in bpy.data.groups:
        if ob.name in group.objects:
          r = True
          break
    return r


@rnaType
class OBJECT_PT_B2CS_groups(csGroupPanel, bpy.types.Panel):
  bl_label = "Crystal Space Mesh Groups"

  def draw(self, context):
    layout = self.layout

    ob = bpy.context.active_object

    for group in bpy.data.groups:
      if ob.name in group.objects:
    
        row = layout.row()
        row.prop(group, "doMerge")
        
        if group.doMerge:
          row = layout.row()
          row.prop(group, "groupedInstances")
        

BoolProperty(['Group'], attr="doMerge", name="Merge", description="Merge the objects in this group into one on export")

BoolProperty(['Group'], attr="groupedInstances", name="Grouped Instances", description="Export children as a grouped Instances")
