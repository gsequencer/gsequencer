#ifndef __AGS_PAD_EDITOR_H__
#define __AGS_PAD_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "../audio/ags_channel.h"

#define AGS_TYPE_PAD_EDITOR                (ags_pad_editor_get_type())
#define AGS_PAD_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PAD_EDITOR, AgsPadEditor))
#define AGS_PAD_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PAD_EDITOR, AgsPadEditorClass))
#define AGS_IS_PAD_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PAD_EDITOR))
#define AGS_IS_PAD_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PAD_EDITOR))
#define AGS_PAD_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PAD_EDITOR, AgsPadEditorClass))

typedef struct _AgsPadEditor AgsPadEditor;
typedef struct _AgsPadEditorClass AgsPadEditorClass;

struct _AgsPadEditor
{
  GtkVBox vbox;

  AgsChannel *pad;

  GList *line_editor;

  GtkHBox *hbox;
  GtkButton *button;
  GtkArrow *arrow;
  GtkLabel *label;
};

struct _AgsPadEditorClass
{
  GtkVBoxClass vbox;
};

AgsPadEditor* ags_pad_editor_new();

#endif /*__AGS_PAD_EDITOR_H__*/
