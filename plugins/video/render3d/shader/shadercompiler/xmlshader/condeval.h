/*
  Copyright (C) 2004 by Frank Richter
	    (C) 2004 by Jorrit Tyberghein

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __CONDEVAL_H__
#define __CONDEVAL_H__

#include "csutil/array.h"
#include "csutil/hashr.h"
#include "iutil/strset.h"
#include "ivideo/shader/shader.h"

#include "docwrap.h"
#include "expparser.h"

/**
 * Processes an expression tree and converts it into an internal 
 * representation and allows later evaluation of those expression.
 */
class csConditionEvaluator
{
  /// Used to resolve SV names.
  csRef<iStringSet> strings;

  /**
   * Possible operations for a node in the internal expression
   * representation.
   */
  enum ConditionOp
  {
    opInvalid = 0,

    opNot,
    opAnd,
    opOr,

    opEqual,
    opNEqual,
    opLesser,
    opLesserEq
  };
  /// Possible types of operands.
  enum OperandType
  {
    operandNone,
    operandOperation,

    operandFloat,
    operandInt,
    operandBoolean,
    operandSV,
    operandSVValueInt,
    operandSVValueFloat,
    operandSVValueTexture,
    operandSVValueBuffer,
  };
  /// An actual operand.
  struct CondOperand
  {
    OperandType type;
    union
    {
      int intVal;
      float floatVal;
      bool boolVal;
      csStringID svName;
      csConditionID operation;
    };
    CondOperand ()
    { memset (this, 0, sizeof (*this)); }
  };
  /// An operation.
  struct CondOperation
  {
    ConditionOp operation;
    CondOperand left;
    CondOperand right;

    CondOperation ()
    { operation = opInvalid; }
  };

  class OperationHashKeyHandler
  {
    /**
     * Test whether an operation is communative.
     * Those ops are treated specially, so e.g. the expression
     * "a == b" gets the same hash as "b == a".
     */
    static bool IsOpCommutative (ConditionOp op);
    static uint ActualHash (ConditionOp operation,
      const CondOperand& left, const CondOperand& right);
  public:
    static uint ComputeHash (const CondOperation& operation);
    static bool CompareKeys (const CondOperation& op1, 
      const CondOperation& op2);
  };
  friend class OperationHashKeyHandler;

  csConditionID nextConditionID;
  csHashReversible<csConditionID, CondOperation, OperationHashKeyHandler>
    conditions;

  csString lastError;
  const char* SetLastError (const char* msg, ...) CS_GNUC_PRINTF (2, 3);

  /**
   * Check whether to operand types are compatible, ie can be compared.
   * E.g. 'bool' and 'int' are not compatible.
   */
  static bool OpTypesCompatible (OperandType t1, OperandType t2);
  /// Get a name for an operand type, for error reporting purposes.
  static const char* OperandTypeDescription (OperandType t);
  /// Get the ID for an operation.
  csConditionID FindCondition (const CondOperation& operation);
  const char* ResolveExpValue (const csExpressionToken& value,
    CondOperand& operand);
  const char* ResolveOperand (csExpression* expression, 
    CondOperand& operand);
  const char* ResolveSVIdentifier (csExpression* expression, 
    CondOperand& operand);

  bool EvaluateOperandB (const CondOperand& operand, 
    const csRenderMeshModes& modes, const csShaderVarStack& stacks);
  int EvaluateOperandI (const CondOperand& operand, 
    const csRenderMeshModes& modes, const csShaderVarStack& stacks);
  float EvaluateOperandF (const CondOperand& operand, 
    const csRenderMeshModes& modes, const csShaderVarStack& stacks);
public:
  csConditionEvaluator (iStringSet* strings);

  /// Convert expression into internal representation.
  const char* ProcessExpression (csExpression* expression, 
    csConditionID& cond);

  /// Evaluate a condition and return the result.
  bool Evaluate (csConditionID condition, const csRenderMeshModes& modes,
    const csShaderVarStack& stacks);
};

#endif // __CONDEVAL_H__
