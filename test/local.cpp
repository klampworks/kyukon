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

//Prevent the while loops from optimising itself out.
volatile bool keep_alive = true;
volatile int count = 0;

BOOST_AUTO_TEST_CASE( free_test_function ) {

	std::vector<std::pair<std::string, bool>> p = {
		{"", false},
		{"", false}
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

void process_index(task *tt) {

	std::vector<std::string> l = parse_index(tt->get_data());
	delete tt;

	const char *path = "127.0.0.1/dl/";

	for (const auto &s : l) {

		std::string togo(std::string(path) + s);

		task *t = new task();
		t->set_url(togo);
		t->set_target_file();
		t->set_callback(&items_callback);
		kyukon::add_task(t, 1);
	}

	std::string man(std::string(path) + "manifest");

	task *t = new task();
	t->set_url(man);
	t->set_target_file();
	t->set_callback(&man_callback);
	kyukon::add_task(t, 1);

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
