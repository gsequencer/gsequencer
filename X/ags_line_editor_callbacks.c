#include <ags/X/ags_line_editor_callbacks.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_pad_editor.h>
#include <ags/X/ags_machine_editor.h>

int
ags_line_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLineEditor *line_editor)
{
  if(old_parent != NULL)
    return(0);

  gtk_box_pack_start((GtkBox *) line_editor,
		     (GtkWidget *) line_editor->link_editor,
		     FALSE, FALSE, 0);
  return(0);
}

int
ags_line_editor_destroy_callback(GtkObject *object, AgsLineEditor *line_editor)
{
  ags_line_editor_destroy(object);

  return(0);
}

int
ags_line_editor_show_callback(GtkWidget *widget, AgsLineEditor *line_editor)
{
  ags_line_editor_show(widget);

  return(0);
}
