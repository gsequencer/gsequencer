/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <ags/app/machine/ags_abyss_synth_callbacks.h>

#include <ags/app/ags_window.h>

#include <math.h>

void
ags_abyss_synth_env_0_attack_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_0_attack;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_0_attack = ags_dial_get_value(abyss_synth->env_0_attack);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_env_0_decay_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_0_decay;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_0_decay = ags_dial_get_value(abyss_synth->env_0_decay);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_env_0_sustain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_0_sustain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_0_sustain = ags_dial_get_value(abyss_synth->env_0_sustain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_env_0_release_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_0_release;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_0_release = ags_dial_get_value(abyss_synth->env_0_release);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_env_0_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_0_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_0_gain = ags_dial_get_value(abyss_synth->env_0_gain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_env_0_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_0_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_0_frequency = ags_dial_get_value(abyss_synth->env_0_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_env_1_attack_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_1_attack;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_1_attack = ags_dial_get_value(abyss_synth->env_1_attack);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_env_1_decay_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_1_decay;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_1_decay = ags_dial_get_value(abyss_synth->env_1_decay);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_env_1_sustain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_1_sustain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_1_sustain = ags_dial_get_value(abyss_synth->env_1_sustain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_env_1_release_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_1_release;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_1_release = ags_dial_get_value(abyss_synth->env_1_release);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_env_1_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_1_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_1_gain = ags_dial_get_value(abyss_synth->env_1_gain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_env_1_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_1_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_1_frequency = ags_dial_get_value(abyss_synth->env_1_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_env_2_attack_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_2_attack;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_2_attack = ags_dial_get_value(abyss_synth->env_2_attack);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-2-attack", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_2_attack);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_env_2_decay_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_2_decay;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_2_decay = ags_dial_get_value(abyss_synth->env_2_decay);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-2-decay", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_2_decay);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_env_2_sustain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_2_sustain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_2_sustain = ags_dial_get_value(abyss_synth->env_2_sustain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-2-sustain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_2_sustain);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_env_2_release_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_2_release;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_2_release = ags_dial_get_value(abyss_synth->env_2_release);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-2-release", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_2_release);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_env_2_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_2_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_2_gain = ags_dial_get_value(abyss_synth->env_2_gain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-2-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_2_gain);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_env_2_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_2_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_2_frequency = ags_dial_get_value(abyss_synth->env_2_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-2-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_2_frequency);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_env_3_attack_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_3_attack;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_3_attack = ags_dial_get_value(abyss_synth->env_3_attack);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-3-attack", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_3_attack);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_env_3_decay_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_3_decay;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_3_decay = ags_dial_get_value(abyss_synth->env_3_decay);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-3-decay", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_3_decay);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_env_3_sustain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_3_sustain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_3_sustain = ags_dial_get_value(abyss_synth->env_3_sustain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-3-sustain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_3_sustain);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_env_3_release_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_3_release;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_3_release = ags_dial_get_value(abyss_synth->env_3_release);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-3-release", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_3_release);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_env_3_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_3_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_3_gain = ags_dial_get_value(abyss_synth->env_3_gain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-3-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_3_gain);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_env_3_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double env_3_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  env_3_frequency = ags_dial_get_value(abyss_synth->env_3_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-env-3-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) env_3_frequency);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_pink_noise_0_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double noise_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  noise_frequency = ags_dial_get_value(abyss_synth->pink_noise_0_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-pink-noise-0-frequency", &port,
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
ags_abyss_synth_pink_noise_0_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double noise_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  noise_gain = ags_dial_get_value(abyss_synth->pink_noise_0_gain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-pink-noise-0-gain", &port,
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
ags_abyss_synth_pink_noise_1_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double noise_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  noise_frequency = ags_dial_get_value(abyss_synth->pink_noise_1_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-pink-noise-1-frequency", &port,
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
ags_abyss_synth_pink_noise_1_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double noise_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  noise_gain = ags_dial_get_value(abyss_synth->pink_noise_1_gain);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-pink-noise-1-gain", &port,
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
ags_abyss_synth_lfo_0_oscillator_callback(GObject *gobject,
					  GParamSpec *pspec,
					  AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  oscillator = gtk_drop_down_get_selected((GtkDropDown *) gobject);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_lfo_0_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_0_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_0_frequency = ags_dial_get_value(abyss_synth->lfo_0_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_lfo_0_depth_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_0_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_0_depth = ags_dial_get_value(abyss_synth->lfo_0_depth);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_lfo_0_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_0_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_0_tuning = ags_dial_get_value(abyss_synth->lfo_0_tuning);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_lfo_1_oscillator_callback(GObject *gobject,
					  GParamSpec *pspec,
					  AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  oscillator = gtk_drop_down_get_selected((GtkDropDown *) gobject);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_lfo_1_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_1_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_1_frequency = ags_dial_get_value(abyss_synth->lfo_1_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_lfo_1_depth_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_1_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_1_depth = ags_dial_get_value(abyss_synth->lfo_1_depth);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_lfo_1_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_1_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_1_tuning = ags_dial_get_value(abyss_synth->lfo_1_tuning);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_lfo_2_oscillator_callback(GObject *gobject,
					  GParamSpec *pspec,
					  AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  oscillator = gtk_drop_down_get_selected((GtkDropDown *) gobject);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-2-oscillator", &port,
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
ags_abyss_synth_lfo_2_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_2_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_2_frequency = ags_dial_get_value(abyss_synth->lfo_2_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-2-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_2_frequency);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_lfo_2_depth_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_2_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_2_depth = ags_dial_get_value(abyss_synth->lfo_2_depth);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-2-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_2_depth);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_lfo_2_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_2_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_2_tuning = ags_dial_get_value(abyss_synth->lfo_2_tuning);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-2-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_2_tuning);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_lfo_3_oscillator_callback(GObject *gobject,
					  GParamSpec *pspec,
					  AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  oscillator = gtk_drop_down_get_selected((GtkDropDown *) gobject);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-3-oscillator", &port,
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
ags_abyss_synth_lfo_3_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_3_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_3_frequency = ags_dial_get_value(abyss_synth->lfo_3_frequency);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-3-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_3_frequency);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_lfo_3_depth_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_3_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_3_depth = ags_dial_get_value(abyss_synth->lfo_3_depth);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-3-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_3_depth);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_lfo_3_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_3_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_3_tuning = ags_dial_get_value(abyss_synth->lfo_3_tuning);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-lfo-3-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) lfo_3_tuning);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_0_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_0;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_0);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_1_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_1;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_1);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_2_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_2;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_2);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_3_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_3;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_3 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_3);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_4_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_4;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_4 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-4", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_4);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_5_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_5;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_5 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-5", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_5);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_6_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_6;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_6 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-6", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_6);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_7_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_7;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_7 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-7", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_7);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_8_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_8;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_8 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-8", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_8);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_9_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_9;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_9 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-9", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_9);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_10_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_10;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_10 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-10", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_10);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_11_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_11;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_11 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-11", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_11);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_12_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_12;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_12 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-12", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_12);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_13_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_13;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_13 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-13", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_13);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_14_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_14;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_14 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-14", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_14);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_modulation_15_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_15;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_15 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-modulation-15", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_15);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_pingpong_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_pingpong;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_pingpong = gtk_check_button_get_active(check_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-pingpong", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_pingpong);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_0_lfo_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-0-lfo-frequency", &port,
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
ags_abyss_synth_seq_1_modulation_0_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_0;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_0);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_1_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_1;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_1);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_2_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_2;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_2);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_3_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_3;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_3 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_3);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_4_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_4;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_4 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-4", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_4);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_5_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_5;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_5 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-5", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_5);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_6_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_6;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_6 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-6", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_6);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_7_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_7;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_7 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-7", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_7);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_8_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_8;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_8 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-8", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_8);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_9_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_9;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_9 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-9", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_9);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_10_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_10;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_10 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-10", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_10);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_11_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_11;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_11 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-11", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_11);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_12_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_12;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_12 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-12", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_12);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_13_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_13;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_13 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-13", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_13);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_14_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_14;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_14 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-14", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_14);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_modulation_15_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_15;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_15 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-modulation-15", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_15);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_pingpong_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_pingpong;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_pingpong = gtk_check_button_get_active(check_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-pingpong", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_pingpong);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_1_lfo_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-1-lfo-frequency", &port,
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
ags_abyss_synth_seq_2_modulation_0_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_0;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_0);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_1_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_1;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_1);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_2_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_2;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_2);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_3_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_3;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_3 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_3);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_4_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_4;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_4 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-4", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_4);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_5_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_5;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_5 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-5", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_5);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_6_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_6;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_6 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-6", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_6);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_7_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_7;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_7 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-7", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_7);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_8_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_8;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_8 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-8", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_8);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_9_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_9;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_9 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-9", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_9);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_10_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_10;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_10 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-10", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_10);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_11_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_11;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_11 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-11", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_11);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_12_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_12;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_12 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-12", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_12);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_13_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_13;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_13 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-13", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_13);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_14_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_14;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_14 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-14", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_14);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_modulation_15_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_15;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_15 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-modulation-15", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_15);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_pingpong_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_pingpong;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_pingpong = gtk_check_button_get_active(check_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-pingpong", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_pingpong);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_2_lfo_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-2-lfo-frequency", &port,
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
ags_abyss_synth_seq_3_modulation_0_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_0;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_0 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_0);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_1_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_1;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_1 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_1);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_2_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_2;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_2 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_2);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_3_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_3;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_3 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_3);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_4_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_4;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_4 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-4", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_4);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_5_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_5;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_5 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-5", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_5);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_6_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_6;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_6 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-6", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_6);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_7_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_7;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_7 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-7", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_7);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_8_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_8;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_8 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-8", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_8);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_9_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_9;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_9 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-9", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_9);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_10_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_10;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_10 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-10", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_10);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_11_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_11;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_11 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-11", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_11);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_12_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_12;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_12 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-12", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_12);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_13_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_13;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_13 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-13", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_13);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_14_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_14;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_14 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-14", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_14);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_modulation_15_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_modulation_15;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_modulation_15 = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-modulation-15", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_modulation_15);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_pingpong_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble seq_pingpong;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  seq_pingpong = gtk_check_button_get_active(check_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-pingpong", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) seq_pingpong);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_seq_3_lfo_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-seq-3-lfo-frequency", &port,
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
ags_abyss_synth_modulation_matrix_callback(AgsModulationMatrix *modulation_matrix,
					   gint x, gint y,
					   AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double lfo_1_tuning;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_1_tuning = ags_dial_get_value(abyss_synth->lfo_1_tuning);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    gint64 sends[AGS_ABYSS_SYNTH_SENDS_COUNT], port_sends[AGS_ABYSS_SYNTH_SENDS_COUNT];

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
    }else if(y == 2){ /* env-2 */
      g_object_get(recall->data,
		   "synth-0-env-2-sends", &port,
		   NULL);
    }else if(y == 3){ /* env-3 */
      g_object_get(recall->data,
		   "synth-0-env-3-sends", &port,
		   NULL);
    }else if(y == 4){ /* lfo-0 */
      g_object_get(recall->data,
		   "synth-0-lfo-0-sends", &port,
		   NULL);
    }else if(y == 5){ /* lfo-1 */
      g_object_get(recall->data,
		   "synth-0-lfo-1-sends", &port,
		   NULL);
    }else if(y == 6){ /* lfo-2 */
      g_object_get(recall->data,
		   "synth-0-lfo-2-sends", &port,
		   NULL);
    }else if(y == 7){ /* lfo-3 */
      g_object_get(recall->data,
		   "synth-0-lfo-3-sends", &port,
		   NULL);
    }else if(y == 8){ /* seq-0 */
      g_object_get(recall->data,
		   "synth-0-seq-0-sends", &port,
		   NULL);
    }else if(y == 9){ /* seq-1 */
      g_object_get(recall->data,
		   "synth-0-seq-1-sends", &port,
		   NULL);
    }else if(y == 10){ /* seq-2 */
      g_object_get(recall->data,
		   "synth-0-seq-2-sends", &port,
		   NULL);
    }else if(y == 11){ /* seq-3 */
      g_object_get(recall->data,
		   "synth-0-seq-3-sends", &port,
		   NULL);
    }else if(y == 12){ /* noise 0 */
      g_object_get(recall->data,
		   "synth-0-pink-noise-0-sends", &port,
		   NULL);
    }else if(y == 13){ /* noise 1 */
      g_object_get(recall->data,
		   "synth-0-pink-noise-1-sends", &port,
		   NULL);
    }

    memset(&(sends[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(port_sends[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

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
      // memcpy(&(sends[0]), port_sends, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint));

      for(i = 0, j = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
	if(ags_modulation_matrix_get_enabled(abyss_synth->modulation_matrix,
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
ags_abyss_synth_osc_0_oscillator_callback(GObject *gobject,
					  GParamSpec *pspec,
					  AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  oscillator = gtk_drop_down_get_selected((GtkDropDown *) gobject);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_osc_0_octave_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_0_octave;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_0_octave = ags_dial_get_value(abyss_synth->osc_0_octave);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_osc_0_key_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_0_key;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_0_key = ags_dial_get_value(abyss_synth->osc_0_key);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_osc_0_phase_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_0_phase;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_0_phase = ags_dial_get_value(abyss_synth->osc_0_phase);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_osc_0_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_0_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_0_volume = ags_dial_get_value(abyss_synth->osc_0_volume);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_osc_0_low_pass_0_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gboolean osc_0_low_pass_0;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_0_low_pass_0 = gtk_check_button_get_active(check_button);

  if(osc_0_low_pass_0 == FALSE){
    return;
  }

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-0-low-pass-sends", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) AGS_ABYSS_SYNTH_OSC_LOW_PASS_0);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_0_low_pass_1_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gboolean osc_0_low_pass_1;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_0_low_pass_1 = gtk_check_button_get_active(check_button);

  if(osc_0_low_pass_1 == FALSE){
    return;
  }

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-0-low-pass-sends", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) AGS_ABYSS_SYNTH_OSC_LOW_PASS_1);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_0_no_low_pass_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gboolean osc_0_no_low_pass;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_0_no_low_pass = gtk_check_button_get_active(check_button);

  if(osc_0_no_low_pass == FALSE){
    return;
  }

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-0-low-pass-sends", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) AGS_ABYSS_SYNTH_OSC_NO_LOW_PASS);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_1_oscillator_callback(GObject *gobject,
					  GParamSpec *pspec,
					  AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  oscillator = gtk_drop_down_get_selected((GtkDropDown *) gobject);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_osc_1_octave_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_1_octave;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_1_octave = ags_dial_get_value(abyss_synth->osc_1_octave);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_osc_1_key_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_1_key;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_1_key = ags_dial_get_value(abyss_synth->osc_1_key);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_osc_1_phase_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_1_phase;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_1_phase = ags_dial_get_value(abyss_synth->osc_1_phase);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_osc_1_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_1_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_1_volume = ags_dial_get_value(abyss_synth->osc_1_volume);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_osc_1_low_pass_0_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gboolean osc_1_low_pass_0;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_1_low_pass_0 = gtk_check_button_get_active(check_button);

  if(osc_1_low_pass_0 == FALSE){
    return;
  }

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-1-low-pass-sends", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) AGS_ABYSS_SYNTH_OSC_LOW_PASS_0);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_1_low_pass_1_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gboolean osc_1_low_pass_1;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_1_low_pass_1 = gtk_check_button_get_active(check_button);

  if(osc_1_low_pass_1 == FALSE){
    return;
  }

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-1-low-pass-sends", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) AGS_ABYSS_SYNTH_OSC_LOW_PASS_1);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_1_no_low_pass_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gboolean osc_1_no_low_pass;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_1_no_low_pass = gtk_check_button_get_active(check_button);

  if(osc_1_no_low_pass == FALSE){
    return;
  }

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-1-low-pass-sends", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) AGS_ABYSS_SYNTH_OSC_NO_LOW_PASS);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_2_oscillator_callback(GObject *gobject,
					  GParamSpec *pspec,
					  AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  oscillator = gtk_drop_down_get_selected((GtkDropDown *) gobject);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-2-oscillator", &port,
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
ags_abyss_synth_osc_2_octave_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_2_octave;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_2_octave = ags_dial_get_value(abyss_synth->osc_2_octave);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-2-octave", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_2_octave);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_2_key_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_2_key;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_2_key = ags_dial_get_value(abyss_synth->osc_2_key);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-2-key", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_2_key);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_2_phase_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_2_phase;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_2_phase = ags_dial_get_value(abyss_synth->osc_2_phase);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-2-phase", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_2_phase);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_2_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_2_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_2_volume = ags_dial_get_value(abyss_synth->osc_2_volume);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-2-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_2_volume);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_2_low_pass_0_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gboolean osc_2_low_pass_0;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_2_low_pass_0 = gtk_check_button_get_active(check_button);

  if(osc_2_low_pass_0 == FALSE){
    return;
  }

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-2-low-pass-sends", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) AGS_ABYSS_SYNTH_OSC_LOW_PASS_0);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_2_low_pass_1_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gboolean osc_2_low_pass_1;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_2_low_pass_1 = gtk_check_button_get_active(check_button);

  if(osc_2_low_pass_1 == FALSE){
    return;
  }

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-2-low-pass-sends", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) AGS_ABYSS_SYNTH_OSC_LOW_PASS_1);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_2_no_low_pass_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gboolean osc_2_no_low_pass;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_2_no_low_pass = gtk_check_button_get_active(check_button);

  if(osc_2_no_low_pass == FALSE){
    return;
  }

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-2-low-pass-sends", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) AGS_ABYSS_SYNTH_OSC_NO_LOW_PASS);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_3_oscillator_callback(GObject *gobject,
					  GParamSpec *pspec,
					  AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  oscillator = gtk_drop_down_get_selected((GtkDropDown *) gobject);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-3-oscillator", &port,
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
ags_abyss_synth_osc_3_octave_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_3_octave;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_3_octave = ags_dial_get_value(abyss_synth->osc_3_octave);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-3-octave", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_3_octave);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_3_key_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_3_key;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_3_key = ags_dial_get_value(abyss_synth->osc_3_key);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-3-key", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_3_key);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_3_phase_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_3_phase;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_3_phase = ags_dial_get_value(abyss_synth->osc_3_phase);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-3-phase", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_3_phase);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_3_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double osc_3_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_3_volume = ags_dial_get_value(abyss_synth->osc_3_volume);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-3-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) osc_3_volume);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_3_low_pass_0_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gboolean osc_3_low_pass_0;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_3_low_pass_0 = gtk_check_button_get_active(check_button);

  if(osc_3_low_pass_0 == FALSE){
    return;
  }

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-3-low-pass-sends", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) AGS_ABYSS_SYNTH_OSC_LOW_PASS_0);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_3_low_pass_1_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gboolean osc_3_low_pass_1;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_3_low_pass_1 = gtk_check_button_get_active(check_button);

  if(osc_3_low_pass_1 == FALSE){
    return;
  }

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-3-low-pass-sends", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) AGS_ABYSS_SYNTH_OSC_LOW_PASS_1);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_osc_3_no_low_pass_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gboolean osc_3_no_low_pass;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  osc_3_no_low_pass = gtk_check_button_get_active(check_button);

  if(osc_3_no_low_pass == FALSE){
    return;
  }

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-osc-3-low-pass-sends", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) AGS_ABYSS_SYNTH_OSC_NO_LOW_PASS);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_ring_0_enabled_callback(GtkCheckButton *button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble ring_0_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  ring_0_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-ring-0-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) ring_0_enabled);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_ring_0_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble ring_0_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  ring_0_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-ring-0-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) ring_0_tuning);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_ring_0_drive_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble ring_0_drive;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  ring_0_drive = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-ring-0-drive", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) ring_0_drive);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_ring_0_mix_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble ring_0_mix;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  ring_0_mix = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-ring-0-mix", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) ring_0_mix);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_ring_0_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble ring_0_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  ring_0_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-ring-0-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) ring_0_gain);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_ring_1_enabled_callback(GtkCheckButton *button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble ring_1_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  ring_1_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-ring-1-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) ring_1_enabled);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_ring_1_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble ring_1_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  ring_1_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-ring-1-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) ring_1_tuning);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_ring_1_drive_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble ring_1_drive;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  ring_1_drive = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-ring-1-drive", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) ring_1_drive);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_ring_1_mix_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble ring_1_mix;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  ring_1_mix = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-ring-1-mix", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) ring_1_mix);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_ring_1_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble ring_1_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  ring_1_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-0-ring-1-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) ring_1_gain);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_abyss_synth_pitch_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double pitch_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  pitch_tuning = ags_dial_get_value(abyss_synth->pitch_tuning);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  double volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  volume = ags_dial_get_value(abyss_synth->volume);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_low_pass_0_cut_off_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_low_pass_0_filter_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_low_pass_0_no_clip_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_low_pass_1_cut_off_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_low_pass_1_filter_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_low_pass_1_no_clip_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_amplifier_0_amp_0_gain_callback(GtkRange *range, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_amplifier_0_amp_1_gain_callback(GtkRange *range, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_amplifier_0_amp_2_gain_callback(GtkRange *range, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_amplifier_0_amp_3_gain_callback(GtkRange *range, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_amplifier_0_filter_gain_callback(GtkRange *range, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  volume = gtk_range_get_value(range);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_chorus_enabled_callback(GtkButton *button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  chorus_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_chorus_input_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_input_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  chorus_input_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_chorus_output_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_output_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  chorus_output_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_chorus_lfo_oscillator_callback(GObject *gobject,
					       GParamSpec *pspec,
					       AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gint lfo_oscillator;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  lfo_oscillator = gtk_drop_down_get_selected((GtkDropDown *) gobject);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-lfo-oscillator", &port,
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
ags_abyss_synth_chorus_lfo_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  chorus_lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_chorus_depth_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  chorus_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_chorus_mix_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_mix;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  chorus_mix = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
ags_abyss_synth_chorus_delay_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_delay;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(abyss_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  chorus_delay = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
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
