#include "ags_panel_callbacks.h"

#include "../ags_window.h"

void
ags_panel_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsPanel *panel)
{
  AgsWindow *window;

  if(old_parent != NULL)
    return;

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);
  panel->machine.name = g_strdup_printf("Default %d\0", window->counter->panel);
  window->counter->panel++;
}

gboolean
ags_panel_destroy_callback(GtkObject *object, AgsPanel *panel)
{
  ags_panel_destroy(object);

  return(TRUE);
}
