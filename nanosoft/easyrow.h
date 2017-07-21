#ifndef NANOSOFT_EASYROW_H
#define NANOSOFT_EASYROW_H

#include <nanosoft/object.h>
#include <nanosoft/easylist.h>

#include <map>
#include <string>

/**
 * Простая запись/кортеж
 *
 * По сути это просто ассоциативный массив строк map<string, string>
 * который в разных частях библиотеки может выполнять различные функции.
 * Первичное назначение - представлять запись в таблице базы данных.
 * Но это также может быть базовый элемент для компонент QListView, QTreeView,
 * QTableView, представлять набор атрибутов XML-тега или что-нибудь еще.
 *
 * @NOTE Класс использует семантику умных указателей. По логике реализации
 * он является указателем на автоматически создаваемый объект. Когда один объект
 * копирует другого, оба объекта начинают ссылаться на одни и те же реальные
 * данные и оба могут модифицировать данные.
 */
class EasyRow
{
private:
	
	/**
	 * Собственно объект хранящий данные
	 */
	class Row: public Object, public std::map<std::string, std::string>
	{
	public:
		void copy(const std::map<std::string, std::string> &row);
	};
	
	/**
	 * Указатель с подсчетом ссылок
	 */
	ptr<Row> ref;
	
public:
	
	/**
	 * Притворяемся что мы map<>
	 *
	 * Реализовано для совместимости с STL, но использовать не рекомендуется
	 */
	typedef Row::const_iterator const_iterator;
	
	/**
	 * Конструктор по-умолчанию
	 *
	 * Создает пустой кортеж
	 */
    EasyRow();
	
	/**
	 * Конструктор копий
	 *
	 * Копирует ссылку на кортеж, в результате оба объекта будут ссылаться
	 * на один набор данных
	 */
	EasyRow(const EasyRow &row);
	
	/**
	 * Конструктор на основе map<string, string>
	 *
	 * Создает новый объект и копирует в него данные из map
	 */
	EasyRow(const std::map<std::string, std::string> &row);
	
	/**
	 * Деструктор
	 *
	 * Освобождает строку. Строка удаляется если на неё больше никто
	 * не ссылается. Если есть еще ссылки, то строка остается в памяти
	 */
	~EasyRow();
	
	/**
	 * Притворяемся что мы map<>
	 *
	 * Реализовано для совместимости с STL, но использовать не рекомендуется
	 */
	inline const_iterator begin() const { return ref->begin(); }
	
	/**
	 * Притворяемся что мы map<>
	 *
	 * Реализовано для совместимости с STL, но использовать не рекомендуется
	 */
	inline const_iterator end() const { return ref->end(); }
	
	/**
	 * Оператор присваивания
	 *
	 * Берет ссылку на новый объект, старый отсоединяется, оба объекта начинают
	 * указывать на один набор данных
	 */
	inline EasyRow& operator = (const EasyRow &row) { ref = row.ref; }
	
	/**
	 * Притворяемся что мы map<>
	 *
	 * Если ключа нет, то он автоматически создается. Если автоматическое
	 * создание недопустимо, то используйте метод get()
	 */
	std::string& operator [] (const std::string &key);
	
	/**
	 * Вернуть значение по ключу
	 *
	 * Если ключа нет, то вернет пустую строку.
	 * Сам объект при этом не меняется.
	 */
	std::string get(const std::string &key) const;
	
	/**
	 * Вернуть значение по ключу
	 *
	 * Если ключа нет, то вернет defval в качестве значения.
	 * Сам объект при этом не меняется.
	 */
	std::string get(const std::string &key, const std::string &defval) const;
	
	/**
	 * Установить значение по ключу
	 *
	 * Если ключа нет, то он автоматически создается
	 */
	void set(const std::string &key, const std::string &value);
	
	/**
	 * Притворяемся что мы map<>, удалить элемент из строки по ключу
	 */
	void erase(const std::string &key);
	
	/**
	 * Удалить элемент из строки по ключу,
	 * более очевидный псевдоним для функции erase
	 */
	inline void remove(const std::string &key) { erase(key); }
	
	/**
	 * Вернуть список ключей
	 */
	EasyList keys() const;
	
	/**
	 * Вернуть список значений
	 */
	EasyList values() const;
	
	/**
	 * Вернуть ссылку на map<>
	 *
	 * Использовать не рекомендуется
	 */
	inline std::map<std::string, std::string>& map() { return *(ref.getObject()); }
	
};

#endif // NANOSOFT_EASYROW_H
