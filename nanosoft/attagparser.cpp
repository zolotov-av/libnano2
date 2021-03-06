
#include <nanosoft/attagparser.h>
#include <nanosoft/filestream.h>

/**
* Конструктор парсера
*/
ATTagParser::ATTagParser()
{
}

/**
* Деструктор потока
*/
ATTagParser::~ATTagParser()
{
}

/**
* Парсинг файла
* @param path путь к файлу
* @return тег в случае успеха и NULL в случае ошибки
*/
ATXmlTag * ATTagParser::parseFile(const char *path)
{
	FileStream file;
	if ( ! file.open(path, FileStream::ro) ) {
		std::cerr << "[ATTagParser] file not found: " << path << std::endl;
		return 0;
	}
	return parseStream(file);
}

/**
* Парсинг файла
* @param path путь к файлу
* @return тег в случае успеха и NULL в случае ошибки
*/
ATXmlTag * ATTagParser::parseFile(const std::string &path)
{
	return parseFile(path.c_str());
}

/**
* Парсинг произвольного потока
* @param path путь к файлу
* @return тег в случае успеха и NULL в случае ошибки
*/
ATXmlTag * ATTagParser::parseStream(stream &s)
{
	depth = 0;
	char buf[4096];
	while ( 1 )
	{
		int r = s.read(buf, sizeof(buf));
		
		if ( r < 0 )
		{
			ATTagBuilder::reset();
			return 0;
		}
		
		if ( r == 0 )
		{
			return fetchResult();
		}
		
		if ( ! parseXML(buf, r, false) )
		{
			ATTagBuilder::reset();
			return 0;
		}
	}
}

/**
* Парсинг строки
* @param xml XML
* @return тег в случае успеха и NULL в случае ошибки
*/
ATXmlTag * ATTagParser::parseString(const std::string &xml)
{
	depth = 0;
	if ( parseXML(xml.c_str(), xml.length(), true) )
	{
		return fetchResult();
	}
	return 0;
}


/**
* Обработчик открытия тега
*/
void ATTagParser::onStartElement(const std::string &name, const EasyRow &atts)
{
	depth ++;
	startElement(name, atts, depth);
}

/**
* Обработчик символьных данных
*/
void ATTagParser::onCharacterData(const std::string &cdata)
{
	characterData(cdata);
}

/**
* Обработчик закрытия тега
*/
void ATTagParser::onEndElement(const std::string &name)
{
	endElement(name);
	depth --;
}

/**
* Обработчик ошибок парсера
*/
void ATTagParser::onParseError(const char *message)
{
	std::cerr << "ATTagParser::onParseError: "<< message << std::endl;
}

/**
* Парсинг файла
* @param path путь к файлу
* @return тег в случае успеха и NULL в случае ошибки
*/
ATXmlTag * parse_xml_file(const char *path)
{
	ATTagParser parser;
	return parser.parseFile(path);
}

/**
* Парсинг файла
* @param path путь к файлу
* @return тег в случае успеха и NULL в случае ошибки
*/
ATXmlTag * parse_xml_file(const std::string &path)
{
	ATTagParser parser;
	return parser.parseFile(path);
}

/**
* Парсинг произвольного потока
* @param s поток с данными
* @return тег в случае успеха и NULL в случае ошибки
*/
ATXmlTag * parse_xml_stream(stream &s)
{
	ATTagParser parser;
	return parser.parseStream(s);
}

/**
* Парсинг произвольной строки
* @param xml XML
* @return тег в случае успеха и NULL в случае ошибки
*/
ATXmlTag * parse_xml_string(const std::string &xml)
{
	ATTagParser parser;
	return parser.parseString(xml);
}
