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

#include <ags/X/ags_midi_preferences_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>
#include <ags/X/ags_sequencer_editor.h>

#include <ags/config.h>

int
ags_midi_preferences_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMidiPreferences *midi_preferences)
{  
  AgsPreferences *preferences;

  if(old_parent != NULL){
    return(0);
  }

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(midi_preferences),
							   AGS_TYPE_PREFERENCES);

  midi_preferences->add = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_end((GtkBox *) GTK_DIALOG(preferences)->action_area,
		   (GtkWidget *) midi_preferences->add,
		   TRUE, FALSE,
		   0);  

  return(0);
}

void
ags_midi_preferences_add_callback(GtkWidget *widget, AgsMidiPreferences *midi_preferences)
{
  AgsWindow *window;
  AgsPreferences *preferences;
  AgsSequencerEditor *sequencer_editor;

  AgsSequencerThread *sequencer_thread;

  AgsThread *main_loop;
  
  AgsApplicationContext *application_context;

  GObject *sequencer;

  GList *start_list, *list;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(midi_preferences),
							   AGS_TYPE_PREFERENCES);
  window = (AgsWindow *) preferences->window;

  application_context = (AgsApplicationContext *) window->application_context;
  g_object_get(application_context,
	       "main-loop", &main_loop,
	       NULL);
  
  /* retrieve first sequencer */
  sequencer = NULL;

  list =
    start_list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
  
  if(list != NULL){
    sequencer = list->data;
  }

  g_list_free(start_list);
  
  /* sequencer editor */
  sequencer_editor = ags_sequencer_editor_new();

  if(sequencer != NULL){
    sequencer_editor->sequencer = sequencer;
    sequencer_editor->sequencer_thread = (GObject *) ags_thread_find_type(main_loop,
									  AGS_TYPE_SEQUENCER_THREAD);
  }
  
  gtk_box_pack_start((GtkBox *) midi_preferences->sequencer_editor,
		     (GtkWidget *) sequencer_editor,
		     FALSE, FALSE,
		     0);
  
  ags_applicable_reset(AGS_APPLICABLE(sequencer_editor));
  ags_connectable_connect(AGS_CONNECTABLE(sequencer_editor));
  g_signal_connect(sequencer_editor->remove, "clicked",
		   G_CALLBACK(ags_midi_preferences_remove_sequencer_editor_callback), midi_preferences);
  gtk_widget_show_all((GtkWidget *) sequencer_editor);
}

void
ags_midi_preferences_remove_sequencer_editor_callback(GtkWidget *button,
						      AgsMidiPreferences *midi_preferences)
{
  AgsWindow *window;
  AgsPreferences *preferences;
  AgsSequencerEditor *sequencer_editor;

  GList *list;
  GObject *sequencer;
  
  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(midi_preferences),
							   AGS_TYPE_PREFERENCES);
  window = (AgsWindow *) preferences->window;

  sequencer_editor = (AgsSequencerEditor *) gtk_widget_get_ancestor(button,
								    AGS_TYPE_SEQUENCER_EDITOR);

  if(!AGS_IS_JACK_DEVOUT(sequencer_editor->sequencer)){
    ags_sequencer_editor_remove_sequencer(sequencer_editor,
					  sequencer_editor->sequencer);
  }
  
  gtk_widget_destroy((GtkWidget *) sequencer_editor);  
}
