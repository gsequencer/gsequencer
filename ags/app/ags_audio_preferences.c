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

gboolean ags_audio_preferences_is_connected(AgsConnectable *connectable);
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
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
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

    g_once_init_leave(&g_define_type_id__static, ags_type_audio_preferences);
  }

  return(g_define_type_id__static);
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
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;

  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_audio_preferences_is_connected;  
  connectable->connect = ags_audio_preferences_connect;
  connectable->disconnect = ags_audio_preferences_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
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
  AgsPreferences *preferences;

  GtkScrolledWindow *scrolled_window;
  GtkGrid *grid;
  GtkBox *hbox;
  GtkLabel *label;

  GMenuModel *add_popup;
  GMenu *menu;
  GMenuItem *item;
  
  AgsConfig *config;
  AgsApplicationContext *application_context;
  
  GSimpleActionGroup *action_group;
  GSimpleAction *action;
  
  gchar *str;
  
  application_context = ags_application_context_get_instance();

  config = ags_config_get_instance();
  
  preferences = (AgsPreferences *) ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context));
  
  gtk_orientable_set_orientation(GTK_ORIENTABLE(audio_preferences),
				 GTK_ORIENTATION_VERTICAL);

  gtk_box_set_spacing((GtkBox *) audio_preferences,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);  
  
  g_signal_connect((GObject *) audio_preferences, "notify::parent",
		   G_CALLBACK(ags_audio_preferences_notify_parent_callback), NULL);

  audio_preferences->flags = 0;
  audio_preferences->connectable_flags = 0;

  /* action group */
  action_group = g_simple_action_group_new();
  gtk_widget_insert_action_group((GtkWidget *) preferences,
				 "audio_preferences",
				 G_ACTION_GROUP(action_group));

#if defined(AGS_WITH_ALSA)
  /* add ALSA output soundcard */
  action = g_simple_action_new("add_alsa_output_soundcard",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_audio_preferences_add_alsa_output_soundcard_callback), audio_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
  
  /* add ALSA input soundcard */
  action = g_simple_action_new("add_alsa_input_soundcard",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_audio_preferences_add_alsa_input_soundcard_callback), audio_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
#endif

#if defined(AGS_WITH_OSS)
  /* add OSS output soundcard */
  action = g_simple_action_new("add_oss_output_soundcard",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_audio_preferences_add_oss_output_soundcard_callback), audio_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
  
  /* add OSS input soundcard */
  action = g_simple_action_new("add_oss_input_soundcard",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_audio_preferences_add_oss_input_soundcard_callback), audio_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
#endif

#if defined(AGS_WITH_JACK)
  /* add JACK output soundcard */
  action = g_simple_action_new("add_jack_output_soundcard",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_audio_preferences_add_jack_output_soundcard_callback), audio_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
  
  /* add JACK input soundcard */
  action = g_simple_action_new("add_jack_input_soundcard",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_audio_preferences_add_jack_input_soundcard_callback), audio_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
#endif

#if defined(AGS_WITH_PULSE)
  /* add PULSE output soundcard */
  action = g_simple_action_new("add_pulse_output_soundcard",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_audio_preferences_add_pulse_output_soundcard_callback), audio_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
  
  /* add PULSE input soundcard */
  action = g_simple_action_new("add_pulse_input_soundcard",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_audio_preferences_add_pulse_input_soundcard_callback), audio_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
#endif

#if defined(AGS_WITH_CORE_AUDIO)
  /* add Core Audio output soundcard */
  action = g_simple_action_new("add_core_audio_output_soundcard",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_audio_preferences_add_core_audio_output_soundcard_callback), audio_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
  
  /* add Core Audio input soundcard */
  action = g_simple_action_new("add_core_audio_input_soundcard",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_audio_preferences_add_core_audio_input_soundcard_callback), audio_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
#endif

#if defined(AGS_WITH_WASAPI)
  /* add WASAPI output soundcard */
  action = g_simple_action_new("add_wasapi_output_soundcard",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_audio_preferences_add_wasapi_output_soundcard_callback), audio_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
  
  /* add WASAPI input soundcard */
  action = g_simple_action_new("add_wasapi_input_soundcard",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_audio_preferences_add_wasapi_input_soundcard_callback), audio_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
