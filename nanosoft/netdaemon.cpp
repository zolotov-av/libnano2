#include <nanosoft/netdaemon.h>
#include <nanosoft/logger.h>
#include <nanosoft/utils.h>

using namespace std;

void my_gnutls_log_func( int level, const char *message)
{
	printf("gnutls: level=%d %s", level, message);
}

/**
* Конструктор демона
* @param fd_limit максимальное число одновременных виртуальных потоков
* @param buf_size размер файлового буфера в блоках
*/
NetDaemon::NetDaemon(int fd_limit, int buf_size): sleep_time(200), timerCount(0), gtimer(0), count(0), active(0) {
	limit = fd_limit;
	epoll = epoll_create(fd_limit);
	
	fds = new fd_info_t[fd_limit];
	fd_info_t *fb, *fd_end = fds + fd_limit;
	for(fb = fds; fb < fd_end; fb++)
	{
		fb->obj = 0;
		fb->size = 0;
		fb->offset = 0;
		fb->quota = 0;
		fb->first = 0;
		fb->last = 0;
	}
	
	bp = bp_pool(buf_size);
	if ( ! bp )
	{
		printf("NetDaemon failed to get BlockPool\n");
	}
	
#ifdef HAVE_GNUTLS
	printf("BEFORE gnutls_global_init()\n");
	if ( gnutls_global_init() )
	{
		fprintf(stderr, "GnuTLS global init fault\n");
	}
	printf("AFTER gnutls_global_init()\n");
	
#ifdef DEBUG_TLS
	gnutls_global_set_log_level(9);
	gnutls_global_set_log_function(my_gnutls_log_func);
#endif // DEBUG_TLS
#endif // HAVE_GNUTLS
}

/**
* Деструктор демона
*/
NetDaemon::~NetDaemon()
{
	int r = ::close(epoll);
	if ( r < 0 ) stderror();
	
	delete [] fds;
	
#ifdef HAVE_GNUTLS
	gnutls_global_deinit();
#endif // HAVE_GNUTLS
}

/**
* Обработчик ошибок
*
* По умолчанию выводит все ошибки в stderr
*/
void NetDaemon::onError(const char *message)
{
	cerr << "[NetDaemon]: " << message << endl;
}

/**
* Обработка системной ошибки
*/
void NetDaemon::stderror()
{
	onError(strerror(errno));
}

/**
* Установить время ожидания для epoll_wait
*
* Оказвыает влияние на частоту таймеров. Если указать
* слишком маленькое значение, то частота таймера (глобального)
* повыситься, но будет тратиться больше процессорного времени
* на обработку таймера. Если указать слишком высокое значение,
* то таймеры будут обратываться реже и возможно будут запаздывать.
*
* Значение по умолчанию (200мс) выбрано из расчета что обычно
* не требуется большая точность таймеров и оверхед на CPU нежелателен
*/
void NetDaemon::setSleepTime(int v)
{
	sleep_time = ( v < 0 ) ? 0 : v;
}

/**
* Вернуть число подконтрольных объектов
*/
int NetDaemon::getObjectCount() const
{
	return count;
}

/**
* Вернуть размер буфера в блоках
*/
int NetDaemon::getBufferSize() const
{
	return bp ? bp->getPoolSize() : 0;
}

/**
* Добавить асинхронный объект
*/
bool NetDaemon::addObject(ptr<AsyncObject> object)
{
	return addObject(object, true);
}

/**
* Добавить асинхронный объект
*/
bool NetDaemon::addObject(ptr<AsyncObject> object, bool enabled)
{
	if ( object == 0 )
	{
		// передали NULL-указатель
		return false;
	}
	
	object->setDaemon(this);
	
	if ( enabled )
	{
		return enableObject(object);
	}
	
	return true;
}

