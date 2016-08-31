extern "C" {
#include <CuTest.h>
}

#include <cstdio>

void TestNothing(CuTest* tc) {
    CuAssert(tc, "Error", 0);
    
    
}

CuSuite* RegisterTests() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, TestNothing);
    return suite;
}

int main(int argc, char* argv[]) {

    CuString* output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, RegisterTests());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);

    printf("%s\n", output->buffer);

    CuSuiteDelete(suite);
    CuStringDelete(output);
    return 0;
}