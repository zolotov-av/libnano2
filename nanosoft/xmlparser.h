#ifndef NANOSOFT_XMLPARSER_H
#define NANOSOFT_XMLPARSER_H

#include <nanosoft/easyrow.h>

#include <expat.h>
#include <string>

/**
 * XML парсер
 */
class XMLParser
{
private:
	
	/**
	 * Парсер expat
	 */
	XML_Parser parser;
	
	/**
	 * Признак парсинга
	 * TRUE - парсер в состоянии обработка куска файла, т.е. мы находимся
	 * в обработчке события
	 */
	bool parsing;
	
	/**
	 * Признак необходимости прервать парсинг и закрыть контекст
	 */
	bool need_close;
	
	/**
	 * Признак необходимости инициализировать контекст повторно
	 *
	 * Бывают случаи когда в обработчике события возникает необходимость
	 * прервать парсинг, и начать парсить новый поток.
	 *
	 * Реализованно для поддержки XMPP в jabber-сервере maward
	 */
	bool need_reset;
	
	/**
	 * Конструктор копий запрещен
	 */
	XMLParser(const XMLParser &p) { }
	
	/**
	 * Оператор присваивания запрещен
	 */
	XMLParser& operator = (const XMLParser &) { return *this; }
	
public:
	
	/**
	 * Конструктор
	 */
	XMLParser();
	
	/**
	 * Деструктор
	 */
	virtual ~XMLParser();
	
protected:
	
	/**
	 * Обработчик открытия тега
	 */
	static void startElementCallback(void *user_data, const XML_Char *name, const XML_Char **atts);
	
	/**
	 * Отработчик символьных данных
	 */
	static void characterDataCallback(void *user_data, const XML_Char *s, int len);
	
	/**
	 * Отбработчик закрытия тега
	 */
	static void endElementCallback(void *user_data, const XML_Char *name);
	
	/**
	 * Обработчик открытия тега
	 */
	virtual void onStartElement(const std::string &name, const EasyRow &atts) = 0;
	
	/**
	 * Обработчик символьных данных
	 */
	virtual void onCharacterData(const std::string &cdata) = 0;
	
	/**
	 * Обработчик закрытия тега
	 */
	virtual void onEndElement(const std::string &name) = 0;
	
	/**
	 * Обработчик ошибок парсера
	 */
	virtual void onParseError(const char *message) = 0;
	
public:
	
	/**
	 * Инициализация парсера
	 *
	 * Если парсер уже инициализирован, то вернет FALSE, нельзя вызывать
	 * из обработчиков (вернет FALSE)
	 */
	bool init();
	
	/**
	 * Сброс парсера
	 *
	 * Закрывает текущий контекст и открывает новый. Можно вызывать из
	 * обработчиков событий, тогда парсинг прерывается и сброс будет
	 * осуществлен по выходу из парсинга
	 */
	bool reset();
	
	/**
	 * Закрытие парсера
	 *
	 * Закрывает текущий контекст. Можно вызывать из обработчиков событий.
	 */
	void close();
	
	/**
	 * Парсинг XML
	 *
	 * Нельзя вызывать из обработчиков событий - вернет FALSE
	 *
	 * @param data буфер с данными
	 * @param len длина буфера с данными
	 * @param isFinal TRUE - последний кусок, FALSE - будет продолжение
	 * @return TRUE - успешно, FALSE - ошибка парсинга
	 */
	bool parseXML(const void *data, size_t len, bool isFinal);
	
};

#endif // NANOSOFT_XMLPARSER_H
