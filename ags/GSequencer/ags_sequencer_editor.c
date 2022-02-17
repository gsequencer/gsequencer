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

#include <ags/GSequencer/ags_sequencer_editor.h>
#include <ags/GSequencer/ags_sequencer_editor_callbacks.h>

#include <ags/GSequencer/ags_ui_provider.h>
#include <ags/GSequencer/ags_preferences.h>

#include <ags/GSequencer/ags_midi_preferences.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_sequencer_editor_class_init(AgsSequencerEditorClass *sequencer_editor);
void ags_sequencer_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sequencer_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_sequencer_editor_init(AgsSequencerEditor *sequencer_editor);

void ags_sequencer_editor_connect(AgsConnectable *connectable);
void ags_sequencer_editor_disconnect(AgsConnectable *connectable);

void ags_sequencer_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_sequencer_editor_apply(AgsApplicable *applicable);
void ags_sequencer_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_sequencer_editor
 * @short_description: A composite widget to configure sequencer
 * @title: AgsSequencerEditor
 * @section_id: 
 * @include: ags/X/ags_sequencer_editor.h
 *
 * #AgsSequencerEditor enables you to make preferences of sequencer.
 */

static gpointer ags_sequencer_editor_parent_class = NULL;

GType
ags_sequencer_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sequencer_editor = 0;

    static const GTypeInfo ags_sequencer_editor_info = {
      sizeof (AgsSequencerEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sequencer_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSequencerEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sequencer_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sequencer_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_sequencer_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_sequencer_editor = g_type_register_static(GTK_TYPE_BOX,
						       "AgsSequencerEditor", &ags_sequencer_editor_info,
						       0);
    
    g_type_add_interface_static(ags_type_sequencer_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_sequencer_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sequencer_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_sequencer_editor_class_init(AgsSequencerEditorClass *sequencer_editor)
{
  ags_sequencer_editor_parent_class = g_type_class_peek_parent(sequencer_editor);
}

void
ags_sequencer_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_sequencer_editor_connect;
  connectable->disconnect = ags_sequencer_editor_disconnect;
}

void
ags_sequencer_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_sequencer_editor_set_update;
  applicable->apply = ags_sequencer_editor_apply;
  applicable->reset = ags_sequencer_editor_reset;
}

void
ags_sequencer_editor_init(AgsSequencerEditor *sequencer_editor)
{
  GtkGrid *grid;
  GtkLabel *label;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(sequencer_editor),
				 GTK_ORIENTATION_VERTICAL);  

  sequencer_editor->flags = 0;
  
  sequencer_editor->sequencer = NULL;
  sequencer_editor->sequencer_thread = NULL;
  
  grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start(GTK_BOX(sequencer_editor),
		     GTK_WIDGET(grid),
		     FALSE, FALSE,
		     2);

  /* backend */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("backend"),
				    "xalign", 0.0,
				    NULL);

  gtk_widget_set_valign(label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(label,
			GTK_ALIGN_FILL);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, 0,
		  1, 1);

  sequencer_editor->backend = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_widget_set_valign(sequencer_editor->backend,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(sequencer_editor->backend,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(sequencer_editor->backend),
		  1, 0,
		  1, 1);

  gtk_combo_box_text_append_text(sequencer_editor->backend,
				 "jack");
  
#ifdef AGS_WITH_ALSA
  gtk_combo_box_text_append_text(sequencer_editor->backend,
				 "alsa");
#endif
  
#ifdef AGS_WITH_OSS
  gtk_combo_box_text_append_text(sequencer_editor->backend,
				 "oss");
#endif

  gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			   0);
  
  /* MIDI card */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("MIDI card"),
				    "xalign", 0.0,
				    NULL);

  gtk_widget_set_valign(label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(label,
			GTK_ALIGN_FILL);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, 1,
		  1, 1);

  sequencer_editor->card = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_widget_set_valign(sequencer_editor->card,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(sequencer_editor->card,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(sequencer_editor->card),
		  1, 1,
		  1, 1);

  /* jack */
  sequencer_editor->jack_hbox = NULL;
  sequencer_editor->add_jack = NULL;
  sequencer_editor->remove_jack = NULL;

  sequencer_editor->jack_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						       0);
  
  gtk_widget_set_valign(sequencer_editor->jack_hbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(sequencer_editor->jack_hbox,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(sequencer_editor->jack_hbox),
		  2, 1,
		  1, 1);

  sequencer_editor->add_jack = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Add"));
  gtk_box_pack_start((GtkBox *) sequencer_editor->jack_hbox,
		     (GtkWidget *) sequencer_editor->add_jack,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);
  
  sequencer_editor->remove_jack = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Remove"));
  gtk_box_pack_start((GtkBox *) sequencer_editor->jack_hbox,
		     (GtkWidget *) sequencer_editor->remove_jack,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);
  
  /*  */
  sequencer_editor->remove = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Remove"));

  gtk_widget_set_valign(sequencer_editor->remove,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(sequencer_editor->remove,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		   GTK_WIDGET(sequencer_editor->remove),
		   2, 7,
		   1, 1);
}

void
ags_sequencer_editor_connect(AgsConnectable *connectable)
{
  AgsSequencerEditor *sequencer_editor;

  sequencer_editor = AGS_SEQUENCER_EDITOR(connectable);

  if((AGS_SEQUENCER_EDITOR_CONNECTED & (sequencer_editor->flags)) != 0){
    return;
  }

  sequencer_editor->flags |= AGS_SEQUENCER_EDITOR_CONNECTED;
  
  /* backend and card */
  g_signal_connect(G_OBJECT(sequencer_editor->backend), "changed",
		   G_CALLBACK(ags_sequencer_editor_backend_changed_callback), sequencer_editor);

  g_signal_connect(G_OBJECT(sequencer_editor->card), "changed",
		   G_CALLBACK(ags_sequencer_editor_card_changed_callback), sequencer_editor);

  /* add / remove jack */
  g_signal_connect(G_OBJECT(sequencer_editor->add_jack), "clicked",
		   G_CALLBACK(ags_sequencer_editor_add_jack_callback), sequencer_editor);

  g_signal_connect(G_OBJECT(sequencer_editor->remove_jack), "clicked",
		   G_CALLBACK(ags_sequencer_editor_remove_jack_callback), sequencer_editor);
}

void
ags_sequencer_editor_disconnect(AgsConnectable *connectable)
{
  AgsSequencerEditor *sequencer_editor;

  sequencer_editor = AGS_SEQUENCER_EDITOR(connectable);

  if((AGS_SEQUENCER_EDITOR_CONNECTED & (sequencer_editor->flags)) == 0){
    return;
  }

  sequencer_editor->flags &= (~AGS_SEQUENCER_EDITOR_CONNECTED);

  /* backend and card */
  g_object_disconnect(G_OBJECT(sequencer_editor->backend),
		      "any_signal::changed",
		      G_CALLBACK(ags_sequencer_editor_backend_changed_callback),
		      sequencer_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(sequencer_editor->card),
		      "any_signal::changed",
		      G_CALLBACK(ags_sequencer_editor_card_changed_callback),
		      sequencer_editor,
		      NULL);

  /* add / remove jack */
  g_object_disconnect(G_OBJECT(sequencer_editor->add_jack),
		      "any_signal::clicked",
		      G_CALLBACK(ags_sequencer_editor_add_jack_callback),
		      sequencer_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(sequencer_editor->remove_jack),
		      "any_signal::clicked",
		      G_CALLBACK(ags_sequencer_editor_remove_jack_callback),
		      sequencer_editor,
		      NULL);
}

void
ags_sequencer_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_sequencer_editor_apply(AgsApplicable *applicable)
{
  AgsMidiPreferences *midi_preferences;
  AgsSequencerEditor *sequencer_editor;
  
  GtkListStore *model;
  GtkTreeIter current;
    
  AgsConfig *config;

  GList *list;	

  gchar *sequencer_group;
  gchar *backend;
  char *device;

  gint nth;
  gboolean use_jack, use_alsa, use_oss;
  
  GValue value =  {0,};

  sequencer_editor = AGS_SEQUENCER_EDITOR(applicable);
  midi_preferences = (AgsMidiPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(sequencer_editor),
								      AGS_TYPE_MIDI_PREFERENCES);

  config = ags_config_get_instance();

  list = gtk_container_get_children((GtkContainer *) midi_preferences->sequencer_editor);
  nth = g_list_index(list,
		     sequencer_editor);

  if(nth < 0){
    return;
  }
  
  sequencer_group = g_strdup_printf("%s-%d",
				    AGS_CONFIG_SEQUENCER,
				    nth);
  g_list_free(list);
  
  /* backend */
  use_jack = TRUE;
  use_alsa = FALSE;
  use_oss = FALSE;

  backend = gtk_combo_box_text_get_active_text(sequencer_editor->backend);
  ags_config_set_value(config,
		       sequencer_group,
		       "backend",
		       backend);

  if(backend != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "jack",
			    5)){
      use_jack = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
			    "alsa",
			    5)){
      use_alsa = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
				  "oss",
				  4)){
      use_oss = TRUE;
    }
  }

  /* device */
  model = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card)));
  
  if(gtk_combo_box_get_active_iter(GTK_COMBO_BOX(sequencer_editor->card),
				   &current)){
    gtk_tree_model_get_value(GTK_TREE_MODEL(model),
			     &current,
			     0,
			     &value);
    device = g_value_get_string(&value);
  }else{
    device = NULL;
  }

  /* handle */
  g_message("%s", device);

  if(use_jack){
    ags_config_set_value(config,
			 sequencer_group,
			 "device",
			 device);
  }else if(use_alsa){
    ags_config_set_value(config,
			 sequencer_group,
			 "device",
			 device);
  }else if(use_oss){
    ags_config_set_value(config,
			 sequencer_group,
			 "device",
			 device);
  }
}

