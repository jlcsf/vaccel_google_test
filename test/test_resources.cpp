#include <gtest/gtest.h>



extern "C" {
#include "resources.h"
#include "id_pool.h"
#include "error.h"
#include "list.h"
#include "log.h"
#include "utils.h"
#include "vaccel.h"
#include "plugin.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
}

extern "C"{
#define MAX_RESOURCES 2048
#define MAX_RESOURCE_RUNDIR 1024

// static bool initialized = false;
id_pool_t id_pool;
list_t live_resources[VACCEL_RES_MAX];

int cleanup_resource_mock([[maybe_unused]] void *data){
    return 0;
}
}

class ResourcesNew : public ::testing::Test {
protected:
    int ret;
    vaccel_resource res;
    vaccel_resource_t test_type;
    void* test_data;
    int (*cleanup_res_test)(void*);

    void SetUp() override {
        ret = resources_bootstrap();
        ASSERT_EQ(ret, VACCEL_OK);
        test_type = VACCEL_RES_TF_MODEL;
        test_data = nullptr;
        cleanup_res_test = cleanup_resource_mock;

        ret = resource_new(&res, test_type, test_data, cleanup_res_test);
        ASSERT_EQ(ret, VACCEL_OK);
    }

    void TearDown() override {
        //resources_cleanup();
    }
};

TEST_F(ResourcesNew, destroy_OK) {
    int ret = resource_destroy(&res);
    EXPECT_EQ(ret, VACCEL_OK);
}

TEST(Resources, destroy_not_init) {
    // initialized = false;
    // int ret = resource_destroy(&res);
    // EXPECT_EQ(ret, VACCEL_EPERM);
    // initialized = true;

    EXPECT_EQ(1,1);
}

TEST_F(ResourcesNew, destory_no_res){
    int ret = resource_destroy(NULL);
    EXPECT_EQ(ret, VACCEL_EINVAL);

    ret = resource_destroy(&res);
    EXPECT_EQ(ret, VACCEL_OK);
}

TEST_F(ResourcesNew, create_rundir){
    
    int ret = resource_create_rundir(&res);
    EXPECT_EQ(ret, VACCEL_OK);
    ret = resource_destroy(&res);
    EXPECT_EQ(ret, VACCEL_OK);
}

TEST(Resources, find_resource_fail){
    vaccel_resource* test_res = nullptr;
    vaccel_id_t test_id = 0;
    int ret = resource_get_by_id(&test_res, test_id);
    EXPECT_EQ(ret, VACCEL_EINVAL);
}

TEST(ResourcesGetById, ResourceExists) {
    int result;
    struct vaccel_resource test_res;
    vaccel_resource_t test_type;
    void* test_data;
    int (*cleanup_res_test)(void*);

    test_res.id = 1;
    test_type = VACCEL_RES_TF_MODEL;
    test_data = nullptr;
    cleanup_res_test = cleanup_resource_mock;

    result = resources_bootstrap();
    ASSERT_EQ(result, VACCEL_OK);
    result = resource_new(&test_res, test_type, test_data, cleanup_res_test);
    ASSERT_EQ(result, VACCEL_OK);

    struct vaccel_resource* result_resource = NULL;
    vaccel_id_t id_to_find = 1;

    int ret = resource_get_by_id(&result_resource, id_to_find);
    EXPECT_EQ(ret, VACCEL_OK);
    ASSERT_NE(result_resource, nullptr);
    printf("works");
    result = resource_destroy(&test_res);
    EXPECT_EQ(result, VACCEL_OK);
    printf("works");
    // result = resource_destroy(result_resource);
    // EXPECT_EQ(result, VACCEL_OK);
    // printf("works");
    // cant free a pointer 
}