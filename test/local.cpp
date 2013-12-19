#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include "../kyukon.hpp"
#include "../task.hpp"

BOOST_AUTO_TEST_CASE( free_test_function ) {

	BOOST_CHECK( true /* test assertion */ );
}
