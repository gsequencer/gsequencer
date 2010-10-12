#ifndef __AGS_MACHINE_PROPERTY_EDITOR_H__
#define __AGS_MACHINE_PROPERTY_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_machine.h"

#define AGS_TYPE_MACHINE_PROPERTY_EDITOR                (ags_machine_property_editor_get_type())
#define AGS_MACHINE_PROPERTY_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_PROPERTY_EDITOR, AgsMachinePropertyEditor))
#define AGS_MACHINE_PROPERTY_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_PROPERTY_EDITOR, AgsMachinePropertyEditorClass))
#define AGS_IS_MACHINE_PROPERTY_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_PROPERTY_EDITOR))
#define AGS_IS_MACHINE_PROPERTY_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_PROPERTY_EDITOR))
#define AGS_MACHINE_PROPERTY_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE_PROPERTY_EDITOR, AgsMachinePropertyEditorClass))

typedef struct _AgsMachinePropertyEditor AgsMachinePropertyEditor;
typedef struct _AgsMachinePropertyEditorClass AgsMachinePropertyEditorClass;

struct _AgsMachinePropertyEditor
{
  GtkVBox vbox;

  GType type;
};

struct _AgsMachinePropertyEditorClass
{
  GtkVBoxClass vbox;
};

GType ags_machine_property_editor_get_type();

void ags_machine_property_editor_update(AgsMachinePropertyEditor *machine_property_editor);
void ags_machine_property_editor_apply(AgsMachinePropertyEditor *machine_property_editor);

AgsMachinePropertyEditor* ags_machine_property_editor_new(GType type);

#endif /*__AGS_MACHINE_PROPERTY_EDITOR_H__*/
