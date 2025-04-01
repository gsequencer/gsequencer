/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/app/machine/ags_raven_synth_callbacks.h>

#include <ags/app/ags_window.h>

#include <math.h>

void
ags_raven_synth_synth_0_oscillator_callback(GtkDropDown *drop_down, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  oscillator = gtk_drop_down_get_selected(drop_down);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_octave_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble octave;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  octave = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_key_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble key;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  key = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_phase_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble phase;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  phase = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_volume_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_lfo_oscillator_callback(GtkDropDown *drop_down, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint lfo_oscillator;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  lfo_oscillator = gtk_drop_down_get_selected(drop_down);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_lfo_depth_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  lfo_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_lfo_tuning_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  lfo_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_sync_enabled_callback(GtkButton *button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_enabled;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_sync_relative_attack_factor_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble relative_attack_factor;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  relative_attack_factor = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-relative-attack-factor-0", &port,
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
ags_raven_synth_synth_0_sync_attack_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_0;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_attack_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_sync_phase_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_0;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_phase_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_sync_relative_attack_factor_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble relative_attack_factor;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  relative_attack_factor = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-relative-attack-factor-1", &port,
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
ags_raven_synth_synth_0_sync_attack_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_1;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_attack_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_sync_phase_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_1;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_phase_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_sync_relative_attack_factor_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble relative_attack_factor;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  relative_attack_factor = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-relative-attack-factor-2", &port,
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
ags_raven_synth_synth_0_sync_attack_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_2;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_attack_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_sync_phase_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_2;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_phase_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_0_sync_relative_attack_factor_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble relative_attack_factor;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  relative_attack_factor = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-relative-attack-factor-3", &port,
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
ags_raven_synth_synth_0_sync_attack_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_3;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_attack_3 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-attack-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_attack_3);

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
ags_raven_synth_synth_0_sync_phase_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_3;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_phase_3 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-sync-phase-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_phase_3);

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
ags_raven_synth_synth_0_seq_tuning_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_0;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_0);

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
ags_raven_synth_synth_0_seq_tuning_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_1;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_1);

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
ags_raven_synth_synth_0_seq_tuning_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_2;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_2);

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
ags_raven_synth_synth_0_seq_tuning_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_3;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_3 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_3);

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
ags_raven_synth_synth_0_seq_tuning_4_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_4;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_4 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-4", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_4);

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
ags_raven_synth_synth_0_seq_tuning_5_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_5;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_5 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-5", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_5);

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
ags_raven_synth_synth_0_seq_tuning_6_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_6;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_6 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-6", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_6);

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
ags_raven_synth_synth_0_seq_tuning_7_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_7;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_7 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-7", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_7);

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
ags_raven_synth_synth_0_seq_tuning_8_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_8;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_8 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-8", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_8);

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
ags_raven_synth_synth_0_seq_tuning_9_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_9;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_9 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-9", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_9);

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
ags_raven_synth_synth_0_seq_tuning_10_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_10;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_10 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-10", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_10);

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
ags_raven_synth_synth_0_seq_tuning_11_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_11;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_11 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-11", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_11);

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
ags_raven_synth_synth_0_seq_tuning_12_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_12;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_12 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-12", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_12);

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
ags_raven_synth_synth_0_seq_tuning_13_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_13;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_13 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-13", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_13);

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
ags_raven_synth_synth_0_seq_tuning_14_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_14;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_14 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-14", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_14);

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
ags_raven_synth_synth_0_seq_tuning_15_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_15;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_15 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-15", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_15);

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
ags_raven_synth_synth_0_seq_tuning_pingpong_callback(GtkCheckButton *check_button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_pingpong;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_pingpong = gtk_check_button_get_active(check_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-pingpong", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_pingpong);

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
ags_raven_synth_synth_0_seq_tuning_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SEQ_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-tuning-lfo-frequency", &port,
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
ags_raven_synth_synth_0_seq_volume_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_0;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_0);

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
ags_raven_synth_synth_0_seq_volume_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_1;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_1);

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
ags_raven_synth_synth_0_seq_volume_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_2;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_2);

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
ags_raven_synth_synth_0_seq_volume_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_3;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_3 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_3);

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
ags_raven_synth_synth_0_seq_volume_4_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_4;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_4 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-4", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_4);

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
ags_raven_synth_synth_0_seq_volume_5_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_5;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_5 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-5", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_5);

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
ags_raven_synth_synth_0_seq_volume_6_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_6;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_6 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-6", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_6);

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
ags_raven_synth_synth_0_seq_volume_7_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_7;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_7 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-7", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_7);

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
ags_raven_synth_synth_0_seq_volume_8_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_8;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_8 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-8", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_8);

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
ags_raven_synth_synth_0_seq_volume_9_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_9;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_9 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-9", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_9);

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
ags_raven_synth_synth_0_seq_volume_10_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_10;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_10 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-10", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_10);

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
ags_raven_synth_synth_0_seq_volume_11_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_11;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_11 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-11", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_11);

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
ags_raven_synth_synth_0_seq_volume_12_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_12;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_12 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-12", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_12);

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
ags_raven_synth_synth_0_seq_volume_13_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_13;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_13 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-13", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_13);

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
ags_raven_synth_synth_0_seq_volume_14_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_14;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_14 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-14", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_14);

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
ags_raven_synth_synth_0_seq_volume_15_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_15;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_15 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-15", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_15);

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
ags_raven_synth_synth_0_seq_volume_pingpong_callback(GtkCheckButton *check_button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_pingpong;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_pingpong = gtk_check_button_get_active(check_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-pingpong", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_pingpong);

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
ags_raven_synth_synth_0_seq_volume_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SEQ_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-volume-lfo-frequency", &port,
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
ags_raven_synth_low_pass_0_cut_off_frequency_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "low-pass-0-cut-off-frequency", &port,
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
ags_raven_synth_low_pass_0_filter_gain_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "low-pass-0-filter-gain", &port,
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
ags_raven_synth_low_pass_0_no_clip_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "low-pass-0-no-clip", &port,
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
ags_raven_synth_amplifier_0_amp_0_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "amplifier-0-amp-0-gain", &port,
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
ags_raven_synth_amplifier_0_amp_1_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "amplifier-0-amp-1-gain", &port,
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
ags_raven_synth_amplifier_0_amp_2_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "amplifier-0-amp-2-gain", &port,
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
ags_raven_synth_amplifier_0_amp_3_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "amplifier-0-amp-3-gain", &port,
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
ags_raven_synth_amplifier_0_filter_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "amplifier-0-filter-gain", &port,
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
ags_raven_synth_synth_1_oscillator_callback(GtkDropDown *drop_down, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  oscillator = gtk_drop_down_get_selected(drop_down);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_octave_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble octave;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  octave = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_key_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble key;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  key = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_phase_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble phase;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  phase = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_volume_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_sync_lfo_oscillator_callback(GtkDropDown *drop_down, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_lfo_oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_lfo_oscillator = gtk_drop_down_get_selected(drop_down);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_sync_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_lfo_frequency;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_lfo_oscillator_callback(GtkDropDown *drop_down, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint lfo_oscillator;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  lfo_oscillator = gtk_drop_down_get_selected(drop_down);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_lfo_depth_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  lfo_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_lfo_tuning_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  lfo_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_sync_enabled_callback(GtkButton *button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_enabled;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_sync_relative_attack_factor_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble relative_attack_factor;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  relative_attack_factor = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-relative-attack-factor-0", &port,
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
ags_raven_synth_synth_1_sync_attack_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_0;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_attack_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_sync_phase_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_0;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_phase_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_sync_relative_attack_factor_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble relative_attack_factor;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  relative_attack_factor = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-relative-attack-factor-1", &port,
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
ags_raven_synth_synth_1_sync_attack_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_1;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_attack_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_sync_phase_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_1;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_phase_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_sync_relative_attack_factor_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble relative_attack_factor;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  relative_attack_factor = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-relative-attack-factor-2", &port,
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
ags_raven_synth_synth_1_sync_attack_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_2;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_attack_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_sync_phase_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_2;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_phase_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_synth_1_sync_relative_attack_factor_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble relative_attack_factor;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  relative_attack_factor = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-relative-attack-factor-3", &port,
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
ags_raven_synth_synth_1_sync_attack_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_attack_3;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_attack_3 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-attack-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_attack_3);

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
ags_raven_synth_synth_1_sync_phase_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble sync_phase_3;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  sync_phase_3 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-sync-phase-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) sync_phase_3);

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
ags_raven_synth_synth_1_seq_tuning_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_0;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_0);

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
ags_raven_synth_synth_1_seq_tuning_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_1;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_1);

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
ags_raven_synth_synth_1_seq_tuning_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_2;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_2);

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
ags_raven_synth_synth_1_seq_tuning_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_3;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_3 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_3);

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
ags_raven_synth_synth_1_seq_tuning_4_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_4;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_4 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-4", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_4);

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
ags_raven_synth_synth_1_seq_tuning_5_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_5;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_5 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-5", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_5);

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
ags_raven_synth_synth_1_seq_tuning_6_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_6;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_6 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-6", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_6);

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
ags_raven_synth_synth_1_seq_tuning_7_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_7;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_7 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-7", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_7);

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
ags_raven_synth_synth_1_seq_tuning_8_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_8;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_8 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-8", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_8);

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
ags_raven_synth_synth_1_seq_tuning_9_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_9;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_9 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-9", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_9);

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
ags_raven_synth_synth_1_seq_tuning_10_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_10;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_10 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-10", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_10);

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
ags_raven_synth_synth_1_seq_tuning_11_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_11;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_11 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-11", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_11);

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
ags_raven_synth_synth_1_seq_tuning_12_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_12;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_12 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-12", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_12);

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
ags_raven_synth_synth_1_seq_tuning_13_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_13;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_13 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-13", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_13);

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
ags_raven_synth_synth_1_seq_tuning_14_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_14;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_14 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-14", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_14);

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
ags_raven_synth_synth_1_seq_tuning_15_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_15;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_15 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-15", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_15);

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
ags_raven_synth_synth_1_seq_tuning_pingpong_callback(GtkCheckButton *check_button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_tuning_pingpong;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_tuning_pingpong = gtk_check_button_get_active(check_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-pingpong", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_tuning_pingpong);

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
ags_raven_synth_synth_1_seq_tuning_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SEQ_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-tuning-lfo-frequency", &port,
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
ags_raven_synth_synth_1_seq_volume_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_0;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_0);

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
ags_raven_synth_synth_1_seq_volume_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_1;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_1);

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
ags_raven_synth_synth_1_seq_volume_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_2;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_2);

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
ags_raven_synth_synth_1_seq_volume_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_3;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_3 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_3);

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
ags_raven_synth_synth_1_seq_volume_4_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_4;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_4 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-4", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_4);

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
ags_raven_synth_synth_1_seq_volume_5_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_5;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_5 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-5", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_5);

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
ags_raven_synth_synth_1_seq_volume_6_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_6;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_6 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-6", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_6);

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
ags_raven_synth_synth_1_seq_volume_7_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_7;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_7 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-7", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_7);

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
ags_raven_synth_synth_1_seq_volume_8_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_8;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_8 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-8", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_8);

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
ags_raven_synth_synth_1_seq_volume_9_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_9;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_9 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-9", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_9);

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
ags_raven_synth_synth_1_seq_volume_10_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_10;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_10 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-10", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_10);

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
ags_raven_synth_synth_1_seq_volume_11_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_11;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_11 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-11", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_11);

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
ags_raven_synth_synth_1_seq_volume_12_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_12;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_12 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-12", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_12);

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
ags_raven_synth_synth_1_seq_volume_13_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_13;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_13 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-13", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_13);

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
ags_raven_synth_synth_1_seq_volume_14_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_14;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_14 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-14", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_14);

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
ags_raven_synth_synth_1_seq_volume_15_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_15;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_15 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-15", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_15);

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
ags_raven_synth_synth_1_seq_volume_pingpong_callback(GtkCheckButton *check_button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_volume_pingpong;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  seq_volume_pingpong = gtk_check_button_get_active(check_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-pingpong", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_volume_pingpong);

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
ags_raven_synth_synth_1_seq_volume_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SEQ_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-1-seq-volume-lfo-frequency", &port,
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
ags_raven_synth_low_pass_1_cut_off_frequency_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "low-pass-1-cut-off-frequency", &port,
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
ags_raven_synth_low_pass_1_filter_gain_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "low-pass-1-filter-gain", &port,
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
ags_raven_synth_low_pass_1_no_clip_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "low-pass-1-no-clip", &port,
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
ags_raven_synth_amplifier_1_amp_0_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "amplifier-1-amp-0-gain", &port,
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
ags_raven_synth_amplifier_1_amp_1_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "amplifier-1-amp-1-gain", &port,
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
ags_raven_synth_amplifier_1_amp_2_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "amplifier-1-amp-2-gain", &port,
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
ags_raven_synth_amplifier_1_amp_3_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "amplifier-1-amp-3-gain", &port,
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
ags_raven_synth_amplifier_1_filter_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "amplifier-1-filter-gain", &port,
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
ags_raven_synth_pitch_type_callback(GObject *gobject,
				    GParamSpec *pspec,
				    AgsRavenSynth *raven_synth)
{
  AgsFxStarSynthAudio *fx_raven_synth_audio;
    
  guint selected;

  GValue value = G_VALUE_INIT;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }
  
  selected = gtk_drop_down_get_selected((GtkDropDown *) gobject);

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value,
		    (gfloat) selected);

  /* play */
  fx_raven_synth_audio = (AgsFxStarSynthAudio *) ags_recall_container_get_recall_audio(raven_synth->raven_synth_play_container);

  ags_port_safe_write(fx_raven_synth_audio->pitch_type,
		      &value);

  /* recall */
  fx_raven_synth_audio = (AgsFxStarSynthAudio *) ags_recall_container_get_recall_audio(raven_synth->raven_synth_recall_container);
  
  ags_port_safe_write(fx_raven_synth_audio->pitch_type,
		      &value);
}

