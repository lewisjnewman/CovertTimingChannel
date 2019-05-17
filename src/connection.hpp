#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>

#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <atomic>

#define SOCKET_RECV_TIMEOUT 10000


class Connection{
	
	typedef void(*CallbackFunction)(const std::vector<uint64_t>& raw_msg);

private:

	std::string dst_hostname;

	//socket file descriptor
	int sock;

	sockaddr_in myaddr;
	sockaddr_in destination;

	std::thread listen_thread;

	//mutex
	std::mutex socket_lock;
	std::mutex message_buffer_lock;

	std::vector<std::vector<uint64_t>> message_buffer;

	std::atomic_bool running;
	
	CallbackFunction callback = nullptr;

public:
	Connection(std::string dst, uint16_t local_port, uint16_t remote_port){

		running = true;

		dst_hostname = dst;

		
		//make new udp socket
		sock = socket(AF_INET, SOCK_DGRAM, 0);
		
		//setup sockaddr structure
		memset((char*)&myaddr, 0, sizeof(sockaddr_in));
		myaddr.sin_family = AF_INET;
		myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		myaddr.sin_port = htons(local_port);

		//bind socket to port
		bind(sock, (sockaddr*)&myaddr, sizeof(sockaddr_in));

		//get host information
		hostent *hp;
		hp = gethostbyname(dst.c_str());

		destination.sin_family = AF_INET;

		//copy host address into destination info
		memcpy((void*)&destination.sin_addr, hp->h_addr_list[0], hp->h_length);

		
		if(remote_port != 0){
			destination.sin_port = htons(remote_port);
		}

		//set socket recv timeout
		timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = SOCKET_RECV_TIMEOUT;
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
		
		//start listen thread
		listen_thread = std::thread(Connection::listen_daemon, sock, &message_buffer, &message_buffer_lock, &socket_lock, &running, &callback);
	}

	~Connection(){
		//mark that the destructor has been called
		running = false;

		//wait for other thread to stop
		listen_thread.join();

		//deallocate socket
		close(sock);
	}

	void set_callback(CallbackFunction cb){
		callback = cb;
	}

	//send a message, values are delay timings in microseconds between each message
	void send(const std::vector<uint64_t> data){
		sendto(sock, NULL, 0, 0, (sockaddr*)&destination, sizeof(sockaddr_in));

		for(auto delay : data){
			usleep(delay);
			sendto(sock, NULL, 0, 0, (sockaddr*)&destination, sizeof(sockaddr_in));
		}

		//sleep to signify end of message
		usleep(3*SOCKET_RECV_TIMEOUT);

	}

	//return the number of messages buffered up
	uint64_t num_messages(){
		message_buffer_lock.lock();
		uint64_t ret = message_buffer.size();
		message_buffer_lock.unlock();
		return ret;
	}

	//return the next message
	std::vector<uint64_t> get_message(){
		message_buffer_lock.lock();
		
		if(message_buffer.empty()){
			return {};
		}
		
		std::vector<uint64_t> ret = message_buffer[0];
		message_buffer.erase(message_buffer.begin());
		message_buffer_lock.unlock();
		return ret;
	}
	
	static void listen_daemon(int sock, std::vector<std::vector<uint64_t>>* message_buffer, std::mutex* msgbuffer_lock, std::mutex* socket_lock, std::atomic_bool* running, CallbackFunction* callback){

		//the buffer to recv messages into not that we will check it
		uint8_t buffer[1500];

		sockaddr_in remaddr;
		socklen_t remaddrlen;

		int recv_len = 0;

		timeval tv;

		std::vector<uint64_t> current = {};

		while(*running){

			socket_lock->lock();
			
			auto start = std::chrono::high_resolution_clock::now();
			recv_len = recvfrom(sock, buffer, 1500, 0, (sockaddr*)&remaddr, &remaddrlen);
			auto elapsed = std::chrono::high_resolution_clock::now() - start;
			
			socket_lock->unlock();
			
			uint64_t time_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

			//message split, push new message onto message buffer			
			if(time_elapsed >= SOCKET_RECV_TIMEOUT && current.size() > 0){
				current.erase(current.begin());
				
				msgbuffer_lock->lock();
				
				message_buffer->push_back(current);
				
				msgbuffer_lock->unlock();
				
				if(*callback != nullptr){
					(*callback)(current);
				}
				
				current = {};
			}
			else{
				current.push_back(time_elapsed);
			}

		}

	}

};


#endif


