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

#include <ags/app/machine/ags_pitch_sampler_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/i18n.h>

void ags_pitch_sampler_open_response_callback(GtkWidget *widget, gint response,
					      AgsPitchSampler *pitch_sampler);

void
ags_pitch_sampler_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsPitchSampler *pitch_sampler)
{
  AgsWindow *window;

  gchar *str;
  
  if(old_parent != NULL){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) pitch_sampler, AGS_TYPE_WINDOW));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_PITCH_SAMPLER)->counter);

  g_object_set(AGS_MACHINE(pitch_sampler),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_PITCH_SAMPLER);
  g_free(str);
}

void
ags_pitch_sampler_open_callback(GtkButton *button, AgsPitchSampler *pitch_sampler)
{
  GtkFileChooserDialog *dialog;

  if(pitch_sampler->open_dialog != NULL){
    return;
  }
  
  pitch_sampler->open_dialog = 
    dialog = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(i18n("Open audio files"),
								  (GtkWindow *) gtk_widget_get_toplevel((GtkWidget *) pitch_sampler),
								  GTK_FILE_CHOOSER_ACTION_OPEN,
								  i18n("_OK"), GTK_RESPONSE_ACCEPT,
								  i18n("_Cancel"), GTK_RESPONSE_CANCEL,
								  NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog),
				       FALSE);
  gtk_widget_show_all((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response",
		   G_CALLBACK(ags_pitch_sampler_open_response_callback), pitch_sampler);
}

void
ags_pitch_sampler_open_response_callback(GtkWidget *widget, gint response,
					 AgsPitchSampler *pitch_sampler)
{
  if(response == GTK_RESPONSE_ACCEPT){
    gchar *filename;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
    gtk_entry_set_text(pitch_sampler->filename,
		       filename);
    ags_pitch_sampler_open_filename(pitch_sampler,
				    filename);
  }

  pitch_sampler->open_dialog = NULL;
  gtk_widget_destroy(widget);
}

void
ags_pitch_sampler_file_control_changed_callback(AgsPitchSamplerFile *pitch_sampler_file,
						AgsPitchSampler *pitch_sampler)
{
  if((AGS_PITCH_SAMPLER_AUTO_UPDATE & (pitch_sampler->flags)) != 0){
    ags_pitch_sampler_update(pitch_sampler);
  }
}

void
ags_pitch_sampler_update_callback(GtkButton *button, AgsPitchSampler *pitch_sampler)
{
  ags_pitch_sampler_update(pitch_sampler);
}

