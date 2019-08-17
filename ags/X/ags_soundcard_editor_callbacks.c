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

#include <ags/X/ags_soundcard_editor_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/config.h>

void
ags_soundcard_editor_backend_changed_callback(GtkComboBox *combo,
					      AgsSoundcardEditor *soundcard_editor)
{
  gchar *str;

  gtk_widget_set_sensitive(GTK_WIDGET(soundcard_editor->capability),
			   TRUE);
  
  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

  if(str != NULL){
    if(!g_ascii_strncasecmp(str,
			    "core-audio",
			    6)){
      gtk_widget_set_sensitive(GTK_WIDGET(soundcard_editor->use_cache),
			       TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(soundcard_editor->cache_buffer_size),
			       TRUE);

      gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->buffer_size,
			       FALSE);
  
      gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->samplerate,
			       FALSE);

      ags_soundcard_editor_load_core_audio_card(soundcard_editor);

      gtk_widget_show_all((GtkWidget *) soundcard_editor->port_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "pulse",
				  6)){
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->capability),
			       0);
      gtk_widget_set_sensitive(GTK_WIDGET(soundcard_editor->capability),
			       FALSE);
      
      gtk_widget_set_sensitive(GTK_WIDGET(soundcard_editor->use_cache),
			       TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(soundcard_editor->cache_buffer_size),
			       TRUE);

      gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->buffer_size,
			       FALSE);
  
      gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->samplerate,
			       FALSE);

      ags_soundcard_editor_load_pulse_card(soundcard_editor);

      gtk_widget_show_all((GtkWidget *) soundcard_editor->port_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "jack",
				  5)){
      gtk_widget_set_sensitive(GTK_WIDGET(soundcard_editor->use_cache),
			       FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(soundcard_editor->cache_buffer_size),
			       FALSE);

      gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->buffer_size,
			       FALSE);
  
      gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->samplerate,
			       FALSE);

      ags_soundcard_editor_load_jack_card(soundcard_editor);

      gtk_widget_show_all((GtkWidget *) soundcard_editor->port_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "alsa",
				  5)){
      gtk_widget_set_sensitive(GTK_WIDGET(soundcard_editor->use_cache),
			       FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(soundcard_editor->cache_buffer_size),
			       FALSE);

      gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->buffer_size,
			       TRUE);
  
      gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->samplerate,
			       TRUE);

      ags_soundcard_editor_load_alsa_card(soundcard_editor);

      //      gtk_widget_hide((GtkWidget *) soundcard_editor->port_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "oss",
				  4)){
      gtk_widget_set_sensitive(GTK_WIDGET(soundcard_editor->use_cache),
			       FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(soundcard_editor->cache_buffer_size),
			       FALSE);

      gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->buffer_size,
			       TRUE);

      gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->samplerate,
			       TRUE);
      
      ags_soundcard_editor_load_oss_card(soundcard_editor);

      //      gtk_widget_hide((GtkWidget *) soundcard_editor->port_hbox);
    }
  }
}

