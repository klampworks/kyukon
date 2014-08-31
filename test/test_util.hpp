#pragma once
#include <chrono>

std::chrono::duration<long> pone(const tscheduler &ts)
{
	return ts.resolve_t_to + std::chrono::seconds(1);
}
