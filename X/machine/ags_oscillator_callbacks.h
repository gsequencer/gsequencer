#ifndef __AGS_OSCILLATOR_CALLBACKS_H__ 
#define __AGS_OSCILLATOR_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/machine/ags_oscillator.h>

void ags_oscillator_wave_callback(GtkComboBox *combo, AgsOscillator *oscillator);
void ags_oscillator_attack_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator);
void ags_oscillator_frame_count_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator);
void ags_oscillator_frequency_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator);
void ags_oscillator_phase_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator);
void ags_oscillator_volume_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator);

#endif /*__AGS_OSCILLATOR_CALLBACKS_H__ */
