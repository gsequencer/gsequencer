#ifndef __AGS_SCRIPT_EDITOR_H__
#define __AGS_SCRIPT_EDITOR_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_SCRIPT_EDITOR                (ags_script_editor_get_type())
#define AGS_SCRIPT_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCRIPT_EDITOR, AgsScriptEditor))
#define AGS_SCRIPT_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SCRIPT_EDITOR, AgsScriptEditorClass))
#define AGS_IS_SCRIPT_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SCRIPT_EDITOR))
#define AGS_IS_SCRIPT_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SCRIPT_EDITOR))
#define AGS_SCRIPT_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SCRIPT_EDITOR, AgsScriptEditorClass))

typedef struct _AgsScriptEditor AgsScriptEditor;
typedef struct _AgsScriptEditorClass AgsScriptEditorClass;

struct _AgsScriptEditor
{
  GtkTextView text_view;
};

struct _AgsScriptEditorClass
{
  GtkTextViewClass text_view;
};

GType ags_script_editor_get_type();

AgsScriptEditor* ags_script_editor_new();

#endif /*__AGS_SCRIPT_EDITOR_H__*/
