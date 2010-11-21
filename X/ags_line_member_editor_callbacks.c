#include <ags/X/ags_line_member_editor_callbacks.h>

int
ags_line_member_editor_destroy_callback(GtkObject *object, AgsLineMemberEditor *line_member_editor)
{
  ags_line_member_editor_destroy(object);
  return(0);
}

int
ags_line_member_editor_show_callback(GtkWidget *widget, AgsLineMemberEditor *line_member_editor)
{
  ags_line_member_editor_show(widget);
  return(0);
}
