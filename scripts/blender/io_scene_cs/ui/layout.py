import bpy

def ObjectReferenceLayout(self, mesh, name=""):
  box = self.box()
  row = box.row()
  row.label(name)
  if mesh:
    row.context_pointer_set("current_viscullmesh", mesh)
    #row.label(mesh.name)
    row.operator_menu_enum("object.select_object_ref", "select_object", text=mesh.name)
    row.operator("object.remove_object_ref", text="", icon='X', emboss=False)
  else:
    row.operator_menu_enum("object.select_object_ref", "select_object")

bpy.types.UILayout.template_object_ref = ObjectReferenceLayout
