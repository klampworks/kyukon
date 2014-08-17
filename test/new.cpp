#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include "../qscheduler.hpp"
#include "../task.hpp"
#include "test_nh_table.hpp"

BOOST_AUTO_TEST_CASE(test_reg_domain) 
{
	qscheduler qs;

	dom_id dom1 = qs.reg_dom(0, nullptr);
	BOOST_CHECK(dom1 != 0);

	dom_id dom2 = qs.reg_dom(0, nullptr);
	BOOST_CHECK(dom2 != dom1 && dom2 != 0);
}

