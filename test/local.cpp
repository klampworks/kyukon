#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include "../kyukon.hpp"
#include "../task.hpp"
#include <iostream>
//#include <sstream>
//#include <thread>
//#include <chrono>
#include <cstdlib>

struct local_fix {

	~local_fix() {
		kyukon::stop();
	}
};

BOOST_FIXTURE_TEST_CASE(check_connection, local_fix) {

	const std::string addr(getenv("KYUKON_TEST_ADDR"));
	BOOST_REQUIRE(!addr.empty());

	std::vector<std::pair<std::string, bool>> p = {
		{"", false},
		{"", false}
	};

	kyukon::init(p);

	unsigned domain_id = kyukon::signup(0, nullptr);
	kyukon::set_do_fillup(false, domain_id);

	volatile bool done = false;

	task *t = new task(domain_id, addr+"/test", "", 
		task::STRING, [&done](task *tt) { 

			if (tt->get_data().empty()) {
				std::cout << "No content recieved, are you sure "
				"the webserver is running?" << std::endl;
				BOOST_REQUIRE(false);
			}

			kyukon::stop();
			done = true;});

	kyukon::add_task(t);

	while(!done);

	std::cout << "...we seem to have a connection to the test server.\n" << std::endl;

}

