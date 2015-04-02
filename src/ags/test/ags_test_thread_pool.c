#include <glib.h>
#include <glib-object.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_main_loop.h>

#include <ags/thread/ags_thread-posix.h>
#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_returnable_thread.h>
#include <ags/thread/ags_thread_pool.h>

int
main(int argc, char **argv)
{
  AgsMain *ags_main;
  AgsThread *audio_loop;
  AgsReturnableThread *thread;
  AgsThreadPool *thread_pool;

  gtk_init(&argc, &argv);

  ags_main = ags_main_new();

  ags_main->main_loop = 
    audio_loop = ags_audio_loop_new(NULL, ags_main);
  g_object_ref(G_OBJECT(audio_loop));
  ags_connectable_connect(AGS_CONNECTABLE(audio_loop));

  /* start thread tree */
  ags_thread_start(audio_loop);
  
  /* complete thread pool */
  thread_pool = ags_thread_pool_new(ags_main);

  thread_pool->parent = audio_loop;
  ags_thread_pool_start(thread_pool);

  while(TRUE){
    thread = ags_thread_pool_pull(thread_pool);
    g_atomic_int_or(&(AGS_RETURNABLE_THREAD(thread)->flags),
		    AGS_RETURNABLE_THREAD_IN_USE);
    g_object_unref(thread);
  }

  pthread_join(audio_loop->thread,
	       NULL);

  return(0);
}
