#include <gtest/gtest.h>
#include "error.h"
#include "log.h"

extern "C" {
#include <stdlib.h>
#include <string.h>
#include <slog.h>
}

TEST(LogLevelAndLogFile, Log) {
    char env_var[] = "VACCEL_DEBUG_LEVEL=4";
    putenv(env_var);
    int ret = vaccel_log_init();
    EXPECT_EQ(ret, VACCEL_OK);
    ret = vaccel_log_shutdown();
    EXPECT_EQ(ret, VACCEL_OK);
}

// for different log levels:

TEST(LogLevel1, Log) {
    char env_var[] = "VACCEL_DEBUG_LEVEL=1";
    putenv(env_var);
    int ret = vaccel_log_init();
    EXPECT_EQ(ret, VACCEL_OK);
    ret = vaccel_log_shutdown();
    EXPECT_EQ(ret, VACCEL_OK);
}

TEST(LogLevel2, Log) {
    char env_var[] = "VACCEL_DEBUG_LEVEL=2";
    putenv(env_var);
    int ret = vaccel_log_init();
    EXPECT_EQ(ret, VACCEL_OK);
    ret = vaccel_log_shutdown();
    EXPECT_EQ(ret, VACCEL_OK);
}

TEST(LogLevel3, Log) {
    char env_var[] = "VACCEL_DEBUG_LEVEL=3";
    putenv(env_var);
    int ret = vaccel_log_init();
    EXPECT_EQ(ret, VACCEL_OK);
    ret = vaccel_log_shutdown();
    EXPECT_EQ(ret, VACCEL_OK);
}

TEST(LogLevel4, Log) {
    char env_var[] = "VACCEL_DEBUG_LEVEL=4";
    putenv(env_var);
    int ret = vaccel_log_init();
    EXPECT_EQ(ret, VACCEL_OK);
    ret = vaccel_log_shutdown();
    EXPECT_EQ(ret, VACCEL_OK);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
