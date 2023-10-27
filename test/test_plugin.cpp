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

struct vaccel_plugin* PluginTestsRegisterFunction::plugin_test = nullptr;

void dummy_function() {}

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