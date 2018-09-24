#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

AgsThread *thread;

pthread_mutex_t *thread_mutex;

thread = ags_thread_new(NULL);

/* get object mutex */
pthread_mutex_lock(ags_thread_get_class_mutex());

thread_mutex = thread->obj_mutex;

pthread_mutex_unlock(ags_thread_get_class_mutex());
