#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "resources.h"
#include "id_pool.h"
#include "error.h"
#include "list.h"
#include "log.h"
#include "utils.h"
#include "vaccel.h"
#include "plugin.h"
  
#ifdef __cplusplus
}
#endif

TEST(vaccel_resource, resources_bootstrap)
{
    int ret = resources_bootstrap();
    EXPECT_EQ(ret, VACCEL_OK);
    EXPECT_EQ(1, 1);
}