void
ags_sequencer_editor_reset(AgsApplicable *applicable)
{
  AgsSequencerEditor *sequencer_editor;

  GObject *sequencer;

  GList *card_id, *card_id_start, *card_name, *card_name_start;

  gchar *backend, *device, *tmp;

  guint nth;
  gboolean use_alsa;
  gboolean found_card;
    
  sequencer_editor = AGS_SEQUENCER_EDITOR(applicable);
  sequencer = sequencer_editor->sequencer;
  
  /* refresh */
  use_alsa = FALSE;

  backend = NULL;
  
  if(AGS_IS_JACK_MIDIIN(sequencer)){
    backend = "jack";
  }else if(AGS_IS_ALSA_MIDIIN(sequencer)){
    backend = "alsa";
  }else if(AGS_IS_OSS_MIDIIN(sequencer)){
    backend = "oss";
  }

  if(backend != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "jack",
			    5)){
      gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			       0);
      
      ags_sequencer_editor_load_jack_card(sequencer_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "alsa",
				  5)){
      use_alsa = TRUE;

#ifdef AGS_WITH_ALSA
      gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			       1);
#endif
      
      ags_sequencer_editor_load_alsa_card(sequencer_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "oss",
				  4)){
#ifdef AGS_WITH_ALSA
      gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			       2);
