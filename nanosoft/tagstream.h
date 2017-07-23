#ifndef NANOSOFT_TAGSTREAM_H
#define NANOSOFT_TAGSTREAM_H

/****************************************************************************

  Попытка переписать поддержку XML (ATXmlTag, TagBuilder, TagHelper)

  Основное планируемое изменение это добавить подсчет ссылок и автоматическое
  удаление тегов, так чтобы гарантированно предотвращать утечки памяти не
  думать о распределении памяти и сосредоточиться только на логике работы.

  В данном файле определяется класс TagStream который осуществляет парсинг
  XML-потоков приходит на замену связке AsyncXMLStream+ATTagBuilder.

****************************************************************************/

#include <nanosoft/xmlparser.h>
#include <nanosoft/easytag.h>

class TagStream: public XMLParser
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
	
public:
	
	/**
	 * Конструктор
	 */
	TagStream();
	
	/**
	 * Деструктор
	 */
	virtual ~TagStream();
	
protected:
	
	/**
	 * Обработчик открытия тега
	 */
	virtual void onStartElement(const std::string &name, const attributes_t &atts);
	
	/**
	 * Обработчик символьных данных
	 */
	virtual void onCharacterData(const std::string &cdata);
	
	/**
	 * Обработчик закрытия тега
	 */
	virtual void onEndElement(const std::string &name);
	
	/**
	 * Событие: начало потока
	 */
	virtual void onStartStream(const std::string &name, const attributes_t &attributes) = 0;
	
	/**
	 * Событие: конец потока
	 */
	virtual void onEndStream() = 0;
	
	/**
	 * Обработчик станзы
	 */
	virtual void onStanza(EasyTag stanza) = 0;
	
};

#endif // NANOSOFT_TAGSTREAM_H
