#include <ags/lib/ags_log.h>

#include <ags/lib/ags_list.h>

#include <time.h>
#include <stdio.h>
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

#define AGS_LOG_MESSAGE_LENGTH 4096
#define AGS_LOG_MESSAGE_DATE_LENGTH 19

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
    //FIXME:JK: doesn't test if it's behindhand, free_float member is reserved for this issue
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

    while(!time_elapsed &&
	  log->output_formated_message != NULL &&
	  log->queue_formated_message != NULL &&
	  log->log != NULL){
      pthread_cond_wait(&(broker_wait_cond),
			&(broker_mutex));
    }

    pthread_mutex_unlock(&(broker_mutex));
  }
}

/**
 * ags_log_output:
 * @ptr an #AgsLog
 *
 * Writes the output_formated_message #GList to file #FILE, which are
 * members of #AgsLog.
 */
void*
ags_log_output(void *ptr)
{
  AgsLogFormatedMessage *formated_message;
  GList *list;
  
  log = AGS_LOG(ptr);

  while((AGS_LOG_RUNNING & (log->flags)) != 0){
    /* wait until output_formated_message has been set */
    pthread_mutex_lock(&(log->output_mutex));

    if((AGS_LOG_COPY_FROM_QUEUE_TO_OUTPUT & (log->flags)) != 0){
      log->flags |= AGS_LOG_OUTPUT_WAITS_FOR_QUEUE;
    }

    while((AGS_LOG_COPY_FROM_QUEUE_TO_OUTPUT & (log->flags)) != 0){
      pthread_cond_wait(&(log->output_wait_cond),
			&(log->output_mutex));
    }

    log->flags &= (~AGS_LOG_OUTPUT_WAITS_FOR_QUEUE);
    pthread_mutex_unlock(&(log->output_mutex));

    /* output output_formated_messages */
    list = log->output_formated_message;

    while(list != NULL){
      formated_message = (AgsLogFormatedMessage *) list->data;

      fwrite(formated_message->message, sizeof(char), formated_message->length, log->file);

      list = list->next;
    }

    /* free output_formated_message */
    ags_list_free_and_free_link(log->output_formated_message);

    /* wait until log_interval has exceeded */
    pthread_mutex_lock(&(log->output_mutex));
    log->output_formated_message = NULL;

    while((AGS_LOG_SUSPEND_OUTPUT & (log->flags)) != 0){
      pthread_cond_wait(&(log->output_wait_cond),
			&(log->output_mutex));
    }

    pthread_mutex_unlock(&(log->output_mutex));
  }
}

/**
 * ags_log_queue:
 * @ptr an #AgsLog
 *
 * Prints #AgsLogMessage to a string and puts it to queue_formated_message
 * #GList.
 */
void*
ags_log_queue(void *ptr)
{
  GList *list;
  AgsLogMessage *message;
  AgsLogFormatedMessage *formated_message;
  time_t time;
  struct tm *date;
  char *str;
  int printed_chars;

  log = AGS_LOG(ptr);

  while((AGS_LOG_RUNNING & (log->flags)) != 0){
    /* set output_formated_message */
    pthread_mutex_lock(&(log->queue_mutex));
    log->flags |= AGS_LOG_COPY_FROM_QUEUE_TO_OUTPUT;

    log->output = log->queue_formated_message;
    log->flags &= (~AGS_LOG_COPY_FROM_QUEUE_TO_OUTPUT);

    /* signal output_wait_cond if necessary */
    if((AGS_LOG_OUTPUT_WAITS_FOR_QUEUE & (log->flags)) != 0){
      pthread_mutex_unlock(&(log->queue_mutex));

      pthread_cond_signal(&(log->output_wait_cond));
    }else{
      pthread_mutex_unlock(&(log->queue_mutex));
    }

    log->queue_formated_message = NULL;

    /* wait until queue_formated_message has been set */
    pthread_mutex_lock(&(log->queue_mutex));

    if((AGS_LOG_COPY_FROM_LOG_TO_QUEUE & (log->flags)) != 0){
      log->flags |= AGS_LOG_QUEUE_WAITS_FOR_LOG;
    }

    while((AGS_LOG_COPY_FROM_LOG_TO_QUEUE & (log->flags)) != 0){
      pthread_cond_wait(&(log->queue_wait_cond),
			&(log->queue_mutex));
    }

    log->flags &= (~AGS_LOG_QUEUE_WAITS_FOR_LOG);
    pthread_mutex_unlock(&(log->queue_mutex));

    /* format queue_formated_message */
    list = log->queue_formated_message;

    while(list != NULL){
      message = (AgsLogMessage *) list->data;

      formated_message = (AgsLogFormatedMessage *) malloc(sizeof(AgsLogFormatedMessage));

      str =
	formated_message->message = (char *) malloc((AGS_LOG_MESSAGE_DATE_LENGTH + 5 + AGS_LOG_MESSAGE_LENGTH) * sizeof(char));

      time = message->time->tv_sec;
      date = localtime(time);
      strftime(str, AGS_LOG_MESSAGE_DATE_LENGTH * sizeof(char), "%Y-%m-%d %H:%M:%S\0", date);
      sprintf(&(str[AGS_LOG_MESSAGE_DATE_LENGTH]), ".%.3d \0", message->time->tv_nsec);

      printed_chars = vsnprintf(&(str[AGS_LOG_MESSAGE_DATE_LENGTH + 4]), AGS_LOG_MESSAGE_LENGTH * sizeof(char), message->format, message->args);

      if(printed_chars >= 0){
	formated_message->length = printed_chars + AGS_LOG_MESSAGE_DATE_LENGTH + 4;
      }else{
	formated_message->length = printed_chars - (AGS_LOG_MESSAGE_DATE_LENGTH + 4);
      }

      log->queue_formated_message = g_list_prepend(log->queue_formated_message,
						   formated_message);

      list = list->next;
    }

    /* wait until log_interval has exceeded */
    pthread_mutex_lock(&(log->queue_mutex));
    log->queue_formated_message = NULL;

    while((AGS_LOG_SUSPEND_QUEUE & (log->flags)) != 0){
      pthread_cond_wait(&(log->queue_wait_cond),
			&(log->queue_mutex));
    }

    pthread_mutex_unlock(&(log->queue_mutex));
  }
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
