
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

	dom_id dom2 = 2;
	thread_id thread2 = 3;

	nh.add_dom(dom2);
	BOOST_CHECK(nh.dom_ids.size() == 2);
	BOOST_CHECK(nh.thread_ids.size() == 1);
	BOOST_CHECK(nh.next_hit.size() == 2);

	nh.add_thread(thread2);
	BOOST_CHECK(nh.dom_ids.size() == 2);
	BOOST_CHECK(nh.thread_ids.size() == 2);
	BOOST_CHECK(nh.next_hit.size() == 4);

	BOOST_CHECK(nh.next(dom1, thread1) == 0);
}

/*
 * When we add a new domain, thread pair, next_hit should be 0 (1/1/1970).
 * When we call update, next_hit should be present time + interval.
 */
BOOST_AUTO_TEST_CASE(test_nh_table_update) 
{
	nh_table nh;
	nh.add_dom(1);
	nh.add_thread(1);

	BOOST_CHECK(nh.next(1, 1) == 0);
	nh.update(1, 1, 0);
	BOOST_CHECK(nh.next(1, 1) != 0);
}

/*
 * nh_table::next(thread_id) should return the next availble dom_id for a 
 * given thread.
 *
 * If we add two domains, update dom1 with a 100 second interval and 
 * update dom2 with a 0 second interval, calling next should return dom2.
 *
 * Additionally, if both domains are updated with a high interval and then
 * next is called before that interval, 0 (invalid domain) should be returned.
 */
BOOST_AUTO_TEST_CASE(test_nh_table_next) 
{
	nh_table nh;
	dom_id dom1 = 1;
	dom_id dom2 = 2;
	thread_id thread1 = 1;

	nh.add_dom(dom1);
	nh.add_dom(dom2);
	nh.add_thread(thread1);

	nh.update(dom1, thread1, 100);
	nh.update(dom2, thread1, 0);
	BOOST_CHECK(nh.next(thread1) == dom2);

	nh.update(dom2, thread1, 100);
	BOOST_CHECK(nh.next(thread1) == 0);
}
