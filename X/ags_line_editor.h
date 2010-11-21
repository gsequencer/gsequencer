#ifndef __AGS_LINE_EDITOR_H__
#define __AGS_LINE_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_link_editor.h>

#define AGS_TYPE_LINE_EDITOR                (ags_line_editor_get_type())
#define AGS_LINE_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LINE_EDITOR, AgsLineEditor))
#define AGS_LINE_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LINE_EDITOR, AgsLineEditorClass))
#define AGS_IS_LINE_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LINE_EDITOR))
#define AGS_IS_LINE_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LINE_EDITOR))
#define AGS_LINE_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LINE_EDITOR, AgsLineEditorClass))

typedef struct _AgsLineEditor AgsLineEditor;
typedef struct _AgsLineEditorClass AgsLineEditorClass;

struct _AgsLineEditor
{
  GtkVBox vbox;

  AgsChannel *channel;

  AgsLinkEditor *link_editor;
};

struct _AgsLineEditorClass
{
  GtkVBoxClass vbox;
};

GType ags_line_editor_get_type(void);

void ags_line_editor_set_channel(AgsLineEditor *line_editor,
				 AgsChannel *channel);

AgsLineEditor* ags_line_editor_new(AgsChannel *channel);

#endif /*__AGS_LINE_EDITOR_H__*/