#else
      gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			       -1);
#endif
      
      ags_sequencer_editor_load_oss_card(sequencer_editor);
    }
  }

  /*  */
  device = ags_sequencer_get_device(AGS_SEQUENCER(sequencer));
  
  card_id_start = NULL;
  card_name_start = NULL;

  ags_sequencer_list_cards(AGS_SEQUENCER(sequencer),
			   &card_id_start, &card_name_start);

  card_id = card_id_start;
  card_name = card_name_start;
  
  nth = 0;
  found_card = FALSE;

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card))));

  while(card_id != NULL){
    //FIXME:JK: work-around for alsa-handle
    tmp = card_id->data;
    
    if(card_id->data != NULL &&
       use_alsa){
      tmp = g_strdup_printf("%s,0",
			    (gchar *) card_id->data);
    }
    
    if(tmp != NULL &&
       device != NULL &&
       !g_ascii_strcasecmp(tmp,
			   device)){
      found_card = TRUE;
    }

    if(tmp != NULL){
      gtk_combo_box_text_append_text(sequencer_editor->card,
				     tmp);
    }
    
    if(use_alsa){
      g_free(tmp);
    }
    
    if(!found_card){
      nth++;
    }
    
    card_id = card_id->next;
    card_name = card_name->next;
  }
  
  if(!found_card){
    nth = 0;
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->card),
			   nth);

  if(card_id_start != NULL){
    g_list_free_full(card_id_start,
		     g_free);
    g_list_free_full(card_name_start,
		     g_free);
  }
}

