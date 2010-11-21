#include <ags/X/machine/ags_panel_callbacks.h>

#include <ags/X/ags_window.h>

void
ags_panel_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsPanel *panel)
{
  AgsWindow *window;

  if(old_parent != NULL)
    return;

  window = AGS_WINDOW(gtk_widget_get_toplevel(widget));
  panel->machine.audio->devout = (GObject *) window->devout;

  panel->machine.name = g_strdup_printf("Default %d\0", window->counter->panel);
  window->counter->panel++;
}

gboolean
ags_panel_destroy_callback(GtkObject *object, AgsPanel *panel)
{
  ags_panel_destroy(object);

  return(TRUE);
}
