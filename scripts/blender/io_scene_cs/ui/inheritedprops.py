import bpy

from io_scene_cs.utilities import BoolProperty

BoolProperty(['Mesh','Object'], attr="goodCollider", name="Good Collider", description="")
BoolProperty(['Mesh','Object'], attr="badCollider", name="Bad Collider", description="")