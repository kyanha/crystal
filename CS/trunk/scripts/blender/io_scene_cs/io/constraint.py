import bpy

def IsVisCullConstraint (self):
  return self.type=='COPY_TRANSFORMS' and self.name.startswith('viscull')

bpy.types.Constraint.IsVisCullConstraint = IsVisCullConstraint



