#include "ags_pad_editor_callbacks.h"

#include "../audio/ags_channel.h"

#include "ags_machine.h"
#include "ags_machine_editor.h"
#include "ags_line_editor.h"

int
ags_pad_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsPadEditor *pad_editor)
{
  return(0);
}

int
ags_pad_editor_destroy_callback(GtkObject *object, AgsPadEditor *pad_editor)
{
  ags_pad_editor_destroy(object);
  return(0);
}

int
ags_pad_editor_show_callback(GtkWidget *widget, AgsPadEditor *pad_editor)
{
  ags_pad_editor_show(widget);
  return(0);
}
