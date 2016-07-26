#include "debugging.h"

char dlog[SIZE] = {'\0'};
int debugwserial = 1;

void dlogadd(char, int);
void dlogdisp(void);

void dlogadd(char latest, int addlen)
{
    int len = strlen(dlog);


    if(len < SIZE - 1)
    {
        dlog[len] = latest;
        dlog[len+1] = '\0';
    }
    else
    {
        printf("\nlog is full\n");
    }

    printf("current length: %d \n",len);
    printf("adding char: %x \n",latest);
    printf("new length: %d \n",strlen(dlog));
}

void dlogdisp(void)
{
    int i = 0;
    int len = strlen(dlog);

    for(i=0;i<len;i++)
    {
        printf("%x ",dlog[i]);
    }

    printf("\n");

}