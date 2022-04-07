/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_audio_preferences.h>
#include <ags/app/ags_audio_preferences_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_preferences.h>
#include <ags/app/ags_soundcard_editor.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_audio_preferences_class_init(AgsAudioPreferencesClass *audio_preferences);
void ags_audio_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_audio_preferences_init(AgsAudioPreferences *audio_preferences);
static void ags_audio_preferences_finalize(GObject *gobject);

void ags_audio_preferences_connect(AgsConnectable *connectable);
void ags_audio_preferences_disconnect(AgsConnectable *connectable);
void ags_audio_preferences_set_update(AgsApplicable *applicable, gboolean update);
void ags_audio_preferences_apply(AgsApplicable *applicable);
void ags_audio_preferences_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_audio_preferences
 * @short_description: A composite widget to do audio related preferences
 * @title: AgsAudioPreferences
 * @section_id: 
 * @include: ags/app/ags_audio_preferences.h
 *
 * #AgsAudioPreferences enables you to make preferences of soundcard, audio channels,
 * samplerate and buffer size.
 */

static gpointer ags_audio_preferences_parent_class = NULL;

GType
ags_audio_preferences_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_preferences = 0;

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
    
    ags_type_audio_preferences = g_type_register_static(GTK_TYPE_BOX,
							"AgsAudioPreferences", &ags_audio_preferences_info,
							0);
    
    g_type_add_interface_static(ags_type_audio_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_audio_preferences,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_preferences);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_preferences_class_init(AgsAudioPreferencesClass *audio_preferences)
{
  GObjectClass *gobject;

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
  GtkGrid *grid;
  GtkBox *hbox;
  GtkLabel *label;

  gchar *str;  

  gtk_orientable_set_orientation(GTK_ORIENTABLE(audio_preferences),
				 GTK_ORIENTATION_VERTICAL);
  
  g_signal_connect_after((GObject *) audio_preferences, "parent-set",
			 G_CALLBACK(ags_audio_preferences_parent_set_callback), (gpointer) audio_preferences);

  audio_preferences->flags = 0;

  /* scrolled window */
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_widget_set_hexpand(scrolled_window,
			 TRUE);
  gtk_widget_set_vexpand(scrolled_window,
			 TRUE);

  gtk_widget_set_halign(scrolled_window,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(scrolled_window,
			GTK_ALIGN_FILL);

  gtk_box_append((GtkBox *) audio_preferences,
		 (GtkWidget *) scrolled_window);

  audio_preferences->soundcard_editor = NULL;
  
  audio_preferences->soundcard_editor_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
								   0);

  gtk_box_set_spacing(audio_preferences->soundcard_editor_box,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) audio_preferences->soundcard_editor_box);

  /*  */
  audio_preferences->add = NULL;
  
  /*  */
  grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_valign((GtkWidget *) grid,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) grid,
			GTK_ALIGN_FILL);

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append((GtkWidget *) audio_preferences,
		 (GtkWidget *) grid);
  
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
    
    gtk_widget_set_halign((GtkWidget *) label,
			  GTK_ALIGN_FILL);
    gtk_widget_set_valign((GtkWidget *) label,
			  GTK_ALIGN_FILL);

    gtk_widget_set_margin_end((GtkWidget *) label,
			      AGS_UI_PROVIDER_DEFAULT_MARGIN_END);
    
    gtk_grid_attach(grid,
		    (GtkWidget *) label,
		    0, 7,
		    1, 1);

    audio_preferences->jack_driver = (GtkComboBoxText *) gtk_combo_box_text_new();
    gtk_widget_set_halign((GtkWidget *) audio_preferences->jack_driver,
			  GTK_ALIGN_FILL);
    gtk_widget_set_valign((GtkWidget *) audio_preferences->jack_driver,
			  GTK_ALIGN_FILL);
    gtk_grid_attach(grid,
		    GTK_WIDGET(audio_preferences->jack_driver),
		    1, 7,
		    1, 1);
  
    label = (GtkLabel *) gtk_label_new(i18n("JACK server"));

    gtk_widget_set_halign((GtkWidget *) label,
			  GTK_ALIGN_FILL);
    gtk_widget_set_valign((GtkWidget *) label,
			  GTK_ALIGN_FILL);

    gtk_widget_set_margin_end((GtkWidget *) label,
			      AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

    gtk_grid_attach(grid,
		    (GtkWidget *) label,
		    0, 8,
		    1, 1);

    hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				  0);

    gtk_box_set_spacing(hbox,
			AGS_UI_PROVIDER_DEFAULT_SPACING);

    gtk_box_set_spacing(hbox,
			AGS_UI_PROVIDER_DEFAULT_SPACING);
    
    gtk_widget_set_halign((GtkWidget *) hbox,
			  GTK_ALIGN_FILL);
    gtk_widget_set_valign((GtkWidget *) hbox,
			  GTK_ALIGN_FILL);
    
    gtk_grid_attach(grid,
		    (GtkWidget *) hbox,
		    1, 8,
		    1, 1);

    audio_preferences->start_jack = (GtkButton *) gtk_button_new_with_label(i18n("start"));
    gtk_box_append(hbox,
		   (GtkWidget *) audio_preferences->start_jack);

    audio_preferences->stop_jack = (GtkButton *) gtk_button_new_with_label(i18n("stop"));
    gtk_box_append(hbox,
		   (GtkWidget *) audio_preferences->stop_jack);
  
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

