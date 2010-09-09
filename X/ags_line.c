#include "ags_line.h"
#include "ags_line_callbacks.h"

#include "../audio/ags_channel.h"

GType ags_line_get_type(void);
void ags_line_class_init(AgsLineClass *line);
void ags_line_init(AgsLine *line);
void ags_line_connect(AgsLine *line);
void ags_line_destroy(GtkObject *object);
void ags_line_show(GtkWidget *widget);

GType
ags_line_get_type(void)
{
  static GType line_type = 0;

  if (!line_type){
    static const GtkTypeInfo line_info = {
      "AgsLine\0",
      sizeof(AgsLine), /* base_init */
      sizeof(AgsLineClass), /* base_finalize */
      (GtkClassInitFunc) ags_line_class_init,
      (GtkObjectInitFunc) ags_line_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    line_type = gtk_ (GTK_TYPE_MENU_ITEM, &line_info);
  }

  return (line_type);
}

void
ags_line_class_init(AgsLineClass *line)
{
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
}

void
ags_line_show(GtkWidget *widget)
{
}

AgsLine*
ags_line_new()
{
  AgsLine *line;

  line = (AgsLine *) g_object_new(AGS_TYPE_LINE, NULL);
  
  return(line);
}
