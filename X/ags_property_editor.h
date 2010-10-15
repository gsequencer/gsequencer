#ifndef __AGS_PROPERTY_EDITOR_H__
#define __AGS_PROPERTY_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_PROPERTY_EDITOR                (ags_property_editor_get_type())
#define AGS_PROPERTY_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PROPERTY_EDITOR, AgsPropertyEditor))
#define AGS_PROPERTY_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PROPERTY_EDITOR, AgsPropertyEditorClass))
#define AGS_IS_PROPERTY_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PROPERTY_EDITOR))
#define AGS_IS_PROPERTY_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PROPERTY_EDITOR))
#define AGS_PROPERTY_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PROPERTY_EDITOR, AgsPropertyEditorClass))

typedef struct _AgsPropertyEditor AgsPropertyEditor;
typedef struct _AgsPropertyEditorClass AgsPropertyEditorClass;

typedef enum{
  AGS_PROPERTY_EDITOR_ENABLED   =  1,
}AgsPropertyEditorFlags;

struct _AgsPropertyEditor
{
  GtkVBox vbox;

  guint flags;

  GtkCheckButton *enabled;
};

struct _AgsPropertyEditorClass
{
  GtkVBoxClass vbox;

  void (*apply)(AgsPropertyEditor *property_editor);
};

GType ags_property_editor_get_type();

AgsPropertyEditor* ags_property_editor_new();

#endif /*__AGS_PROPERTY_EDITOR_H__*/

