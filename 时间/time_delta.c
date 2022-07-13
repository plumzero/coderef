#include <stdio.h>
#include <time.h>

#define USAGE  \
    "Usage: ./program <String: format time> <String: format time>\n"  \
    "For example:\n"  \
    "   ./program 11:24:36.613466 11:25:16.259351 \n"  \
    "   ./program 11:25:16.259351 11:24:36.613466 \n"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, USAGE);
        return -1;
    }

    int ret = -1;
    int hour, min, sec, microsec;

    hour = min = sec = microsec = 0;
    ret = sscanf(argv[1], "%d:%d:%d.%d", &hour, &min, &sec, &microsec);
    if (ret != 4) {
        fprintf(stderr, USAGE);
        return -1;
    }
    long int t1 = (hour * 3600 + min * 60 + sec) * 1000000 + microsec;

    hour = min = sec = microsec = 0;
    ret = sscanf(argv[2], "%d:%d:%d.%d", &hour, &min, &sec, &microsec);
    if (ret != 4) {
        fprintf(stderr, USAGE);
        return -1;
    }
    long int t2 = (hour * 3600 + min * 60 + sec) * 1000000 + microsec;

    long int diff = t2 - t1;

    printf("%ld\n", diff);
    

    return 0;
}