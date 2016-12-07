#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

AgsConfig *config;
gchar *str;

config = ags_config_get_instance();
ags_config_load_defaults(config);

str = ags_config_get_value(config,
                           "thread\0",
                           "model\0");
