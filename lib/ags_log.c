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

AgsLogMessage* ags_log_message_alloc();
void ags_log_add_message();

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

  /* broker thread related members */
  pthread_mutexattr_init(&(log->broker_mutex_attr));
  pthread_mutex_init(&(log->broker_mutex), &(log->broker_mutex_attr));
  pthread_cond_init(&(log->broker_wait_cond), NULL);

  /* output thread related members */
  log->output_formated_message = NULL;

  pthread_mutexattr_init(&(log->output_mutex_attr));
  pthread_mutex_init(&(log->output_mutex), &(log->output_mutex_attr));
  pthread_cond_init(&(log->output_wait_cond), NULL);

  /* queue thread related members */
  log->queue_formated_message = NULL;
  log->queue_message = NULL;

  pthread_mutexattr_init(&(log->queue_mutex_attr));
  pthread_mutex_init(&(log->queue_mutex), &(log->queue_mutex_attr));
  pthread_cond_init(&(log->queue_wait_cond), NULL);

  /* log messages */
  log->log = NULL;
  log->active_logs = 0;
  log->suspended_logs = 0;
  pthread_cond_init(&(log->log_wait_cond), NULL);
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
  gboolean time_elapsed;

  auto void* ags_log_broker_sleep();

  void* ags_log_broker_sleep(){
    //FIXME:JK: doesn't test if it's behindhand, free_float member is reserved for this issue
    nanosleep(log->log_interval);

    pthread_mutex_lock(&(log->broker_mutex));
    time_elapsed = TRUE;
    pthread_mutex_unlock(&(log->broker_mutex));
    pthread_cond_signal(&(log->broker_wait_cond));
  }

  log = AGS_LOG(ptr);

  goto ags_log_broker_AFTER_SIGNAL;

  /* start broker */
  while((AGS_LOG_RUNNING & (log->flags)) != 0){
    time_elapsed = FALSE;
    pthread_create(&(sleep_thread), NULL,
		   &ags_log_broker_sleep, NULL);

    log->flags &= ~(AGS_LOG_SUSPEND_OUTPUT |
		    AGS_LOG_SUSPEND_QUEUE |
		    AGS_LOG_SUSPEND_LOG);

    /* wake up all waiting log requests and wait for them */
    pthread_cond_broadcast(&(log->log_wait_cond));

    pthread_mutex_lock(&(log->broker_mutex));

    while(log->suspended_logs != 0){
      pthread_cond_wait(&(log->broker_wait_cond),
			&(log->broker_mutex));
    }

    pthread_mutex_unlock(&(log->broker_mutex));

    /* wake up queue thread and wait for it */
    pthread_cond_signal(&(log->queue_wait_cond));

    pthread_mutex_lock(&(log->broker_mutex));

    while(!log->queue_active){
      pthread_cond_wait(&(log->broker_wait_cond),
			&(log->broker_mutex));
    }

    pthread_mutex_unlock(&(log->broker_mutex));

    /* wake up output thread and wait for it */
    pthread_cond_signal(&(log->output_wait_cond));

    pthread_mutex_lock(&(log->broker_mutex));

    while(!log->output_active){
      pthread_cond_wait(&(log->broker_wait_cond),
			&(log->broker_mutex));
    }

    pthread_mutex_unlock(&(log->broker_mutex));

    /* wait log_interval amount of time */
    pthread_mutex_lock(&(log->broker_mutex));

  ags_log_broker_AFTER_SIGNAL:

    while(!time_elapsed &&
	  log->output_formated_message != NULL &&
	  log->queue_formated_message != NULL &&
	  log->log != NULL){
      pthread_cond_wait(&(log->broker_wait_cond),
			&(log->broker_mutex));
    }

    pthread_mutex_unlock(&(log->broker_mutex));
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

  goto ags_log_output_SUSPEND;

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
  ags_log_output_SUSPEND:

    pthread_mutex_lock(&(log->output_mutex));
    log->flags |= AGS_LOG_SUSPEND_OUTPUT;
    log->output_active = FALSE;
    log->output_formated_message = NULL;

    while((AGS_LOG_SUSPEND_OUTPUT & (log->flags)) != 0){
      pthread_cond_wait(&(log->output_wait_cond),
			&(log->output_mutex));
    }

    log->output_active = TRUE;
    pthread_mutex_unlock(&(log->output_mutex));

    /* wake up broker */
    pthread_cond_signal(&(log->broker_wait_cond));
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

  goto ags_log_queue_SUSPEND;

  while((AGS_LOG_RUNNING & (log->flags)) != 0){
    /* set output_formated_message */
    pthread_mutex_lock(&(log->queue_mutex));
    log->flags |= AGS_LOG_COPY_FROM_QUEUE_TO_OUTPUT;

    log->output_formated_message = log->queue_formated_message;
    log->flags &= (~AGS_LOG_COPY_FROM_QUEUE_TO_OUTPUT);

    /* signal output_wait_cond if necessary */
    if((AGS_LOG_OUTPUT_WAITS_FOR_QUEUE & (log->flags)) != 0){
      pthread_mutex_unlock(&(log->queue_mutex));

      pthread_cond_signal(&(log->output_wait_cond));
    }else{
      pthread_mutex_unlock(&(log->queue_mutex));
    }

    log->queue_formated_message = NULL;

    /* format queue_formated_message */
    list = log->queue_message;
    log->queue_message = NULL;

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

      formated_message->message[formated_message->length] = '\0';

      log->queue_formated_message = g_list_prepend(log->queue_formated_message,
						   formated_message);

      list = list->next;
    }

    /* wait until log_interval has exceeded */
  ags_log_queue_SUSPEND:

    pthread_mutex_lock(&(log->queue_mutex));
    log->flags |= AGS_LOG_SUSPEND_QUEUE;
    log->queue_active = FALSE;
    log->queue_formated_message = NULL;

    while((AGS_LOG_SUSPEND_QUEUE & (log->flags)) != 0){
      pthread_cond_wait(&(log->queue_wait_cond),
			&(log->queue_mutex));
    }

    log->queue_active = TRUE;
    pthread_mutex_unlock(&(log->queue_mutex));

    /* wake up broker */
    pthread_cond_signal(&(log->broker_wait_cond));
  }
}

