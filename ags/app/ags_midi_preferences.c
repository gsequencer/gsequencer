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

#include <ags/app/ags_midi_preferences.h>
#include <ags/app/ags_midi_preferences_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_preferences.h>
#include <ags/app/ags_sequencer_editor.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_midi_preferences_class_init(AgsMidiPreferencesClass *midi_preferences);
void ags_midi_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_midi_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_midi_preferences_init(AgsMidiPreferences *midi_preferences);

gboolean ags_midi_preferences_is_connected(AgsConnectable *connectable);
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
 * @include: ags/app/ags_midi_preferences.h
 *
 * #AgsMidiPreferences enables you to make preferences of sequencer.
 */

static gpointer ags_midi_preferences_parent_class = NULL;

GType
ags_midi_preferences_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
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

    g_once_init_leave(&g_define_type_id__static, ags_type_midi_preferences);
  }

  return(g_define_type_id__static);
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
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;

  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_midi_preferences_is_connected;  
  connectable->connect = ags_midi_preferences_connect;
  connectable->disconnect = ags_midi_preferences_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
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
  AgsPreferences *preferences;

  GtkScrolledWindow *scrolled_window;

  GMenuModel *add_popup;
  GMenu *menu;
  GMenuItem *item;
  
  GSimpleActionGroup *action_group;
  GSimpleAction *action;
  
  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  preferences = (AgsPreferences *) ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context));

  gtk_orientable_set_orientation(GTK_ORIENTABLE(midi_preferences),
				 GTK_ORIENTATION_VERTICAL);

  gtk_box_set_spacing((GtkBox *) midi_preferences,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);  
  
  g_signal_connect((GObject *) midi_preferences, "notify::parent",
		   G_CALLBACK(ags_midi_preferences_notify_parent_callback), NULL);

  midi_preferences->connectable_flags = 0;
  
  /* action group */
  action_group = g_simple_action_group_new();
  gtk_widget_insert_action_group((GtkWidget *) preferences,
				 "midi_preferences",
				 G_ACTION_GROUP(action_group));

#if defined(AGS_WITH_ALSA)
  /* add ALSA input sequencer */
  action = g_simple_action_new("add_alsa_input_sequencer",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_midi_preferences_add_alsa_input_sequencer_callback), midi_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
#endif

#if defined(AGS_WITH_OSS)
  /* add OSS input sequencer */
  action = g_simple_action_new("add_oss_input_sequencer",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_midi_preferences_add_oss_input_sequencer_callback), midi_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
#endif

#if defined(AGS_WITH_JACK)
  /* add JACK input sequencer */
  action = g_simple_action_new("add_jack_input_sequencer",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_midi_preferences_add_jack_input_sequencer_callback), midi_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
#endif

#if defined(AGS_WITH_PULSE)
  //nothing
#endif

#if defined(AGS_WITH_CORE_MIDI)
  /* add Core Midi input sequencer */
  action = g_simple_action_new("add_core_midi_input_sequencer",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_midi_preferences_add_core_midi_input_sequencer_callback), midi_preferences);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
#endif

#if defined(AGS_WITH_WASAPI)
  /* add WASAPI input sequencer */
  action = g_simple_action_new("add_wasapi_input_sequencer",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_midi_preferences_add_wasapi_input_sequencer_callback), midi_preferences);
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
  
  gtk_box_append((GtkBox *) midi_preferences,
		 (GtkWidget *) scrolled_window);
  
  midi_preferences->sequencer_editor_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
								  AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) midi_preferences->sequencer_editor_box);

  /*  */
  midi_preferences->add_menu_button = (GtkMenuButton *) gtk_menu_button_new();
  gtk_box_prepend(preferences->action_area,
		  (GtkWidget *) midi_preferences->add_menu_button);
  
  menu = (GMenu *) g_menu_new();
  add_popup = G_MENU_MODEL(menu);

  /*  */
#if defined(AGS_WITH_ALSA)
  item = g_menu_item_new(i18n("add ALSA input sequencer"),
			 "midi_preferences.add_alsa_input_sequencer");
  g_menu_append_item(menu,
		     item);
#endif

#if defined(AGS_WITH_OSS)
  item = g_menu_item_new(i18n("add OSSv4 input sequencer"),
			 "midi_preferences.add_oss_input_sequencer");
  g_menu_append_item(menu,
		     item);
#endif

#if defined(AGS_WITH_JACK)
  item = g_menu_item_new(i18n("add JACK input sequencer"),
			 "midi_preferences.add_jack_input_sequencer");
  g_menu_append_item(menu,
		     item);
#endif

#if defined(AGS_WITH_PULSE)
#endif

