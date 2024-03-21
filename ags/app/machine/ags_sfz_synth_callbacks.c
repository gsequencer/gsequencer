/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/machine/ags_sfz_synth_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/i18n.h>

#include <complex.h>
#include <math.h>

void ags_sfz_synth_open_dialog_response_callback(AgsFileDialog *file_dialog, gint response,
						 AgsMachine *machine);

void
ags_sfz_synth_destroy_callback(GtkWidget *widget, AgsSFZSynth *sfz_synth)
{
  if(sfz_synth->open_dialog != NULL){
    gtk_window_destroy((GtkWindow *) sfz_synth->open_dialog);
  }
}

void
ags_sfz_synth_open_clicked_callback(GtkWidget *widget, AgsSFZSynth *sfz_synth)
{
  AgsWindow *window;
  AgsFileDialog *file_dialog;
  AgsFileWidget *file_widget;

  AgsApplicationContext *application_context;

  /* get application context */  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  file_dialog = (AgsFileDialog *) ags_file_dialog_new((GtkWindow *) window,
						      i18n("open Soundfont2 file"));

  file_widget = ags_file_dialog_get_file_widget(file_dialog);

  ags_file_widget_add_bookmark(file_widget,
			       "/usr/share/sounds/sfz");
  
  sfz_synth->open_dialog = (GtkWidget *) file_dialog;

  gtk_widget_set_visible((GtkWidget *) file_dialog,
			 TRUE);

  g_signal_connect((GObject *) file_dialog, "response",
		   G_CALLBACK(ags_sfz_synth_open_dialog_response_callback), AGS_MACHINE(sfz_synth));
}

void
ags_sfz_synth_open_dialog_response_callback(AgsFileDialog *file_dialog, gint response,
					    AgsMachine *machine)
{
  AgsSFZSynth *sfz_synth;

  sfz_synth = AGS_SFZ_SYNTH(machine);

  if(response == GTK_RESPONSE_ACCEPT){
    gchar *filename;

    filename = ags_file_widget_get_filename(file_dialog->file_widget);

    gtk_editable_set_text(GTK_EDITABLE(sfz_synth->filename),
			  filename);

    ags_sfz_synth_open_filename(sfz_synth,
				filename);
  }

  sfz_synth->open_dialog = NULL;

  gtk_window_destroy((GtkWindow *) file_dialog);
}

