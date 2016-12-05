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

#include <ags/X/ags_sequencer_editor.h>
#include <ags/X/ags_sequencer_editor_callbacks.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_midiin.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/audio/thread/ags_sequencer_thread.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/X/ags_midi_preferences.h>

#include <ags/config.h>

void ags_sequencer_editor_class_init(AgsSequencerEditorClass *sequencer_editor);
void ags_sequencer_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sequencer_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_sequencer_editor_init(AgsSequencerEditor *sequencer_editor);
void ags_sequencer_editor_connect(AgsConnectable *connectable);
void ags_sequencer_editor_disconnect(AgsConnectable *connectable);
void ags_sequencer_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_sequencer_editor_apply(AgsApplicable *applicable);
void ags_sequencer_editor_reset(AgsApplicable *applicable);
static void ags_sequencer_editor_finalize(GObject *gobject);

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
  static GType ags_type_sequencer_editor = 0;

  if(!ags_type_sequencer_editor){
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
    
    ags_type_sequencer_editor = g_type_register_static(GTK_TYPE_VBOX,
						       "AgsSequencerEditor\0", &ags_sequencer_editor_info,
						       0);
    
    g_type_add_interface_static(ags_type_sequencer_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_sequencer_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_sequencer_editor);
}

void
ags_sequencer_editor_class_init(AgsSequencerEditorClass *sequencer_editor)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_sequencer_editor_parent_class = g_type_class_peek_parent(sequencer_editor);

  /* GtkObjectClass */
  gobject = (GObjectClass *) sequencer_editor;

  gobject->finalize = ags_sequencer_editor_finalize;
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
  GtkTable *table;
  GtkLabel *label;

  sequencer_editor->flags = 0;
  
  sequencer_editor->sequencer = NULL;
  sequencer_editor->sequencer_thread = NULL;
  
  table = (GtkTable *) gtk_table_new(3, 8, FALSE);
  gtk_box_pack_start(GTK_BOX(sequencer_editor),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     2);

  /* backend */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "backend\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  sequencer_editor->backend = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(sequencer_editor->backend),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  gtk_combo_box_text_append_text(sequencer_editor->backend,
				 "jack\0");
  
#ifdef AGS_WITH_ALSA
  gtk_combo_box_text_append_text(sequencer_editor->backend,
				 "alsa\0");
#endif
  
  gtk_combo_box_text_append_text(sequencer_editor->backend,
				 "oss\0");
  gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			   0);
  
  /* sound card */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "MIDI card\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  sequencer_editor->card = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(sequencer_editor->card),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* jack */
  sequencer_editor->jack_hbox = NULL;
  sequencer_editor->add_jack = NULL;
  sequencer_editor->remove_jack = NULL;

  sequencer_editor->jack_hbox = (GtkHBox *) gtk_hbox_new(FALSE,
							 0);
  gtk_table_attach(table,
		   GTK_WIDGET(sequencer_editor->jack_hbox),
		   2, 3,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  sequencer_editor->add_jack = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) sequencer_editor->jack_hbox,
		     (GtkWidget *) sequencer_editor->add_jack,
		     FALSE, FALSE,
		     0);
  
  sequencer_editor->remove_jack = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) sequencer_editor->jack_hbox,
		     (GtkWidget *) sequencer_editor->remove_jack,
		     FALSE, FALSE,
		     0);
  
  /*  */
  sequencer_editor->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_table_attach(table,
		   GTK_WIDGET(sequencer_editor->remove),
		   2, 3,
		   7, 8,
		   GTK_FILL, GTK_FILL,
		   0, 0);
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
  g_signal_connect(G_OBJECT(sequencer_editor->backend), "changed\0",
		   G_CALLBACK(ags_sequencer_editor_backend_changed_callback), sequencer_editor);

  g_signal_connect(G_OBJECT(sequencer_editor->card), "changed\0",
		   G_CALLBACK(ags_sequencer_editor_card_changed_callback), sequencer_editor);

  /* add / remove jack */
  g_signal_connect(G_OBJECT(sequencer_editor->add_jack), "clicked\0",
		   G_CALLBACK(ags_sequencer_editor_add_jack_callback), sequencer_editor);

  g_signal_connect(G_OBJECT(sequencer_editor->remove_jack), "clicked\0",
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
		      "changed\0",
		      G_CALLBACK(ags_sequencer_editor_backend_changed_callback),
		      sequencer_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(sequencer_editor->card),
		      "changed\0",
		      G_CALLBACK(ags_sequencer_editor_card_changed_callback),
		      sequencer_editor,
		      NULL);

  /* add / remove jack */
  g_object_disconnect(G_OBJECT(sequencer_editor->add_jack),
		      "clicked\0",
		      G_CALLBACK(ags_sequencer_editor_add_jack_callback),
		      sequencer_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(sequencer_editor->remove_jack),
		      "clicked\0",
		      G_CALLBACK(ags_sequencer_editor_remove_jack_callback),
		      sequencer_editor,
		      NULL);
}

