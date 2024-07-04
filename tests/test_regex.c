#include "testlib/asserts.h"
#include "testlib/tests.h"

#include "regex.h"



Test tests[] = {
    {.name=NULL, .func=NULL},
};

int main(int argc, char* argv[]) {
    return default_main(&argv[1], argc - 1);
}
