#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include "../kyukon.hpp"
#include "../task.hpp"
#include <iostream>
#include <fstream>

std::vector<std::string> parse_index(void);

BOOST_AUTO_TEST_CASE( free_test_function ) {

	std::vector<std::string> l = parse_index();
	const char *path = "/var/www/localhost/htdocs/dl/";

	for (const auto &s : l) {

		std::string togo(std::string(path) + s);
		std::cout << togo << std::endl;
	}

	std::string man(std::string(path) + "manifest");
	std::cout << man << std::endl;

	BOOST_CHECK( !l.empty() /* test assertion */ );
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
