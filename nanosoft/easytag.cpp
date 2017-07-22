
#include <nanosoft/easytag.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/**
* Конструктор по умолчанию
*
* Создает пустой текстовый блок
*/
EasyTag::EasyTag()
{
	tag = new EasyNode();
}

/**
* Конструктор
*
* Создает тег с указанным именем
*/
EasyTag::EasyTag(const char *tag_name)
{
	tag = EasyNode::tag(tag_name);
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
* Оператор присваивания
*
* Удаляет всех потомков и добавляет одну секцию CDATA
*/
const std::string & EasyTag::operator = (const std::string &text)
{
	tag->clear();
	tag->append(text);
	return text;
}

/**
* Оператор добавления CDATA
*
* Добавить секцию CDATA в конец тега
*/
void EasyTag::operator += (const std::string &text)
{
	tag->append(text);
}

/**
* Оператор добавления подтега
*
* Добавить подтега в конец тега
*/
void EasyTag::operator += (EasyTag t)
{
	tag->append(t.tag);
}