static void
ags_sequencer_editor_finalize(GObject *gobject)
{
  //TODO:JK: implement me
  
  G_OBJECT_CLASS(ags_sequencer_editor_parent_class)->finalize(gobject);
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

  GList *tasks;
  GList *list;	

  gchar *sequencer_group;
  gchar *backend;
  char *device, *str;

  gint nth;
  gboolean use_jack, use_alsa, use_oss;
  
  GValue value =  {0,};

  pthread_mutex_t *application_mutex;

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
  
  sequencer_group = g_strdup_printf("%s-%d\0",
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
		       "backend\0",
		       backend);

  if(backend != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "jack\0",
			    5)){
      use_jack = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
			    "alsa\0",
			    5)){
      use_alsa = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
				  "oss\0",
				  4)){
      use_oss = TRUE;
    }
  }

  /* device */
  model = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card)));
  
  if(gtk_combo_box_get_active_iter(GTK_COMBO_BOX(sequencer_editor->card),
				   &current)){
    gtk_tree_model_get_value(model,
			     &current,
			     0,
			     &value);
    device = g_value_get_string(&value);
  }else{
    device = NULL;
  }

  /* handle */
  g_message("%s\0", device);

  if(use_jack){
    ags_config_set_value(config,
			 sequencer_group,
			 "device\0",
			 device);
  }else if(use_alsa){
    ags_config_set_value(config,
			 sequencer_group,
			 "device\0",
			 device);
  }else if(use_oss){
    ags_config_set_value(config,
			 sequencer_group,
			 "device\0",
			 device);
  }
}

void
ags_sequencer_editor_reset(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsPreferences *preferences;
  AgsSequencerEditor *sequencer_editor;

  GObject *sequencer;

  GList *card_id, *card_id_start, *card_name, *card_name_start;

  gchar *backend, *device, *str, *tmp;

  guint nth;
  gboolean use_alsa;
  gboolean found_card;
  
  GError *error;
  
  sequencer_editor = AGS_SEQUENCER_EDITOR(applicable);
  sequencer = sequencer_editor->sequencer;
  
  /* refresh */
  use_alsa = FALSE;

  backend = NULL;
  
  if(AGS_IS_JACK_MIDIIN(sequencer)){
    backend = "jack\0";
  }else if(AGS_IS_MIDIIN(sequencer)){
    if((AGS_MIDIIN_ALSA & (AGS_MIDIIN(sequencer)->flags)) != 0){
      backend = "alsa\0";
    }else if((AGS_MIDIIN_OSS & (AGS_MIDIIN(sequencer)->flags)) != 0){
      backend = "oss\0";
    }
  }

  if(backend != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "jack\0",
			    5)){
      gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			       0);
      
      ags_sequencer_editor_load_jack_card(sequencer_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "alsa\0",
				  5)){
      use_alsa = TRUE;

#ifdef AGS_WITH_ALSA
      gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			       1);
