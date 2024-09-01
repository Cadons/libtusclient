#include "libtusclient.h"
#include <vector>
#include <string>

int main() {
    libtusclient();

    std::vector<std::string> vec;
    vec.push_back("test_package");

    libtusclient_print_vector(vec);
}
