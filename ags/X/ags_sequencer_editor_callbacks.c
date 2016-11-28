/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_task.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_midiin.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/config.h>

void
ags_sequencer_editor_backend_changed_callback(GtkComboBox *combo,
					      AgsSequencerEditor *sequencer_editor)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

  if(str != NULL){
    if(!g_ascii_strncasecmp(str,
			    "jack\0",
			    5)){
      ags_sequencer_editor_load_jack_card(sequencer_editor);

      gtk_widget_show_all((GtkWidget *) sequencer_editor->jack_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "alsa\0",
				  5)){
      ags_sequencer_editor_load_alsa_card(sequencer_editor);

      gtk_widget_hide((GtkWidget *) sequencer_editor->jack_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "oss\0",
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
  //TODO:JK: implement me
}

void
ags_sequencer_editor_add_jack_callback(GtkWidget *button,
				       AgsSequencerEditor *sequencer_editor)
{
  //TODO:JK: implement me
}

void
ags_sequencer_editor_remove_jack_callback(GtkWidget *button,
					  AgsSequencerEditor *sequencer_editor)
{
  //TODO:JK: implement me
}
