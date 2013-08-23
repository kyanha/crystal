import os

import bpy

from io_scene_cs.utilities import rnaType, rnaOperator, B2CS, GetExportPath
from io_scene_cs.io import Export


class csSettingsPanel():
  bl_space_type = "PROPERTIES"
  bl_region_type = "WINDOW"
  bl_context = "render"
  b2cs_context = "render"
  bl_label = ""

  @classmethod
  def poll(cls, context):
    return True
 
#=============== Crystal Space Export panel ====================

try:
  default_path = bpy.context.user_preferences.addons["io_scene_cs"].preferences.exportpath
except:
  default_path = os.environ.get("TEMP")
  if not default_path:
    if os.name == 'nt':
      default_path = "c:/tmp/"
    else:
      default_path = "/tmp/"
  elif not default_path.endswith(os.sep):
    default_path += os.sep

  
@rnaType    
class B2CS_OT_export(bpy.types.Operator):
  "Export"
  bl_idname = "io_scene_cs.export"
  bl_label = "Export"

  def execute(self, context): 
    
    try:
      exportPath = context.user_preferences.addons["io_scene_cs"].preferences.exportpath
    except:
      exportPath = B2CS.properties.exportPath
    Export(exportPath)

    return {'FINISHED'}

def WalkTestPath():
  if os.name == 'nt':
    walktest = "walktest.exe"
  else:
    walktest = "walktest"
    
  crystal_env = os.environ.get("CRYSTAL")
  if not crystal_env:
    print("Warning: CRYSTAL environment variable not set!")
    return None
  
  # Look either in the 'CRYSTAL' and 'CRYSTAL/bin' paths
  path = os.path.join(crystal_env, walktest).replace('\\', '/')
  if os.path.exists(path):
    return path
  return os.path.join(crystal_env, "bin", walktest).replace('\\', '/')
            
def HasCrystalSpace():
  path = WalkTestPath()    
  return path and os.path.exists(path)


@rnaType    
class B2CS_OT_export_run(bpy.types.Operator):
  "Export and Run"
  bl_idname = "io_scene_cs.export_run"
  bl_label = "Export and Run."

  def execute(self, context): 
    
    try:
      exportPath = context.user_preferences.addons["io_scene_cs"].preferences.exportpath
    except:
      exportPath = B2CS.properties.exportPath
    Export(exportPath)
    
    options = " "
    if B2CS.properties.console:
      options += "-console "
    if B2CS.properties.verbose:
      options += "-verbose=-scf "
    if B2CS.properties.silent:
      options += "-silent "
    if B2CS.properties.bugplug:
      options += "-plugin=bugplug "
     
    #import commands  
    #output = commands.getstatusoutput(WalkTestPath() + options + path)
    #print(output)
    
    import shlex, subprocess
    print(WalkTestPath())
    args = shlex.split(WalkTestPath() + options + exportPath)
    print(args)
    output = subprocess.call(args)
    print(output)

    return {'FINISHED'}
      

@rnaType
class RENDER_PT_csSettingsPanel(csSettingsPanel, bpy.types.Panel):
  bl_label = "Crystal Space Export"
  
  def draw(self, context):
    layout = self.layout
    
    row = layout.row()
    row.prop(B2CS.properties, "exportOnlyCurrentScene")
    
    row = layout.row()
    row.prop(B2CS.properties, "library")
    row = layout.row()
    if not B2CS.properties.library:
      row.prop(B2CS.properties, "sharedMaterial")
    row = layout.row()
    row.prop(B2CS.properties, "enableDoublesided")
    row = layout.row()
    try:
      row.prop(context.user_preferences.addons["io_scene_cs"].preferences, "exportpath")
    except:
      row.prop(B2CS.properties, "exportPath")

    row = layout.row()
    row.operator("io_scene_cs.export", text="Export")
      
    row = layout.row()
    if not B2CS.properties.library:
      if HasCrystalSpace():
        row.operator("io_scene_cs.export_run", text="Export and Run")
        row = layout.row()
        row.prop(B2CS.properties, "console")
        row.prop(B2CS.properties, "verbose")
        row.prop(B2CS.properties, "silent")
        row.prop(B2CS.properties, "bugplug")
      else:
        row.label(text="'walktest' isn't available!")
      

#=============== Crystal Space Factory Reference panel ======================

class csFactoryRef (bpy.types.PropertyGroup):
  name = bpy.props.StringProperty(name="Name Prop", default="Unknown", \
                description="Name of an existing Crystal Space factory")
  vfs  = bpy.props.StringProperty(name="VFS Prop", default=default_path, \
                description="VFS path of an existing Crystal Space library file")

bpy.utils.register_class(csFactoryRef)


@rnaOperator
class B2CS_OT_csAddFactory(bpy.types.Operator):
  bl_idname = "io_scene_cs.add_factory"
  bl_label = "Add factory reference"
  bl_description = "Define a reference to existing Crystal Space factory"

  def execute(self, context): 
    new_fact = B2CS.properties.FactoryRefs.add()

    return {'FINISHED'}

