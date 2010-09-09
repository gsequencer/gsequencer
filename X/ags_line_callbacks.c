#include "ags_line_callbacks.h"

int
ags_line_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLine *line)
{
  if(old_parent == NULL)
    gtk_label_set_label(line->label, g_strdup_printf("line %d\0", line->channel->audio_channel));
}

int
ags_line_destroy_callback(GtkObject *object, AgsLine *line)
{
  ags_line_destroy(object);
}

int
ags_line_show_callback(GtkWidget *widget, AgsLine *line)
{
  ags_line_show(widget);
}
