
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>

std::string epoch_to_clock(time_t epoch_time)
{
  struct tm *st;
  char tmbuf[64] = { 0 };
  
  st = localtime(&epoch_time);
  int n = strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", st);
  return std::string(tmbuf, n);
}

int clock_to_epoch(std::string clock_time, time_t *epoch_time)
{ 
  int ret = -1;
  int year, month, day, hour, minute, second;
  struct tm ts;
  
  year = month = day = hour = minute = second = 0;
  
  ret = sscanf(clock_time.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
  if (ret != 6) {
    fprintf(stderr, "timestamp format error(ret: %d)\n", ret);
    return -1;
  }
  
  if (year < 1970) {
    fprintf(stderr, "year error, %04d not in [1970, ...)\n", year);
    return -1;
  }
  if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
    if (day < 0 || day > 31) {
      fprintf(stderr, "%04d-%02d-%02d error\n", year, month, day);
      return -1;
    }
  } else if (month == 4 || month == 6 || month == 9 || month == 11) {
    if (day < 0 || day > 30) {
      fprintf(stderr, "%04d-%02d-%02d error\n", year, month, day);
      return -1;
    }
  } else if (month == 2) {
    if((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0 && year % 3200 != 0) || year % 172800 == 0) {
      if (day < 0 || day > 29) {
        fprintf(stderr, "%04d-%02d-%02d error\n", year, month, day);
        return -1;
      }
    } else {
      if (day < 0 || day > 28) {
        fprintf(stderr, "%04d-%02d-%02d error\n", year, month, day);
        return -1;
      }
    }
  } else {
    fprintf(stderr, "month error, %02d not in [1, 12]\n", month);
    return -1;
  }
  if (hour < 0 || hour > 23) {
    fprintf(stderr, "hour error, %02d not in [0, 23]\n", hour);
    return -1;
  }
  if (minute < 0 || minute > 59) {
    fprintf(stderr, "minute error, %02d not in [0, 59]\n", minute);
    return -1;
  }
  if (second < 0 || second > 59) {
    fprintf(stderr, "second error, %02d not in [0, 59]\n", second);
    return -1;
  }
  
  ts.tm_year = year - 1900;
  ts.tm_mon  = month - 1;
  ts.tm_mday = day;
  ts.tm_hour = hour;
  ts.tm_min  = minute;
  ts.tm_sec  = second;
  
  *epoch_time = mktime(&ts);
  
  return 0;
}

int next_rotation_tp(std::string clock_time)
{
  time_t epoch_time;
  int ret = clock_to_epoch(clock_time, &epoch_time);
  if (ret != 0) {
    printf("clock_to_epoch\n");
    return -1;
  }

  struct tm * date = localtime(&epoch_time);
  date->tm_mday = 1;
  date->tm_hour = 0;
  date->tm_min = 0;
  date->tm_sec = 0;
  time_t last_time = mktime(date);
  int the_month = date->tm_mon + 1;
  int days = 0;
  switch (the_month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      days = 31;
      break;
    case 4:
    case 6:
    case 9:
    case 11:
      days = 30;
      break;
    case 2:
      int year = 1900 + date->tm_year;
      if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0 && year % 3200 != 0) || year % 172800 == 0) {
        days = 29;
      } else {
        days = 28;
      }
    break;
  }

  time_t next_time = last_time + days * 24 * 3600;
  int diff_time = next_time - last_time;

  printf("epoc_time=%s\n", epoch_to_clock(epoch_time).c_str());
  printf("last_time=%s\n", epoch_to_clock(last_time).c_str());
  printf("next_time=%s\n", epoch_to_clock(next_time).c_str());
  printf("diff_time=%d\n", diff_time);
  printf("%d x 24 x 3600=%d\n", days, days * 24 * 3600);

  return 0;
}

//  ./month_rotate '2023-02-21 13:22:45'
//  ./month_rotate '2023-12-21 13:22:45'
//  ./month_rotate '2024-02-21 13:22:45'
//  ./month_rotate '2024-03-21 13:22:45'
//  ./month_rotate '2024-04-21 13:22:45'

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "%s <clocktime, eg '2023-12-30 13:22:45'>\n", argv[0]);
    return -1;
  }
  
  next_rotation_tp(argv[1]);
  
  return 0;
}