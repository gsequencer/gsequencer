#include <ags/X/editor/ags_file_selection_callbacks.h>

void
ags_file_selection_remove_callback(GtkButton *button, GtkHBox *hbox)
{
  gtk_widget_destroy(hbox);
}
