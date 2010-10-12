#ifndef __AGS_LINEMEMBEREDITOR_H__
#define __AGS_LINEMEMBEREDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_LINE_MEMBER_EDITOR                (ags_line_member_editor_get_type())
#define AGS_LINE_MEMBER_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LINE_MEMBER_EDITOR, AgsLineMemberEditor))
#define AGS_LINE_MEMBER_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LINE_MEMBER_EDITOR, AgsLineMemberEditorClass))
#define AGS_IS_LINE_MEMBER_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LINE_MEMBER_EDITOR))
#define AGS_IS_LINE_MEMBER_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LINE_MEMBER_EDITOR))
#define AGS_LINE_MEMBER_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LINE_MEMBER_EDITOR, AgsLineMemberEditorClass))

typedef struct _AgsLineMemberEditor AgsLineMemberEditor;
typedef struct _AgsLineMemberEditorClass AgsLineMemberEditorClass;

struct _AgsLineMemberEditor
{
  GtkVBox vbox;

  GtkVBox *line_member;

  GtkButton *add;
  GtkButton *remove;
};

struct _AgsLineMemberEditorClass
{
  GtkVBoxClass vbox;
};

GType ags_line_member_editor_get_type(void);

AgsLineMemberEditor* ags_line_member_editor_new();

#endif /*__AGS_LINE_MEMBER_EDITOR_H__*/
