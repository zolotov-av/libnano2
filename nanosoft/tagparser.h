#ifndef NANOSOFT_TAGPARSER_H
#define NANOSOFT_TAGPARSER_H

/****************************************************************************

  Попытка переписать поддержку XML (ATXmlTag, TagBuilder, TagHelper)

  Основное планируемое изменение это добавить подсчет ссылок и автоматическое
  удаление тегов, так чтобы гарантированно предотвращать утечки памяти не
  думать о распределении памяти и сосредоточиться только на логике работы.

  В данном файле определяется класс TagParser который приходит на замену
  классу ATTagParser.

****************************************************************************/

#include <nanosoft/stream.h>
#include <nanosoft/easyrow.h>
#include <nanosoft/easytag.h>
#include <nanosoft/xmlparser.h>

/**
 * Парсер XML-тегов
 */
class TagParser: public XMLParser
{
private:
	
	/**
	 * Глубина обрабатываемого тега
	 */
	int depth;
	
	/**
	 * Ссылка на голову тега
	 */
	EasyTag tag;
	
	/**
	 * Ссылка на текущий обрабатываемый тег
	 */
	EasyTag cur;
	
	/**
	 * Сообщение об ошибке
	 */
	std::string error;
	
	/**
	 * Конструктор копий запрещен
	 */
	TagParser(const TagParser &p) { }
	
	/**
	 * Оператор присваивания запрещен
	 */
	TagParser& operator = (const TagParser &) { return *this; }
	
public:
	
	/**
	 * Конструктор
	 */
	TagParser();
	
	/**
	 * Деструктор
	 */
	virtual ~TagParser();
	
	/**
	 * Парсинг буфера
	 *
	 * В случае ошибки возвращается CDATA с сообщением об ошибке
	 */
	EasyTag parse(void *buf, size_t buf_len);
	
	/**
	 * Парсинг файла
	 *
	 * В случае ошибки возвращается CDATA с сообщением об ошибке
	 */
	EasyTag parseFile(const char *path);
	
	/**
	 * Парсинг файла
	 *
	 * В случае ошибки возвращается CDATA с сообщением об ошибке
	 */
	EasyTag parseFile(const std::string &path) { return parseFile(path.c_str()); }
	
	/**
	 * Парсинг произвольного потока
	 *
	 * В случае ошибки возвращается CDATA с сообщением об ошибке
	 */
	EasyTag parseStream(stream &s);
	
	/**
	 * Парсинг строки
	 *
	 * В случае ошибки возвращается CDATA с сообщением об ошибке
	 */
	EasyTag parseString(const std::string &text);
	
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
	virtual void onParseError(const char *message);
	
};

#endif // NANOSOFT_TAGPARSER_H