#if defined(AGS_WITH_CORE_MIDI)
  item = g_menu_item_new(i18n("add Core Midi input sequencer"),
			 "midi_preferences.add_core_midi_input_sequencer");
  g_menu_append_item(menu,
		     item);
#endif

#if defined(AGS_WITH_WASAPI)
  item = g_menu_item_new(i18n("add WASAPI input sequencer"),
			 "midi_preferences.add_wasapi_input_sequencer");
  g_menu_append_item(menu,
		     item);
#endif
  
  gtk_menu_button_set_menu_model(midi_preferences->add_menu_button,
				 add_popup);

  gtk_widget_set_visible((GtkWidget *) midi_preferences->add_menu_button,
			 FALSE);
  
  midi_preferences->add = NULL;  
}

static void
ags_midi_preferences_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_midi_preferences_parent_class)->finalize(gobject);
}

gboolean
ags_midi_preferences_is_connected(AgsConnectable *connectable)
{
  AgsMidiPreferences *midi_preferences;
  
  gboolean is_connected;
  
  midi_preferences = AGS_MIDI_PREFERENCES(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (midi_preferences->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_midi_preferences_connect(AgsConnectable *connectable)
{
  AgsMidiPreferences *midi_preferences;

  midi_preferences = AGS_MIDI_PREFERENCES(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  midi_preferences->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
}

void
ags_midi_preferences_disconnect(AgsConnectable *connectable)
{
  AgsMidiPreferences *midi_preferences;

  midi_preferences = AGS_MIDI_PREFERENCES(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  midi_preferences->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
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
  
  AgsThread *main_loop;
  AgsThread *first_sequencer_thread, *sequencer_thread;
  
  AgsApplicationContext *application_context;

  GList *start_list, *list;
  GList *start_sequencer, *sequencer;
  GList *tmp_sequencer;

  gboolean found_editor;
  
  midi_preferences = AGS_MIDI_PREFERENCES(applicable);

  application_context = ags_application_context_get_instance();

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  list =
    start_list = ags_midi_preferences_get_sequencer_editor(midi_preferences);

  while(list != NULL){
    ags_applicable_apply(AGS_APPLICABLE(list->data));

    list = list->next;
  }

  /* reset */
  sequencer =
    start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));

  while(sequencer != NULL){
    list = start_list;

    found_editor = FALSE;

    while(list != NULL){
      if(AGS_SEQUENCER_EDITOR(list->data)->sequencer == (GObject *) sequencer->data){
	found_editor = TRUE;

	break;
      }

      list = list->next;
    }
    
    if(!found_editor){
      tmp_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));

      if(g_list_find(tmp_sequencer, sequencer->data) != NULL){
	first_sequencer_thread = ags_thread_find_type(main_loop,
						      AGS_TYPE_SEQUENCER_THREAD);

	sequencer_thread = (AgsThread *) ags_sequencer_thread_find_sequencer((AgsSequencerThread *) first_sequencer_thread,
									     sequencer->data);

	/* remove */
	tmp_sequencer = g_list_remove(tmp_sequencer,
				      sequencer->data);
	
	ags_thread_remove_child(main_loop,
				sequencer_thread);

	/* unref */
	g_object_unref(sequencer->data);

	g_object_unref(first_sequencer_thread);

	g_object_unref(sequencer_thread);
      }
      
      ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				       tmp_sequencer);
    }
        
    sequencer = sequencer->next;
  }

  /* unref */
  g_list_free_full(start_sequencer,
		   (GDestroyNotify) g_object_unref);
  
  g_list_free(start_list);
}

void
ags_midi_preferences_reset(AgsApplicable *applicable)
{
  AgsMidiPreferences *midi_preferences;
  AgsSequencerEditor *sequencer_editor;

  AgsThread *main_loop;
  AgsThread *sequencer_thread;
  
  AgsApplicationContext *application_context;

  GList *start_list, *list;

  midi_preferences = AGS_MIDI_PREFERENCES(applicable);
  
  application_context = ags_application_context_get_instance();

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  
  sequencer_thread = ags_thread_find_type(main_loop,
					  AGS_TYPE_SEQUENCER_THREAD);

  /* clear */
  list =
    start_list = ags_midi_preferences_get_sequencer_editor(midi_preferences);

  while(list != NULL){
    ags_midi_preferences_remove_sequencer_editor(midi_preferences,
						 list->data);
    
    list = list->next;
  }
  
  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);

  /* reset */
  list =
    start_list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    sequencer_editor = ags_sequencer_editor_new();    

    sequencer_editor->sequencer = list->data;
    sequencer_editor->sequencer_thread = (GObject *) ags_sequencer_thread_find_sequencer((AgsSequencerThread *) sequencer_thread,
											 list->data);

    ags_midi_preferences_add_sequencer_editor(midi_preferences,
					      sequencer_editor);

    gtk_widget_show((GtkWidget *) sequencer_editor);
    
    ags_applicable_reset(AGS_APPLICABLE(sequencer_editor));
    ags_connectable_connect(AGS_CONNECTABLE(sequencer_editor));

    g_signal_connect(sequencer_editor->remove, "clicked",
		     G_CALLBACK(ags_midi_preferences_remove_sequencer_editor_callback), midi_preferences);
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
  
  /* unref */
  g_object_unref(main_loop);
}

