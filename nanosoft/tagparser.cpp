
#include <nanosoft/tagparser.h>
#include <nanosoft/filestream.h>

#include <stdio.h>

static EasyTag error_tag(const std::string &msg)
{
	EasyTag node;
	node.setText(msg);
	return node;
}

/**
* Конструктор
*/
TagParser::TagParser(): depth(0)
{
}

/**
* Деструктор
*/
TagParser::~TagParser()
{
}

/**
* Парсинг буфера
*
* В случае ошибки возвращается CDATA с сообщением об ошибке
*/
EasyTag TagParser::parse(void *buf, size_t buf_len)
{
	if ( parseXML(buf, buf_len, true) )
	{
		return tag;
	}
	
	return error_tag(error);
}

/**
* Парсинг файла
*
* В случае ошибки возвращается CDATA с сообщением об ошибке
*/
EasyTag TagParser::parseFile(const char *path)
{
	FileStream file;
	if ( ! file.open(path, FileStream::ro) )
	{
		return error_tag("file not found");
	}
	return parseStream(file);
}

/**
* Парсинг произвольного потока
*
* В случае ошибки возвращается CDATA с сообщением об ошибке
*/
EasyTag TagParser::parseStream(stream &s)
{
	char buf[4096];
	size_t ret;
	
	do
	{
		ret = s.read(buf, sizeof(buf));
		if ( ! parseXML(buf, ret, ret == 0) )
		{
			return error_tag(error);
		}
	}
	while ( ret > 0 );
	
	return tag;
}

/**
* Парсинг строки
*
* В случае ошибки возвращается CDATA с сообщением об ошибке
*/
EasyTag TagParser::parseString(const std::string &text)
{
	if ( parseXML(text.c_str(), text.length(), true) )
	{
		return tag;
	}
	
	return error_tag(error);
}

/**
* Обработчик открытия тега
*/
void TagParser::onStartElement(const std::string &name, const EasyRow &atts)
{
	depth ++;
	if ( depth == 1 )
	{
		cur = tag = EasyTag(name, atts);
	}
	else
	{
		cur = cur.createTag(name, atts);
	}
}

/**
* Обработчик символьных данных
*/
void TagParser::onCharacterData(const std::string &cdata)
{
	if ( depth > 0 )
	{
		cur.createText(cdata);
	}
}

/**
* Обработчик закрытия тега
*/
void TagParser::onEndElement(const std::string &name)
{
	if ( depth == 1 )
	{
		// ok
	}
	else
	{
		cur = cur.parent();
	}
	depth--;
}

/**
* Обработчик ошибок парсера
*/
void TagParser::onParseError(const char *message)
{
	printf("TagParser::onParseError(%s)\n", message);
	error = message;
}
