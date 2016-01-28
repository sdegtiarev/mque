#include <iostream>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <time.h>
#include <unistd.h>
#include <assert.h>
std::deque<timespec> que;
int flag=0;
std::mutex mtx;
std::condition_variable cv1, cv2;



void echo()
{
	std::unique_lock<std::mutex> lock(mtx);
	for(;;) {
		cv1.wait(lock, []{ return !que.empty(); });
		
		auto t0=que.front();
		que.pop_front();
		timespec t1;
		clock_gettime(CLOCK_MONOTONIC, &t1);
		std::cout<<(t1.tv_sec-t0.tv_sec)*1e9+(t1.tv_nsec-t0.tv_nsec)<<"\n";

		assert(flag);
		cv2.notify_one();
	}
}



int main()
{
	std::thread(&echo).detach();

	std::unique_lock<std::mutex> lock(mtx);
	for(int i=0; i < 1000000; ++i) {
		{
			timespec t;
			clock_gettime(CLOCK_MONOTONIC, &t);
			auto wakeup=que.empty();
			que.push_back(t);

			if(wakeup) cv1.notify_one();
			flag=1;
			cv2.wait(lock);
			flag=0;
		}
	}

	return 0;
}


		
