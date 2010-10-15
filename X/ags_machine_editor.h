#ifndef __AGS_MACHINE_EDITOR_H__
#define __AGS_MACHINE_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_machine.h"
#include "ags_channel_listing_editor.h"
#include "ags_property_collection_editor.h"
#include "ags_channel_resize_editor.h"

#define AGS_TYPE_MACHINE_EDITOR                (ags_machine_editor_get_type())
#define AGS_MACHINE_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_EDITOR, AgsMachineEditor))
#define AGS_MACHINE_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_EDITOR, AgsMachineEditorClass))
#define AGS_IS_MACHINE_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_EDITOR))
#define AGS_IS_MACHINE_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_EDITOR))
#define AGS_MACHINE_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE_EDITOR, AgsMachineEditorClass))

typedef struct _AgsMachineEditor AgsMachineEditor;
typedef struct _AgsMachineEditorClass AgsMachineEditorClass;

struct _AgsMachineEditor
{
  GtkDialog dialog;

  AgsMachine *machine;

  GtkNotebook *notebook;

  GtkScrolledWindow *output_scrolled_window;
  AgsChannelListingEditor *output_editor;

  GtkScrolledWindow *input_scrolled_window;
  AgsChannelListingEditor *input_editor;

  GtkScrolledWindow *output_link_editor_scrolled_window;
  AgsPropertyCollectionEditor *output_link_editor;

  GtkScrolledWindow *input_link_editor_scrolled_window;
  AgsPropertyCollectionEditor *input_link_editor;

  GtkScrolledWindow *resize_editor_scrolled_window;
  AgsChannelResizeEditor *resize_editor;

  GtkButton *add;
  GtkButton *remove;
  GtkButton *apply;
  GtkButton *ok;
  GtkButton *cancel;
};

struct _AgsMachineEditorClass
{
  GtkDialogClass dialog;

  void (*set_machine)(AgsMachineEditor *machine_editor,
		      AgsMachine *machine);
};

GType ags_machine_editor_get_type(void);

void ags_machine_editor_add_children(AgsMachineEditor *machine_editor);

void ags_machine_editor_set_machine(AgsMachineEditor *machine_editor,
				    AgsMachine *machine);

void ags_machine_editor_add_input(AgsMachineEditor *machine_editor);
void ags_machine_editor_add_output(AgsMachineEditor *machine_editor);

void ags_machine_editor_remove_input(AgsMachineEditor *machine_editor);
void ags_machine_editor_remove_output(AgsMachineEditor *machine_editor);

AgsMachineEditor* ags_machine_editor_new(AgsMachine *machine);

#endif /*__AGS_MACHINE_EDITOR_H__*/
