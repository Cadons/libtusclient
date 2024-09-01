#include <gtest/gtest.h>
#include <iostream>

#include "libtusclient.h"

TEST(libtusclient, test) {
    libtusclient();
    std::vector<std::string> strings;
    strings.push_back("Hello");
    strings.push_back("World");
    libtusclient_print_vector(strings);
}

