/*
    Copyright (C) 2002 by Jorrit Tyberghein

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

#ifndef __IVARIA_ENGSEQ_H__
#define __IVARIA_ENGSEQ_H__

#include "csutil/scf.h"

struct iSequence;
struct iSequenceManager;
struct iSequenceTrigger;
struct iSequenceWrapper;
struct iMaterialWrapper;
struct iSector;
struct iMeshWrapper;
struct iPolygon3D;
struct iObject;
struct iLight;
struct iCamera;
class csColor;
class csReversibleTransform;
class csVector3;
class csBox3;
class csSphere;

SCF_VERSION (iParameterESM, 0, 0, 1);

/**
 * This interface is a parameter resolver. The operations
 * in the engine sequence manager use instances of this class
 * to get the required object (mesh, light, material, ...).
 * The engine sequence manager itself currently provides two ready-made
 * implementations of this resolver:
 * <ul>
 * <li>iEngineSequenceParameters->CreateParameterESM() which will create
 *     a resolver that gets the requested parameter from the given
 *     iEngineSequenceParameters instance. This is useful for sequences
 *     where you don't know in advance on what objects you will use it.
 * <li>iEngineSequenceManager->CreateParameterESM() which will give
 *     a resolver that returns a constant value. This is useful for
 *     operations where the object to operate on is known in advance.
 * </ul>
 */
struct iParameterESM : public iBase
{
  /**
   * Get the value based on userdata which is given to the
   * operations. If IsConstant() returns true then the params
   * parameter will not be used!
   */
  virtual iBase* GetValue (iBase* params = NULL) const = 0;

  /**
   * Returns true if the value is constant and immediatelly available
   * upon request. In that case operations can store that value so
   * then don't have to ask for it every time. If this function returns
   * false then the operation MUST call GetValue() every time it
   * wants to do something with the object.
   */
  virtual bool IsConstant () const = 0;
};

SCF_VERSION (iEngineSequenceParameters, 0, 0, 2);

/**
 * An interface for passing on parameters to the engine sequence
 * manager. You can create a ready-made instance of this interface
 * by calling iSequenceWrapper->CreateBaseParameterBlock(). This will
 * create an empty parameter block that specifies the supported parameters
 * (and optional default values) that are relevant for that sequence.
 * When running a sequence later you can call
 * iSequenceWrapper->CreateParameterBlock() to make a clone of the
 * base parameter block and then fill in the values.<br>
 * To use a value from this parameter block you can call CreateParameterESM()
 * which will return a parameter that you can give to an operation.
 */
struct iEngineSequenceParameters : public iBase
{
  /**
   * Get the number of parameters supported.
   */
  virtual int GetParameterCount () const = 0;

  /**
   * Get a parameter.
   */
  virtual iBase* GetParameter (int idx) const = 0;

  /**
   * Get a parameter by name.
   */
  virtual iBase* GetParameter (const char* name) const = 0;

  /**
   * Get a parameter index by name.
   */
  virtual int GetParameterIdx (const char* name) const = 0;

  /**
   * Get parameter name.
   */
  virtual const char* GetParameterName (int idx) const = 0;

  /**
   * Add a parameter. Warning! ONLY call this for setting up the
   * base parameter block. Don't use this to set the values for
   * parameters later on blocks created with CreateParameterBlock()!
   */
  virtual void AddParameter (const char* name, iBase* def_value = NULL) = 0;

  /**
   * Set a parameter by index.
   */
  virtual void SetParameter (int idx, iBase* value) = 0;

  /**
   * Set a parameter by name.
   */
  virtual void SetParameter (const char* name, iBase* value) = 0;

  /**
   * Create a parameter ESM which keeps a reference to this parameter
   * block and knows how to resolve the specified parameter. Returns
   * NULL if the parameter 'name' is not known in this block. You can use
   * the return of this function to give as an argument for operations.
   */
  virtual csPtr<iParameterESM> CreateParameterESM (const char* name) = 0;
};

SCF_VERSION (iSequenceWrapper, 0, 2, 0);

/**
 * A sequence wrapper. This objects holds the reference
 * to the original sequence and also implements iObject.
 * Basically a sequence corresponds to a series of operations
 * that are time based and can be scheduled on the sequence manager.
 * This class enhances iSequence with support for custom operations
 * and parameter blocks.
 * <p>
 * Note that many parameters given to the AddOperation functions
 * are of type iParameterESM.
 */
