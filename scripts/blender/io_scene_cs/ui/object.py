import bpy
import operator

from io_scene_cs.utilities import rnaType, settings, GetPreferences, prepend_draw
from bpy.types import PropertyGroup
  

class csObjectPanel():
  bl_space_type = "PROPERTIES"
  bl_region_type = "WINDOW"
  bl_context = "object"
  # COMPAT_ENGINES must be defined in each subclass, external engines can add themselves here

  @classmethod
  def poll(cls, context):
    ob = bpy.context.active_object
    r = (ob and ob.type == 'MESH' and not ob.portal.enabled and not ob.IsVisCullMesh())
    rd = context.scene.render
    return r and (rd.engine in cls.COMPAT_ENGINES)


class SelectFactoryRef(bpy.types.Operator):
    bl_idname = "object.select_fact_ref"
    bl_label = "Select CS factory"

    def avail_factories(self,context):
        items = [(str(i),f.name,f.vfs) for i,f in enumerate(GetPreferences().FactoryRefs)]
        items.append((str(-1),' NONE','None'))
        return sorted(items, key=operator.itemgetter(1))
    select_factory = bpy.props.EnumProperty(items = avail_factories, name = "Available CS factories")

    @classmethod
    def poll(cls, context):
        return context.mode == 'OBJECT'
    
    def execute(self,context):
        ob = context.scene.objects.active
        if int(self.select_factory) != -1:
          ob.b2cs.csFactoryName = GetPreferences().FactoryRefs[int(self.select_factory)].name
          ob.b2cs.csFactoryVfs = GetPreferences().FactoryRefs[int(self.select_factory)].vfs
        else:
          ob.b2cs.csFactoryName = ''
          ob.b2cs.csFactoryVfs = ''
        return {'FINISHED'}



class SelectObjectRef(bpy.types.Operator):
    bl_idname = "object.select_object_ref"
    bl_label = "Select Object"

    def avail_objects(self,context):
        ob = context.active_object
        items = [(str(i),o.name,o.name) for i,o in enumerate(bpy.data.objects) if not o.IsVisCullMesh() and o.GetVisCullMesh() is None and o != ob]
        items.append((str(-1),' NONE','None'))
        return sorted(items, key=operator.itemgetter(1))
    select_object = bpy.props.EnumProperty(items = avail_objects, name = "Available Objects")

    @classmethod
    def poll(cls, context):
        return context.mode == 'OBJECT'
    
    def execute(self,context):
        ob = context.active_object
        mesh = ob.GetVisCullMesh()
        if mesh:
          mesh.UnMakeThisAVisCullMesh()
        
        index = int(self.select_object)
        if int(self.select_object) != -1:
          mesh = bpy.data.objects[index]
          mesh.MakeThisAVisCullMesh(ob)
          
        return {'FINISHED'}


class B2CS_OT_RemoveObjectRef(bpy.types.Operator):
  bl_idname = "object.remove_object_ref"
  bl_label = "Remove material reference"
  bl_description = "Remove a reference to existing Crystal Space material"

  def execute(self, context): 
    if context.current_viscullmesh:
      context.current_viscullmesh.UnMakeThisAVisCullMesh()
    return {'FINISHED'}


@rnaType
class OBJECT_PT_csFactoryRef(csObjectPanel, bpy.types.Panel):
  bl_label = "Crystal Space Factories"
  COMPAT_ENGINES = {'CRYSTALSPACE'}

  def draw(self, context):    
    ob = context.active_object

    if ob.type == 'MESH':
      # Draw a checkbox to define current mesh object as a CS factory reference
      layout = self.layout
      
      
      mesh = ob.GetVisCullMesh()
      layout.template_object_ref(mesh, "Viscull mesh")
      
      row = layout.row()
      row.prop(ob.b2cs, "csFactRef")

      if ob.b2cs.csFactRef:
        # Let the user select a CS factory
        row = layout.row()
        if ob.b2cs.csFactoryName == '':
          row.operator_menu_enum("object.select_fact_ref", "select_factory", text=SelectFactoryRef.bl_label)
        else:
          row.operator_menu_enum("object.select_fact_ref", "select_factory", text=ob.b2cs.csFactoryName)
          # Verify that factory reference still exists
          factories = [f.name for f in GetPreferences().FactoryRefs]
          if not ob.b2cs.csFactoryName in factories:
            row = layout.row()
            row.label(text="WARNING: this factory reference has been deleted!", icon='ERROR')


@prepend_draw(type='PHYSICS_PT_game_physics')
def PHYSICS_PT_game_physics_prepend_draw(self, context):  
  if context.scene.render.engine=='CRYSTALSPACE':
    ob = context.active_object
    layout = self.layout
    row = layout.row()
    if ob.game.physics_type in ['RIGID_BODY', 'SOFT_BODY']:
      row.label(text="Physics type supported in CS", icon='INFO')
    else:
      row.label(text="Physics type not supported in CS", icon='ERROR')


@settings(type='Object')
class CrystalSpaceSettingsObject(PropertyGroup):
  csFactRef = bpy.props.BoolProperty(
            name="Object replaced by a CS factory object",
            description="Replace this object by a Crystal Space factory",
            default=False)
  csFactoryName = bpy.props.StringProperty(
            name="Reference of CS factory",
            description="Name of an existing Crystal Space factory",
            default="")
  csFactoryVfs = bpy.props.StringProperty(
            name="VFS path of CS factory",
            description="VFS path of a Crystal Space library file",
            default="")

