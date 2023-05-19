#pragma once

#include <queue>
#include <list>
#include <mutex>
#include <condition_variable>


namespace Queue
{

template <typename T>
class TSQueue
{
public:
    TSQueue() = default;

    //TSQueue(int a) {}

    TSQueue(const TSQueue<T> &) = delete;

    TSQueue<T>& operator=(const TSQueue<T> &) = delete;

    TSQueue<T>(TSQueue<T>&&) = default;
    
    TSQueue<T>& operator= (TSQueue<T>&&) = default;   //added move assignment operator

    void push(T&& data);
    
    //void push(const T& data)

    T& pop();

    T& peek();

    uint32_t size(); // const

    bool empty(); // const

    inline std::queue<T> get_queue()
    {
        return _queue;
    }

private:

    std::condition_variable _cv_empty;
    std::mutex _mutex; 
    std::queue<T> _queue; 
};

template <typename T>
class MyQueue
{
public:
    MyQueue() = default; 

    void push(T&& data);


    void init();

    T pop();

    T& peek();

    uint32_t size(); // const

    bool empty(); // const

    inline std::list<T>& get_queue()
    {
        return _queue;
    }
private:

    std::unique_ptr<std::condition_variable> _cv_empty_ptr = std::make_unique<std::condition_variable>();        
    std::unique_ptr<std::mutex> _mutex_ptr = std::make_unique<std::mutex>();
    std::list<T> _queue; 

};

} // namespace TQueue