#endif
      
      ags_sequencer_editor_load_alsa_card(sequencer_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "oss\0",
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
    
    if(use_alsa){
      tmp = g_strdup_printf("%s,0\0",
			    card_id->data);
    }
    
    if(!g_ascii_strcasecmp(tmp,
			   device)){
      found_card = TRUE;
    }

    gtk_combo_box_text_append_text(sequencer_editor->card,
				   tmp);

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
ags_sequencer_editor_add_jack(AgsSequencerEditor *sequencer_editor,
			      gchar *device)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsJackServer *jack_server;
  AgsJackMidiin *jack_midiin;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsThread *sequencer_thread;

  AgsApplicationContext *application_context;

  GList *distributed_manager;
  GList *card_name, *card_uri;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(sequencer_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);

  application_context = (AgsApplicationContext *) window->application_context;
  application_mutex = window->application_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  
  /* create sequencer */
  pthread_mutex_lock(application_mutex);

  distributed_manager = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

  if(distributed_manager != NULL){
    jack_server = AGS_JACK_SERVER(distributed_manager->data);
  }else{
    g_warning("distributed manager not found\0");

    pthread_mutex_unlock(application_mutex);
    
    return;
  }
  
  jack_midiin = (AgsJackMidiin *) ags_distributed_manager_register_sequencer(AGS_DISTRIBUTED_MANAGER(jack_server),
									     FALSE);

  pthread_mutex_unlock(application_mutex);

  if(jack_midiin == NULL){
    return;
  }

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) jack_midiin);
  

  /* add new */
  main_loop = (AgsThread *) application_context->main_loop;
  
  sequencer_editor->sequencer = jack_midiin;

  ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				   g_list_append(ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context)),
						 jack_midiin));

  pthread_mutex_unlock(application_mutex);  
    
  g_object_ref(jack_midiin);

  sequencer_thread = (AgsThread *) ags_sequencer_thread_new(jack_midiin);
  sequencer_editor->sequencer_thread = (GObject *) sequencer_thread;
  
  ags_thread_add_child_extended(main_loop,
				sequencer_thread,
				TRUE, TRUE);

  /*  */
  pthread_mutex_lock(mutex);

  card_name = NULL;
  card_uri = NULL;
  ags_sequencer_list_cards(AGS_SEQUENCER(jack_midiin),
			   &card_uri, &card_name);

  pthread_mutex_unlock(mutex);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card))));

  while(card_uri != NULL){
    gtk_combo_box_text_append_text(sequencer_editor->card,
    				   card_uri->data);
    
    card_uri = card_uri->next;
  }
}

void
ags_sequencer_editor_remove_jack(AgsSequencerEditor *sequencer_editor,
				 gchar *device)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsJackMidiin *jack_midiin;

  AgsApplicationContext *application_context;
  AgsThread *main_loop;

  GList *distributed_manager;
  GList *sequencer;
  GList *card_id;

  pthread_mutex_t *application_mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(sequencer_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = (AgsApplicationContext *) window->application_context;

  application_mutex = window->application_mutex;
  
  /* create sequencer */
  pthread_mutex_lock(application_mutex);

  main_loop = application_context->main_loop;
  distributed_manager = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

  if(distributed_manager == NULL){
    g_warning("distributed manager not found\0");

    pthread_mutex_unlock(application_mutex);
    
    return;
  }

  sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
  jack_midiin = NULL;
  
  while(sequencer != NULL){
    if(AGS_IS_JACK_MIDIIN(sequencer->data) &&
       !g_ascii_strcasecmp(ags_sequencer_get_device(AGS_SEQUENCER(sequencer->data)),
			   device)){
      jack_midiin = sequencer->data;
      break;
    }
    
    sequencer = sequencer->next;
  }

  pthread_mutex_unlock(application_mutex);

  if(jack_midiin == NULL){
    return;
  }
  
  /*  */
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card))));
  gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			   -1);

  if(distributed_manager != NULL){
    ags_distributed_manager_unregister_sequencer(AGS_DISTRIBUTED_MANAGER(distributed_manager->data),
						 sequencer);
  }

  /* remove */
  if(jack_midiin == sequencer_editor->sequencer){
    sequencer_editor->sequencer = NULL;
  }
  
  if(jack_midiin != NULL){
    ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				     g_list_remove(ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context)),
						   jack_midiin));
    g_object_unref(jack_midiin);
  }
  
  if(sequencer_editor->sequencer_thread != NULL){
    ags_thread_stop((AgsThread *) sequencer_editor->sequencer_thread);

    ags_thread_remove_child(main_loop,
			    (AgsThread *) sequencer_editor->sequencer_thread);
    
    //    g_object_unref(sequencer_editor->sequencer_thread);
    
    sequencer_editor->sequencer_thread = NULL;
  }
}

