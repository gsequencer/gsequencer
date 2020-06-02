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

#include <ags/X/machine/ags_sf2_synth.h>
#include <ags/X/machine/ags_sf2_synth_callbacks.h>

#include <ags/i18n.h>

void ags_sf2_synth_class_init(AgsSF2SynthClass *sf2_synth);
void ags_sf2_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sf2_synth_init(AgsSF2Synth *sf2_synth);
void ags_sf2_synth_finalize(GObject *gobject);

void ags_sf2_synth_connect(AgsConnectable *connectable);
void ags_sf2_synth_disconnect(AgsConnectable *connectable);

void ags_sf2_synth_show(GtkWidget *widget);

void ags_sf2_synth_resize_audio_channels(AgsMachine *machine,
					 guint audio_channels, guint audio_channels_old,
					 gpointer data);
void ags_sf2_synth_resize_pads(AgsMachine *machine, GType channel_type,
			       guint pads, guint pads_old,
			       gpointer data);

void ags_sf2_synth_map_recall(AgsMachine *machine);

void ags_sf2_synth_input_map_recall(AgsSF2Synth *sf2_synth,
				    guint audio_channel_start,
				    guint input_pad_start);
void ags_sf2_synth_output_map_recall(AgsSF2Synth *sf2_synth,
				     guint audio_channel_start,
				     guint output_pad_start);

/**
 * SECTION:ags_sf2_synth
 * @short_description: sf2_synth
 * @title: AgsSF2Synth
 * @section_id:
 * @include: ags/X/machine/ags_sf2_synth.h
 *
 * The #AgsSF2Synth is a composite widget to act as sf2_synth.
 */

static gpointer ags_sf2_synth_parent_class = NULL;
static AgsConnectableInterface *ags_sf2_synth_parent_connectable_interface;

GHashTable *ags_sf2_synth_sf2_loader_completed = NULL;

GType
ags_sf2_synth_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sf2_synth = 0;

    static const GTypeInfo ags_sf2_synth_info = {
      sizeof(AgsSF2SynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sf2_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSF2Synth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sf2_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sf2_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_sf2_synth = g_type_register_static(AGS_TYPE_MACHINE,
						"AgsSF2Synth", &ags_sf2_synth_info,
						0);
    
    g_type_add_interface_static(ags_type_sf2_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sf2_synth);
  }

  return g_define_type_id__volatile;
}

void
ags_sf2_synth_class_init(AgsSF2SynthClass *sf2_synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_sf2_synth_parent_class = g_type_class_peek_parent(sf2_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) sf2_synth;

  gobject->finalize = ags_sf2_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) sf2_synth;

  machine->map_recall = ags_sf2_synth_map_recall;
}

void
ags_sf2_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_sf2_synth_connectable_parent_interface;

  ags_sf2_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_sf2_synth_connect;
  connectable->disconnect = ags_sf2_synth_disconnect;
}

