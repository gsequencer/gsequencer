#ifndef __AGS_DRUM_INPUT_PAD_CALLBACKS_H__
#define __AGS_DRUM_INPUT_PAD_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/machine/ags_drum_input_pad.h>

void ags_drum_input_pad_option_callback(GtkOptionMenu *option, AgsDrumInputPad *drum_input_pad);
void ags_drum_input_pad_open_callback(GtkWidget *button, AgsDrumInputPad *drum_input_pad);
void ags_drum_input_pad_play_callback(GtkToggleButton *button, AgsDrumInputPad *drum_input_pad);
void ags_drum_input_pad_play_done_callback(AgsRecall *recall, AgsDrumInputPad *drum_input_pad);
void ags_drum_input_pad_edit_callback(GtkWidget *toggle_button, AgsDrumInputPad *drum_input_pad);

#endif /*__AGS_DRUM_INPUT_PAD_CALLBACKS_H__*/
