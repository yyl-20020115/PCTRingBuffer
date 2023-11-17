#include "ProducerConsumters.h"
#include "TimeUtils.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <fstream>
#include <sstream>

void do_reader(pid_t pid, bool create_shm){
    std::stringstream ss;
    ss<<"log/log_receiver_"<<pid<<".txt";
    std::ofstream fs(ss.str());

    size_t* sp = (size_t*)shm_map2("min_time",sizeof(size_t), create_shm);
    if(create_shm && (sp!=nullptr)){
        *sp = ~0ull;
    }
    RingBuffer<SharedData>* buffer = RingBuffer<SharedData>::open_shm_buffer("shm_shared_data_ring_buffer");
    if(buffer!=nullptr){
        bool any = true;
        while(any){
            SharedData s = {0};

            buffer->wait(s);

            long long ct = get_current_nanoseconds();

            long long dt = ct - s.send_at;
            bool changed = false;
            if(sp!=nullptr){
                if(dt<*sp)
                {
                    __sync_lock_test_and_set(sp,dt);
                    changed = true;
                }
            }

            if(create_shm )
            {
                if(changed)
                {
                    std::cout<<"min_time="<<(unsigned long long)*sp<<std::endl;
                }
            }
            fs<<"received position:"<<s.send_at<<", dt="<<dt<<" ns"<<std::endl;
        }
    }
}
int main()
{
    std::cout<<"reader_test started"<<std::endl;
    do_reader(getpid(),true);

    return 0;
}
