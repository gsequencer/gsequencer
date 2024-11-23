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

#include <ags/app/ags_soundcard_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_preferences.h>

#include <ags/config.h>

void
ags_soundcard_editor_backend_changed_callback(GtkComboBox *combo,
					      AgsSoundcardEditor *soundcard_editor)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

  if(str != NULL){
    if(!g_ascii_strncasecmp(str,
			    "wasapi",
			    6)){
      ags_soundcard_editor_show_wasapi_control(soundcard_editor);
    }else{
      ags_soundcard_editor_hide_wasapi_control(soundcard_editor);
    }
    
    if(!g_ascii_strncasecmp(str,
			    "core-audio",
			    11)){
      ags_soundcard_editor_load_core_audio_card(soundcard_editor);

      gtk_widget_show((GtkWidget *) soundcard_editor->port_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "pulse",
				  6)){
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->capability),
			       0);

      ags_soundcard_editor_load_pulse_card(soundcard_editor);

      gtk_widget_show((GtkWidget *) soundcard_editor->port_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "jack",
				  5)){
      ags_soundcard_editor_load_jack_card(soundcard_editor);

      gtk_widget_show((GtkWidget *) soundcard_editor->port_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "wasapi",
				  6)){      
      ags_soundcard_editor_load_wasapi_card(soundcard_editor);

      //      gtk_widget_hide((GtkWidget *) soundcard_editor->port_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "alsa",
				  5)){
      ags_soundcard_editor_load_alsa_card(soundcard_editor);

      //      gtk_widget_hide((GtkWidget *) soundcard_editor->port_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "oss",
				  4)){
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

  guint channels_min, channels_max;
  guint rate_min, rate_max;
  guint buffer_size_min, buffer_size_max;

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

    gtk_spin_button_set_range(soundcard_editor->audio_channels, 0.0, 24.0);
    gtk_spin_button_set_range(soundcard_editor->samplerate, 1.0, 192000.0);
    gtk_spin_button_set_range(soundcard_editor->buffer_size, 1.0, 65535.0);

    soundcard_editor->flags &= (~AGS_SOUNDCARD_EDITOR_BLOCK_CARD);

    g_error_free(error);

    gtk_widget_show(GTK_WIDGET(dialog));
    
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
  AgsSetAudioChannels *set_audio_channels;

  AgsApplicationContext *application_context;

  GObject *soundcard;

  soundcard = soundcard_editor->soundcard;

  application_context = ags_application_context_get_instance();

  /* create set output device task */
  set_audio_channels = ags_set_audio_channels_new(soundcard,
						  (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetAudioChannels */
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) set_audio_channels);
}

void
ags_soundcard_editor_samplerate_changed_callback(GtkSpinButton *spin_button,
						 AgsSoundcardEditor *soundcard_editor)
{
  AgsSetSamplerate *set_samplerate;

  AgsApplicationContext *application_context;

  GObject *soundcard;
  
  soundcard = soundcard_editor->soundcard;

  application_context = ags_application_context_get_instance();

  /* create set output device task */
  set_samplerate = ags_set_samplerate_new(soundcard,
					  (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetSamplerate */
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) set_samplerate);
}

void
ags_soundcard_editor_buffer_size_changed_callback(GtkSpinButton *spin_button,
						  AgsSoundcardEditor *soundcard_editor)
{
  AgsSetBufferSize *set_buffer_size;

  AgsApplicationContext *application_context;

  GObject *soundcard;

  soundcard = soundcard_editor->soundcard;

  application_context = ags_application_context_get_instance();

  /* create set output device task */
  set_buffer_size = ags_set_buffer_size_new(soundcard,
					    (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetBufferSize */
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) set_buffer_size);
}

void
ags_soundcard_editor_format_changed_callback(GtkComboBox *combo_box,
					     AgsSoundcardEditor *soundcard_editor)
{
  AgsSetFormat *set_format;

  AgsApplicationContext *application_context;

  GObject *soundcard;

  guint format;

  soundcard = soundcard_editor->soundcard;

  application_context = ags_application_context_get_instance();

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
  case 5:
    format = AGS_SOUNDCARD_FLOAT;
    break;
  case 6:
    format = AGS_SOUNDCARD_DOUBLE;
    break;
  default:
    g_warning("unsupported format");
    
    return;
  }

  /* create set output device task */
  set_format = ags_set_format_new((GObject *) soundcard,
				  format);

  /* append AgsSetBufferSize */
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) set_format);
}
