#include "ecc.hpp"

#include <vector>
#include <cstdint>
#include <arpa/inet.h>


void size_wrap(std::vector<uint8_t>& data){
	//we send 4 bits at a time so double the size of the vector
	uint32_t size = data.size()*2;
	
	//convert from host byte order to network byte order
	size = htonl(size);
	
	uint8_t byte1 = (size>>24)&0xFF;
	uint8_t byte2 = (size>>16)&0xFF;
	uint8_t byte3 = (size>>8)&0xFF;
	uint8_t byte4 = size&0xFF;
	
	//insert size at front of list
	data.insert(data.begin(), byte4);
	data.insert(data.begin(), byte3);
	data.insert(data.begin(), byte2);
	data.insert(data.begin(), byte1);
}


void size_unwrap(std::vector<uint8_t>& data){
	//remove first 4 bytes from data stream because that is where we stored the size
	data.erase(data.begin());
	data.erase(data.begin());
	data.erase(data.begin());
	data.erase(data.begin());
}


bool size_check(const std::vector<uint8_t>& data){
	uint8_t byte1, byte2, byte3, byte4;
	
	//get bytes from array
	byte1 = data[0];
	byte2 = data[1];
	byte3 = data[2];
	byte4 = data[3];
	
	//convert back into 1 var
	uint32_t size = (byte1<<24)|(byte2<<16)|(byte3<<8)|(byte4);
	
	//convert from network byte order to host byte order
	size = ntohl(size);

	//-4 because we added a uint32_t to the front of the vector
	//divide by 2 because we send data in 4bit chunks
	return size == (data.size()-4)/2;	
}

/*

//32 bit Cyclic redundancy check
void crc32_wrap(std::vector<uint8_t>& data){

	uint32_t crc_value = -1;
	
	for(auto byte : data){
		uint32_t lookup = crc_value ^ byte & 0xFF;
		crc_value = (crc_value >> 8) ^ CRCTable[lookup];
	}

	
	//convert from host byte order to network byte order
	crc_value = htonl(crc_value);
	
	uint8_t byte1 = (crc_value>>24)&0xFF;
	uint8_t byte2 = (crc_value>>16)&0xFF;
	uint8_t byte3 = (crc_value>>8)&0xFF;
	uint8_t byte4 = crc_value&0xFF;
	
	//insert size at front of list
	data.insert(data.begin(), byte4);
	data.insert(data.begin(), byte3);
	data.insert(data.begin(), byte2);
	data.insert(data.begin(), byte1);
	
}

void crc32_unwrap(std::vector<uint8_t>& data){
	//remove first 4 bytes from data stream because that is where we stored the crc value
	data.erase(data.begin());
	data.erase(data.begin());
	data.erase(data.begin());
	data.erase(data.begin());
}

bool crc32_check(const std::vector<uint8_t>& data){
	uint8_t byte1, byte2, byte3, byte4;
	
	//get bytes from array
	byte1 = data[0];
	byte2 = data[1];
	byte3 = data[2];
	byte4 = data[3];
	
	//convert back into 1 var
	uint32_t crc_value = (byte1<<24)|(byte2<<16)|(byte3<<8)|(byte4);
	
	//convert from network byte order to host byte order
	crc_value = ntohl(crc_value);
}

*/

