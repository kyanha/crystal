import bpy

from io_scene_cs.utilities import rnaType, rnaOperator, B2CS, BoolProperty

from io_scene_cs.utilities import HasSetProperty, RemoveSetPropertySet 

from io_scene_cs.utilities import RemovePanels, RestorePanels 


class csModifierPanel():
  bl_space_type = "PROPERTIES"
  bl_region_type = "WINDOW"
  bl_context = "modifier"
  b2cs_context = "modifiers"
  bl_label = ""
  REMOVED = []

  @classmethod
  def poll(cls, context):
    ob = bpy.context.active_object
    arrays = [mod for mod in ob.modifiers \
                if mod.type=='ARRAY' and mod.fit_type=='FIXED_COUNT'] 
    r = (ob and ob.type == 'MESH' and ob.data and \
           len(ob.modifiers)!=0 and len(arrays) == len(ob.modifiers))
    if r:
      csModifierPanel.REMOVED = RemovePanels("modifiers")
    else:
      RestorePanels(csModifierPanel.REMOVED)
      csModifierPanel.REMOVED = []
    return r


@rnaOperator
class MESH_OT_csModifier_RemoveProperty(bpy.types.Operator):
  bl_idname = "csmodifier.removeproperty"
  bl_label = ""

  def invoke(self, context, event):
    ob = bpy.context.active_object.data
    RemoveSetPropertySet(ob, self.properties.prop)
    return('FINISHED',)
        

@rnaType
class MESH_PT_csModifier(csModifierPanel, bpy.types.Panel):
  bl_label = "Crystal Space Modifiers"

  def draw(self, context):
    ob = context.active_object

    if ob.type == 'MESH' and len(ob.modifiers) != 0:
      # Draw a checkbox to decide how to export object's modifiers
      layout = self.layout
      row = layout.row()
      row.prop(ob.data, "array_as_meshobj")


BoolProperty(['Mesh'], 
             attr="array_as_meshobj", 
             name="Export arrays as mesh factory instances",
             description="Export fixed count array copies as instances" + \
               " of object factory ('meshobj') in world file",
             default=True)
