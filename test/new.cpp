#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#define protected public
#define private public
#include "../qscheduler.hpp"
#include "../task.hpp"
#include "../domain_settings.hpp"
#include "test_nh_table.hpp"
#include "test_qscheduler.hpp"
#include "test_tscheduler.hpp"
#include "test_util.hpp"
