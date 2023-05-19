#pragma once

#include <thread>
#include <string_view>

#include "queue.h"
//#include "process.h"
#include "raw_data.h"



namespace Thread
{

class TThread {

public:
	/**
	 * Starts the thread.
	 **/
	inline void start() {
		uthread = std::thread(&TThread::run, this);
	}

	/**
	 * Waits for the thread to terminate.
	 **/
	inline void join() {
		uthread.join();
	}

	inline void detach() {
		uthread.detach();
	}

protected:

	virtual void run() = 0;	

private:
	std::thread uthread;
};



class ConsumerThread: public TThread
{
public:
    explicit ConsumerThread(
        Queue::MyQueue <std::vector<int32_t> >&  q,
        std::string file_name,
        std::string context);
    ~ConsumerThread();

    void run();
    Queue::MyQueue <std::vector<int32_t> >&  _in_queue;

    std::string _file_name;
    std::string _context;
};

class TransformerThread: public TThread
{
public:
    explicit TransformerThread(Queue::MyQueue <RawData::DataBlock*>&  in_q,
                               Queue::MyQueue <std::vector<int32_t> >&  out_q,
                               std::string  context);
    ~TransformerThread();

    void run();

    void convert4ui8_2_ieeef(RawData::DataBlock *d);

    void convert2i16_2_ieeef(RawData::DataBlock *d);   

    void convert(RawData::DataBlock *dd); 

private:
    Queue::MyQueue <RawData::DataBlock*> & _in_queue;
    Queue::MyQueue <std::vector<int32_t> >& _out_queue;
    std::string _context;


};


#define MAGIC_WORD            (uint32_t)0xDDCCBBAA

enum
{
    ui8 = 0,
    i16,
    i32,
    ieeef
};


enum ProcessState
{
    process_magic_word = 0,
    process_frame_count,
    process_source_id,
    process_payload_type,
    process_payload_size,
    process_payload//,
    //process_frame_success
};

// from process.h
class DataProcessor
{
public:
    explicit DataProcessor(
        std::unordered_map<uint16_t, Queue::MyQueue <RawData::DataBlock*> >& umap,
        Queue::MyQueue <std::vector<int32_t> >& _out_queue,
        std::vector<Thread::TransformerThread*>&  transformers,
        std::string context);

    ~DataProcessor();

    ProcessState get_state();//const;

    void set_state(ProcessState state);
    
    bool fill_frames(int buff_size);

    void set_buff(uint8_t *buff);

    void set_size(uint32_t size);

    uint32_t get_success_frames_cnt();

    uint32_t get_failed_frames_cnt();

    void inc_success_frames();

    void inc_failed_frames();

private:
    uint32_t get_raw_bytes_number();
    
    bool are_enough_raw_bytes();
    
    int search_magic_number();

    
    int get_frame_counter();

    int get_source_id();

    int get_payload_type();

    int get_payload_size();

    int get_payload();

    
private:

    ProcessState _state;

    uint32_t _buffer_size;

    uint32_t _frame_bytes_processed = 0;
    uint32_t _total_bytes_processed = 0;
    uint32_t _left_process_frame_bytes = sizeof(uint32_t);
    uint32_t _magic_number = 0;
    uint16_t _frame_counter = 0;
    uint8_t _source_id;
    uint8_t _payload_type;
    uint32_t _payload_size;
    bool _start_frame_not_found = false;

    uint32_t _success_frames_cnt = 0;
    uint32_t _failed_frames_cnt = 0;

    uint8_t *_pbuff;
    std::string _context;


    std::unordered_map<uint16_t, Queue::MyQueue <RawData::DataBlock*> >& _umap;
    Queue::MyQueue <std::vector<int32_t> >& _out_queue;
    std::vector<Thread::TransformerThread*>&  _transformers;
};



class ProcessThread: public TThread
{
public:
    explicit ProcessThread(
        std::unordered_map<uint16_t, Queue::MyQueue <RawData::DataBlock*> >& m,
        Queue::MyQueue <std::vector<int32_t> >& out_queue,
        std::vector<Thread::TransformerThread*>&  transformers,
        std::string file_name,
        std::string context);
    ~ProcessThread();

    void run();

private:
    std::string _file_name;
    DataProcessor _processor;
    std::string _context;
};




} // namespace Thread