void
ags_sf2_synth_init(AgsSF2Synth *sf2_synth)
{
  GtkHBox *sf2_hbox;
  GtkHBox *sf2_file_hbox;
  GtkHBox *sf2_preset_hbox;
  GtkHBox *hbox;
  GtkTreeView *sf2_bank_tree_view;
  GtkTreeView *sf2_program_tree_view;
  GtkTreeViewColumn *sf2_bank_column;
  GtkTreeViewColumn *sf2_program_column;
  GtkTreeViewColumn *sf2_preset_column;
  GtkScrolledWindow *scrolled_window;
  GtkLabel *label;
    
  GtkCellRenderer *sf2_bank_renderer;
  GtkCellRenderer *sf2_program_renderer;
  GtkCellRenderer *sf2_preset_renderer;
  
  GtkListStore *sf2_bank;
  GtkListStore *sf2_program;
  
  AgsAudio *audio;
  
  g_signal_connect_after((GObject *) sf2_synth, "parent_set",
			 G_CALLBACK(ags_sf2_synth_parent_set_callback), (gpointer) sf2_synth);

  audio = AGS_MACHINE(sf2_synth)->audio;

  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION));
  ags_audio_set_behaviour_flags(audio, (AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING |
					AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT));

  g_object_set(audio,
	       "min-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       "max-input-pads", 128,
	       "audio-start-mapping", 0,
	       "audio-end-mapping", 128,
	       "midi-start-mapping", 0,
	       "midi-end-mapping", 128,
	       NULL);
  
  AGS_MACHINE(sf2_synth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				    AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(sf2_synth)->file_input_flags |= AGS_MACHINE_ACCEPT_SOUNDFONT2;

  /* context menu */
  ags_machine_popup_add_connection_options((AgsMachine *) sf2_synth,
  					   (AGS_MACHINE_POPUP_MIDI_DIALOG));

  /* audio resize */
  g_signal_connect_after(G_OBJECT(sf2_synth), "resize-audio-channels",
			 G_CALLBACK(ags_sf2_synth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(sf2_synth), "resize-pads",
			 G_CALLBACK(ags_sf2_synth_resize_pads), NULL);

  /* flags */
  sf2_synth->flags = 0;

  /* mapped IO */
  sf2_synth->mapped_input_pad = 0;
  sf2_synth->mapped_output_pad = 0;

  sf2_synth->playback_play_container = ags_recall_container_new();
  sf2_synth->playback_recall_container = ags_recall_container_new();

  sf2_synth->notation_play_container = ags_recall_container_new();
  sf2_synth->notation_recall_container = ags_recall_container_new();

  sf2_synth->envelope_play_container = ags_recall_container_new();
  sf2_synth->envelope_recall_container = ags_recall_container_new();

  sf2_synth->buffer_play_container = ags_recall_container_new();
  sf2_synth->buffer_recall_container = ags_recall_container_new();

  /* context menu */
  ags_machine_popup_add_edit_options((AgsMachine *) sf2_synth,
				     (AGS_MACHINE_POPUP_ENVELOPE));
  
  /* name and xml type */
  sf2_synth->name = NULL;
  sf2_synth->xml_type = "ags-sf2-synth";

  /* SF2 */
  sf2_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
			 0);
  gtk_container_add((GtkContainer *) (gtk_bin_get_child((GtkBin *) sf2_synth)),
		    (GtkWidget *) sf2_hbox);

  /* file */
  sf2_file_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
			      0);  
  gtk_box_pack_start(GTK_BOX(sf2_hbox),
		     GTK_WIDGET(sf2_file_hbox),
		     FALSE, FALSE,
		     0);

  sf2_synth->filename = (GtkEntry *) gtk_entry_new();
  gtk_widget_set_valign(sf2_synth->filename,
			GTK_ALIGN_START);
  gtk_box_pack_start((GtkBox *) sf2_file_hbox,
		     (GtkWidget *) sf2_synth->filename,
		     FALSE, FALSE,
		     0);
  
  sf2_synth->open = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OPEN);
  gtk_widget_set_valign(sf2_synth->open,
			GTK_ALIGN_START);
  gtk_box_pack_start((GtkBox *) sf2_file_hbox,
		     (GtkWidget *) sf2_synth->open,
		     FALSE, FALSE,
		     0);

  /* preset - bank and program */
  sf2_preset_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_pack_start(GTK_BOX(sf2_hbox),
		     GTK_WIDGET(sf2_preset_hbox),
		     FALSE, FALSE,
		     0);

  scrolled_window = gtk_scrolled_window_new(NULL,
					    NULL);
  gtk_widget_set_size_request(scrolled_window,
			      AGS_SF2_SYNTH_BANK_WIDTH_REQUEST,
			      AGS_SF2_SYNTH_BANK_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_pack_start((GtkBox *) sf2_preset_hbox,
		     (GtkWidget *) scrolled_window,
		     FALSE, FALSE,
		     0);
  
  sf2_synth->bank = 
    sf2_bank_tree_view = gtk_tree_view_new();
  gtk_container_add(scrolled_window,
		    sf2_bank_tree_view);
    
  gtk_widget_set_size_request(sf2_bank_tree_view,
			      AGS_SF2_SYNTH_BANK_WIDTH_REQUEST,
			      AGS_SF2_SYNTH_BANK_HEIGHT_REQUEST);

  sf2_bank_renderer = gtk_cell_renderer_text_new();

  sf2_bank_column = gtk_tree_view_column_new_with_attributes(i18n("bank"),
							     sf2_bank_renderer,
							     NULL);
  gtk_tree_view_append_column(sf2_bank_tree_view,
			      sf2_bank_column);
  
  sf2_bank = gtk_list_store_new(1,
				G_TYPE_INT);

  gtk_tree_view_set_model(sf2_bank_tree_view,
			  GTK_TREE_MODEL(sf2_bank));  
  
  scrolled_window = gtk_scrolled_window_new(NULL,
					    NULL);
  gtk_widget_set_size_request(scrolled_window,
			      AGS_SF2_SYNTH_PROGRAM_WIDTH_REQUEST,
			      AGS_SF2_SYNTH_PROGRAM_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_pack_start((GtkBox *) sf2_preset_hbox,
		     (GtkWidget *) scrolled_window,
		     FALSE, FALSE,
		     0);

  sf2_synth->program = 
    sf2_program_tree_view = gtk_tree_view_new();
  gtk_container_add(scrolled_window,
		    sf2_program_tree_view);

  gtk_widget_set_size_request(sf2_program_tree_view,
			      AGS_SF2_SYNTH_PROGRAM_WIDTH_REQUEST,
			      AGS_SF2_SYNTH_PROGRAM_HEIGHT_REQUEST);

  sf2_program_renderer = gtk_cell_renderer_text_new();
  sf2_preset_renderer = gtk_cell_renderer_text_new();
  
  sf2_program_column = gtk_tree_view_column_new_with_attributes(i18n("program"),
								sf2_program_renderer,
								NULL);
  gtk_tree_view_append_column(sf2_program_tree_view,
			      sf2_program_column);

  sf2_preset_column = gtk_tree_view_column_new_with_attributes(i18n("preset"),
							       sf2_preset_renderer,
							       NULL);
  gtk_tree_view_append_column(sf2_program_tree_view,
			      sf2_preset_column);
  
  sf2_program = gtk_list_store_new(2,
				   G_TYPE_INT,
				   G_TYPE_STRING);

  gtk_tree_view_set_model(sf2_program_tree_view,
			  GTK_TREE_MODEL(sf2_program));

  /* lower */
  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
		     0);
  gtk_box_pack_start(GTK_BOX(sf2_hbox),
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("lower"));
  gtk_widget_set_valign(label,
			GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);
  
  sf2_synth->lower = gtk_spin_button_new_with_range(-72.0,
						    72.0,
						    1.0);
  gtk_widget_set_valign(sf2_synth->lower,
			GTK_ALIGN_START);
  gtk_spin_button_set_digits(sf2_synth->lower,
			     2);
  gtk_spin_button_set_value(sf2_synth->lower,
			    -48.0);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(sf2_synth->lower),
		     FALSE, FALSE,
		     0);  
  
  /* dialog */
  sf2_synth->open_dialog = NULL;

  /* SF2 loader */
  if(ags_sf2_synth_sf2_loader_completed == NULL){
    ags_sf2_synth_sf2_loader_completed = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							       NULL,
							       NULL);
  }

  g_hash_table_insert(ags_sf2_synth_sf2_loader_completed,
		      sf2_synth, ags_sf2_synth_sf2_loader_completed_timeout);
  g_timeout_add(1000 / 4, (GSourceFunc) ags_sf2_synth_sf2_loader_completed_timeout, (gpointer) sf2_synth);
}

