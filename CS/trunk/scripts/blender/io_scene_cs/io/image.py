import bpy

from .util import *

def IdentifyNormalMap(self, material):
  """ Test if this texture is a normal map
  """
  self["isNormalMap"] = False
  for slot in material.texture_slots:
    if slot and slot.use_map_normal and slot.texture and slot.texture.type =='IMAGE' \
          and slot.texture.image and slot.texture.image.uname == self.uname :
      self["isNormalMap"] = True   
  return self["isNormalMap"]

bpy.types.Image.IdentifyNormalMap = IdentifyNormalMap

bpy.types.Image.isNormalMap = property(lambda self: self.get("isNormalMap", False))


def TextureIsBinAlpha(self):
  return self.type == 'IMAGE' and self.b2cs.binAlpha
 
bpy.types.Image.IsBinAlpha = TextureIsBinAlpha


def TextureAsCS(self, func, depth=0, **kwargs):
  """ Write an xml description of this texture 
  """
  path = kwargs.get('path', 'textures/')

  func(' '*depth +'<texture name="%s">'%(self.uname))
  func(' '*depth +'  <file>'+path+'%s</file>'%(self.ufilename))
  if self.isNormalMap:
    func(' '*depth +'  <class>normalmap</class>')    
  if self.IsBinAlpha():
    func(' '*depth +'  <alpha><binary /></alpha>')
  func(' '*depth +'</texture>')

bpy.types.Image.AsCS = TextureAsCS
