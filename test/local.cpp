#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include "../kyukon.hpp"
#include "../task.hpp"
#include <iostream>
#include <sstream>

std::vector<std::string> parse_index(const std::string &);
void process_index(task*);
void items_callback(task*);
void man_callback(task*);
unsigned domain_id = 0;

//Prevent the while loops from optimising itself out.
volatile bool keep_alive = true;
volatile int count = 0;

BOOST_AUTO_TEST_CASE(check_connection) {

	std::cout << "Checking connection..." << std::endl;

	std::vector<std::pair<std::string, bool>> p = {
		{"", false},
		{"", false}
	};
	kyukon::init(p);

	domain_id = kyukon::signup(0, nullptr);
	kyukon::set_do_fillup(domain_id, false);

	volatile bool done = false;

	task *t = new task(domain_id, "192.168.100.136/index.html", "", 
		task::STRING, [&done](task *tt){ 

			if (tt->get_data().empty()) {
				std::cout << "No content recieved, are you sure "
				"the webserver is running?" << std::endl;
				BOOST_REQUIRE(false);
			}

			kyukon::stop();
			done = true;});

	kyukon::add_task(t);

	while(!done);

	std::cout << "...we seem to have a connection to the test server." << std::endl;
}

BOOST_AUTO_TEST_CASE( free_test_function ) {

	std::vector<std::pair<std::string, bool>> p = {
		{"", false},
		{"", false}
	};
	kyukon::init(p);

	domain_id = kyukon::signup(0, nullptr);
	kyukon::set_do_fillup(domain_id, false);

	task *t = new task(domain_id, "192.168.100.136/index.html", "", 
		task::STRING, &process_index);

	kyukon::add_task(t);

	while(keep_alive);

	std::cout << "Validating checksums..." << std::endl;
	//Inverted because 0 means no errors for exit codes.
	BOOST_CHECK(!system("md5sum -c manifest"));
}

void process_index(task *tt) {

	if (tt->get_data().empty()) {
		std::cout << "No content recieved, are you sure "
		"the webserver is running?" << std::endl;
		BOOST_REQUIRE(false);
	}

	std::vector<std::string> l = parse_index(tt->get_data());
	delete tt;

	const char *path = "192.168.100.136/dl/";

	for (const auto &s : l) {

		std::string togo(std::string(path) + s);

		task *t = new task(domain_id, std::move(togo), "", task::FILE, &items_callback);
		kyukon::add_task(t);
	}

	std::string man(std::string(path) + "manifest");

	task *t = new task(domain_id, std::move(man), "", task::FILE, &man_callback);
	kyukon::add_task(t);

}

std::vector<std::string> parse_index(const std::string &data) {

	std::vector<std::string> ret;

	std::stringstream ss(data);
	std::string tmp;

	while(std::getline(ss, tmp)) {
		
		ret.push_back(tmp);
	}

	//Copy ellision.
	return std::move(ret);
}

void man_callback(task *t) {

	while(count < 10);
	keep_alive = false;
	kyukon::stop();
}

void items_callback(task *t) {

	count++;
	delete t;
}