/**
 * ags_midi_preferences_add_sequencer:
 * @midi_preferences: the #AgsMidi_Preferences
 * @backend: the backend to use
 * @is_output: %TRUE if is output, otherwise %FALSE
 * 
 * Add sequencer.
 *
 * Since: 8.3.2
 */
void
ags_midi_preferences_add_sequencer(AgsMidiPreferences *midi_preferences,
				   gchar *backend,
				   gboolean is_output)
{
  g_return_if_fail(backend != NULL);
  
  if(is_output){
    //empty    
  }

  if(!is_output){
    AgsSequencerEditor *sequencer_editor;
  
    AgsPulseServer *pulse_server;

    AgsJackServer *jack_server;
    AgsJackMidiin *jack_midiin;
  
    AgsCoreAudioServer *core_audio_server;
    AgsCoreAudioMidiin *core_audio_midiin;

    AgsAlsaMidiin *alsa_midiin;

    AgsOssMidiin *oss_midiin;
  
    AgsThread *main_loop;
    AgsThread *sequencer_thread, *default_sequencer_thread;
  
    AgsApplicationContext *application_context;

    GObject *sequencer;

    GType server_type;
  
    GList *start_sound_server, *sound_server;
    GList *start_list, *list;
    GList *start_card_id, *card_id;
    GList *start_card_name, *card_name;
  
    gchar *tmp;
  
    gboolean use_alsa, use_oss, use_core_audio, use_pulse, use_jack, use_wasapi;
    gboolean is_output;
    gboolean initial_sequencer;

    application_context = ags_application_context_get_instance();

    main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

    core_audio_server = NULL;
    core_audio_midiin = NULL;
  
    pulse_server = NULL;
  
    jack_server = NULL;
    jack_midiin = NULL;

    alsa_midiin = NULL;

    oss_midiin = NULL;

    /* sequencer */
    sequencer = NULL;

    server_type = G_TYPE_NONE;
  
    use_alsa = FALSE;
    use_oss = FALSE;
    use_core_audio = FALSE;
    use_pulse = FALSE;
    use_jack = FALSE;
    use_wasapi = FALSE;

    is_output = FALSE;
  
    initial_sequencer = FALSE;
  
    /* sequencer editor */
    sequencer_editor = ags_sequencer_editor_new();

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
				    "core-midi",
				    10)){
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

    /* create sequencer */
    sound_server =
      start_sound_server = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));

    if((sound_server = ags_list_util_find_type(start_sound_server,
					       server_type)) != NULL){
      if(use_core_audio){
	GValue *param_value = NULL;
	
	gchar **param_strv = NULL;
      
	core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server->data);

	core_audio_midiin = (AgsCoreAudioMidiin *) ags_sound_server_register_sequencer_with_params(AGS_SOUND_SERVER(core_audio_server),
												   is_output,
												   (gchar **) param_strv, param_value);
	sequencer = (GObject *) core_audio_midiin;

	g_strfreev(param_strv);
	g_free(param_value);
      }else if(use_pulse){
	pulse_server = AGS_PULSE_SERVER(sound_server->data);
      
	//TODO:JK: implement me
      }else if(use_jack){
	jack_server = AGS_JACK_SERVER(sound_server->data);

	jack_midiin = (AgsJackMidiin *) ags_sound_server_register_sequencer(AGS_SOUND_SERVER(jack_server),
									    is_output);
	sequencer = (GObject *) jack_midiin;
      }else{
	//TODO:JK: implement me
      }
    }else{
      if(use_alsa){
	alsa_midiin = ags_alsa_midiin_new();

	sequencer = (GObject *) alsa_midiin;      
      }else if(use_oss){
	oss_midiin = ags_oss_midiin_new();

	sequencer = (GObject *) oss_midiin;
      }else if(use_wasapi){
	//TODO:JK: implement me
      }else{
	//TODO:JK: implement me
      }
    }
  
    if(sequencer != NULL){
      sequencer_editor->sequencer = sequencer;
      sequencer_editor->sequencer_thread = (GObject *) ags_thread_find_type(main_loop,
									    AGS_TYPE_SEQUENCER_THREAD);

      if((start_list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context))) == NULL){
	initial_sequencer = TRUE;
      }

      g_object_ref(sequencer);
      start_list = g_list_append(start_list,
				 sequencer);
    
      ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				       start_list);

      g_list_foreach(start_list,
		     (GFunc) g_object_unref,
		     NULL);

      sequencer_thread = (AgsThread *) ags_sequencer_thread_new(sequencer);
      sequencer_editor->sequencer_thread = (GObject *) sequencer_thread;
  
      ags_thread_add_child_extended(main_loop,
				    sequencer_thread,
				    TRUE, TRUE);
    }
  
    ags_midi_preferences_add_sequencer_editor(midi_preferences,
					      sequencer_editor);

    /*  */
    start_card_id = NULL;
    start_card_name = NULL;
  
    ags_sequencer_list_cards(AGS_SEQUENCER(sequencer),
			     &start_card_id, &start_card_name);

    gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card))));

    card_id = start_card_id;
    card_name = start_card_name;
  
    while(card_id != NULL){
#ifdef AGS_WITH_CORE_AUDIO
      tmp = card_name->data;
#else
      tmp = card_id->data;
#endif
    
      if(tmp != NULL){
	gtk_combo_box_text_append_text(sequencer_editor->card,
				       tmp);
      }
    
      card_id = card_id->next;
      card_name = card_name->next;
    }
  
    ags_applicable_reset(AGS_APPLICABLE(sequencer_editor));
    ags_connectable_connect(AGS_CONNECTABLE(sequencer_editor));

    g_signal_connect(sequencer_editor->remove, "clicked",
		     G_CALLBACK(ags_midi_preferences_remove_sequencer_editor_callback), midi_preferences);

    gtk_widget_show((GtkWidget *) sequencer_editor);

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
 * ags_midi_preferences_get_sequencer_editor:
 * @midi_preferences: the #AgsMidi_Preferences
 * 
 * Get line member of @midi_preferences.
 * 
 * Returns: the #GList-struct containing #AgsSequencerEditor
 *
 * Since: 4.0.0
 */
