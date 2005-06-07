#include "cssysdef.h"
#include "xml_def.h"
#include "registrar.h"

#include "iutil/document.h"


namespace aws
{

void
defFile::ParseNode(registry *reg, csRef< iDocumentNodeIterator> &pos)
{
	// Walk through all of the nodes and
	while(pos->HasNext())
	{
		bool had_value=false;

		csRef<iDocumentNode> child = pos->Next ();
		std::string name(child->GetValue());

		if (name == "component" ||
			name == "window")
		{
			csRef< iDocumentNodeIterator> new_pos = child->GetNodes();
			registry *child_reg = new registry(child->GetAttributeValue("name"));

			reg->addChild(child_reg);
			child_reg->setParent(reg);

			ParseNode(child_reg, new_pos);
		}
		else
		{
			
			csRef< iDocumentAttributeIterator > attr_pos = child->GetAttributes();

			// Loop over all of the attributes in the element and add them as keys into the map.
            while(attr_pos->HasNext())
			{
                csRef< iDocumentAttribute > attr = attr_pos->Next();
				
				std::string a_name(attr->GetName());

				//  If the name of the attribute is value, then the name of the key is the same as the name of the element, otherwise
				// we use element_name.attribute_name as the key name.
				if (a_name=="value")			
				{				
					reg->insert(child->GetValue(), autom::keeper(new autom::string(attr->GetValue())));				
					had_value=true;
				}
				else
				{
					a_name = name + "." + a_name;
					reg->insert(a_name, autom::keeper(new autom::string(attr->GetValue())));				
				}
			}
		}

		if (!had_value)
		{
			std::string txt(child->GetContentsValue());		
			reg->insert(name, autom::Compile(txt));
		}
	}
}

bool 
defFile::Parse(const std::string &txt, registry &reg)
{
	/*csRef<iPluginManager> plugin_mgr =  CS_QUERY_REGISTRY (object_reg, iPluginManager);	
	csRef<iDocumentSystem> inputDS = csPtr<iDocumentSystem> (CS_LOAD_PLUGIN (plugin_mgr, "crystalspace.documentsystem.xmltiny", iDocumentSystem));*/

	csRef<iDocumentSystem> xml = (csPtr<iDocumentSystem> (new csTinyDocumentSystem ()));;
	csRef<iDocument>	   doc = xml->CreateDocument ();

	doc->Parse(txt.c_str(), true);

	csRef< iDocumentNode > node = doc->GetRoot();
	csRef< iDocumentNodeIterator> pos = node->GetNodes();

	ParseNode(&reg, pos);

	delete xml;
	delete doc;

	return true;
}

} // end namespace