void
ags_sequencer_editor_add_source(AgsSequencerEditor *sequencer_editor,
				gchar *device)
{
  AgsJackServer *jack_server;
  AgsJackMidiin *jack_midiin;

  AgsThread *main_loop;
  AgsThread *sequencer_thread;

  AgsApplicationContext *application_context;

  GList *start_sound_server, *sound_server;
  GList *start_sequencer;
  GList *card_name, *card_uri;

  application_context = ags_application_context_get_instance();
  
  /* create sequencer */
  sound_server =
    start_sound_server = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));

  if((sound_server = ags_list_util_find_type(sound_server,
					     AGS_TYPE_JACK_SERVER)) != NULL){
    jack_server = AGS_JACK_SERVER(sound_server->data);
  }else{
    g_warning("sound server not found");
    
    g_list_free_full(start_sound_server,
		     g_object_unref);

    return;
  }
  
  jack_midiin = (AgsJackMidiin *) ags_sound_server_register_sequencer(AGS_SOUND_SERVER(jack_server),
								      FALSE);

  if(jack_midiin == NULL){    
    g_list_free_full(start_sound_server,
		     g_object_unref);

    return;
  }
  
  /* add new */
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  
  sequencer_editor->sequencer = (GObject *) jack_midiin;

  start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
  g_list_foreach(start_sequencer,
		 (GFunc) g_object_unref,
		 NULL);
    
  g_object_ref(jack_midiin);  
  ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				   g_list_append(start_sequencer,
						 jack_midiin));

  sequencer_thread = (AgsThread *) ags_sequencer_thread_new((GObject *) jack_midiin);
  sequencer_editor->sequencer_thread = (GObject *) sequencer_thread;
  
  ags_thread_add_child_extended(main_loop,
				sequencer_thread,
				TRUE, TRUE);

  /*  */
  card_name = NULL;
  card_uri = NULL;
  ags_sequencer_list_cards(AGS_SEQUENCER(jack_midiin),
			   &card_uri, &card_name);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card))));

  while(card_uri != NULL){
    if(card_uri->data != NULL){
      gtk_combo_box_text_append_text(sequencer_editor->card,
				     card_uri->data);
    }
    
    card_uri = card_uri->next;
  }

  g_object_unref(main_loop);

  /* unref */
  g_list_free_full(start_sound_server,
		   g_object_unref);
}

void
ags_sequencer_editor_remove_source(AgsSequencerEditor *sequencer_editor,
				   gchar *device)
{
  AgsJackMidiin *jack_midiin;

  AgsApplicationContext *application_context;
  AgsThread *main_loop;

  GList *start_sound_server, *sound_server;
  GList *start_sequencer, *sequencer;

  application_context = ags_application_context_get_instance();
  
  /* create sequencer */
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  
  start_sound_server = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));

  if((sound_server = ags_list_util_find_type(start_sound_server,
					     AGS_TYPE_JACK_SERVER)) == NULL){
    g_object_unref(main_loop);

    g_list_free_full(start_sound_server,
		     g_object_unref);
    
    g_warning("sound server not found");
    
    return;
  }

  jack_midiin = NULL;

  sequencer = 
    start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
  
  while(sequencer != NULL){
    if(AGS_IS_JACK_MIDIIN(sequencer->data) &&
       !g_ascii_strcasecmp(ags_sequencer_get_device(AGS_SEQUENCER(sequencer->data)),
			   device)){
      jack_midiin = sequencer->data;

      break;
    }
    
    sequencer = sequencer->next;
  }
  
  if(jack_midiin == NULL){
    g_object_unref(main_loop);

    g_list_free_full(start_sound_server,
		     g_object_unref);

    g_list_free_full(start_sequencer,
		     g_object_unref);

    return;
  }
  
  /*  */
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card))));
  gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			   -1);

