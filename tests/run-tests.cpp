extern "C" {
#include <CuTest.h>
}

#include <cstdio>
#include <cstdint>
#include <zhvm.h>

void TestGetSetRegisters(CuTest* tc) {
    zhvm::memory mem;

    int64_t reg[16];
    for (uint32_t i = 0; i < 16; ++i) {
        reg[i] = rand();
    }

    for (uint32_t i = 0; i < 16; ++i) {
        mem.Set(i, reg[i]);
    }

    CuAssertIntEquals(tc, 0, mem.Get(0));

    CuAssertIntEquals(tc, reg[1], mem.Get(1));
    CuAssertIntEquals(tc, reg[2], mem.Get(2));
    CuAssertIntEquals(tc, reg[3], mem.Get(3));
    CuAssertIntEquals(tc, reg[4], mem.Get(4));
    CuAssertIntEquals(tc, reg[5], mem.Get(5));
    CuAssertIntEquals(tc, reg[6], mem.Get(6));
    CuAssertIntEquals(tc, reg[7], mem.Get(7));
    CuAssertIntEquals(tc, reg[8], mem.Get(8));
    CuAssertIntEquals(tc, reg[9], mem.Get(9));
    CuAssertIntEquals(tc, reg[10], mem.Get(10));
    CuAssertIntEquals(tc, reg[11], mem.Get(11));
    CuAssertIntEquals(tc, reg[12], mem.Get(12));
    CuAssertIntEquals(tc, reg[13], mem.Get(13));
    CuAssertIntEquals(tc, reg[14], mem.Get(14));
    CuAssertIntEquals(tc, reg[15], mem.Get(15));
}

void TestGetSetMemory(CuTest* tc) {
    zhvm::memory mem;
    const size_t memsz = 1024;
    mem.NewImage(memsz);

    int8_t bt = rand() % 0xFF;
    int16_t sh = rand() % 0xFFFF;
    int32_t lg = rand() % 0xFFFFFFFF;
    int64_t qd = rand() % 0xFFFFFFFFFFFFFFFF;

    off_t bto = rand() % (memsz - sizeof (int8_t));
    off_t sho = rand() % (memsz - sizeof (int16_t));
    off_t lgo = rand() % (memsz - sizeof (int32_t));
    off_t qdo = rand() % (memsz - sizeof (int64_t));

    mem.SetByte(bto, bt);
    mem.SetShort(sho, sh);
    mem.SetLong(lgo, lg);
    mem.SetQuad(qdo, qd);
    
    CuAssertIntEquals(tc, qd, mem.GetQuad(qdo));
    CuAssertIntEquals(tc, bt, mem.GetByte(bto));
    CuAssertIntEquals(tc, sh, mem.GetShort(sho));
    CuAssertIntEquals(tc, lg, mem.GetLong(lgo));
}

CuSuite* RegisterTests() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, TestGetSetRegisters);
    SUITE_ADD_TEST(suite, TestGetSetMemory);
    return suite;
}

int main(int argc, char* argv[]) {
    srand(time(0));

    CuString* output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, RegisterTests());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);

    int result = suite->failCount;

    fprintf(stderr, "%s\n", output->buffer);

    CuSuiteDelete(suite);
    CuStringDelete(output);

    return result;
}