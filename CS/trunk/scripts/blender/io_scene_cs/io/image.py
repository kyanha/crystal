import bpy

from .util import *

def TextureAsCS(self, func, depth=0, **kwargs):
  path = 'textures/'
  if 'path' in kwargs:
    path = kwargs['path']
  func(' '*depth +'<texture name="%s">'%(self.uname))
  func(' '*depth +'  <file>'+path+'%s</file>'%(self.ufilename))
  #func(' '*depth +'  <class>normalmap/heightmap</class>')
  if self.IsBinAlpha():
    func(' '*depth +'  <alpha><binary /></alpha>')
  func(' '*depth +'</texture>')

bpy.types.Image.AsCS = TextureAsCS


def TextureIsBinAlpha(self):
  return self.type == 'IMAGE' and self.binAlpha
 
bpy.types.Image.IsBinAlpha = TextureIsBinAlpha