#endif
  
  /* scrolled window */
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_widget_set_hexpand((GtkWidget *) scrolled_window,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) scrolled_window,
			 TRUE);

  gtk_widget_set_halign((GtkWidget *) scrolled_window,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) scrolled_window,
			GTK_ALIGN_FILL);

  gtk_box_append((GtkBox *) audio_preferences,
		 (GtkWidget *) scrolled_window);

  audio_preferences->soundcard_editor = NULL;
  
  audio_preferences->soundcard_editor_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
								   0);

  gtk_box_set_spacing((GtkBox *) audio_preferences->soundcard_editor_box,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) audio_preferences->soundcard_editor_box);

  /*  */
  audio_preferences->add_menu_button = (GtkMenuButton *) gtk_menu_button_new();
  gtk_box_prepend(preferences->action_area,
		  (GtkWidget *) audio_preferences->add_menu_button);
  
  menu = (GMenu *) g_menu_new();
  add_popup = G_MENU_MODEL(menu);

  /*  */
#if defined(AGS_WITH_ALSA)
  item = g_menu_item_new(i18n("add ALSA output soundcard"),
			 "audio_preferences.add_alsa_output_soundcard");
  g_menu_append_item(menu,
		     item);
  
  item = g_menu_item_new(i18n("add ALSA input soundcard"),
			 "audio_preferences.add_alsa_input_soundcard");
  g_menu_append_item(menu,
		     item);
#endif

#if defined(AGS_WITH_OSS)
  item = g_menu_item_new(i18n("add OSSv4 output soundcard"),
			 "audio_preferences.add_oss_output_soundcard");
  g_menu_append_item(menu,
		     item);
  
  item = g_menu_item_new(i18n("add OSSv4 input soundcard"),
			 "audio_preferences.add_oss_input_soundcard");
  g_menu_append_item(menu,
		     item);
#endif

#if defined(AGS_WITH_JACK)
  item = g_menu_item_new(i18n("add JACK output soundcard"),
			 "audio_preferences.add_jack_output_soundcard");
  g_menu_append_item(menu,
		     item);
  
  item = g_menu_item_new(i18n("add JACK input soundcard"),
			 "audio_preferences.add_jack_input_soundcard");
  g_menu_append_item(menu,
		     item);
#endif

#if defined(AGS_WITH_PULSE)
  item = g_menu_item_new(i18n("add Pulseaudio output soundcard"),
			 "audio_preferences.add_pulse_output_soundcard");
  g_menu_append_item(menu,
		     item);
  
  item = g_menu_item_new(i18n("add Pulseaudio input soundcard"),
			 "audio_preferences.add_pulse_input_soundcard");
  g_menu_append_item(menu,
		     item);
#endif

#if defined(AGS_WITH_CORE_AUDIO)
  item = g_menu_item_new(i18n("add Core Audio output soundcard"),
			 "audio_preferences.add_core_audio_output_soundcard");
  g_menu_append_item(menu,
		     item);
  
  item = g_menu_item_new(i18n("add Core Audio input soundcard"),
			 "audio_preferences.add_core_audio_input_soundcard");
  g_menu_append_item(menu,
		     item);
#endif

#if defined(AGS_WITH_WASAPI)
  item = g_menu_item_new(i18n("add WASAPI output soundcard"),
			 "audio_preferences.add_wasapi_output_soundcard");
  g_menu_append_item(menu,
		     item);
  
  item = g_menu_item_new(i18n("add WASAPI input soundcard"),
			 "audio_preferences.add_wasapi_input_soundcard");
  g_menu_append_item(menu,
		     item);
