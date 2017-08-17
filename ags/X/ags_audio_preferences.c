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

#include <ags/X/ags_audio_preferences.h>
#include <ags/X/ags_audio_preferences_callbacks.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/task/ags_change_soundcard.h>
#include <ags/audio/task/ags_apply_presets.h>

#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>
#include <ags/X/ags_soundcard_editor.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_audio_preferences_class_init(AgsAudioPreferencesClass *audio_preferences);
void ags_audio_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_audio_preferences_init(AgsAudioPreferences *audio_preferences);
void ags_audio_preferences_connect(AgsConnectable *connectable);
void ags_audio_preferences_disconnect(AgsConnectable *connectable);
void ags_audio_preferences_set_update(AgsApplicable *applicable, gboolean update);
void ags_audio_preferences_apply(AgsApplicable *applicable);
void ags_audio_preferences_reset(AgsApplicable *applicable);
static void ags_audio_preferences_finalize(GObject *gobject);

/**
 * SECTION:ags_audio_preferences
 * @short_description: A composite widget to do audio related preferences
 * @title: AgsAudioPreferences
 * @section_id: 
 * @include: ags/X/ags_audio_preferences.h
 *
 * #AgsAudioPreferences enables you to make preferences of soundcard, audio channels,
 * samplerate and buffer size.
 */

static gpointer ags_audio_preferences_parent_class = NULL;

GType
ags_audio_preferences_get_type(void)
{
  static GType ags_type_audio_preferences = 0;

  if(!ags_type_audio_preferences){
    static const GTypeInfo ags_audio_preferences_info = {
      sizeof (AgsAudioPreferencesClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_preferences_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioPreferences),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_preferences_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_preferences_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_audio_preferences_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_audio_preferences = g_type_register_static(GTK_TYPE_VBOX,
							"AgsAudioPreferences", &ags_audio_preferences_info,
							0);
    
    g_type_add_interface_static(ags_type_audio_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_audio_preferences,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_audio_preferences);
}

void
ags_audio_preferences_class_init(AgsAudioPreferencesClass *audio_preferences)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_audio_preferences_parent_class = g_type_class_peek_parent(audio_preferences);

  /* GtkObjectClass */
  gobject = (GObjectClass *) audio_preferences;

  gobject->finalize = ags_audio_preferences_finalize;
}

void
ags_audio_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_audio_preferences_connect;
  connectable->disconnect = ags_audio_preferences_disconnect;
}

void
ags_audio_preferences_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_audio_preferences_set_update;
  applicable->apply = ags_audio_preferences_apply;
  applicable->reset = ags_audio_preferences_reset;
}