struct iSequenceWrapper : public iBase
{
  /**
   * Query object.
   */
  virtual iObject* QueryObject () = 0;

  /**
   * Get the sequence. It is allowed to use the underlying sequence
   * for general sequence operations like adding conditions, operations,
   * and general sequence management. The AddOperationBla() functions
   * provided in this wrapper do nothing more than add custom operations
   * through the regular iSequence->AddOperation().
   */
  virtual iSequence* GetSequence () = 0;

  /**
   * Create a parameter block for this sequence wrapper. After
   * creating this you can initialized the parameters (with names
   * and optional default values). Later on you can use
   * GetSequenceParameters() to get a copy of a parameter block to
   * use for running a sequence.
   */
  virtual iEngineSequenceParameters* CreateBaseParameterBlock () = 0;

  /**
   * Get the pointer to the base parameter block (or NULL if there is
   * no such block).
   */
  virtual iEngineSequenceParameters* GetBaseParameterBlock () = 0;

  /**
   * Create a parameter block which you can then fill in and then
   * give as a parameter running this sequence. This essentially
   * creates a copy of the base parameter block created with
   * CreateBaseParameterBlock(). Modifications on the parameter block
   * returned by this function have no effect on the parameter block
   * which is kept internally. You should only set the values
   * of the given parameter block and not create/remove variables.
   * This function returns NULL if there is no parameter block
   * for this sequence.
   */
  virtual csPtr<iEngineSequenceParameters> CreateParameterBlock () = 0; 

  /**
   * Operation: set a material on a mesh.
   */
  virtual void AddOperationSetMaterial (csTicks time, iParameterESM* mesh,
		  iParameterESM* mat) = 0;

  /**
   * Operation: set a material on a polygon.
   */
  virtual void AddOperationSetPolygonMaterial (csTicks time,
  		  iParameterESM* polygon, iParameterESM* mat) = 0;

  /**
   * Operation: set a light color.
   */
  virtual void AddOperationSetLight (csTicks time, iParameterESM* light,
		  const csColor& color) = 0;

  /**
   * Operation: fade a light to some color during some time.
   */
  virtual void AddOperationFadeLight (csTicks time, iParameterESM* light,
		  const csColor& color, csTicks duration) = 0;

  /**
   * Operation: set dynamic ambient light color.
   */
  virtual void AddOperationSetAmbient (csTicks time, iParameterESM* light,
		  const csColor& color) = 0;

  /**
   * Operation: fade dynamic ambient light to some color during some time.
   */
  virtual void AddOperationFadeAmbient (csTicks time, iParameterESM* light,
		  const csColor& color, csTicks duration) = 0;

  /**
   * Operation: set a mesh color.
   */
  virtual void AddOperationSetMeshColor (csTicks time, iParameterESM* mesh,
		  const csColor& color) = 0;

  /**
   * Operation: fade a mesh to some color during some time.
   */
  virtual void AddOperationFadeMeshColor (csTicks time, iParameterESM* mesh,
		  const csColor& color, csTicks duration) = 0;

  /**
   * Operation: set a fog color and density.
   */
  virtual void AddOperationSetFog (csTicks time, iParameterESM* sector,
		  const csColor& color, float density) = 0;

  /**
   * Operation: fade fog to some color/density during some time.
   */
  virtual void AddOperationFadeFog (csTicks time, iParameterESM* sector,
		  const csColor& color, float density, csTicks duration) = 0;

  /**
   * Operation: rotate object during some time. After the time has elapsed
   * the rotation will be equal to the given angle here.
   * Axis is 0, 1, or 2 for x, y, or z. If axis is -1 it is not used.
   */
  virtual void AddOperationRotateDuration (csTicks time, iParameterESM* mesh,
  		int axis1, float tot_angle1,
		int axis2, float tot_angle2,
		int axis3, float tot_angle3,
		const csVector3& offset,
		csTicks duration) = 0;

  /**
   * Operation: move object during some time. After the time has elapsed
   * the total relative move will be equal to the 'offset'.
   */
  virtual void AddOperationMoveDuration (csTicks time, iParameterESM* mesh,
		const csVector3& offset, csTicks duration) = 0;

