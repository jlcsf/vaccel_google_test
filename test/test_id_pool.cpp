#include <gtest/gtest.h>
// #include <fff.h>

// DEFINE_FFF_GLOBALS;

extern "C" {
#include "id_pool.h"
#include "error.h"
}

TEST(IdPoolTest, IdPoolNew) {
    id_pool_t test_pool;

    EXPECT_EQ(id_pool_new(&test_pool, 10), VACCEL_OK);
    EXPECT_EQ(id_pool_new(&test_pool, 0), VACCEL_EINVAL);
    // You can't check for VACCEL_ENOMEM in the same way as in Catch2
}

TEST(IdPoolTest, IdPoolDestroy) {
    id_pool_t test_pool;
    EXPECT_EQ(id_pool_new(&test_pool, 10), VACCEL_OK);
    EXPECT_EQ(id_pool_destroy(&test_pool), VACCEL_OK);
    EXPECT_EQ(id_pool_destroy(NULL), VACCEL_EINVAL);
}

TEST(IdPoolTest, IdPoolGet) {
    id_pool_t test_pool;
    id_pool_new(&test_pool, 3);

    vaccel_id_t id_test = id_pool_get(&test_pool);
    EXPECT_EQ(id_test, 1);

    id_test = id_pool_get(&test_pool);
    EXPECT_EQ(id_test, 2);

    id_test = id_pool_get(&test_pool);
    EXPECT_EQ(id_test, 3);

    id_test = id_pool_get(&test_pool);
    EXPECT_EQ(id_test, 0);

    // No pool to get ids from
    EXPECT_EQ(id_pool_get(NULL), 0);
}

TEST(IdPoolTest, IdPoolRelease) {
    id_pool_t test_pool;
    id_pool_new(&test_pool, 3);

    vaccel_id_t id_test = id_pool_get(&test_pool);
    EXPECT_EQ(id_test, 1);

    id_pool_get(&test_pool);

    // Release id 1 back into the pool
    id_pool_release(&test_pool, 1);

    // Get id 1 back
    id_test = id_pool_get(&test_pool);
    EXPECT_EQ(id_test, 1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