void
ags_soundcard_editor_card_changed_callback(GtkComboBox *combo,
					   AgsSoundcardEditor *soundcard_editor)
{
  GObject *soundcard;
  
  gchar *card;

  guint channels, channels_min, channels_max;
  guint rate, rate_min, rate_max;
  guint buffer_size, buffer_size_min, buffer_size_max;

  GError *error;

  if((AGS_SOUNDCARD_EDITOR_BLOCK_CARD & (soundcard_editor->flags)) != 0){
    return;
  }

  soundcard_editor->flags |= AGS_SOUNDCARD_EDITOR_BLOCK_CARD;
  
  soundcard = soundcard_editor->soundcard;

  /*  */
  card = gtk_combo_box_text_get_active_text(soundcard_editor->card);
  
  /* reset dialog */
  error = NULL;
  ags_soundcard_pcm_info(AGS_SOUNDCARD(soundcard),
			 card,
			 &channels_min, &channels_max,
			 &rate_min, &rate_max,
			 &buffer_size_min, &buffer_size_max,
			 &error);

  
  if(error != NULL){
    GtkMessageDialog *dialog;

    dialog = (GtkMessageDialog *) gtk_message_dialog_new((GtkWindow *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
											       AGS_TYPE_PREFERENCES),
							 GTK_DIALOG_MODAL,
							 GTK_MESSAGE_ERROR,
							 GTK_BUTTONS_CLOSE,
							 "%s", error->message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(GTK_WIDGET(dialog));

    gtk_spin_button_set_range(soundcard_editor->audio_channels, 0.0, 24.0);
    gtk_spin_button_set_range(soundcard_editor->samplerate, 1.0, 192000.0);
    gtk_spin_button_set_range(soundcard_editor->buffer_size, 1.0, 65535.0);

    soundcard_editor->flags &= (~AGS_SOUNDCARD_EDITOR_BLOCK_CARD);

    g_error_free(error);
    
    return;
  }

  if(card != NULL){
    ags_soundcard_set_device(AGS_SOUNDCARD(soundcard),
			     card);
    
    gtk_spin_button_set_range(soundcard_editor->audio_channels,
			      channels_min, channels_max);
    gtk_spin_button_set_range(soundcard_editor->samplerate,
			      rate_min, rate_max);
    gtk_spin_button_set_range(soundcard_editor->buffer_size,
			      buffer_size_min, buffer_size_max);
  }

  soundcard_editor->flags &= (~AGS_SOUNDCARD_EDITOR_BLOCK_CARD);
}

void
ags_soundcard_editor_add_port_callback(GtkWidget *button,
				       AgsSoundcardEditor *soundcard_editor)
{
  ags_soundcard_editor_add_port(soundcard_editor,
				NULL);
}

void
ags_soundcard_editor_remove_port_callback(GtkWidget *button,
					  AgsSoundcardEditor *soundcard_editor)
{
  ags_soundcard_editor_remove_port(soundcard_editor,
				   gtk_combo_box_text_get_active_text(soundcard_editor->card));
}

void
ags_soundcard_editor_audio_channels_changed_callback(GtkSpinButton *spin_button,
						     AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  GObject *soundcard;
  AgsSetAudioChannels *set_audio_channels;

  AgsApplicationContext *application_context;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  soundcard = window->soundcard;

  application_context = (AgsApplicationContext *) window->application_context;

  /* create set output device task */
  set_audio_channels = ags_set_audio_channels_new(soundcard,
						  (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetAudioChannels */
  ags_xorg_application_context_schedule_task(application_context,
					     (GObject *) set_audio_channels);
}

void
ags_soundcard_editor_samplerate_changed_callback(GtkSpinButton *spin_button,
						 AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  GObject *soundcard;
  AgsSetSamplerate *set_samplerate;

  AgsApplicationContext *application_context;
  
  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  soundcard = soundcard_editor->soundcard;

  application_context = (AgsApplicationContext *) window->application_context;

  /* create set output device task */
  set_samplerate = ags_set_samplerate_new(soundcard,
					  (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetSamplerate */
  ags_xorg_application_context_schedule_task(application_context,
					     (GObject *) set_samplerate);
}

void
ags_soundcard_editor_buffer_size_changed_callback(GtkSpinButton *spin_button,
						  AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  GObject *soundcard;
  AgsSetBufferSize *set_buffer_size;

  AgsApplicationContext *application_context;
  
  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  soundcard = soundcard_editor->soundcard;

  application_context = (AgsApplicationContext *) window->application_context;

  /* create set output device task */
  set_buffer_size = ags_set_buffer_size_new(soundcard,
					    (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetBufferSize */
  ags_xorg_application_context_schedule_task(application_context,
					     (GObject *) set_buffer_size);
}

void
ags_soundcard_editor_format_changed_callback(GtkComboBox *combo_box,
					     AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  GObject *soundcard;
  AgsSetFormat *set_format;

  AgsApplicationContext *application_context;

  guint format;
  
  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);

  soundcard = soundcard_editor->soundcard;

  application_context = (AgsApplicationContext *) window->application_context;

  /* format */
  switch(gtk_combo_box_get_active(GTK_COMBO_BOX(soundcard_editor->format))){
  case 0:
    format = AGS_SOUNDCARD_SIGNED_8_BIT;
    break;
  case 1:
    format = AGS_SOUNDCARD_SIGNED_16_BIT;
    break;
  case 2:
    format = AGS_SOUNDCARD_SIGNED_24_BIT;
    break;
  case 3:
    format = AGS_SOUNDCARD_SIGNED_32_BIT;
    break;
  case 4:
    format = AGS_SOUNDCARD_SIGNED_64_BIT;
    break;
  default:
    g_warning("unsupported format");
    
    return;
  }

  /* create set output device task */
  set_format = ags_set_format_new((GObject *) soundcard,
				  format);

  /* append AgsSetBufferSize */
  ags_xorg_application_context_schedule_task(application_context,
					     (GObject *) set_format);
}
