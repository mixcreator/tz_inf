#pragma once

#include <cstdint>
#include <iostream>
#include <vector>


namespace RawData
{

struct DataBlock
{
    uint32_t _magic_word;
    uint16_t _frame_counter;
    uint8_t  _source_id;
    uint8_t  _payload_type;
    uint32_t _payload_size;
    uint8_t* _d;
    std::vector<uint8_t> _vd;

    explicit DataBlock(
        uint32_t magic_word,
        uint16_t frame_counter,
        uint8_t  source_id,
        uint8_t  payload_type,
        uint32_t payload_size,
        uint8_t *raw_data);

    ~DataBlock();
};


} // namespace RawData