  /**
   * Operation: enable/disable a given trigger.
   */
  virtual void AddOperationTriggerState (csTicks time,
  		  iParameterESM* trigger, bool en) = 0;

  /**
   * Operation: enable checking of trigger state every 'delay'
   * milliseconds (or disable with delay == 0). Use this in
   * combination with AddOperationTestTrigger().
   */
  virtual void AddOperationCheckTrigger (csTicks time,
  		  iParameterESM* trigger, csTicks delay) = 0;

  /**
   * Operation: test trigger state and run a sequence if trigger
   * is still valid or another sequence if not (both sequences
   * can be NULL in which case nothing is run).
   * Use in combination with AddOperationCheckTrigger().
   */
  virtual void AddOperationTestTrigger (csTicks time,
  		  iParameterESM* trigger,
		  iSequence* trueSequence,
		  iSequence* falseSequence) = 0;
};

SCF_VERSION (iSequenceTrigger, 0, 0, 3);

/**
 * A sequence trigger. When all conditions in a trigger are
 * true it will run a sequence. Note that after the succesfull firing
 * of a trigger it will automatically be disabled.
 */
struct iSequenceTrigger : public iBase
{
  /**
   * Query object.
   */
  virtual iObject* QueryObject () = 0;

  /**
   * Condition: true if camera is in some sector.
   */
  virtual void AddConditionInSector (iSector* sector) = 0;

  /**
   * Condition: true if camera is in some sector and bbox.
   */
  virtual void AddConditionInSector (iSector* sector,
	const csBox3& box) = 0;

  /**
   * Condition: true if camera is in some sector and sphere.
   */
  virtual void AddConditionInSector (iSector* sector,
	const csSphere& sphere) = 0;

  /**
   * Condition: true if (part of) sector is visible.
   */
  virtual void AddConditionSectorVisible (iSector* sector) = 0;

  /**
   * Condition: true if clicked on a mesh.
   */
  virtual void AddConditionMeshClick (iMeshWrapper* mesh) = 0;

  /**
   * Condition: light change.  Call this to add a trigger
   * which fires a sequence when a light gets darker than 
   * a certain value or lighter than a certain value, or
   * whenever a light changes.
   */
  virtual void AddConditionLightChange (iLight *whichlight, 
				        int oper,csColor& col) = 0;

  /**
   * Condition: manual trigger. Call this to set add a trigger
   * that requires manual confirmation. The 'Trigger()' function
   * can then be used later to actually do the trigger.
   */
  virtual void AddConditionManual () = 0;

  /**
   * Enable/disable this trigger. Triggers start enabled by
   * default.
   */
  virtual void SetEnabled (bool en) = 0;

  /**
   * Get enabled/disabled state.
   */
  virtual bool IsEnabled () const = 0;

  /**
   * Clear all conditions.
   */
  virtual void ClearConditions () = 0;

  /**
   * Trigger the manual condition.
   */
  virtual void Trigger () = 0;

  /**
   * Set the parameter block to use for the sequence when it is fired.
   */
  virtual void SetParameters (iEngineSequenceParameters* params) = 0;

  /**
   * Get the parameter block.
   */
  virtual iEngineSequenceParameters* GetParameters () const = 0;

  /**
   * Attach the sequence that will be fired when all trigger
   * conditions are valid.
   */
  virtual void FireSequence (csTicks delay, iSequenceWrapper* seq) = 0;

  /**
   * Get the attached sequence.
   */
  virtual iSequenceWrapper* GetFiredSequence () = 0;

  /**
   * Test the conditions of this trigger every 'delay' milliseconds.
   * Use this in combination with CheckState(). If 'delay' == 0
   * then this testing is disabled (default).
   */
  virtual void TestConditions (csTicks delay) = 0;

  /**
   * This function returns true if the trigger conditions are
   * valid. This only works if TestConditions() has been called
   * and it doesn't work immediatelly after TestConditions() because
   * TestConditions() needs to take some time before it actually
   * can retest the conditions.
   */
  virtual bool CheckState () = 0;

