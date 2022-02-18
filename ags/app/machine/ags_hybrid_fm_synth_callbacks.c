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

#include <ags/app/machine/ags_hybrid_fm_synth_callbacks.h>

#include <ags/app/ags_window.h>

#include <math.h>

void
ags_hybrid_fm_synth_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_HYBRID_FM_SYNTH)->counter);

  g_object_set(AGS_MACHINE(hybrid_fm_synth),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_HYBRID_FM_SYNTH);

  g_free(str);
}

void
ags_hybrid_fm_synth_synth_0_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  oscillator = gtk_combo_box_get_active(combo_box);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) oscillator);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_0_octave_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble octave;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  octave = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-octave", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) octave);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_0_key_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble key;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  key = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-key", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) key);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_0_phase_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble phase;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  phase = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-phase", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) phase);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_0_volume_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) volume);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_0_lfo_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint lfo_oscillator;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  lfo_oscillator = gtk_combo_box_get_active(combo_box);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_oscillator);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_0_lfo_frequency_callback(GtkSpinButton *spin_button, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_frequency;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_frequency);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_0_lfo_depth_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_depth;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  lfo_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_depth);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_0_lfo_tuning_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_tuning;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  lfo_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_tuning);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_1_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  oscillator = gtk_combo_box_get_active(combo_box);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) oscillator);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_1_octave_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble octave;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  octave = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-octave", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) octave);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_1_key_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble key;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  key = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-key", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) key);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_1_phase_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble phase;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  phase = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-phase", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) phase);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_1_volume_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) volume);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_1_lfo_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint lfo_oscillator;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  lfo_oscillator = gtk_combo_box_get_active(combo_box);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-lfo-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_oscillator);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_1_lfo_frequency_callback(GtkSpinButton *spin_button, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_frequency;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_frequency);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_1_lfo_depth_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_depth;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  lfo_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-lfo-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_depth);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_1_lfo_tuning_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_tuning;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  lfo_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-lfo-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_tuning);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_2_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  oscillator = gtk_combo_box_get_active(combo_box);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-2-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) oscillator);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_2_octave_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble octave;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  octave = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-2-octave", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) octave);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_2_key_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble key;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  key = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-2-key", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) key);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_2_phase_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble phase;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  phase = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-2-phase", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) phase);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_2_volume_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-2-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) volume);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_2_lfo_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint lfo_oscillator;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  lfo_oscillator = gtk_combo_box_get_active(combo_box);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-2-lfo-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_oscillator);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_2_lfo_frequency_callback(GtkSpinButton *spin_button, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_frequency;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-2-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_frequency);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_2_lfo_depth_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_depth;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  lfo_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-2-lfo-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_depth);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_synth_2_lfo_tuning_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_tuning;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  lfo_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-2-lfo-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_tuning);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_sequencer_enabled_callback(GtkButton *button, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sequencer_enabled;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  sequencer_enabled = gtk_toggle_button_get_active((GtkToggleButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "sequencer-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sequencer_enabled);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_sequencer_sign_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  oscillator = gtk_combo_box_get_active(combo_box);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "sequencer-sign", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) oscillator);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_pitch_tuning_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble pitch_tuning;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  pitch_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "pitch-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) pitch_tuning);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_noise_gain_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble noise_gain;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  noise_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "noise-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) noise_gain);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_low_pass_enabled_callback(GtkButton *button, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble low_pass_enabled;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  low_pass_enabled = gtk_toggle_button_get_active((GtkToggleButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "low-pass-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) low_pass_enabled);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_low_pass_q_lin_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble low_pass_q_lin;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  low_pass_q_lin = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "low-pass-q-lin", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) low_pass_q_lin);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_low_pass_filter_gain_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble low_pass_filter_gain;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  low_pass_filter_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "low-pass-filter-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) low_pass_filter_gain);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_high_pass_enabled_callback(GtkButton *button, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble high_pass_enabled;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  high_pass_enabled = gtk_toggle_button_get_active((GtkToggleButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "high-pass-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) high_pass_enabled);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_high_pass_q_lin_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble high_pass_q_lin;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  high_pass_q_lin = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "high-pass-q-lin", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) high_pass_q_lin);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_high_pass_filter_gain_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble high_pass_filter_gain;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  high_pass_filter_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "high-pass-filter-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) high_pass_filter_gain);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_chorus_enabled_callback(GtkButton *button, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_enabled;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  chorus_enabled = gtk_toggle_button_get_active((GtkToggleButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_enabled);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_chorus_input_volume_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_input_volume;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  chorus_input_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-input-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_input_volume);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_chorus_output_volume_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_output_volume;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  chorus_output_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-output-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_output_volume);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_chorus_lfo_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth)
{
  //TODO:JK: implement me
}

void
ags_hybrid_fm_synth_chorus_lfo_frequency_callback(GtkSpinButton *spin_button, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_lfo_frequency;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  chorus_lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_lfo_frequency);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}  

void
ags_hybrid_fm_synth_chorus_depth_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_depth;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  chorus_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_depth);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_chorus_mix_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_mix;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  chorus_mix = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-mix", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_mix);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_chorus_delay_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_delay;

  audio = AGS_MACHINE(hybrid_fm_synth)->audio;

  chorus_delay = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_FM_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-delay", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_delay);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_hybrid_fm_synth_volume_callback(GtkRange *range, AgsHybridFMSynth *hybrid_fm_synth)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat volume;

  volume = (gfloat) gtk_range_get_value(range);
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(hybrid_fm_synth)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_VOLUME_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "volume", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  volume);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}
