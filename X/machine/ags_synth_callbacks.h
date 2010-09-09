#ifndef __AGS_SYNTH_CALLBACKS_H__ 
#define __AGS_SYNTH_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_synth.h"

void ags_synth_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsSynth *synth);
gboolean ags_synth_destroy_callback(GtkObject *object, AgsSynth *synth);

void ags_synth_lower_callback(GtkSpinButton *spin_button, AgsSynth *synth);

void ags_synth_auto_update_callback(GtkToggleButton *toggle, AgsSynth *synth);
void ags_synth_update_callback(GtkButton *button, AgsSynth *synth);

#endif /*__AGS_SYNTH_CALLBACKS_H__ */
