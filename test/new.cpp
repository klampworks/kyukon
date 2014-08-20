#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include "../qscheduler.hpp"
#define protected public
#include "../task.hpp"
#include "test_nh_table.hpp"
#include "../domain_settings.hpp"

/* Registering domains should return unique dom_ids. */
BOOST_AUTO_TEST_CASE(test_reg_domain_unique) 
{
	qscheduler qs;

	dom_id dom1 = qs.reg_dom(0, nullptr);
	BOOST_CHECK(dom1 != 0);

	dom_id dom2 = qs.reg_dom(0, nullptr);
	BOOST_CHECK(dom2 != dom1 && dom2 != 0);
}

/*
 * Adding a task should add it to the task_list for the right domain.
 * The task list should be empty initially.
 * After adding the task, the task list size should reflect the new task.
 * The task in the list should be the same task we added.
 */
BOOST_AUTO_TEST_CASE(test_qs_add_task) 
{
	qscheduler qs;
	dom_id dom1 = qs.reg_dom(0, nullptr);

	task *t = new task();
	t->domain_id = dom1;

	BOOST_CHECK(qs.domains.at(dom1)->task_list.empty());

	qs.add_task(t);
	BOOST_CHECK(!qs.domains.at(dom1)->task_list.empty());
	BOOST_CHECK(qs.domains.at(dom1)->task_list.top() == t);
	delete t;
}

/*
 * Adding a thread should add it to the next_hit table.
 */
BOOST_AUTO_TEST_CASE(test_qs_add_thread) 
{
	qscheduler qs;
	thread_id thread = 2501;
	qs.reg_thread(thread);
	BOOST_CHECK(qs.next_hit.thread_ids.back() == thread);
}
	


