import bpy 
from bpy.app.handlers import persistent
import time

def getCurrentMilliSeconds():
    return time.time()*1000

@persistent       
def post_ob_updated(scene): 
    '''
    Triggers on:
    - material editting
    '''
    ob = scene.objects.active 
    if ob is not None and ob.is_updated: 
        ob.data['changeToken'] = getCurrentMilliSeconds() #Flag data as changed so material changes get re-exported
        print("%s - Object is_updated (post)" % ob.name) 

@persistent        
def post_ob_updated_data(scene): 
    '''
    Triggers on:
    - mesh data editting
    '''
    ob = scene.objects.active 
    if ob is not None and ob.is_updated_data:
        ob.data['changeToken'] = getCurrentMilliSeconds()
        print("%s - Object is_updated_data (post)" % ob.name)  
        print("  %s(%d)" % (ob.data.name, ob.data['changeToken']))  

@persistent
def post_ob_data_updated(scene): 
    ob = scene.objects.active 
    if ob is not None and ob.data.is_updated: 
        print("%s - Object data is_updated (post)" % ob.data.name) 

@persistent        
def post_ob_data_updated_data(scene): 
    ob = scene.objects.active 
    if ob is not None and ob.data.is_updated_data: 
        print("%s - Object data is_updated_data (post)" % ob.data.name) 


bpy.app.handlers.scene_update_post.clear() 
bpy.app.handlers.scene_update_post.clear() 
bpy.app.handlers.scene_update_post.clear() 
bpy.app.handlers.scene_update_post.clear() 

bpy.app.handlers.scene_update_post.append(post_ob_updated) 
bpy.app.handlers.scene_update_post.append(post_ob_updated_data) 
bpy.app.handlers.scene_update_post.append(post_ob_data_updated) 
bpy.app.handlers.scene_update_post.append(post_ob_data_updated_data)
