import os

import bpy

#from io_scene_cs import B2CS

from io_scene_cs.utilities import rnaType, rnaOperator, B2CS

from io_scene_cs.utilities import RemovePanels, RestorePanels 

from io_scene_cs.io import Export


class csSettingsPanel():
  bl_space_type = "PROPERTIES"
  bl_region_type = "WINDOW"
  bl_context = "render"
  b2cs_context = "render"
  bl_label = ""
  REMOVED = []

  @classmethod
  def poll(cls, context):
    return True
 
   
@rnaType    
class B2CS_OT_export(bpy.types.Operator):
  "Export"
  bl_idname = "io_scene_cs.export"
  bl_label = "Export"

  def execute(self, context): 
    
    Export(B2CS.properties.exportPath)

    return {'FINISHED'}

def WalkTestPath():
  if os.name == 'nt':
    walktest = "walktest.exe"
  else:
    walktest = "walktest"
  return os.path.join(os.environ.get("CRYSTAL"), walktest).replace('\\', '/')
            
def HasCrystalSpace():    
  return os.path.exists(WalkTestPath())


@rnaType    
class B2CS_OT_export_run(bpy.types.Operator):
  "Export and Run"
  bl_idname = "io_scene_cs.export_run"
  bl_label = "Export and Run."

  def execute(self, context): 
    
    path = B2CS.properties.exportPath.replace('\\', '/')
    
    Export(path)
    
    options = " "
    if B2CS.properties.console:
      options += "-console "
    if B2CS.properties.verbose:
      options += "-verbose=-scf "
    if B2CS.properties.silent:
      options += "-silent "
      
    #import commands  
    #output = commands.getstatusoutput(WalkTestPath() + options + path)
    #print(output)
    
    import shlex, subprocess
    print(WalkTestPath())
    args = shlex.split(WalkTestPath() + options + path)
    print(args)
    output = subprocess.call(args)
    print(output)

    return {'FINISHED'}
      

@rnaType
class RENDER_PT_csSettingsPanel(csSettingsPanel, bpy.types.Panel):
  bl_label = "Crystal Space scene"
  bl_options = {'HIDE_HEADER'}
  
  def draw(self, context):
    layout = self.layout
    
    row = layout.row()
    
    row = layout.row()
    #row.prop(io_scene_cs.B2CS.properties, "exportPath")
    row.prop(B2CS.properties, "exportPath")
      
    row = layout.row()
    row.operator("io_scene_cs.export", text="Export")
      
    row = layout.row()
    if HasCrystalSpace():
      row.operator("io_scene_cs.export_run", text="Export and Run")
      row = layout.row()
      row.prop(B2CS.properties, "console")
      row = layout.row()
      row.prop(B2CS.properties, "verbose")
      row = layout.row()
      row.prop(B2CS.properties, "silent")
    else:
      row.label(text="Walktest isn't available!")
      
        
default_path = os.environ.get("TEMP")
if not default_path:
  if os.name == 'nt':
    default_path = "c:/tmp/"
  else:
    default_path = "/tmp/"
elif not default_path.endswith(os.sep):
  default_path += os.sep
            
B2CS.StringProperty( attr="exportPath",
        name="Export path",
        description="Export path", default=default_path, subtype='DIR_PATH')
        
B2CS.BoolProperty( attr="console",
        name="console",
        description="Enable console", default=True)
        
B2CS.BoolProperty( attr="verbose",
        name="verbose",
        description="Enable verbose", default=True)
        
B2CS.BoolProperty( attr="silent",
        name="silent",
        description="Enable silent", default=True)
