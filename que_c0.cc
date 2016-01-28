#include <iostream>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <time.h>
#include <unistd.h>
#include <assert.h>
std::deque<timespec> que;
std::mutex mtx;
std::condition_variable cv;



void echo()
{
	for(;;) {
		std::unique_lock<std::mutex> lock(mtx);
		while(!que.empty()) {
			auto t0=que.front();
			que.pop_front();
			timespec t1;
			clock_gettime(CLOCK_MONOTONIC, &t1);
			std::cout<<(t1.tv_sec-t0.tv_sec)*1e9+(t1.tv_nsec-t0.tv_nsec)<<"\n";
		}
		cv.notify_one();
	}
}



int main()
{
	std::thread(&echo).detach();

//	std::unique_lock<std::mutex> lock(mtx);
	for(int i=0; i < 1000000; ++i) {
//		std::unique_lock<std::mutex> lock(mtx);
		timespec t;
		clock_gettime(CLOCK_MONOTONIC, &t);
		std::unique_lock<std::mutex> lock(mtx);
		que.push_back(t);
		cv.wait(lock);
	}

	return 0;
}


		
