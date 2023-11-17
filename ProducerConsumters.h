#ifndef __PRODUCER_CONSUMERS_H__
#define __PRODUCER_CONSUMERS_H__

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <unistd.h>
#include <cassert>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sched.h>
#include "Shared_Memory_Layout.h"
#include "Shared_Memory_Utils.h"

struct alignas(64) SharedData {
    uint64_t id;
    // sending time
    long long send_at;
};

// 定义环形缓冲区模板类
template<typename T>
class RingBuffer {
public:
    static const size_t DefaultPositionCount = 256;
    static const int DefaultIdleCount = 8;
public:
    static RingBuffer<T>* create_shm_buffer(const char* filename,size_t t_count = DefaultPositionCount){
            RingBuffer<T>* buffer = shm_map<RingBuffer<T>>(filename,(t_count*sizeof(T)+t_count*sizeof(unsigned char)),true);
        if(buffer!=nullptr){
            //USED:Placement New
            buffer = new (buffer)
                RingBuffer<T>((unsigned char*)buffer,t_count);
        }
        return buffer;
    }
public:
    static RingBuffer<T>* open_shm_buffer(const char* filename,size_t t_count = DefaultPositionCount){
        return shm_map<RingBuffer<T>>(filename,t_count*sizeof(T)+t_count*sizeof(unsigned char),true);
    }

public:
    // 构造函数
    RingBuffer(unsigned char* address, size_t size, int idle_count = DefaultIdleCount)
    : pbuffer(0)
    , pflags(0)
    , idle_count(idle_count)
    , capacity_mask(size - 1) {
        // 确保size是2的整数次幂
        assert((size & (size - 1)) == 0 && "Size must be a power of 2");
        this->pbuffer= sizeof(RingBuffer<T>);
        this->pflags = sizeof(RingBuffer<T>)+size*sizeof(T);
        memset(address + pbuffer,0,size*sizeof(T));
        memset(address + pflags,0,size*sizeof(unsigned char));
    }

    ~RingBuffer(){
    }

    // 发送数据到缓冲区
    void send(const T& value) {
        // 原子操作当前的发送位置，保证不管多个进程还是多个线程都在缓存中找到唯一位置
        size_t i = send_index.fetch_add(1, std::memory_order_seq_cst);
        ((T*)((unsigned char*)this+pbuffer))[i & capacity_mask] = value;
        //原子操作将全1写入pflags[i]
        __sync_lock_test_and_set(&((unsigned char*)this+pflags)[i&capacity_mask],0xff);
    }

    // 等待并从缓冲区中获取数据
    void wait(T& value) {
        //__sync_val_compare_and_swap
        //__sync_bool_compare_and_swap

        int ic = 0;
        size_t i = receive_index.load(std::memory_order_seq_cst);
        while(!__sync_bool_compare_and_swap(&((unsigned char*)this+pflags)[i&capacity_mask],0xff,0)){
            i++;
            ic++;
            if(ic >= this->idle_count){
                sched_yield();
                ic = 0;
            }
        }
        i&=capacity_mask;
        receive_index.store(i, std::memory_order_seq_cst);

        value = ((T*)((unsigned char*)this+pbuffer))[i];
    }

private:
    size_t pbuffer;
    size_t pflags;
    int idle_count;
    std::atomic_size_t send_index = 0;    // 当前发送数据位置的原子计数器
    std::atomic_size_t receive_index = 0;  //当前接收数据位置的原子计数器
    const size_t capacity_mask;          // 用于确定缓冲区位置的掩码

};

#endif
