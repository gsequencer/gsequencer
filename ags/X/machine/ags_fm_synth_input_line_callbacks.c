/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/machine/ags_fm_synth_input_line_callbacks.h>
#include <ags/X/machine/ags_fm_synth.h>

void
ags_fm_synth_input_line_samplerate_changed_callback(AgsLine *line,
						    guint samplerate, guint old_samplerate,
						    gpointer user_data)
{
  AgsFMOscillator *fm_oscillator;

  guint i;
  
  fm_oscillator = AGS_FM_SYNTH_INPUT_LINE(line)->fm_oscillator;
  
  gtk_spin_button_set_value(fm_oscillator->attack,
			    samplerate * (gtk_spin_button_get_value(fm_oscillator->attack) / old_samplerate));

  gtk_spin_button_set_value(fm_oscillator->frame_count,
			    samplerate * (gtk_spin_button_get_value(fm_oscillator->frame_count) / old_samplerate));
  
  gtk_spin_button_set_value(fm_oscillator->phase,
			    samplerate * (gtk_spin_button_get_value(fm_oscillator->phase) / old_samplerate));

  for(i = 0; i < fm_oscillator->sync_point_count; i++){
    gtk_spin_button_set_value(fm_oscillator->sync_point[i * 2],
			      samplerate * (gtk_spin_button_get_value(fm_oscillator->sync_point[i * 2]) / old_samplerate));
			      
    gtk_spin_button_set_value(fm_oscillator->sync_point[i * 2 + 1],
			      samplerate * (gtk_spin_button_get_value(fm_oscillator->sync_point[i * 2 + 1]) / old_samplerate));
  }
}

void
ags_fm_synth_input_line_fm_oscillator_control_changed_callback(AgsFMOscillator *fm_oscillator,
							       AgsFMSynthInputLine *fm_synth_input_line)
{
  AgsFMSynth *fm_synth;

  fm_synth = (AgsFMSynth *) gtk_widget_get_ancestor((GtkWidget *) fm_synth_input_line,
						    AGS_TYPE_FM_SYNTH);

  if((AGS_FM_SYNTH_AUTO_UPDATE & (fm_synth->flags)) != 0){
    ags_fm_synth_update(fm_synth);
  }
}
