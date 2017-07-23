#ifndef NANOSOFT_EASYTAG_H
#define NANOSOFT_EASYTAG_H

/****************************************************************************

  Попытка переписать поддержку XML (ATXmlTag, TagBuilder, TagHelper)

  Основное планируемое изменение это добавить подсчет ссылок и автоматическое
  удаление тегов, так чтобы гарантированно предотвращать утечки памяти не
  думать о распределении памяти и сосредоточиться только на логике работы.

  В данном файле определяется класс EasyTag который приходит на замену
  классу TagHelper. Класс EasyTag инкапсулирует в себе ссылки на объекты
  EasyNode, заботиться о выделении и освобождении памяти. Объект EasyTag
  всегда ссылается на рабочий EasyNode, т.е. никогда не бывает NULL, так
  что нет необходимости делать проверки на NULL.

****************************************************************************/

#include <nanosoft/easynode.h>

/**
 * Тег XML-документа
 * 
 * Класс EasyTag может представлять одиночный XML-тег, дерево XML-тегов или
 * просто текстовый блок.
 * 
 * Класс EasyTag приходит на замену классу TagHelper и предоставляет
 * аналогичный функционал, с тем отличием, что EasyTag никогда не бывает NULL,
 * обеспечивает подсчет ссылок и автоматическое удаление объектов, так что
 * пользователю можно совсем не думать распределении памяти (хотя проблемы
 * фрагментации памяти потенциально могут оставаться).
 */
class EasyTag
{
private:
	
	/**
	 * Ссылка на тег
	 */
	EasyNode::Ref tag;
	
	/**
	 * Конструктор
	 */
	EasyTag(EasyNode::Ref ref): tag(ref) { }
	
	/**
	 * Конструктор
	 */
	EasyTag(EasyNode *ptr): tag(ptr) { }
	
public:
	
	/**
	 * Конструктор по умолчанию
	 *
	 * Создает пустой текстовый блок
	 */
	EasyTag();
	
	/**
	 * Конструктор
	 *
	 * Создает тег с указанным именем
	 */
	explicit EasyTag(const char *tag_name);
	
	/**
	 * Конструктор
	 *
	 * Создает тег с указанным именем
	 */
	explicit EasyTag(const std::string &tag_name);
	
	/**
	 * Конструктор
	 *
	 * Создает тег с указанным именем и атрибутами
	 */
	EasyTag(const std::string &tag_name, const EasyRow &atts);
	
	/**
	 * Деструктор
	 */
	~EasyTag();
	
	/**
	 * Вернуть имя тега
	 */
	std::string name() const { return tag->name; }
	
	/**
	 * Установить имя тега
	 */
	void setName(const char *name) { tag->name = name; }
	
	/**
	 * Вернуть ссылку атрибуты
	 */
	EasyRow& attr() { return tag->attr; }
	
	/**
	 * Установить ссылку на атрибуты
	 */
	void setAttr(const EasyRow &attr) { tag->attr = attr; }
	
	/**
	 * Вернуть содержимое CDATA
	 */
	std::string text() const { return tag->text; }
	
	/**
	 * Установить содержимое CDATA
	 */
	void setText(const std::string &text) { tag->text = text; }
	
	/**
	 * Проверить существование атрибута
	 */
	bool hasAttribute(const std::string &name) const { return tag->attr.exists(name); }
	
	/**
	 * Проверить существование атрибута
	 */
	bool hasAttribute(const char *name) const { return tag->attr.exists(name); }
	
	/**
	 * Вернуть значение атрибута
	 */
	std::string getAttribute(const std::string &name, const std::string &defval = "") const { return tag->attr.get(name, defval); }
	
	/**
	 * Вернуть значение атрибута
	 */
	const std::string getAttribute(const char *name, const char *defval = "") const { return tag->attr.get(name, defval); }
	
	/**
	 * Установить значение атрибута
	 *
	 * Если атрибут уже есть, то сменить его значение
	 * Если атрибута нет, то добавить атрибут с указанным значением
	 */
	void setAttribute(const std::string &name, const std::string &value) { tag->attr.set(name, value); }
	
	/**
	 * Установить значение атрибута
	 *
	 * Если атрибут уже есть, то сменить его значение
	 * Если атрибута нет, то добавить атрибут с указанным значением
	 */
	void setAttribute(const char *name, const std::string &value) { tag->attr.set(name, value); }
	
	/**
	 * Установить значение атрибута
	 *
	 * Если атрибут уже есть, то сменить его значение
	 * Если атрибута нет, то добавить атрибут с указанным значением
	 */
	void setAttribute(const char *name, const char *value) { tag->attr.set(name, value); }
	
	/**
	 * Установить значение атрибута с форматированием в стиле printf()
	 *
	 * Если атрибут уже есть, то сменить его значение
	 * Если атрибута нет, то добавить атрибут с указанным значением
	 */
	void setAttributef(const char *name, const char *fmt, ...);
	
	/**
	 * Удалить атрибут
	 *
	 * Если атрибута нет, то ничего не делать - это не ошибка
	 */
	void removeAttribute(const std::string &name) { tag->attr.erase(name); }
	
	/**
	 * Сериализовать тег в строку
	 */
	std::string serialize() const { return tag->serialize(); }
	
	/**
	 * Сериализовать тег в строку
	 */
	std::string cdata() const { return tag->cdata(); }
	
	/**
	 * Сбросить всех потомков
	 */
	void clear() { tag->clear(); }
	
	/**
	 * Создать дочений тег и вернуть ссылку на него
	 */
	EasyTag createTag(const std::string &name, const EasyRow &atts);
	
	/**
	 * Создать дочений текстовый блок и вернуть ссылку на него
	 */
	void createText(const std::string &text);
	
	/**
	 * Добавить тег/дерево дочерним элементом
	 */
	void append(EasyTag tree);
	
	/**
	 * Создать копию дерева
	 */
	EasyTag copy() const { return EasyNode::copy(tag); }
	
	/**
	 * Найти голову дерева
	 */
	EasyTag head();
	
	/**
	 * Вернуть ссылку на предка
	 *
	 * Если предка нет, то вернет сам себя
	 */
	EasyTag parent() const;
	
	/**
	 * Индексный оператор
	 *
	 * Ищет первый тег по указанному пути, если каких-то промежуточных узлов
	 * нет, то они создаются
	 */
	EasyTag operator [] (const char *path) { return tag->pickup(path); }
	EasyTag operator [] (const std::string &path) { return tag->pickup(path.c_str()); }
	
	/**
	 * Преобразование к строке
	 *
	 * Возращает рекурсивную конкатенацию всех секций CDATA
	 */
	operator std::string () const { return tag->cdata(); }
	
	/**
	 * Оператор присваивания
	 *
	 * Сбрасывает всех потомков и добавляет одну секцию CDATA
	 * NOTE замещается не текущий тег, а его потомки
	 * NOTE функция возвращет исходный текст, а не this
	 */
	const std::string & operator = (const std::string &text);
	
	/**
	 * Оператор добавления CDATA
	 *
	 * Добавить секцию CDATA в конец тега
	 */
	void operator += (const std::string &text) { createText(text); }
	
	/**
	 * Оператор добавления подтега
	 *
	 * Добавить подтега в конец тега
	 */
	void operator += (EasyTag tree) { append(tree); }
	
};

#endif // NANOSOFT_EASYTAG_H
