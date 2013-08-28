import bpy
from bpy.props import StringProperty, BoolProperty, CollectionProperty
from bpy.types import AddonPreferences
from .utilities import rnaType, GetDefaultPath, GetDefaultPath, settings, GetPreferences
from .ui.renderlayer import csFactoryRef, csMaterialRef
from bpy.types import PropertyGroup


class ExportCSPreferences(AddonPreferences):
  bl_idname = __package__
  
  def draw(self, context):
    layout = self.layout
    layout.label(text="This is a preferences view for our addon")
    layout.prop(GetPreferences(), "exportPath")


@settings(type='Text', attribute='preferences')
class CrystalSpaceSettingsGlobal(PropertyGroup):
  exportPath = StringProperty(
          name = "Export Path",
          default = GetDefaultPath(),
          subtype ='DIR_PATH',
          )
  
  console = BoolProperty(
          name = "Console",
          description="Enable the '-console' flag of 'walktest'", 
          default = True,
          )
          
  verbose = BoolProperty(
          name = "Verbose",
          description="Enable the '-verbose=-scf' flag of 'walktest'", 
          default = True,
          )
          
  silent = BoolProperty(
          name = "Silent",
          description="Enable the '-silent' flag of 'walktest'", 
          default = True,
          )
          
  bugplug = BoolProperty(
          name = "Bugplug",
          description="Enable bugplug in 'walktest'", 
          default = False,
          )
          
  library = BoolProperty(
          name = "Export as a CS library",
          description="Export all mesh factories in an unique CS library file", 
          default = False,
          )
          
  enableDoublesided = BoolProperty(
          name = "Enable double sided meshes",
          description="Global enabling of the 'Double Sided' option for all meshes", 
          default = False,
          )
          
  exportOnlyCurrentScene = BoolProperty(
          name = "Export only current scene",
          description="Export only current scene", 
          default = False,
          )
          
  sharedMaterial = BoolProperty(
          name = "Shared materials and textures",
          description="Define all textures and materials in the world file", 
          default = True,
          )
          
  FactoryRefs = CollectionProperty(
          name = "Collection of Crystal Space factory references",
          description="Define references to existing Crystal Space factories", 
          type=csFactoryRef,
          )
          
  MaterialRefs = CollectionProperty(
          name = "Collection of Crystal Space material references",
          description="Define references to existing Crystal Space materials", 
          type=csMaterialRef,
          )

  