#if 0
  if((sound_server = ags_list_util_find_type(start_sound_server,
					     AGS_TYPE_JACK_SERVER)) != NULL){
    ags_sound_server_unregister_sequencer(AGS_SOUND_SERVER(sound_server->data),
						 jack_midiin);
  }
#endif

  /* remove */
  if((GObject *) jack_midiin == sequencer_editor->sequencer){
    sequencer_editor->sequencer = NULL;
  }

#if 0
  if(jack_midiin != NULL){
    GList *tmp;

    tmp = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
					   
    ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				     g_list_remove(tmp,
						   jack_midiin));
    g_object_unref(jack_midiin);

    g_list_free_full(tmp,
		     g_object_unref);
  }
#endif
  
#if 0
  if(sequencer_editor->sequencer_thread != NULL){
    ags_thread_stop((AgsThread *) sequencer_editor->sequencer_thread);

    ags_thread_remove_child(main_loop,
			    (AgsThread *) sequencer_editor->sequencer_thread);
    
    //    g_object_unref(sequencer_editor->sequencer_thread);
    
    sequencer_editor->sequencer_thread = NULL;
  }
#endif

  /* unref */
  g_object_unref(main_loop);

  g_list_free_full(start_sound_server,
		   g_object_unref);

  g_list_free_full(start_sequencer,
		   g_object_unref);
}

void
ags_sequencer_editor_add_sequencer(AgsSequencerEditor *sequencer_editor,
				   GObject *sequencer)
{
  AgsThread *main_loop;
  AgsThread *sequencer_thread;

  AgsApplicationContext *application_context;

  GList *start_sequencer;
  
  if(sequencer == NULL ||
     AGS_IS_JACK_MIDIIN(sequencer)){
    return;
  }
  
  application_context = ags_application_context_get_instance();

  if(AGS_IS_ALSA_MIDIIN(sequencer)){
    ags_sequencer_set_device(AGS_SEQUENCER(sequencer),
			     "hw:0,0");
  }else if(AGS_IS_OSS_MIDIIN(sequencer)){
    ags_sequencer_set_device(AGS_SEQUENCER(sequencer),
			     "/dev/midi00");
  }else{
    g_warning("unknown sequencer implementation");
  }
  
  /*  */
  start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
  
  if(g_list_find(start_sequencer,
		 sequencer) != NULL){
    g_list_free_full(start_sequencer,
		     g_object_unref);
    
    return;
  }

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  
  sequencer_editor->sequencer = sequencer;

  g_list_foreach(start_sequencer,
		 (GFunc) g_object_unref,
		 NULL);
  
  g_object_ref(sequencer);
  ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				   g_list_append(start_sequencer,
						 sequencer));    

  sequencer_thread = (AgsThread *) ags_sequencer_thread_new(sequencer);
  sequencer_editor->sequencer_thread = (GObject *) sequencer_thread;
  
  ags_thread_add_child_extended(main_loop,
				sequencer_thread,
				TRUE, TRUE);

  /* unref */
  g_object_unref(main_loop);
}

void
ags_sequencer_editor_remove_sequencer(AgsSequencerEditor *sequencer_editor,
				      GObject *sequencer)
{
  AgsApplicationContext *application_context;

  if(AGS_IS_JACK_MIDIIN(sequencer)){
    return;
  }
  
  application_context = ags_application_context_get_instance();
    
  if(sequencer == sequencer_editor->sequencer){
    sequencer_editor->sequencer = NULL;
  }
}

