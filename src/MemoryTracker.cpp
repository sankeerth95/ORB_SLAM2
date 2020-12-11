//
// Created by root on 12/9/20.
//


#include <chrono>

#include <include/MemoryTracker.h>


static std::mutex allocation_lock;

static AllocationTracker::AllocationStats memStats;


void AllocationTracker::UpdateAlloc(size_t i) {
    allocation_lock.lock();
    memStats.bytesAllocated += i;
    allocation_lock.unlock();
}

void AllocationTracker::UpdateFree(size_t i) {
    allocation_lock.lock();
    memStats.bytesFreed += i;
    allocation_lock.unlock();
}

size_t AllocationTracker::getNetComponent(){
    allocation_lock.lock();
    size_t netBytes = memStats.bytesAllocated - memStats.bytesFreed;
    allocation_lock.unlock();
    return netBytes;
}



PublishThread::PublishThread(size_t(*f)(), const std::string& filename):ofs(filename),getnum(f){
}

void PublishThread::WriteBytes() {
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

void PublishThread::BeginSession(){
    stop_thread = false;
    mtx.unlock();
    t = new std::thread(&PublishThread::WriteBytes, this);
}

void PublishThread::EndSession(){
    mtx.lock();
    stop_thread = true;
    mtx.unlock();
    t->join();
}

PublishThread& PublishThread::Get(std::string sess){
    static PublishThread m(&AllocationTracker::getNetComponent, sess);
    return m;
}




void MemTracker::UpdateBytes(size_t size){
    mtx.lock();
    tracked_bytes += size;
    mtx.unlock();
}

size_t MemTracker::GetTrackedBytes(){
    mtx.lock();
    size_t bytes_recorded = tracked_bytes;
    mtx.unlock();
    return bytes_recorded;
}
