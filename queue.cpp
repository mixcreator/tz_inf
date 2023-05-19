#include "queue.h"
#include "raw_data.h"


namespace Queue
{

template <typename DataType>
void TSQueue<DataType>::push(DataType&& data)
{
        std::unique_lock<std::mutex> lock(_mutex);
        _queue.push(data);
        _cv_empty.notify_one();
}
    
template <typename DataType>
DataType& TSQueue<DataType>::pop()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cv_empty.wait(lock, [this](){return !_queue.empty();});
    
    DataType& res =  _queue.front();
    _queue.pop();
    return res;
}

template <typename DataType>
DataType& TSQueue<DataType>::peek()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cv_empty.wait(lock, [this](){return !_queue.empty();});
    
    return _queue.front();
}
    
template <typename DataType>
uint32_t TSQueue<DataType>::size() // const
{
    std::unique_lock<std::mutex> lock(_mutex);
    return _queue.size();
}

template <typename DataType>
bool TSQueue<DataType>::empty() // const
{
    std::unique_lock<std::mutex> lock(_mutex);
    return _queue.empty();
}

template class TSQueue<uint8_t>;
template class TSQueue<uint8_t*>;
template class TSQueue<std::string>;



template <typename DataType>
void MyQueue<DataType>::init()
{
    _mutex_ptr = std::make_unique<std::mutex>();
    _cv_empty_ptr = std::make_unique<std::condition_variable>();
}

template <typename DataType>
void MyQueue<DataType>::push(DataType&& data)
{
    std::unique_lock<std::mutex> lock(*_mutex_ptr.get());
    _queue.push_back(std::move(data)); // !!!
    _cv_empty_ptr->notify_one();
}

template <typename DataType>
DataType MyQueue<DataType>::pop()
{
    std::unique_lock<std::mutex> lock(*_mutex_ptr.get());
    _cv_empty_ptr->wait(lock, [this](){return !_queue.empty();});
    
    DataType res =  _queue.front();
    //_queue.pop();
    _queue.pop_front();
    return res;
}

template <typename DataType>
DataType& MyQueue<DataType>::peek()
{
    std::unique_lock<std::mutex> lock(*_mutex_ptr.get());
    _cv_empty_ptr->wait(lock, [this](){return !_queue.empty();});
    
    return _queue.front();
}
    
template <typename DataType>
uint32_t MyQueue<DataType>::size() // const
{
    std::unique_lock<std::mutex> lock(*_mutex_ptr.get());
    return _queue.size();
}

template <typename DataType>
bool MyQueue<DataType>::empty() // const
{
    std::unique_lock<std::mutex> lock(*_mutex_ptr.get());
    return _queue.empty();
}

template class MyQueue <std::vector<int32_t> >;
template class MyQueue <RawData::DataBlock*>;

} // namespace Queue
