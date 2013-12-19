#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include "../kyukon.hpp"
#include "../task.hpp"

BOOST_AUTO_TEST_CASE( free_test_function ) {

	prase_index();
	BOOST_CHECK( true /* test assertion */ );
}

std::vector<std::string> parse_index() {

	const char *filename = "/var/www/localhost/htdocs/index/html";

	std::vector<std::string> ret;

	std::ifstream ifs(filename);
	std::string tmp;

	while(std::getline(ifs, tmp)) {
		
		ret.push_back(tmp);
	}

	//Copy ellision.
	return std::move(ret);
}
