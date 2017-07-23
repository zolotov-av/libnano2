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
	typedef node_list_t::iterator iterator;
	
	/**
	 * Константный итератор
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
	 * Счетчик созданных узлов
	 */
	static int node_created;
	
	/**
	 * Счетчик удаленных узлов
	 */
	static int node_destroyed;
	
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
	explicit EasyNode(const std::string &tag_name);
	
	/**
	 * Конструктор
	 *
	 * Создает узел-тег с указанным именем и атрибутами
	 */
	EasyNode(const std::string &tag_name, const EasyRow &atts);
	
	/**
	 * Конструктор
	 *
	 * Создает копию узла (без дочених элементов)
	 */
	EasyNode(const Ref &node);
	
	/**
	 * Деструктор
	 */
	virtual ~EasyNode();
	
	/**
	 * Создать узел CDATA
	 */
	static Ref cdata(const std::string &value);
	
	/**
	 * Создать копию узла/дерева
	 */
	static Ref copy(const Ref &tree);
	
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
	 * Найти голову дерева
	 */
	Ref head();
	
	/**
	 * Сбросить всех потомков
	 */
	void clear();
	
	/**
	 * Добавить секцию CDATA
	 */
	void append(const std::string &value);
	
	/**
	 * Добавить узел/дерево
	 */
	void append(Ref tree);
	
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
