#ifndef TEST_H
#define TEST_H
#ifdef TESTING
bool test();
#include "test_declare.h" // define TEST() macro
#include "test_include.h" // include t_*.h
#include "test_undefine.h"
#endif
#endif
