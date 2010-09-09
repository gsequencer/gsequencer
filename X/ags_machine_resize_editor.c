#include "ags_machine_resize_editor.h"
#include "ags_machine_resize_editor_callbacks.h"

#include "../object/ags_marshal.h"

#include "ags_machine_editor.h"

#include "../audio/ags_audio.h"
#include "../audio/ags_channel.h"
#include "../audio/ags_output.h"
#include "../audio/ags_input.h"

GType ags_machine_resize_editor_get_type();
void ags_machine_resize_editor_class_init(AgsMachineResizeEditorClass *machine_resize_editor);
void ags_machine_resize_editor_init(AgsMachineResizeEditor *machine_resize_editor);
void ags_machine_resize_editor_destroy(GtkObject *object);
void ags_machine_resize_editor_connect(AgsMachineResizeEditor *machine_resize_editor);

void ags_machine_resize_editor_real_resize_audio_channels(AgsMachineResizeEditor *machine_resize_editor,
							  guint audio_channels, guint audio_channels_old);

void ags_machine_resize_editor_real_resize_input_pads(AgsMachineResizeEditor *machine_resize_editor,
						      guint pads, guint pads_old);
void ags_machine_resize_editor_real_resize_output_pads(AgsMachineResizeEditor *machine_resize_editor,
						       guint pads, guint pads_old);

enum{
  RESIZE_AUDIO_CHANNELS,
  RESIZE_INPUT_PADS,
  RESIZE_OUTPUT_PADS,
  LAST_SIGNAL,
};

static guint machine_resize_editor_signals[LAST_SIGNAL];

GType
ags_machine_resize_editor_get_type()
{
  static GType machine_resize_editor_type = 0;

  if (!machine_resize_editor_type){
    static const GtkTypeInfo machine_resize_editor_info = {
      "AgsMachineResizeEditor\0",
      sizeof(AgsMachineResizeEditor), /* base_init */
      sizeof(AgsMachineResizeEditorClass), /* base_finalize */
      (GtkClassInitFunc) ags_machine_resize_editor_class_init,
      (GtkObjectInitFunc) ags_machine_resize_editor_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    machine_resize_editor_type = gtk_type_unique (GTK_TYPE_VBOX, &machine_resize_editor_info);
  }

  return (machine_resize_editor_type);
}

void
ags_machine_resize_editor_class_init(AgsMachineResizeEditorClass *machine_resize_editor)
{
  machine_resize_editor->resize_audio_channels = ags_machine_resize_editor_real_resize_audio_channels;

  machine_resize_editor->resize_input_pads = ags_machine_resize_editor_real_resize_input_pads;
  machine_resize_editor->resize_output_pads = ags_machine_resize_editor_real_resize_output_pads;

  machine_resize_editor_signals[RESIZE_AUDIO_CHANNELS] =
    g_signal_new("resize_audio_channels\0",
		 G_TYPE_FROM_CLASS (machine_resize_editor),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsMachineResizeEditorClass, resize_audio_channels),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  machine_resize_editor_signals[RESIZE_INPUT_PADS] =
    g_signal_new("resize_input_pads\0",
		 G_TYPE_FROM_CLASS (machine_resize_editor),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsMachineResizeEditorClass, resize_input_pads),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  machine_resize_editor_signals[RESIZE_OUTPUT_PADS] =
    g_signal_new("resize_output_pads\0",
		 G_TYPE_FROM_CLASS (machine_resize_editor),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsMachineResizeEditorClass, resize_output_pads),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);
}

void
ags_machine_resize_editor_init(AgsMachineResizeEditor *machine_resize_editor)
{
  GtkHBox *hbox;
  GtkLabel *label;

  g_signal_connect_after((GObject *) machine_resize_editor, "parent_set\0",
			 G_CALLBACK(ags_machine_resize_editor_parent_set_callback), machine_resize_editor);

  /* */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) machine_resize_editor, (GtkWidget *) hbox, FALSE, FALSE, 0);

  label = (GtkLabel *) gtk_label_new(g_strdup("audio channels\0"));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 0);

  machine_resize_editor->audio_channels = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1024.0, 1.0);
  //  spin->adjustment->value = (gdouble) machine_editor->machine->audio->audio_channels;
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) machine_resize_editor->audio_channels, FALSE, FALSE, 0);

  /* */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) machine_resize_editor, (GtkWidget *) hbox, FALSE, FALSE, 0);

  label = (GtkLabel *) gtk_label_new(g_strdup("output pads\0"));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 0);

  machine_resize_editor->output_pads = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1024.0, 1.0);
  //  spin->adjustment->value = (gdouble) machine_editor->machine->audio->output_pads;
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) machine_resize_editor->output_pads, FALSE, FALSE, 0);

  /* */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) machine_resize_editor, (GtkWidget *) hbox, FALSE, FALSE, 0);

  label = (GtkLabel *) gtk_label_new(g_strdup("input pads\0"));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 0);

  machine_resize_editor->input_pads = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1024.0, 1.0);
  //  spin->adjustment->value = (gdouble) machine_editor->machine->audio->input_pads;
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) machine_resize_editor->input_pads, FALSE, FALSE, 0);
}

void
ags_machine_resize_editor_destroy(GtkObject *object)
{
}

