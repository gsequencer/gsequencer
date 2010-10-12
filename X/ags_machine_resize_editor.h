#ifndef __AGS_MACHINE_RESIZE_EDITOR_H__
#define __AGS_MACHINE_RESIZE_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_MACHINE_RESIZE_EDITOR                (ags_machine_resize_editor_get_type())
#define AGS_MACHINE_RESIZE_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_RESIZE_EDITOR, AgsMachineResizeEditor))
#define AGS_MACHINE_RESIZE_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_RESIZE_EDITOR, AgsMachineResizeEditorClass))
#define AGS_IS_MACHINE_RESIZE_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_RESIZE_EDITOR))
#define AGS_IS_MACHINE_RESIZE_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_RESIZE_EDITOR))
#define AGS_MACHINE_RESIZE_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE_RESIZE_EDITOR, AgsMachineResizeEditorClass))

typedef struct _AgsMachineResizeEditor AgsMachineResizeEditor;
typedef struct _AgsMachineResizeEditorClass AgsMachineResizeEditorClass;

struct _AgsMachineResizeEditor
{
  GtkVBox vbox;

  GtkSpinButton *audio_channels;
  GtkSpinButton *output_pads;
  GtkSpinButton *input_pads;
};

struct _AgsMachineResizeEditorClass
{
  GtkVBoxClass vbox;

  void (*resize_audio_channels)(AgsMachineResizeEditor *machine_resize_editor,
				guint audio_channels, guint audio_channels_old);

  void (*resize_input_pads)(AgsMachineResizeEditor *machine_resize_editor,
			    guint pads, guint pads_old);
  void (*resize_output_pads)(AgsMachineResizeEditor *machine_resize_editor,
			     guint pads, guint pads_old);
};

GType ags_machine_resize_editor_get_type();

void ags_machine_resize_editor_resize_audio_channels(AgsMachineResizeEditor *machine_resize_editor,
						     guint audio_channels, guint audio_channels_old);

void ags_machine_resize_editor_resize_input_pads(AgsMachineResizeEditor *machine_resize_editor,
						 guint pads, guint pads_old);
void ags_machine_resize_editor_resize_output_pads(AgsMachineResizeEditor *machine_resize_editor,
						  guint pads, guint pads_old);

void ags_machine_resize_editor_apply(AgsMachineResizeEditor *machine_resize_editor,
				     gboolean emit_signal);

AgsMachineResizeEditor* ags_machine_resize_editor_new();

#endif /*__AGS_MACHINE_RESIZE_EDITOR_H__*/
