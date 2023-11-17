#include "ProducerConsumters.h"
#include "TimeUtils.h"
#include <fstream>
#include <sstream>

int main()
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
            s.id = i++;
            buffer->send(s);
            fs<<"sent data:"<<s.id<<", t= "<<s.send_at<<std::endl;
            usleep(100*1000); //100ms
        }
    }

    return 0;
}

