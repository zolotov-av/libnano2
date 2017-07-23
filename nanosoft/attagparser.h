#ifndef MAWAR_ATTAGPARSER_H
#define MAWAR_ATTAGPARSER_H

#include <nanosoft/stream.h>
#include <nanosoft/tagbuilder.h>
#include <nanosoft/xmlparser.h>
#include <nanosoft/xml_types.h>
#include <nanosoft/xml_tag.h>
#include <nanosoft/easyrow.h>

/**
* Паресер XML-файлов
*
* ATTagParser парсит файл целиком в один ATXmlTag
*/
class ATTagParser: protected XMLParser, protected ATTagBuilder
{
public:
	/**
	* Конструктор парсера
	*/
	ATTagParser();
	
	/**
	* Деструктор парсера
	*/
	~ATTagParser();
	
	/**
	* Парсинг файла
	* @param path путь к файлу
	* @return тег в случае успеха и NULL в случае ошибки
	*/
	ATXmlTag * parseFile(const char *path);
	ATXmlTag * parseFile(const std::string &path);
	
	/**
	* Парсинг произвольного потока
	* @param s поток с данными
	* @return тег в случае успеха и NULL в случае ошибки
	*/
	ATXmlTag * parseStream(stream &s);
	
	/**
	* Парсинг строки
	* @param xml XML
	* @return тег в случае успеха и NULL в случае ошибки
	*/
	ATXmlTag * parseString(const std::string &xml);
	
protected:
	/**
	* Обработчик открытия тега
	*/
	virtual void onStartElement(const std::string &name, const EasyRow &atts);
	
	/**
	* Обработчик символьных данных
	*/
	virtual void onCharacterData(const std::string &cdata);
	
	/**
	* Обработчик закрытия тега
	*/
	virtual void onEndElement(const std::string &name);
	
	/**
	* Обработчик ошибок парсера
	*/
	void onParseError(const char *message);
private:
	/**
	* Глубина обрабатываемого тега
	*/
	int depth;
};

/**
* Парсинг файла
* @param path путь к файлу
* @return тег в случае успеха и NULL в случае ошибки
*/
ATXmlTag * parse_xml_file(const char *path);
ATXmlTag * parse_xml_file(const std::string &path);

/**
* Парсинг произвольного потока
* @param s поток с данными
* @return тег в случае успеха и NULL в случае ошибки
*/
ATXmlTag * parse_xml_stream(stream &s);

/**
* Парсинг произвольной строки
* @param xml XML
* @return тег в случае успеха и NULL в случае ошибки
*/
ATXmlTag * parse_xml_string(const std::string &xml);

#endif // MAWAR_ATTAGPARSER_H
