#ifndef __AGS_DRUM_CALLBACKS_H__
#define __AGS_DRUM_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_drum.h"

void ags_drum_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsDrum *drum);
gboolean ags_drum_destroy_callback(GtkObject *object, AgsDrum *drum);
void ags_drum_show_callback(GtkWidget *widget, AgsDrum *drum);

void ags_drum_bpm_callback(GtkWidget *spin_button, AgsDrum *drum); // AgsNavigation->bpm

void ags_drum_open_callback(GtkWidget *toggle_button, AgsDrum *drum);

void ags_drum_loop_button_callback(GtkWidget *button, AgsDrum *drum);
void ags_drum_run_callback(GtkWidget *toggle_button, AgsDrum *drum);

void ags_drum_run_delay_done(AgsRecall *recall, AgsRecallID *recall_id, AgsDrum *drum);

void ags_drum_tic_callback(GtkWidget *option_menu, AgsDrum *drum);
void ags_drum_length_spin_callback(GtkWidget *spin_button, AgsDrum *drum);

void ags_drum_index0_callback(GtkWidget *toggle_button, AgsDrum *drum);
void ags_drum_index1_callback(GtkWidget *toggle_button, AgsDrum *drum);

void ags_drum_pad_callback(GtkWidget *toggle_button, AgsDrum *drum);

void ags_drum_offset_callback(GtkWidget *widget, AgsDrum *drum);

#endif /*__AGS_DRUM_CALLBACKS_H__*/
