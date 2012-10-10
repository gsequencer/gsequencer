#include <ags/X/editor/ags_file_selection_callbacks.h>

void
ags_file_selection_remove_callback(GtkButton *button, GtkTable *table)
{
  AgsFileSelection *file_selection;

  file_selection = (AgsFileSelection *) gtk_widget_get_ancestor(GTK_WIDGET(table),
								AGS_TYPE_FILE_SELECTION);

  ags_file_selection_remove_entry(file_selection, GTK_WIDGET(table));
}
