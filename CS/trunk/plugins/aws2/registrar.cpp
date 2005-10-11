/*
    Copyright (C) 2005 by Christopher Nelson

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
#include "registrar.h"

namespace aws
{

namespace autom
{

static registrar *_reg_object=0;
static scope      _global_scope;
static nil	  _global_nil_object;

extern void install_builtin();

registrar *
Registrar()
{
      if (_reg_object==0)
      {
	      _reg_object = new registrar();	
	      install_builtin();
      }
      
      return _reg_object;	
}

scope *
GlobalScope()
{
  return &_global_scope;
}

nil*
Nil()
{
  return &_global_nil_object;
}

void 
Rescope(object *o, scope *sc)
{
  switch(o->ObjectType())
  {
  case iObject::T_VAR:
    static_cast<var *>(o)->setScope(sc);
    break;
  case iObject::T_FUNCTION:
    static_cast<function *>(o)->setScope(sc);
    break;
  default:
    break;
  }
}

void 
registrar::assign(const csString &name, func_ptr func)
{  
  lobby[getId(name)]=func;	
}	

std::pair<bool, registrar::func_ptr> 
registrar::lookup(const csString& name)
{      
    func_map_type::iterator func = lobby.find(getId(name));
  
    if (func!=lobby.end()) return std::make_pair(true, func->second);
    else return std::make_pair(false, func_ptr(0,0));      
}

void 
scope::addChild(const csString &name, scope *child)
{
  addChild(Registrar()->getId(name), child);
}

void 
scope::addChild(uint id, scope *child)
{
  children[id] = child;
}

scope *
scope::findChild(const csString &name)
{
  return findChild(Registrar()->getId(name));
}

scope *
scope::findChild(uint id)
{
  child_scope_map_type::iterator pos = children.find(id);

  if (pos==children.end()) return 0;
  else return pos->second;
}

keeper 
scope::get(const csString &name)
{
  uint id = Registrar()->getId(name);

  variable_map_type::iterator pos = vars.find(id);

  if (pos==vars.end()) return ( parent==0 ? keeper(Nil()) : parent->get(id) );
  else return pos->second;
}

keeper 
scope::get(uint id)
{
  variable_map_type::iterator pos = vars.find(id);

  if (pos==vars.end()) return ( parent==0 ? keeper(Nil()) : parent->get(id) );
  else return pos->second;
}

void 
scope::set(const csString &name, const keeper &val)
{
  vars[Registrar()->getId(name)]=val;
}

void 
scope::set(uint id, const keeper &val)
{
  vars[id]=val;
}

static object *
parseString(std::string::iterator &pos, const std::string::iterator &end)
{
	string  *o = new string;
	
	if (o->parseObject(pos, end)) return o;
	else
	{
		delete o;	
		return 0;	
	}	
}

static object *
parseFloat(std::string::iterator &pos, const std::string::iterator &end)
{
	floating  *o = new floating;
	
	if (o->parseObject(pos, end)) return o;
	else
	{
		delete o;
		return 0;	
	}	
}

static object *
parseInt(std::string::iterator &pos, const std::string::iterator &end)
{
	integer  *o = new integer;
	
	if (o->parseObject(pos, end)) return o;
	else
	{
		delete o;
		return 0;	
	}	
}

static object *
parseList(std::string::iterator &pos, const std::string::iterator &end)
{
	list  *o = new list;
	
	if (o->parseObject(pos, end)) return o;
	else
	{
		delete o;
		return 0;	
	}	
}

static object *
parseFunction(std::string::iterator &pos, const std::string::iterator &end, scope *sc, function *parent=0)
{
	function  *o = new function;
	
	if (o->parseObject(pos, end)) 
	{
		o->setParent(parent);
		o->setScope(sc);
		return o;
	}
	else
	{
		delete o;
		return 0;	
	}	
}

static object *
parseReference(std::string::iterator &pos, const std::string::iterator &end, function *parent=0)
{
	reference  *o = new reference;
	
	if (o->parseObject(pos, end)) 
	{
		o->setParent(parent);
		return o;
	}
	else
	{
		delete o;
		return 0;	
	}	
}

static object *
parseNil(std::string::iterator &pos, const std::string::iterator &end)
{	
	if (_global_nil_object.parseObject(pos, end)) return Nil();
	else
	{	
		return 0;	
	}	
}

static object *
parseBlob(std::string::iterator &pos, const std::string::iterator &end)
{	
	blob  *o = new blob;
	
	if (o->parseObject(pos, end)) return o;
	else
	{
		delete o;
		return 0;	
	}		
}

static object *
parseVar(std::string::iterator &pos, const std::string::iterator &end, scope *sc)
{
  var *o = new var;

  if (o->parseObject(pos, end))
  {
    o->setScope(sc);
    return o;
  }

  delete o;
  return 0;
}

object *
Parse(std::string::iterator &pos, const std::string::iterator &end, scope *sc)
{	
	object *o=0;
	if (sc==0) sc=GlobalScope();
	
	while(pos!=end && isspace(*pos)) { ++pos; }
	
	if (pos==end) return 0;
		
	switch(*pos)
	{
		case ':':
		case '%': return parseFunction(pos, end, sc);		
		case '/': return parseBlob(pos, end);
		case '"': return parseString(pos, end);
		case '[': return parseList(pos, end);
		case 'n': return parseNil(pos, end);
		case '*': return parseVar(pos, end, sc);
		
		default:		
			if ((o=parseFloat(pos, end))!=0) 	return o;
			else if ((o=parseInt(pos, end))!=0) 	return o;	
			
		break;			
	}
	
	return 0;
}

object *
ParseParameter(std::string::iterator &pos, const std::string::iterator &end, function *parent, scope *sc)
{
	object *o=0;

	if (sc==0) sc=GlobalScope();
	
	while(pos!=end && isspace(*pos)) { ++pos; }
	
	if (pos==end) return 0;
		
	switch(*pos)
	{
		case ':':
		case '%': return parseFunction(pos, end, sc, parent);
		case '$': return parseReference(pos, end, parent);
		case '/': return parseBlob(pos, end);
		case '"': return parseString(pos, end);
		case '[': return parseList(pos, end);
		case 'n': return parseNil(pos, end);
		case '*': return parseVar(pos, end, sc);
		
		default:		
			if ((o=parseFloat(pos, end))!=0) 	return o;
			else if ((o=parseInt(pos, end))!=0) 	return o;	
			
		break;			
	}
	
	return 0;	
}

keeper 
Compile(std::string &str)
{
	std::string::iterator pos = str.begin();
		
	object *o = Parse(pos, str.end());		
	
	return keeper(o);
}

} // namespace autom

} // namespace aws
