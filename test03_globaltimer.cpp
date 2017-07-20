
#include <nanosoft/netdaemon.h>
#include <nanosoft/asyncdns.h>

void on_timer(const timeval &tv, NetDaemon* daemon)
{
	static int count = 0;
	if ( count++ > 10 )
	{
		printf("done\n");
		exit(0);
	}
	
	int h = (tv.tv_sec / (60 * 60)) % 24;
	int m = (tv.tv_sec / 60) % 60;
	int s = tv.tv_sec % 60;
	int ms = tv.tv_usec / 1000;
	printf("time: %02d:%02d:%02d.%03d\n", h, m, s, ms);
}

int main(int argc, char** argv)
{
	// время ожидания по умолчанию
	int sleep_time = 100;
	
	if ( argc > 1 )
	{
		sscanf(argv[1], "%d", &sleep_time);
	}
	
	printf("sleep_time: %d\n", sleep_time);
	if ( sleep_time < 2 )
	{
		printf("wrong sleep_time, need sleep_time >= 2\n");
		return -1;
	}
	
	NetDaemon daemon(10, 16);
	daemon.setSleepTime(sleep_time);
	daemon.setGlobalTimer(on_timer, &daemon);
	
	// нам нужен хоть один объект, чтобы работал NetDaemon
	adns = new AsyncDNS(&daemon);
	daemon.addObject(adns);
	
	daemon.run();
	
	return 0;
}
