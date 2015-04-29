#include <ags/log/ags_log.h>

#include <ags/object/ags_application_context.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/log/task/ags_log_task.h>

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

enum{
  PROP_0,
  PROP_FILE,
  PROP_APPLICATION_CONTEXT,
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
  
  param_spec = g_param_spec_object("application-context\0",
				   "assigned application_context\0",
				   "The AgsApplicationContext it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);
}

void
ags_log_init(AgsLog *log)
{
  log->flags = 0;
  log->mode = AGS_LOG_WARN | AGS_LOG_ERROR;

  log->application_context = NULL;
  log->application_mutex = NULL;
  
  log->file = NULL;

  g_atomic_int_set(&(log->queued),
		   0);
  g_atomic_int_set(&(log->pending),
		   0);
  
  g_atomic_pointer_set(&(log->formated_message),
		       NULL);
  g_atomic_pointer_set(&(log->queued_formated_message),
		       NULL);
  
  log->log = NULL;
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
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = g_value_get_object(value);

      if(log->application_context == application_context)
	return;

      if(log->application_context != NULL){
	g_object_unref(log->application_context);
      }

      if(application_context != NULL){
	pthread_mutex_t *mutex;
	pthread_mutexattr_t attr;
	
	g_object_ref(application_context);

	/* set application mutex */
	log->application_mutex = application_context->mutex;

	/* add to mutex manager */
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr,
				  PTHREAD_MUTEX_RECURSIVE);
	
	mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex,
			   &attr);
	
	pthread_mutex_lock(log->application_mutex);
	
	mutex_manager = ags_mutex_manager_get_instance();
	ags_mutex_manager_insert(mutex_manager,
				 (GObject *) log,
				 mutex);
	
	pthread_mutex_unlock(&(log->application_mutex));
      }else{
	log->application_mutex = NULL;
      }

      log->application_context = application_context;
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
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, window->application_context);
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
  GList *list;

  log = AGS_LOG(gobject);

  /* close file */
  if(log->file != NULL){
    fclose(log->file);
  }
  
  if(log->application_context != NULL){
    /* remove of mutex manager */
    pthread_mutex_lock(log->application_mutex);
    
    mutex_manager = ags_mutex_manager_get_instance();
    
    ags_mutex_manager_remove(mutex_manager,
			     gobject);
    
    pthread_mutex_unlock(log->application_mutex);

    /* unref */
    g_object_unref(G_OBJECT(log->application_context));
  }

  /* free messages */
  if(log->log != NULL){
    g_list_free_full(log->log,
		     ags_log_formated_message_free);
  }
}

AgsLogMessage*
ags_log_message_alloc(struct timespec timestamp, char *format, va_list args)
{
  AgsLogMessage *log_message;

  /* alloc AgsLogMessage */
  log_message = (AgsLogMessage *) malloc(sizeof(AgsLogMessage));

  log_message->mode = 0;
  log_message->timestamp = timestamp;
  log_message->format = format;
  log_message->args = args;

  return(log_message);
}

void
ags_log_message_free(AgsLogMessage *log_message)
{
  free(log_message->timestamp);
  free(log_message->format);

  free(log_message);
}

AgsLogFormatedMessage*
ags_log_formated_message_alloc(guint length, char *message)
{
  AgsLogFormatedMessage *formated_message;

  /* alloc AgsLogMessage */
  formated_message = (AgsLogFormatedMessage *) malloc(sizeof(AgsLogFormatedMessage));

  formated_message->length = length;
  formated_message->message = message;
  
  return(formated_message);
}

void
ags_log_formated_message_free(AgsLogFormatedMessage *formated_message)
{
  free(formated_message->message);
  free(formated_message);
}

void
ags_log_debug(AgsLog *log, char *format, ...)
{
  AgsMutexManager *mutex_manager;

  AgsLogMessage *log_message;

  pthread_mutex_t *log_mutex;
  va_list args;

  /* lookup mutex */
  pthread_mutex_lock(log->application_mutex);

  mutex_manager = ags_mutex_manager_get_instance();
  log_mutex = ags_mutex_manager_lookup(mutex_manager,
				       (GObject *) log);
  
  pthread_mutex_unlock(log->application_mutex);
  
  /* add message to queue */
  pthread_mutex_lock(log_mutex);

  if((AGS_LOG_DEBUG & (log->mode)) != 0){
    struct timespec timestamp;
    va_list args_copy;
      
    va_start(args, format);
    va_copy(args_copy, args);
    va_end(args);

    timestamp = (struct timespec *) malloc(sizeof(struct timespec));
    clock_gettime(CLOCK_REALTIME, timestamp);
    
    log_message = ags_log_message_alloc(timestamp, format, args);
    log_message->mode = AGS_LOG_DEBUG;
    
    /* queue*/
    ags_log_add_queue(log, log_message);
  }else{
    /* discard */
    va_start(args, format);
    va_end(args);
  }

  pthread_mutex_unlock(log_mutex);
}

