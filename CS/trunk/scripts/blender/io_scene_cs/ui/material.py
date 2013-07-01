
import bpy

from io_scene_cs.utilities import rnaType, B2CS, EnumProperty, StringProperty, FloatProperty, SHADERS, GetShaderName


class csMaterialPanel():
  bl_space_type = 'PROPERTIES'
  bl_region_type = 'WINDOW'
  bl_context = "material"
  b2cs_context = "material"
  
  @classmethod
  def poll(cls, context):
    r = (context.material or context.object)
    return r    


@rnaType
class MATERIAL_PT_B2CS__context_material(csMaterialPanel, bpy.types.Panel):
    bl_label = "Crystal Space Material"

    def draw(self, context):
        layout = self.layout

        mat = context.material

        if mat:   
          layout.separator()    
          row = layout.row()
          row.prop(mat, "depthwrite_step")
          row = layout.row()
          row.prop(mat, "ambient_step")
          row = layout.row()
          row.prop(mat, "diffuse_step")
          row = layout.row()
          row.prop(mat, "priority")
          row = layout.row()
          row.prop(mat, "zbuf_mode")
          name1 = GetShaderName(mat.ambient_step)
          name2 = GetShaderName(mat.diffuse_step)
          if name1 == 'reflect_water_plane' or \
                name2 == 'reflect_water_plane':
            row = layout.row()
            row.prop(mat, "water_fog_color")
            row = layout.row()
            row.prop(mat, "water_perturb_scale")
            row = layout.row()
            row.prop(mat, "water_fog_density")

       
        
EnumProperty(['Material'], attr="depthwrite_step", name="Depthwrite", description="",
  items=(("DEFAULT", "Default", "Default"),
        ("*null", "*null", "Shader with no effect.")),
  default="DEFAULT")
  
EnumProperty(['Material'], attr="ambient_step", name="Ambient", description="",
  items=SHADERS,
  default="DEFAULT")
  
EnumProperty(['Material'], attr="diffuse_step", name="Diffuse", description="",
  items=SHADERS,
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

