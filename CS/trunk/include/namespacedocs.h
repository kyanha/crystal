/*
  Copyright (C) 2007 by Marten Svanfeldt

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

#ifndef CS_NAMESPACEDOCS_H
#define CS_NAMESPACEDOCS_H

/** \file
 * Documentation for namespaces in CS
 */

/**
 * Main namespace for CrystalSpace
 */
namespace CS
{
  /**
   * Debugging utilities and helpers
   */
  namespace Debug
  {}

  /**
   * Contains deprecated code that will be removed after next stable release
   * \deprecated
   */
  namespace Deprecated
  {}

  /**
   * Document system / XML helper classes
   */
  namespace DocSystem
  {}

  /**
   * Mathematical functions and definitions
   */
  namespace Math
  {}

  /**
   * Memory allocation
   */
  namespace Memory
  {}

  /**
   * Meta-programming helpers
   */
  namespace Meta
  {}

  /**
   * Platform specific code and wrappers
   */
  namespace Platform
  {}

  /**
   * Implementation of all plugins
   */
  namespace Plugin
  {}

  /**
   * Common code shared among a number of plugins
   */
  namespace PluginCommon
  {}

  /**
   * Functions and classes for threading, locking and atomic operations
   */
  namespace Threading
  {}

  /**
   * Assorted utility functions and classes that doesn't belong anywhere 
   * else
   */
  namespace Utility
  {}
}

#endif