void
ags_log_message(AgsLog *log, char *format, ...)
{
  AgsMutexManager *mutex_manager;

  AgsLogMessage *log_message;

  pthread_mutex_t *log_mutex;
  va_list args;

  /* lookup mutex */
  pthread_mutex_lock(log->application_mutex);

  mutex_manager = ags_mutex_manager_get_instance();
  log_mutex = ags_mutex_manager_lookup(mutex_manager,
				       (GObject *) log);
  
  pthread_mutex_unlock(log->application_mutex);
  
  /* add message to queue */
  pthread_mutex_lock(log_mutex);

  if((AGS_LOG_DEBUG & (log->flags)) != 0){
    struct timespec timestamp;
    va_list args_copy;
      
    va_start(args, format);
    va_copy(args_copy, args);
    va_end(args);

    timestamp = (struct timespec *) malloc(sizeof(struct timespec));
    clock_gettime(CLOCK_REALTIME, timestamp);
    
    log_message = ags_log_message_alloc(timestamp, format, args);
    log_message->mode = AGS_LOG_MESSAGE;

    /* queue*/
    ags_log_add_queue(log, log_message);
  }else{
    /* discard */
    va_start(args, format);
    va_end(args);
  }

  pthread_mutex_unlock(log_mutex);
}

void
ags_log_warn(AgsLog *log, char *format, ...)
{
  AgsMutexManager *mutex_manager;

  AgsLogMessage *log_message;

  pthread_mutex_t *log_mutex;
  va_list args;

  /* lookup mutex */
  pthread_mutex_lock(log->application_mutex);

  mutex_manager = ags_mutex_manager_get_instance();
  log_mutex = ags_mutex_manager_lookup(mutex_manager,
				       (GObject *) log);
  
  pthread_mutex_unlock(log->application_mutex);
  
  /* add message to queue */
  pthread_mutex_lock(log_mutex);

  if((AGS_LOG_DEBUG & (log->flags)) != 0){
    struct timespec timestamp;
    va_list args_copy;
      
    va_start(args, format);
    va_copy(args_copy, args);
    va_end(args);

    timestamp = (struct timespec *) malloc(sizeof(struct timespec));
    clock_gettime(CLOCK_REALTIME, timestamp);
    
    log_message = ags_log_message_alloc(timestamp, format, args);
    log_message->mode = AGS_LOG_WARN;

    /* queue*/
    ags_log_add_queue(log, log_message);
  }else{
    /* discard */
    va_start(args, format);
    va_end(args);
  }

  pthread_mutex_unlock(log_mutex);
}

void
ags_log_error(AgsLog *log, char *format, ...)
{
  AgsMutexManager *mutex_manager;

  AgsLogMessage *log_message;

  pthread_mutex_t *log_mutex;
  va_list args;

  /* lookup mutex */
  pthread_mutex_lock(log->application_mutex);

  mutex_manager = ags_mutex_manager_get_instance();
  log_mutex = ags_mutex_manager_lookup(mutex_manager,
				       (GObject *) log);
  
  pthread_mutex_unlock(log->application_mutex);
  
  /* add message to queue */
  pthread_mutex_lock(log_mutex);

  if((AGS_LOG_DEBUG & (log->flags)) != 0){
    struct timespec timestamp;
    va_list args_copy;
      
    va_start(args, format);
    va_copy(args_copy, args);
    va_end(args);

    timestamp = (struct timespec *) malloc(sizeof(struct timespec));
    clock_gettime(CLOCK_REALTIME, timestamp);
    
    log_message = ags_log_message_alloc(timestamp, format, args);
    log_message->mode = AGS_LOG_ERROR;

    /* queue*/
    ags_log_add_queue(log, log_message);
  }else{
    /* discard */
    va_start(args, format);
    va_end(args);
  }

  pthread_mutex_unlock(log_mutex);
}

void
ags_log_add_queue(AgsLog *log, AgsLogMessage *log_message)
{
  AgsLogTask *log_task;

  AgsTaskThread *task_thread;

  task_thread = (AgsTaskThread *) ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(log->application_context));
  
  log_task = ags_log_task_new(log,
			      log_message);
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(log_task));
}

void
ags_log_output(AgsLog *log,
	       GError **error)
{
  GList *formated_message;
  guint pending;

  /* change log status */
  pthread_mutex_lock(log->mutex);
  
  formated_message = g_atomic_pointer_get(&(log->queued_formated_message));
  g_atomic_pointer_set(&(log->queued_formated_message),
		       NULL);
  g_atomic_pointer_set(&(log->formated_message),
		       formated_message);

  pending = g_atomic_int_get(&(log->pending));
  g_atomic_int_set(&(log->queued),
		   0);

  g_atomic_int_set(&(log->pending),
		   pending);

  pthread_mutex_unlock(log->mutex);

  /* output */
  while(formated_message != NULL){
    fprintf(log->file,
	    "%*s\0",
	    AGS_FORMATED_MESSAGE(formated_message->data)->length, AGS_FORMATED_MESSAGE(formated_message->data)->message);

    formated_message = formated_message->next;
  }

  /* reset log status */
  g_atomic_pointer_set(&(log->formated_message),
		       NULL);

  /* track or free */
  if((AGS_LOG_TRACK & (log->flags)) != 0){
    log->log = g_list_concat(formated_message,
			     log->log);
  }else{
    g_list_free_full(formated_message,
		     ags_log_formated_message_free);
  }
}

AgsLog*
ags_log_new(GObject *application_context)
{
  AgsLog *log;

  log = (AgsLog *) g_object_new(AGS_TYPE_LOG,
				"application-context\0", application_context,
				NULL);

  return(log);
}