void
ags_audio_preferences_init(AgsAudioPreferences *audio_preferences)
{
  GtkScrolledWindow *scrolled_window;
  GtkTable *table;
  GtkHBox *hbox;
  GtkLabel *label;

  gchar *str;  
  
  g_signal_connect_after((GObject *) audio_preferences, "parent-set",
			 G_CALLBACK(ags_audio_preferences_parent_set_callback), (gpointer) audio_preferences);

  audio_preferences->flags = 0;

  /* scrolled window */
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL,
								  NULL);
  gtk_box_pack_start((GtkBox *) audio_preferences,
		     (GtkWidget *) scrolled_window,
		     TRUE, TRUE,
		     0);
  
  audio_preferences->soundcard_editor = (GtkVBox *) gtk_vbox_new(FALSE,
								 0);
  gtk_scrolled_window_add_with_viewport(scrolled_window,
					(GtkWidget *) audio_preferences->soundcard_editor);

  /*  */
  audio_preferences->connect_sink = NULL;
  audio_preferences->add = NULL;
  
  /*  */
  table = (GtkTable *) gtk_table_new(2, 9, FALSE);
  gtk_box_pack_start(GTK_BOX(audio_preferences),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     2);
  
  str = ags_config_get_value(ags_config_get_instance(),
			     AGS_CONFIG_GENERIC,
			     "disable-feature");
  
  /* JACK */  
  if(str == NULL ||
     !g_ascii_strncasecmp(str,
			  "experimental",
			  13)){
    audio_preferences->jack_driver = NULL;
    audio_preferences->start_jack = NULL;
    audio_preferences->stop_jack = NULL;
  }else{

    label = (GtkLabel *) gtk_label_new(i18n("JACK driver"));
    gtk_table_attach(table,
		     GTK_WIDGET(label),
		     0, 1,
		     7, 8,
		     GTK_FILL, GTK_FILL,
		     0, 0);

    audio_preferences->jack_driver = (GtkComboBoxText *) gtk_combo_box_text_new();
    gtk_table_attach(table,
		     GTK_WIDGET(audio_preferences->jack_driver),
		     1, 2,
		     7, 8,
		     GTK_FILL, GTK_FILL,
		     0, 0);
  
    label = (GtkLabel *) gtk_label_new(i18n("JACK server"));
    gtk_table_attach(table,
		     GTK_WIDGET(label),
		     0, 1,
		     7, 8,
		     GTK_FILL, GTK_FILL,
		     0, 0);

    hbox = (GtkHBox *) gtk_hbox_new(TRUE, 0);
    gtk_table_attach(table,
		     GTK_WIDGET(hbox),
		     1, 2,
		     8, 9,
		     GTK_FILL, GTK_FILL,
		     0, 0);

    audio_preferences->start_jack = (GtkButton *) gtk_button_new_with_label(i18n("start"));
    gtk_box_pack_start(GTK_BOX(hbox),
		       GTK_WIDGET(audio_preferences->start_jack),
		       FALSE, FALSE,
		       0);

    audio_preferences->stop_jack = (GtkButton *) gtk_button_new_with_label(i18n("stop"));
    gtk_box_pack_start(GTK_BOX(hbox),
		       GTK_WIDGET(audio_preferences->stop_jack),
		       FALSE, FALSE,
		       0);
  
    /* set default insensitive */
    gtk_widget_set_sensitive((GtkWidget *) audio_preferences->jack_driver,
			     FALSE);
  
    gtk_widget_set_sensitive((GtkWidget *) audio_preferences->start_jack,
			     FALSE);
    gtk_widget_set_sensitive((GtkWidget *) audio_preferences->stop_jack,
			     FALSE);
  }

  g_free(str);
}

void
ags_audio_preferences_connect(AgsConnectable *connectable)
{
  AgsAudioPreferences *audio_preferences;

  audio_preferences = AGS_AUDIO_PREFERENCES(connectable);

  if((AGS_AUDIO_PREFERENCES_CONNECTED & (audio_preferences->flags)) != 0){
    return;
  }

  audio_preferences->flags |= AGS_AUDIO_PREFERENCES_CONNECTED;
  
  if(audio_preferences->add != NULL){
    g_signal_connect(G_OBJECT(audio_preferences->add), "clicked",
		     G_CALLBACK(ags_audio_preferences_add_callback), audio_preferences);
  }
  
  if(audio_preferences->connect_sink != NULL){
    g_signal_connect(G_OBJECT(audio_preferences->connect_sink), "clicked",
		     G_CALLBACK(ags_audio_preferences_connect_sink_callback), audio_preferences);    
  }
    
  /* experimental */
  if(audio_preferences->start_jack != NULL){
    g_signal_connect(G_OBJECT(audio_preferences->start_jack), "clicked",
		     G_CALLBACK(ags_audio_preferences_start_jack_callback), audio_preferences);
  }

  if(audio_preferences->stop_jack != NULL){
    g_signal_connect(G_OBJECT(audio_preferences->stop_jack), "clicked",
		     G_CALLBACK(ags_audio_preferences_stop_jack_callback), audio_preferences);
  }
}

