//
// Created by sankeerth on 04/12/20.
//

#ifndef ORB_SLAM2_MEMORYTRACKER_H
#define ORB_SLAM2_MEMORYTRACKER_H

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <fstream>


namespace AllocationTracker{

    static std::mutex allocation_lock;

    struct AllocationStats{
        size_t bytesAllocated = 0;
        size_t bytesFreed = 0;
        size_t heapComponent() {
            allocation_lock.lock();
            size_t netBytes = bytesAllocated - bytesFreed;
            allocation_lock.unlock();
            return netBytes;
        }
    };

    static AllocationStats memStats;

    static void PrintMemoryStats(){
        std::cout << "allocation: " << memStats.bytesAllocated << "\n";
        std::cout << "freed: " << memStats.bytesFreed << "\n";
        std::cout << "usage: " << memStats.heapComponent() << "\n" << std::endl;
    }

    static size_t WriteMemoryStats(){
        return memStats.heapComponent();
    }
}

void* operator new(size_t size){
    AllocationTracker::allocation_lock.lock();
    AllocationTracker::memStats.bytesAllocated += size;
    size_t* ptr = (size_t*)malloc(size + sizeof(size_t));
    *ptr = size;
    void* ret = (void*)(ptr+1);
    AllocationTracker::allocation_lock.unlock();
    return ret;
}

void operator delete(void* ptr){
    AllocationTracker::allocation_lock.lock();
    size_t* p = (size_t*)ptr;
    size_t size = p[-1];
    AllocationTracker::memStats.bytesFreed += size;
    void* p2 = (void*)(&p[-1]);
    free(p2);
    AllocationTracker::allocation_lock.unlock();
}


class PublishThread{
    bool stop_thread;
    std::ofstream ofs;
    std::thread* t;
    size_t(*getnum)();
    std::mutex mtx;

public:
    PublishThread(size_t(*f)(), std::string filename):getnum(f), ofs(filename){
    };

    void WriteBytes() {
        while (true) {
            mtx.lock();
            bool stop_now = stop_thread;
            size_t x = getnum();
            mtx.unlock();
            ofs << x << "\n";
            ofs.flush();
            if(stop_now) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    void BeginSession(){
        stop_thread = false;
        mtx.unlock();
        t = new std::thread(&PublishThread::WriteBytes, this);
    }

    void EndSession(){
        mtx.lock();
        stop_thread = true;
        mtx.unlock();
        t->join();
    }

    static PublishThread& Get(std::string sess="stat.txt"){
        static PublishThread m(&AllocationTracker::WriteMemoryStats, sess);
        return m;
    }
};


class MemTracker{
    size_t tracked_bytes = 0;
    std::mutex mtx;
public:
    void UpdateBytes(size_t size){
        mtx.lock();
        tracked_bytes += size;
        mtx.unlock();
    }

    size_t GetTrackedBytes(){
        mtx.lock();
        size_t bytes_recorded = tracked_bytes;
        mtx.unlock();
        return bytes_recorded;
    }

};

//#define MEMTRACK
#ifdef MEMTRACK

#define MEMTRACK_START(name) Memtracker::Get(name).BeginSession()
#define MEMTRACK_STOP() MemTracker::Get().EndSession()

#endif // MEMTRACK



#endif //ORB_SLAM2_MEMORYTRACKER_H
