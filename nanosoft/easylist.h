#ifndef NANOSOFT_EASYLIST_H
#define NANOSOFT_EASYLIST_H

#include <nanosoft/object.h>

#include <string>
#include <vector>

/**
 * Простой список строк
 *
 * EasyList предоставляет семантику массива строк.
 *
 * @NOTE Класс использует семантику умных указателей. По логике реализации
 * он является указателем на автоматически создаваемый объект. Когда один объект
 * копирует другого, оба объекта начинают ссылаться на одни и те же реальные
 * данные и оба могут модифицировать данные.
 */
class EasyList
{
private:
	
	/**
	 * Собственно объект хранящий данные
	 */
	class List: public Object, public std::vector<std::string>
	{
	public:
		void copy(const std::vector<std::string> &list);
	};
	
	/**
	 * Указатель с подсчетом ссылок
	 */
	ptr<List> ref;
	
public:
	
	/**
	 * Конструктор по-умолчанию
	 *
	 * Создает пустой список
	 */
	EasyList();
	
	/**
	 * Конструктор
	 *
	 * Создает пустой список на size элементов
	 */
	EasyList(int size);
	
	/**
	 * Конструктор копий
	 *
	 * Копирует ссылку на объект, в результате оба объекта будут ссылаться
	 * на один набор данных
	 */
	EasyList(const EasyList &list);
	
	/**
	 * Конструктор на основе std::vector<std::string>
	 *
	 * Создает новый объект и копирует в него данные из map
	 */
	EasyList(const std::vector<std::string> &list);
	
	/**
	 * Деструктор
	 *
	 * Освобождает строку. Строка удаляется если на неё больше никто
	 * не ссылается. Если есть еще ссылки, то строка остается в памяти
	 */
	~EasyList();
	
	/**
	 * Оператор присваивания
	 *
	 * Берет ссылку на новый объект, старый отсоединяется, оба объекта начинают
	 * указывать на один набор данных
	 */
	inline EasyList& operator = (const EasyList &list) { ref = list.ref; return *this; }
	
	/**
	 * Притворяемся что мы vector<>
	 *
	 * Возвращает ссылку на элемент по индексу pos.
	 * Проверка на выход за границы не выполняется.
	 */
	std::string& operator [] (int pos);
	
	/**
	 * Вернуть значение по индексу
	 *
	 * Проверка на выход за границы не выполняется.
	 */
	std::string get(int pos) const;
	
	/**
	 * Установить значение по индексу
	 *
	 * Проверка на выход за границы не выполняется.
	 */
	void set(int pos, const std::string &value);
	
	/**
	 * Возвращает число строк в списке
	 */
	inline int size() const { return ref->size(); }
	
	/**
	 * Возвращает число строк в списке
	 */
	inline int count() const { return ref->size(); }
	
	/**
	 * Установить размер списка
	 */
	void resize(int size);
	
};

/**
 * Соединить строки в одну строку
 */
std::string implode(const std::string &sep, const EasyList &list);

#endif // NANOSOFT_EASYLIST_H