static void
ags_audio_preferences_finalize(GObject *gobject)
{
  //TODO:JK: implement me
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_preferences_parent_class)->finalize(gobject);
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
  
  audio_preferences = AGS_AUDIO_PREFERENCES(connectable);

  if((AGS_AUDIO_PREFERENCES_CONNECTED & (audio_preferences->flags)) == 0){
    return;
  }

  audio_preferences->flags &= (~AGS_AUDIO_PREFERENCES_CONNECTED);

  if(audio_preferences->add != NULL){
    g_object_disconnect(G_OBJECT(audio_preferences->add),
			"any_signal::clicked",
			G_CALLBACK(ags_audio_preferences_add_callback),
			audio_preferences,
			NULL);
  }

  /* experimental */
  if(audio_preferences->start_jack != NULL){
    g_object_disconnect(G_OBJECT(audio_preferences->start_jack),
			"any_signal::clicked",
			G_CALLBACK(ags_audio_preferences_start_jack_callback),
			audio_preferences,
			NULL);
  }
  
  if(audio_preferences->stop_jack != NULL){
    g_object_disconnect(G_OBJECT(audio_preferences->stop_jack),
			"any_signal::clicked",
			G_CALLBACK(ags_audio_preferences_stop_jack_callback),
			audio_preferences,
			NULL);
  }
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

  GList *list;

  audio_preferences = AGS_AUDIO_PREFERENCES(applicable);

  list = audio_preferences->soundcard_editor;

  while(list != NULL){
    ags_applicable_apply(AGS_APPLICABLE(list->data));

    list = list->next;
  }
}

void
ags_audio_preferences_reset(AgsApplicable *applicable)
{
  AgsAudioPreferences *audio_preferences;
  AgsSoundcardEditor *soundcard_editor;

  AgsThread *main_loop;
  AgsThread *soundcard_thread;
  
  AgsApplicationContext *application_context;
  
  GList *start_list, *list;

  audio_preferences = AGS_AUDIO_PREFERENCES(applicable);
  
  application_context = ags_application_context_get_instance();
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  
  soundcard_thread = ags_thread_find_type((AgsThread *) main_loop,
					  AGS_TYPE_SOUNDCARD_THREAD);

  /* clear */
  list = audio_preferences->soundcard_editor;

  while(list != NULL){
    gtk_box_remove(audio_preferences->soundcard_editor,
		   GTK_WIDGET(list->data));

    list = list->next;
  }

  g_list_free(audio_preferences->soundcard_editor);

  audio_preferences->soundcard_editor = NULL;

  /* reset */
  list =
    start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    soundcard_editor = ags_soundcard_editor_new();    

    soundcard_editor->soundcard = list->data;
    soundcard_editor->soundcard_thread = (GObject *) ags_soundcard_thread_find_soundcard((AgsSoundcardThread *) soundcard_thread,
											 list->data);
  
    if(audio_preferences->soundcard_editor != NULL){
      gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->buffer_size,
			       FALSE);
    }

    audio_preferences->soundcard_editor = g_list_prepend(audio_preferences->soundcard_editor,
							 soundcard_editor);

    gtk_box_append((GtkBox *) audio_preferences->soundcard_editor_box,
		   (GtkWidget *) soundcard_editor);
    
    ags_applicable_reset(AGS_APPLICABLE(soundcard_editor));
    ags_connectable_connect(AGS_CONNECTABLE(soundcard_editor));
    g_signal_connect(soundcard_editor->remove, "clicked",
		   G_CALLBACK(ags_audio_preferences_remove_soundcard_editor_callback), audio_preferences);
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
  
  gtk_widget_show((GtkWidget *) audio_preferences->soundcard_editor_box);

  /* unref */
  g_object_unref(main_loop);
}

