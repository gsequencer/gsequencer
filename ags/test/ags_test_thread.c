#include <libxml/tree.h>
#include <glib-object.h>

#include <ags/thread/ags_thread.h>

#define AGS_TEST_THREAD_DEFAULT_ENCODING "UTF-8\0"
#define AGS_TEST_THREAD_DEFAULT_FILE "./thread_tree.xml\0"

AgsThread* ags_test_thread_create_audio_loop(xmlNode *current);
AgsThread* ags_test_thread_create_thread(xmlNode *current);
AgsThread* ags_test_thread_read_thread(xmlNode *current);

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static FILE *log;

AgsThread*
ags_test_thread_create_audio_loop(AgsThread *thread, xmlNode *current)
{
}

AgsThread*
ags_test_thread_create_thread(AgsThread *thread, xmlNode *current)
{
}

void
ags_test_thread_read_thread(AgsThread *thread, xmlNode *current)
{
  while(current != NULL){
    if(current->type == XML_ELEMENT_NODE && !xmlStrncmp("thread\0", current->name, 6)){
      ags_test_thread_create_thread(current);
    }
    
    current = current->next;
  }
}

int
main(int argc, char **argv)
{
  AgsThread *thread;
  xmlDoc *doc;
  xmlNode *current;
  char *name;
  char *encoding;
  char *dtd;

  log = fopen("./log\0", "a\0");
  name = AGS_TEST_THREAD_DEFAULT_FILE;
  encoding = AGS_TEST_THREAD_DEFAULT_ENCODING;

  doc = xmlReadFile(name, NULL, 0);

  if(doc == NULL){
    return(-1);
  }

  thread = (AgsThread *) ags_audio_loop_new();
  current = xmlDocGetRootElement(doc);

  ags_test_thread_read_thread(thread, current);

  return(0);
}
