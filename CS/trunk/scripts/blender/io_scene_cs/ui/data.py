import bpy

from io_scene_cs.utilities import rnaType, rnaOperator, B2CS, BoolProperty

from io_scene_cs.utilities import HasSetProperty, RemoveSetPropertySet 

from io_scene_cs.utilities import RemovePanels, RestorePanels 


class csFactoryPanel():
  bl_space_type = "PROPERTIES"
  bl_region_type = "WINDOW"
  bl_context = "data"
  b2cs_context = "data"
  bl_label = ""
  REMOVED = []

  @classmethod
  def poll(cls, context):
    ob = bpy.context.active_object
    r = (ob and ob.type == 'MESH' and ob.data)
    if r:
      csFactoryPanel.REMOVED = RemovePanels("data", ["DATA_PT_uv_texture", "DATA_PT_vertex_colors", "DATA_PT_vertex_groups"])
    else:
      RestorePanels(csFactoryPanel.REMOVED)
      csFactoryPanel.REMOVED = []
    return r


@rnaOperator
class MESH_OT_csFactory_RemoveProperty(bpy.types.Operator):
  bl_idname = "csFactory_RemoveProperty"
  bl_label = ""

  def invoke(self, context, event):
    ob = bpy.context.active_object.data
    RemoveSetPropertySet(ob, self.properties.prop)
    return('FINISHED',)
        

@rnaType
class MESH_PT_csFactory(csFactoryPanel, bpy.types.Panel):
  bl_label = "Crystal Space mesh factory"

  def LayoutAddProperty(self, row, ob, name):
    split = row.split(percentage=0.5)
    colL = split.column()
    colR = split.column()
  
    colL.prop(ob, name)
  
    if not HasSetProperty(ob, name):
      colR.label(text="(default: '%s')"%getattr(ob, name))
    else:
      d = colR.operator("csFactory_RemoveProperty", text="Default")
      d.prop = name

  
  def draw(self, context):
    layout = self.layout
    
    ob = bpy.context.active_object
    
    if ob.type == 'MESH':
      ob = bpy.context.active_object.data
      row = layout.row()
      self.LayoutAddProperty(row, ob, "goodCollider")
  
      row = layout.row()
      self.LayoutAddProperty(row, ob, "badCollider")
      
      row = layout.row()
      row.prop(ob, "use_imposter")
    

BoolProperty(['Mesh'], attr="use_imposter", name="imposter", description="Set wether this mesh should employ an imposter", default=False)
