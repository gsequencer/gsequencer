#include "ags_line.h"
#include "ags_line_callbacks.h"

#include "../audio/ags_channel.h"

GType ags_line_get_type(void);
void ags_line_class_init(AgsLineClass *line);
void ags_line_init(AgsLine *line);
void ags_line_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_line_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_line_connect(AgsLine *line);
void ags_line_destroy(GtkObject *object);
void ags_line_show(GtkWidget *widget);

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_line_parent_class = NULL;

GType
ags_line_get_type(void)
{
  static GType ags_type_line = 0;

  if(!ags_type_line){
    static const GTypeInfo ags_line_info = {
      sizeof(AgsLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_line_init,
    };

    ags_type_line = g_type_register_static(GTK_TYPE_MENU_ITEM,
					   "AgsLine\0", &ags_line_info,
					   0);
  }

  return(ags_type_line);
}

void
ags_line_class_init(AgsLineClass *line)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_line_parent_class = g_type_class_peek_parent(line);

  gobject = G_OBJECT_CLASS(line);

  gobject->set_property = ags_line_set_property;
  gobject->get_property = ags_line_get_property;

  param_spec = g_param_spec_object("channel\0",
				   "assigned channel\0",
				   "The channel it is assigned with\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);
				  
}

void
ags_line_init(AgsLine *line)
{
  g_signal_connect_after((GObject *) line, "parent_set\0",
			 G_CALLBACK(ags_line_parent_set_callback), (gpointer) line);

  line->channel = NULL;

  line->table = (GtkTable *) gtk_table_new(2, 2, FALSE);
  gtk_container_add((GtkContainer *) line, (GtkWidget *) line->table);

  line->label = (GtkLabel *) gtk_label_new(NULL);
  gtk_table_attach(line->table,
		   (GtkWidget *) line->label,
		   0, 2,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);
}

void
ags_line_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsLine *line;

  fprintf(stdout, "ags_line_set_property\n\0");

  line = AGS_LINE(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      line->channel = channel;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsLine *line;

  fprintf(stdout, "ags_line_get_property\n\0");

  line = AGS_LINE(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    g_value_set_object(value, line->channel);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_connect(AgsLine *line)
{
  g_signal_connect((GObject *) line, "destroy\0",
		   G_CALLBACK(ags_line_destroy_callback), (gpointer) line);

  g_signal_connect((GObject *) line, "show\0",
		   G_CALLBACK(ags_line_show_callback), (gpointer) line);
}

void
ags_line_destroy(GtkObject *object)
{
  /* empty */
}

void
ags_line_show(GtkWidget *widget)
{
  /* empty */
}

AgsLine*
ags_line_new(AgsChannel *channel)
{
  AgsLine *line;

  line = (AgsLine *) g_object_new(AGS_TYPE_LINE,
				  "channel", channel,
				  NULL);

  return(line);
}
