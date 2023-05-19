#include "raw_data.h"

namespace RawData
{

DataBlock::DataBlock(
    uint32_t magic_word,
    uint16_t frame_counter,
    uint8_t  source_id,
    uint8_t  payload_type,
    uint32_t payload_size,
    uint8_t *raw_data)
: _magic_word(magic_word),
  _frame_counter(frame_counter), 
  _source_id(source_id),
  _payload_type(payload_type),
  _payload_size(payload_size)
{
    std::copy(raw_data, raw_data+payload_size, back_inserter(_vd)); 
}

DataBlock::~DataBlock()
{
}

} // namespace RawData