/**
* Активировать объект
*/
bool NetDaemon::enableObject(ptr<AsyncObject> object)
{
	if ( object == 0 )
	{
		// передали NULL-указатель
		return false;
	}
	
	if ( object->getDaemon() != this )
	{
		// ошиблись демоном
		return false;
	}
	
	// проверяем корректность файлового дескриптора
	if ( object->fd < 0 || object->fd >= limit )
	{
		logger.unexpected("NetDaemon::enableObject(): wrong file descriptor");
		return false;
	}
	
	fd_info_t *fb = &fds[object->fd];
	
	if ( fb->obj == 0 )
	{
		// принудительно выставить O_NONBLOCK
		int flags = fcntl(object->fd, F_GETFL, 0);
		if ( flags >= 0 )
		{
			int r = fcntl(object->fd, F_SETFL, flags | O_NONBLOCK);
			if ( r == -1 )
			{
				logger.unexpected("NetDaemon::enableObject(), fcntl(+O_NONBLOCK) fault: %s", strerror(errno));
				return false;
			}
		}
		
		// добавить в epoll
		struct epoll_event event;
		event.data.fd = object->fd;
		event.events = object->getEventsMask();
		if ( fb->size > 0 )
		{
			event.events |= EPOLLOUT; // TODO fb->size == 0 ?
			logger.unexpected("NetDaemon::enableObject(%d), fb->size > 0\n", object->fd);
		}
		int r = epoll_ctl(epoll, EPOLL_CTL_ADD, object->fd, &event);
		if ( r == -1  )
		{
			logger.unexpected("NetDaemon::enableObject(%d), epoll_ctl(EPOLL_CTL_ADD) fault: %s\n", object->fd, strerror(errno));
			return false;
		}
		else
		{
			fb->obj = object;
			count ++;
			return true;
		}
	}
	
	if ( fb->obj == object )
	{
		return true;
	}
	else
	{
		logger.unexpected("NetDaemon::enableObject(%d), wrong object\n", object->fd);
		return false;
	}
}

/**
* Деактивировать объект
*
* Объект остается под контролем демона и остается в его внутренних
* структурах, но на время отключается от epoll
*/
bool NetDaemon::disableObject(ptr<AsyncObject> object)
{
	if ( object == 0 )
	{
		// передали NULL-указатель
		return false;
	}
	
	if ( object->getDaemon() != this )
	{
		// ошиблись демоном
		return false;
	}
	
	// проверяем корректность файлового дескриптора
	if ( object->fd < 0 || object->fd >= limit )
	{
		logger.unexpected("NetDaemon::disableObject(): wrong file descriptor");
		return false;
	}
	
	fd_info_t *fb = &fds[object->fd];
	
	if ( fb->obj == 0 )
	{
		// объект в epoll не значится
		return true;
	}
	
	if ( fb->obj != object )
	{
		// что-то пошло не так...
		logger.unexpected("NetDaemon::disableObject(%d), wrong object\n", object->fd);
		return false;
	}
	
	int r = epoll_ctl(epoll, EPOLL_CTL_DEL, object->fd, 0);
	if ( r == -1 )
	{
		logger.unexpected("NetDaemon::disableObject(%d), epoll_ctl(EPOLL_CTL_DEL) fault: %s\n", object->fd, strerror(errno));
		return false;
	}
	else
	{
		fb->obj = 0;
		cleanup(object->fd);
		count--;
		return true;
	}
}

/**
* Уведомить NetDaemon, что объект изменил свою маску
*/
bool NetDaemon::modifyObject(ptr<AsyncObject> object)
{
	// проверяем корректность файлового дескриптора
	if ( object->fd < 0 || object->fd >= limit )
	{
		logger.unexpected("NetDaemon::modifyObject(): wrong descriptor/object");
		return false;
	}
	
	if ( fds[object->fd].obj != object )
	{
		fprintf(stderr, "NetDaemon::modifyObject(%d), wrong object\n", object->fd);
		return false;
	}
	
	return resetObject(object);
}

/**
* Удалить асинхронный объект
*/
bool NetDaemon::removeObject(ptr<AsyncObject> object)
{
	if ( object == 0 )
	{
		// передали NULL-указатель
		return false;
	}
	
	if ( object->getDaemon() != this )
	{
		// чужой демон
		return false;
	}
	
	if ( disableObject(object) )
	{
		object->setDaemon(0);
		return true;
	}
	
	return false;
}

/**
* Возобновить работу с асинхронным объектом
*/
bool NetDaemon::resetObject(ptr<AsyncObject> &object)
{
	struct epoll_event event;
	event.data.fd = object->fd;
	event.events = object->getEventsMask();
	if ( fds[object->fd].size > 0 ) event.events |= EPOLLOUT;
	int r = epoll_ctl(epoll, EPOLL_CTL_MOD, object->fd, &event);
	if ( r == -1 )
	{
		fprintf(stderr, "NetDaemon::resetObject(%d), epoll_ctl(EPOLL_CTL_MOD) fault: %s\n", object->fd, strerror(errno));
		return false;
	}
	
	return true;
}

/**
* Действие активного цикла
*/
void NetDaemon::doActiveAction(int wait_time)
{
	struct epoll_event event;
	int r = epoll_wait(epoll, &event, 1, wait_time);
	if ( r > 0 )
	{
		ptr<AsyncObject> obj = fds[event.data.fd].obj;
		if ( obj != 0 )
		{
			obj->onEvent(event.events);
			
			// если объект ещё в epoll, то сбросить события
			if ( fds[event.data.fd].obj == obj ) resetObject(obj);
		}
	}
	if ( r < 0 ) stderror();
}

