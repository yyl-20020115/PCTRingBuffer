#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

void reader_test(int max_processes);
void writer_test();

int main(int argc, char *argv[])
{
    system("rm -rf ./log");
    system("mkdir  ./log");

    pid_t pid = fork();
    if(pid == 0){
        std::cout<<"in writer process"<<std::endl;
        //producer
        writer_test();
    }else if(pid>0){ //self
        sleep(1);
        std::cout<<"in reader process"<<std::endl;
        //consumers
        reader_test(20);
    }else{
        //error
    }
    return pid;
}