void
ags_log_start_queue(AgsLog *log)
{
  log->flags |= (AGS_LOG_RUNNING |
		 AGS_LOG_SUSPEND_OUTPUT |
		 AGS_LOG_SUSPEND_QUEUE |
		 AGS_LOG_SUSPEND_LOG);

  pthread_create(&(log->queue_thread), NULL,
		 &ags_log_queue, log);
  pthread_create(&(log->output_thread), NULL,
		 &ags_log_output, log);
  pthread_create(&(log->broker_thread), NULL,
		 &ags_log_broker, log);
}

void
ags_log_stop_queue(AgsLog *log)
{
  log->flags &= (~AGS_LOG_RUNNING);
}

AgsLogMessage*
ags_log_message_alloc()
{
  AgsLogMessage *log_message;

  /* alloc AgsLogMessage */
  log_message = (AgsLogMessage *) malloc(sizeof(AgsLogMessage));

  /* set timestamp */
  log_message->time = (struct timespec *) malloc(sizeof(struct timespec));
  clock_gettime(CLOCK_REALTIME, log_message->time);

  return(log_message);
}

void
ags_log_add_message(AgsLog *log, AgsLogMessage *log_message, pthread_mutex_t mutex)
{
  pthread_t log_thread;

  auto void* ags_log_add_message_thread(void *ptr);

  void* ags_log_add_message_thread(void *ptr){
    log->suspended_logs += 1;

    while((AGS_LOG_SUSPEND_LOG & (log->flags)) != 0){
      pthread_cond_wait(&(log_message->log_wait_cond),
			&mutex);
    }

    log->active_logs += 1;
    log->log = g_list_prepend(log->log,
			      log_message);

    log->active_logs -= 1;
    log->suspended_logs -= 1;
    
    pthread_mutex_unlock(&mutex);

    pthread_cond_signal(&(log->broker_wait_cond));
  }

  if(log->log != NULL){
    log->queue_message = log->log;
  }

  pthread_create(&(log_thread), NULL,
		 &ags_log_add_message_thread, NULL);
}

void
ags_log_debug(AgsLog *log, char *format, ...)
{
  AgsLogMessage *log_debug;
  pthread_mutex_t log_mutex;
  pthread_mutexattr_t log_mutex_attr;
  va_list args;

  if((AGS_LOG_OMMIT_DEBUG) != 0){
    return;
  }
  
  log_debug = ags_log_message_alloc();

  log_debug->debug = TRUE;
  log_debug->format = format;

  va_start(args, format);
  va_copy(log_debug->args, args);
  va_end(args);

  /* add to queue */
  pthread_mutexattr_init(&(log_mutex_attr));
  pthread_mutex_init(&(log_mutex), &(log_mutex_attr));

  pthread_mutex_lock(&(log_mutex));

  if((AGS_LOG_SUSPEND & (log->flags)) == 0){
    log->active_logs += 1;
    log->log = g_list_prepend(log->log, log_message);
    log->active_logs -= 1;

    pthread_mutex_unlock(&(log_mutex));
  }else{
    ags_log_add_message(log, log_debug, log_mutex);
  }
}

void
ags_log_message(AgsLog *log, char *format, ...)
{
  AgsLogMessage *log_message;
  pthread_mutex_t log_mutex;
  pthread_mutexattr_t log_mutex_attr;
  va_list args;
  
  log_message = ags_log_message_alloc();

  log_debug->debug = FALSE;
  log_message->format = format;

  va_start(args, format);
  va_copy(log_message->args, args);
  va_end(args);

  /* add to queue */
  pthread_mutexattr_init(&(log_mutex_attr));
  pthread_mutex_init(&(log_mutex), &(log_mutex_attr));

  pthread_mutex_lock(&(log_mutex));

  if((AGS_LOG_SUSPEND & (log->flags)) == 0){
    log->active_logs += 1;
    log->log = g_list_prepend(log->log, log_message);
    log->active_logs -= 1;

    pthread_mutex_unlock(&(log_mutex));
  }else{
    ags_log_add_message(log, log_message, log_mutex);
  }
}

AgsLog*
ags_log_new()
{
  AgsLog *log;

  log = (AgsLog *) g_object_new(AGS_TYPE_LOG, NULL);

  return(log);
}
