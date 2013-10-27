About 
======

An asynchronous parallel networking library. This project makes full use of the multi-paradigm nature of C++, utilising an Imperative style where classes are not needed, an Object Oriented style where inheritance and polymorphism actually is needed and Functional style higher order functions and control flow. 

Kyukon was the original name of Ninetails, the Pokemon.

Purpose
======

This library is designed for environment where network traffic is limited by an external factor. Where a series of unrelated network operations would otherwise be done in serial, Kyukon allows them to act in parallel, thus minimising wasted bandwidth and increasing the speed of the overall operation. For example if a webserver limits each connection to 50kb/s then, assuming your DSL bandwidth is 500kb/s, opening 10 parallel connections would yield a 10x speedup.

How it Works
======

	1) Initialise the library and tell it how many threads to use.
	2) Create a task object and tell it the URL to grab and what function to call when it's done.
	3) Add the task to the library's processing queue.
	4) Continue processing from the callback function once it is invoked by the library.

At the moment the library only supports HTTP GET requests and depends on curl.
The library expects task objects to be pointers to avoid unnecessary copying of data. Due to the asynchronous nature of the library, these objects should be allocated on the heap and deleted in the user defined callback functions. 
Each callback is called in its own thread in order to keep the networking threads free for networking operations.
This makes Kyukon ideal for parallel webcrawlers modeled as an FSM where each callback corresponds to a different state or type-of-webpage.

Hello Kitty
======

Below is a simple examples of how the can be used. This small example makes a Bing images search for Hello Kitty, and downloads the first page of thumbnails as files.

	#include "kyukon.hpp"
	#include "task.hpp"

	//Globals
	bool keep_alive = true;
	int expected = 0;

	//Prototypes
	void initial_callback(task*);
	void final_callback(task*);

	int main() {

		//Initialse the library with 2 threads.
		kyukon::init(2);

		//Create a new task.
		task *t = new task();

		//Set the url to grab.
		t->set_url("http://www.bing.com/images/search?q=hello+kitty");

		//Set the referer (some sites check this to avoid hotlinking)
		t->set_ref("http://www.bing.com/images");

		//Tell the library that we want this html stored as an std::string.
		t->set_target_string();

		//Tell the library which function to call when it has grabbed the requested page.
		t->set_callback(&initial_callback);

		//Add the task to the queue for processing.
		kyukon::add_task(t);

		//Avoid main returning until we have what we came for.
		while(keep_alive);
	}

	void initial_callback(task *t) {

		//Data is the html we have just grabbed.
		const std::string tmp = t->get_data(),
				  ref = t->get_url();

		//We don't need this anymore.
		delete t;

		size_t st = 0, en = 0;

		//TODO: Replace this block with std::regex once all mainstream compilers 
		//and runtime libraries have stable support.
		for(size_t pos = 0;;pos = st + en) {

			//Parse out the URLs for the image thumbnails from the HTML source.
			pos = tmp.find("bing.net/th?id=H", pos);

			if (pos == std::string::npos)
				break;

			st = tmp.rfind("\"", pos) + 1;
			en = tmp.find("\"", st) - st;

			task *t = new task();
			t->set_url(tmp.substr(st, en));
			t->set_ref(ref);

			//This time we want the result in the form of a file.
			//At the moment Kyukon decides the filename based on the url.
			t->set_target_file();

			t->set_callback(&final_callback);

			kyukon::add_task(t);

			//Increment the number of items we expect to be returned.
			expected++;
		}
	}

	void final_callback(task *t) {

		static int count = 0;

		delete t;

		//Once count reaches expected we know we have everything we came for,
		//tell main to exit by setiing keep_alive to false.
		keep_alive = !(++count == expected);	
	}

This example could be expanded by moving the search-page logic out of main and having final callback invoke it with an incremented page number, allowing it to crawl past the first page of results.