void
ags_sfz_synth_synth_octave_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble octave;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  octave = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-octave", &port,
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
ags_sfz_synth_synth_key_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble key;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  key = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-key", &port,
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
ags_sfz_synth_synth_volume_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-volume", &port,
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
ags_sfz_synth_chorus_enabled_callback(GtkButton *button, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_enabled;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  chorus_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
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
ags_sfz_synth_chorus_input_volume_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_input_volume;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  chorus_input_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
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
ags_sfz_synth_chorus_output_volume_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_output_volume;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  chorus_output_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
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
ags_sfz_synth_chorus_lfo_oscillator_callback(GtkComboBox *combo_box, AgsSFZSynth *sfz_synth)
{  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  //TODO:JK: implement me
}

void
ags_sfz_synth_chorus_lfo_frequency_callback(GtkSpinButton *spin_button, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_lfo_frequency;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  chorus_lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
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
ags_sfz_synth_chorus_depth_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_depth;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  chorus_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
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
ags_sfz_synth_chorus_mix_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_mix;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  chorus_mix = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
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
ags_sfz_synth_chorus_delay_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_delay;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  chorus_delay = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
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
ags_sfz_synth_volume_callback(GtkRange *range, AgsSFZSynth *sfz_synth)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat volume;
  
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  volume = (gfloat) gtk_range_get_value(range);
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(sfz_synth)->audio,
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
ags_sfz_synth_tremolo_enabled_callback(GtkCheckButton *button, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

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
ags_sfz_synth_tremolo_gain_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

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
ags_sfz_synth_tremolo_lfo_depth_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_lfo_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

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
ags_sfz_synth_tremolo_lfo_freq_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_lfo_freq;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

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
ags_sfz_synth_tremolo_tuning_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

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
ags_sfz_synth_vibrato_enabled_callback(GtkCheckButton *button, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  vibrato_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
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
ags_sfz_synth_vibrato_gain_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  vibrato_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
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
ags_sfz_synth_vibrato_lfo_depth_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_lfo_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  vibrato_lfo_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
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
ags_sfz_synth_vibrato_lfo_freq_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_lfo_freq;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  vibrato_lfo_freq = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
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
ags_sfz_synth_vibrato_tuning_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  vibrato_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
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

void
ags_sfz_synth_wah_wah_enabled_callback(GtkCheckButton *button, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  gdouble wah_wah_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  wah_wah_enabled = (gdouble) gtk_check_button_get_active((GtkCheckButton *) button);

  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
  
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-enabled", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_enabled);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
}

void
ags_sfz_synth_wah_wah_length_callback(GtkComboBox *combo_box, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  gdouble wah_wah_length;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  wah_wah_length = gtk_combo_box_get_active(sfz_synth->wah_wah_length);

  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
  
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-length-mode", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_length);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
}

void
ags_sfz_synth_wah_wah_attack_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  double _Complex wah_wah_attack;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  wah_wah_attack = ags_dial_get_value(sfz_synth->wah_wah_attack_x) + ags_dial_get_value(sfz_synth->wah_wah_attack_y) * I;

  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
  
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-attack", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_attack);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
  
  gtk_widget_queue_draw(sfz_synth->wah_wah_drawing_area);
}

void
ags_sfz_synth_wah_wah_decay_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  double _Complex wah_wah_decay;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  wah_wah_decay = ags_dial_get_value(sfz_synth->wah_wah_decay_x) + ags_dial_get_value(sfz_synth->wah_wah_decay_y) * I;

  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
  
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-decay", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_decay);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
  
  gtk_widget_queue_draw(sfz_synth->wah_wah_drawing_area);
}

void
ags_sfz_synth_wah_wah_sustain_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  double _Complex wah_wah_sustain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  wah_wah_sustain = ags_dial_get_value(sfz_synth->wah_wah_sustain_x) + ags_dial_get_value(sfz_synth->wah_wah_sustain_y) * I;

  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
  
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-sustain", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_sustain);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
  
  gtk_widget_queue_draw(sfz_synth->wah_wah_drawing_area);
}

void
ags_sfz_synth_wah_wah_release_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  double _Complex wah_wah_release;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  wah_wah_release = ags_dial_get_value(sfz_synth->wah_wah_release_x) + ags_dial_get_value(sfz_synth->wah_wah_release_y) * I;

  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
  
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-release", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_release);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
  
  gtk_widget_queue_draw(sfz_synth->wah_wah_drawing_area);
}

void
ags_sfz_synth_wah_wah_ratio_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  double _Complex wah_wah_ratio;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  wah_wah_ratio = ags_dial_get_value(sfz_synth->wah_wah_ratio);
  
  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;

    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-ratio", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_ratio);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }  

  gtk_widget_queue_draw(sfz_synth->wah_wah_drawing_area);
}

void
ags_sfz_synth_wah_wah_lfo_depth_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  gdouble wah_wah_lfo_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  wah_wah_lfo_depth = ags_dial_get_value(dial);
  
  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;

    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-lfo-depth", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_lfo_depth);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }  
}

void
ags_sfz_synth_wah_wah_lfo_freq_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  gdouble wah_wah_lfo_freq;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  wah_wah_lfo_freq = ags_dial_get_value(dial);
  
  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;

    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-lfo-freq", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_lfo_freq);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }  
}

void
ags_sfz_synth_wah_wah_tuning_callback(AgsDial *dial, AgsSFZSynth *sfz_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  gdouble wah_wah_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sfz_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;

  wah_wah_tuning = ags_dial_get_value(dial);
  
  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;

    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-tuning", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_tuning);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }  
}
