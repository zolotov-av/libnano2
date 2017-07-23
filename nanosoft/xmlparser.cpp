
#include <nanosoft/xmlparser.h>

#include <stdio.h>

/**
* Конструктор
*/
XMLParser::XMLParser(): parser(NULL), parsing(false), need_close(false), need_reset(false)
{
	if ( ! init() )
	{
		printf("XMLParser() constructor's init failed\n");
	}
}

/**
* Деструктор
*/
XMLParser::~XMLParser()
{
	close();
}

/**
* Обработчик открытия тега
*/
void XMLParser::startElementCallback(void *user_data, const XML_Char *name, const XML_Char **atts)
{
	EasyRow attributes;
	
	for(int i = 0; atts[i]; i += 2)
	{
		attributes[ atts[i] ] = atts[i + 1];
	}
	
	static_cast<XMLParser *>(user_data)->onStartElement(name, attributes);
}

/**
* Отработчик символьных данных
*/
void XMLParser::characterDataCallback(void *user_data, const XML_Char *s, int len)
{
	static_cast<XMLParser *>(user_data)->onCharacterData(std::string(s, len));
}

/**
* Отбработчик закрытия тега
*/
void XMLParser::endElementCallback(void *user_data, const XML_Char *name)
{
	static_cast<XMLParser *>(user_data)->onEndElement(name);
}

/**
* Инициализация парсера
*
* Если парсер уже инициализирован, то вернет FALSE, нельзя вызывать
* из обработчиков (вернет FALSE)
*/
bool XMLParser::init()
{
	// если мы в обработчике, то парсер инициализирован
	// если парсер иниализирован, то вернуть FALSE
	if ( parser ) return false;
	
	parser = XML_ParserCreate((XML_Char *) "UTF-8");
	if ( parser )
	{
		XML_SetUserData(parser, (void*) this);
		XML_SetElementHandler(parser, startElementCallback, endElementCallback);
		XML_SetCharacterDataHandler(parser, characterDataCallback);
		XML_SetDefaultHandler(parser, NULL);
		return true;
	}
	
	return false;
}

/**
* Сброс парсера
*
* Закрывает текущий контекст и открывает новый. Можно вызывать из
* обработчиков событий, тогда парсинг прерывается и сброс будет
* осуществлен по выходу из парсинга
*/
bool XMLParser::reset()
{
	// если мы находимся в обработчике, то установить флаги и вернуть TRUE
	if ( parsing )
	{
		need_close = true;
		need_reset = true;
		return true;
	}
	
	close();
	return init();
}

/**
* Закрытие парсера
*
* Закрывает текущий контекст. Можно вызывать из обработчиков событий.
*/
void XMLParser::close()
{
	// если мы находимся в обработчике, то установить флаги и выйти
	if ( parsing )
	{
		need_close = true;
		need_reset = false;
		return;
	}
	
	if ( parser )
	{
		XML_ParserFree(parser);
		parser = NULL;
	}
}

/**
* Парсинг XML
*
* @param data буфер с данными
* @param len длина буфера с данными
* @param isFinal TRUE - последний кусок, FALSE - будет продолжение
* @return TRUE - успешно, FALSE - ошибка парсинга
*/
bool XMLParser::parseXML(const char *data, size_t len, bool isFinal)
{
	// если парсер не инициализирован, то вернуть FALSE
	if ( ! parser ) return false;
	
	// если парсер вызыван из обработчика (рекурсия), то вернуть FALSE
	if ( parsing ) return false;
	
	parsing = true;
	int r = XML_Parse(parser, data, len, isFinal);
	parsing = false;
	
	if ( need_close )
	{
		XML_ParserFree(parser);
		parser = NULL;
	}
	
	if ( need_reset ) return init();
	
	if ( !r )
	{
		onParseError( XML_ErrorString(XML_GetErrorCode(parser)) );
		return false;
	}
	
	return true;
}
