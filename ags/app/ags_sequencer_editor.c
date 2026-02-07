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

#include <ags/app/ags_sequencer_editor.h>
#include <ags/app/ags_sequencer_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_preferences.h>

#include <ags/app/ags_midi_preferences.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_sequencer_editor_class_init(AgsSequencerEditorClass *sequencer_editor);
void ags_sequencer_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sequencer_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_sequencer_editor_init(AgsSequencerEditor *sequencer_editor);

gboolean ags_sequencer_editor_is_connected(AgsConnectable *connectable);
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
 * @include: ags/app/ags_sequencer_editor.h
 *
 * #AgsSequencerEditor enables you to make preferences of sequencer.
 */

static gpointer ags_sequencer_editor_parent_class = NULL;

GType
ags_sequencer_editor_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
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

    g_once_init_leave(&g_define_type_id__static, ags_type_sequencer_editor);
  }

  return(g_define_type_id__static);
}

void
ags_sequencer_editor_class_init(AgsSequencerEditorClass *sequencer_editor)
{
  ags_sequencer_editor_parent_class = g_type_class_peek_parent(sequencer_editor);
}

void
ags_sequencer_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_sequencer_editor_is_connected;  
  connectable->connect = ags_sequencer_editor_connect;
  connectable->disconnect = ags_sequencer_editor_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
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

  gtk_box_set_spacing((GtkBox *) sequencer_editor,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  sequencer_editor->flags = 0;
  
  sequencer_editor->sequencer = NULL;
  sequencer_editor->sequencer_thread = NULL;
  
  grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_vexpand((GtkWidget *) grid,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) grid,
			 FALSE);

  gtk_widget_set_halign((GtkWidget *) grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) grid,
			GTK_ALIGN_START);

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append((GtkBox *) sequencer_editor,
		 (GtkWidget *) grid);

  /* backend */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("backend"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  sequencer_editor->backend = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_widget_set_valign((GtkWidget *) sequencer_editor->backend,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) sequencer_editor->backend,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) sequencer_editor->backend,
		  1, 0,
		  1, 1);

#ifdef AGS_WITH_CORE_AUDIO
  gtk_combo_box_text_append_text(sequencer_editor->backend,
				 "core-midi");
#endif
  
#ifdef AGS_WITH_ALSA
  gtk_combo_box_text_append_text(sequencer_editor->backend,
				 "alsa");
#endif
  
#ifdef AGS_WITH_OSS
  gtk_combo_box_text_append_text(sequencer_editor->backend,
				 "oss");
#endif

#ifdef AGS_WITH_JACK
  gtk_combo_box_text_append_text(sequencer_editor->backend,
				 "jack");
#endif

  gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			   0);
  
  /* MIDI card */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("MIDI card"),
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  sequencer_editor->card = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_widget_set_valign((GtkWidget *) sequencer_editor->card,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) sequencer_editor->card,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) sequencer_editor->card,
		  1, 1,
		  1, 1);

  /* source */
  sequencer_editor->source_hbox = NULL;
  sequencer_editor->add_source = NULL;
  sequencer_editor->remove_source = NULL;

  /*  */
  sequencer_editor->remove = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Remove"));

  gtk_widget_set_valign((GtkWidget *) sequencer_editor->remove,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) sequencer_editor->remove,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) sequencer_editor->remove,
		  2, 7,
		  1, 1);
}

gboolean
ags_sequencer_editor_is_connected(AgsConnectable *connectable)
{
  AgsSequencerEditor *sequencer_editor;

  gboolean retval;

  sequencer_editor = AGS_SEQUENCER_EDITOR(connectable);

  retval = ((AGS_CONNECTABLE_CONNECTED & (sequencer_editor->connectable_flags)) != 0) ? TRUE: FALSE;

  return(retval);
}

