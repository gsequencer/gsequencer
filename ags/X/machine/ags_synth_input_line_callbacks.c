/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/X/machine/ags_synth_input_line_callbacks.h>
#include <ags/X/machine/ags_synth.h>

void
ags_synth_input_line_samplerate_changed_callback(AgsLine *line,
						 guint samplerate, guint old_samplerate,
						 gpointer user_data)
{
  AgsOscillator *oscillator;

  guint i;
  
  oscillator = AGS_SYNTH_INPUT_LINE(line)->oscillator;
  
  gtk_spin_button_set_value(oscillator->attack,
			    samplerate * (gtk_spin_button_get_value(oscillator->attack) / old_samplerate));

  gtk_spin_button_set_value(oscillator->frame_count,
			    samplerate * (gtk_spin_button_get_value(oscillator->frame_count) / old_samplerate));
  
  gtk_spin_button_set_value(oscillator->phase,
			    samplerate * (gtk_spin_button_get_value(oscillator->phase) / old_samplerate));

  for(i = 0; i < oscillator->sync_point_count; i++){
    gtk_spin_button_set_value(oscillator->sync_point[i * 2],
			      samplerate * (gtk_spin_button_get_value(oscillator->sync_point[i * 2]) / old_samplerate));
			      
    gtk_spin_button_set_value(oscillator->sync_point[i * 2 + 1],
			      samplerate * (gtk_spin_button_get_value(oscillator->sync_point[i * 2 + 1]) / old_samplerate));
  }
}

void
ags_synth_input_line_oscillator_control_changed_callback(AgsOscillator *oscillator,
							 AgsSynthInputLine *synth_input_line)
{
  AgsSynth *synth;

  synth = (AgsSynth *) gtk_widget_get_ancestor((GtkWidget *) synth_input_line,
					       AGS_TYPE_SYNTH);

  if(ags_synth_test_flags(synth, AGS_SYNTH_AUTO_UPDATE)){
    ags_synth_update(synth);
  }
}