#endif
  
  gtk_menu_button_set_menu_model(audio_preferences->add_menu_button,
				 add_popup);
  
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

  gtk_box_append((GtkBox *) audio_preferences,
		 (GtkWidget *) grid);
  
  str = ags_config_get_value(config,
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

gboolean
ags_audio_preferences_is_connected(AgsConnectable *connectable)
{
  AgsAudioPreferences *audio_preferences;
  
  gboolean is_connected;
  
  audio_preferences = AGS_AUDIO_PREFERENCES(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (audio_preferences->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_audio_preferences_connect(AgsConnectable *connectable)
{
  AgsAudioPreferences *audio_preferences;

  audio_preferences = AGS_AUDIO_PREFERENCES(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  audio_preferences->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
        
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

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  audio_preferences->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

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
  AgsSoundcardEditor *soundcard_editor;

  AgsThread *main_loop;
  AgsThread *first_soundcard_thread, *soundcard_thread;
  
  AgsApplicationContext *application_context;
  
  GList *start_soundcard, *soundcard;
  GList *start_list, *list;
  GList *tmp_soundcard;

  gboolean found_editor;

  audio_preferences = AGS_AUDIO_PREFERENCES(applicable);

  application_context = ags_application_context_get_instance();

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  list =
    start_list = ags_audio_preferences_get_soundcard_editor(audio_preferences);

  while(list != NULL){
    ags_applicable_apply(AGS_APPLICABLE(list->data));

    list = list->next;
  }

  /* reset */
  soundcard =
    start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  while(soundcard != NULL){
    list = start_list;

    found_editor = FALSE;

    while(list != NULL){
      if(AGS_SOUNDCARD_EDITOR(list->data)->soundcard == (GObject *) soundcard->data){
	found_editor = TRUE;

	break;
      }

      list = list->next;
    }
    
    if(!found_editor){
      tmp_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

      if(g_list_find(tmp_soundcard, soundcard->data) != NULL){
	first_soundcard_thread = ags_thread_find_type(main_loop,
						      AGS_TYPE_SOUNDCARD_THREAD);

	soundcard_thread = (AgsThread *) ags_soundcard_thread_find_soundcard((AgsSoundcardThread *) first_soundcard_thread,
									     soundcard->data);

	/* remove */
	tmp_soundcard = g_list_remove(tmp_soundcard,
				      soundcard->data);
	
	ags_thread_remove_child(main_loop,
				soundcard_thread);

	/* unref */
	g_object_unref(soundcard->data);

	g_object_unref(first_soundcard_thread);

	g_object_unref(soundcard_thread);
      }
      
      ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				       tmp_soundcard);
    }
        
    soundcard = soundcard->next;
  }

  /* unref */
  g_list_free_full(start_soundcard,
		   (GDestroyNotify) g_object_unref);
  

  g_list_free(start_list);
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
  GList *start_soundcard, *soundcard;

  guint channels;
  guint samplerate;
  guint buffer_size;
  AgsSoundcardFormat format;
  
  audio_preferences = AGS_AUDIO_PREFERENCES(applicable);
  
  application_context = ags_application_context_get_instance();
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  
  soundcard_thread = ags_thread_find_type((AgsThread *) main_loop,
					  AGS_TYPE_SOUNDCARD_THREAD);

  /* clear */
  list =
    start_list = ags_audio_preferences_get_soundcard_editor(audio_preferences);

  while(list != NULL){
    ags_audio_preferences_remove_soundcard_editor(audio_preferences,
						  AGS_SOUNDCARD_EDITOR(list->data));

    list = list->next;
  }

  g_list_free(start_list);

  /* reset */
  soundcard =
    start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
  
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  if(soundcard != NULL){
    ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard->data),
			      &channels,
			      &samplerate,
			      &buffer_size,
			      &format);
  }
  
  while(soundcard != NULL){
    soundcard_editor = ags_soundcard_editor_new();    

    soundcard_editor->soundcard = soundcard->data;
    soundcard_editor->soundcard_thread = (GObject *) ags_soundcard_thread_find_soundcard((AgsSoundcardThread *) soundcard_thread,
											 soundcard->data);
  
    if(audio_preferences->soundcard_editor != NULL){
      gtk_spin_button_set_value(soundcard_editor->audio_channels,
				(gdouble) channels);
      
      gtk_spin_button_set_value(soundcard_editor->samplerate,
				(gdouble) samplerate);
      
      gtk_spin_button_set_value(soundcard_editor->buffer_size,
				(gdouble) buffer_size);

      switch(format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
				 0);
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
				 1);
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
				 2);
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
				 3);
	break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
	gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
				 4);
	break;
      }
    }

    ags_audio_preferences_add_soundcard_editor(audio_preferences,
					       soundcard_editor);
    
    ags_applicable_reset(AGS_APPLICABLE(soundcard_editor));
    ags_connectable_connect(AGS_CONNECTABLE(soundcard_editor));

    g_signal_connect(soundcard_editor->remove, "clicked",
		   G_CALLBACK(ags_audio_preferences_remove_soundcard_editor_callback), audio_preferences);
    
    soundcard = soundcard->next;
  }

  g_list_free_full(start_soundcard,
		   (GDestroyNotify) g_object_unref);
  
  gtk_widget_show((GtkWidget *) audio_preferences->soundcard_editor_box);

  /* unref */
  g_object_unref(main_loop);
}

