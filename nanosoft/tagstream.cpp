
#include <nanosoft/tagstream.h>

/**
* Конструктор
*/
TagStream::TagStream(): depth(0)
{
}

/**
* Деструктор
*/
TagStream::~TagStream()
{
}

/**
* Обработчик открытия тега
*/
void TagStream::onStartElement(const std::string &name, const EasyRow &atts)
{
	depth ++;
	switch ( depth )
	{
	case 1:
		onStartStream(name, atts);
		break;
	case 2: // начало станзы
		cur = tag = EasyTag(name, atts);
		break;
	default: // добавить тег в станзу
		cur = cur.createTag(name, atts);
	}
}

/**
* Обработчик символьных данных
*/
void TagStream::onCharacterData(const std::string &cdata)
{
	if ( depth > 1 )
	{
		cur.createText(cdata);
	}
}

/**
* Обработчик закрытия тега
*/
void TagStream::onEndElement(const std::string &name)
{
	switch (depth)
	{
	case 1:
		onEndStream();
		break;
	case 2: {
		onStanza(tag);
		break;
	}
	default:
		cur = cur.parent();
	}
	depth --;
}
