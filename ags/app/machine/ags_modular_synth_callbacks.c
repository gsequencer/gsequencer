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

#include <ags/app/machine/ags_modular_synth_callbacks.h>

#include <ags/app/ags_window.h>

#include <math.h>

void
ags_modular_synth_env_0_attack_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_0_attack;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  env_0_attack = ags_dial_get_value(modular_synth->env_0_attack);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-0-attack", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_0_attack);

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
ags_modular_synth_env_0_decay_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_0_decay;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  env_0_decay = ags_dial_get_value(modular_synth->env_0_decay);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-0-decay", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_0_decay);

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
ags_modular_synth_env_0_sustain_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_0_sustain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  env_0_sustain = ags_dial_get_value(modular_synth->env_0_sustain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-0-sustain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_0_sustain);

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
ags_modular_synth_env_0_release_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_0_release;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  env_0_release = ags_dial_get_value(modular_synth->env_0_release);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-0-release", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_0_release);

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
ags_modular_synth_env_0_gain_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_0_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  env_0_gain = ags_dial_get_value(modular_synth->env_0_gain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-0-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_0_gain);

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
ags_modular_synth_env_0_frequency_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_0_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  env_0_frequency = ags_dial_get_value(modular_synth->env_0_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-0-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_0_frequency);

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
ags_modular_synth_env_1_attack_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_1_attack;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  env_1_attack = ags_dial_get_value(modular_synth->env_1_attack);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-1-attack", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_1_attack);

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
ags_modular_synth_env_1_decay_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_1_decay;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  env_1_decay = ags_dial_get_value(modular_synth->env_1_decay);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-1-decay", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_1_decay);

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
ags_modular_synth_env_1_sustain_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_1_sustain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  env_1_sustain = ags_dial_get_value(modular_synth->env_1_sustain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-1-sustain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_1_sustain);

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
ags_modular_synth_env_1_release_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_1_release;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  env_1_release = ags_dial_get_value(modular_synth->env_1_release);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-1-release", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_1_release);

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
ags_modular_synth_env_1_gain_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_1_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  env_1_gain = ags_dial_get_value(modular_synth->env_1_gain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-1-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_1_gain);

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
ags_modular_synth_env_1_frequency_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_1_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  env_1_frequency = ags_dial_get_value(modular_synth->env_1_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-1-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_1_frequency);

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
ags_modular_synth_lfo_0_oscillator_callback(GObject *gobject,
					    GParamSpec *pspec,
					    AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  oscillator = gtk_drop_down_get_selected((GtkDropDown *) gobject);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-0-oscillator", &port,
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
ags_modular_synth_lfo_0_frequency_callback(GtkSpinButton *spin_button, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_0_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  lfo_0_frequency = ags_dial_get_value(modular_synth->lfo_0_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-0-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_0_frequency);

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
ags_modular_synth_lfo_0_depth_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_0_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  lfo_0_depth = ags_dial_get_value(modular_synth->lfo_0_depth);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-0-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_0_depth);

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
ags_modular_synth_lfo_0_tuning_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_0_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  lfo_0_tuning = ags_dial_get_value(modular_synth->lfo_0_tuning);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-0-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_0_tuning);

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
ags_modular_synth_lfo_1_oscillator_callback(GObject *gobject,
					    GParamSpec *pspec,
					    AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  oscillator = gtk_drop_down_get_selected((GtkDropDown *) gobject);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-1-oscillator", &port,
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
ags_modular_synth_lfo_1_frequency_callback(GtkSpinButton *spin_button, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_1_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  lfo_1_frequency = ags_dial_get_value(modular_synth->lfo_1_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-1-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_1_frequency);

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
ags_modular_synth_lfo_1_depth_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_1_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  lfo_1_depth = ags_dial_get_value(modular_synth->lfo_1_depth);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-1-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_1_depth);

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
ags_modular_synth_lfo_1_tuning_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_1_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  lfo_1_tuning = ags_dial_get_value(modular_synth->lfo_1_tuning);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-1-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_1_tuning);

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
ags_modular_synth_modulation_matrix_callback(AgsModulationMatrix *modulation_matrix,
					     gint x, gint y,
					     AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_1_tuning;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  lfo_1_tuning = ags_dial_get_value(modular_synth->lfo_1_tuning);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    gint64 sends[AGS_MODULAR_SYNTH_SENDS_COUNT], port_sends[AGS_MODULAR_SYNTH_SENDS_COUNT];

    guint i, j;
    
    port = NULL;
    
    if(y == 0){ /* env-0 */
      g_object_get(recall->data,
		   "synth-0-env-0-sends", &port,
		   NULL);
    }else if(y == 1){ /* env-1 */
      g_object_get(recall->data,
		   "synth-0-env-1-sends", &port,
		   NULL);
    }else if(y == 2){ /* lfo-0 */
      g_object_get(recall->data,
		   "synth-0-lfo-0-sends", &port,
		   NULL);
    }else if(y == 3){ /* lfo-1 */
      g_object_get(recall->data,
		   "synth-0-lfo-1-sends", &port,
		   NULL);
    }else if(y == 4){ /* noise */
      g_object_get(recall->data,
		   "synth-0-noise-sends", &port,
		   NULL);
    }

    memset(&(sends[0]), 0, AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(port_sends[0]), 0, AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint64));

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_POINTER);

      g_value_set_pointer(&value,
			  &(port_sends[0]));

      ags_port_safe_read(port,
			 &value);
    }
      
    if(port != NULL){
      GValue value = G_VALUE_INIT;

      //NOTE:JK: we fill sends
      // memcpy(&(sends[0]), port_sends, AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint));

      for(i = 0, j = 0; i < AGS_MODULAR_SYNTH_SENDS_COUNT; i++){
	if(ags_modulation_matrix_get_enabled(modular_synth->modulation_matrix,
					     i, y)){
	  sends[j] = (1L << i);
	  
	  j++;
	}
      }
	
      g_value_init(&value,
		   G_TYPE_POINTER);
	  
      g_value_set_pointer(&value,
			  (gpointer) &(sends[0]));

      ags_port_safe_write(port,
			  &value);
    }
    
    if(port != NULL){
      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_modular_synth_noise_frequency_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double noise_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  noise_frequency = ags_dial_get_value(modular_synth->noise_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-noise-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) noise_frequency);

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
ags_modular_synth_noise_gain_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double noise_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  noise_gain = ags_dial_get_value(modular_synth->noise_gain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-noise-gain", &port,
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
ags_modular_synth_osc_0_oscillator_callback(GObject *gobject,
					    GParamSpec *pspec,
					    AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  oscillator = gtk_drop_down_get_selected((GtkDropDown *) gobject);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-0-oscillator", &port,
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
ags_modular_synth_osc_0_octave_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_0_octave;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  osc_0_octave = ags_dial_get_value(modular_synth->osc_0_octave);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-0-octave", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_0_octave);

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
ags_modular_synth_osc_0_key_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_0_key;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  osc_0_key = ags_dial_get_value(modular_synth->osc_0_key);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-0-key", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_0_key);

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
ags_modular_synth_osc_0_phase_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_0_phase;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  osc_0_phase = ags_dial_get_value(modular_synth->osc_0_phase);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-0-phase", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_0_phase);

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
ags_modular_synth_osc_0_volume_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_0_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  osc_0_volume = ags_dial_get_value(modular_synth->osc_0_volume);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-0-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_0_volume);

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
ags_modular_synth_osc_1_oscillator_callback(GObject *gobject,
					    GParamSpec *pspec,
					    AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  oscillator = gtk_drop_down_get_selected((GtkDropDown *) gobject);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-1-oscillator", &port,
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
ags_modular_synth_osc_1_octave_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_1_octave;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  osc_1_octave = ags_dial_get_value(modular_synth->osc_1_octave);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-1-octave", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_1_octave);

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
ags_modular_synth_osc_1_key_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_1_key;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  osc_1_key = ags_dial_get_value(modular_synth->osc_1_key);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-1-key", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_1_key);

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
ags_modular_synth_osc_1_phase_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_1_phase;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  osc_1_phase = ags_dial_get_value(modular_synth->osc_1_phase);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-1-phase", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_1_phase);

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
ags_modular_synth_osc_1_volume_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_1_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  osc_1_volume = ags_dial_get_value(modular_synth->osc_1_volume);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-1-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_1_volume);

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
ags_modular_synth_pitch_tuning_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double pitch_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  pitch_tuning = ags_dial_get_value(modular_synth->pitch_tuning);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-pitch-tuning", &port,
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
ags_modular_synth_volume_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  volume = ags_dial_get_value(modular_synth->volume);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_low_pass_0_cut_off_frequency_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_low_pass_0_filter_gain_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_low_pass_0_no_clip_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_amplifier_0_amp_0_gain_callback(GtkRange *range, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_amplifier_0_amp_1_gain_callback(GtkRange *range, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_amplifier_0_amp_2_gain_callback(GtkRange *range, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_amplifier_0_amp_3_gain_callback(GtkRange *range, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_amplifier_0_filter_gain_callback(GtkRange *range, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_chorus_enabled_callback(GtkButton *button, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  chorus_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_chorus_input_volume_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_input_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  chorus_input_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_chorus_output_volume_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_output_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  chorus_output_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_chorus_lfo_oscillator_callback(GObject *gobject,
					       GParamSpec *pspec,
					       AgsModularSynth *modular_synth)
{
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  //TODO:JK: implement me
}

void
ags_modular_synth_chorus_lfo_frequency_callback(GtkSpinButton *spin_button, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  chorus_lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_chorus_depth_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  chorus_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_chorus_mix_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_mix;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  chorus_mix = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
ags_modular_synth_chorus_delay_callback(AgsDial *dial, AgsModularSynth *modular_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_delay;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(modular_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  chorus_delay = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
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
