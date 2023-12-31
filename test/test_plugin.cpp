/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

extern "C" {
#include "plugin.h"
#include "error.h"
#include "list.h"
}

#include <stdbool.h>
#include <string.h>
#include <dlfcn.h>

static const char *pname = "mock_plugin";
void dummy_function() {}
static int no_op() {return 1;}
static int no_op_exec(){return 2;}
static int no_op_fpga() {return 3;}

class PluginTestsRegister : public ::testing::Test {
	static int fini(void)
	{
		return VACCEL_OK;
	} 
	static int init(void)
	{
		return VACCEL_OK;
	}

protected:
	/* initialize structures with zeros */
	struct vaccel_plugin plugin = {0};
	struct vaccel_plugin_info pinfo = {0};

	void SetUp() override
	{
		plugin.info = &this->pinfo;
		plugin.info->name = pname;
		list_init_entry(&plugin.entry);
		list_init_entry(&plugin.ops);
		plugin.info->init = init;
		plugin.info->fini = fini;

		plugins_bootstrap();
	}

	void TearDown() override
	{
		plugins_shutdown();
	}
};

class PluginTestsUnregister : public ::testing::Test {
	static int fini(void)
	{
		return VACCEL_OK;
	} 
	static int init(void)
	{
		return VACCEL_OK;
	}

protected:
	/* initialize structures with zeros */
	struct vaccel_plugin plugin = {0};
	struct vaccel_plugin_info pinfo = {0};

	void SetUp() override
	{
		plugin.info = &this->pinfo;
		plugin.info->name = pname;
		list_init_entry(&plugin.entry);
		list_init_entry(&plugin.ops);
		plugin.info->init = init;
		plugin.info->fini = fini;

		plugins_bootstrap();

		register_plugin(&plugin);
	}

	void TearDown() override
	{
		plugins_shutdown();
	}
};

class PluginTestsRegisterFunction : public ::testing::Test {
protected:
    static struct vaccel_plugin* plugin_test;

    static int fini() {
        return VACCEL_OK;
    }

    static int init() {
        return VACCEL_OK;
    }

    void SetUp() override {
        plugins_bootstrap();

        void* dl_handle = dlopen("../plugins/noop/libvaccel-noop.so", RTLD_LAZY);
        if (!dl_handle) {
            FAIL() << "Error loading plugin library: " << dlerror();
        }

        void* plugin_symbol = dlsym(dl_handle, "vaccel_plugin");
        if (!plugin_symbol) {
            FAIL() << "Error loading vaccel_plugin symbol from the library: " << dlerror();
        }

        plugin_test = static_cast<struct vaccel_plugin*>(plugin_symbol);
        register_plugin(plugin_test);
    }

    void TearDown() override {
        plugins_shutdown();
    }
};

class PluginTestsRegisterOperation : public ::testing::Test {
    static int fini(void) {
        return VACCEL_OK;
    }

    static int init(void) {
        return VACCEL_OK;
    }

protected:
    struct vaccel_plugin no_op_plugin = {0};
    struct vaccel_plugin_info noop_pinfo = {0};

	struct vaccel_op no_op_operation = {
    .type = VACCEL_NO_OP,
    .func = (void *)no_op,
    .owner = nullptr,
	};


    void SetUp() override {
        no_op_plugin.info = &noop_pinfo;
        no_op_plugin.info->name = pname;
        list_init_entry(&no_op_plugin.entry);
        list_init_entry(&no_op_plugin.ops);
        no_op_plugin.info->init = init;
        no_op_plugin.info->fini = fini;

        plugins_bootstrap();

		no_op_operation.owner = &no_op_plugin;
        
		register_plugin(&no_op_plugin);
		
        register_plugin_function(&no_op_operation);
    }

    void TearDown() override {
        plugins_shutdown();
    }
};

class PluginTestsRegisterOperationMultiple : public ::testing::Test {
    static int fini(void) {
        return VACCEL_OK;
    }

    static int init(void) {
        return VACCEL_OK;
    }

protected:
    struct vaccel_plugin no_op_plugin = {0};
    struct vaccel_plugin_info noop_pinfo = {0};

	struct vaccel_op exec_operation = {
		.type = VACCEL_EXEC,
		.func = (void *)no_op_exec,
		.owner = &no_op_plugin,
	};

	struct vaccel_op copy_operation = {
		.type = VACCEL_F_ARRAYCOPY,
		.func = (void *)no_op_fpga,
		.owner = &no_op_plugin,
	};
	struct vaccel_op array_ops[2] = {exec_operation, copy_operation};

