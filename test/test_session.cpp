#include <gtest/gtest.h>

#include <gtest/gtest.h>
#include "fff.h"
#include <atomic>

DEFINE_FFF_GLOBALS;

extern "C" {
#include "session.h"
#include "plugin.h"
#include "log.h"
#include "utils.h"
#include "id_pool.h"

FAKE_VALUE_FUNC(struct vaccel_plugin*, get_virtio_plugin);
FAKE_VALUE_FUNC(struct vaccel_session*, sess_free);
}

int mock_sess_init(vaccel_session* sess, uint32_t flags) {
    (void)sess;
    (void)flags;
    return 0;
}

int mock_sess_update(vaccel_session* sess, uint32_t flags) {
    (void)sess;
    (void)flags;
    return 0;
}

int mock_sess_free(vaccel_session* sess) {
    (void)sess;
    return 0;
}

int mock_sess_register(uint32_t sess_id, vaccel_id_t resource_id){
    (void) sess_id;
    (void) resource_id;
    return 0;
}

int mock_sess_unregister(uint32_t sess_id, vaccel_id_t resource_id){
    (void) sess_id;
    (void) resource_id;
    return 0;
}

class SessionTest : public ::testing::Test {
protected:
    void SetUp() override {
        sessions_bootstrap();
    }

    void TearDown() {
        sessions_cleanup();
        
        sessions_bootstrap(); // gtest doing double frees... temp fix I guess.
    }
};

TEST_F(SessionTest, sess_init) {
    struct vaccel_session sess;
    int result = vaccel_sess_init(&sess, 1);
    EXPECT_EQ(result, VACCEL_OK);
}

TEST_F(SessionTest, sess_null) {
    int result = vaccel_sess_init(NULL, 1);
    EXPECT_EQ(result, VACCEL_EINVAL);
}


TEST_F(SessionTest, sess_update) {
    struct vaccel_session sess;
    int result = vaccel_sess_init(&sess, 1);
    ASSERT_EQ(result, VACCEL_OK);

    result = vaccel_sess_update(&sess, 2);
    EXPECT_EQ(result, VACCEL_OK);

    result = vaccel_sess_update(NULL, 2);
    EXPECT_EQ(result, VACCEL_EINVAL);
}



TEST_F(SessionTest, sess_free) {
    struct vaccel_session sess;
    int result = vaccel_sess_init(&sess, 1);
    ASSERT_EQ(result, VACCEL_OK);

    result = vaccel_sess_free(&sess);
    EXPECT_EQ(result, VACCEL_OK);

    result = vaccel_sess_free(NULL);
    EXPECT_EQ(result, VACCEL_EINVAL);
}

TEST_F(SessionTest, sess_register) {
    struct vaccel_session sess;
    int result = vaccel_sess_init(&sess, 1);
    ASSERT_EQ(result, VACCEL_OK);

    struct vaccel_resource res;
    res.type = VACCEL_RES_SHARED_OBJ;

    result = vaccel_sess_register(&sess, &res);
    EXPECT_EQ(result, VACCEL_OK);

    result = vaccel_sess_register(NULL, &res);
    EXPECT_EQ(result, VACCEL_EINVAL);

    result = vaccel_sess_register(&sess, NULL);
    EXPECT_EQ(result, VACCEL_EINVAL);

    res.type = VACCEL_RES_MAX;
    result = vaccel_sess_register(&sess, &res);
    EXPECT_EQ(result, VACCEL_EINVAL);

    res.type = VACCEL_RES_SHARED_OBJ;
}

TEST_F(SessionTest, sess_unregister) {
    struct vaccel_session sess;
    int result = vaccel_sess_init(&sess, 1);
    ASSERT_EQ(result, VACCEL_OK);

    struct vaccel_resource res;
    res.type = VACCEL_RES_SHARED_OBJ;

    result = vaccel_sess_register(&sess, &res);
    ASSERT_EQ(result, VACCEL_OK);

    bool check_bool = vaccel_sess_has_resource(&sess, &res);
    ASSERT_EQ(check_bool, true);

    result = vaccel_sess_unregister(&sess, &res);
    EXPECT_EQ(result, VACCEL_OK);

    check_bool = vaccel_sess_has_resource(&sess, &res);
    ASSERT_EQ(check_bool, false);
}


TEST_F(SessionTest, sess_unregister_null) {
    struct vaccel_session sess;
    int result = vaccel_sess_init(&sess, 1);
    ASSERT_EQ(result, VACCEL_OK);

    struct vaccel_resource res;
    res.type = VACCEL_RES_SHARED_OBJ;

    result = vaccel_sess_register(&sess, &res);
    ASSERT_EQ(result, VACCEL_OK);

    bool check_bool = vaccel_sess_has_resource(&sess, &res);
    ASSERT_EQ(check_bool, true);

    result = vaccel_sess_unregister(NULL, &res);
    ASSERT_EQ(result, VACCEL_EINVAL);

    result = vaccel_sess_unregister(&sess, NULL);
    ASSERT_EQ(result, VACCEL_EINVAL);

    res.type = VACCEL_RES_MAX;
    result = vaccel_sess_unregister(&sess, &res);
    ASSERT_EQ(result, VACCEL_EINVAL);

    res.type = VACCEL_RES_SHARED_OBJ;
    result = vaccel_sess_unregister(&sess, &res);
    ASSERT_EQ(result, VACCEL_OK);

    check_bool = vaccel_sess_has_resource(&sess, &res);
    ASSERT_EQ(check_bool, false);

}

TEST_F(SessionTest, sess_viritio) {
    struct vaccel_session sess;
    int result = vaccel_sess_init(&sess, 1);
    ASSERT_EQ(result, VACCEL_OK);

    RESET_FAKE(get_virtio_plugin);

    // Create a mock plugin
    struct vaccel_plugin_info v_mock_info;
    v_mock_info.name = "fake_virtio";
    v_mock_info.sess_init = mock_sess_init;
    v_mock_info.sess_free = mock_sess_free;
    v_mock_info.sess_update = mock_sess_update;
    v_mock_info.sess_register = mock_sess_register;
    v_mock_info.sess_unregister = mock_sess_unregister;

    struct vaccel_plugin v_mock;
    v_mock.info = &v_mock_info;

    get_virtio_plugin_fake.return_val = &v_mock;

    result = vaccel_sess_update(&sess, 2);
    ASSERT_EQ(result, VACCEL_OK);

    struct vaccel_resource res;
    res.type = VACCEL_RES_SHARED_OBJ;

    result = vaccel_sess_register(&sess, &res);
    ASSERT_EQ(result, VACCEL_OK);

    result = vaccel_sess_unregister(&sess, &res);
    EXPECT_EQ(result, VACCEL_OK);

    result = vaccel_sess_free(&sess);
    EXPECT_EQ(result, VACCEL_OK);

    EXPECT_EQ(get_virtio_plugin_fake.call_count, 4);
}