/**
 * ags_audio_preferences_add_soundcard:
 * @audio_preferences: the #AgsAudio_Preferences
 * @backend: the backend to use
 * @is_output: %TRUE if is output, otherwise %FALSE
 * 
 * Add soundcard.
 *
 * Since: 8.3.2
 */
void
ags_audio_preferences_add_soundcard(AgsAudioPreferences *audio_preferences,
				    gchar *backend,
				    gboolean is_output)
{
  g_return_if_fail(backend != NULL);
  
  if(is_output){
    AgsSoundcardEditor *soundcard_editor;

    AgsPulseServer *pulse_server;
    AgsPulseDevout *pulse_devout;

    AgsJackServer *jack_server;
    AgsJackDevout *jack_devout;
  
    AgsCoreAudioServer *core_audio_server;
    AgsCoreAudioDevout *core_audio_devout;
  
    AgsAlsaDevout *alsa_devout;

    AgsOssDevout *oss_devout;
  
    AgsWasapiDevout *wasapi_devout;
  
    AgsThread *main_loop;
    AgsThread *soundcard_thread, *default_soundcard_thread;
    AgsThread *export_thread;
  
    AgsApplicationContext *application_context;

    GObject *soundcard;

    GType server_type;
 
    GList *start_sound_server, *sound_server;
    GList *start_list, *list;
    GList *start_card_id, *card_id;
    GList *start_card_name, *card_name;
  
    gchar *tmp;
  
    guint pcm_channels;
    guint buffer_size;
    AgsSoundcardFormat format;
    guint samplerate;
    gboolean use_alsa, use_oss, use_core_audio, use_pulse, use_jack, use_wasapi;
    gboolean is_output;
    gboolean initial_soundcard;
  
    application_context = ags_application_context_get_instance();

    main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

    core_audio_server = NULL;
    core_audio_devout = NULL;
  
    pulse_server = NULL;
    pulse_devout = NULL;
  
    jack_server = NULL;
    jack_devout = NULL;

    alsa_devout = NULL;

    oss_devout = NULL;
  
    /* soundcard */
    soundcard = NULL;

    server_type = G_TYPE_NONE;
  
    use_alsa = FALSE;
    use_oss = FALSE;
    use_core_audio = FALSE;
    use_pulse = FALSE;
    use_jack = FALSE;
    use_wasapi = FALSE;
  
    is_output = TRUE;
  
    initial_soundcard = FALSE;
  
    /* soundcard editor */
    soundcard_editor = ags_soundcard_editor_new();

    gtk_combo_box_set_active((GtkComboBox *) soundcard_editor->capability,
			     0);
  
    /* determine backend */
    if(backend != NULL){
      if(!g_ascii_strncasecmp(backend,
			      "alsa",
			      5)){
	use_alsa = TRUE;
      }else if(!g_ascii_strncasecmp(backend,
				    "oss",
				    4)){
	use_oss = TRUE;
      }else if(!g_ascii_strncasecmp(backend,
				    "core-audio",
				    11)){
	server_type = AGS_TYPE_CORE_AUDIO_SERVER;
      
	use_core_audio = TRUE;
      }else if(!g_ascii_strncasecmp(backend,
				    "pulse",
				    6)){
	server_type = AGS_TYPE_PULSE_SERVER;
      
	use_pulse = TRUE;
      }else if(!g_ascii_strncasecmp(backend,
				    "jack",
				    5)){
	server_type = AGS_TYPE_JACK_SERVER;

	use_jack = TRUE;
      }else if(!g_ascii_strncasecmp(backend,
				    "wasapi",
				    7)){
	use_wasapi = TRUE;
      }
    }
  
    /* presets */
    pcm_channels = gtk_spin_button_get_value_as_int(soundcard_editor->audio_channels);
    samplerate = gtk_spin_button_get_value_as_int(soundcard_editor->samplerate);
    buffer_size = gtk_spin_button_get_value_as_int(soundcard_editor->buffer_size);
    format = AGS_SOUNDCARD_DEFAULT_FORMAT;

    switch(gtk_combo_box_get_active(GTK_COMBO_BOX(soundcard_editor->format))){
    case 0:
      {
	format = AGS_SOUNDCARD_SIGNED_8_BIT;
      }
      break;
    case 1:
      {
	format = AGS_SOUNDCARD_SIGNED_16_BIT;
      }
      break;
    case 2:
      {
	format = AGS_SOUNDCARD_SIGNED_24_BIT;
      }
      break;
    case 3:
      {
	format = AGS_SOUNDCARD_SIGNED_32_BIT;
      }
      break;
    case 4:
      {
	format = AGS_SOUNDCARD_SIGNED_64_BIT;
      }
      break;
    case 5:
      {
	format = AGS_SOUNDCARD_FLOAT;
      }
      break;
    case 6:
      {
	format = AGS_SOUNDCARD_DOUBLE;
      }
      break;
    }

    /* create soundcard */
    sound_server =
      start_sound_server = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));

    if((sound_server = ags_list_util_find_type(start_sound_server,
					       server_type)) != NULL){
      if(use_core_audio){
	GValue *param_value = g_new0(GValue,
				     4);
	
	gchar **param_strv = (gchar **) g_malloc(5 * sizeof(gchar *));

	param_strv[0] = g_strdup("pcm-channels");
	param_strv[1] = g_strdup("buffer-size");
	param_strv[2] = g_strdup("format");
	param_strv[3] = g_strdup("samplerate");
	param_strv[4] = NULL;

	g_value_init(param_value, G_TYPE_UINT);
	g_value_set_uint(param_value,
			 pcm_channels);
	
	g_value_init(param_value + 1, G_TYPE_UINT);
	g_value_set_uint(param_value + 1,
			 buffer_size);

	g_value_init(param_value + 2, G_TYPE_UINT);
	g_value_set_uint(param_value + 2,
			 format);

	g_value_init(param_value + 3, G_TYPE_UINT);
	g_value_set_uint(param_value + 3,
			 samplerate);
      
	core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server->data);

	if(is_output){
	  core_audio_devout = (AgsCoreAudioDevout *) ags_sound_server_register_soundcard_with_params(AGS_SOUND_SERVER(core_audio_server),
												     is_output,
												     (gchar **) param_strv, param_value);
	  soundcard = (GObject *) core_audio_devout;
	}

	g_strfreev(param_strv);
	g_free(param_value);
      }else if(use_pulse){
	pulse_server = AGS_PULSE_SERVER(sound_server->data);

	pulse_devout = (AgsPulseDevout *) ags_sound_server_register_soundcard(AGS_SOUND_SERVER(pulse_server),
									      is_output);
	soundcard = (GObject *) pulse_devout;
      }else if(use_jack){
	jack_server = AGS_JACK_SERVER(sound_server->data);

	jack_devout = (AgsJackDevout *) ags_sound_server_register_soundcard(AGS_SOUND_SERVER(jack_server),
									    is_output);
	soundcard = (GObject *) jack_devout;
      }else{
	//TODO:JK: implement me
      }
    }else{
      if(use_alsa){
	alsa_devout = ags_alsa_devout_new();

	soundcard = (GObject *) alsa_devout;      
      }else if(use_oss){
	oss_devout = ags_oss_devout_new();

	soundcard = (GObject *) oss_devout;
      }else if(use_wasapi){
	wasapi_devout = ags_wasapi_devout_new();

	soundcard = (GObject *) wasapi_devout;
      }else{
	//TODO:JK: implement me
      }
    }
  
    if(soundcard != NULL){
      soundcard_editor->soundcard = soundcard;
      soundcard_editor->soundcard_thread = (GObject *) ags_thread_find_type(main_loop,
									    AGS_TYPE_SOUNDCARD_THREAD);

      if((start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context))) == NULL){
	initial_soundcard = TRUE;
      }

      g_object_ref(soundcard);
      start_list = g_list_append(start_list,
				 soundcard);
    
      ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				       start_list);

      g_list_foreach(start_list,
		     (GFunc) g_object_unref,
		     NULL);

      soundcard_thread = (AgsThread *) ags_soundcard_thread_new(soundcard,
								ags_soundcard_get_capability(AGS_SOUNDCARD(soundcard)));
      soundcard_editor->soundcard_thread = (GObject *) soundcard_thread;
  
      ags_thread_add_child_extended(main_loop,
				    soundcard_thread,
				    TRUE, TRUE);

      if((default_soundcard_thread = (AgsThread *) ags_sound_provider_get_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context))) == NULL){
	ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context),
							(GObject *) soundcard_thread);
      }else{
	g_object_unref(default_soundcard_thread);
      }

      /* export thread */
      export_thread = (AgsThread *) ags_export_thread_new(soundcard,
							  NULL);
      ags_thread_add_child_extended(main_loop,
				    (AgsThread *) export_thread,
				    TRUE, TRUE);
    }
  
    ags_audio_preferences_add_soundcard_editor(audio_preferences,
					       soundcard_editor);

    /*  */
    start_card_id = NULL;
    start_card_name = NULL;
  
    ags_soundcard_list_cards(AGS_SOUNDCARD(soundcard),
			     &start_card_id, &start_card_name);

    gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card))));

    card_id = start_card_id;
    card_name = start_card_name;
  
    while(card_id != NULL){
#ifdef AGS_WITH_CORE_AUDIO
      tmp = card_name->data;
#else
      tmp = card_id->data;
#endif
    
      if(tmp != NULL){
	gtk_combo_box_text_append_text(soundcard_editor->card,
				       tmp);
      }
    
      card_id = card_id->next;
      card_name = card_name->next;
    }

    /* unref */
    g_object_unref(main_loop);
  
    g_list_free_full(start_card_id,
		     (GDestroyNotify) g_free);
  
    g_list_free_full(start_card_name,
		     (GDestroyNotify) g_free);
  
    ags_applicable_reset(AGS_APPLICABLE(soundcard_editor));
    ags_connectable_connect(AGS_CONNECTABLE(soundcard_editor));

    g_signal_connect(soundcard_editor->remove, "clicked",
		     G_CALLBACK(ags_audio_preferences_remove_soundcard_editor_callback), audio_preferences);

    gtk_widget_show((GtkWidget *) soundcard_editor);

    /* reset default card */  
    g_object_unref(main_loop);  
  }

  if(!is_output){
    AgsSoundcardEditor *soundcard_editor;

    AgsPulseServer *pulse_server;
    AgsPulseDevin *pulse_devin;

    AgsJackServer *jack_server;
    AgsJackDevin *jack_devin;
  
    AgsCoreAudioServer *core_audio_server;
    AgsCoreAudioDevin *core_audio_devin;

    AgsAlsaDevin *alsa_devin;

    AgsOssDevin *oss_devin;
  
    AgsWasapiDevin *wasapi_devin;
  
    AgsThread *main_loop;
    AgsThread *soundcard_thread, *default_soundcard_thread;
    AgsThread *export_thread;
  
    AgsApplicationContext *application_context;

    GObject *soundcard;

    GType server_type;
  
    GList *start_sound_server, *sound_server;
    GList *start_list, *list;
    GList *start_card_id, *card_id;
    GList *start_card_name, *card_name;
  
    gchar *tmp;
  
    guint pcm_channels;
    guint buffer_size;
    AgsSoundcardFormat format;
    guint samplerate;
    gboolean use_alsa, use_oss, use_core_audio, use_pulse, use_jack, use_wasapi;
    gboolean is_output;
    gboolean initial_soundcard;
  
    application_context = ags_application_context_get_instance();

    main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

    core_audio_server = NULL;
    core_audio_devin = NULL;
  
    pulse_server = NULL;
    pulse_devin = NULL;
  
    jack_server = NULL;
    jack_devin = NULL;

    alsa_devin = NULL;

    oss_devin = NULL;

    /* soundcard */
    soundcard = NULL;

    server_type = G_TYPE_NONE;
  
    use_alsa = FALSE;
    use_oss = FALSE;
    use_core_audio = FALSE;
    use_pulse = FALSE;
    use_jack = FALSE;
    use_wasapi = FALSE;

    is_output = FALSE;
  
    initial_soundcard = FALSE;
  
    /* soundcard editor */
    soundcard_editor = ags_soundcard_editor_new();

    gtk_combo_box_set_active((GtkComboBox *) soundcard_editor->capability,
			     1);

    /* determine backend */
    if(backend != NULL){
      if(!g_ascii_strncasecmp(backend,
			      "alsa",
			      5)){
	use_alsa = TRUE;
      }else if(!g_ascii_strncasecmp(backend,
				    "oss",
				    4)){
	use_oss = TRUE;
      }else if(!g_ascii_strncasecmp(backend,
				    "core-audio",
				    11)){
	server_type = AGS_TYPE_CORE_AUDIO_SERVER;
      
	use_core_audio = TRUE;
      }else if(!g_ascii_strncasecmp(backend,
				    "pulse",
				    6)){
	server_type = AGS_TYPE_PULSE_SERVER;
      
	use_pulse = TRUE;
      }else if(!g_ascii_strncasecmp(backend,
				    "jack",
				    5)){
	server_type = AGS_TYPE_JACK_SERVER;

	use_jack = TRUE;
      }else if(!g_ascii_strncasecmp(backend,
				    "wasapi",
				    7)){
	use_wasapi = TRUE;
      }
    }
  
    /* presets */
    pcm_channels = gtk_spin_button_get_value_as_int(soundcard_editor->audio_channels);
    samplerate = gtk_spin_button_get_value_as_int(soundcard_editor->samplerate);
    buffer_size = gtk_spin_button_get_value_as_int(soundcard_editor->buffer_size);
    format = AGS_SOUNDCARD_DEFAULT_FORMAT;

    switch(gtk_combo_box_get_active(GTK_COMBO_BOX(soundcard_editor->format))){
    case 0:
      {
	format = AGS_SOUNDCARD_SIGNED_8_BIT;
      }
      break;
    case 1:
      {
	format = AGS_SOUNDCARD_SIGNED_16_BIT;
      }
      break;
    case 2:
      {
	format = AGS_SOUNDCARD_SIGNED_24_BIT;
      }
      break;
    case 3:
      {
	format = AGS_SOUNDCARD_SIGNED_32_BIT;
      }
      break;
    case 4:
      {
	format = AGS_SOUNDCARD_SIGNED_64_BIT;
      }
      break;
    case 5:
      {
	format = AGS_SOUNDCARD_FLOAT;
      }
      break;
    case 6:
      {
	format = AGS_SOUNDCARD_DOUBLE;
      }
      break;
    }
  
    /* create soundcard */
    sound_server =
      start_sound_server = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));

    if((sound_server = ags_list_util_find_type(start_sound_server,
					       server_type)) != NULL){
      if(use_core_audio){
	GValue *param_value = g_new0(GValue,
				     4);
	
	gchar **param_strv = (gchar **) g_malloc(5 * sizeof(gchar *));

	param_strv[0] = g_strdup("pcm-channels");
	param_strv[1] = g_strdup("buffer-size");
	param_strv[2] = g_strdup("format");
	param_strv[3] = g_strdup("samplerate");
	param_strv[4] = NULL;

	g_value_init(param_value, G_TYPE_UINT);
	g_value_set_uint(param_value,
			 pcm_channels);
	
	g_value_init(param_value + 1, G_TYPE_UINT);
	g_value_set_uint(param_value + 1,
			 buffer_size);

	g_value_init(param_value + 2, G_TYPE_UINT);
	g_value_set_uint(param_value + 2,
			 format);

	g_value_init(param_value + 3, G_TYPE_UINT);
	g_value_set_uint(param_value + 3,
			 samplerate);
      
	core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server->data);

	if(!is_output){
	  core_audio_devin = (AgsCoreAudioDevin *) ags_sound_server_register_soundcard_with_params(AGS_SOUND_SERVER(core_audio_server),
												   is_output,
												   (gchar **) param_strv, param_value);
	  soundcard = (GObject *) core_audio_devin;
	}

	g_strfreev(param_strv);
	g_free(param_value);
      }else if(use_pulse){
	pulse_server = AGS_PULSE_SERVER(sound_server->data);

	pulse_devin = (AgsPulseDevin *) ags_sound_server_register_soundcard(AGS_SOUND_SERVER(pulse_server),
									    is_output);
	soundcard = (GObject *) pulse_devin;
      }else if(use_jack){
	jack_server = AGS_JACK_SERVER(sound_server->data);

	jack_devin = (AgsJackDevin *) ags_sound_server_register_soundcard(AGS_SOUND_SERVER(jack_server),
									  is_output);
	soundcard = (GObject *) jack_devin;
      }else{
	//TODO:JK: implement me
      }
    }else{
      if(use_alsa){
	alsa_devin = ags_alsa_devin_new();

	soundcard = (GObject *) alsa_devin;      
      }else if(use_oss){
	oss_devin = ags_oss_devin_new();

	soundcard = (GObject *) oss_devin;
      }else if(use_wasapi){
	wasapi_devin = ags_wasapi_devin_new();

	soundcard = (GObject *) wasapi_devin;
      }else{
	//TODO:JK: implement me
      }
    }
  
    if(soundcard != NULL){
      soundcard_editor->soundcard = soundcard;
      soundcard_editor->soundcard_thread = (GObject *) ags_thread_find_type(main_loop,
									    AGS_TYPE_SOUNDCARD_THREAD);

      if((start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context))) == NULL){
	initial_soundcard = TRUE;
      }

      g_object_ref(soundcard);
      start_list = g_list_append(start_list,
				 soundcard);
    
      ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				       start_list);

      g_list_foreach(start_list,
		     (GFunc) g_object_unref,
		     NULL);

      soundcard_thread = (AgsThread *) ags_soundcard_thread_new(soundcard,
								ags_soundcard_get_capability(AGS_SOUNDCARD(soundcard)));
      soundcard_editor->soundcard_thread = (GObject *) soundcard_thread;
  
      ags_thread_add_child_extended(main_loop,
				    soundcard_thread,
				    TRUE, TRUE);

      if((default_soundcard_thread = (AgsThread *) ags_sound_provider_get_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context))) == NULL){
	ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context),
							(GObject *) soundcard_thread);
      }else{
	g_object_unref(default_soundcard_thread);
      }

      /* export thread */
      export_thread = (AgsThread *) ags_export_thread_new(soundcard,
							  NULL);
      ags_thread_add_child_extended(main_loop,
				    (AgsThread *) export_thread,
				    TRUE, TRUE);
    }
  
    ags_audio_preferences_add_soundcard_editor(audio_preferences,
					       soundcard_editor);
  
    /*  */
    start_card_id = NULL;
    start_card_name = NULL;
  
    ags_soundcard_list_cards(AGS_SOUNDCARD(soundcard),
			     &start_card_id, &start_card_name);

    gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card))));

    card_id = start_card_id;
    card_name = start_card_name;
  
    while(card_id != NULL){
#ifdef AGS_WITH_CORE_AUDIO
      tmp = card_name->data;
#else
      tmp = card_id->data;
#endif
    
      if(tmp != NULL){
	gtk_combo_box_text_append_text(soundcard_editor->card,
				       tmp);
      }
    
      card_id = card_id->next;
      card_name = card_name->next;
    }

    ags_applicable_reset(AGS_APPLICABLE(soundcard_editor));
    ags_connectable_connect(AGS_CONNECTABLE(soundcard_editor));

    g_signal_connect(soundcard_editor->remove, "clicked",
		     G_CALLBACK(ags_audio_preferences_remove_soundcard_editor_callback), audio_preferences);

    gtk_widget_show((GtkWidget *) soundcard_editor);

    /* unref */
    g_list_free_full(start_card_id,
		     (GDestroyNotify) g_free);
  
    g_list_free_full(start_card_name,
		     (GDestroyNotify) g_free);
  
    /* reset default card */  
    g_object_unref(main_loop);  
  }
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
		   (GtkWidget *) soundcard_editor);
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
    
    gtk_box_remove(GTK_BOX(audio_preferences->soundcard_editor_box),
		   GTK_WIDGET(soundcard_editor));
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
