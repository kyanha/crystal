
import bpy
import operator

from io_scene_cs.utilities import rnaType, B2CS, EnumProperty, StringProperty, BoolProperty, FloatProperty, CollectionProperty, SHADERSETS, GetShaderSetName


class csMaterialPanel():
  bl_space_type = 'PROPERTIES'
  bl_region_type = 'WINDOW'
  bl_context = "material"
  b2cs_context = "material"
  
  @classmethod
  def poll(cls, context):
    r = (context.material or context.object)
    return r    


class SelectMaterialRef(bpy.types.Operator):
    bl_idname = "material.select_mat_ref"
    bl_label = "Select CS material"

    def avail_materials(self,context):
        items = [(str(i),m.name,m.vfs) for i,m in enumerate(B2CS.properties.MaterialRefs)]
        items.append((str(-1),' NONE','None'))
        return sorted(items, key=operator.itemgetter(1))
    select_material = bpy.props.EnumProperty(items = avail_materials, name = "Available CS materials")

    @classmethod
    def poll(cls, context):
        return context.material != None
    
    def execute(self,context):
        mat = context.material
        if int(self.select_material) != -1:
          mat.csMaterialName = B2CS.properties.MaterialRefs[int(self.select_material)].name
          mat.csMaterialVfs = B2CS.properties.MaterialRefs[int(self.select_material)].vfs
        else:
          mat.csMaterialName = 'None'
          mat.csMaterialVfs = ''
        return {'FINISHED'}

@rnaType
class MATERIAL_PT_B2CS__context_material(csMaterialPanel, bpy.types.Panel):
    bl_label = "Crystal Space Material"

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
              materials = [m.name for m in B2CS.properties.MaterialRefs]
              if not mat.csMaterialName in materials:
                row = layout.row()
                row.label(text="WARNING: this material reference has been deleted!", icon='ERROR')

          else:
            # CS material properties (used if this material is not replaced by
            # a reference to an existing CS material)
            layout.separator()
            
            row = layout.row()
            row.prop(mat, "shaderset")
            name = GetShaderSetName(mat.shaderset)
            if name == 'water_plane':
              row = layout.row()
              row.prop(mat, "water_fog_color")
              row = layout.row()
              row.prop(mat, "water_perturb_scale")
              row = layout.row()
              row.prop(mat, "water_fog_density")


EnumProperty(['Material'], attr="shaderset", name="ShaderSet", description="",
  items=SHADERSETS,
  default="DEFAULT")


EnumProperty(['Material'], attr="priority", name="Render priority",
     description="Priority level in which the object will be renderered", 
     items=[('init','init',''),('sky','sky',''),('sky2','sky2',''),
            ('portal','portal',''),('wall','wall',''),('wall2','wall2',''),('object','object',''),
            ('object2','object2',''),('transp','transp',''),('alpha','alpha',''),('final','final','')],
     default='object')

EnumProperty(['Material'], attr="zbuf_mode", name="Z-buffer mode",
     description="Behavior of the rendering of the object regarding the Z-Buffer",
     items=[('znone','Z-None',"Don't test or write"),
            ('zfill','Z-Fill',"Write unconditionally"),
            ('ztest','Z-Test',"Test only"),
            ('zuse','Z-Use',"Test, write if successful")],
     default='zuse')

StringProperty(['Material'], attr="water_fog_color",
        name="Water Fog Color",
        description="Fog color inside the water surface", 
        default='0,0.1,0.15,1')

FloatProperty(['Material'], attr="water_fog_density",
        name="Water Fog Density",
        description="Fog density inside the water surface", 
        default=3.0)

StringProperty(['Material'], attr="water_perturb_scale",
        name="Water Perturb Scale",
        description="Fog perturb scale", 
        default='0.9,0.9,0,0')

BoolProperty(['Material'], 
             attr="csMatRef", 
             name="Material replaced by a CS material", 
             description="Replace this material by a Crystal Space material", 
             default=False)

StringProperty(['Material'], 
               attr="csMaterialName", 
               name="Reference of CS material", 
               description="Name of an existing Crystal Space material",
               default='None')

StringProperty(['Material'], 
               attr="csMaterialVfs", 
               name="VFS path of CS material", 
               description="VFS path of a Crystal Space library file",
               default='')