void
ags_sequencer_editor_add_sequencer(AgsSequencerEditor *sequencer_editor,
				   GObject *sequencer)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsThread *main_loop;
  AgsThread *sequencer_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;

  if(sequencer == NULL ||
     AGS_IS_JACK_MIDIIN(sequencer)){
    return;
  }
  
  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(sequencer_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = (AgsApplicationContext *) window->application_context;
  application_mutex = window->application_mutex;

  if(AGS_IS_MIDIIN(sequencer)){
    if((AGS_MIDIIN_ALSA & (AGS_MIDIIN(sequencer)->flags)) != 0){
      ags_sequencer_set_device(AGS_SEQUENCER(sequencer),
			       "hw:0,0\0");
    }else if((AGS_MIDIIN_OSS & (AGS_MIDIIN(sequencer)->flags)) != 0){
      ags_sequencer_set_device(AGS_SEQUENCER(sequencer),
			       "/dev/dsp0\0");
    }else{
      g_warning("unknown sequencer implementation\0");
    }
  }else{
    g_warning("unknown sequencer implementation\0");
  }
  
  /*  */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  if(g_list_find(ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context)),
		 sequencer) != NULL){
    pthread_mutex_unlock(application_mutex);  
  
    return;
  }
  
  sequencer_editor->sequencer = sequencer;

  ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				   g_list_append(ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context)),
						 sequencer));

  pthread_mutex_unlock(application_mutex);  
    
  g_object_ref(sequencer);

  sequencer_thread = (AgsThread *) ags_sequencer_thread_new(sequencer);
  sequencer_editor->sequencer_thread = (GObject *) sequencer_thread;
  
  ags_thread_add_child_extended(main_loop,
				sequencer_thread,
				TRUE, TRUE);
}

void
ags_sequencer_editor_remove_sequencer(AgsSequencerEditor *sequencer_editor,
				      GObject *sequencer)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsThread *main_loop;
  AgsThread *sequencer_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;

  if(AGS_IS_JACK_MIDIIN(sequencer)){
    return;
  }
  
  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(sequencer_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = (AgsApplicationContext *) window->application_context;  

  application_mutex = window->application_mutex;
  
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;
    
  if(sequencer == sequencer_editor->sequencer){
    sequencer_editor->sequencer = NULL;
  }
  
  if(sequencer != NULL){
    ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				     g_list_remove(ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context)),
						   sequencer));
    g_object_unref(sequencer);
  }
  
  if(sequencer_editor->sequencer_thread != NULL){
    ags_thread_stop((AgsThread *) sequencer_editor->sequencer_thread);

    ags_thread_remove_child(main_loop,
			    (AgsThread *) sequencer_editor->sequencer_thread);
    
    //    g_object_unref(sequencer_editor->sequencer_thread);
    
    sequencer_editor->sequencer_thread = NULL;
  }

  pthread_mutex_unlock(application_mutex);
}

