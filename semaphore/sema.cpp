#include <mutex>
#include <iostream>
#include <thread>
#include <deque>
#include <condition_variable>
#include <cstdlib>
#include <ctime>
#include <cassert>

std::condition_variable not_full, not_empty;
std::mutex m;
std::deque<int> queue;
size_t max_size = 5;

void deposit(int v)
{
	/* Lock access to the deque. */
	std::unique_lock<std::mutex> l(m);

	/* Block until queue size is smaller than the max size permitted. */
	not_full.wait(l, []() { return queue.size() < max_size;});

	/* Queue size should never be bigger than max_size. */
	assert(queue.size() < max_size);

	/* Insert element. */
	queue.push_back(v);

	/* Notify threads waiting on not_empty to recheck their predicate. */
	not_empty.notify_one();
}

int fetch()
{
	/* Lock access to the deque. */
	std::unique_lock<std::mutex> l(m);

	/* Block until queue is not empty. */
	not_empty.wait(l, [](){return !queue.empty();});

	/* Queue size should never be zero at this point. */
	assert(!queue.empty());

	/* Remove element. */
	int ret = queue.back();
	queue.pop_back();

	/* Notify threads waiting on not_full to recheck their predicate. */
	not_full.notify_one();

	return ret;
}

int main()
{
	volatile bool done = false;
	std::thread([&done]() {
		while(!done) {
			std::cout << fetch() << std::endl;
		}
	}).detach();

	/* Fetch thread will block on the predicate !queue.empty(). */
	std::this_thread::sleep_for(std::chrono::seconds(5));

	/* As soon as this thread deposits a value, 
	 * thread 1 will print it out. */
	std::thread([&done]() {

		srand(time(NULL));
		while(!done) {
			deposit(rand());	
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

	}).detach();

	/* Continue until sigint. */
	for(;;);
}

