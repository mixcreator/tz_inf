#include <iostream>
#include <string>
#include <fstream>

#include <unistd.h>

#include "thread.h"
//#include "process.h"
#include "raw_data.h"


namespace Thread
{


//ConsumerThread::ConsumerThread(Queue::MyQueue <uint8_t>& q, std::string file_name) 
//ConsumerThread::ConsumerThread(Queue::MyQueue <std::unique_ptr<uint8_t> >& q, std::string file_name) 
//ConsumerThread::ConsumerThread(Queue::MyQueue <std::unique_ptr<uint8_t[]> >& q, std::string file_name) 

ConsumerThread::ConsumerThread(Queue::MyQueue <std::vector<int32_t> >& q, std::string file_name, std::string context)
: _in_queue(q), _file_name(std::move(file_name)), _context(std::move(context))
{
    std::cout << "CTH:#" << _context <<  " this: " << this << " filename: " << _file_name << std::endl << std::flush;

}

ConsumerThread::~ConsumerThread()
{

}

void ConsumerThread::run()
{
    std::cout << "CTH:#" << this->_context <<  " this: " << this << " start! filename: " << this->_file_name << std::endl << std::flush; 
    
    std::ofstream wf(this->_file_name, std::ios::out | std::ios::binary);
    if(!wf)
    {
      std::cout << "Cannot open file!" << this->_file_name << std::endl << std::flush;
      return;
    }

    while(true)
    {
         auto data = _in_queue.pop();
         std::cout << "CTH:#" << _context << "  size: " << data.size() << "write ..." << std::endl << std::flush; 
         wf.write((char*)&data[0], data.size() * sizeof(int32_t));
         wf.flush();
         //wf.close();

    }
    wf.close();

}

TransformerThread::TransformerThread(
    Queue::MyQueue <RawData::DataBlock*>&  in_q, 
    Queue::MyQueue <std::vector<int32_t> >&  out_q,
    std::string context) : 
_in_queue(in_q), _out_queue(out_q), _context(std::move(context))
{

}

TransformerThread::~TransformerThread()
{

}

void TransformerThread::convert2i16_2_ieeef(RawData::DataBlock *dd)
{
    int size = dd->_vd.size();
    int num_i32_blocks = size/sizeof(int32_t);
    int modulo = dd->_vd.size() % sizeof(int32_t);

    std::vector<int32_t> resv;

    int i = 0;
    int32_t res;
    for(; i < num_i32_blocks; i++)
    {
        res = (dd->_vd[i * sizeof(int32_t) + 3] << 16) |
              (dd->_vd[i * sizeof(int32_t) + 2] << 24) |
              (dd->_vd[i * sizeof(int32_t) + 1]) |
              (dd->_vd[i * sizeof(int32_t)] << 8);
        
        resv.push_back(res);             
    }

    if(modulo) 
    {
        int32_t res = 0;
        switch(modulo)
        {
            case 3:
                res = ((dd->_vd[i * sizeof(int32_t) + 2] << 24) |
                       (dd->_vd[i * sizeof(int32_t) + 1]) | 
                       (dd->_vd[i * sizeof(int32_t)] << 8));
                break;
            case 2:
                res =  ((dd->_vd[i * sizeof(int32_t) + 1]) | 
                        (dd->_vd[i * sizeof(int32_t)] << 8));
                break;
            case 1:
                res = dd->_vd[i * sizeof(int32_t)] << 8;
                break;
        }
        resv.push_back(res);
    }


    _out_queue.push(std::move(resv));    
}

void TransformerThread::convert4ui8_2_ieeef(RawData::DataBlock *dd)
{
    int size = dd->_vd.size();
    int num_i32_blocks = size/sizeof(int32_t);
    int modulo = dd->_vd.size() % sizeof(int32_t);

    std::vector<int32_t> resv;

    int i = 0;
    int32_t res;
    for(; i < num_i32_blocks; i++)
    {
        res = (dd->_vd[i * sizeof(int32_t) + 3] << 24) |
              (dd->_vd[i * sizeof(int32_t) + 2] << 16) |
              (dd->_vd[i * sizeof(int32_t) + 1] << 8) |
              (dd->_vd[i * sizeof(int32_t)]);
        
        resv.push_back(res);             
    }

    if(modulo) 
    {
        int32_t res = 0;
        switch(modulo)
        {
            case 3:
                res = ((dd->_vd[i * sizeof(int32_t) + 2] << 16) |
                       (dd->_vd[i * sizeof(int32_t) + 1] << 8) | 
                       (dd->_vd[i * sizeof(int32_t)]));
                break;
            case 2:
                res =  ((dd->_vd[i * sizeof(int32_t) + 1] << 8) | 
                        (dd->_vd[i * sizeof(int32_t)]));
                break;
            case 1:
                res = dd->_vd[i * sizeof(int32_t)];
                break;
        }
        resv.push_back(res);
    }

    _out_queue.push(std::move(resv));
}

void TransformerThread::convert(RawData::DataBlock *dd)
{
    int size = dd->_vd.size();
    int num_i32_blocks = size/sizeof(int32_t);
    int modulo = dd->_vd.size() % sizeof(int32_t);

    std::vector<int32_t> resv;

    int i = 0;
    int32_t res;
    for(; i < num_i32_blocks; i++)
    {
        res = (dd->_vd[i * sizeof(int32_t) + 3] << 24) |
              (dd->_vd[i * sizeof(int32_t) + 2] << 16) |
              (dd->_vd[i * sizeof(int32_t) + 1] << 8) |
              (dd->_vd[i * sizeof(int32_t)]);
        
        resv.push_back(res);             
    }

    if(modulo) 
    {
        int32_t res = 0;
        switch(modulo)
        {
            case 3:
                res = ((dd->_vd[i * sizeof(int32_t) + 2] << 16) |
                       (dd->_vd[i * sizeof(int32_t) + 1] << 8) | 
                       (dd->_vd[i * sizeof(int32_t)]));
                break;
            case 2:
                res =  ((dd->_vd[i * sizeof(int32_t) + 1] << 8) | 
                        (dd->_vd[i * sizeof(int32_t)]));
                break;
            case 1:
                res = dd->_vd[i * sizeof(int32_t)];
                break;
        }
        resv.push_back(res);
    }

    _out_queue.push(std::move(resv));

}

void TransformerThread::run()
{
    std::cout << "TTH:#" << this->_context <<  " this: " << this << " start!" << std::endl << std::flush;

    while(true)
    {
        auto data = _in_queue.pop();
        
        std::cout << "---------------->>TranTH:#" 
                  << this->_context <<  " " << this << " data plsz:" 
                                                          << data->_payload_size 
                                                          << " data pl tp: "
                                                          << data->_payload_type
                                                          << std::endl << std::flush;
        
        fflush(stdout);
        uint8_t tp = data->_payload_type;
        switch(tp)
        {
            case ui8:
                convert4ui8_2_ieeef(data);
                break;
            case i16:
                convert2i16_2_ieeef(data);
                break;
            case i32:
            case ieeef:
                convert(data);
                break;
        }
        delete data; //!!!!!!!
        data = nullptr;
    }
}

ProcessThread::ProcessThread(
    std::unordered_map<uint16_t, Queue::MyQueue <RawData::DataBlock*> >& m,
    Queue::MyQueue <std::vector<int32_t> >& out_queue,
    std::vector<Thread::TransformerThread*>&  transformers,
    std::string file_name,
    std::string context)
: _file_name(file_name), 
  _processor(m, out_queue, transformers, context),_context(std::move(context))
{
    std::cout << "TH: " << _context <<  " this: " << this << " filename: " << _file_name << std::endl << std::flush;
}

ProcessThread::~ProcessThread()
{

}

void ProcessThread::run()
{
    std::cout << "TH: " << this->_context <<  " this: " << this << " start!" << std::endl << std::flush;
    while(true)
    {
        std::ifstream ins(_file_name, std::ios::in |std::ios::binary);
        if(!ins)
        {
            std::cout << "TH: " << this->_context << " this: " << this << " Cannot open file: " << _file_name << " Exit "<< std::endl << std::flush;
            return;
        }

        uint32_t length = 25856;//102;//104;//1000;
        uint8_t * buffer = new uint8_t [length];
        std::cout << "TH: " << this->_context <<  " this: " << this << " try reading " << length << "bytes"<< std::endl << std::flush; 
        ins.read((char*)buffer, length);;
        if (ins)
        {
            std::cout << "all characters read successfully.";
            std::cout << "----------------------------------" << std::endl << std::flush;
        }
        else
        {
            std::cout << "error: only " << ins.gcount() << " could be read" << std::endl << std::flush;
            std::cout << "----------------------------------" << std::endl << std::flush;

        }
        _processor.set_buff(buffer);
        bool res = _processor.fill_frames(length);
        if(!res)
        {
            _processor.inc_failed_frames();
        }

        ins.close();   

        delete []buffer;
        buffer = nullptr;
        usleep(100000);
    }
}

} // namespace Thread