/**
* Запустить демона
*/
int NetDaemon::run()
{
	active = 1;
	
	int wait_ts;
	int64_t curr_ts, next_ts;
	struct timeval tv;
	
	gettimeofday(&tv, 0);
	curr_ts = millitime(tv);
	wait_ts = sleep_time - curr_ts % sleep_time;
	next_ts = curr_ts + wait_ts;
	
	while ( active )
	{
		doActiveAction(wait_ts);
		
		gettimeofday(&tv, 0);
		curr_ts = millitime(tv);
		if ( curr_ts >= next_ts )
		{
			// если пришло время сработать таймеру
			processTimers(tv);
			gettimeofday(&tv, 0);
			curr_ts = millitime(tv);
			wait_ts = sleep_time - curr_ts % sleep_time;
			next_ts = curr_ts + wait_ts;
		}
		else if ( next_ts - curr_ts > sleep_time )
		{
			// если время прыгнуло назад
			// в нормальной ситации разница между curr_ts и next_ts не более
			// чем sleep_time
			printf("Oops, time jump\n");
			
			// всё равно запускаем событие и инициализируем таймер
			processTimers(tv);
			gettimeofday(&tv, 0);
			curr_ts = millitime(tv);
			wait_ts = sleep_time - curr_ts % sleep_time;
			next_ts = curr_ts + wait_ts;
		}
		else
		{
			// если время еще не пришло, то просто расчитаем время ожидания
			wait_ts = sleep_time - curr_ts % sleep_time;
		}
	}

	return 0;
}

/**
* Остановить демона
*
* Функция run() возвращает управление, но состояние сохраняется и run()
* может быть запущена снова
*/
void NetDaemon::stop()
{
	active = 0;
}

/**
* Установить таймер
* @param calltime время запуска таймера
* @param callback функция обратного вызова
* @param data указатель на пользовательские данные
* @return TRUE - таймер установлен, FALSE - таймер установить не удалось
*/
bool NetDaemon::callAt(time_t calltime, timer_callback_t callback, void *data)
{
	timers.push(timer(calltime, callback, data));
	timerCount++;
	return true;
}

/**
* Обработать таймеры
*/
void NetDaemon::processTimers(const struct timeval &tv)
{
	timer t;
	if ( gtimer )
	{
		gtimer(tv, gtimer_data);
	}
	onProcessTimer();
	while ( timerCount > 0 )
	{
		t = timers.top();
		if ( t.expires <= tv.tv_sec )
		{
			timers.pop();
			timerCount --;
			t.fire(tv);
		}
		else return;
	}
}

/**
* Вернуть число свободных блоков в буфере
* @return число свободных блоков в буфере
*/
size_t NetDaemon::getFreeSize() const
{
	return bp ? bp->getFreeCount() : 0;
}

/**
* Вернуть размер буферизованных данных
* @param fd файловый дескриптор
* @return размер буферизованных данных
*/
size_t NetDaemon::getBufferedSize(int fd)
{
	return ( fd >= 0 && fd < limit ) ? fds[fd].size : 0;
}

/**
* Вернуть квоту файлового дескриптора
* @param fd файловый дескриптор
* @return размер квоты
*/
size_t NetDaemon::getQuota(int fd)
{
	return ( fd >= 0 && fd < limit ) ? fds[fd].quota : 0;
}

/**
* Установить квоту буфер файлового дескриптора
* @param fd файловый дескриптор
* @param quota размер квоты
* @return TRUE квота установлена, FALSE квота не установлена
*/
bool NetDaemon::setQuota(int fd, size_t quota)
{
	if ( fd >= 0 && fd < limit )
	{
		fds[fd].quota = quota;
		return true;
	}
	return false;
}