void
ags_sequencer_editor_connect(AgsConnectable *connectable)
{
  AgsSequencerEditor *sequencer_editor;

  sequencer_editor = AGS_SEQUENCER_EDITOR(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  sequencer_editor->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  /* backend and card */
  g_signal_connect(G_OBJECT(sequencer_editor->backend), "changed",
		   G_CALLBACK(ags_sequencer_editor_backend_changed_callback), sequencer_editor);

  g_signal_connect(G_OBJECT(sequencer_editor->card), "changed",
		   G_CALLBACK(ags_sequencer_editor_card_changed_callback), sequencer_editor);
}

void
ags_sequencer_editor_disconnect(AgsConnectable *connectable)
{
  AgsSequencerEditor *sequencer_editor;

  sequencer_editor = AGS_SEQUENCER_EDITOR(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  sequencer_editor->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

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

  GList *start_list;	

  gchar *sequencer_group;
  gchar *backend;
  char *device;

  gint nth;
  gboolean use_alsa, use_oss, use_core_midi, use_jack;
  
  GValue value =  {0,};

  sequencer_editor = AGS_SEQUENCER_EDITOR(applicable);
  midi_preferences = (AgsMidiPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(sequencer_editor),
								    AGS_TYPE_MIDI_PREFERENCES);

  config = ags_config_get_instance();

  start_list = ags_midi_preferences_get_sequencer_editor(midi_preferences);

  nth = g_list_index(start_list,
		     sequencer_editor);

  g_list_free(start_list);

  if(nth < 0){    
    return;
  }
  
  sequencer_group = g_strdup_printf("%s-%d",
				    AGS_CONFIG_SEQUENCER,
				    nth);
  
  /* backend */
  use_alsa = FALSE;
  use_oss = FALSE;
  use_core_midi = FALSE;
  use_jack = TRUE;

  backend = gtk_combo_box_text_get_active_text(sequencer_editor->backend);
  ags_config_set_value(config,
		       sequencer_group,
		       "backend",
		       backend);

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
      use_core_midi = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
				  "jack",
				  5)){
      use_jack = TRUE;
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

  if(use_alsa){
    ags_config_set_value(config,
			 sequencer_group,
			 "device",
			 device);
  }else if(use_oss){
    ags_config_set_value(config,
			 sequencer_group,
			 "device",
			 device);
  }else if(use_core_midi){
    ags_config_set_value(config,
			 sequencer_group,
			 "device",
			 device);
  }else if(use_jack){
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
  
  if(AGS_IS_ALSA_MIDIIN(sequencer)){
    backend = "alsa";
  }else if(AGS_IS_OSS_MIDIIN(sequencer)){
    backend = "oss";
  }else if(AGS_IS_CORE_AUDIO_MIDIIN(sequencer)){
    backend = "core-midi";
  }else if(AGS_IS_JACK_MIDIIN(sequencer)){
    backend = "jack";
  }

  if(backend != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "alsa",
			    5)){
      use_alsa = TRUE;

#ifdef AGS_WITH_ALSA
      gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			       0);
#endif
      
      //      ags_sequencer_editor_load_alsa_card(sequencer_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "oss",
				  4)){
#ifdef AGS_WITH_OSS
      gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			       0);
#endif
      
      //      ags_sequencer_editor_load_oss_card(sequencer_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "core-midi",
				  10)){
#ifdef AGS_WITH_CORE_AUDIO
      gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			       0);
#endif
      
      //      ags_sequencer_editor_load_core_midi_card(sequencer_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "jack",
				  5)){
#ifdef AGS_WITH_JACK
      gtk_combo_box_set_active(GTK_COMBO_BOX(sequencer_editor->backend),
			       1);
#endif
      
      //      ags_sequencer_editor_load_jack_card(sequencer_editor);
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
ags_sequencer_editor_load_core_midi_card(AgsSequencerEditor *sequencer_editor)
{
  //empty
}

void
ags_sequencer_editor_load_jack_card(AgsSequencerEditor *sequencer_editor)
{
  //empty
}

void
ags_sequencer_editor_load_alsa_card(AgsSequencerEditor *sequencer_editor)
{
  //empty
}

void
ags_sequencer_editor_load_oss_card(AgsSequencerEditor *sequencer_editor)
{
  //empty
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
