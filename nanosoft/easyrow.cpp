
#include <nanosoft/easyrow.h>

void EasyRow::Row::copy(const std::map<std::string, std::string> &row)
{
	std::map<std::string, std::string>::operator = (row);
}

/**
* Конструктор по-умолчанию
*
* Создает пустой кортеж
*/
EasyRow::EasyRow()
{
	ref = new Row();
}

/**
* Конструктор копий
*
* Копирует ссылку на кортеж, в результате оба объекта будут ссылаться
* на один набор данных
*/
EasyRow::EasyRow(const EasyRow &row)
{
	ref = row.ref;
}

/**
* Конструктор на основе map<string, string>
*
* Создает новый объект и копирует в него данные из map
*/
EasyRow::EasyRow(const std::map<std::string, std::string> &row)
{
	ref = new Row();
	ref->copy(row);
}

/**
* Деструктор
*
* Освобождает строку. Строка удаляется если на неё больше никто
* не ссылается. Если есть еще ссылки, то строка остается в памяти
*/
EasyRow::~EasyRow()
{
}

/**
* Притворяемся что мы map<>
*
* Если ключа нет, то он автоматически создается. Если автоматическое
* создание недопустимо, то используйте метод get()
*/
std::string& EasyRow::operator [] (const std::string &key)
{
	return ref->operator [] (key); 
}

/**
* Вернуть значение по ключу
*
* Если ключа нет, то вернет пустую строку.
* Сам объект при этом не меняется.
*/
std::string EasyRow::get(const std::string &key) const
{
	const_iterator it = ref->find(key);
	if ( it != ref->end() )
	{
		return it->second;
	}
	
	return std::string();
}

/**
* Вернуть значение по ключу
*
* Если ключа нет, то вернет defval в качестве значения.
* Сам объект при этом не меняется.
*/
std::string EasyRow::get(const std::string &key, const std::string &defval) const
{
	const_iterator it = ref->find(key);
	if ( it != ref->end() )
	{
		return it->second;
	}
	
	return defval;
}

/**
* Установить значение по ключу
*
* Если ключа нет, то он автоматически создается
*/
void EasyRow::set(const std::string &key, const std::string &value)
{
	ref->operator [] (key) = value;
}

/**
* Притворяемся что мы map<>, удалить элемент из строки по ключу
*/
void EasyRow::erase(const std::string &key)
{
	ref->erase(key);
}

/**
* Вернуть список ключей
*/
EasyList EasyRow::keys() const
{
	EasyList result(ref->size());
	
	int i = 0;
	const_iterator it = begin();
	while ( it != end() )
	{
		result.set(i++, it->first);
		++it;
	}
	
	return result;
}

/**
* Вернуть список значений
*/
EasyList EasyRow::values() const
{
	EasyList result(ref->size());
	
	int i = 0;
	const_iterator it = begin();
	while ( it != end() )
	{
		result.set(i++, it->second);
		++it;
	}
	
	return result;
}