void
ags_audio_preferences_disconnect(AgsConnectable *connectable)
{
  AgsAudioPreferences *audio_preferences;

  gchar *str;
  
  audio_preferences = AGS_AUDIO_PREFERENCES(connectable);

  if((AGS_AUDIO_PREFERENCES_CONNECTED & (audio_preferences->flags)) == 0){
    return;
  }

  audio_preferences->flags &= (~AGS_AUDIO_PREFERENCES_CONNECTED);

  if(audio_preferences->add != NULL){
    g_object_disconnect(G_OBJECT(audio_preferences->add),
			"clicked",
			G_CALLBACK(ags_audio_preferences_add_callback),
			audio_preferences,
			NULL);
  }
  
  if(audio_preferences->connect_sink != NULL){
    g_object_disconnect(G_OBJECT(audio_preferences->connect_sink),
			"clicked",
			G_CALLBACK(ags_audio_preferences_connect_sink_callback),
			audio_preferences,
			NULL);
  }

  /* experimental */
  if(audio_preferences->start_jack != NULL){
    g_object_disconnect(G_OBJECT(audio_preferences->start_jack),
			"clicked",
			G_CALLBACK(ags_audio_preferences_start_jack_callback),
			audio_preferences,
			NULL);
  }
  
  if(audio_preferences->stop_jack != NULL){
    g_object_disconnect(G_OBJECT(audio_preferences->stop_jack),
			"clicked",
			G_CALLBACK(ags_audio_preferences_stop_jack_callback),
			audio_preferences,
			NULL);
  }
}

static void
ags_audio_preferences_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

void
ags_audio_preferences_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_audio_preferences_apply(AgsApplicable *applicable)
{
  AgsAudioPreferences *audio_preferences;

  GList *list_start, *list;

  audio_preferences = AGS_AUDIO_PREFERENCES(applicable);

  list =
    list_start = gtk_container_get_children((GtkContainer *) audio_preferences->soundcard_editor);

  while(list != NULL){
    ags_applicable_apply(AGS_APPLICABLE(list->data));

    list = list->next;
  }
  
  g_list_free(list_start);
}

void
ags_audio_preferences_reset(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsPreferences *preferences;
  AgsAudioPreferences *audio_preferences;
  AgsSoundcardEditor *soundcard_editor;
  
  AgsThread *soundcard_thread;
  
  AgsApplicationContext *application_context;

  GObject *soundcard;
  
  GList *list_start, *list;

  audio_preferences = AGS_AUDIO_PREFERENCES(applicable);
  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							   AGS_TYPE_PREFERENCES);
  window = (AgsWindow *) preferences->window;
  
  application_context = (AgsApplicationContext *) window->application_context;
  soundcard_thread = ags_thread_find_type((AgsThread *) application_context->main_loop,
					  AGS_TYPE_SOUNDCARD_THREAD);

  /* clear */
  list =
    list_start = gtk_container_get_children((GtkContainer *) audio_preferences->soundcard_editor);

  while(list != NULL){
    gtk_widget_destroy(GTK_WIDGET(list->data));

    list = list->next;
  }
  
  g_list_free(list_start);

  /* reset */
  list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    soundcard_editor = ags_soundcard_editor_new();    

    soundcard_editor->soundcard = list->data;
    soundcard_editor->soundcard_thread = (GObject *) ags_soundcard_thread_find_soundcard((AgsSoundcardThread *) soundcard_thread,
											 list->data);
    gtk_box_pack_start((GtkBox *) audio_preferences->soundcard_editor,
		       (GtkWidget *) soundcard_editor,
		       FALSE, FALSE,
		       0);
    
    ags_applicable_reset(AGS_APPLICABLE(soundcard_editor));
    ags_connectable_connect(AGS_CONNECTABLE(soundcard_editor));
    g_signal_connect(soundcard_editor->remove, "clicked",
		   G_CALLBACK(ags_audio_preferences_remove_soundcard_editor_callback), audio_preferences);
    
    list = list->next;
  }

  gtk_widget_show_all((GtkWidget *) audio_preferences->soundcard_editor);
}

/**
 * ags_audio_preferences_new:
 *
 * Creates an #AgsAudioPreferences
 *
 * Returns: a new #AgsAudioPreferences
 *
 * Since: 0.4
 */
AgsAudioPreferences*
ags_audio_preferences_new()
{
  AgsAudioPreferences *audio_preferences;

  audio_preferences = (AgsAudioPreferences *) g_object_new(AGS_TYPE_AUDIO_PREFERENCES,
							   NULL);
  
  return(audio_preferences);
}
