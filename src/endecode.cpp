#include "endecode.hpp"

#include <cstdint>
#include <vector>

// 0.0ms, 0.5ms, 1.0ms etc...
uint64_t coding_table[] = {0000, 0500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 9000};

std::vector<uint64_t> encode(std::vector<uint8_t> raw_binary){

	std::vector<uint64_t> delays = {};
	
	for(uint8_t byte : raw_binary){
		uint8_t lower = byte & 0x0F;
		uint8_t higher = (byte>>4);
		delays.push_back(coding_table[higher]);
		delays.push_back(coding_table[lower]);
	}
	
	return delays;
}


std::vector<uint8_t> decode(std::vector<uint64_t> raw_delay){
	
	std::vector<uint8_t> data = {};
	
	//check for odd number of delays
	//if so add a zero onto the end to make it even
	if(raw_delay.size()%2 != 0){
		raw_delay.push_back(0);
	}
	
	uint32_t i = 0;
	while(i < raw_delay.size()){
		uint64_t high = raw_delay[i];
		uint64_t low = raw_delay[i+1];
		
		uint8_t byte = 0;
		
		for(uint32_t i = 0; i < 16; i++){
			if(high > coding_table[i] && high < coding_table[i+1]){
				byte |= i<<4;
				break;
			}
		}
		
		for(uint32_t i = 0; i < 16; i++){
			if(low > coding_table[i] && low < coding_table[i+1]){
				byte |= i;
				break;
			}
		}
		
		data.push_back(byte);
		
		i += 2;
	}
	
	return data;
}

