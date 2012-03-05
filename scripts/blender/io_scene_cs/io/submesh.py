from .util import *
from .renderbuffer import *


class SubMesh:
  def __init__(self, name, material, image, indices):
    self.name = name
    self.material = material
    self.image = image
    self.indices = indices

  class IndexBuffer(RenderBuffer):
    _name_ = "indices"
    _components_ = 1
    _type_ = "uint"

    def __init__(self, indices):
      self.indices = indices

    def Iter(self):
      for i, index in enumerate(self.indices):
        if i%3 == 0:
          if i!=0:
            yield faceIndices
          faceIndices = '<e c0="%d" />' % (index)
        else:
          faceIndices += ' <e c0="%d" />' % (index)
          if i==len(self.indices)-1:
            yield faceIndices

    def BeginTag(self, animesh):
      if animesh:
        return '<index type="%s" components="%d" indices="yes">' % (self._type_, self._components_)
      else:
        return '<indexbuffer indices="yes" checkelementcount="no" components="%d" type="%s">' % (self._components_, self._type_)

    def EndTag(self, animesh):
      if animesh:
        return '</index>'
      else:
        return '</indexbuffer>'

  def GetIndexBuffer(self):
    return SubMesh.IndexBuffer(self.indices)

  def AsCS(self, func, depth=0, animesh=False):
    if self.name:
      if self.material:
        self.name = '%s-%s'%(self.name,self.material.uname)
      if self.image:
        self.name = '%s-%s'%(self.name,self.image.uname)        
      func(' '*depth +'<submesh name="%s">'%(self.name))
    else:
      func(' '*depth +'<submesh>')

    if self.material:
      func(' '*depth +'  <material>'+self.material.uname+'</material>')
    if not animesh and self.image:
      if not self.material or (self.material and not self.material.HasDiffuseTexture()):
        func(' '*depth +'  <shadervar type="texture" name="tex diffuse">%s</shadervar>'%(self.image.uname))
    SubMesh.IndexBuffer(self.indices).AsCS(func, depth+2, animesh)
    func(' '*depth +'</submesh>')

  def GetDependencies(self):
    dependencies = EmptyDependencies()
    if self.material:
      MergeDependencies(dependencies, self.material.GetDependencies())
    if self.image:
      dependencies['T'][self.image.uname] = self.image
    return dependencies
