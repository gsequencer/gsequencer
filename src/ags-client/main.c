#include <gtk/gtk.h>

#include <ags-client/X/ags_client_window.h>
#include <ags/object/ags_connectable.h>

int
main(int argc, char **argv)
{
  AgsClientWindow *client_window;

  gtk_init(&argc, &argv);

  client_window = ags_client_window_new();
  ags_connectable_connect(AGS_CONNECTABLE(client_window));
  gtk_widget_show_all(GTK_WIDGET(client_window));
  
  gtk_main();

  return(0);
}