  /**
   * Force the sequence of this trigger to be fired right now.
   * Note that this will even fire if the trigger is disabled and
   * conditions are completely ignored.
   * <p>
   * Also calling ForceFire() will NOT cause the trigger to become disabled
   * (as opposed to when a trigger normally fires). So if you want to make
   * sure the trigger does not accidently fire again right after firing it
   * you should disable the trigger (and possibly let the sequence enable
   * it again).
   * <p>
   * Note that ForceFire() still respects the fire delay with which the
   * sequence was registered. If you use 'now' == true then this delay
   * will be ignored and the sequence will be started immediatelly.
   */
  virtual void ForceFire (bool now = false) = 0;
};

SCF_VERSION (iSequenceTimedOperation, 0, 0, 1);

/**
 * A timed operation for the engine sequence manager.
 * This is basically something that needs to run over some period
 * of time. The 'elapsed' value that needs to be implemented by
 * subclasses will go from 0 to 1. When the time expires (goes beyond
 * 1) then the operation will be deleted automatically.
 * Timed operations are usually fired from within a sequence operation
 * (iSequenceOperation).
 */
struct iSequenceTimedOperation : public iBase
{
  /**
   * Do the operation. 'time' will be between 0 and 1.
   */
  virtual void Do (float time, iBase* params) = 0;
};

SCF_VERSION (iEngineSequenceManager, 0, 0, 3);

/**
 * Sequence manager specifically designed for working on
 * the engine.
 */
struct iEngineSequenceManager : public iBase
{
  /**
   * Get a pointer to the underlying sequence manager that
   * is being used.
   */
  virtual iSequenceManager* GetSequenceManager () = 0;

  /**
   * Set the camera to use for some of the features (like clicking
   * on mesh objects). If this is not set then those features will
   * not be available.
   */
  virtual void SetCamera (iCamera* camera) = 0;

  /**
   * Get the camera that is used for some features.
   */
  virtual iCamera* GetCamera () = 0;
  
  /**
   * Create a parameter ESM for a constant value.
   */
  virtual csPtr<iParameterESM> CreateParameterESM (iBase* value) = 0;

  //-----------------------------------------------------------------------

  /**
   * Create a new trigger with a given name.
   */
  virtual csPtr<iSequenceTrigger> CreateTrigger (const char* name) = 0;

  /**
   * Remove trigger from the manager.
   */
  virtual void RemoveTrigger (iSequenceTrigger* trigger) = 0;

  /**
   * Remove all triggers.
   */
  virtual void RemoveTriggers () = 0;

  /**
   * Get the number of triggers.
   */
  virtual int GetTriggerCount () const = 0;

  /**
   * Get a trigger.
   */
  virtual iSequenceTrigger* GetTrigger (int idx) const = 0;

  /**
   * Get a trigger by name.
   */
  virtual iSequenceTrigger* FindTriggerByName (const char* name) const = 0;

  /**
   * Fire a trigger manually, specifying the name.
   * This will call ForceFire() on the trigger (if one is found). If
   * now == false then the usual delay will be respected. Otherwise
   * the sequence will be run immediatelly without the default delay.
   */
  virtual bool FireTriggerByName (const char *name, bool now = false) const = 0;

  //-----------------------------------------------------------------------

  /**
   * Create a new sequence with a given name.
   */
  virtual csPtr<iSequenceWrapper> CreateSequence (const char* name) = 0;

  /**
   * Remove sequence from the manager.
   */
  virtual void RemoveSequence (iSequenceWrapper* seq) = 0;

  /**
   * Remove all sequences.
   */
  virtual void RemoveSequences () = 0;

  /**
   * Get the number of sequences.
   */
  virtual int GetSequenceCount () const = 0;

  /**
   * Get a sequence.
   */
  virtual iSequenceWrapper* GetSequence (int idx) const = 0;

  /**
   * Get a sequence by name.
   */
  virtual iSequenceWrapper* FindSequenceByName (const char* name) const = 0;

  /**
   * Run a sequence and don't mess around with triggers.
   */
  virtual bool RunSequenceByName (const char *name,int delay) const = 0;

  //-----------------------------------------------------------------------

  /**
   * Start a timed operation with a given delta (in ticks).
   * The delta has to be interpreted as the amount of time that has
   * already elapsed since the beginning of the timed operation.
   * The params block is increffed for as long as is needed so you
   * can release your reference.
   */
  virtual void FireTimedOperation (csTicks delta, csTicks duration,
  	iSequenceTimedOperation* op, iBase* params = NULL) = 0;

  //-----------------------------------------------------------------------
};

#endif // __IVARIA_ENGSEQ_H__

