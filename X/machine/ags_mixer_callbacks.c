#include <ags/X/machine/ags_mixer_callbacks.h>

#include <ags/X/ags_window.h>

void
ags_mixer_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMixer *mixer)
{
  AgsWindow *window;

  if(old_parent != NULL)
    return;

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);
  mixer->machine.name = g_strdup_printf("Default %d\0", window->counter->mixer);
  window->counter->mixer++;
}

gboolean
ags_mixer_destroy_callback(GtkObject *object, AgsMixer *mixer)
{
  ags_mixer_destroy(object);

  return(TRUE);
}
