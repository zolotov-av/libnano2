
#include <nanosoft/easyresultset.h>

/**
* Конструктор по умолчанию, создает пустой набор данных
*/
EasyResultSet::EasyResultSet()
{
	ref = new ResultSet();
}

/**
* Конструктор
*
* Пустой набор данных на size строк (кортежей).
*/
EasyResultSet::EasyResultSet(int size)
{
	ref = new ResultSet();
	resize(size);
}

/**
* Конструктор копий, копирует ссылку на реальный объект,
* в результате оба объекта ссылаются реальный набор данных
*/
EasyResultSet::EasyResultSet(const EasyResultSet &r): ref(r.ref)
{
}

/**
* Деструктор, совобождает ссылку. Если на объект никто не ссылается
* то он удаляется. Если ссылки еще есть, то продолжает жить.
*/
EasyResultSet::~EasyResultSet()
{
}

/**
* Установить размер набора данных (число строк)
*/
void EasyResultSet::resize(int size)
{
	ref->resize(size);
}

/**
* Очистить список
*/
void EasyResultSet::clear()
{
	ref->clear(); 
}

/**
* Сбросить сисок, данные не очищаются, сбрасывается указатель
* и создается новый чистый объект. Если на старые данные никто не
* ссылается, то они осовбождаются.
*/
void EasyResultSet::reset()
{
	ref = new ResultSet();
}

/**
* Претворяемся что мы vector<>
*/
EasyRow EasyResultSet::operator [] (int idx)
{
	return ref->operator [] (idx);
}

/**
* Вернуть строку по индексу
*
* Проверка на выход за границы не выполняется.
*/
EasyRow EasyResultSet::get(int pos) const
{
	return ref->operator [] (pos);
}

/**
* Установить строку по индексу
*
* Проверка на выход за границы не выполняется.
*
* NOTE копируется ссылка на объект, если нужно создать независимую копию
* объекта, то используйте copy()
*/
void EasyResultSet::set(int pos, EasyRow row)
{
	ref->operator [] (pos) = row;
}
