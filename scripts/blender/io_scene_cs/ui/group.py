import bpy

from io_scene_cs.utilities import rnaType, rnaOperator, B2CS, BoolProperty

@rnaType
class OBJECT_PT_B2CS_groups(bpy.types.Panel):
  bl_space_type = 'PROPERTIES'
  bl_region_type = 'WINDOW'
  bl_context = "object"
  b2cs_context = "object"
  bl_label = "Crystal Space mesh groups"
  
  @classmethod
  def poll(cls, context):
    ob = bpy.context.active_object
    return ob

  def draw(self, context):
    layout = self.layout

    ob = context.object
    
    row = layout.row(align=True)
    row.operator("object.group_link", text="Add to Group")
    row.operator("object.group_add", text="", icon='ZOOMIN')

    index = 0
    value = str(tuple(context.scene.cursor_location))
    for group in bpy.data.groups:
      if ob.name in group.objects:
        col = layout.column(align=True)

        col.context_pointer_set("group", group)

        row = col.box().row()
        row.prop(group, "name", text="")
        row.operator("object.group_remove", text="", icon='X', emboss=False)

        split = col.box().split()

        col = split.column()
        col.prop(group, "layers", text="Dupli")

        
        row = col.row()
        row.prop(group, "doMerge")
        
        if group.doMerge:
          row = col.row()
          row.prop(group, "groupedInstances")

        col = split.column()
        col.prop(group, "dupli_offset", text="")
        
        prop = col.operator("wm.context_set_value", text="From Cursor")
        prop.data_path = "object.users_group[%d].dupli_offset" % index
        prop.value = value
        index += 1
        

BoolProperty(['Group'], attr="doMerge", name="Merge", description="Merge the objects in this group into one on export")
BoolProperty(['Group'], attr="groupedInstances", name="Grouped Instances", description="Export children as a grouped Instances")
