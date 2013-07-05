/*
    Copyright (C) 2012-2013 by Christian Van Brussel
    Copyright (C) 2012 by Andrei Barsan

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
#ifndef __IUTIL_MODIFIABLE_H__
#define __IUTIL_MODIFIABLE_H__

#include "csutil/scf_interface.h"
#include "csutil/variant.h"
#include "iutil/strset.h"

struct iObjectRegistry;

namespace CS {
namespace Utility {

struct iModifiable;
struct iModifiableConstraint;
struct iModifiableDescription;

//----------------- iModifiableParameter ---------------------

/**
 * Description of a specific parameter of a iMovable.
 * \sa BaseModifiableParameter for a default implementation
 */
struct iModifiableParameter : public virtual iBase
{
  SCF_INTERFACE (iModifiableParameter, 1, 0, 0);

  /**
   * Get the ID of this parameter. This ID is built from the string returned by
   * GetLabel()
   */
  virtual csStringID GetID () const = 0;

  /**
   * Get the label of this parameter. A label identifies (supposedly uniquely)
   * this parameter.
   */
  virtual const char* GetLabel () const = 0;

  /**
   * Get the parameter's name
   * \note You might want to process this string by the translator.
   */
  virtual const char* GetName () const = 0;

  /**
   * Get the parameter's textual description
   * \note You might want to process this string by the translator.
   */
  virtual const char* GetDescription () const = 0;

  /**
   * Get the type of this parameter
   */
  virtual csVariantType GetType () const = 0;

  /**
   * Get this parameter's constraint, or nullptr if there are none.
   */
  virtual iModifiableConstraint* GetConstraint () const = 0;
};

//----------------- iModifiableDescription ---------------------

/**
 * The descriptor of an CS::Utility::iModifiable object. It contains all the
 * information needed in order to expose and access the properties of a iModifiable.
 *
 * It can be used fo an automated access to an object, eg in order to generate a
 * graphical user interface (see the CSEditing library), to animate it over time, or
 * to save and load it.
 * 
 * A iModifiableDescription is created through iModifiable::GetDescription() if a user
 * application would like to access the object without knowing its external interface.
 * The iModifiableDescription will then allow to know how the object should be accessed.
 *
 * \sa BaseModifiableDescription for a default implementation
 */
struct iModifiableDescription : public virtual iBase
{
  SCF_INTERFACE (iModifiableDescription, 1, 0, 0);

  /**
   * Get the label of this modifiable. A label identifies (supposedly uniquely) the
   * description of the modifiable.
   */
  virtual const char* GetLabel () const = 0;

  /**
   * Get the name of this modifiable
   * \note You might want to process this string by the translator.
   */
  virtual const char* GetName () const = 0;

  /**
   * Get the number of editable parameters of this description. This won't include
   * the parameters of the child descriptions.
   * \sa GetTotalParameterCount(), GetChild()
   */
  virtual size_t GetParameterCount () const = 0;

  /**
   * Get the total number of editable parameters of this description, that is
   * including the parameters of all children and grand-children.
   * \sa GetParameterCount(), GetChild()
   */
  virtual size_t GetTotalParameterCount () const = 0;

  /**
   * Get a parameter based on its ID.
   */
  virtual iModifiableParameter* GetParameter (csStringID id) const = 0;

  /**
   * Get a parameter based on its index.
   */
  virtual iModifiableParameter* GetParameter (size_t index) const = 0;

  /**
   * Find the index of a parameter based on its ID.
   * \return The index of the parameter, or (size_t) ~0 if it was not found.
   */
  virtual size_t FindParameter (csStringID id) const = 0;

  /**
   * Get the count of child descriptions. Child descriptions forms a hierarchical
   * structure of sets of parameters.
   */
  virtual size_t GetChildrenCount () const = 0;

  /**
   * Get the child description with the given index.
   */
  virtual iModifiableDescription* GetChild (size_t index) const = 0;

  /**
   * Get the list of resource entries of this description. Each entry is a path
   * containing additional data such as the translation files of this description.
   */
  virtual iStringArray* GetResources () const = 0;
};

//----------------- iModifiableListener ---------------------

/**
 * Listener for the notification of the state changes of a CS::Utility::iModifiable
 */
struct iModifiableListener : public virtual iBase
{
  SCF_INTERFACE (iModifiableListener, 1, 0, 0);

  /**
   * The value of the parameter of the given CS::Utility::iModifiable has changed.
   * \param modifiable The modifiable who's value has changed.
   * \param parameterIndex The index of the parameter that has changed.
   *
   * \note The new value of the parameter can be found eg through
   * CS::Utility::iModifiable::GetParameterValue().
   */
  // TODO: return bool to veto?
  virtual void ValueChanged (iModifiable* modifiable, size_t parameterIndex) = 0;
};