    void SetUp() override {
		int ret;
        no_op_plugin.info = &noop_pinfo;
        no_op_plugin.info->name = pname;
        list_init_entry(&no_op_plugin.entry);
        list_init_entry(&no_op_plugin.ops);
        no_op_plugin.info->init = init;
        no_op_plugin.info->fini = fini;

        plugins_bootstrap();

		ret = register_plugin(&no_op_plugin);
		ASSERT_EQ(ret, VACCEL_OK);

		ret = register_plugin_functions(array_ops, (sizeof(array_ops) / sizeof(array_ops[0])));
		ASSERT_EQ(ret, VACCEL_OK);	

    }

    void TearDown() override {
        plugins_shutdown();
    }
};

struct vaccel_plugin* PluginTestsRegisterFunction::plugin_test = nullptr;

TEST_F(PluginTestsRegister, plugin_init_null) {
	ASSERT_EQ(register_plugin(NULL), VACCEL_EINVAL);
}

TEST_F(PluginTestsRegister, plugin_init_null_name) {
	struct vaccel_plugin_info info = { 0 };
	struct vaccel_plugin plugin;
	plugin.info = &info;
	plugin.entry = LIST_ENTRY_INIT(plugin.entry);

	ASSERT_EQ(register_plugin(&plugin), VACCEL_EINVAL);
}

TEST(plugin_init_not_bootstraped, not_ok) {
	struct vaccel_plugin plugin = {};
	ASSERT_EQ(register_plugin(&plugin), VACCEL_EBACKEND);
}

TEST_F(PluginTestsRegister, plugin_init_values) {
	ASSERT_EQ(register_plugin(&plugin), VACCEL_OK);
	ASSERT_EQ(strcmp(plugin.info->name, "mock_plugin"), 0);
	ASSERT_TRUE(list_empty(&plugin.ops));
}

TEST_F(PluginTestsRegister, plugin_register_null) {
	ASSERT_EQ(register_plugin(NULL), VACCEL_EINVAL);
}

TEST(plugin_register_not_bootstraped, not_ok) {
	struct vaccel_plugin plugin = {};
	ASSERT_EQ(register_plugin(&plugin), VACCEL_EBACKEND);
}

TEST_F(PluginTestsRegister, plugin_register_existing) {
	ASSERT_EQ(register_plugin(&plugin), VACCEL_OK);
	ASSERT_EQ(register_plugin(&plugin), VACCEL_EEXISTS);
}

TEST_F(PluginTestsRegister, plugin_register_with_ops) {
}

TEST_F(PluginTestsRegister, plugin_invalid_pinfo){
	struct vaccel_plugin_info temp = pinfo;
	pinfo.fini = NULL;
	ASSERT_EQ(register_plugin(&plugin), VACCEL_EINVAL);
	pinfo.init = NULL;
	ASSERT_EQ(register_plugin(&plugin), VACCEL_EINVAL);
	pinfo.name = NULL;
	ASSERT_EQ(register_plugin(&plugin), VACCEL_EINVAL);
	pinfo = temp;
	ASSERT_EQ(register_plugin(&plugin), VACCEL_OK);
}


TEST_F(PluginTestsUnregister, no_link_entry){
	list_unlink_entry(&plugin.entry);
	ASSERT_EQ(unregister_plugin(&plugin), VACCEL_ENOENT);
}

TEST_F(PluginTestsUnregister, no_info_entry){
	plugin.info = NULL;
	ASSERT_EQ(unregister_plugin(&plugin), VACCEL_EINVAL);
}

TEST_F(PluginTestsUnregister, valid_unregister){
	ASSERT_EQ(unregister_plugin(NULL), VACCEL_EINVAL);
	ASSERT_EQ(unregister_plugin(&plugin), VACCEL_OK);
}

TEST_F(PluginTestsRegisterFunction, invalid_register){
	ASSERT_EQ(register_plugin_function(NULL), VACCEL_EINVAL);
}

TEST_F(PluginTestsRegisterFunction, invalid_vaccel_function) {
    struct vaccel_op test_op;
    test_op.func = NULL;
    test_op.type = VACCEL_FUNCTIONS_NR + 1;
    test_op.owner = plugin_test;
    ASSERT_EQ(register_plugin_function(&test_op), VACCEL_EINVAL);
}

