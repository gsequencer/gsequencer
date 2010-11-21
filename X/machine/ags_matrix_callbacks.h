#ifndef __AGS_MATRIX_CALLBACKS_H__
#define __AGS_MATRIX_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/machine/ags_matrix.h>

void ags_matrix_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMatrix *matrix);
gboolean ags_matrix_destroy_callback(GtkObject *object, AgsMatrix *matrix);

void ags_matrix_run_callback(GtkWidget *widget, AgsMatrix *matrix);

void ags_matrix_index_callback(GtkWidget *widget, AgsMatrix *matrix);
gboolean ags_matrix_drawing_area_expose_callback(GtkWidget *widget, GdkEventExpose *event, AgsMatrix *matrix);
gboolean ags_matrix_drawing_area_button_press_callback(GtkWidget *widget, GdkEventButton *event, AgsMatrix *matrix);
void ags_matrix_adjustment_value_changed_callback(GtkWidget *widget, AgsMatrix *matrix);

void ags_matrix_bpm_callback(GtkWidget *spin_button, AgsMatrix *matrix);
void ags_matrix_length_spin_callback(GtkWidget *spin_button, AgsMatrix *matrix);
void ags_matrix_tact_callback(GtkWidget *option_menu, AgsMatrix *matrix);
void ags_matrix_loop_button_callback(GtkWidget *button, AgsMatrix *matrix);

void ags_matrix_run_delay_done(AgsRecall *recall, AgsMatrix *matrix);

void ags_matrix_copy_pattern_done(AgsRecall *recall, AgsMatrix *matrix);
void ags_matrix_copy_pattern_cancel(AgsRecall *recall, AgsMatrix *matrix);
void ags_matrix_copy_pattern_loop(AgsRecall *recall, AgsMatrix *matrix);

void ags_matrix_play_done(AgsRecall *recall, AgsMatrix *matrix);
void ags_matrix_play_cancel(AgsRecall *recall, AgsMatrix *matrix);

#endif /*__AGS_MATRIX_CALLBACKS_H__*/
