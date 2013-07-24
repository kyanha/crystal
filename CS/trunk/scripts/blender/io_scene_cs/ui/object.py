import bpy
import operator

from io_scene_cs.utilities import rnaType, rnaOperator, B2CS, BoolProperty, StringProperty, CollectionProperty
  

class csObjectPanel():
  bl_space_type = "PROPERTIES"
  bl_region_type = "WINDOW"
  bl_context = "object"
  b2cs_context = "object"
  bl_label = ""

  @classmethod
  def poll(cls, context):
    ob = bpy.context.active_object
    r = (ob and ob.type == 'MESH' and not ob.portal)
    return r


class SelectFactoryRef(bpy.types.Operator):
    bl_idname = "object.select_fact_ref"
    bl_label = "Select CS factory"

    def avail_factories(self,context):
        items = [(str(i),f.name,f.vfs) for i,f in enumerate(B2CS.properties.FactoryRefs)]
        items.append((str(-1),' NONE','None'))
        return sorted(items, key=operator.itemgetter(1))
    select_factory = bpy.props.EnumProperty(items = avail_factories, name = "Available CS factories")

    @classmethod
    def poll(cls, context):
        return context.mode == 'OBJECT'
    
    def execute(self,context):
        ob = context.scene.objects.active
        if int(self.select_factory) != -1:
          ob.csFactoryName = B2CS.properties.FactoryRefs[int(self.select_factory)].name
          ob.csFactoryVfs = B2CS.properties.FactoryRefs[int(self.select_factory)].vfs
        else:
          ob.csFactoryName = ''
          ob.csFactoryVfs = ''
        return {'FINISHED'}


@rnaType
class OBJECT_PT_csFactoryRef(csObjectPanel, bpy.types.Panel):
  bl_label = "Crystal Space Factories"

  def draw(self, context):    
    ob = context.active_object

    if ob.type == 'MESH':
      # Draw a checkbox to define current mesh object as a CS factory reference
      layout = self.layout
      row = layout.row()
      row.prop(ob, "csFactRef")

      if ob.csFactRef:
        # Let the user select a CS factory
        row = layout.row()
        if ob.csFactoryName == '':
          row.operator_menu_enum("object.select_fact_ref", "select_factory", text=SelectFactoryRef.bl_label)
        else:
          row.operator_menu_enum("object.select_fact_ref", "select_factory", text=ob.csFactoryName)
          # Verify that factory reference still exists
          factories = [f.name for f in B2CS.properties.FactoryRefs]
          if not ob.csFactoryName in factories:
            row = layout.row()
            row.label(text="WARNING: this factory reference has been deleted!", icon='ERROR')



BoolProperty(['Object'], 
             attr="csFactRef", 
             name="Object replaced by a CS factory object", 
             description="Replace this object by a Crystal Space factory", 
             default=False)

StringProperty(['Object'], 
               attr="csFactoryName", 
               name="Reference of CS factory", 
               description="Name of an existing Crystal Space factory",
               default='')

StringProperty(['Object'], 
               attr="csFactoryVfs", 
               name="VFS path of CS factory", 
               description="VFS path of a Crystal Space library file",
               default='')
