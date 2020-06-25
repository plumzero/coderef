#include <stdio.h>

int main(void)
{
	int input;
	int *p;
	printf("Please input an integer:");
	scanf("%d", &input);
	*p = input;	/* 这里出错，赋值前p并未初始化，因此p为野指针 */
	printf("The integer you input is %d\n", *p);
	return 0;
}
