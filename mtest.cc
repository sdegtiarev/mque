#include <boost/thread.hpp>
#include <time.h>
#include <atomic>
#include <mutex>
#include <iostream>
static void usage();

const size_t N=10000;
unsigned long stat[N];
std::atomic<unsigned> idx;
std::unique_ptr<boost::barrier> start;

void test_void() {}

void test_yield() { boost::this_thread::yield(); }

void test_sleep(unsigned t) { usleep(t); }

void test_mutex() 
{
	static std::mutex m;
	std::lock_guard<std::mutex> lock(m);
	boost::this_thread::yield();
}


template<typename FN>
std::function<void()> test(FN f)
{
	return [=]{
		timespec st, ft;
		start->wait();
		for(unsigned i=idx++; i < N; i=idx++) {
			clock_gettime(CLOCK_MONOTONIC, &st);
			f();
			clock_gettime(CLOCK_MONOTONIC, &ft);
			stat[i]=(ft.tv_sec-st.tv_sec)*1000000000+(ft.tv_nsec-st.tv_nsec);
		}
	};
}


int main(int argc, char**argv)
{
	std::function<void()>	mode=test_void;
	unsigned nth=1, delay=0;
	int opt;
	while((opt=getopt(argc,argv,"MYU:t:h")) != -1)
	switch(opt) {
		case 'M': mode=test_mutex; break;
		case 'Y': mode=test_yield; break;
		case 'U': delay=atol(optarg); mode=[=]{test_sleep(delay);}; break;
		case 't': nth=atol(optarg); if(nth) break;
		case 'h':
		default : usage(); return 1;
	}

	start.reset(new boost::barrier(nth));
	boost::thread_group thr;
	for(int i=0; i != nth; ++i)
		thr.create_thread(test(mode));
	thr.join_all();
	
	for(size_t i=0; i < N; ++i)
		std::cout<<stat[i]<<std::endl;
	return 0;
}


static void usage()
{
	std::cout<<
	"yld: yield() timing\n"
	"Options:\n"
	"  -U<mks>    : run uleep(mks) test\n"
	"  -Y         : run yield() test\n"
	"  -M         : run mutex lock()/unlock() test\n"
	"  -t<thr>    : start with <thr> threads\n"
	"  -h         : print this help and quit\n"
	<<std::endl;
}
