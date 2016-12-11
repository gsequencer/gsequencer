#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

AgsThread *main_loop;
AgsThread *thread;

AgsApplicationContext *application_context;

application_context = ags_application_context_get_instance();

main_loop = ags_generic_main_loop_new(application_context);
g_object_set(application_context,
             "main-loop\0", main_loop,
             NULL);

thread = ags_thread_new(NULL);
ags_thread_add_child_extended(main_loop,
                              thread,
                              TRUE, TRUE);
g_atomic_pointer_set(&(main_loop->start_queue),
                     g_list_prepend(g_atomic_pointer_get(&(main_loop->start_queue)),
                                    thread));

ags_thread_start(main_loop);
