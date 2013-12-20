#include "task.hpp"

task::task(unsigned domain_id) {
	this->domain_id = domain_id;
}

task::task() {}

task::task(const std::string &url_p, const std::string &ref, 
	task_target target, const std::function<void(task*)> &callback)
{
	
	set_url(url);
	set_ref(ref);
	set_callback(callback);

	if (target == STRING)
		set_target_string();
	else 
		set_target_file();
	
}

void task::prepare_task(const std::string &url, const std::string &ref, 
			const std::function<void(task*)> &callback) {

	set_url(url);
	set_ref(ref);
	set_callback(callback);
}

void task::prepare_result(const std::string &data, const long &status_code, 
			const double &data_size) {

	set_data(data);
	set_status_code(status_code);
	set_data_size(data_size);
}

void task::set_url(const std::string &url) {
	this->url = url;
}

void task::set_ref(const std::string &ref) {
	this->ref = ref;
}

void task::set_data(const std::string &data) {
	this->data = data;
}

void task::set_status_code(const long &status_code) {
	this->status_code = status_code;
}

void task::set_data_size(const double &data_size) {
	this->data_size = data_size;
}

void task::set_callback(const std::function<void(task*)> &callback) {
	this->callback = callback;
}

void task::set_target_string() {
	target = 0;
}

void task::set_target_file() {
	target = 1;
}

void task::set_priority(const int &priority) {
	this->priority = priority;
}

void task::inc_priority() {
	priority++;
}

void task::set_filepath(const std::string &filepath) {
	this->filepath =filepath;
}

void task::set_max_retries(unsigned max_retries) {
	this->max_retries = max_retries;
}

bool task::inc_retries() {
	return ++retries == max_retries;
}

std::string task::get_url() const {
	return url;
}

std::string task::get_ref() const {
	return ref;
}

std::string task::get_data() const {
	return data;
}

long task::get_status_code() const {
	return status_code;
}

double task::get_data_size() const {
	return data_size;
}

std::function<void(task*)> task::get_callback() const {
	return callback;
}

int task::get_target() const {
	return target;
}

int task::get_priority() const {
	return priority;
}

const std::string task::get_filepath() const {
	return filepath;
}

unsigned task::get_retries() const {
	return retries;
}

unsigned task::get_domain_id() const {
	return domain_id;
}
