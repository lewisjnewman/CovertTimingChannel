#include <iostream>
#include <string>

#include <cstring>

#include "connection.hpp"
#include "endecode.hpp"
#include "ecc.hpp"

//convert string to list of raw bytes
std::vector<uint8_t> get_raw_data(const std::string& str){
	std::vector<uint8_t> data;
	data.resize(str.size());
	memcpy(&data[0], str.c_str(), data.size());
	return data;
}

void recieve_message(const std::vector<uint64_t>& raw_message){
	auto recv_msg = decode(raw_message);
	
	if(size_check(recv_msg)){
		std::cout << "Error with Message Size" << std::endl;
	}
	
	size_unwrap(recv_msg);

	//print out the final decoded message
	std::cout << "The Decoded Message is: ";
	for(auto byte : recv_msg){
		printf("%c", byte);
	}
	std::cout << "\n" << "Message Recieved" << std::endl;
}

int main(int argc, char** argv){


	Connection c1("localhost", 10000, 10001);
	Connection c2("localhost", 10001, 10000);

	c2.set_callback(recieve_message);

	const char* str_msg = "Hello, World. What a nice day it is today";
	
	std::vector<uint8_t> msg;
	
	msg.resize(strlen(str_msg));
	memcpy(&msg[0], str_msg, msg.size());
	
	size_wrap(msg);
	
	auto delays = encode(msg);
	
	c1.send(delays);

	std::cout << "Message Sent" << std::endl;

	sleep(3);

	return 0;
}
