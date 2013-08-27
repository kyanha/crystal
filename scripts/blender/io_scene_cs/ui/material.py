
import bpy
import operator

from io_scene_cs.utilities import rnaType, GetPreferences, SHADERSETS, GetShaderSetName, settings, RENDERPRIORITIES, ZBUFFERMODES
from bpy.types import PropertyGroup

class csMaterialPanel():
  bl_space_type = 'PROPERTIES'
  bl_region_type = 'WINDOW'
  bl_context = "material" 
  # COMPAT_ENGINES must be defined in each subclass, external engines can add themselves here
  
  @classmethod
  def poll(cls, context):
      rd = context.scene.render
      r = (context.material or context.object)
      return r and (rd.engine in cls.COMPAT_ENGINES)


class SelectMaterialRef(bpy.types.Operator):
    bl_idname = "material.select_mat_ref"
    bl_label = "Select CS material"

    def avail_materials(self,context):
        items = [(str(i),m.name,m.vfs) for i,m in enumerate(GetPreferences().MaterialRefs)]
        items.append((str(-1),' NONE','None'))
        return sorted(items, key=operator.itemgetter(1))
    select_material = bpy.props.EnumProperty(items = avail_materials, name = "Available CS materials")

    @classmethod
    def poll(cls, context):
        return context.material != None
    
    def execute(self,context):
        mat = context.material
        if int(self.select_material) != -1:
          mat.csMaterialName = GetPreferences().MaterialRefs[int(self.select_material)].name
          mat.csMaterialVfs = GetPreferences().MaterialRefs[int(self.select_material)].vfs
        else:
          mat.csMaterialName = 'None'
          mat.csMaterialVfs = ''
        return {'FINISHED'}

@rnaType
class MATERIAL_PT_B2CS__context_material(csMaterialPanel, bpy.types.Panel):
    bl_label = "Crystal Space Material"
    COMPAT_ENGINES = {'CRYSTALSPACE'}

    def draw(self, context):
        layout = self.layout

        mat = context.material

        if mat:
          layout = self.layout
          row = layout.row()
          row.prop(mat, "priority")
          row = layout.row()
          row.prop(mat, "zbuf_mode")

          # Draw a checkbox to define current material as a CS material reference
          layout.separator()
          row = layout.row()
          row.prop(mat, "csMatRef")

          if mat.csMatRef:
            # Let the user select a CS material
            row = layout.row()
            if mat.csMaterialName == 'None':
              row.operator_menu_enum("material.select_mat_ref", "select_material", text=SelectMaterialRef.bl_label)
            else:
              row.operator_menu_enum("material.select_mat_ref", "select_material", text=mat.csMaterialName)
              # Verify that factory reference still exists
              materials = [m.name for m in GetPreferences().MaterialRefs]
              if not mat.csMaterialName in materials:
                row = layout.row()
                row.label(text="WARNING: this material reference has been deleted!", icon='ERROR')

          else:
            # CS material properties (used if this material is not replaced by
            # a reference to an existing CS material)
            layout.separator()
            
            row = layout.row()
            row.prop(mat.b2cs, "shaderset")
            name = GetShaderSetName(mat.b2cs.shaderset)
            if name == 'water_plane':
              row = layout.row()
              row.prop(mat.water, "water_fog_color")
              row = layout.row()
              row.prop(mat.water, "water_perturb_scale")
              row = layout.row()
              row.prop(mat.water, "water_fog_density")




@settings(type='Material')
class CrystalSpaceSettingsMaterial(PropertyGroup):
  shaderset = bpy.props.EnumProperty(
            name="ShaderSet",
            description="",
            items=SHADERSETS,
            default="DEFAULT")
            
  priority = bpy.props.EnumProperty(
            name="Render priority",
            description="Priority level in which the object will be renderered",
            items=RENDERPRIORITIES,
            default="object")
            
  zbuf_mode = bpy.props.EnumProperty(
            name="Z-buffer mode",
            description="Behavior of the rendering of the object regarding the Z-Buffer",
            items=ZBUFFERMODES,
            default="zuse")
            
            
  csMatRef = bpy.props.BoolProperty(
            name="Material replaced by a CS material",
            description="Replace this material by a Crystal Space material",
            default=False)

  csMaterialName = bpy.props.StringProperty(
            name="Reference of CS material",
            description="Name of an existing Crystal Space material",
            default="None")

  csMaterialVfs = bpy.props.StringProperty(
            name="VFS path of CS material",
            description="VFS path of a Crystal Space library file",
            default="")


@settings(type='Material', attribute='water')
class CrystalSpaceSettingsMaterial(PropertyGroup):            
  water_fog_color = bpy.props.StringProperty(
            name="Water Fog Color",
            description="Fog color inside the water surface",
            default='0,0.1,0.15,1')
            
  water_fog_density = bpy.props.FloatProperty(
            name="Water Fog Density",
            description="Fog density inside the water surface",
            default=3.0)
            
  water_perturb_scale = bpy.props.StringProperty(
            name="Water Perturb Scale",
            description="Fog perturb scale",
            default='0.9,0.9,0,0')
        


