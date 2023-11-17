#include "ProducerConsumters.h"
#include "TimeUtils.h"
#include <fstream>
#include <sstream>

void writer_test()
{
    std::stringstream ss;
    ss<<"log/log_sender_"<<getpid()<<".txt";
    std::ofstream fs(ss.str());

    std::cout<<"writer started"<<std::endl;
    RingBuffer<SharedData>* buffer = RingBuffer<SharedData>::create_shm_buffer("shm_shared_data_ring_buffer");
    if(buffer!=nullptr){
        size_t i = 0;
        bool any = true;
        while(any){
            SharedData s = {0};
            s.send_at = get_current_nanoseconds();
            buffer->send(s);
            fs<<"sent data i ="<<i++<<", t= "<<s.send_at<<std::endl;
            usleep(100*1000); //100ms
        }
    }
}