void
ags_sequencer_editor_load_jack_card(AgsSequencerEditor *sequencer_editor)
{
  AgsJackMidiin *jack_midiin;

  AgsApplicationContext *application_context;

  GList *start_sound_server, *sound_server;
  GList *start_sequencer, *sequencer;
  GList *card_id;

  application_context = ags_application_context_get_instance();

  /* create sequencer */
  sound_server =
    start_sound_server = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));

  if((sound_server = ags_list_util_find_type(sound_server,
					     AGS_TYPE_JACK_SERVER)) == NULL){
    g_warning("sound server not found");

    g_list_free_full(sound_server,
		     g_object_unref);
    
    return;
  }

  sequencer = 
    start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
  jack_midiin = NULL;
  
  while(sequencer != NULL){
    if(AGS_IS_JACK_MIDIIN(sequencer->data)){
      jack_midiin = sequencer->data;
      break;
    }
    
    sequencer = sequencer->next;
  }

  card_id = NULL;
  ags_sequencer_list_cards(AGS_SEQUENCER(jack_midiin),
			   &card_id, NULL);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card))));

  while(card_id != NULL){
    if(card_id->data != NULL){
      gtk_combo_box_text_append_text(sequencer_editor->card,
				     card_id->data);
    }
    
    card_id = card_id->next;
  }

  g_list_free_full(start_sound_server,
		   g_object_unref);
  g_list_free_full(start_sequencer,
		   g_object_unref);
}

void
ags_sequencer_editor_load_alsa_card(AgsSequencerEditor *sequencer_editor)
{
  AgsAlsaMidiin *alsa_midiin;

  GList *card_id;
  
  /*  */
  alsa_midiin = g_object_new(AGS_TYPE_ALSA_MIDIIN,
			     NULL);

  card_id = NULL;
  ags_sequencer_list_cards(AGS_SEQUENCER(alsa_midiin),
			   &card_id, NULL);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card))));

  while(card_id != NULL){
    if(card_id->data != NULL){
      gtk_combo_box_text_append_text(sequencer_editor->card,
				     card_id->data);
    }
    
    card_id = card_id->next;
  }

  /* remove previous */
  if(sequencer_editor->sequencer != NULL){
    ags_sequencer_editor_remove_sequencer(sequencer_editor,
					  (GObject *) sequencer_editor->sequencer);
  }

  /* add new */
  ags_sequencer_editor_add_sequencer(sequencer_editor,
				     (GObject *) alsa_midiin);
}

void
ags_sequencer_editor_load_oss_card(AgsSequencerEditor *sequencer_editor)
{
  AgsOssMidiin *oss_midiin;

  GList *card_id;
    
  /*  */  
  oss_midiin = g_object_new(AGS_TYPE_OSS_MIDIIN,
			    NULL);

  card_id = NULL;
  ags_sequencer_list_cards(AGS_SEQUENCER(oss_midiin),
			   &card_id, NULL);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card))));

  while(card_id != NULL){
    if(card_id->data != NULL){
      gtk_combo_box_text_append_text(sequencer_editor->card,
				     card_id->data);
    }
    
    card_id = card_id->next;
  }

  /* remove previous */
  if(sequencer_editor->sequencer != NULL){
    ags_sequencer_editor_remove_sequencer(sequencer_editor,
					  (GObject *) sequencer_editor->sequencer);
  }

  /* add new */
  ags_sequencer_editor_add_sequencer(sequencer_editor,
				     (GObject *) oss_midiin);
}

/**
 * ags_sequencer_editor_new:
 *
 * Create a new instance of #AgsSequencerEditor
 *
 * Returns: the new #AgsSequencerEditor
 *
 * Since: 3.0.0
 */
AgsSequencerEditor*
ags_sequencer_editor_new()
{
  AgsSequencerEditor *sequencer_editor;

  sequencer_editor = (AgsSequencerEditor *) g_object_new(AGS_TYPE_SEQUENCER_EDITOR,
							 NULL);
  
  return(sequencer_editor);
}
