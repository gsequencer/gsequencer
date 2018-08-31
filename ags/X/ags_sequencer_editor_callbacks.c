/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/ags_sequencer_editor_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <ags/config.h>

void
ags_sequencer_editor_backend_changed_callback(GtkComboBox *combo,
					      AgsSequencerEditor *sequencer_editor)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

  if(str != NULL){
    if(!g_ascii_strncasecmp(str,
			    "core-audio",
			    5)){
      ags_sequencer_editor_load_core_audio_card(sequencer_editor);

      gtk_widget_show_all((GtkWidget *) sequencer_editor->source_hbox);
    }else if(!g_ascii_strncasecmp(str,
			    "jack",
			    5)){
      ags_sequencer_editor_load_jack_card(sequencer_editor);

      gtk_widget_show_all((GtkWidget *) sequencer_editor->source_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "alsa",
				  5)){
      ags_sequencer_editor_load_alsa_card(sequencer_editor);

      gtk_widget_hide((GtkWidget *) sequencer_editor->source_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "oss",
				  4)){
      ags_sequencer_editor_load_oss_card(sequencer_editor);

      gtk_widget_hide((GtkWidget *) sequencer_editor->source_hbox);
    }
  }
}

void
ags_sequencer_editor_card_changed_callback(GtkComboBox *combo,
					   AgsSequencerEditor *sequencer_editor)
{
  AgsWindow *window;

  GObject *sequencer;

  GtkTreeIter current;
  
  gchar *str;
  gchar *card;

  gboolean use_alsa;
  guint channels, channels_min, channels_max;
  guint rate, rate_min, rate_max;
  guint buffer_size, buffer_size_min, buffer_size_max;

  GError *error;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(sequencer_editor),
							      AGS_TYPE_PREFERENCES))->window);
  sequencer = sequencer_editor->sequencer;

  /*  */
  use_alsa = FALSE;

  str = NULL;
  
  if(AGS_IS_CORE_AUDIO_MIDIIN(sequencer)){
    str = "core-audio";
  }else if(AGS_IS_JACK_MIDIIN(sequencer)){
    str = "jack";
  }else if(AGS_IS_MIDIIN(sequencer)){
    if((AGS_MIDIIN_ALSA & (AGS_MIDIIN(sequencer)->flags)) != 0){
      str = "alsa";
    }else if((AGS_MIDIIN_OSS & (AGS_MIDIIN(sequencer)->flags)) != 0){
      str = "oss";
    }
  }

  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "alsa",
			  5)){
    use_alsa = TRUE;
  }

  card = gtk_combo_box_text_get_active_text(sequencer_editor->card);
  
  if(card != NULL &&
     use_alsa){
    if(index(card,
	     ',') != NULL){
      str = g_strndup(card,
		      index(card,
			    ',') - card);

      g_free(card);      
      card = str;
    }
  }
  
  /* reset dialog */
  if(card != NULL){
    ags_sequencer_set_device(AGS_SEQUENCER(sequencer),
			     card);
  }
}

void
ags_sequencer_editor_add_source_callback(GtkWidget *button,
				       AgsSequencerEditor *sequencer_editor)
{
  ags_sequencer_editor_add_source(sequencer_editor,
				  NULL);
}

void
ags_sequencer_editor_remove_source_callback(GtkWidget *button,
					  AgsSequencerEditor *sequencer_editor)
{
  ags_sequencer_editor_remove_source(sequencer_editor,
				     gtk_combo_box_text_get_active_text(sequencer_editor->card));
}
