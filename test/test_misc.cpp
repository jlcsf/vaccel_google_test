#include <gtest/gtest.h>


extern "C" {
#include "misc.h"
#include "error.h"
#include "plugin.h"
#include "log.h"

#include "session.h"
}

TEST(VaccelGetPlugins, VaccelGetPlugins) {

    // int result = vaccel_get_plugins(&session, VACCEL_NO_OP);
    // EXPECT_EQ(result, VACCEL_OK);

    EXPECT_EQ(vaccel_get_plugins(NULL, VACCEL_NO_OP), VACCEL_EINVAL);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
