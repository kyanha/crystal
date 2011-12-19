/*
  Copyright (C) 2008 by Marten Svanfeldt

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"
#include "animation.h"

#include "nodes.h"
#include "blendnode.h"
#include "fsmnode.h"
#include "othernodes.h"

#include "csgeom/math.h"

using namespace CS::Animation;

CS_PLUGIN_NAMESPACE_BEGIN(Skeleton2)
{
  CS_LEAKGUARD_IMPLEMENT(AnimationPacketFactory);
  
  AnimationPacketFactory::AnimationPacketFactory (const char* name)
    : scfImplementationType (this), name (name)
  {
  }

  csPtr<CS::Animation::iSkeletonAnimPacket> AnimationPacketFactory::CreateInstance (
    CS::Animation::iSkeleton* skeleton)
  {
    csRef<AnimationPacket> newP;
    newP.AttachNew (new AnimationPacket (this));

    // Setup the root
    if (animRoot)
      newP->animRoot = animRoot->CreateInstance (newP, skeleton);

    return csPtr<CS::Animation::iSkeletonAnimPacket> (newP);
  }

  const char* AnimationPacketFactory::GetName () const
  {
    return name;
  }

  CS::Animation::iSkeletonAnimation* AnimationPacketFactory::CreateAnimation (
    const char* name)
  {
    csRef<Animation> newFact;
    newFact.AttachNew(new Animation (name));

    animationList.Push (newFact);

    return newFact;
  }

  CS::Animation::iSkeletonAnimation* AnimationPacketFactory::FindAnimation (
    const char* name)
  {
    for (size_t i = 0; i < animationList.GetSize (); ++i)
    {
      if (strcmp (animationList[i]->GetName (), name) == 0)
        return animationList[i];
    }

    return 0;
  }

  size_t AnimationPacketFactory::FindAnimationIndex (
    const char* name)
  {
    for (size_t i = 0; i < animationList.GetSize (); ++i)
    {
      if (strcmp (animationList[i]->GetName (), name) == 0)
        return i;
    }

    return (size_t) -1;
  }

  void AnimationPacketFactory::RemoveAnimation (const char* name)
  {
    for (size_t i = 0; i < animationList.GetSize (); ++i)
      if (strcmp (animationList[i]->GetName (), name) == 0)
      {
	animationList.DeleteIndex (i);
	return;
      }
  }

  void AnimationPacketFactory::RemoveAnimation (size_t index)
  {
    animationList.DeleteIndex (index);
  }

  void AnimationPacketFactory::ClearAnimations ()
  {
    animationList.DeleteAll ();
  }

  CS::Animation::iSkeletonAnimation* AnimationPacketFactory::GetAnimation (size_t index)
  {
    return animationList[index];
  }

  size_t AnimationPacketFactory::GetAnimationCount () const
  {
    return animationList.GetSize ();
  }

  void AnimationPacketFactory::SetAnimationRoot (CS::Animation::iSkeletonAnimNodeFactory* root)
  {
    animRoot = root;
  }

  CS::Animation::iSkeletonAnimNodeFactory* AnimationPacketFactory::GetAnimationRoot () const
  {
    return animRoot;
  }

  csPtr<CS::Animation::iSkeletonAnimationNodeFactory> AnimationPacketFactory::CreateAnimationNode (
    const char* name)
  {
    csRef<CS::Animation::iSkeletonAnimationNodeFactory> ref;
    ref.AttachNew (new AnimationNodeFactory (name));
    return csPtr<CS::Animation::iSkeletonAnimationNodeFactory> (ref);
  }
  
  csPtr<CS::Animation::iSkeletonBlendNodeFactory> AnimationPacketFactory::CreateBlendNode (
    const char* name)
  {
    csRef<CS::Animation::iSkeletonBlendNodeFactory> ref;
    ref.AttachNew (new BlendNodeFactory (name));
    return csPtr<CS::Animation::iSkeletonBlendNodeFactory> (ref);
  }

  csPtr<CS::Animation::iSkeletonPriorityNodeFactory> AnimationPacketFactory::CreatePriorityNode (
    const char* name)
  {
    csRef<CS::Animation::iSkeletonPriorityNodeFactory> ref;
    ref.AttachNew (new PriorityNodeFactory (name));
    return csPtr<CS::Animation::iSkeletonPriorityNodeFactory> (ref);
  }

  csPtr<CS::Animation::iSkeletonRandomNodeFactory> AnimationPacketFactory::CreateRandomNode (
    const char* name)
  {
    csRef<CS::Animation::iSkeletonRandomNodeFactory> ref;
    ref.AttachNew (new RandomNodeFactory (name));
    return csPtr<CS::Animation::iSkeletonRandomNodeFactory> (ref);
  }

  csPtr<CS::Animation::iSkeletonFSMNodeFactory> AnimationPacketFactory::CreateFSMNode (
    const char* name)
  {
    csRef<CS::Animation::iSkeletonFSMNodeFactory> ref;
    ref.AttachNew (new FSMNodeFactory (name));
    return csPtr<CS::Animation::iSkeletonFSMNodeFactory> (ref);
  }


  CS_LEAKGUARD_IMPLEMENT(AnimationPacket);

  AnimationPacket::AnimationPacket (AnimationPacketFactory* factory)
    : scfImplementationType (this), factory (factory)
  {    
  }
  
  CS::Animation::iSkeletonAnimPacketFactory* AnimationPacket::GetFactory () const
  {
    return factory;
  }

  CS::Animation::iSkeletonAnimNode* AnimationPacket::GetAnimationRoot () const
  {
    return animRoot;
  }
  


  CS_LEAKGUARD_IMPLEMENT(Animation);

  Animation::Animation (const char* name)
    : scfImplementationType (this), name (name), duration (0),
    isBindSpace (false)
  {
  }

  const char* Animation::GetName () const
  {
    return name;
  }

  ChannelID Animation::AddChannel (CS::Animation::BoneID bone)
  {
    // Check if there is any channel with this bone before
    ChannelID channelId = FindChannel (bone);
    
    if (channelId == InvalidChannelID)
    {
      AnimationChannel* channel = new AnimationChannel (bone);

      channelId = (ChannelID)channels.Push (channel);
    }

    return channelId;
  }

  void Animation::RemoveChannel (CS::Animation::ChannelID channel)
  {
    CS_ASSERT(channel < channels.GetSize ());
    channels.DeleteIndex (channel);
  }

  ChannelID Animation::FindChannel (CS::Animation::BoneID bone) const
  {
    for (size_t i = 0; i < channels.GetSize (); ++i)
    {
      if (channels[i]->bone == bone)
        return (ChannelID)i;
    }

    return InvalidChannelID;
  }

  size_t Animation::GetChannelCount () const
  {
    return channels.GetSize ();
  }

  CS::Animation::BoneID Animation::GetChannelBone (CS::Animation::ChannelID channel) const
  {
    CS_ASSERT(channel < channels.GetSize ());

    AnimationChannel* ch = channels[channel];
    return ch->bone;
  }

  void Animation::SetChannelBone (CS::Animation::ChannelID channel,
				  CS::Animation::BoneID bone)
  {
    CS_ASSERT(channel < channels.GetSize ());

    AnimationChannel* ch = channels[channel];
    ch->bone = bone;
  }

  void Animation::AddKeyFrame (ChannelID channel, float time, 
    const csQuaternion& rotation, const csVector3& offset)
  {
    CS_ASSERT(channel < channels.GetSize ());

    AnimationChannel* ch = channels[channel];
    
    KeyFrame k;
    k.time = time;
    k.rotation = rotation;
    k.offset = offset;

    if (time > duration)
      duration = time;

    ch->keyFrames.InsertSorted (k, KeyFrameCompare);
  }

  void Animation::SetKeyFrame (CS::Animation::ChannelID channel, 
			       CS::Animation::KeyFrameID keyframe,
			       const csQuaternion& rotation,
			       const csVector3& offset)
  {
    CS_ASSERT(channel < channels.GetSize ());

    AnimationChannel* ch = channels[channel];

    CS_ASSERT(keyframe < ch->keyFrames.GetSize ());

    KeyFrame& k = ch->keyFrames[keyframe];
    k.rotation = rotation;
    k.offset = offset;
  }

  void Animation::AddOrSetKeyFrame (ChannelID channel, float time, 
				    const csQuaternion& rotation)
  {
    CS_ASSERT(channel < channels.GetSize ());

    AnimationChannel* ch = channels[channel];

    // Search for any keyframe with that time value
    for (size_t i = 0; i < ch->keyFrames.GetSize (); i++)
      if (fabs (ch->keyFrames[i].time - time) < SMALL_EPSILON)
      {
	ch->keyFrames[i].rotation = rotation;
	return;
      }

    // Insert the new key
    AddKeyFrame (channel, time, rotation, csVector3 (0.0f));
  }

  void Animation::AddOrSetKeyFrame (ChannelID channel, float time, 
				    const csVector3& offset)
  {
    CS_ASSERT(channel < channels.GetSize ());

    AnimationChannel* ch = channels[channel];

    // Search for any keyframe with that time value
    for (size_t i = 0; i < ch->keyFrames.GetSize (); i++)
      if (fabs (ch->keyFrames[i].time - time) < SMALL_EPSILON)
      {
	ch->keyFrames[i].offset = offset;
	return;
      }

    // Insert the new key
    AddKeyFrame (channel, time, csQuaternion (), offset);
  }

  size_t Animation::GetKeyFrameCount (ChannelID channel) const
  {
    CS_ASSERT(channel < channels.GetSize ());

    return channels[channel]->keyFrames.GetSize ();
  }

  void Animation::GetKeyFrame (ChannelID channel, KeyFrameID keyframe, CS::Animation::BoneID& bone,
    float& time, csQuaternion& rotation, csVector3& offset)
  {
    CS_ASSERT(channel < channels.GetSize ());

    AnimationChannel* ch = channels[channel];

    CS_ASSERT(keyframe < ch->keyFrames.GetSize ());

    const KeyFrame& k = ch->keyFrames[keyframe];
    bone = ch->bone;
    time = k.time;
    rotation = k.rotation;
    offset = k.offset;
  }

  void Animation::GetTwoKeyFrames (ChannelID channel, float time, CS::Animation::BoneID& bone,
    float& timeBefore, csQuaternion& beforeRot, csVector3& beforeOffset,
    float& timeAfter, csQuaternion& afterRot, csVector3& afterOffset)
  {
    CS_ASSERT(channel < channels.GetSize ());

    AnimationChannel* ch = channels[channel];

    // TODO: this is not correct anymore
    // Find last key with key.time <= time
    size_t before, after;
    csArrayCmp<KeyFrame, float> cmp (time, KeyFrameTimeCompare);
    size_t index = ch->keyFrames.FindSortedKey (cmp, &before);

    // Exact match
    if (index != csArrayItemNotFound)
    {
      before = index;
    }

    after = before+1;
    //@@TODO: Maybe setting for this?
    if (after == ch->keyFrames.GetSize ())
    {
      after = 0;
    }

    const KeyFrame& k1 = ch->keyFrames[before];
    const KeyFrame& k2 = ch->keyFrames[after];

    bone = ch->bone;
    timeBefore = k1.time;
    beforeRot = k1.rotation;
    beforeOffset = k1.offset;

    timeAfter = k2.time;
    afterRot = k2.rotation;
    afterOffset = k2.offset;
  }  

  // Apply a specific keyframe
  inline void ApplySingleKey (CS::Animation::AnimatedMeshState* state,
			      CS::Animation::BoneID boneID,
			      Animation::KeyFrame k1,
			      float baseWeight)
  {
    csQuaternion& q = state->GetQuaternion (boneID);
    csVector3& v = state->GetVector (boneID);
    q = q.SLerp (k1.rotation, baseWeight);
    v = csLerp (v, k1.offset, baseWeight);
    state->SetBoneUsed (boneID);
  }

  // Apply the interpolation between two keyframes
  inline void ApplyInterpolatedKeys (CS::Animation::AnimatedMeshState* state,
				     CS::Animation::BoneID boneID,
				     Animation::KeyFrame k1,
				     Animation::KeyFrame k2,
				     float time1, float time2,
				     float playbackTime,
				     float baseWeight)
  {
    // Blend between the two keyframes
    const float t = (playbackTime - time1) / (time2 - time1);
    csQuaternion qResult = k1.rotation.SLerp (k2.rotation, t);
    csVector3 vResult = csLerp (k1.offset, k2.offset, t);

    csQuaternion& q = state->GetQuaternion (boneID);
    csVector3& v = state->GetVector (boneID);
    q = q.SLerp (qResult, baseWeight);
    v = csLerp (v, vResult, baseWeight);
    state->SetBoneUsed (boneID);
  }

  void Animation::BlendState (CS::Animation::AnimatedMeshState* state, 
			      float baseWeight, float playbackTime) const
  {
    csArrayCmp<KeyFrame, float> cmp (playbackTime, KeyFrameTimeCompare);

    for (size_t c = 0; c < channels.GetSize (); ++c)
    {
      AnimationChannel* channel = channels[c];

      // Find the keyframes corresponding to the playback time

      // If there is no keyframe then do nothing
      if (channel->keyFrames.GetSize () == 0)
	continue;

      // If there is only one keyframe then apply it
      if (channel->keyFrames.GetSize () == 1)
      {
	ApplySingleKey (state, channel->bone,
			channel->keyFrames[0], baseWeight);
	continue;
      }

      // Search for the first keyframe
      size_t keyIndex, result;
      // TODO: a more optimized data structure/scheme for finding keyframes may be interesting,
      // eg by starting from the last found key...
      result = channel->keyFrames.FindSortedKey (cmp, &keyIndex);

      // If the playback time is exactly at a keyframe then apply it
      if (result != csArrayItemNotFound)
      {
	ApplySingleKey (state, channel->bone,
			channel->keyFrames[result],
			baseWeight);
	continue;
      }

      // Check that the playback time found by FindSortedKey is really after the keyframe that
      // was found
      if (keyIndex < channel->keyFrames.GetSize ()
	  && channel->keyFrames[keyIndex].time > playbackTime
	  && keyIndex > 0)
	keyIndex--;

      // If we are at or after the last keyframe then blend with the first keyframe
      if (keyIndex >= channel->keyFrames.GetSize () - 1)
      {
	const KeyFrame& k1 = channel->keyFrames[channel->keyFrames.GetSize () - 1];
	const KeyFrame& k2 = channel->keyFrames[0];
	ApplyInterpolatedKeys (state, channel->bone,
			       k1, k2, k1.time, duration + k2.time,
			       playbackTime, baseWeight);
	continue;
      }

      // Check that the playback time is really after the keyframe that was found
      if (channel->keyFrames[keyIndex].time > playbackTime)
      {
	// Check that we are not already at the first keyframe
	if (keyIndex > 0)
	  keyIndex--;

	// There are no keyframes before this one, therefore blend with the last keyframe
	else
	{
	  const KeyFrame& k1 = channel->keyFrames[channel->keyFrames.GetSize () - 1];
	  const KeyFrame& k2 = channel->keyFrames[0];
	  ApplyInterpolatedKeys (state, channel->bone,
				 k1, k2, k1.time - duration, k2.time,
				 playbackTime, baseWeight);
	  continue;
	}
      }

      // Normal interpolation between two consecutive keyframes
      const KeyFrame& k1 = channel->keyFrames[keyIndex];
      const KeyFrame& k2 = channel->keyFrames[keyIndex + 1];
      ApplyInterpolatedKeys (state, channel->bone,
			     k1, k2, k1.time, k2.time,
			     playbackTime, baseWeight);
    }
  }

  float Animation::GetDuration () const
  {
    return duration;
  }

  void Animation::SetFramesInBindSpace (bool isBindSpace)
  {
    this->isBindSpace = isBindSpace;
  }

  bool Animation::GetFramesInBindSpace () const
  {
    return isBindSpace;
  }

  void Animation::ConvertFrameSpace (CS::Animation::iSkeletonFactory* skeleton)
  {
    if (isBindSpace)
      return;

    for (size_t ch = 0; ch < channels.GetSize (); ch++)
    {
      AnimationChannel* channel = channels[ch];

      // Get the bind transform of the bone
      csQuaternion skeletonRotation;
      csVector3 skeletonOffset;
      skeleton->GetTransformBoneSpace (channel->bone, skeletonRotation, skeletonOffset);

      // Convert each keyframe for this bone
      for (size_t keyframe = 0; keyframe < channel->keyFrames.GetSize (); keyframe++)
      {
	KeyFrame& keyFrame = channel->keyFrames[keyframe];
	keyFrame.rotation = keyFrame.rotation * skeletonRotation.GetConjugate ();
	keyFrame.offset = keyFrame.offset - skeletonOffset;
      }
    }

    isBindSpace = true;
  }

  int Animation::KeyFrameCompare (KeyFrame const& k1, KeyFrame const& k2)
  {
    return csComparator<float, float>::Compare (k1.time, k2.time);    
  }

  int Animation::KeyFrameTimeCompare (KeyFrame const& k, float const& time)
  {
    return csComparator<float, float>::Compare (k.time, time);
  }

}
CS_PLUGIN_NAMESPACE_END(Skeleton2)
