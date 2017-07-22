
#include <nanosoft/easynode.h>
#include <nanosoft/xmlwriter.h>

#include <stdio.h>
#include <string.h>

using namespace nanosoft;

/**
* Конструктор
*
* Создает пустой текстовый блок
*/
EasyNode::EasyNode(): type(EASYNODE_CDATA)
{
	printf("EasyNode[%p] create\n", this);
}

/**
* Конструктор
*
* Создает узел-тег с указанным именем
*/
EasyNode::EasyNode(const char *tag_name): type(EASYNODE_TAG), name(tag_name)
{
	printf("EasyNode[%p] create <%s>\n", this, tag_name);
}

/**
* Деструктор
*/
EasyNode::~EasyNode()
{
	if ( type == EASYNODE_TAG )
	{
		printf("EasyNode[%p] destroy <%s>\n", this, name.c_str());
	}
	else
	{
		printf("EasyNode[%p] destroy\n", this);
	}
}

/**
* Создать узел тег
*/
EasyNode::Ref EasyNode::tag(const char *name)
{
	return new EasyNode(name);
}

/**
* Создать узел CDATA
*/
EasyNode::Ref EasyNode::cdata(const std::string &value)
{
	EasyNode *node = new EasyNode();
	node->text = value;
	return node;
}

/**
* Сериализовать в виде строки
*/
std::string EasyNode::serialize() const
{
	if ( type == EASYNODE_CDATA ) return text;
	
	std::string xml = "<" + name;
	for(attributes_t::const_iterator it = attr.begin(); it != attr.end(); ++it)
	{
		xml += " " + it->first + "=\"" + XMLWriter::escape(it->second) + "\"";
	}
    if( nodes.empty() )
	{
		xml += " />";
	}
	else
	{
		xml += ">";
		for(const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			xml += (*it)->serialize();
		}
		xml += "</" + name + ">";
	}
	
	return xml;
}

/**
* Извлечь CDATA
*
* Соединяет все секции CDATA в одну текстовую строку
*/
std::string EasyNode::cdata() const
{
	if ( type == EASYNODE_CDATA ) return text;
	
	std::string result;
	for(const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		result += (*it)->cdata();
	}
	return result;
}

/**
* Добавить секцию CDATA
*/
void EasyNode::append(const std::string &value)
{
	nodes.push_back( cdata(value) );
}

/**
* Найти первого потомка-тега с указанным именем
*/
EasyNode* EasyNode::find(const char *name) const
{
	for(const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		EasyNode *node = it->getObject();
		if ( node->type == EASYNODE_TAG && node->name == name ) return node;
	}
	return NULL;
}

/**
* Найти тег по указанному пути
*
* Если каких-то промежуточных узлов нет, то они создаются
*/
EasyNode::Ref EasyNode::pickup(const char *path)
{
	EasyNode *child;
	EasyNode *cur = this;
	
	const char *remain = strchr(path, '/');
	while ( remain )
	{
		std::string name(path, remain);
		child = cur->find(name.c_str());
		if ( child == NULL )
		{
			child = new EasyNode(name.c_str());
			cur->nodes.push_back(child);
		}
		cur = child;
		path = remain + 1;
		remain = strchr(path, '/');
	}
	
	child = cur->find(path);
	if ( child == 0 )
	{
		child = new EasyNode(path);
		cur->nodes.push_back(child);
	}
	
	return child;
}
