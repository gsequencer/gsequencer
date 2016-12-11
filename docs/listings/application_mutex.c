#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <pthread.h>

AgsApplicationContext *application_context;

AgsThread *main_loop;
AgsMutexManager *mutex_manager;

pthread_mutex_t *application_mutex;

mutex_manager = ags_mutex_manager_get_instance();
application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

pthread_mutex_lock(application_mutex);

application_context = ags_application_context_get_instance();
g_object_get(application_context,
             "main-loop\0", &main_loop,
             NULL);

pthread_mutex_unlock(application_mutex);
