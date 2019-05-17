#ifndef ECC_HPP
#define ECC_HPP

#include <vector>
#include <cstdint>

//simple size checking
extern void size_wrap(std::vector<uint8_t>& data);
extern void size_unwrap(std::vector<uint8_t>& data);
extern bool size_check(const std::vector<uint8_t>& data);

#endif
