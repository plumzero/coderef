
#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * ThreadLog 类的测试函数
 *
 * g++ -g tp_log.cpp threadpool.cpp -o tp_log -std=c++11 -lpthread
 */


int main()
{
	ThreadPoolNS::ThreadLog m_log(stdout);
	m_log.thread_log_print(ThreadPoolNS::TP_LOG_LEVEL_DBUG, "%s %d\n", "hello world", 100);
	m_log.thread_log_print(ThreadPoolNS::TP_LOG_LEVEL_DBUG, "hello world 200\n");
	TPECHO(DBUG, "%s %d", "hello world", 300);
	TPECHO(INFO, "hello world 400");
	TPECHO(DBUG, "hello world 500");
	
	TPECHO(INFO, "id = #%ui", 12136);
	
	return 0;
}