void
ags_sf2_synth_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_sf2_synth_parent_class)->finalize(gobject);
}

void
ags_sf2_synth_connect(AgsConnectable *connectable)
{
  AgsSF2Synth *sf2_synth;
  
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_sf2_synth_parent_connectable_interface->connect(connectable);

  /* AgsSF2Synth */
  sf2_synth = AGS_SF2_SYNTH(connectable);

  //TODO:JK: implement me
}

void
ags_sf2_synth_disconnect(AgsConnectable *connectable)
{
  AgsSF2Synth *sf2_synth;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_sf2_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsSF2Synth */
  sf2_synth = AGS_SF2_SYNTH(connectable);

  //TODO:JK: implement me
}

void
ags_sf2_synth_resize_audio_channels(AgsMachine *machine,
				    guint audio_channels, guint audio_channels_old,
				    gpointer data)
{
  AgsSF2Synth *sf2_synth;

  sf2_synth = (AgsSF2Synth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_sf2_synth_input_map_recall(sf2_synth,
				     audio_channels_old,
				     0);
      
      ags_sf2_synth_output_map_recall(sf2_synth,
				      audio_channels_old,
				      0);
    }
  }
}

void
ags_sf2_synth_resize_pads(AgsMachine *machine, GType type,
			  guint pads, guint pads_old,
			  gpointer data)
{
  AgsSF2Synth *sf2_synth;

  gboolean grow;

  sf2_synth = (AgsSF2Synth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_sf2_synth_input_map_recall(sf2_synth,
				       0,
				       pads_old);
      }
    }else{
      sf2_synth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_sf2_synth_output_map_recall(sf2_synth,
					0,
					pads_old);
      }
    }else{
      sf2_synth->mapped_output_pad = pads;
    }
  }
}

