/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/app/ags_midi_preferences.h>
#include <ags/app/ags_midi_preferences_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_preferences.h>
#include <ags/app/ags_sequencer_editor.h>

#include <ags/config.h>

void ags_midi_preferences_class_init(AgsMidiPreferencesClass *midi_preferences);
void ags_midi_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_midi_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_midi_preferences_init(AgsMidiPreferences *midi_preferences);

void ags_midi_preferences_connect(AgsConnectable *connectable);
void ags_midi_preferences_disconnect(AgsConnectable *connectable);

void ags_midi_preferences_set_update(AgsApplicable *applicable, gboolean update);
void ags_midi_preferences_apply(AgsApplicable *applicable);
void ags_midi_preferences_reset(AgsApplicable *applicable);
static void ags_midi_preferences_finalize(GObject *gobject);

/**
 * SECTION:ags_midi_preferences
 * @short_description: A composite widget to do midi related preferences
 * @title: AgsMidiPreferences
 * @section_id: 
 * @include: ags/X/ags_midi_preferences.h
 *
 * #AgsMidiPreferences enables you to make preferences of sequencer, midi channels,
 * samplerate and buffer size.
 */

static gpointer ags_midi_preferences_parent_class = NULL;

GType
ags_midi_preferences_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_preferences = 0;

    static const GTypeInfo ags_midi_preferences_info = {
      sizeof (AgsMidiPreferencesClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_preferences_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiPreferences),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_preferences_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_midi_preferences_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_midi_preferences_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_midi_preferences = g_type_register_static(GTK_TYPE_BOX,
						       "AgsMidiPreferences", &ags_midi_preferences_info,
						       0);
    
    g_type_add_interface_static(ags_type_midi_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_midi_preferences,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_preferences);
  }

  return g_define_type_id__volatile;
}

void
ags_midi_preferences_class_init(AgsMidiPreferencesClass *midi_preferences)
{
  GObjectClass *gobject;

  ags_midi_preferences_parent_class = g_type_class_peek_parent(midi_preferences);

  /* GtkObjectClass */
  gobject = (GObjectClass *) midi_preferences;

  gobject->finalize = ags_midi_preferences_finalize;
}

void
ags_midi_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_midi_preferences_connect;
  connectable->disconnect = ags_midi_preferences_disconnect;
}

void
ags_midi_preferences_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_midi_preferences_set_update;
  applicable->apply = ags_midi_preferences_apply;
  applicable->reset = ags_midi_preferences_reset;
}

void
ags_midi_preferences_init(AgsMidiPreferences *midi_preferences)
{
  GtkScrolledWindow *scrolled_window;
  
  gtk_orientable_set_orientation(GTK_ORIENTABLE(midi_preferences),
				 GTK_ORIENTATION_VERTICAL);
  
  g_signal_connect_after((GObject *) midi_preferences, "parent-set",
			 G_CALLBACK(ags_midi_preferences_parent_set_callback), (gpointer) midi_preferences);

  midi_preferences->flags = 0;

  /* scrolled window */
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL,
								  NULL);
  gtk_box_pack_start((GtkBox *) midi_preferences,
		     (GtkWidget *) scrolled_window,
		     TRUE, TRUE,
		     0);
  
  midi_preferences->sequencer_editor = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
							      0);
  gtk_container_add((GtkContainer *) scrolled_window,
		    (GtkWidget *) midi_preferences->sequencer_editor);

  /*  */
  midi_preferences->add = NULL;  
}

static void
ags_midi_preferences_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_midi_preferences_parent_class)->finalize(gobject);
}

void
ags_midi_preferences_connect(AgsConnectable *connectable)
{
  AgsMidiPreferences *midi_preferences;

  midi_preferences = AGS_MIDI_PREFERENCES(connectable);

  if((AGS_MIDI_PREFERENCES_CONNECTED & (midi_preferences->flags)) != 0){
    return;
  }

  midi_preferences->flags |= AGS_MIDI_PREFERENCES_CONNECTED;
  
  if(midi_preferences->add != NULL){
    g_signal_connect(G_OBJECT(midi_preferences->add), "clicked",
		     G_CALLBACK(ags_midi_preferences_add_callback), midi_preferences);
  }
}

