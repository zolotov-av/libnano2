#ifndef NANOSOFT_EASYNODE_H
#define NANOSOFT_EASYNODE_H

/****************************************************************************

  Попытка переписать поддержку XML (ATXmlTag, TagBuilder, TagHelper)

  Основное планируемое изменение это добавить подсчет ссылок и автоматическое
  удаление тегов, так чтобы гарантированно предотвращать утечки памяти не
  думать о распределении памяти и сосредоточиться только на логике работы.

  В данном файле определяется класс EasyNode который приходит на замену
  классу ATXmlTag. В классе EasyNode для упрощения все внутренние структуры
  и методы открыты публично, непосредственная работа с классом EasyNode
  не предполагается, он будет целиком спрятан в классе EasyTag который
  обеспечивает полноценную и безопасную работу.

****************************************************************************/

#include <nanosoft/object.h>
#include <nanosoft/easyrow.h>

#include <string>
#include <vector>

/**
 * EasyNode представляет XML-тег
 */
#define EASYNODE_TAG 1

/**
 * EasyNode представляет CDATA
 */
#define EASYNODE_CDATA 2

/**
 * Узел XML-документа
 *
 * EasyNode может представлять как XML-тег, так и текстовый блок
 *
 * NOTE Пользователь никогда не должен использовать этот класс напрямую,
 * реальная работа должна осуществляться только через класс EasyTag
 *
 * NOTE Для упрощения класс не использует приватные члены и не обеспечивает
 *   безопасность, т.к. он сам будет сокрыт внутри класса EasyTag и прямая
 *   работа пользователя с этим классом не предполагается.
 */
class EasyNode: public Object
{
public:
	
	/**
	 * Указатель с подсчетом ссылок на EasyNode
	 */
	typedef ptr<EasyNode> Ref;
	
	/**
	 * Список потомков
	 */
	typedef std::vector<Ref> node_list_t;
	
	/**
	 * Итератор
	 */
	typedef node_list_t::const_iterator const_iterator;
	
	/**
	 * Тип узла
	 */
	int type;
	
	/**
	 * Атрибуты тега
	 */
	EasyRow attr;
	
	/**
	 * Имя тега
	 */
	std::string name;
	
	/**
	 * Текстовые данные
	 */
	std::string text;
	
	/**
	 * Ссылка на родителя
	 */
	EasyNode *parent;
	
	/**
	 * Потомки узла
	 */
	node_list_t nodes;
	
	/**
	 * Конструктор
	 *
	 * Создает пустой текстовый блок
	 */
	EasyNode();
	
	/**
	 * Конструктор
	 *
	 * Создает узел-тег с указанным именем
	 */
	EasyNode(const char *tag_name);
	
	/**
	 * Деструктор
	 */
	virtual ~EasyNode();
	
	/**
	 * Создать узел тег
	 */
	static Ref tag(const char *name);
	
	/**
	 * Создать узел CDATA
	 */
	static Ref cdata(const std::string &value);
	
	/**
	 * Сериализовать в виде строки
	 */
	std::string serialize() const;
	
	/**
	 * Извлечь CDATA
	 *
	 * Соединяет все секции CDATA в одну текстовую строку
	 */
	std::string cdata() const;
	
	/**
	 * Добавить секцию CDATA
	 */
	void append(const std::string &value);
	
	/**
	 * Найти первого потомка-тега с указанным именем
	 */
	EasyNode* find(const char *name) const;
	
	/**
	 * Найти тег по указанному пути
	 *
	 * Если каких-то промежуточных узлов нет, то они создаются
	 */
	Ref pickup(const char *path);
};

#endif // NANOSOFT_EASYNODE_H
