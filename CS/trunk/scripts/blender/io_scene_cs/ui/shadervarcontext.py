import bpy

from io_scene_cs.utilities import rnaType

    
class csShaderVarContextPanel():
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"
    bl_label = "Crystal Space Shader"
    bl_label = ""

    @classmethod
    def poll(cls, context): 
        ob = bpy.context.active_object
        r = ob and ob.type == 'MESH' and ob.data and ob.data.cstype != "none"
        return r


@rnaType
class OBJECT_PT_csShaderVarContext(csShaderVarContextPanel, bpy.types.Panel):
    bl_label = "Crystal Space Shader"

    def draw(self, context):
        layout = self.layout
        
        ob = bpy.context.active_object