void
ags_pitch_sampler_enable_lfo_callback(GtkToggleButton *toggle, AgsPitchSampler *pitch_sampler)
{
  AgsAudio *audio;  
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;
  AgsPort *port;

  GList *start_play, *play;
  GList *start_recall, *recall;

  audio = AGS_MACHINE(pitch_sampler)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  while(channel != NULL){  
    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    /* play */
    play = ags_recall_find_type(start_play,
				AGS_TYPE_FX_LFO_CHANNEL);
      
    if(play != NULL){
      GValue value = {0};
      
      g_object_get(play->data,
		   "enabled", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			(gtk_toggle_button_get_active(toggle) ? 1.0: 0.0));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_play);

    /* recall */
    recall = ags_recall_find_type(start_recall,
				  AGS_TYPE_FX_LFO_CHANNEL);
      
    if(recall != NULL){
      GValue value = {0};
      
      g_object_get(recall->data,
		   "enabled", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			(gtk_toggle_button_get_active(toggle) ? 1.0: 0.0));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_recall);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

void
ags_pitch_sampler_lfo_freq_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler)
{
  AgsAudio *audio;  
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;
  AgsPort *port;

  GList *start_play, *play;
  GList *start_recall, *recall;

  audio = AGS_MACHINE(pitch_sampler)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  while(channel != NULL){  
    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    /* play */
    play = ags_recall_find_type(start_play,
				AGS_TYPE_FX_LFO_CHANNEL);
      
    if(play != NULL){
      GValue value = {0};
      
      g_object_get(play->data,
		   "lfo-freq", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_play);

    /* recall */
    recall = ags_recall_find_type(start_recall,
				  AGS_TYPE_FX_LFO_CHANNEL);
      
    if(recall != NULL){
      GValue value = {0};
      
      g_object_get(recall->data,
		   "lfo-freq", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_recall);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

void
ags_pitch_sampler_lfo_phase_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler)
{
  AgsAudio *audio;  
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;
  AgsPort *port;

  GList *start_play, *play;
  GList *start_recall, *recall;

  audio = AGS_MACHINE(pitch_sampler)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  while(channel != NULL){  
    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    /* play */
    play = ags_recall_find_type(start_play,
				AGS_TYPE_FX_LFO_CHANNEL);
      
    if(play != NULL){
      GValue value = {0};
      
      g_object_get(play->data,
		   "lfo-phase", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_play);

    /* recall */
    recall = ags_recall_find_type(start_recall,
				  AGS_TYPE_FX_LFO_CHANNEL);
      
    if(recall != NULL){
      GValue value = {0};
      
      g_object_get(recall->data,
		   "lfo-phase", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_recall);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

void
ags_pitch_sampler_lfo_depth_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler)
{
  AgsAudio *audio;  
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;
  AgsPort *port;

  GList *start_play, *play;
  GList *start_recall, *recall;

  audio = AGS_MACHINE(pitch_sampler)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  while(channel != NULL){  
    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    /* play */
    play = ags_recall_find_type(start_play,
				AGS_TYPE_FX_LFO_CHANNEL);
      
    if(play != NULL){
      GValue value = {0};
      
      g_object_get(play->data,
		   "lfo-depth", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_play);

    /* recall */
    recall = ags_recall_find_type(start_recall,
				  AGS_TYPE_FX_LFO_CHANNEL);
      
    if(recall != NULL){
      GValue value = {0};
      
      g_object_get(recall->data,
		   "lfo-depth", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_recall);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

void
ags_pitch_sampler_lfo_tuning_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler)
{
  AgsAudio *audio;  
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;
  AgsPort *port;

  GList *start_play, *play;
  GList *start_recall, *recall;

  audio = AGS_MACHINE(pitch_sampler)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  while(channel != NULL){  
    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    /* play */
    play = ags_recall_find_type(start_play,
				AGS_TYPE_FX_LFO_CHANNEL);
      
    if(play != NULL){
      GValue value = {0};
      
      g_object_get(play->data,
		   "lfo-tuning", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_play);

    /* recall */
    recall = ags_recall_find_type(start_recall,
				  AGS_TYPE_FX_LFO_CHANNEL);
      
    if(recall != NULL){
      GValue value = {0};
      
      g_object_get(recall->data,
		   "lfo-tuning", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_recall);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

void
ags_pitch_sampler_enable_aliase_callback(GtkWidget *widget, AgsPitchSampler *pitch_sampler)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat enabled;

  enabled = gtk_toggle_button_get_active(widget) ? 1.0: 0.0;
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(pitch_sampler)->audio,
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

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "enabled", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  enabled);

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
ags_pitch_sampler_aliase_a_amount_callback(AgsDial *dial, AgsPitchSampler *pitch_sampler)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat amount;

  amount = gtk_adjustment_get_value(ags_dial_get_adjustment(dial));
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(pitch_sampler)->audio,
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

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "a-amount", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  amount);

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
ags_pitch_sampler_aliase_a_phase_callback(AgsDial *dial, AgsPitchSampler *pitch_sampler)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat phase;

  phase = gtk_adjustment_get_value(ags_dial_get_adjustment(dial));
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(pitch_sampler)->audio,
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

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "a-phase", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  phase);

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
ags_pitch_sampler_aliase_b_amount_callback(AgsDial *dial, AgsPitchSampler *pitch_sampler)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat amount;

  amount = gtk_adjustment_get_value(ags_dial_get_adjustment(dial));
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(pitch_sampler)->audio,
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

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "b-amount", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  amount);

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
ags_pitch_sampler_aliase_b_phase_callback(AgsDial *dial, AgsPitchSampler *pitch_sampler)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat phase;

  phase = gtk_adjustment_get_value(ags_dial_get_adjustment(dial));
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(pitch_sampler)->audio,
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

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "b-phase", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  phase);

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
ags_pitch_sampler_volume_callback(GtkRange *range, AgsPitchSampler *pitch_sampler)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat volume;

  volume = (gfloat) gtk_range_get_value(range);
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(pitch_sampler)->audio,
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
