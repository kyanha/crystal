/*
    Copyright (C) 2000 by Jorrit Tyberghein

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

#ifndef __CS_MOTION_H__
#define __CS_MOTION_H__

#include "iengine/motion.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "csutil/parray.h"
#include "csgeom/quaterni.h"

struct iVirtualClock;

struct csMotionFrame
{
  float frametime;
  csVector3 pos;
  csQuaternion rot;
};

class csMotionBone
{
public:
  char *name;
  unsigned int hash;

  csMotionFrame *frames;
  int framecount;

  csMotionBone():name(0), hash(0), frames(0), framecount(0) {}

  unsigned int GetHash() const { return hash; }
  char* GetName() { return name; }

  void SelectFrameForTime(float time,float duration, float *weight, int *curframe, int *nextframe);
  void Animate(float time,float duration, csVector3 &v, csQuaternion &q, bool interpolate);
};

typedef csPDelArray<csMotionBone> csMotionBoneVector;

class csMotionTemplate : public iMotionTemplate
{
public:
  char* name;
  unsigned int hash;

  float duration;
  int loopcount;
  bool loopflip;

  csMotionBoneVector bones;

  SCF_DECLARE_IBASE;

  csMotionTemplate();
  virtual ~csMotionTemplate();

  void SetName (const char* name);
  unsigned int GetHash() const { return hash; }

  virtual const char* GetName ();
  virtual const float GetDuration() {return duration;};
  virtual const int   GetLoopCount() {return loopcount;};
  virtual const bool  GetLoopFlip() {return loopflip;};
  virtual void SetDuration (float duration);
  virtual void SetLoopCount (int loopcount);
  virtual void SetLoopFlip (bool enable);

  virtual int AddBone (const char* name);
  virtual int FindBoneByName (const char* name);

  virtual void AddFrameBone (int boneid, float frametime, const csVector3 &v, const csQuaternion &q);
};

typedef csPDelArray<csMotionTemplate> csMotionTemplateVector;

class csMotionStackItem
{
public:
  csMotionTemplate *motion;

  float frametime;
  float rate;

  int loopcount;
  bool loopflip;

  csMotionStackItem(csMotionTemplate *Tmotion);
  ~csMotionStackItem();

  void DoLoop();
  bool Update(float timedelta);
};

class csMotionStackVector : public csPDelArray<csMotionStackItem>
{
public:
  //csMotionStackVector (int ilimit = 0, int ithreshold = 0)
    //: csMotionStackVectorBase(ilimit, ithreshold) {}
};

#define MAX_MOTION_PER_BONE 12
struct csMotionBoneCacheItem
{
  unsigned int hash;
  iSkeletonBone *bone;
  int nummotions;
  unsigned short stacks[MAX_MOTION_PER_BONE];
  unsigned short boneids[MAX_MOTION_PER_BONE];
};

class csMotionController : public iMotionController
{
  iSkeletonBone* skel;

  bool paused;

  csMotionStackVector stack;
  bool stackchanged;

  csMotionBoneCacheItem* bonecache;
  int bonecachesize;
  int bonecachelimit;
public:
  SCF_DECLARE_IBASE;

  csMotionController(iSkeletonBone* Tskel);
  virtual ~csMotionController();

  iSkeletonBone* GetSkeleton() const { return skel; }
  void Update(float timedelta);
  void RecalculateBoneCache();
  bool AddToBoneCache(unsigned int hash, int stack, int bone);
  void Animate();

  virtual void SetMotion(iMotionTemplate *motion);
  virtual void BlendMotion(iMotionTemplate *motion);
  virtual void Pause(bool enable);
};

typedef csPDelArray<csMotionController> csMotionControllerVector;

class csMotionManager : public iMotionManager
{
  csTicks oldtime;

  csMotionTemplateVector motions;
  csMotionControllerVector controllers;

  iObjectRegistry* object_reg;
  csRef<iVirtualClock> vc;

public:
  SCF_DECLARE_IBASE;

  csMotionManager(iBase *iParent);
  virtual ~csMotionManager();

  virtual bool Initialize (iObjectRegistry *object_reg);

  csMotionTemplate* FindMotionTemplateByName (const char* name);
  csMotionTemplate* AddMotionTemplate (const char* name);

  csMotionController* FindMotionControllerBySkeleton (iSkeletonBone *skel);
  csMotionController* AddMotionController (iSkeletonBone *skel);

  void UpdateController(csMotionController *controller, float timedelta);

  virtual iMotionTemplate* AddMotion (const char* name) {
    return AddMotionTemplate(name);
  }
  virtual void DeleteMotion (iMotionTemplate* motiontemp);
  virtual iMotionTemplate* FindMotionByName (const char* name) {
    return FindMotionTemplateByName(name);
  }

  virtual iMotionController* AddController (iSkeletonBone *skel) {
    return AddMotionController(skel);
  }
  virtual void DeleteController (iMotionController *inst);
  virtual iMotionController* FindControllerBySkeleton (iSkeletonBone *skel) {
    return FindMotionControllerBySkeleton(skel);
  }

  virtual void UpdateAll ( float timedelta );
  virtual void UpdateAll ( unsigned int curtime );
  virtual void UpdateAll ();

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csMotionManager);
    virtual bool Initialize (iObjectRegistry* p)
    { return scfParent->Initialize(p); }
  } scfiComponent;
};

iSkeletonBone *csFindBone( iSkeletonBone *bone, unsigned int hash );

#endif // __CS_MOTION_H__