/**
* Добавить данные в буфер (thread-unsafe)
*
* Если включена компрессия, то сначала сжать данные
*
* @param fd файловый дескриптор
* @param fb указатель на описание файлового буфера
* @param data указатель на данные
* @param len размер данных
* @return TRUE данные приняты, FALSE данные не приняты - нет места
*/
bool NetDaemon::put(int fd, fd_info_t *fb, const char *data, size_t len)
{
	if ( fb->quota != 0 && (fb->size + len) > fb->quota )
	{
		// превышение квоты
		return false;
	}
	
	nano_block_t *block;
	
	if ( fb->size > 0 )
	{
		// смещение к свободной части последнего блока или 0, если последний
		// блок заполнен полностью
		size_t offset = (fb->offset + fb->size) % BLOCKSPOOL_BLOCK_SIZE;
		
		// размер свободной части последнего блока
		size_t rest = offset > 0 ? BLOCKSPOOL_BLOCK_SIZE - offset : 0;
		
		// размер недостающей части, которую надо выделить из общего буфера
		size_t need = len - rest;
		
		if ( len <= rest ) rest = len;
		else
		{
			// выделить недостающие блоки
			block = bp->allocBySize(need);
			if ( block == 0 ) return false;
		}
		
		// если последний блок заполнен не полностью, то дописать в него
		if ( offset > 0 )
		{
			memcpy(fb->last->data + offset, data, rest);
			fb->size += rest;
			data += rest;
			len -= rest;
			if ( len == 0 ) return true;
		}
		
		fb->last->next = block;
		fb->last = block;
	}
	else // fb->size == 0
	{
		block = bp->allocBySize(len);
		if ( block == 0 )
		{
			// нет буфера
			return false;
		}
		
		fb->first = block;
		fb->offset = 0;
	}
	
	// записываем полные блоки
	while ( len >= BLOCKSPOOL_BLOCK_SIZE )
	{
		memcpy(block->data, data, BLOCKSPOOL_BLOCK_SIZE);
		data += BLOCKSPOOL_BLOCK_SIZE;
		len -= BLOCKSPOOL_BLOCK_SIZE;
		fb->size += BLOCKSPOOL_BLOCK_SIZE;
		fb->last = block;
		block = block->next;
	}
	
	// если что-то осталось записываем частичный блок
	if ( len > 0 )
	{
		memcpy(block->data, data, len);
		fb->size += len;
		fb->last = block;
	}
	
	return true;
}

/**
* Добавить данные в буфер (thread-safe)
*
* @param fd файловый дескриптор в который надо записать
* @param data указатель на данные
* @param len размер данных
* @return TRUE данные приняты, FALSE данные не приняты - нет места
*/
bool NetDaemon::put(int fd, const char *data, size_t len)
{
	// проверяем корректность файлового дескриптора
	if ( fd < 0 || fd >= limit )
	{
		// плохой дескриптор
		fprintf(stderr, "StanzaBuffer[%d]: wrong descriptor\n", fd);
		return false;
	}
	
	// проверяем размер, зачем делать лишние движения если len = 0?
	if ( len == 0 ) return true;
	
	// находим описание файлового буфера
	return put(fd, &fds[fd], data, len);
}

/**
* Записать данные из буфера в файл/сокет
*
* @param fd файловый дескриптор
* @return TRUE буфер пуст, FALSE в буфере ещё есть данные
*/
bool NetDaemon::push(int fd)
{
	// проверяем корректность файлового дескриптора
	if ( fd < 0 || fd >= limit )
	{
		// плохой дескриптор
		fprintf(stderr, "StanzaBuffer[%d]: wrong descriptor\n", fd);
		return false;
	}
	
	// находим описание файлового буфера
	fd_info_t *fb = &fds[fd];
	
	// список освободившихся блоков
	nano_block_t *unused = 0;
	
	while ( fb->size > 0 )
	{
		// размер не записанной части блока
		size_t rest = BLOCKSPOOL_BLOCK_SIZE - fb->offset;
		if ( rest > fb->size ) rest = fb->size;
		
		// попробовать записать
		ssize_t r = write(fd, fb->first->data + fb->offset, rest);
		if ( r <= 0 ) break;
		
		fb->size -= r;
		fb->offset += r;
		
		// если блок записан полностью,
		if ( r == rest )
		{
			// добавить его в список освободившихся
			nano_block_t *block = fb->first;
			fb->first = block->next;
			fb->offset = 0;
			block->next = unused;
			unused = block;
		}
		else
		{
			// иначе пора прерваться и вернуться в epoll
			break;
		}
	}
	
	bp->free(unused);
	
	return fb->size <= 0;
}

/**
* Удалить блоки файлового дескриптора
*
* @param fd файловый дескриптор
*/
void NetDaemon::cleanup(int fd)
{
	// проверяем корректность файлового дескриптора
	if ( fd < 0 || fd >= limit )
	{
		// плохой дескриптор
		fprintf(stderr, "StanzaBuffer[%d]: wrong descriptor\n", fd);
		return;
	}
	
	fd_info_t *p = &fds[fd];
	free(p->first);
	p->size = 0;
	p->offset = 0;
	p->quota = 0;
	p->first = 0;
	p->last = 0;
}
