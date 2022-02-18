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

#include <ags/app/ags_sequencer_editor_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_preferences.h>

#include <ags/config.h>

void
ags_sequencer_editor_backend_changed_callback(GtkComboBox *combo,
					      AgsSequencerEditor *sequencer_editor)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

  if(str != NULL){
    if(!g_ascii_strncasecmp(str,
			    "jack",
			    5)){
      ags_sequencer_editor_load_jack_card(sequencer_editor);

      gtk_widget_show_all((GtkWidget *) sequencer_editor->jack_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "alsa",
				  5)){
      ags_sequencer_editor_load_alsa_card(sequencer_editor);

      gtk_widget_hide((GtkWidget *) sequencer_editor->jack_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "oss",
				  4)){
      ags_sequencer_editor_load_oss_card(sequencer_editor);

      gtk_widget_hide((GtkWidget *) sequencer_editor->jack_hbox);
    }
  }
}

void
ags_sequencer_editor_card_changed_callback(GtkComboBox *combo,
					   AgsSequencerEditor *sequencer_editor)
{
  GObject *sequencer;

  gchar *str;
  gchar *card;

  gboolean use_alsa;

  sequencer = sequencer_editor->sequencer;

  /*  */
  use_alsa = FALSE;

  str = NULL;
  
  if(AGS_IS_JACK_MIDIIN(sequencer)){
    str = "jack";
  }else if(AGS_IS_ALSA_MIDIIN(sequencer)){
    str = "alsa";
  }else if(AGS_IS_OSS_MIDIIN(sequencer)){
    str = "oss";
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
    if(strchr(card,
	      ',') != NULL){
      str = g_strndup(card,
		      strchr(card,
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
ags_sequencer_editor_add_jack_callback(GtkWidget *button,
				       AgsSequencerEditor *sequencer_editor)
{
  ags_sequencer_editor_add_source(sequencer_editor,
				  NULL);
}

void
ags_sequencer_editor_remove_jack_callback(GtkWidget *button,
					  AgsSequencerEditor *sequencer_editor)
{
  ags_sequencer_editor_remove_source(sequencer_editor,
				     gtk_combo_box_text_get_active_text(sequencer_editor->card));
}
