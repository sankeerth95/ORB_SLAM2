//
// Created by sankeerth on 04/12/20.
//

#ifndef ORB_SLAM2_MEMORYTRACKER_H
#define ORB_SLAM2_MEMORYTRACKER_H

#include <memory>
#include <fstream>
#include <thread>
#include <mutex>
#include <limits>

#define DONOTREDEFINENEW
#ifndef DONOTREDEFINENEW
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
#endif //DONTREDEFINENEW

namespace AllocationTracker{

    struct AllocationStats{
        size_t bytesAllocated = 0;
        size_t bytesFreed = 0;
    }; 

    void UpdateAlloc(size_t i);

    void UpdateFree(size_t i);

    size_t getNetComponent();

    template<typename T>
    struct my_alloc {

        typedef T value_type;

        my_alloc() = default;
        template <class U> constexpr my_alloc (const my_alloc <U>&) noexcept {}

        [[nodiscard]] T* allocate(std::size_t n){
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
                throw std::bad_alloc();

            if (auto p = static_cast<T*>(std::malloc(n*sizeof(T)))) {
                UpdateAlloc(sizeof(T)*n);
                return p;
            }

            throw std::bad_alloc();
        }

        void deallocate(T* p, std::size_t n) noexcept{
            UpdateFree(sizeof(T)*n);
            std::free(p);
        }
    };
}


template <class T, class U>
bool operator==(const AllocationTracker::my_alloc <T>&, const AllocationTracker::my_alloc <U>&) { return true; }
template <class T, class U>
bool operator!=(const AllocationTracker::my_alloc <T>&, const AllocationTracker::my_alloc <U>&) { return false; }


class PublishThread{
    bool stop_thread;
    std::ofstream ofs;
    std::thread* t;
    size_t(*getnum)();
    std::mutex mtx;

public:
    PublishThread(size_t(*f)(), const std::string& filename);

    void WriteBytes();

    void BeginSession();

    void EndSession();

    static PublishThread& Get(std::string sess="stat.txt");
};

//SimpleMemtracker.....
class MemTracker{
    size_t tracked_bytes = 0;
    std::mutex mtx;
public:
    void UpdateBytes(size_t size);
    size_t GetTrackedBytes();
};

//#define MEMTRACK
#ifdef MEMTRACK

#define MEMTRACK_START(name) Memtracker::Get(name).BeginSession()
#define MEMTRACK_STOP() MemTracker::Get().EndSession()

#endif // MEMTRACK



#endif //ORB_SLAM2_MEMORYTRACKER_H
