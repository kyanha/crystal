#!/usr/bin/env python2

import types, string, re

try:
    from cspace import *
except:
    print "WARNING: Failed to import module cspace"
    import traceback
    traceback.print_exc()

print 'imported cspace'

def CreateRoom(matname):
    print 'Start creating polygons from Python script...'
    engine = CS_QUERY_REGISTRY(object_reg, iEngine)
    room = engine.GetSectors().FindByName("room")
    walls = engine.CreateSectorWallsMesh(room,"walls")
    thingstate = SCF_QUERY_INTERFACE(walls.GetMeshObject(), iThingState)
    material=engine.GetMaterialList().FindByName(matname)

    poly=thingstate.GetFactory().CreatePolygon('floor')
    poly.CreateVertex(csVector3(-5,0,5))
    poly.CreateVertex(csVector3(5,0,5))
    poly.CreateVertex(csVector3(5,0,-5))
    poly.CreateVertex(csVector3(-5,0,-5))
    poly.SetTextureSpace(poly.GetVertex(0), poly.GetVertex(1), 3)
    poly.SetMaterial(material)

    poly=thingstate.GetFactory().CreatePolygon('ceiling')
    poly.CreateVertex(csVector3(-5,20,-5))
    poly.CreateVertex(csVector3(5,20,-5))
    poly.CreateVertex(csVector3(5,20,5))
    poly.CreateVertex(csVector3(-5,20,5))
    poly.SetTextureSpace(poly.GetVertex(0), poly.GetVertex(1), 3)
    poly.SetMaterial(material)

    poly=thingstate.GetFactory().CreatePolygon('w1')
    poly.CreateVertex(csVector3(-5,20,5))
    poly.CreateVertex(csVector3(5,20,5))
    poly.CreateVertex(csVector3(5,0,5))
    poly.CreateVertex(csVector3(-5,0,5))
    poly.SetTextureSpace(poly.GetVertex(0), poly.GetVertex(1), 3)
    poly.SetMaterial(material)

    poly=thingstate.GetFactory().CreatePolygon('w2')
    poly.CreateVertex(csVector3(5,20,5))
    poly.CreateVertex(csVector3(5,20,-5))
    poly.CreateVertex(csVector3(5,0,-5))
    poly.CreateVertex(csVector3(5,0,5))
    poly.SetTextureSpace(poly.GetVertex(0), poly.GetVertex(1), 3)
    poly.SetMaterial(material)

    poly=thingstate.GetFactory().CreatePolygon('w3')
    poly.CreateVertex(csVector3(-5,20,-5))
    poly.CreateVertex(csVector3(-5,20,5))
    poly.CreateVertex(csVector3(-5,0,5))
    poly.CreateVertex(csVector3(-5,0,-5))
    poly.SetTextureSpace(poly.GetVertex(0), poly.GetVertex(1), 3)
    poly.SetMaterial(material)

    poly=thingstate.GetFactory().CreatePolygon('w4')
    poly.CreateVertex(csVector3(5,20,-5))
    poly.CreateVertex(csVector3(-5,20,-5))
    poly.CreateVertex(csVector3(-5,0,-5))
    poly.CreateVertex(csVector3(5,0,-5))
    poly.SetTextureSpace(poly.GetVertex(0), poly.GetVertex(1), 3)
    poly.SetMaterial(material)
    print 'Finished!'

