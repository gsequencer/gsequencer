#include <ags/lib/ags_log.h>

#include <time.h>
#include <string.h>

void ags_log_class_init(AgsLogClass *log);
void ags_log_init(AgsLog *log);
void ags_log_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_log_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_log_finalize(GObject *gobject);

void* ags_log_broker(void *ptr);
void* ags_log_output(void *ptr);
void* ags_log_queue(void *ptr);

enum{
  PROP_0,
  PROP_FILE,
};

static gpointer ags_log_parent_class = NULL;

GType
ags_log_get_type (void)
{
  static GType ags_type_log = 0;

  if(!ags_type_log){
    static const GTypeInfo ags_log_info = {
      sizeof (AgsLogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_log_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_log_init,
    };

    ags_type_log = g_type_register_static(G_TYPE_OBJECT,
					  "AgsLog\0",
					  &ags_log_info,
					  0);
  }

  return (ags_type_log);
}

void
ags_log_class_init(AgsLogClass *log)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_log_parent_class = g_type_class_peek_parent(log);

  /* GObjectClass */
  gobject = (GObjectClass *) log;

  gobject->set_property = ags_log_set_property;
  gobject->get_property = ags_log_get_property;

  /* properties */
  param_spec = g_param_spec_pointer("file\0",
				    "output file\0",
				    "The file to do the output\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE,
				  param_spec);

}

void
ags_log_init(AgsLog *log)
{
  log->flags = 0;
  
  log->file = NULL;
  
  log->log_interval = (struct timespec *) malloc(sizeof(struct timespec));
  log->log_interval->tv_sec = 0;
  log->log_interval->tv_nsec = 10000000; /* sleep 10 mili seconds */

  log->output_formated_message = NULL;
  log->queue_formated_message = NULL;
  log->log_message = NULL;
}

void
ags_log_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsLog *log;

  log = AGS_LOG(gobject);
  
  switch(prop_id){
  case PROP_FILE:
    {
      log->file = g_value_get_pointer(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_log_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsLog *log;

  log = AGS_LOG(gobject);
  
  switch(prop_id){
  case PROP_FILE:
    {
      g_value_set_pointer(value, log->file);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_log_finalize(GObject *gobject)
{
  AgsLog *log;
  GList *list, *list_next;

  log = AGS_LOG(gobject);
}

/**
 * ags_log_broker:
 * @ptr an #AgsLog
 *
 * Wakes up sleeping output, queue and log threads.
 */
void*
ags_log_broker(void *ptr)
{
  AgsLog *log;
  pthread_t sleep_thread;
  pthread_mutex_t broker_mutex;
  pthread_mutexattr_t broker_mutex_attr;
  pthread_cond_t broker_wait_cond;
  gboolean time_elapsed;

  auto void* ags_log_broker_sleep();

  void* ags_log_broker_sleep(){
    //FIXME:JK: doesn't test if it's behindhand
    nanosleep(log->log_interval);

    pthread_mutex_lock(&(broker_mutex));
    time_elapsed = TRUE;
    pthread_mutex_unlock(&(broker_mutex));
    pthread_cond_signal(&(broker_wait_cond));
  }

  log = AGS_LOG(ptr);

  pthread_mutexattr_init(&broker_mutex);
  pthread_mutex_init(&(broker_mutex_attr), NULL);
  pthread_cond_init(&(broker_wait_cond), NULL);

  while((AGS_LOG_RUNNING & (log->flags)) != 0){
    time_elapsed = FALSE;
    pthread_create(&(sleep_thread), NULL,
		   &ags_log_broker_sleep, NULL);


    pthread_cond_broadcast(&(log->log_wait_cond));

    pthread_cond_signal(&(log->queue_wait_cond));

    pthread_cond_signal(&(log->output_wait_cond));

    pthread_mutex_lock(&(broker_mutex));

    while(!time_elapsed){
      pthread_cond_wait(&(broker_wait_cond),
			&(broker_mutex));
    }

    pthread_mutex_unlock(&(broker_mutex));
  }
}

void*
ags_log_output(void *ptr)
{
}

void*
ags_log_queue(void *ptr)
{
}


void
ags_log_start_queue(AgsLog *log)
{
}

void
ags_log_stop_queue(AgsLog *log)
{
}

void
ags_log_message(AgsLog *log, char *format, ...)
{
  AgsLogMessage *log_message;
  va_list args;

  auto void ags_log_add_message();
  
  void ags_log_add_message(){
    
  };

  /* alloc AgsLogMessage */
  log_message = (AgsLogMessage *) malloc(sizeof(AgsLogMessage));

  /* copy parameters */
  log_message->time = (struct timespec *) malloc(sizeof(struct timespec));
  clock_gettime(CLOCK_REALTIME, log_message->time);

  log_message->format = format;

  va_start(args, format);
  va_copy(log_message->args, args);
  va_end(args);

  /* add to queue */
}

AgsLog*
ags_log_new()
{
  AgsLog *log;

  log = (AgsLog *) g_object_new(AGS_TYPE_LOG, NULL);

  return(log);
}
