import bpy

from io_scene_cs.utilities import rnaType, B2CS, BoolProperty, StringProperty


class csPortalPanel():
  bl_space_type = "PROPERTIES"
  bl_region_type = "WINDOW"
  bl_context = "data"
  b2cs_context = "data"
  bl_label = ""

  @classmethod
  def poll(cls, context):
    ob = bpy.context.active_object
    r = (ob and ob.type == 'MESH' and ob.data)
    return r
        

@rnaType
class MESH_PT_csPortal(csPortalPanel, bpy.types.Panel):
  bl_label = "Crystal Space Portal"

  def draw(self, context):    
    ob = context.active_object

    if ob.type == 'MESH':
      # Draw a checkbox to define current mesh object as a portal
      layout = self.layout
      row = layout.row()
      row.prop(ob.data, "portal")

      if ob.data.portal:
          # Get the user defined name of the target mesh
          row = layout.row()
          split = row.split(percentage=0.5)
          colL = split.column()
          colL.label(text='Destination mesh:', icon='MESH_DATA')
          colR = split.column()
          colR.prop(ob.data, "destinationPortal", text='')

          # Verify that the target mesh name corresponds 
          # to a mesh object of the world
          source = ob.data
          target = source.destinationPortal
          if target:
              targetIsValid = False
              for o in bpy.data.objects:
                  if o.name == target and o.type=='MESH':
                      targetIsValid = True
                      break
              # Write an error message if the given name is not valid
              if not targetIsValid:
                  row = colR.row()
                  row.label(text="ERROR: unknown mesh name")


BoolProperty(['Mesh'], 
             attr="portal", 
             name="Portal", 
             description="Whether or not this mesh is defined as a CS portal", 
             default=False)

StringProperty(['Mesh'], 
               attr="destinationPortal", 
               name="Destination portal", 
               description="Name of the destination mesh for this portal",
               default='')
