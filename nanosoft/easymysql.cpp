
#include <nanosoft/easymysql.h>
#include <nanosoft/easylib.h>

#include <cstdio>
#include <cstring>

/**
* Соединение сервером
*
* В случае неудачи выводит в stderr сообщение об ошибке и возращает FALSE
* @return TRUE - соединение установлено, FALSE ошибка соединения
*/
bool EasyMySQL::reconnect()
{
	if ( connectType == CONNECT_INET )
	{
		printf("[EasyMySQL] connect(host=%s, user=%s, db=%s)\n", cstr(host), cstr(user), cstr(database));
		
		if ( mysql_real_connect(&conn, cstr(host), cstr(user), cstr(password), cstr(database), port, 0, 0) )
		{
			mysql_set_character_set(&conn, "utf8");
			return true;
		}
		
		fprintf(stderr, "[MySQL] %s\n", mysql_error(&conn));
		return false;
	}
	
	if ( connectType == CONNECT_UNIX )
	{
		printf("[EasyMySQL] connect(socket=%s, user=%s, db=%s)\n", cstr(sock), cstr(user), cstr(database));
		
		if ( mysql_real_connect(&conn, 0, cstr(user), cstr(password), cstr(database), 0, cstr(sock), 0) )
		{
			mysql_set_character_set(&conn, "utf8");
			return true;
		}
		
		fprintf(stderr, "[MySQL] %s\n", mysql_error(&conn));
		return false;
	}
	
	return false;
}

/**
* Обертка вокруг mysql_real_query()
*
* имеет дополнительную обработку - в случае если сервер отключился,
* то делает повторную попытку подключиться и выполнить запрос
*/
int EasyMySQL::real_query(const char *sql, size_t len)
{
	int status = mysql_real_query(&conn, sql, len);
	if ( status == 0 ) return 0;
	
	fprintf(stderr, "[MySQL] %s\n", mysql_error(&conn));
	
	unsigned int err = mysql_errno(&conn);
	if ( err == CR_SERVER_GONE_ERROR /* || err == CR_SERVER_LOST */ )
	{
		close();
		if ( reconnect() )
		{
			status = mysql_real_query(&conn, sql, len);
			if ( status == 0 ) return 0;
			fprintf(stderr, "[MySQL] double fault: %s\n", mysql_error(&conn));
			return status;
		}
	}
	return status;
}

/**
* Обертка вокруг mysql_real_query()
*
* имеет дополнительную обработку - в случае если сервер отключился,
* то делает повторную попытку подключиться и выполнить запрос
*/
int EasyMySQL::real_query(const std::string &sql)
{
	return real_query(sql.c_str(), sql.length());
}

/**
* Конструктор
*/
EasyMySQL::EasyMySQL()
{
	if ( ! mysql_init(&conn) )
	{
		fprintf(stderr, "[EasyMySQL] mysql_init failed\n");
	}
}

/**
* Деструктор
*/
EasyMySQL::~EasyMySQL()
{
	mysql_close(&conn);
}

/**
* Соединение сервером
*
* В случае неудачи выводит в stderr сообщение об ошибке и возращает FALSE
*
* @param host хост
* @param database имя БД к которой подключаемся
* @param user пользователь
* @param password пароль
* @param port порт
* @return TRUE - соединение установлено, FALSE ошибка соединения
*/
bool EasyMySQL::connect(const std::string &host, const std::string &database, const std::string &user, const std::string &password, int port)
{
	this->connectType = CONNECT_INET;
	this->host = host;
	this->port = port;
	this->database = database;
	this->user = user;
	this->password = password;
	
	return reconnect();
}

/**
* Соединение сервером
*
* В случае неудачи выводит в stderr сообщение об ошибке и возращает FALSE
*
* @param sock путь к Unix-сокету
* @param database имя БД к которой подключаемся
* @param user пользователь
* @param password пароль
* @return TRUE - соединение установлено, FALSE ошибка соединения
*/
bool EasyMySQL::connectUnix(const std::string &sock, const std::string &database, const std::string &user, const std::string &password)
{
	this->connectType = CONNECT_UNIX;
	this->sock = sock;
	this->database = database;
	this->user = user;
	this->password = password;
	
	return reconnect();
}

