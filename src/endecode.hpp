#ifndef ENDECODE_HPP
#define ENDECODE_HPP

//endecode = encode/decode

#include <cstdint>
#include <vector>

extern std::vector<uint64_t> encode(std::vector<uint8_t> raw_binary);
extern std::vector<uint8_t> decode(std::vector<uint64_t> raw_delay);

#endif
