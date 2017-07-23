
#include <nanosoft/easytag.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/**
* Конструктор по умолчанию
*
* Создает пустой текстовый блок
*/
EasyTag::EasyTag(): tag(new EasyNode())
{
}

/**
* Конструктор
*
* Создает тег с указанным именем
*/
EasyTag::EasyTag(const char *tag_name): tag(new EasyNode(tag_name))
{
}

/**
* Конструктор
*
* Создает тег с указанным именем
*/
EasyTag::EasyTag(const std::string &tag_name): tag(new EasyNode(tag_name))
{
}

/**
* Конструктор
*
* Создает тег с указанным именем и атрибутами
*/
EasyTag::EasyTag(const std::string &tag_name, const EasyRow &atts):
	tag(new EasyNode(tag_name, atts))
{
}

/**
* Деструктор
*/
EasyTag::~EasyTag()
{
}

/**
* Установить значение атрибута с форматированием в стиле printf()
*
* Если атрибут уже есть, то сменить его значение
* Если атрибута нет, то добавить атрибут с указанным значением
*/
void EasyTag::setAttributef(const char *name, const char *fmt, ...)
{
	char *value;
	va_list args;
	va_start(args, fmt);
	int len = vasprintf(&value, fmt, args);
	va_end(args);
	tag->attr[name] = value;
	free(value);
}

/**
* Создать дочений тег и вернуть ссылку на него
*/
EasyTag EasyTag::createTag(const std::string &name, const EasyRow &atts)
{
	EasyNode::Ref node = new EasyNode(name, atts);
	tag->append(node);
	return node;
}

/**
* Создать дочений текстовый блок и вернуть ссылку на него
*/
void EasyTag::createText(const std::string &text)
{
	tag->append(text);
}

/**
* Добавить тег/дерево дочерним элементом
*/
void EasyTag::append(EasyTag tree)
{
	tag->append(tree.tag);
}

/**
* Оператор присваивания
*
* Сбрасывает всех потомков и добавляет одну секцию CDATA
* NOTE замещается не текущий тег, а его потомки
* NOTE функция возвращет исходный текст, а не this
*/
const std::string & EasyTag::operator = (const std::string &text)
{
	tag->clear();
	tag->append(text);
	return text;
}