/**
* Экранировать строку
*/
std::string EasyMySQL::escape(const std::string &text)
{
	char *buf = new char[text.length() * 2 + 1];
	size_t len = mysql_real_escape_string(&conn, buf, text.c_str(), text.length());
	std::string result(buf, len);
	delete [] buf;
	return result;
}

/**
* Экранировать строку и заключить её в кавычки
*/
std::string EasyMySQL::quote(const std::string &text)
{
	return '"' + escape(text) + '"';
}

/**
* Экранировать строки
*/
EasyRow EasyMySQL::quoteRow(const EasyRow &row)
{
	EasyRow result;
	EasyRow::const_iterator it = row.begin();
	while ( it != row.end() )
	{
		result[it->first] = quote(it->second);
		++it;
	}
	return result;
}

/**
* Выполнить запрос не возвращающий набор данных
*/
bool EasyMySQL::exec(const std::string &sql)
{
	int status = real_query(sql);
	MYSQL_RES *res = mysql_use_result(&conn);
	if ( res )
	{
		// если есть набор данных, то читаем и игнорируем его
		while ( mysql_fetch_row(res) );
		mysql_free_result(res);
	}
	return status == 0;
}

/**
* Извлечь одну строку
*
* Функция выполняет SQL-запрос и возвращает первую строку.
* Если запрос возвращает больше строк, то они игнорируются.
*/
EasyRow EasyMySQL::queryOne(const std::string &sql)
{
	EasyRow row;
	real_query(sql);
	MYSQL_RES *res = mysql_use_result(&conn);
	if ( res )
	{
		int field_count = mysql_field_count(&conn);
		MYSQL_FIELD *fields = mysql_fetch_fields(res);
		MYSQL_ROW values = mysql_fetch_row(res);
		
		if ( values )
		{
			unsigned long *lengths = mysql_fetch_lengths(res);
			for(int i = 0; i < field_count; i++)
			{
				row[std::string(fields[i].name, fields[i].name_length)] = std::string(values[i], lengths[i]);
			}

			while ( mysql_fetch_row(res) ) ;
		}
		mysql_free_result(res);
	}
	return row;
}

/**
* Выполнить запрос
*
* Функция выполняет SQL-запрос и возвращает набор данных
* в виде списка строк.
*/
EasyResultSet EasyMySQL::queryAll(const std::string &sql)
{
	EasyResultSet result;
	real_query(sql);
	MYSQL_RES *res = mysql_use_result(&conn);
	if ( res )
	{
		MYSQL_FIELD *fields = mysql_fetch_fields(res);
		int field_count = mysql_num_fields(res);
		
		EasyList head(field_count);
		for(int i = 0; i < field_count; i ++)
		{
			head.set(i, std::string(fields[i].name, fields[i].name_length));
		}
		
		int irow = 0;
		MYSQL_ROW values;
		
		while ( values = mysql_fetch_row(res) )
		{
			EasyRow row;
			unsigned long *lengths = mysql_fetch_lengths(res);
			for(int i = 0; i < field_count; i++)
			{
				row[ head.get(i) ] = std::string(values[i], lengths[i]);
			}
			result[ irow++ ] = row;
		}
		
		mysql_free_result(res);
	}
	return result;
}

bool EasyMySQL::insert(const std::string &table, const EasyRow &row)
{
	return exec("INSERT INTO " + table + " (" + implode(", ", row.keys()) + ") VALUES (" + implode(", ", row.values()) + ")");
}

bool EasyMySQL::insertQuoted(const std::string &table, const EasyRow &row)
{
	return insert(table, quoteRow(row));
}

bool EasyMySQL::replace(const std::string &table, const EasyRow &row)
{
	return exec("REPLACE INTO " + table + " (" + implode(", ", row.keys()) + ") VALUES (" + implode(", ", row.values()) + ")");
}

bool EasyMySQL::replaceQuoted(const std::string &table, const EasyRow &row)
{
	return replace(table, quoteRow(row));
}

void EasyMySQL::close()
{
	mysql_close(&conn);
	if ( ! mysql_init(&conn) )
	{
		fprintf(stderr, "[EasyMySQL] mysql_init failed\n");
	}
}