/**
 * ags_audio_preferences_get_soundcard_editor:
 * @audio_preferences: the #AgsAudio_Preferences
 * 
 * Get line member of @audio_preferences.
 * 
 * Returns: the #GList-struct containing #AgsSoundcardEditor
 *
 * Since: 4.0.0
 */
GList*
ags_audio_preferences_get_soundcard_editor(AgsAudioPreferences *audio_preferences)
{
  g_return_val_if_fail(AGS_IS_AUDIO_PREFERENCES(audio_preferences), NULL);

  return(g_list_reverse(g_list_copy(audio_preferences->soundcard_editor)));
}

/**
 * ags_audio_preferences_add_soundcard_editor:
 * @audio_preferences: the #AgsAudio_Preferences
 * @soundcard_editor: the #AgsSoundcardEditor
 * @x: the x position
 * @y: the y position
 * @width: the width
 * @height: the height
 * 
 * Add @soundcard_editor to @audio_preferences.
 * 
 * Since: 4.0.0
 */
void
ags_audio_preferences_add_soundcard_editor(AgsAudioPreferences *audio_preferences,
					   AgsSoundcardEditor *soundcard_editor)
{
  g_return_if_fail(AGS_IS_AUDIO_PREFERENCES(audio_preferences));
  g_return_if_fail(AGS_IS_SOUNDCARD_EDITOR(soundcard_editor));

  if(g_list_find(audio_preferences->soundcard_editor, soundcard_editor) == NULL){
    audio_preferences->soundcard_editor = g_list_prepend(audio_preferences->soundcard_editor,
							 soundcard_editor);
    
    gtk_box_append(audio_preferences->soundcard_editor_box,
		   soundcard_editor);
  }
}

/**
 * ags_audio_preferences_remove_soundcard_editor:
 * @audio_preferences: the #AgsAudio_Preferences
 * @soundcard_editor: the #AgsSoundcardEditor
 * 
 * Remove @soundcard_editor from @audio_preferences.
 * 
 * Since: 4.0.0
 */
void
ags_audio_preferences_remove_soundcard_editor(AgsAudioPreferences *audio_preferences,
					      AgsSoundcardEditor *soundcard_editor)
{
  g_return_if_fail(AGS_IS_AUDIO_PREFERENCES(audio_preferences));
  g_return_if_fail(AGS_IS_SOUNDCARD_EDITOR(soundcard_editor));

  if(g_list_find(audio_preferences->soundcard_editor, soundcard_editor) != NULL){
    audio_preferences->soundcard_editor = g_list_remove(audio_preferences->soundcard_editor,
							soundcard_editor);
    
    gtk_box_remove(audio_preferences->soundcard_editor_box,
		   soundcard_editor);
  }
}

/**
 * ags_audio_preferences_new:
 *
 * Create a new instance of #AgsAudioPreferences
 *
 * Returns: the new #AgsAudioPreferences
 *
 * Since: 3.0.0
 */
AgsAudioPreferences*
ags_audio_preferences_new()
{
  AgsAudioPreferences *audio_preferences;

  audio_preferences = (AgsAudioPreferences *) g_object_new(AGS_TYPE_AUDIO_PREFERENCES,
							   NULL);
  
  return(audio_preferences);
}
