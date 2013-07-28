#include <gtk/gtk.h>

#include <ags-client/scripting/ags_client_window.h>

int
main(int argc, char **argv)
{
  AgsClientWindow *client_window;

  gtk_init(&argc, &argv);

  client_window = ags_client_window_new();
  gtk_widget_show_all(GTK_WIDGET(client_window));
  
  gtk_main();

  return(0);
}
