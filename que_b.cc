#include <iostream>
#include <deque>
#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <boost/asio.hpp>
#include <time.h>
std::deque<timespec> que;
std::mutex mtx;
std::condition_variable cv;
boost::asio::io_service io;
using work=boost::asio::io_service::work;
using work_p=std::unique_ptr<work>;

void echo()
{
	static int cnt=0;
	timespec t0;
	while(!que.empty()) {
		{
			std::unique_lock<std::mutex> lock(mtx);
			t0=que.front();
			que.pop_front();
		}
		timespec t1;
		clock_gettime(CLOCK_MONOTONIC, &t1);
		std::cout<<(t1.tv_sec-t0.tv_sec)*1e9+(t1.tv_nsec-t0.tv_nsec)<<"\n";

		cv.notify_one();
	}
}


void submit(work_p w_)
{
	work_p w{std::move(w_)};
	std::unique_lock<std::mutex> lock(mtx);
	for(int i=0; i < 1000000; ++i) {
		timespec t;
		clock_gettime(CLOCK_MONOTONIC, &t);
		que.push_back(t);
		io.post(echo);
		cv.wait(lock);
	}
}


int main()
{
	std::thread(&submit, work_p{new work(io)}).detach();
	return io.run();
}

