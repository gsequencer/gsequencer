#ifndef __AGS_MACHINE_LINK_EDITOR_H__
#define __AGS_MACHINE_LINK_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_MACHINE_LINK_EDITOR                (ags_machine_link_editor_get_type())
#define AGS_MACHINE_LINK_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_LINK_EDITOR, AgsMachineLinkEditor))
#define AGS_MACHINE_LINK_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_LINK_EDITOR, AgsMachineLinkEditorClass))
#define AGS_IS_MACHINE_LINK_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_LINK_EDITOR))
#define AGS_IS_MACHINE_LINK_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_LINK_EDITOR))
#define AGS_MACHINE_LINK_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE_LINK_EDITOR, AgsMachineLinkEditorClass))

typedef struct _AgsMachineLinkEditor AgsMachineLinkEditor;
typedef struct _AgsMachineLinkEditorClass AgsMachineLinkEditorClass;

struct _AgsMachineLinkEditor
{
  GtkVBox vbox;

  GtkOptionMenu *input_link;
  GtkSpinButton *input_first_line;
  GtkSpinButton *input_first_link;
  GtkSpinButton *input_n;

  GtkOptionMenu *output_link;
  GtkSpinButton *output_first_line;
  GtkSpinButton *output_first_link;
  GtkSpinButton *output_n;
};

struct _AgsMachineLinkEditorClass
{
  GtkVBoxClass vbox;

  void (*reset_input)(AgsMachineLinkEditor *machine_link_editor);
  void (*reset_output)(AgsMachineLinkEditor *machine_link_editor);
};

GType ags_machine_link_editor_get_type();

void ags_machine_link_editor_reset_input(AgsMachineLinkEditor *machine_link_editor);
void ags_machine_link_editor_reset_output(AgsMachineLinkEditor *machine_link_editor);

void ags_machine_link_editor_apply(AgsMachineLinkEditor *machine_link_editor,
				   gboolean emit_signal);

AgsMachineLinkEditor* ags_machine_link_editor_new();

#endif /*__AGS_MACHINE_LINK_EDITOR_H__*/
