#include <libxml/tree.h>
#include <glib-object.h>

#include <ags/thread/ags_thread.h>

AgsThread* ags_test_thread_create_audio_loop(xmlNode *current);
AgsThread* ags_test_thread_create_thread(xmlNode *current);
AgsThread* ags_test_thread_read_thread(xmlNode *current);

AgsThread*
ags_test_thread_create_audio_loop(xmlNode *current)
{
}

AgsThread*
ags_test_thread_create_thread(xmlNode *current)
{
}

AgsThread*
ags_test_thread_read_thread(xmlNode *current)
{
}

int
main(int argc, char **argv)
{
  AgsThread *thread, *current;

  return(0);
}
