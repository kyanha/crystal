import bpy

import os
import shutil


def EmptyDependencies():
  return {'T':{}, 'M':{}, 'A':{}, 'F':{}, 'G':{}, }

  
def MergeDependencies(dep1, dep2):
  for typ in dep1:
    dep1[typ].update(dep2[typ])


def Join(*args):
  path =  os.path.join(*args)
  return path.replace('\\', '/')
  
UNIQUE_NAMES = {'None': '', }
def GetUniqueName(obj):
  library = obj.library.filepath if obj.library else None
  if str(library) not in UNIQUE_NAMES:
    UNIQUE_NAMES[str(library)] = str(len(UNIQUE_NAMES))+'-'
  return bpy.path.clean_name(UNIQUE_NAMES[str(library)] + obj.name)

UNIQUE_FILENAMES = {}
def GetUniqueFileName(obj):
  path, fileName = os.path.split(bpy.path.abspath(obj.filepath))
  if fileName not in UNIQUE_FILENAMES:
    UNIQUE_FILENAMES[fileName] = {path:'',}
  elif path not in UNIQUE_FILENAMES[fileName]:
      UNIQUE_FILENAMES[fileName][path] = str(len(UNIQUE_FILENAMES[fileName]))+'-'
  return UNIQUE_FILENAMES[fileName][path] + fileName

#Absolute path is unique.
bpy.types.Image.uname = property(GetUniqueName)
bpy.types.Image.ufilename = property(GetUniqueFileName)
bpy.types.Material.uname = property(GetUniqueName)
bpy.types.Mesh.uname = property(GetUniqueName)
bpy.types.Object.uname = property(GetUniqueName)  
bpy.types.Group.uname = property(GetUniqueName)
bpy.types.Scene.uname = property(GetUniqueName)

# Some support code
def GetTextures(self):
  return [t.texture for t in self.texture_slots if t and t.texture]

bpy.types.Material.textures = property(GetTextures)

def GetMaterials(self):
  return [m.material for m in self.material_slots if m and m.material]

bpy.types.Mesh.materials = property(GetMaterials)
bpy.types.Object.materials = property(GetMaterials)

def SaveImage(self, path):
  dst = Join(path, 'textures/', self.ufilename)
  print("INFO: saving image", self.name, "to", dst, "...")
  try:
    if self.packed_file:
      self.save_render(dst)
    else:
      library = os.path.split(bpy.path.abspath(self.library.filepath))[0] if self.library else None
      if library != None and self.filepath.startswith('//'):
        src = os.path.join(library, self.filepath[2:])
      else:
        src = bpy.path.abspath(self.filepath) 
      shutil.copyfile(src, dst)
    print("INFO: Done.")
    return True
  except IOError:
    print("WARNING: couldn't copy image %s!"%(src))
  return False

bpy.types.Image.save_export = SaveImage

def AbsPathImage(self):
  return os.path.normpath(bpy.path.abspath(self.filepath))

bpy.types.Image.absfilepath = property(AbsPathImage)

# Utility method to search in a list
def find (f, seq):
  ''' Return index of first item in sequence where f(item) == True. '''
  for i,item in enumerate(seq):
    if f(item): 
      return i

# Utility method to decompose a matrix
def DecomposeMatrix (matrix):
  ''' Return the location, rotation and scale parts of a transformation matrix '''
  loc, quat, scale = matrix.decompose()
  # transform quaternion to a 4x4 rotation matrix
  rot = quat.to_matrix().to_4x4()
  return loc, rot, scale
