#include "cssysdef.h"
#include "registrar.h"

namespace autom
{

static registrar *_reg_object=0;
static nil	   	  _global_nil_object;

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
registrar::assign(const std::string &name, func_ptr func)
{
	std::vector<std::string> parts;
	
	cont_ptr cont=lobby;			
	
	if (std::split(const_cast<std::string&>(name), '@', parts)>1)
	{
		std::vector<std::string> cont_names;
		
		if (std::split(parts[1], '.', cont_names))
		{																	
			// Find the container - creating it if necessary.							
			for(std::vector<std::string>::iterator pos=cont_names.begin(); pos!=cont_names.end(); ++pos)				
			{		
				container::cont_map_type::iterator name_pos = cont->cont_map.find(*pos);

				if (name_pos==cont->cont_map.end())											 
				  name_pos = (cont->cont_map.insert(std::make_pair(*pos, new container()))).first;									
				
				cont = name_pos->second;							
			}				

		}	
		
	}	
	
	// Assign the function name
	cont->func_map[parts[0]] = func;							
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

object *
Parse(std::string::iterator &pos, const std::string::iterator &end)
{	
	object *o=0;
	
	//filter::console_output out;
	
	//out << "keila: parsing input\n";
		
	if ((o=parseString(pos, end))!=0)			return o;
	else if ((o=parseFloat(pos, end))!=0) 		return o;
	else if ((o=parseInt(pos, end))!=0) 		return o;	
	else if ((o=parseNil(pos, end))!=0) 		return o;	
	else if ((o=parseList(pos, end))!=0) 		return o;	
	else if ((o=parseFunction(pos, end))!=0)	return o;
	
	return o;
}

object *
ParseParameter(std::string::iterator &pos, const std::string::iterator &end, function *parent)
{
	object *o=0;
	
	if ((o=parseString(pos, end))!=0)					return o;
	else if ((o=parseFloat(pos, end))!=0) 				return o;
	else if ((o=parseInt(pos, end))!=0) 				return o;	
	else if ((o=parseNil(pos, end))!=0) 				return o;	
	else if ((o=parseReference(pos, end, parent))!=0)	return o;
	else if ((o=parseList(pos, end))!=0) 				return o;	
	else if ((o=parseFunction(pos, end, parent))!=0)	return o;
		
	return o;	
}

keeper 
Compile(std::string &str)
{
	std::string::iterator pos = str.begin();
		
	object *o = Parse(pos, str.end());		
	
	return keeper(o);
}

}
