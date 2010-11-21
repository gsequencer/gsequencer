#include <ags/X/ags_window_callbacks.h>

gboolean
ags_window_delete_event_callback(GtkWidget *widget, AgsWindow *window)
{
  ags_window_destroy(window);

  gtk_main_quit();

  return(TRUE);
}

gboolean
ags_window_destroy_callback(GtkObject *object, AgsWindow *window)
{
  ags_window_destroy(object);

  return(TRUE);
}

void
ags_window_show_callback(GtkWidget *widget, AgsWindow *window)
{
  ags_window_show(widget);
}