GList*
ags_midi_preferences_get_sequencer_editor(AgsMidiPreferences *midi_preferences)
{
  g_return_val_if_fail(AGS_IS_MIDI_PREFERENCES(midi_preferences), NULL);

  return(g_list_reverse(g_list_copy(midi_preferences->sequencer_editor)));
}

/**
 * ags_midi_preferences_add_sequencer_editor:
 * @midi_preferences: the #AgsMidi_Preferences
 * @sequencer_editor: the #AgsSequencerEditor
 * 
 * Add @sequencer_editor to @midi_preferences.
 * 
 * Since: 4.0.0
 */
void
ags_midi_preferences_add_sequencer_editor(AgsMidiPreferences *midi_preferences,
					  AgsSequencerEditor *sequencer_editor)
{
  g_return_if_fail(AGS_IS_MIDI_PREFERENCES(midi_preferences));
  g_return_if_fail(AGS_IS_SEQUENCER_EDITOR(sequencer_editor));

  if(g_list_find(midi_preferences->sequencer_editor, sequencer_editor) == NULL){
    midi_preferences->sequencer_editor = g_list_prepend(midi_preferences->sequencer_editor,
							sequencer_editor);
    
    gtk_box_append(midi_preferences->sequencer_editor_box,
		   (GtkWidget *) sequencer_editor);
  }
}

/**
 * ags_midi_preferences_remove_sequencer_editor:
 * @midi_preferences: the #AgsMidi_Preferences
 * @sequencer_editor: the #AgsSequencerEditor
 * 
 * Remove @sequencer_editor from @midi_preferences.
 * 
 * Since: 4.0.0
 */
void
ags_midi_preferences_remove_sequencer_editor(AgsMidiPreferences *midi_preferences,
					     AgsSequencerEditor *sequencer_editor)
{
  g_return_if_fail(AGS_IS_MIDI_PREFERENCES(midi_preferences));
  g_return_if_fail(AGS_IS_SEQUENCER_EDITOR(sequencer_editor));

  if(g_list_find(midi_preferences->sequencer_editor, sequencer_editor) != NULL){
    midi_preferences->sequencer_editor = g_list_remove(midi_preferences->sequencer_editor,
						       sequencer_editor);
    
    gtk_box_remove(midi_preferences->sequencer_editor_box,
		   (GtkWidget *) sequencer_editor);
  }
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
