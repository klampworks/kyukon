#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include "../kyukon.hpp"
#include "../task.hpp"
#include <iostream>
#include <fstream>

std::vector<std::string> parse_index(void);
void process_index(task*);
volatile bool keep_alive = true;

BOOST_AUTO_TEST_CASE( free_test_function ) {

	std::vector<std::pair<std::string, bool>> p = {
		{"", false},
		//{"", false}
	};
	kyukon::init(p);

	kyukon::signup(1, domain_settings());
	kyukon::set_do_fillup(1, false);


	task *t = new task();
	t->set_url("127.0.0.1/index.html");
	t->set_target_string();
	t->set_callback(&process_index);
	kyukon::add_task(t, 1);

	while(keep_alive);

	BOOST_CHECK( true /* test assertion */ );
}

void process_index(task *t) {

	keep_alive = false;
	kyukon::stop();
	return;

	std::vector<std::string> l = parse_index();
	const char *path = "/var/www/localhost/htdocs/dl/";

	for (const auto &s : l) {

		std::string togo(std::string(path) + s);
		std::cout << togo << std::endl;
	}

	std::string man(std::string(path) + "manifest");
	std::cout << man << std::endl;
}

std::vector<std::string> parse_index() {

	const char *filename = "/var/www/localhost/htdocs/index.html";

	std::vector<std::string> ret;

	std::ifstream ifs(filename);
	std::string tmp;

	while(std::getline(ifs, tmp)) {
		
		ret.push_back(tmp);
	}

	//Copy ellision.
	return std::move(ret);
}