void
ags_sf2_synth_map_recall(AgsMachine *machine)
{
  AgsSF2Synth *sf2_synth;
  
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  sf2_synth = AGS_SF2_SYNTH(machine);

  audio = machine->audio;
  
  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->playback_play_container, sf2_synth->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->notation_play_container, sf2_synth->notation_recall_container,
				       "ags-fx-notation",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->envelope_play_container, sf2_synth->envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->buffer_play_container, sf2_synth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* depending on destination */
  ags_sf2_synth_input_map_recall(sf2_synth,
				 0,
				 0);

  /* depending on destination */
  ags_sf2_synth_output_map_recall(sf2_synth,
				  0,
				  0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_sf2_synth_parent_class)->map_recall(machine);  
}

void
ags_sf2_synth_input_map_recall(AgsSF2Synth *sf2_synth,
			       guint audio_channel_start,
			       guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(sf2_synth->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  position = 0;
  
  input_pads = 0;
  audio_channels = 0;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->playback_play_container, sf2_synth->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->notation_play_container, sf2_synth->notation_recall_container,
				       "ags-fx-notation",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->envelope_play_container, sf2_synth->envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->buffer_play_container, sf2_synth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  sf2_synth->mapped_input_pad = input_pads;
}

void
ags_sf2_synth_output_map_recall(AgsSF2Synth *sf2_synth,
				guint audio_channel_start,
				guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(sf2_synth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  sf2_synth->mapped_output_pad = output_pads;
}

/**
 * ags_sf2_synth_sf2_loader_completed_timeout:
 * @sf2_synth: the #AgsSF2Synth
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed poll completed, otherwise %FALSE
 *
 * Since: 3.4.0
 */
gboolean
ags_sf2_synth_sf2_loader_completed_timeout(AgsSF2Synth *sf2_synth)
{
  if(g_hash_table_lookup(ags_sf2_synth_sf2_loader_completed,
			 sf2_synth) != NULL){
    //TODO:JK: implement me
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_sf2_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsSF2Synth
 *
 * Returns: the new #AgsSF2Synth
 *
 * Since: 3.4.0
 */
AgsSF2Synth*
ags_sf2_synth_new(GObject *soundcard)
{
  AgsSF2Synth *sf2_synth;

  sf2_synth = (AgsSF2Synth *) g_object_new(AGS_TYPE_SF2_SYNTH,
					   NULL);

  g_object_set(AGS_MACHINE(sf2_synth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(sf2_synth);
}
