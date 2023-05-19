#include "raw_data.h"

#include "thread.h"


namespace Thread
{

DataProcessor::DataProcessor(
    std::unordered_map<uint16_t, Queue::MyQueue <RawData::DataBlock*> >& umap,
    Queue::MyQueue <std::vector<int32_t> >& out_queue,
    std::vector<Thread::TransformerThread*>&  transformers,
    std::string context)
: _context(std::move(context)), _umap(umap), _out_queue(out_queue), _transformers(transformers)
{
    _state = process_magic_word;
}


DataProcessor::~DataProcessor()
{

}


ProcessState DataProcessor::get_state()
{
        return _state;
}

void DataProcessor::set_state(ProcessState state)
{
    _state = state;
}

uint32_t DataProcessor::get_raw_bytes_number() // const
{  
    return _buffer_size - _total_bytes_processed;  
}

bool DataProcessor::are_enough_raw_bytes() // const
{
    return (_left_process_frame_bytes <= get_raw_bytes_number());
}

void DataProcessor::set_buff(uint8_t *buff)
{
    _pbuff = buff;
}

void DataProcessor::set_size(uint32_t size)
{
    _buffer_size = size;
}

int DataProcessor::search_magic_number()
{  
    if(are_enough_raw_bytes())
    {
        _magic_number = *(uint32_t*)_pbuff;

        if( _magic_number == MAGIC_WORD)
        {
            _pbuff += sizeof(uint32_t);
            _frame_bytes_processed += sizeof(uint32_t);
            _total_bytes_processed += sizeof(uint32_t);
            
            set_state(process_frame_count);
            _left_process_frame_bytes = sizeof(uint16_t); // sizeof frame counter
            _magic_number = 0;
            _start_frame_not_found = true;

            return 0;
        }
        _pbuff += sizeof(uint8_t);
        _frame_bytes_processed += sizeof(uint8_t);
        _total_bytes_processed += sizeof(uint8_t);
        _start_frame_not_found = false;   
    }
    return -1;
}

int DataProcessor::get_frame_counter()
{
    if(are_enough_raw_bytes())
    {
        _frame_counter = *((uint16_t*)_pbuff);
        _pbuff += sizeof(uint16_t);
        _frame_bytes_processed += sizeof(uint16_t);
        _total_bytes_processed += sizeof(uint16_t);

        _left_process_frame_bytes = sizeof(uint8_t);    // sizeof source id
        set_state(process_source_id);
        //_frame_counter = 0;
        
        return 0;
    }
    return -1;
}

int DataProcessor::get_source_id()
{
    if(are_enough_raw_bytes())
    {
        _source_id = *((uint8_t*)_pbuff);

        // check _source_id !!!

        _pbuff += sizeof(uint8_t);
        _frame_bytes_processed += sizeof(uint8_t);
        _total_bytes_processed += sizeof(uint8_t);

        _left_process_frame_bytes = sizeof(uint8_t);    // sizeof payload type
        set_state(process_payload_type);
        return 0;
    }

    return -1;
}

int DataProcessor::get_payload_type()
{
    if(are_enough_raw_bytes())
    {
        _payload_type = *((uint8_t*)_pbuff);

        // check _payload_type !!!
        _pbuff += sizeof(uint8_t);
        _frame_bytes_processed += sizeof(uint8_t);
        _total_bytes_processed += sizeof(uint8_t);

        _left_process_frame_bytes = sizeof(uint32_t);    // sizeof payload size
        set_state(process_payload_size);
        return 0;
    }

    return -1;
}

int DataProcessor::get_payload_size()
{
    if(are_enough_raw_bytes())
    {
        _payload_size = *(uint32_t*)_pbuff;
        _frame_bytes_processed += sizeof(uint32_t);
        _total_bytes_processed += sizeof(uint32_t);
        _pbuff += sizeof(uint32_t);
        _left_process_frame_bytes = _payload_size; // sizeof payload 
        set_state(process_payload);
        return 0;
    }

    return -1;

}

int DataProcessor::get_payload()
{
    if(are_enough_raw_bytes())
    {
        auto data = new RawData::DataBlock(
            MAGIC_WORD,
            _frame_counter,
            _source_id,
            _payload_type,
            _payload_size,
             _pbuff);

        #if 1
        printf("-----------------------------------------K<%d> %d\n", _source_id, (_umap.find(data->_source_id) == _umap.end()));fflush(stdout);
        
        if(_umap.find(data->_source_id) == _umap.end())
        {
            std::cout << this->_context <<  " this: " << this << " Key does not exists <" << _source_id << ">" << std::endl << std::flush;

            _umap.emplace(
                std::make_pair(
                    _source_id,
                    Queue::MyQueue <RawData::DataBlock*>() ) );

            Thread::TransformerThread *tth = 
                new Thread::TransformerThread(
                    _umap[_source_id], 
                    _out_queue,
                    std::to_string(_source_id));

            _transformers.push_back(tth);
            #if 1
            tth->start();
            #endif

            _umap[_source_id].push(std::move(data));
            

        }
        else
        {
            std::cout << this->_context <<  " this: " << this << " Key  exists, push ... " << _source_id << std::endl << std::flush;
            _umap[_source_id].push(std::move(data));
            
        }
        #endif

        _pbuff += _payload_size;
        _frame_bytes_processed += _payload_size;
        _total_bytes_processed += _payload_size;

        _left_process_frame_bytes = sizeof(uint32_t);  // sizeof magic word (start state)

        _frame_bytes_processed = 0; // ??

        set_state(process_magic_word);
        _magic_number = 0;
        _frame_counter = 0;
        _payload_size = 0;


        return 0;
    }
    _left_process_frame_bytes = sizeof(uint32_t);  // sizeof magic word (start state)
    set_state(process_magic_word);

    return -1;
}

uint32_t DataProcessor::get_success_frames_cnt()
{
    return _success_frames_cnt;
}

uint32_t DataProcessor::get_failed_frames_cnt()
{
    return _failed_frames_cnt;
}

void DataProcessor::inc_success_frames()
{
    _success_frames_cnt++;
}

void DataProcessor::inc_failed_frames()
{
    _failed_frames_cnt++;
}


bool DataProcessor::fill_frames(const int last_read_bytes)
{
    set_size((uint32_t)last_read_bytes);
    _frame_bytes_processed = 0;
    _total_bytes_processed = 0;
    
    while(_total_bytes_processed < last_read_bytes)
    {
        switch(get_state())
        {
            case process_magic_word:
            {
                if(search_magic_number() < 0)
                {
                    return false;
                }
                break;
            }
            case process_frame_count:
            {
                if(get_frame_counter() < 0)
                {
                    return false;
                }
                break;
            }
            case process_source_id:
            {
                if(get_source_id() < 0)
                {
                    return false;
                }
                break;
            }
            case process_payload_type:
            {
                if(get_payload_type() < 0)
                {
                    return false;
                }
                break;
            }
            case process_payload_size:
            {
                if(get_payload_size() < 0)
                {
                    return false;
                }
                break;
            } 
            case process_payload:
            {
                if(get_payload() < 0) 
                {
                    return false;
                }
                inc_success_frames();
                break;
            }
        }
    }

    if(!_start_frame_not_found)
        return -1;

    return true;
}


} // namespace Thread



