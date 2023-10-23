#include <gtest/gtest.h>

extern "C" {
#include "error.h"
#include "fpga.h"
#include "session.h"
}

TEST(vaccel_fpga_plugins, vaccel_fpga_add) {

    const char* vaccel_backends = std::getenv("VACCEL_BACKENDS");

    if (vaccel_backends) {
        std::cout << "VACCEL_BACKENDS: " << vaccel_backends << std::endl;
    } else {
        std::cerr << "VACCEL_BACKENDS environment variable not set." << std::endl;
    }

    struct vaccel_session sess;
    float A[] = {1, 2, 3, 4, 5};
    float B[] = {1, 2, 6, 4, 5};
    float C[] = {1, 2, 6, 4, 5};

    size_t len_A = sizeof(A) / sizeof(A[0]);
    size_t len_B = sizeof(B) / sizeof(B[0]);

    int ret = vaccel_sess_init(&sess, 0);
    EXPECT_EQ(ret, VACCEL_OK);

    ret = vaccel_fpga_vadd(&sess, A, B, C, len_A, len_B);
    float C_expected[] = {2, 4, 9, 8, 10};
    size_t len_C = sizeof(C) / sizeof(C[0]);

    EXPECT_EQ(vaccel_sess_free(&sess), VACCEL_OK);
    EXPECT_NE(ret, VACCEL_ENOTSUP);

    EXPECT_EQ(len_C, len_A);

    for (size_t i = 0; i < len_C; ++i) {
        EXPECT_FLOAT_EQ(C[i], C_expected[i]);
    }
}

TEST(vaccel_fpga_plugins, vaccel_fpga_copy) {
    struct vaccel_session sess;
    int A[] = {1, 2, 3, 4, 5};
    int B[] = {1, 1, 1, 1, 1};

    size_t len_A = sizeof(A) / sizeof(A[0]);
    size_t len_B = sizeof(B) / sizeof(B[0]);

    int ret = vaccel_sess_init(&sess, 0);
    EXPECT_EQ(ret, VACCEL_OK);

    ret = vaccel_fpga_arraycopy(&sess, A, B, len_A);
    int B_expected[] = {1, 2, 3, 4, 5};

    EXPECT_EQ(vaccel_sess_free(&sess), VACCEL_OK);
    EXPECT_NE(ret, VACCEL_ENOTSUP);

    for (size_t i = 0; i < len_B; ++i) {
        EXPECT_EQ(B[i], B_expected[i]);
    }
}

TEST(vaccel_fpga_plugins, vaccel_fpga_mmult) {
    struct vaccel_session test_sess;
    float a[] = {1.2, 3.2, 3.0, 4.1, 5.7};
    float b[] = {1.1, 0.2, 6.1, 4.6, 5.2};
    float c[] = {0.1, 0.1, 0.1, 0.1, 0.1};
    size_t len_c = sizeof(c) / sizeof(c[0]);

    int ret = vaccel_sess_init(&test_sess, 0);
    EXPECT_EQ(ret, VACCEL_OK);

    ret = vaccel_fpga_mmult(&test_sess, a, b, c, len_c);
    float C_expected[] = {9.1, 9.1, 9.1, 9.1, 9.1};

    EXPECT_EQ(vaccel_sess_free(&test_sess), VACCEL_OK);
    EXPECT_NE(ret, VACCEL_ENOTSUP);

    for (size_t i = 0; i < len_c; ++i) {
        EXPECT_FLOAT_EQ(c[i], C_expected[i]);
    }
}

TEST(vaccel_fpga_plugins, vaccel_fpga_parallel) {
    struct vaccel_session test_sess;
    float a[] = {1.2, 3.2, 3.0, 4.1, 5.7};
    float b[] = {1.1, 0.2, 6.1, 4.6, 5.2};
    size_t len_a = sizeof(a) / sizeof(a[0]);
    float add_out[len_a];
    float mult_out[len_a];

    int ret = vaccel_sess_init(&test_sess, 0);
    EXPECT_EQ(ret, VACCEL_OK);

    ret = vaccel_fpga_parallel(&test_sess, a, b, add_out, mult_out, len_a);

    EXPECT_EQ(vaccel_sess_free(&test_sess), VACCEL_OK);
    EXPECT_NE(ret, VACCEL_ENOTSUP);

}