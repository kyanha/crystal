import bpy

import os

from . import util

from . import transform

from . import image
from . import material

from . import renderbuffer
from . import submesh

from . import data
from . import object
from . import group
from . import scene


def Export(path):
  print("\nEXPORTING: "+util.Join(path, 'world')+" ====================================")

  scn = bpy.context.scene
  deps = scn.GetDependencies()

  # Create the export directories
  if not os.path.exists(util.Join(path, 'factories/')):
    os.makedirs(util.Join(path, 'factories/'))
  if not os.path.exists(util.Join(path, 'textures/')):
    os.makedirs(util.Join(path, 'textures/'))

  # Create a 'world' file containing the xml description of the scene
  f = open(util.Join(path, 'world'), 'w')
  def Write(fi):
    def write(data):
      fi.write(data+'\n')
    return write

  Write(f)('<world>')

  # Export the objects composing the scene
  for typ in deps:
    if typ == 'T':
      # Textures
      Write(f)('  <textures>')
      for name, tex in deps[typ].items():
        tex.AsCS(Write(f), 4)
      Write(f)('  </textures>')
    elif typ == 'M':
      # Materials
      Write(f)('  <materials>')
      for name, mat in deps[typ].items():
        mat.AsCS(Write(f), 4)
      Write(f)('  </materials>')
    elif typ == 'A':
      # Animated meshes
      for name, fact in deps[typ].items():
        if not fact.object.parent:
          print('\nEXPORT OBJECT "%s" AS A CS ANIMATED MESH\n'%(fact.object.name))
          print('Writing fact',fact.uname,':', util.Join(path, 'factories/', fact.object.name))
          fact.AsCSRef(Write(f), 2, 'factories/')
          # Export mesh factory
          fa = open(util.Join(path, 'factories/', fact.object.name), 'w')
          fact.AsCSLib(Write(fa), 2, path, True)
          fa.close()          
          # Export skeleton and animations
          if fact.object.type == 'ARMATURE':
            if fact.object.data.bones:
              ob = fact.object
              print('Exporting skeleton and animations:', util.Join(path, 'factories/', 'skel%s_rig'%(ob.name)))
              fb = open(util.Join(path, 'factories/', 'skel%s_rig'%(ob.name)), 'w')
              ob.data.AsCSSkelAnim(Write(fb), 2, ob)
              fb.close()
    elif typ == 'F':
      # General meshes
      for name, fact in deps[typ].items():
        print('\nEXPORT OBJECT "%s" AS A CS GENERAL MESH\n'%(fact.object.name))
        print('Writing fact',fact.uname,':', util.Join(path, 'factories/', fact.object.name))
        fact.AsCSRef(Write(f), 2, 'factories/')
        fa = open(util.Join(path, 'factories/', fact.object.name), 'w')
        fact.AsCSLib(Write(fa), 2, path, False)
        fa.close()
    elif typ == 'G':
      # Groups of objects
      for name, group in deps[typ].items():
        print('\nEXPORT GROUP "%s" AS A CS GENERAL MESH\n'%(group.uname))
        print('Writing group', util.Join(path, 'factories/', group.uname))
        group.AsCSRef(Write(f), 2, 'factories/')
        fa = open(util.Join(path, 'factories/', group.uname), 'w')
        group.AsCSFactory(Write(fa), 2)
        fa.close()

  # Export the scene description in the 'world' file
  scn.AsCS(Write(f), 2)

  for ob in scn.objects:
    if ob.type == 'CAMERA':
      ob.AsCS(Write(f), 2)

  Write(f)('</world>')
  f.close()
  print("\nEXPORTING complete ==================================================")
