#include "locker.h"
#include "pthreadpool.h"

int main(int argc, char* argv[]){
    pthreadpool<int> p;
    int i = 1;
    while(i++){
        p.append(&i);
    }
    return 0;
}