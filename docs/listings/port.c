#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

AgsPort *port;
GValue value = {0,};

/* create port */
port = ags_port_new();

/* initialize and set value */
g_value_init(&value,
	     G_TYPE_FLOAT);
g_value_set_float(&value,
		  0.0);

/* perform thread-safe operation */
ags_port_safe_write(port,
		    &value);
