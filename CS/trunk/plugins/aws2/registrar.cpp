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

nil*
Nil()
{
	return	&_global_nil_object;
}

void 
registrar::assign(const csString &name, func_ptr func)
{
  lobby[name]=func;	
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
parseFunction(std::string::iterator &pos, const std::string::iterator &end, function *parent=0)
{
	function  *o = new function;
	
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

object *
Parse(std::string::iterator &pos, const std::string::iterator &end)
{	
	object *o=0;
	
	while(pos!=end && isspace(*pos)) { ++pos; }
	
	if (pos==end) return 0;
		
	switch(*pos)
	{
		case ':':
		case '%': return parseFunction(pos, end);		
		case '/': return parseBlob(pos, end);
		case '"': return parseString(pos, end);
		case '[': return parseList(pos, end);
		case 'n': return parseNil(pos, end);
		
		default:		
			if ((o=parseFloat(pos, end))!=0) 		return o;
			else if ((o=parseInt(pos, end))!=0) 	return o;	
			
		break;			
	}
	
	return 0;
}

object *
ParseParameter(std::string::iterator &pos, const std::string::iterator &end, function *parent)
{
	object *o=0;
	
	while(pos!=end && isspace(*pos)) { ++pos; }
	
	if (pos==end) return 0;
		
	switch(*pos)
	{
		case ':':
		case '%': return parseFunction(pos, end, parent);
		case '$': return parseReference(pos, end, parent);
		case '/': return parseBlob(pos, end);
		case '"': return parseString(pos, end);
		case '[': return parseList(pos, end);
		case 'n': return parseNil(pos, end);
		
		default:		
			if ((o=parseFloat(pos, end))!=0) 		return o;
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