void
ags_raven_synth_pitch_tuning_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble pitch_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  pitch_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_noise_gain_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble noise_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  noise_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_chorus_enabled_callback(GtkButton *button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  chorus_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_chorus_input_volume_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_input_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  chorus_input_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_chorus_output_volume_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_output_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  chorus_output_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_chorus_lfo_oscillator_callback(GtkDropDown *drop_down, AgsRavenSynth *raven_synth)
{
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  //TODO:JK: implement me
}

void
ags_raven_synth_chorus_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  chorus_lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_chorus_depth_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  chorus_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_chorus_mix_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_mix;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  chorus_mix = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_chorus_delay_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_delay;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  chorus_delay = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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
ags_raven_synth_volume_callback(GtkRange *range, AgsRavenSynth *raven_synth)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat volume;

  volume = (gfloat) gtk_range_get_value(range);
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(raven_synth)->audio,
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

void
ags_raven_synth_tremolo_enabled_callback(GtkCheckButton *button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  tremolo_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TREMOLO_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "tremolo-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) tremolo_enabled);

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
ags_raven_synth_tremolo_gain_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  tremolo_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TREMOLO_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "tremolo-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) tremolo_gain);

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
ags_raven_synth_tremolo_lfo_depth_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_lfo_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  tremolo_lfo_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TREMOLO_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "tremolo-lfo-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) tremolo_lfo_depth);

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
ags_raven_synth_tremolo_lfo_freq_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_lfo_freq;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  tremolo_lfo_freq = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TREMOLO_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "tremolo-lfo-freq", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) tremolo_lfo_freq);

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
ags_raven_synth_tremolo_tuning_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  tremolo_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TREMOLO_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "tremolo-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) tremolo_tuning);

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
ags_raven_synth_vibrato_enabled_callback(GtkCheckButton *button, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  vibrato_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "vibrato-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) vibrato_enabled);

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
ags_raven_synth_vibrato_gain_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  vibrato_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "vibrato-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) vibrato_gain);

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
ags_raven_synth_vibrato_lfo_depth_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_lfo_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  vibrato_lfo_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "vibrato-lfo-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) vibrato_lfo_depth);

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
ags_raven_synth_vibrato_lfo_freq_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_lfo_freq;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  vibrato_lfo_freq = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "vibrato-lfo-freq", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) vibrato_lfo_freq);

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
ags_raven_synth_vibrato_tuning_callback(AgsDial *dial, AgsRavenSynth *raven_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(raven_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  vibrato_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "vibrato-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) vibrato_tuning);

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
