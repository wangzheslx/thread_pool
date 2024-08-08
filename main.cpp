#include "locker.h"
#include "pthreadpool.h"
#include <stdio.h>
#include <unistd.h>
int main(int argc, char* argv[]){
    pthreadpool<int> p;
    int i = 1;
    while(i++){
        int * t = new int;
        *t = i;
        if(p.append(t) == false){
            printf("sleep1\n");
            //sleep(5);
        }
        //sleep(1);
    }
    return 0;
}