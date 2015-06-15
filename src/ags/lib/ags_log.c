#include <ags/lib/ags_log.h>

#include <ags/lib/ags_list.h>

#include <time.h>
#include <stdlib.h>
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

void* ags_log_timer(void *args);
void* ags_log_broker(void *ptr);
void* ags_log_output(void *ptr);
void* ags_log_queue(void *ptr);

void ags_log_start_threads(AgsLog *log);

AgsLogMessage* ags_log_message_alloc();
void* ags_log_add_message_thread(void *ptr);
void ags_log_add_message();

enum{
  PROP_0,
  PROP_FILE,
};

struct _AgsLogAddMessageArgs
{
  AgsLog *log;
  AgsLogMessage *log_message;
  pthread_mutex_t mutex;
};

static gpointer ags_log_parent_class = NULL;
AgsLog *ags_default_log = NULL;

#define AGS_LOG_MESSAGE_LENGTH 4096
#define AGS_LOG_MESSAGE_DATE_LENGTH 19

GType
ags_log_get_type (void)
{
  static GType ags_type_log = 0;

  if(!ags_type_log){
    static const GTypeInfo ags_log_info = {
      sizeof(AgsLogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_log_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_log_init,
    };

    ags_type_log = g_type_register_static(G_TYPE_OBJECT,
					    "AgsLog\0", &ags_log_info,
					    0);
  }

  return(ags_type_log);
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
  pthread_cond_init(&(log->start_wait_cond), NULL);

  /* timer thread related members */  
  log->log_interval = (struct timespec *) malloc(sizeof(struct timespec));
  log->log_interval->tv_sec = 1;
  log->log_interval->tv_nsec = 0;// 10000000; /* sleep 10 mili seconds */

  pthread_mutexattr_init(&(log->timer_mutex_attr));
  pthread_mutex_init(&(log->timer_mutex), &(log->broker_mutex_attr));

  log->timer_started = FALSE;
  log->timer_active = FALSE;
  log->timer_awaken = FALSE;

  pthread_cond_init(&(log->timer_wait_cond), NULL);

  /* broker thread related members */
  pthread_mutexattr_init(&(log->broker_mutex_attr));
  pthread_mutex_init(&(log->broker_mutex), &(log->broker_mutex_attr));

  log->broker_started = FALSE;
  log->broker_active = FALSE;
  log->broker_awaken = FALSE;

  pthread_cond_init(&(log->broker_wait_cond), NULL);

  /* output thread related members */
  log->output_formated_message = NULL;

  pthread_mutexattr_init(&(log->output_mutex_attr));
  pthread_mutex_init(&(log->output_mutex), &(log->output_mutex_attr));

  log->output_started = FALSE;
  log->output_active = FALSE;
  log->output_awaken = FALSE;

  pthread_cond_init(&(log->output_wait_cond), NULL);

  /* queue thread related members */
  log->queue_formated_message = NULL;
  log->queue_message = NULL;

  pthread_mutexattr_init(&(log->queue_mutex_attr));
  pthread_mutex_init(&(log->queue_mutex), &(log->queue_mutex_attr));

  log->queue_started = FALSE;
  log->queue_active = FALSE;
  log->queue_awaken = FALSE;

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
 * ags_log_timer:
 * @ptr an #AgsLog
 *
 * Sleeps specified amount of time and signals broker to wake up as long as
 * log is running.
 */
void*
ags_log_timer(void *ptr)
{
  AgsLog *log;

  log = AGS_LOG(ptr);
  log->timer_active = FALSE;
  log->signaled_timer = FALSE;

  printf("timer - start\n\0");
  goto ags_log_timer_SUSPEND;

  while((AGS_LOG_RUNNING & (log->flags)) != 0){
    printf("timer - loop\n\0");

    /*  */
    pthread_mutex_lock(&(log->timer_mutex));
    log->timer_active = TRUE;
    pthread_mutex_unlock(&(log->timer_mutex));

    /* run timer */
    //FIXME:JK: doesn't test if it's behindhand, free_float member is reserved for this issue
    //    nanosleep(log->log_interval, NULL);

    /*  */
    pthread_mutex_lock(&(log->timer_mutex));
    log->timer_active = FALSE;
    pthread_mutex_unlock(&(log->timer_mutex));

    /* keep in sync with broker */
  ags_log_timer_SUSPEND:
    if((AGS_LOG_STARTING & (log->flags)) != 0){
      log->timer_started = TRUE;
      pthread_cond_signal(&(log->start_wait_cond));
    }

    if(!log->broker_active){
      printf("  timer:0\n\0");
      pthread_cond_signal(&(log->broker_wait_cond));

      pthread_mutex_lock(&(log->timer_mutex));
      log->timer_awaken = FALSE;
      
      while((AGS_LOG_TIMER_SLEEP & (log->flags)) != 0 ||
	    !log->broker_active ||
	    !log->signaled_timer){
	pthread_cond_wait(&(log->timer_wait_cond),
			  &(log->timer_mutex));
      }
    }else{
      printf("  timer:1\n\0");
      pthread_mutex_lock(&(log->timer_mutex));
      log->timer_awaken = FALSE;
      
      while((AGS_LOG_TIMER_SLEEP & (log->flags)) != 0){
	pthread_cond_wait(&(log->timer_wait_cond),
			  &(log->timer_mutex));
      }
      
    }

    log->flags |= AGS_LOG_TIMER_SLEEP;  
    log->timer_awaken = TRUE;
    log->signaled_timer = FALSE;
    pthread_mutex_unlock(&(log->timer_mutex));
    printf("  timer:2\n\0");
    pthread_cond_signal(&(log->broker_wait_cond));
  }
  
  pthread_exit(NULL);
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

  log = AGS_LOG(ptr);

  log->flags |= AGS_LOG_TIMER_SLEEP;

  log->signaled_timer =
    log->signaled_output =
    log->signaled_queue =
    log->signaled_log = TRUE;
  printf("broker - start\n\0");

  goto ags_log_broker_SUSPEND;

  /* start broker */
  while((AGS_LOG_RUNNING & (log->flags)) != 0){
    printf("broker - loop\n\0");

    //    log->flags &= ~(AGS_LOG_SUSPEND);

    log->broker_active = TRUE;


    /* wake up queue thread and wait for it * /
    log->signaled_queue = TRUE;
    pthread_cond_signal(&(log->queue_wait_cond));

    pthread_mutex_lock(&(log->broker_mutex));

    while(!log->queue_awaken ||
    ){
      pthread_cond_wait(&(log->broker_wait_cond),
			&(log->broker_mutex));
    }

    pthread_mutex_unlock(&(log->broker_mutex));

    /* wake up all waiting log requests and wait for them * /
    pthread_mutex_lock(&(log->broker_mutex));

    while(log->active_logs != 0){
      if(!signaled_log){
	pthread_mutex_unlock(&(log->broker_mutex));
 	pthread_cond_broadcast(&(log->log_wait_cond));
	pthread_mutex_lock(&(log->broker_mutex));
	
	signaled_log = TRUE;
      }

      pthread_cond_wait(&(log->broker_wait_cond),
			&(log->broker_mutex));
    }

    pthread_mutex_unlock(&(log->broker_mutex));
 
    //    printf("broker - log\n\0");

    /* wake up output thread and wait for it * /
    log->signaled_output = TRUE;
    pthread_cond_signal(&(log->output_wait_cond));

    pthread_mutex_lock(&(log->broker_mutex));

    while(!log->output_awaken){
      pthread_cond_wait(&(log->broker_wait_cond),
			&(log->broker_mutex));
    }

    pthread_mutex_unlock(&(log->broker_mutex));

    /* wake up timer thread and wait for it */
    pthread_mutex_lock(&(log->broker_mutex));

    if(!log->timer_active){
      pthread_mutex_unlock(&(log->broker_mutex));

      printf("  broker:0\n\0");
      log->signaled_timer = TRUE;
      pthread_cond_signal(&(log->timer_wait_cond));

      pthread_mutex_lock(&(log->broker_mutex));
      
      while(!log->timer_awaken){
	pthread_cond_wait(&(log->broker_wait_cond),
			  &(log->broker_mutex));
      }
    
      printf("  broker:1\n\0");
      //      pthread_mutex_unlock(&(log->broker_mutex));
    }else{
      pthread_mutex_lock(&(log->broker_mutex));
    }

    /* reset status and entry point */
  ags_log_broker_SUSPEND:
    if((AGS_LOG_STARTING & (log->flags)) != 0){
      log->broker_started = TRUE;

      ags_log_start_threads(log);

      pthread_cond_signal(&(log->start_wait_cond));
    }

    log->broker_active = FALSE;

    /* wait log_interval amount of time */
    printf("  broker:2\n\0");

    while(log->timer_active ||
	  log->output_active ||
	  log->queue_active ||
	  log->active_logs != 0){

      //TODO:JK: may be you want set AGS_LOG_TIMER_SLEEP here for better throughput

      /* sleep until all threads are done */
      pthread_cond_wait(&(log->broker_wait_cond),
			&(log->broker_mutex));

      /* set timer to sleep */
      if(!log->timer_active){
	log->flags |= AGS_LOG_TIMER_SLEEP;
      }

      /* set output to sleep */
      if(!log->output_active){
	log->flags |= AGS_LOG_OUTPUT_SUSPEND;
      }

      /* set queue to sleep */
      if(!log->queue_active){
	log->flags |= AGS_LOG_QUEUE_SUSPEND;
      }

      /* set log to sleep */
      if(log->active_logs == 0){
	log->flags |= AGS_LOG_SUSPEND;
      }
    }

    log->broker_awaken = TRUE;
    printf("  broker:3\n\0");
    pthread_mutex_unlock(&(log->broker_mutex));
  }


  pthread_exit(NULL);
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
  AgsLog *log;
  AgsLogFormatedMessage *formated_message;
  GList *list;
  
  log = AGS_LOG(ptr);

  goto ags_log_output_SUSPEND;

  while((AGS_LOG_RUNNING & (log->flags)) != 0){
    printf("output - loop\n\0");

    /* wait until output_formated_message has been set * /
    pthread_mutex_lock(&(log->output_mutex));

    if((AGS_LOG_COPY_FROM_QUEUE_TO_OUTPUT & (log->flags)) != 0){
      log->flags |= AGS_LOG_OUTPUT_WAITS_FOR_QUEUE;
    }

    while((AGS_LOG_COPY_FROM_QUEUE_TO_OUTPUT & (log->flags)) != 0){
      pthread_cond_wait(&(log->output_wait_cond),
			&(log->output_mutex));
    }

    log->flags &= (~AGS_LOG_OUTPUT_WAITS_FOR_QUEUE);
    log->output_active = TRUE;

    pthread_mutex_unlock(&(log->output_mutex));

    /* output output_formated_messages */
    //FIXME:JK: set AGS_LOG_OUTPUT_SUSPEND
    list = log->output_formated_message;

    while(list != NULL){
      formated_message = (AgsLogFormatedMessage *) list->data;

      fwrite(formated_message->message, sizeof(char), formated_message->length, log->file);

      list = list->next;
    }

    /* free output_formated_message */
    ags_list_free_and_free_link(log->output_formated_message);
    log->output_formated_message = NULL;
    log->output_active = FALSE;

    /* wait until log_interval has exceeded */
  ags_log_output_SUSPEND:
    if((AGS_LOG_STARTING & (log->flags)) != 0){
      log->output_started = TRUE;
      pthread_cond_signal(&(log->start_wait_cond));
    }

    pthread_mutex_lock(&(log->output_mutex));
    log->output_active = FALSE;

    pthread_mutex_unlock(&(log->output_mutex));
    pthread_cond_signal(&(log->broker_wait_cond));

    pthread_mutex_lock(&(log->output_mutex));
    log->output_awaken = FALSE;

    while((AGS_LOG_OUTPUT_SUSPEND & (log->flags)) != 0 ||
	  !log->broker_active ||
	  !log->signaled_output){
      pthread_cond_wait(&(log->output_wait_cond),
			&(log->output_mutex));
    }

    log->output_awaken = TRUE;
    log->signaled_output = FALSE;
    log->flags |= AGS_LOG_OUTPUT_SUSPEND;
    pthread_mutex_unlock(&(log->output_mutex));
    pthread_cond_signal(&(log->broker_wait_cond));
  }

  pthread_exit(NULL);
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
  AgsLog *log;
  GList *list;
  AgsLogMessage *message;
  AgsLogFormatedMessage *formated_message;
  time_t time;
  struct tm *date;
  char *str;
  int printed_chars;

  log = AGS_LOG(ptr);
  log->queue_active = FALSE;

  goto ags_log_queue_SUSPEND;

  while((AGS_LOG_RUNNING & (log->flags)) != 0){
    printf("queue - loop\n\0");

    /* set output_formated_message * /
    pthread_mutex_lock(&(log->queue_mutex));
    log->flags |= AGS_LOG_COPY_FROM_QUEUE_TO_OUTPUT;

    log->output_formated_message = log->queue_formated_message;
    log->flags &= (~AGS_LOG_COPY_FROM_QUEUE_TO_OUTPUT);

    /* signal output_wait_cond if necessary * /
    if((AGS_LOG_OUTPUT_WAITS_FOR_QUEUE & (log->flags)) != 0){
      pthread_mutex_unlock(&(log->queue_mutex));

      pthread_cond_signal(&(log->output_wait_cond));
    }else{
      pthread_mutex_unlock(&(log->queue_mutex));
    }

    log->queue_formated_message = NULL;

    /* format queue_formated_message */
    pthread_mutex_lock(&(log->queue_mutex));
    log->queue_active = TRUE;
    pthread_mutex_unlock(&(log->queue_mutex));

    list = log->queue_message;
    log->queue_message = NULL;

    while(list != NULL){
      message = (AgsLogMessage *) list->data;

      formated_message = (AgsLogFormatedMessage *) malloc(sizeof(AgsLogFormatedMessage));

      str =
	formated_message->message = (char *) malloc((AGS_LOG_MESSAGE_DATE_LENGTH + 5 + AGS_LOG_MESSAGE_LENGTH) * sizeof(char));

      time = message->time->tv_sec;
      date = localtime(&time);
      strftime(str, AGS_LOG_MESSAGE_DATE_LENGTH * sizeof(char), "%Y-%m-%d %H:%M:%S\0", date);
      sprintf(&(str[AGS_LOG_MESSAGE_DATE_LENGTH]), ".%.3lld \0", (long long int) message->time->tv_nsec);

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

    log->queue_formated_message = NULL;

    /* wait until log_interval has exceeded */
  ags_log_queue_SUSPEND:
    if((AGS_LOG_STARTING & (log->flags)) != 0){
      log->queue_started = TRUE;
      pthread_cond_signal(&(log->start_wait_cond));
    }

    pthread_mutex_lock(&(log->queue_mutex));
    log->queue_active = FALSE;

    pthread_mutex_unlock(&(log->queue_mutex));
    pthread_cond_signal(&(log->broker_wait_cond));

    pthread_mutex_lock(&(log->queue_mutex));
    log->queue_awaken = FALSE;

    while((AGS_LOG_QUEUE_SUSPEND & (log->flags)) != 0 ||
	  !log->broker_active ||
	  !log->signaled_queue){
      pthread_cond_wait(&(log->queue_wait_cond),
			&(log->queue_mutex));
    }

    log->queue_awaken = TRUE;
    log->signaled_queue = FALSE;
    log->flags |= AGS_LOG_QUEUE_SUSPEND;
    pthread_mutex_unlock(&(log->queue_mutex));
    pthread_cond_signal(&(log->broker_wait_cond));
  }


  pthread_exit(NULL);
}

void
ags_log_start_threads(AgsLog *log){
  /* */
  pthread_create(&(log->timer_thread), NULL,
		 &ags_log_timer, log);

  pthread_mutex_lock(&(log->start_mutex));
  
  while(!log->timer_started){
    pthread_cond_wait(&(log->start_wait_cond),
		      &(log->start_mutex));
  }

  pthread_mutex_unlock(&(log->start_mutex));

  /**/
  log->queue_started = FALSE;
  pthread_create(&(log->queue_thread), NULL,
		 &ags_log_queue, log);

  pthread_mutex_lock(&(log->start_mutex));

  while(!log->queue_started){
    pthread_cond_wait(&(log->start_wait_cond),
		      &(log->start_mutex));
  }

  pthread_mutex_unlock(&(log->start_mutex));

  /**/
  log->output_started = FALSE;
  pthread_create(&(log->output_thread), NULL,
		 &ags_log_output, log);

  pthread_mutex_lock(&(log->start_mutex));

  while(!log->output_started){
    pthread_cond_wait(&(log->start_wait_cond),
		      &(log->start_mutex));
  }

  pthread_mutex_unlock(&(log->start_mutex));
}

void
ags_log_start_queue(AgsLog *log)
{
  pthread_mutex_t start_mutex;
  pthread_mutexattr_t start_mutex_attr;

  log->flags |= AGS_LOG_STARTING;
 /*		 AGS_LOG_RUNNING |
		 AGS_LOG_QUEUE_SUSPEND |
		 AGS_LOG_OUTPUT_SUSPEND);
  pthread_mutexattr_init(&(start_mutex_attr));
  pthread_mutex_init(&(start_mutex), &(start_mutex_attr));
  */

  /* */
  /*
  log->broker_started = FALSE;
  log->timer_started = FALSE;
  */

  pthread_create(&(log->broker_thread), NULL,
		 &ags_log_broker, log);

  pthread_mutex_lock(&(start_mutex));
  
  while(!log->broker_started){
    pthread_cond_wait(&(log->start_wait_cond),
		      &start_mutex);
  }

  log->flags &= ~(AGS_LOG_STARTING);
  pthread_mutex_unlock(&(start_mutex));
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

void*
ags_log_add_message_thread(void *ptr)
{
  struct _AgsLogAddMessageArgs *args;
  AgsLog *log;
  AgsLogMessage *log_message;
  pthread_mutex_t mutex;

  args = (struct _AgsLogAddMessageArgs *) ptr;

  log = args->log;
  log_message = args->log_message;
  mutex = args->mutex;

  free(args);

  log->suspended_logs += 1;
  
  while((AGS_LOG_SUSPEND & (log->flags)) != 0){
    pthread_cond_wait(&(log->log_wait_cond),
		      &mutex);
  }

  if(log->suspended_logs == 0){
    log->flags |= AGS_LOG_SUSPEND;
  }
  
  log->active_logs += 1;
  log->log = g_list_prepend(log->log,
			    log_message);
  
  log->active_logs -= 1;
  log->suspended_logs -= 1;
    
  pthread_mutex_unlock(&mutex);
  
  pthread_cond_signal(&(log->broker_wait_cond));

  pthread_exit(NULL);
}

void
ags_log_add_message(AgsLog *log, AgsLogMessage *log_message, pthread_mutex_t mutex)
{
  struct _AgsLogAddMessageArgs *args;
  pthread_t log_thread;

  if(log->log != NULL){
    log->queue_message = log->log;
  }

  args = (struct _AgsLogAddMessageArgs *) malloc(sizeof(struct _AgsLogAddMessageArgs));

  args->log = log;
  args->log_message = log_message;
  args->mutex = mutex;

  pthread_create(&(log_thread), NULL,
		 &ags_log_add_message_thread, args);
}

void
ags_log_debug(AgsLog *log, char *format, ...)
{
  AgsLogMessage *log_debug;
  pthread_mutex_t log_mutex;
  pthread_mutexattr_t log_mutex_attr;
  va_list args;

  if((AGS_LOG_RUNNING & (log->flags)) == 0 ||
     (AGS_LOG_OMMIT_DEBUG) != 0){
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
    log->log = g_list_prepend(log->log, log_debug);
    log->active_logs -= 1;

    pthread_mutex_unlock(&(log_mutex));

    pthread_cond_signal(&(log->broker_wait_cond));
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

  if((AGS_LOG_RUNNING & (log->flags)) == 0){
    return;
  }

  log_message = ags_log_message_alloc();

  log_message->debug = FALSE;
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

    pthread_cond_signal(&(log->broker_wait_cond));
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
