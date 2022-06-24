#define BOOST_TEST_MODULE thread_pool_test
#include <boost/test/included/unit_test.hpp>
#include "myPool.h"
#include <memory.h>
using namespace std;

int count = 0;

void testFun(void* arg){
    ::count++;
    printf("i = %d\n", *(int *)arg);
}

BOOST_AUTO_TEST_SUITE(thread_pool_test)

BOOST_AUTO_TEST_CASE(test_multiRun)
{
    ::count = 0;
    ThreadPool *poolm = new ThreadPool(1000, 2000);
    printf("线程池初始化成功\n");
    int i = 0;
    for (i = 0; i < 100; ++i) {
        poolm->pushJob(testFun, &i, sizeof(int));
    }
    sleep(1);
    cout<<::count;
    BOOST_CHECK(::count == 100);
    free(poolm);
}


BOOST_AUTO_TEST_SUITE_END()