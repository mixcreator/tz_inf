#include <unistd.h>

#include "thread.h"
#include "queue.h"
#include "raw_data.h"


const uint16_t max_consumers = 4;
const uint16_t filename_len = 256;

int main()
{
    std::unordered_map<uint16_t, Queue::MyQueue <RawData::DataBlock*> > umap;

    Queue::MyQueue <std::vector<int32_t> >  transform_data;

    std::vector<Thread::ConsumerThread*>  consumers;
    for (int i = 0; i < max_consumers; i++)
    {
        char buff[filename_len];
        std::snprintf(buff, filename_len, "file%d.data", i+1);
        consumers.push_back(new Thread::ConsumerThread(transform_data, buff, std::to_string(i+1)));
    }

    for(auto t: consumers)
    {
        usleep(500000);
        t->start();
    }

    std::vector<Thread::TransformerThread*>  transformers;
    
    //Thread::ProcessThread thp(umap, transform_data, transformers, "data1.dat", "PTH");
    Thread::ProcessThread thp(umap, transform_data, transformers, "data2.dat", "PTH");
   
    thp.start();

    for(auto t: consumers)
    {
        t->join();
    }

    thp.join();
    
    
    for(auto t: transformers)
    {
        t->join();
    }

    return 0;
}