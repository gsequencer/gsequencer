/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/GSequencer/machine/ags_hybrid_synth_callbacks.h>

#include <ags/GSequencer/ags_window.h>

#include <math.h>

void
ags_hybrid_synth_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsHybridSynth *hybrid_synth)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_HYBRID_SYNTH)->counter);

  g_object_set(AGS_MACHINE(hybrid_synth),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_HYBRID_SYNTH);

  g_free(str);
}

void
ags_hybrid_synth_synth_0_oscillator_callback(GtkComboBox *combo_box, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  oscillator = gtk_combo_box_get_active(combo_box);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_synth_0_octave_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble octave;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  octave = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_synth_0_key_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble key;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  key = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_synth_0_phase_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble phase;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  phase = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_synth_0_volume_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_synth_0_sync_enabled_callback(GtkButton *button, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_enabled;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_enabled = gtk_toggle_button_get_active((GtkToggleButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_enabled);

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
ags_hybrid_synth_synth_0_sync_relative_attack_factor_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble relative_attack_factor;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  relative_attack_factor = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-relative-attack-factor", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) relative_attack_factor);

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
ags_hybrid_synth_synth_0_sync_attack_0_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_0;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_attack_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-attack-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_attack_0);

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
ags_hybrid_synth_synth_0_sync_phase_0_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_0;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_phase_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-phase-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_phase_0);

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
ags_hybrid_synth_synth_0_sync_attack_1_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_1;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_attack_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-attack-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_attack_1);

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
ags_hybrid_synth_synth_0_sync_phase_1_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_1;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_phase_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-phase-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_phase_1);

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
ags_hybrid_synth_synth_0_sync_attack_2_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_2;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_attack_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-attack-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_attack_2);

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
ags_hybrid_synth_synth_0_sync_phase_2_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_2;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_phase_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-phase-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_phase_2);

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
ags_hybrid_synth_synth_0_sync_lfo_oscillator_callback(GtkComboBox *combo_box, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_lfo_oscillator;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_lfo_oscillator = gtk_combo_box_get_active(combo_box);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-lfo-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_lfo_oscillator);

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
ags_hybrid_synth_synth_0_sync_lfo_frequency_callback(GtkSpinButton *spin_button, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_lfo_frequency;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_lfo_frequency);

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
ags_hybrid_synth_synth_1_oscillator_callback(GtkComboBox *combo_box, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  oscillator = gtk_combo_box_get_active(combo_box);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_synth_1_octave_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble octave;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  octave = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_synth_1_key_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble key;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  key = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_synth_1_phase_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble phase;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  phase = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_synth_1_volume_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_synth_1_sync_enabled_callback(GtkButton *button, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_enabled;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_enabled = gtk_toggle_button_get_active((GtkToggleButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_enabled);

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
ags_hybrid_synth_synth_1_sync_relative_attack_factor_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble relative_attack_factor;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  relative_attack_factor = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-relative-attack-factor", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) relative_attack_factor);

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
ags_hybrid_synth_synth_1_sync_attack_0_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_0;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_attack_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-attack-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_attack_0);

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
ags_hybrid_synth_synth_1_sync_phase_0_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_0;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_phase_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-phase-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_phase_0);

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
ags_hybrid_synth_synth_1_sync_attack_1_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_1;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_attack_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-attack-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_attack_1);

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
ags_hybrid_synth_synth_1_sync_phase_1_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_1;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_phase_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-phase-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_phase_1);

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
ags_hybrid_synth_synth_1_sync_attack_2_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_2;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_attack_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-attack-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_attack_2);

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
ags_hybrid_synth_synth_1_sync_phase_2_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_2;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_phase_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-phase-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_phase_2);

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
ags_hybrid_synth_synth_1_sync_lfo_oscillator_callback(GtkComboBox *combo_box, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_lfo_oscillator;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_lfo_oscillator = gtk_combo_box_get_active(combo_box);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-lfo-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_lfo_oscillator);

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
ags_hybrid_synth_synth_1_sync_lfo_frequency_callback(GtkSpinButton *spin_button, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_lfo_frequency;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sync_lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_lfo_frequency);

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
ags_hybrid_synth_sequencer_enabled_callback(GtkButton *button, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sequencer_enabled;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  sequencer_enabled = gtk_toggle_button_get_active((GtkToggleButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_sequencer_sign_callback(GtkComboBox *combo_box, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  oscillator = gtk_combo_box_get_active(combo_box);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_pitch_tuning_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble pitch_tuning;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  pitch_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_noise_gain_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble noise_gain;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  noise_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_low_pass_enabled_callback(GtkButton *button, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble low_pass_enabled;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  low_pass_enabled = gtk_toggle_button_get_active((GtkToggleButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_low_pass_q_lin_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble low_pass_q_lin;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  low_pass_q_lin = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_low_pass_filter_gain_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble low_pass_filter_gain;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  low_pass_filter_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_high_pass_enabled_callback(GtkButton *button, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble high_pass_enabled;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  high_pass_enabled = gtk_toggle_button_get_active((GtkToggleButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_high_pass_q_lin_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble high_pass_q_lin;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  high_pass_q_lin = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_high_pass_filter_gain_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble high_pass_filter_gain;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  high_pass_filter_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_chorus_enabled_callback(GtkButton *button, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_enabled;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  chorus_enabled = gtk_toggle_button_get_active((GtkToggleButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_chorus_input_volume_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_input_volume;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  chorus_input_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_chorus_output_volume_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_output_volume;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  chorus_output_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_chorus_lfo_oscillator_callback(GtkComboBox *combo_box, AgsHybridSynth *hybrid_synth)
{
  //TODO:JK: implement me
}

void
ags_hybrid_synth_chorus_lfo_frequency_callback(GtkSpinButton *spin_button, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_lfo_frequency;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  chorus_lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_chorus_depth_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_depth;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  chorus_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_chorus_mix_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_mix;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  chorus_mix = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_chorus_delay_callback(AgsDial *dial, AgsHybridSynth *hybrid_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_delay;

  audio = AGS_MACHINE(hybrid_synth)->audio;

  chorus_delay = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SYNTH_AUDIO)) != NULL){
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
ags_hybrid_synth_volume_callback(GtkRange *range, AgsHybridSynth *hybrid_synth)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat volume;

  volume = (gfloat) gtk_range_get_value(range);
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(hybrid_synth)->audio,
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
