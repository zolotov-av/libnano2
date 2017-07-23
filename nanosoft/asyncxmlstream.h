#ifndef NANOSOFT_ASYNCXMLSTREAM_H
#define NANOSOFT_ASYNCXMLSTREAM_H

#include <nanosoft/asyncstream.h>
#include <nanosoft/xmlparser.h>
#include <nanosoft/easyrow.h>

#include <errno.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

/**
* Асинхронный парсер XML потоков
*/
class AsyncXMLStream: public AsyncStream, public XMLParser
{
protected:
	/**
	* Обработчик прочитанных данных
	*/
	virtual void onRead(const char *data, size_t len);
	
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
	virtual void onParseError(const char *message);
	
	/**
	* Пир (peer) закрыл поток.
	*
	* Мы уже ничего не можем отправить в ответ,
	* можем только корректно закрыть соединение с нашей стороны.
	*/
	virtual void onPeerDown();
public:
	/**
	* Конструктор
	*/
	AsyncXMLStream(int afd);
	
	/**
	* Деструктор
	*/
	virtual ~AsyncXMLStream();
};

#endif // NANOSOFT_ASYNCXMLSTREAM_H