@rnaOperator
class B2CS_OT_csRemoveFactory(bpy.types.Operator):
  bl_idname = "io_scene_cs.rmv_factory"
  bl_label = "Remove factory reference"
  bl_description = "Remove a reference to existing CS factory"

  def execute(self, context): 
    for i,fact in enumerate(B2CS.properties.FactoryRefs):
      if fact.name == context.current_factory.name:
        B2CS.properties.FactoryRefs.remove(i)
        break

    return {'FINISHED'}


@rnaType
class RENDER_PT_csReferenceFactoryPanel(csSettingsPanel, bpy.types.Panel):
  bl_label = "Crystal Space Factory References"
  bl_options = {'DEFAULT_CLOSED'}

  def draw(self, context):
    layout = self.layout
    
    row = layout.row(align=True)
    row.operator("io_scene_cs.add_factory", text="Add factory reference")

    for fact in  B2CS.properties.FactoryRefs:
      col = layout.column()
      col.context_pointer_set("current_factory", fact)
      box = col.box()
      
      row = box.row()
      row.prop(fact, "name", text="Factory name")
      row.operator("io_scene_cs.rmv_factory", text="", icon='X', emboss=False)

      row = box.row()
      row.prop(fact, "vfs", text="VFS path")

#============= Crystal Space Material Reference panel ====================

class csMaterialRef (bpy.types.PropertyGroup):
  name = bpy.props.StringProperty(name="Name Prop", default="Unknown", \
                description="Name of an existing Crystal Space material")
  vfs  = bpy.props.StringProperty(name="VFS Prop", default=default_path, \
                description="VFS path of an existing Crystal Space library file")

bpy.utils.register_class(csMaterialRef)


@rnaOperator
class B2CS_OT_csAddMaterial(bpy.types.Operator):
  bl_idname = "io_scene_cs.add_material"
  bl_label = "Add material reference"
  bl_description = "Define a reference to existing Crystal Space material"

  def execute(self, context): 
    new_mat = B2CS.properties.MaterialRefs.add()

    return {'FINISHED'}

@rnaOperator
class B2CS_OT_csRemoveMaterial(bpy.types.Operator):
  bl_idname = "io_scene_cs.rmv_material"
  bl_label = "Remove material reference"
  bl_description = "Remove a reference to existing Crystal Space material"

  def execute(self, context): 
    for i,mat in enumerate(B2CS.properties.MaterialRefs):
      if mat.name == context.current_material.name:
        B2CS.properties.MaterialRefs.remove(i)
        break

    return {'FINISHED'}


@rnaType
class RENDER_PT_csReferenceMaterialPanel(csSettingsPanel, bpy.types.Panel):
  bl_label = "Crystal Space Material References"
  bl_options = {'DEFAULT_CLOSED'}

  def draw(self, context):
    layout = self.layout
    
    row = layout.row(align=True)
    row.operator("io_scene_cs.add_material", text="Add material reference")

    for mat in  B2CS.properties.MaterialRefs:
      col = layout.column()
      col.context_pointer_set("current_material", mat)
      box = col.box()
      
      row = box.row()
      row.prop(mat, "name", text="Material name")
      row.operator("io_scene_cs.rmv_material", text="", icon='X', emboss=False)

      row = box.row()
      row.prop(mat, "vfs", text="VFS path")



B2CS.StringProperty( attr="exportPath",
        name="Export path",
        description="Export path", 
        default=default_path, subtype='DIR_PATH')

B2CS.BoolProperty( attr="console",
        name="Console",
        description="Enable the '-console' flag of 'walktest'", 
        default=True)
        
B2CS.BoolProperty( attr="verbose",
        name="Verbose",
        description="Enable the '-verbose=-scf' flag of 'walktest'", 
        default=True)

B2CS.BoolProperty( attr="silent",
        name="Silent",
        description="Enable the '-silent' flag of 'walktest'", 
        default=True)

B2CS.BoolProperty( attr="bugplug",
        name="Bugplug",
        description="Enable bugplug in 'walktest'",
        default=False)

B2CS.BoolProperty( attr="library",
        name="Export as a CS library",
        description="Export all mesh factories in an unique CS library file", 
        default=False)

B2CS.BoolProperty( attr="enableDoublesided",
        name="Enable double sided meshes",
        description="Global enabling of the 'Double Sided' option for all meshes",
        default=False)

B2CS.BoolProperty( attr="sharedMaterial",
        name="Shared materials and textures",
        description="Define all textures and materials in the world file",
        default=True)

B2CS.CollectionProperty( attr="FactoryRefs",
        name="Collection of Crystal Space factory references",
        type=csFactoryRef,
        description="Define references to existing Crystal Space factories")

B2CS.CollectionProperty( attr="MaterialRefs",
        name="Collection of Crystal Space material references",
        type=csMaterialRef,
        description="Define references to existing Crystal Space materials")
        
        
B2CS.BoolProperty( attr="exportOnlyCurrentScene",
        name="Export only current scene",
        description="Export only current scene", 
        default=False)
