
#include <nanosoft/easylist.h>

void EasyList::List::copy(const std::vector<std::string> &list)
{
	std::vector<std::string>::operator = (list);
}

/**
* Конструктор по-умолчанию
*
* Создает пустой список
*/
EasyList::EasyList()
{
	ref = new List();
}

/**
* Конструктор
*
* Создает пустой список на size элементов
*/
EasyList::EasyList(int size)
{
	ref = new List();
	resize(size);
}

/**
* Конструктор копий
*
* Копирует ссылку на объект, в результате оба объекта будут ссылаться
* на один набор данных
*/
EasyList::EasyList(const EasyList &list)
{
	ref = list.ref;
}

/**
* Конструктор на основе std::vector<std::string>
*
* Создает новый объект и копирует в него данные из map
*/
EasyList::EasyList(const std::vector<std::string> &list)
{
	ref = new List();
	ref->copy(list);
}

/**
* Деструктор
*
* Освобождает строку. Строка удаляется если на неё больше никто
* не ссылается. Если есть еще ссылки, то строка остается в памяти
*/
EasyList::~EasyList()
{
}

/**
* Притворяемся что мы vector<>
*
* Возвращает ссылку на элемент по индексу pos.
* Проверка на выход за границы не выполняется.
*/
std::string& EasyList::operator [] (int pos)
{
	return ref->operator [] (pos);
}

/**
* Вернуть значение по индексу
*
* Проверка на выход за границы не выполняется.
*/
std::string EasyList::get(int pos) const
{
	return ref->operator [] (pos);
}

/**
* Установить значение по индексу
*
* Проверка на выход за границы не выполняется.
*/
void EasyList::set(int pos, const std::string &value)
{
	ref->operator [] (pos)  = value;
}

/**
* Установить размер списка
*/
void EasyList::resize(int size)
{
	ref->resize(size);
}

/**
 * Соединить строки в одну строку
 */
std::string implode(const std::string &sep, const EasyList &list)
{
	int count = 0;
	count = list.count();
	if ( count == 0 ) return std::string();
	
	std::string result = list.get(0);
	for(int i = 1; i < count; i++)
	{
		result += sep + list.get(i);
	}
	
	return result;
}
