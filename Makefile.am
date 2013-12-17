AM_CXXFLAGS=-std=c++11
lib_LTLIBRARIES = libkyukon.la
libkyukon_la_SOURCES = task.cpp kon.cpp kyukon.cpp 
libkyukon_ladir = '$(includedir)/kyukon'
libkyukon_la_HEADERS = task.hpp kon.hpp kyukon.hpp
libkyukon_la_LDFLAGS = -fPIC -rpath /usr/local/lib -shared -lcurl

bin_PROGRAMS = unit_test
unit_test_SOURCES = test/main.cpp
unit_test_LDADD = libkyukon.la
