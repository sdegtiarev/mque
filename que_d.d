import std.stdio, std.concurrency, std.datetime, core.thread;


void main()
{
	immutable int N=1000000;
	auto tid=spawn(&f, N);
	foreach(i; 0..N) {
		auto t=MonoTime.currTime;
		tid.send(thisTid, t.ticks);
		// wait receiver for 
		receiveOnly!int();
	}
}


void f(int n)
{
	foreach(i; 0..n) {
		auto m=receiveOnly!(Tid,long)();
		writeln(MonoTime.currTime.ticks-m[1]);
		// ask for next message
		m[0].send(0);
	}
}


