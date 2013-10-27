#pragma once

class task;

namespace kyukon {

	void init(unsigned threads);
	void add_task(task *t);

}

