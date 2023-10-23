#include <gtest/gtest.h>
#include <atomic>
extern "C"{
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

#include "plugin.h"
#include "session.h"
#include "log.h"
#include "vaccel.h"
#include "resources.h"
#include "utils.h"

#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>

#define MAX_RUNDIR_PATH 1024

/* Runtime directory for holding resources related with the
 * runtime */
static char rundir[MAX_RUNDIR_PATH];

// static int load_backend_plugin(const char *path)
// {
// 	int ret;
// 	struct vaccel_plugin *plugin, **plugin_p;

// 	void *dl = dlopen(path, RTLD_LAZY);
// 	if (!dl) {
// 		vaccel_error("Could not dlopen plugin %s: %s", path, dlerror());
// 		return VACCEL_ENOENT;
// 	}

// 	plugin_p = (vaccel_plugin**)dlsym(dl, "vaccel_plugin");

// 	if (!plugin_p) {
// 		vaccel_error("Not a vaccel plugin: %s", path);
// 		ret = VACCEL_ELIBBAD;
// 		goto close_dl;
// 	}

// 	plugin = *plugin_p;
// 	ret = register_plugin(plugin);
// 	if (ret != VACCEL_OK)
// 		goto close_dl;

// 	/* Initialize the plugin */
// 	ret = plugin->info->init();
// 	if (ret != VACCEL_OK)
// 		goto close_dl;

// 	/* Keep dl handle so we can later close the library */
// 	plugin->dl_handle = dl;

// 	vaccel_debug("Loaded plugin %s from %s", plugin->info->name, path);

// 	return VACCEL_OK;

// close_dl:
// 	dlclose(dl);
// 	return ret;
// }

// static int load_backend_plugins(char *plugins)
// {
// 	char *plugin;

// 	char *plugins_tmp = strdup(plugins);
// 	if (!plugins_tmp)
// 		return VACCEL_ENOMEM;

// 	char *p = plugins_tmp;
// 	plugin = strtok(p, ":");
// 	while (plugin) {
// 		int ret = load_backend_plugin(plugin);
// 		if (ret != VACCEL_OK)
// 			return ret;

// 		plugin = strtok(NULL, ":");
// 	}
	
// 	free(plugins_tmp);
// 	return VACCEL_OK;
// }

static int create_vaccel_rundir(void)
{
	int ret = snprintf(rundir, MAX_RUNDIR_PATH, "/run/user/%u", getuid());
	if (ret == MAX_RUNDIR_PATH) {
		vaccel_fatal("rundir path '%s' too long", rundir);
		return VACCEL_ENAMETOOLONG;	
	}

	if (!dir_exists(rundir)) {
		vaccel_debug("User rundir does not exist. Will try to create it");
		if (!dir_exists("/run/user")) {
			vaccel_debug("/run/user dir does not exist. Will try to create it first");
			ret = mkdir("/run/user", 0700);
			if (ret) {
				vaccel_fatal("Could not create user rundir: %s",
						strerror(errno));
				return VACCEL_ENOENT;
			}
		}
		ret = mkdir(rundir, 0700);
		if (ret) {
			vaccel_fatal("Could not create user rundir: %s",
					strerror(errno));
			return VACCEL_ENOENT;
		}

		vaccel_debug("Created user rundir %s", rundir);
	}

	ret = snprintf(rundir, MAX_RUNDIR_PATH,
			"/run/user/%u/vaccel.XXXXXX", getuid());
	if (ret == MAX_RUNDIR_PATH) {
		vaccel_error("rundir path '%s' too big", rundir);
		return VACCEL_ENAMETOOLONG;
	}

	char *dir = mkdtemp(rundir);
	if (!dir) {
		vaccel_error("Could not initialize top-level rundir: %s", dir);
		return errno;
	}

	vaccel_debug("Created top-level rundir: %s", rundir);
	
	return VACCEL_OK;
}

// static int cleanup_vaccel_rundir(void)
// {
// 	/* Try to cleanup the rundir. At the moment, we do not fail
// 	 * if this fails, we just warn the user */
// 	if (cleanup_rundir(rundir))
// 		vaccel_warn("Could not cleanup rundir '%s'", rundir);

// 	return VACCEL_OK;
// }

const char *vaccel_rundir(void)
{
	return rundir;
}


}


TEST(VaccelTest, ConstructorDestructor)
{
    int ret = vaccel_log_init();
    ASSERT_EQ(ret, VACCEL_OK);
    ret = create_vaccel_rundir();
    ASSERT_EQ(ret, VACCEL_OK);
    ret = sessions_bootstrap();
    ASSERT_EQ(ret, VACCEL_OK);
    ret = resources_bootstrap();
    ASSERT_EQ(ret, VACCEL_OK);
    ret = plugins_bootstrap();
    ASSERT_EQ(ret, VACCEL_OK);

    // const char* plugins = getenv("VACCEL_BACKENDS");

    // ASSERT_TRUE(plugins == nullptr);

    // ASSERT_EQ(load_backend_plugins(const_cast<char*>(plugins)), VACCEL_OK);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