// ----------------- iModifiable ---------------------

/**
 * This interface is to be implemented by all objects willing to be modified automatically.
 * It provide access to the parameters of the CS objects, and to their type, description and
 * constraints. 
 *
 * @TODO An iModifiableDescription object should also be provided to allow a listing of the properties
 * , helping programs such as cseditor to generate Graphical User Interfaces in order
 * to modify their attributes. It can also be used by any automated process such as an animation
 * or a persistance system.
 *
 * \remark Triggers a crystalspace.modifiable.param.set event when a parameter value
 * gets set
 *
 * \see iModifiableDescription
 * \see iModifiableParameter
 */
struct iModifiable : public virtual iBase 
{
  SCF_INTERFACE (iModifiable, 1, 0 ,0);

  /**
   * Returns this object's description.
   */
  virtual csPtr<iModifiableDescription> GetDescription (iObjectRegistry* object_reg) const = 0;

  /**
   * Returns the value of one of this object's parameters. 
   * \remark Each modifiable property should have its own id in
   * order to be properly accessible.
   */
  virtual void GetParameterValue (size_t parameterIndex, csVariant& value) const = 0;

  /**
   * Sets a value for the parameter with the unique identifier id. 
   * \remark Each modifiable property should have its own id in 
   * order to be properly accessible. 
   *
   * \return true if the value can be set, false if a property with
   * that index couldn't be found
   */
  virtual bool SetParameterValue (size_t parameterIndex, const csVariant& value) = 0;

  /**
   * This method is there only to help securing the current code generation. This method
   * should therefore be removed once a better code generation system would be implemented.
   */
  virtual size_t GetTotalParameterCount () const = 0;

  /**
   * Add a listener to the list.
   */
  virtual void AddListener (iModifiableListener* listener) = 0;

  /**
   * Remove the given listener from the list.
   */
  virtual void RemoveListener (iModifiableListener* listener) = 0;
};

//----------------- iModifiableConstraintType ---------------------

// TODO: doc
enum iModifiableConstraintType
{
  MODIFIABLE_CONSTRAINT_BOUNDED = 0,
  MODIFIABLE_CONSTRAINT_ENUM,
  MODIFIABLE_CONSTRAINT_VFS_FILE,
  MODIFIABLE_CONSTRAINT_VFS_DIR,
  MODIFIABLE_CONSTRAINT_VFS_PATH,
  MODIFIABLE_CONSTRAINT_TEXT_ENTRY,
  MODIFIABLE_CONSTRAINT_TEXT_BLOB,
  MODIFIABLE_CONSTRAINT_BITMASK
  //MODIFIABLE_CONSTRAINT_ARRAY_STATIC
};

//----------------- iModifiableConstraint ---------------------

/**
 * Useful for validating various iModifiable parameters.
 * It's generally attached to an iModifiableProperty in an iModifiable object's 
 * GetDescription () method.
 *
 * \see iModifiable
 * \see PUSH_PARAM_CONSTRAINT for a helper macro
 */
struct iModifiableConstraint : public virtual iBase
{
  SCF_INTERFACE (iModifiableConstraint, 1, 0, 0);

  virtual iModifiableConstraintType GetType () const = 0;
  
  /**
   * Takes in a const csVariant* that it validates, according to the rules of a specific
   * constraint type. For instance, a long value could be limited by a bounded constraint
   * so that it stays between certain limits.
   *
   * Other types, such as enum constraints, don't actually use this function, since they
   * also have a helper role in creating a GUI that validates itself. For instance, the
   * enum constraint generate combo boxes with the allowed values, so it's certain that
   * any value the user might pick is valid.
   */
  // TODO: remove
  virtual bool Validate (const csVariant* variant) const = 0;
};

//----------------- iModifiableConstraintBounded ---------------------

/**
 * Constraint that forces a variant to either stay under a certain value, over
 * a certain value, or between two values. 
 */
struct iModifiableConstraintBounded : public virtual iModifiableConstraint
{
  virtual bool HasMinimum () const = 0;
  virtual csVariant& GetMinimum () const = 0;

  virtual bool HasMaximum () const = 0;
  virtual csVariant& GetMaximum () const = 0;
};

//----------------- iModifiableConstraintEnum ---------------------

/**
 * @TODO
 */
struct iModifiableConstraintEnum : public virtual iModifiableConstraint
{
  virtual size_t GetValueCount () const = 0;
  // TODO: return csVariant
  virtual long GetValue (size_t index) const = 0;
  virtual const char* GetLabel (size_t index) const = 0;
};

} // namespace Utility
} // namespace CS

#endif // __IUTIL_MODIFIABLE_H__
