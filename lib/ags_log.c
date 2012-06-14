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
  log->file = NULL;

  
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

AgsLog*
ags_log_new()
{
  AgsLog *log;

  log = (AgsLog *) g_object_new(AGS_TYPE_LOG, NULL);

  return(log);
}
