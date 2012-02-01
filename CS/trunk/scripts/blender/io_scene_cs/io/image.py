import bpy

from .util import *
from io_scene_cs.utilities import BoolProperty


BoolProperty(['Image'], attr="isNormalMap", name="isNormalMap", default=False)

def IdentifyNormalMap(self, material):
  for slot in material.texture_slots:
    if slot and slot.use_map_normal and slot.texture and slot.texture.type =='IMAGE' \
          and slot.texture.image and slot.texture.image.uname == self.uname :
      self.isNormalMap = True   
      return True
  self.isNormalMap = False
  return False

bpy.types.Image.IdentifyNormalMap = IdentifyNormalMap


def TextureAsCS(self, func, depth=0, **kwargs):
  path = 'textures/'
  if 'path' in kwargs:
    path = kwargs['path']
  if 'material' in kwargs:
    material = kwargs['material']
    self.IdentifyNormalMap(material)

  func(' '*depth +'<texture name="%s">'%(self.uname))
  func(' '*depth +'  <file>'+path+'%s</file>'%(self.ufilename))
  if self.isNormalMap:
    func(' '*depth +'  <class>normalmap</class>')    
  if self.IsBinAlpha():
    func(' '*depth +'  <alpha><binary /></alpha>')
  func(' '*depth +'</texture>')

bpy.types.Image.AsCS = TextureAsCS


def TextureIsBinAlpha(self):
  return self.type == 'IMAGE' and self.binAlpha
 
bpy.types.Image.IsBinAlpha = TextureIsBinAlpha
