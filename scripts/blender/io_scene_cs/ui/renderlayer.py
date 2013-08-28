import os
import bpy
from io_scene_cs.utilities import rnaType, GetPreferences, GetExportPath, GetDefaultPath

class csReferencesPanel():
  bl_space_type = "PROPERTIES"
  bl_region_type = "WINDOW"
  bl_context = "render_layer"
  # COMPAT_ENGINES must be defined in each subclass, external engines can add themselves here

  @classmethod
  def poll(cls, context):
    rd = context.scene.render
    return (rd.engine in cls.COMPAT_ENGINES)


@rnaType
class RENDER_PT_csReferenceMaterialPanel(csReferencesPanel, bpy.types.Panel):
  bl_label = "Crystal Space Material References"
  bl_options = {'DEFAULT_CLOSED'}
  COMPAT_ENGINES = {'CRYSTALSPACE'}

  def draw(self, context):
    layout = self.layout
    
    row = layout.row(align=True)
    row.operator("io_scene_cs.add_material", text="Add material reference")

    for mat in GetPreferences().MaterialRefs:
      col = layout.column()
      col.context_pointer_set("current_material", mat)
      box = col.box()
      
      row = box.row()
      row.prop(mat, "name", text="Material name")
      row.operator("io_scene_cs.rmv_material", text="", icon='X', emboss=False)

      row = box.row()
      row.prop(mat, "vfs", text="VFS path")

      
@rnaType
class RENDER_PT_csReferenceFactoryPanel(csReferencesPanel, bpy.types.Panel):
  bl_label = "Crystal Space Factory References"
  bl_options = {'DEFAULT_CLOSED'}
  COMPAT_ENGINES = {'CRYSTALSPACE'}

  def draw(self, context):
    layout = self.layout
    
    row = layout.row(align=True)
    row.operator("io_scene_cs.add_factory", text="Add factory reference")

    for fact in GetPreferences().FactoryRefs:
      col = layout.column()
      col.context_pointer_set("current_factory", fact)
      box = col.box()
      
      row = box.row()
      row.prop(fact, "name", text="Factory name")
      row.operator("io_scene_cs.rmv_factory", text="", icon='X', emboss=False)

      row = box.row()
      row.prop(fact, "vfs", text="VFS path")


#=============== Crystal Space Factory Reference panel ======================

class csFactoryRef (bpy.types.PropertyGroup):
  name = bpy.props.StringProperty(name="Name Prop", default="Unknown", \
                description="Name of an existing Crystal Space factory")
  vfs  = bpy.props.StringProperty(name="VFS Prop", default=GetDefaultPath(), \
                description="VFS path of an existing Crystal Space library file")

bpy.utils.register_class(csFactoryRef)


class B2CS_OT_csAddFactory(bpy.types.Operator):
  bl_idname = "io_scene_cs.add_factory"
  bl_label = "Add factory reference"
  bl_description = "Define a reference to existing Crystal Space factory"

  def execute(self, context): 
    new_fact = GetPreferences().FactoryRefs.add()

    return {'FINISHED'}


class B2CS_OT_csRemoveFactory(bpy.types.Operator):
  bl_idname = "io_scene_cs.rmv_factory"
  bl_label = "Remove factory reference"
  bl_description = "Remove a reference to existing CS factory"

  def execute(self, context): 
    for i,fact in enumerate(GetPreferences().FactoryRefs):
      if fact.name == context.current_factory.name:
        GetPreferences().FactoryRefs.remove(i)
        break

    return {'FINISHED'}




#============= Crystal Space Material Reference panel ====================

class csMaterialRef (bpy.types.PropertyGroup):
  name = bpy.props.StringProperty(name="Name Prop", default="Unknown", \
                description="Name of an existing Crystal Space material")
  vfs  = bpy.props.StringProperty(name="VFS Prop", default=GetDefaultPath(), \
                description="VFS path of an existing Crystal Space library file")

bpy.utils.register_class(csMaterialRef)



class B2CS_OT_csAddMaterial(bpy.types.Operator):
  bl_idname = "io_scene_cs.add_material"
  bl_label = "Add material reference"
  bl_description = "Define a reference to existing Crystal Space material"

  def execute(self, context): 
    new_mat = GetPreferences().MaterialRefs.add()

    return {'FINISHED'}


class B2CS_OT_csRemoveMaterial(bpy.types.Operator):
  bl_idname = "io_scene_cs.rmv_material"
  bl_label = "Remove material reference"
  bl_description = "Remove a reference to existing Crystal Space material"

  def execute(self, context): 
    for i,mat in enumerate(GetPreferences().MaterialRefs):
      if mat.name == context.current_material.name:
        GetPreferences().MaterialRefs.remove(i)
        break

    return {'FINISHED'}