TEST_F(PluginTestsRegisterFunction, unknown_function_type) {
    struct vaccel_op test_op;
    test_op.func = (void *)&dummy_function;
    test_op.type = VACCEL_FUNCTIONS_NR + 1;
    test_op.owner = plugin_test;
    ASSERT_EQ(register_plugin_function(&test_op), VACCEL_EINVAL + 1);
}

TEST_F(PluginTestsRegisterFunction, unknown_plugin) {
    struct vaccel_op test_op;
    test_op.func = (void *)&dummy_function;
    test_op.type = 1;
    test_op.owner = NULL;
    ASSERT_EQ(register_plugin_function(&test_op), VACCEL_EINVAL + 2);
}

TEST_F(PluginTestsRegisterFunction, valid_registeration) {
    struct vaccel_op test_op;
	// test_op.func = get_plugin_op(VACCEL_NO_OP, 0);
	test_op.func = (void *)&dummy_function; 
    test_op.type = 1;
    test_op.owner = plugin_test;
    ASSERT_EQ(register_plugin_function(&test_op), VACCEL_OK);
}

TEST_F(PluginTestsRegisterOperation, fetch_operation) {
    void* operation = get_plugin_op(VACCEL_NO_OP, 0);
    ASSERT_NE(operation, nullptr);

    int ret = reinterpret_cast<int (*)(void)>(operation)();
    ASSERT_EQ(ret, 1);
}

TEST_F(PluginTestsRegisterOperation, fetch_operation_unknown_function) {
	enum vaccel_op_type unknown_function = static_cast<enum vaccel_op_type>(VACCEL_FUNCTIONS_NR + 1);
    void* operation = get_plugin_op(unknown_function, 0);
    ASSERT_EQ(operation, nullptr);
}

TEST_F(PluginTestsRegisterOperation, fetch_operation_not_implemented) {
    void* operation = get_plugin_op(VACCEL_BLAS_SGEMM, 0);
    ASSERT_EQ(operation, nullptr);
}

TEST_F(PluginTestsRegisterOperationMultiple, fetch_operation_env_prio) {
    // we have implemented FPGA in our fixture and lets assume our plugin only implements FPGA functions
	noop_pinfo.type = VACCEL_PLUGIN_FPGA;

	void* operation = get_plugin_op(VACCEL_F_ARRAYCOPY, VACCEL_PLUGIN_FPGA);
    ASSERT_NE(operation, nullptr);

    int ret = reinterpret_cast<int (*)(void)>(operation)();
    ASSERT_EQ(ret, 3);
}

TEST_F(PluginTestsRegisterOperation, find_noop) {

	int ret = get_available_plugins(VACCEL_NO_OP);
	ASSERT_EQ(ret,VACCEL_OK);
}

TEST_F(PluginTestsRegisterOperation, implementation_not_found) {

	int ret = get_available_plugins(VACCEL_EXEC);
	ASSERT_EQ(ret,VACCEL_OK);
}

TEST_F(PluginTestsRegisterOperationMultiple, mult_operations){

	void* operation ;
	int ret;

	operation = get_plugin_op(VACCEL_EXEC, 0);
    ASSERT_NE(operation, nullptr);
    ret = reinterpret_cast<int (*)(void)>(operation)();
    ASSERT_EQ(ret, 2);

	operation = get_plugin_op(VACCEL_F_ARRAYCOPY, 0);
    ASSERT_NE(operation, nullptr);
    ret = reinterpret_cast<int (*)(void)>(operation)();
    ASSERT_EQ(ret, 3);
	
}

// TEST(PluginTests, check_plugin_loaded_operation) {

// 	int ret;
// 	ret = plugins_bootstrap();

// 	ASSERT_EQ(ret, VACCEL_OK);

// 	void* dl_handle = dlopen("../plugins/noop/libvaccel-noop.so", RTLD_LAZY);
// 	if (!dl_handle) {
// 		FAIL() << "Error loading plugin library: " << dlerror();
// 	}

// 	void* plugin_symbol = dlsym(dl_handle, "vaccel_plugin");
// 	if (!plugin_symbol) {
// 		FAIL() << "Error loading vaccel_plugin symbol from the library: " << dlerror();
// 	}
// 	struct vaccel_plugin* plugin_test;
// 	plugin_test = static_cast<struct vaccel_plugin*>(plugin_symbol);

// 	ret = register_plugin(plugin_test);

// 	ASSERT_EQ(ret, VACCEL_OK);


// 	ret = register_plugin(plugin_test_noop);

//     enum vaccel_op_type op_type = VACCEL_NO_OP;
//     void* operation = get_plugin_op(op_type, 0);
//     ASSERT_NE(operation, nullptr);
// }