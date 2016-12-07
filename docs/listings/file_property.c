#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

AgsApplicationContext *application_context;
AgsFile *file;

application_context = ags_application_context_get_instance();

file = ags_file_new();
g_object_set(application_context,
             "file\0", file,
             NULL);
