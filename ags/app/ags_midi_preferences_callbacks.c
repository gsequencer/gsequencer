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

#include <ags/app/ags_midi_preferences_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_preferences.h>
#include <ags/app/ags_sequencer_editor.h>

#include <ags/i18n.h>

#include <ags/config.h>

void
ags_midi_preferences_notify_parent_callback(GObject *gobject,
					    GParamSpec *pspec,
					    gpointer user_data)
{
  //empty
}

void
ags_midi_preferences_add_input_sequencer_callback(GAction *action, GVariant *parameter,
						  AgsMidiPreferences *midi_preferences)
{
  AgsSequencerEditor *sequencer_editor;

  AgsThread *main_loop;
  
  AgsApplicationContext *application_context;

  GObject *sequencer;

  GList *start_list, *list;

  application_context = ags_application_context_get_instance();

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* sequencer */
  sequencer = NULL;
  
  /* sequencer editor */
  sequencer_editor = ags_sequencer_editor_new();

  if(sequencer != NULL){
    sequencer_editor->sequencer = sequencer;
    sequencer_editor->sequencer_thread = (GObject *) ags_thread_find_type(main_loop,
									  AGS_TYPE_SEQUENCER_THREAD);
   }
  
  ags_midi_preferences_add_sequencer_editor(midi_preferences,
					     sequencer_editor);
  
  ags_applicable_reset(AGS_APPLICABLE(sequencer_editor));
  ags_connectable_connect(AGS_CONNECTABLE(sequencer_editor));

  g_signal_connect(sequencer_editor->remove, "clicked",
		   G_CALLBACK(ags_midi_preferences_remove_sequencer_editor_callback), midi_preferences);

  gtk_widget_show((GtkWidget *) sequencer_editor);

  /* reset default card */  
  g_object_unref(main_loop);  
}

void
ags_midi_preferences_remove_sequencer_editor_callback(GtkWidget *button,
						      AgsMidiPreferences *midi_preferences)
{
  AgsSequencerEditor *sequencer_editor;
  
  sequencer_editor = (AgsSequencerEditor *) gtk_widget_get_ancestor(button,
								    AGS_TYPE_SEQUENCER_EDITOR);

  if(!AGS_IS_JACK_DEVOUT(sequencer_editor->sequencer)){
    ags_sequencer_editor_remove_sequencer(sequencer_editor,
					  sequencer_editor->sequencer);
  }

  ags_midi_preferences_remove_sequencer_editor(midi_preferences,
					       sequencer_editor);
}
