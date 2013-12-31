#include "task.hpp"

task::task(unsigned domain_id) {
	this->domain_id = domain_id;
}

task::task() {}

task::task(unsigned domain_id, const std::string &url_p, const std::string &ref_p, 
	task_target target_p, const std::function<void(task*)> &callback_p)
{
	
	this->domain_id = domain_id;
	set_url(url_p);
	set_ref(ref_p);
	set_callback(callback_p);
	target = target_p;
}


task::task(unsigned domain_id, std::string &&url_p, std::string &&ref_p, 
	task_target target_p, std::function<void(task*)> &&callback_p)
{

	this->domain_id = domain_id;
	std::swap(url, url_p);
	std::swap(ref, ref_p);
	std::swap(callback, callback_p);
	target = target_p;
}

void task::prepare_result(std::string &&data, long &&status_code, 
			double &&data_size) {

	set_data(std::move(data));
	set_status_code(std::move(status_code));
	set_data_size(std::move(data_size));
}

void task::set_url(const std::string &url) {
	this->url = url;
}

void task::set_ref(const std::string &ref) {
	this->ref = ref;
}

void task::set_data(std::string &&data) {
	std::swap(this->data, data);
}

void task::set_status_code(long &&status_code) {
	std::swap(this->status_code, status_code);
}

void task::set_data_size(double &&data_size) {
	std::swap(this->data_size, data_size);
}

void task::set_callback(const std::function<void(task*)> &callback) {
	this->callback = callback;
}

void task::set_target_string() {
	target = STRING;
}

void task::set_target_file() {
	target = FILE;
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

unsigned task::inc_retries() {
	return ++retries;
}

void task::set_curl_result(std::string &&res) {
	curl_result = res;
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

task::task_target task::get_target() const {
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

std::string task::get_curl_result() const {
	return curl_result;
}
