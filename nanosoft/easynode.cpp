
#include <nanosoft/easynode.h>
#include <nanosoft/xmlwriter.h>

#include <string.h>

using namespace nanosoft;

/**
* Счетчик созданных узлов
*/
int EasyNode::node_created = 0;

/**
* Счетчик удаленных узлов
*/
int EasyNode::node_destroyed = 0;

/**
* Конструктор
*
* Создает пустой текстовый блок
*/
EasyNode::EasyNode(): type(EASYNODE_CDATA), parent(NULL)
{
	node_created++;
}

/**
* Конструктор
*
* Создает узел-тег с указанным именем
*/
EasyNode::EasyNode(const std::string &tag_name): type(EASYNODE_TAG),
	name(tag_name), parent(NULL)
{
	node_created++;
}

/**
* Конструктор
*
* Создает узел-тег с указанным именем и атрибутами
*/
EasyNode::EasyNode(const std::string &tag_name, const EasyRow &atts):
	type(EASYNODE_TAG), name(tag_name), attr(atts), parent(NULL)
{
	node_created++;
}

/**
* Конструктор
*
* Создает копию узла (без дочених элементов)
*/
EasyNode::EasyNode(const Ref &node): type(node->type), name(node->name),
	text(node->text), attr(node->attr.copy()), parent(NULL)
{
	node_created++;
}

/**
* Деструктор
*/
EasyNode::~EasyNode()
{
	node_destroyed++;
	clear();
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
* Создать копию узла/дерева
*/
EasyNode::Ref EasyNode::copy(const Ref &tree)
{
	Ref node = new EasyNode(tree);
	const_iterator end = tree->nodes.end();
	for(const_iterator it = tree->nodes.begin(); it != end; ++it)
	{
		node->append(copy(*it));
	}
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
* Найти голову дерева
*/
EasyNode::Ref EasyNode::head()
{
	EasyNode *node = this;
	while ( node->parent ) node = node->parent;
	return node;
}

/**
* Сбросить всех потомков
*/
void EasyNode::clear()
{
	for(iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		(*it)->parent = NULL;
	}
	nodes.clear();
}

/**
* Добавить секцию CDATA
*/
void EasyNode::append(const std::string &value)
{
	append( cdata(value) );
}

/**
* Добавить узел/дерево
*/
void EasyNode::append(Ref tree)
{
	if ( tree->parent )
	{
		Ref node = copy(tree);
		node->parent = this;
		nodes.push_back(node);
		return;
	}
	else
	{
		tree->parent = this;
		nodes.push_back(tree);
	}
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
			cur->append(child);
		}
		cur = child;
		path = remain + 1;
		remain = strchr(path, '/');
	}
	
	child = cur->find(path);
	if ( child == 0 )
	{
		child = new EasyNode(path);
		cur->append(child);
	}
	
	return child;
}