void
ags_machine_resize_editor_connect(AgsMachineResizeEditor *machine_resize_editor)
{
  AgsMachineEditor *machine_editor;

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_resize_editor, AGS_TYPE_MACHINE_EDITOR);

  /* AgsMachineEditor */
  g_signal_connect_after((GObject *) machine_editor, "add_input\0",
			 G_CALLBACK(ags_machine_resize_editor_add_input_callback), machine_resize_editor);

  g_signal_connect_after((GObject *) machine_editor, "add_output\0",
			 G_CALLBACK(ags_machine_resize_editor_add_output_callback), machine_resize_editor);

  g_signal_connect_after((GObject *) machine_editor, "remove_input\0",
			 G_CALLBACK(ags_machine_resize_editor_remove_input_callback), machine_resize_editor);

  g_signal_connect_after((GObject *) machine_editor, "remove_output\0",
			 G_CALLBACK(ags_machine_resize_editor_remove_output_callback), machine_resize_editor);
}

void
ags_machine_resize_editor_real_resize_audio_channels(AgsMachineResizeEditor *machine_resize_editor,
						     guint audio_channels, guint audio_channels_old)
{
  AgsMachineEditor *machine_editor;

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_resize_editor,
								AGS_TYPE_MACHINE_EDITOR);
  ags_audio_set_audio_channels(machine_editor->machine->audio,
			       audio_channels);
}

void
ags_machine_resize_editor_resize_audio_channels(AgsMachineResizeEditor *machine_resize_editor,
						guint audio_channels, guint audio_channels_old)
{
  g_return_if_fail(AGS_IS_MACHINE_RESIZE_EDITOR(machine_resize_editor));

  g_object_ref((GObject *) machine_resize_editor);
  g_signal_emit(G_OBJECT(machine_resize_editor),
		machine_resize_editor_signals[RESIZE_AUDIO_CHANNELS], 0,
		audio_channels, audio_channels_old);
  g_object_unref((GObject *) machine_resize_editor);
}

void
ags_machine_resize_editor_real_resize_input_pads(AgsMachineResizeEditor *machine_resize_editor,
					    guint pads, guint pads_old)
{
  AgsMachineEditor *machine_editor;

  fprintf(stdout, "ags_machine_resize_editor_resize_input_pads\n\0");

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_resize_editor,
								AGS_TYPE_MACHINE_EDITOR);
  ags_audio_set_pads(machine_editor->machine->audio,
		     AGS_TYPE_INPUT, pads);
}

void
ags_machine_resize_editor_resize_input_pads(AgsMachineResizeEditor *machine_resize_editor,
					    guint pads, guint pads_old)
{
  g_signal_emit(G_OBJECT(machine_resize_editor),
		machine_resize_editor_signals[RESIZE_INPUT_PADS], 0,
		pads, pads_old);
}

void
ags_machine_resize_editor_real_resize_output_pads(AgsMachineResizeEditor *machine_resize_editor,
					     guint pads, guint pads_old)
{
  AgsMachineEditor *machine_editor;

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_resize_editor,
								AGS_TYPE_MACHINE_EDITOR);

  ags_audio_set_pads(machine_editor->machine->audio,
		     AGS_TYPE_OUTPUT, pads);
}

void
ags_machine_resize_editor_resize_output_pads(AgsMachineResizeEditor *machine_resize_editor,
					     guint pads, guint pads_old)
{
  g_return_if_fail(AGS_IS_MACHINE_RESIZE_EDITOR(machine_resize_editor));

  g_object_ref((GObject *) machine_resize_editor);
  g_signal_emit(G_OBJECT(machine_resize_editor),
		machine_resize_editor_signals[RESIZE_OUTPUT_PADS], 0,
		pads, pads_old);
  g_object_unref((GObject *) machine_resize_editor);
}

void
ags_machine_resize_editor_apply(AgsMachineResizeEditor *machine_resize_editor,
				gboolean emit_signal)
{
  AgsMachineEditor *machine_editor;

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_resize_editor, AGS_TYPE_MACHINE_EDITOR);

  if(emit_signal){
    g_signal_emit_by_name((GObject *) machine_resize_editor, "resize_audio_channels\0", (guint) machine_resize_editor->audio_channels->adjustment->value, machine_editor->machine->audio->audio_channels);
    g_signal_emit_by_name((GObject *) machine_resize_editor, "resize_input_pads\0", (guint) machine_resize_editor->input_pads->adjustment->value, machine_editor->machine->audio->input_pads);
    g_signal_emit_by_name((GObject *) machine_resize_editor, "resize_output_pads\0", (guint) machine_resize_editor->output_pads->adjustment->value, machine_editor->machine->audio->output_pads);
  }else{
    AGS_MACHINE_RESIZE_EDITOR_GET_CLASS(machine_resize_editor)->resize_audio_channels(machine_resize_editor, (guint) machine_resize_editor->audio_channels->adjustment->value, machine_editor->machine->audio->audio_channels);
    AGS_MACHINE_RESIZE_EDITOR_GET_CLASS(machine_resize_editor)->resize_input_pads(machine_resize_editor, (guint) machine_resize_editor->input_pads->adjustment->value, machine_editor->machine->audio->input_pads);
    AGS_MACHINE_RESIZE_EDITOR_GET_CLASS(machine_resize_editor)->resize_output_pads(machine_resize_editor, (guint) machine_resize_editor->output_pads->adjustment->value, machine_editor->machine->audio->output_pads);
  }
}

AgsMachineResizeEditor*
ags_machine_resize_editor_new()
{
  AgsMachineResizeEditor *machine_resize_editor;

  machine_resize_editor = (AgsMachineResizeEditor *) g_object_new(AGS_TYPE_MACHINE_RESIZE_EDITOR, NULL);

  return(machine_resize_editor);
}