void
ags_midi_preferences_disconnect(AgsConnectable *connectable)
{
  AgsMidiPreferences *midi_preferences;

  midi_preferences = AGS_MIDI_PREFERENCES(connectable);

  if((AGS_MIDI_PREFERENCES_CONNECTED & (midi_preferences->flags)) == 0){
    return;
  }

  midi_preferences->flags &= (~AGS_MIDI_PREFERENCES_CONNECTED);

  if(midi_preferences->add != NULL){
    g_object_disconnect(G_OBJECT(midi_preferences->add),
			"any_signal::clicked",
			G_CALLBACK(ags_midi_preferences_add_callback),
			midi_preferences,
			NULL);
  }
}

void
ags_midi_preferences_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_midi_preferences_apply(AgsApplicable *applicable)
{
  AgsMidiPreferences *midi_preferences;

  GList *list_start, *list;

  midi_preferences = AGS_MIDI_PREFERENCES(applicable);

  list =
    list_start = gtk_container_get_children((GtkContainer *) midi_preferences->sequencer_editor);

  while(list != NULL){
    ags_applicable_apply(AGS_APPLICABLE(list->data));

    list = list->next;
  }
  
  g_list_free(list_start);
}

void
ags_midi_preferences_reset(AgsApplicable *applicable)
{
  AgsMidiPreferences *midi_preferences;
  AgsSequencerEditor *sequencer_editor;

  AgsThread *main_loop;
  AgsThread *sequencer_thread;
  
  AgsApplicationContext *application_context;

  GList *list_start, *list;

  midi_preferences = AGS_MIDI_PREFERENCES(applicable);
  
  application_context = ags_application_context_get_instance();

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  
  sequencer_thread = ags_thread_find_type(main_loop,
					  AGS_TYPE_SEQUENCER_THREAD);

  /* clear */
  list =
    list_start = gtk_container_get_children((GtkContainer *) midi_preferences->sequencer_editor);

  while(list != NULL){
    gtk_widget_destroy(GTK_WIDGET(list->data));

    list = list->next;
  }
  
  g_list_free(list_start);

  /* reset */
  list =
    list_start = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    sequencer_editor = ags_sequencer_editor_new();    

    sequencer_editor->sequencer = list->data;
    sequencer_editor->sequencer_thread = (GObject *) ags_sequencer_thread_find_sequencer((AgsSequencerThread *) sequencer_thread,
											 list->data);
    gtk_box_pack_start((GtkBox *) midi_preferences->sequencer_editor,
		       (GtkWidget *) sequencer_editor,
		       FALSE, FALSE,
		       0);
    
    ags_applicable_reset(AGS_APPLICABLE(sequencer_editor));
    ags_connectable_connect(AGS_CONNECTABLE(sequencer_editor));
    g_signal_connect(sequencer_editor->remove, "clicked",
		   G_CALLBACK(ags_midi_preferences_remove_sequencer_editor_callback), midi_preferences);
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  gtk_widget_show_all((GtkWidget *) midi_preferences->sequencer_editor);

  /* unref */
  g_object_unref(main_loop);
}

/**
 * ags_midi_preferences_new:
 *
 * Create a new instance of #AgsMidiPreferences
 *
 * Returns: the new #AgsMidiPreferences
 *
 * Since: 3.0.0
 */
AgsMidiPreferences*
ags_midi_preferences_new()
{
  AgsMidiPreferences *midi_preferences;

  midi_preferences = (AgsMidiPreferences *) g_object_new(AGS_TYPE_MIDI_PREFERENCES,
							 NULL);
  
  return(midi_preferences);
}
