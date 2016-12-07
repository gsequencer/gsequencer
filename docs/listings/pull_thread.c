#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

void thread_run_callback(AgsThread *thread, gpointer data);

void
thread_run_callback(AgsThread *thread, gpointer data)
{
  g_message("%s\0", (gchar *) data);
}

int
main(int argc, char **argv)
{
  AgsThread *main_loop;
  AgsThread *thread_0, *thread_1;
  AgsThreadPool *thread_pool;
  
  AgsApplicationContext *application_context;

  application_context = ags_thread_application_context();

  main_loop = application_context->main_loop;
  
  thread_pool = ags_concurrency_provider_get_thread_pool(AGS_CONCURRENCY_PROVIDER(application_context));

  ags_thread_start(main_loop);
  ags_thread_pool_start(thread_pool);
  
  /* create thread 0 */
  thread_0 = ags_thread_pool_pull(task_thread->thread_pool);
  
  pthread_mutex_lock(AGS_RETURNABLE_THREAD(thread_0)->reset_mutex);

  g_atomic_pointer_set(&(AGS_RETURNABLE_THREAD(thread_0)->safe_data),
                       "thread 0\0");

  ags_returnable_thread_connect_safe_run(AGS_RETURNABLE_THREAD(thread_0),
                                         ags_task_thread_append_task_queue);

  g_atomic_int_or(&(AGS_RETURNABLE_THREAD(thread_0)->flags),
                  AGS_RETURNABLE_THREAD_IN_USE);
    
  pthread_mutex_unlock(AGS_RETURNABLE_THREAD(thread_0)->reset_mutex);

  /* create thread 1 */
  thread_1 = ags_thread_pool_pull(task_thread->thread_pool);
  
  pthread_mutex_lock(AGS_RETURNABLE_THREAD(thread_1)->reset_mutex);

  g_atomic_pointer_set(&(AGS_RETURNABLE_THREAD(thread_1)->safe_data),
                       "thread 1\0");

  ags_returnable_thread_connect_safe_run(AGS_RETURNABLE_THREAD(thread_1),
                                         ags_task_thread_append_task_queue);

  g_atomic_int_or(&(AGS_RETURNABLE_THREAD(thread_1)->flags),
                  AGS_RETURNABLE_THREAD_IN_USE);
    
  pthread_mutex_unlock(AGS_RETURNABLE_THREAD(thread_1)->reset_mutex);

  /*  */
  pthread_join(*(main_loop->thread));
  
  return(0);
}
