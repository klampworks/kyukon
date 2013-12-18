#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include "../kyukon.hpp"
#include "../task.hpp"

//Globals
bool keep_alive = true;
int expected = 0;

//Prototypes
void initial_callback(task*);
void final_callback(task*);

BOOST_AUTO_TEST_CASE( free_test_function ) {

    //Initialse the library with 2 threads.
    kyukon::init(2);

    //Create a new task.
    task *t = new task();

    //Set the url to grab.
    t->set_url("http://www.bing.com/images/search?q=hello+kitty");

    //Set the referer (some sites check this to avoid hotlinking)
    t->set_ref("http://www.bing.com/images");

    //Tell the library that we want this html stored as an std::string.
    t->set_target_string();

    //Tell the library which function to call when it has grabbed the requested page.
    t->set_callback(&initial_callback);

    //Add the task to the queue for processing.
    kyukon::add_task(t);

    //Avoid main returning until we have what we came for.
    while(keep_alive);

    BOOST_CHECK( true /* test assertion */ );
}

void initial_callback(task *t) {

    //Data is the html we have just grabbed.
    const std::string tmp = t->get_data(),
              ref = t->get_url();

    //We don't need this anymore.
    delete t;

    size_t st = 0, en = 0;

    //TODO: Replace this block with std::regex once all mainstream compilers 
    //and runtime libraries have stable support.
    for(size_t pos = 0;;pos = st + en) {

        //Parse out the URLs for the image thumbnails from the HTML source.
        pos = tmp.find("bing.net/th?id=H", pos);

        if (pos == std::string::npos)
            break;

        st = tmp.rfind("\"", pos) + 1;
        en = tmp.find("\"", st) - st;

        task *t = new task();
        t->set_url(tmp.substr(st, en));
        t->set_ref(ref);

        //This time we want the result in the form of a file.
        //At the moment Kyukon decides the filename based on the url.
        t->set_target_file();

        t->set_callback(&final_callback);

        kyukon::add_task(t);

        //Increment the number of items we expect to be returned.
        expected++;
    }
}

void final_callback(task *t) {

    static int count = 0;

    delete t;

    //Once count reaches expected we know we have everything we came for,
    //tell main to exit by setiing keep_alive to false.
    keep_alive = !(++count == expected);    
}


