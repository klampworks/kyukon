#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include "../qscheduler.hpp"
#include "../task.hpp"

BOOST_AUTO_TEST_CASE(test_reg_domain) 
{
	qscheduler qs;

	dom_id dom1 = qs.reg_dom(0, nullptr);
	BOOST_CHECK(dom1 != 0);

	dom_id dom2 = qs.reg_dom(0, nullptr);
	BOOST_CHECK(dom2 != dom1 && dom2 != 0);
}

BOOST_AUTO_TEST_CASE(test_nh_table) 
{
	nh_table nh;
	BOOST_CHECK(nh.dom_ids.empty());
	BOOST_CHECK(nh.thread_ids.empty());
	BOOST_CHECK(nh.next_hit.empty());

	dom_id dom1 = 1;
	thread_id thread1 = 2;

	nh.add_dom(dom1);
	BOOST_CHECK(nh.dom_ids.size() == 1);
	BOOST_CHECK(nh.thread_ids.empty());
	BOOST_CHECK(nh.next_hit.empty());

	nh.add_thread(thread1);
	BOOST_CHECK(nh.dom_ids.size() == 1);
	BOOST_CHECK(nh.thread_ids.size() == 1);
	BOOST_CHECK(nh.next_hit.size() == 1);

	long res = nh.next_hit[std::pair<dom_id, thread_id>(dom1, thread1)];
	BOOST_CHECK(res == 0);
}
