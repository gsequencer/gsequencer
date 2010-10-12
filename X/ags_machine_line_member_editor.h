#ifndef __AGS_MACHINE_LINE_MEMBER_EDITOR_H__
#define __AGS_MACHINE_LINE_MEMBER_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_MACHINE_LINE_MEMBER_EDITOR                (ags_machine_line_member_editor_get_type())
#define AGS_MACHINE_LINE_MEMBER_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_LINE_MEMBER_EDITOR, AgsMachineLineMemberEditor))
#define AGS_MACHINE_LINE_MEMBER_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_LINE_MEMBER_EDITOR, AgsMachineLineMemberEditorClass))
#define AGS_IS_MACHINE_LINE_MEMBER_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_LINE_MEMBER_EDITOR))
#define AGS_IS_MACHINE_LINE_MEMBER_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_LINE_MEMBER_EDITOR))
#define AGS_MACHINE_LINE_MEMBER_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE_LINE_MEMBER_EDITOR, AgsMachineLineMemberEditorClass))

typedef struct _AgsMachineLineMemberEditor AgsMachineLineMemberEditor;
typedef struct _AgsMachineLineMemberEditorClass AgsMachineLineMemberEditorClass;

struct _AgsMachineLineMemberEditor
{
  GtkVBox vbox;
};

struct _AgsMachineLineMemberEditorClass
{
  GtkVBoxClass vbox;
};

GType ags_machine_line_member_editor_get_type();

void ags_machine_line_member_editor_apply(AgsMachineLineMemberEditor *machine_line_member_editor, gboolean emit_signal);

AgsMachineLineMemberEditor* ags_machine_line_member_editor_new();

#endif /*__AGS_MACHINE_LINE_MEMBER_EDITOR_H__*/