void
ags_sequencer_editor_load_jack_card(AgsSequencerEditor *sequencer_editor)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsJackMidiin *jack_midiin;

  AgsApplicationContext *application_context;

  GList *distributed_manager;
  GList *sequencer;
  GList *card_id;

  pthread_mutex_t *application_mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(sequencer_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = (AgsApplicationContext *) window->application_context;

  application_mutex = window->application_mutex;

  /* create sequencer */
  pthread_mutex_lock(application_mutex);

  distributed_manager = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

  if(distributed_manager == NULL){
    g_warning("distributed manager not found\0");

    pthread_mutex_unlock(application_mutex);

    return;
  }

  sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
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
    gtk_combo_box_text_append_text(sequencer_editor->card,
				   card_id->data);

    
    card_id = card_id->next;
  }

  /*  */
  pthread_mutex_unlock(application_mutex);
}

void
ags_sequencer_editor_load_alsa_card(AgsSequencerEditor *sequencer_editor)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsMidiin *midiin;

  AgsApplicationContext *application_context;

  GList *list;
  GList *card_id;

  pthread_mutex_t *application_mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(sequencer_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = (AgsApplicationContext *) window->application_context;

  application_mutex = window->application_mutex;
  
  /*  */
  pthread_mutex_lock(application_mutex);

  midiin = g_object_new(AGS_TYPE_MIDIIN,
			NULL);
  midiin->flags &= (~AGS_MIDIIN_OSS);
  midiin->flags |= AGS_MIDIIN_ALSA;
  g_object_set(midiin,
	       "application-context\0", application_context,
	       NULL);

  card_id = NULL;
  ags_sequencer_list_cards(AGS_SEQUENCER(midiin),
			   &card_id, NULL);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card))));

  while(card_id != NULL){
    gtk_combo_box_text_append_text(sequencer_editor->card,
				   card_id->data);

    
    card_id = card_id->next;
  }

  /* remove previous */
  if(sequencer_editor->sequencer != NULL){
    ags_sequencer_editor_remove_sequencer(sequencer_editor,
					  (GObject *) sequencer_editor->sequencer);
  }

  /* add new */
  ags_sequencer_editor_add_sequencer(sequencer_editor,
				     (GObject *) midiin);

  pthread_mutex_unlock(application_mutex);
}

void
ags_sequencer_editor_load_oss_card(AgsSequencerEditor *sequencer_editor)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsMidiin *midiin;

  AgsApplicationContext *application_context;

  GList *list;
  GList *card_id;

  pthread_mutex_t *application_mutex;
  
  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(sequencer_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = (AgsApplicationContext *) window->application_context;

  application_mutex = window->application_mutex;
  
  /*  */  
  pthread_mutex_lock(application_mutex);

  midiin = g_object_new(AGS_TYPE_MIDIIN,
			NULL);
  midiin->flags &= (~AGS_MIDIIN_ALSA);
  midiin->flags |= AGS_MIDIIN_OSS;
  g_object_set(midiin,
	       "application-context\0", application_context,
	       NULL);

  card_id = NULL;
  ags_sequencer_list_cards(AGS_SEQUENCER(midiin),
			   &card_id, NULL);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card))));

  while(card_id != NULL){
    gtk_combo_box_text_append_text(sequencer_editor->card,
				   card_id->data);

    
    card_id = card_id->next;
  }

  /* remove previous */
  if(sequencer_editor->sequencer != NULL){
    ags_sequencer_editor_remove_sequencer(sequencer_editor,
					  (GObject *) sequencer_editor->sequencer);
  }

  /* add new */
  ags_sequencer_editor_add_sequencer(sequencer_editor,
				     (GObject *) midiin);

  pthread_mutex_unlock(application_mutex);
}

/**
 * ags_sequencer_editor_new:
 *
 * Creates an #AgsSequencerEditor
 *
 * Returns: a new #AgsSequencerEditor
 *
 * Since: 0.7.111
 */
AgsSequencerEditor*
ags_sequencer_editor_new()
{
  AgsSequencerEditor *sequencer_editor;

  sequencer_editor = (AgsSequencerEditor *) g_object_new(AGS_TYPE_SEQUENCER_EDITOR,
							 NULL);
  
  return(sequencer_editor);
}
