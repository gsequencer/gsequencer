/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/machine/ags_fm_syncsynth.h>
#include <ags/X/machine/ags_fm_syncsynth_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <ags/X/file/ags_gui_file_xml.h>

#include <math.h>

#include <ags/i18n.h>

void ags_fm_syncsynth_class_init(AgsFMSyncsynthClass *fm_syncsynth);
void ags_fm_syncsynth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fm_syncsynth_plugin_interface_init(AgsPluginInterface *plugin);
void ags_fm_syncsynth_init(AgsFMSyncsynth *fm_syncsynth);
void ags_fm_syncsynth_finalize(GObject *gobject);

void ags_fm_syncsynth_connect(AgsConnectable *connectable);
void ags_fm_syncsynth_disconnect(AgsConnectable *connectable);

void ags_fm_syncsynth_show(GtkWidget *widget);
void ags_fm_syncsynth_map_recall(AgsMachine *machine);
gchar* ags_fm_syncsynth_get_name(AgsPlugin *plugin);
void ags_fm_syncsynth_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_fm_syncsynth_get_xml_type(AgsPlugin *plugin);
void ags_fm_syncsynth_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_fm_syncsynth_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
void ags_fm_syncsynth_read_resolve_audio(AgsFileLookup *file_lookup,
					 AgsMachine *machine);
xmlNode* ags_fm_syncsynth_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_fm_syncsynth_resize_audio_channels(AgsMachine *machine,
					    guint audio_channels, guint audio_channels_old,
					    gpointer data);
void ags_fm_syncsynth_resize_pads(AgsMachine *machine, GType channel_type,
				  guint pads, guint pads_old,
				  gpointer data);

void ags_fm_syncsynth_input_map_recall(AgsFMSyncsynth *fm_syncsynth,
				       guint input_pad_start);
void ags_fm_syncsynth_output_map_recall(AgsFMSyncsynth *fm_syncsynth,
					guint output_pad_start);

/**
 * SECTION:ags_fm_syncsynth
 * @short_description: fm_syncsynth
 * @title: AgsFMSyncsynth
 * @section_id:
 * @include: ags/X/machine/ags_fm_syncsynth.h
 *
 * The #AgsFMSyncsynth is a composite widget to act as fm_syncsynth.
 */

static gpointer ags_fm_syncsynth_parent_class = NULL;
static AgsConnectableInterface *ags_fm_syncsynth_parent_connectable_interface;

extern GHashTable *ags_machine_generic_output_message_monitor;
extern GHashTable *ags_machine_generic_input_message_monitor;

GType
ags_fm_syncsynth_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fm_syncsynth = 0;

    static const GTypeInfo ags_fm_syncsynth_info = {
      sizeof(AgsFMSyncsynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fm_syncsynth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFMSyncsynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fm_syncsynth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_fm_syncsynth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_fm_syncsynth_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_fm_syncsynth = g_type_register_static(AGS_TYPE_MACHINE,
						   "AgsFMSyncsynth", &ags_fm_syncsynth_info,
						   0);
    
    g_type_add_interface_static(ags_type_fm_syncsynth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_fm_syncsynth,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fm_syncsynth);
  }

  return g_define_type_id__volatile;
}

void
ags_fm_syncsynth_class_init(AgsFMSyncsynthClass *fm_syncsynth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_fm_syncsynth_parent_class = g_type_class_peek_parent(fm_syncsynth);

  /* GObjectClass */
  gobject = (GObjectClass *) fm_syncsynth;

  gobject->finalize = ags_fm_syncsynth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) fm_syncsynth;

  machine->map_recall = ags_fm_syncsynth_map_recall;
}

void
ags_fm_syncsynth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_fm_syncsynth_connectable_parent_interface;

  ags_fm_syncsynth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_fm_syncsynth_connect;
  connectable->disconnect = ags_fm_syncsynth_disconnect;
}

void
ags_fm_syncsynth_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_fm_syncsynth_get_name;
  plugin->set_name = ags_fm_syncsynth_set_name;
  plugin->get_xml_type = ags_fm_syncsynth_get_xml_type;
  plugin->set_xml_type = ags_fm_syncsynth_set_xml_type;
  plugin->read = ags_fm_syncsynth_read;
  plugin->write = ags_fm_syncsynth_write;
}

void
ags_fm_syncsynth_init(AgsFMSyncsynth *fm_syncsynth)
{
  GtkHBox *hbox;
  GtkVBox *vbox;
  GtkTable *table;
  GtkLabel *label;

  AgsAudio *audio;

  g_signal_connect_after((GObject *) fm_syncsynth, "parent_set",
			 G_CALLBACK(ags_fm_syncsynth_parent_set_callback), (gpointer) fm_syncsynth);
  
  audio = AGS_MACHINE(fm_syncsynth)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_SYNTH));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION));
  ags_audio_set_behaviour_flags(audio, (AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING |
					AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT));
  g_object_set(audio,
	       "min-audio-channels", 1,
	       "max-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       "max-input-pads", 128,
	       "audio-start-mapping", 0,
	       "audio-end-mapping", 128,
	       "midi-start-mapping", 0,
	       "midi-end-mapping", 128,
	       NULL);

  AGS_MACHINE(fm_syncsynth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				       AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(fm_syncsynth)->mapping_flags |= AGS_MACHINE_MONO;

  AGS_MACHINE(fm_syncsynth)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(fm_syncsynth)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(fm_syncsynth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(fm_syncsynth)->output_line_type = G_TYPE_NONE;

  /* context menu */
  ags_machine_popup_add_connection_options((AgsMachine *) fm_syncsynth,
  					   (AGS_MACHINE_POPUP_MIDI_DIALOG));

  /* audio resize */
  g_signal_connect(fm_syncsynth, "samplerate-changed",
		   G_CALLBACK(ags_fm_syncsynth_samplerate_changed_callback), NULL);

  g_signal_connect_after(G_OBJECT(fm_syncsynth), "resize-audio-channels",
			 G_CALLBACK(ags_fm_syncsynth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(fm_syncsynth), "resize-pads",
			 G_CALLBACK(ags_fm_syncsynth_resize_pads), NULL);
  
  /* create widgets */
  fm_syncsynth->flags = 0;

  /* mapped IO */
  fm_syncsynth->mapped_input_pad = 0;
  fm_syncsynth->mapped_output_pad = 0;
 
  /* context menu */
  ags_machine_popup_add_edit_options((AgsMachine *) fm_syncsynth,
				     (AGS_MACHINE_POPUP_ENVELOPE));
  
  /* name and xml type */
  fm_syncsynth->name = NULL;
  fm_syncsynth->xml_type = "ags-fm_syncsynth";
 
  /* create widgets */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) (gtk_bin_get_child((GtkBin *) fm_syncsynth)), (GtkWidget *) hbox);

  fm_syncsynth->fm_oscillator = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) fm_syncsynth->fm_oscillator,
		     FALSE,
		     FALSE,
		     0);

  /* add fm oscillator */
  ags_fm_syncsynth_add_fm_oscillator(fm_syncsynth,
				     ags_fm_oscillator_new());
  
  /* add and remove buttons */
  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) vbox, FALSE, FALSE, 0);

  fm_syncsynth->add = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) fm_syncsynth->add, FALSE, FALSE, 0);

  fm_syncsynth->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) fm_syncsynth->remove, FALSE, FALSE, 0);
  
  /* update */
  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) vbox, FALSE, FALSE, 0);

  fm_syncsynth->auto_update = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("auto update"));
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) fm_syncsynth->auto_update, FALSE, FALSE, 0);

  fm_syncsynth->update = (GtkButton *) gtk_button_new_with_label(i18n("update"));
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) fm_syncsynth->update, FALSE, FALSE, 0);

  /* table */
  table = (GtkTable *) gtk_table_new(3, 2, FALSE);
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) table, FALSE, FALSE, 0);

  /* lower - frequency */  
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("lower"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  fm_syncsynth->lower = (GtkSpinButton *) gtk_spin_button_new_with_range(AGS_FM_SYNCSYNTH_BASE_NOTE_MIN,
									 AGS_FM_SYNCSYNTH_BASE_NOTE_MAX,
									 1.0);
  gtk_spin_button_set_digits(fm_syncsynth->lower,
			     2);
  fm_syncsynth->lower->adjustment->value = -48.0;
  gtk_table_attach(table,
		   GTK_WIDGET(fm_syncsynth->lower),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* loop start */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("loop start"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  fm_syncsynth->loop_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, AGS_FM_OSCILLATOR_DEFAULT_FRAME_COUNT, 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(fm_syncsynth->loop_start),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* loop end */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("loop end"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  fm_syncsynth->loop_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, AGS_FM_OSCILLATOR_DEFAULT_FRAME_COUNT, 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(fm_syncsynth->loop_end),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* output - discard messages */
  g_hash_table_insert(ags_machine_generic_output_message_monitor,
		      fm_syncsynth,
		      ags_machine_generic_output_message_monitor_timeout);

  g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0,
		(GSourceFunc) ags_machine_generic_output_message_monitor_timeout,
		(gpointer) fm_syncsynth);

  /* input - discard messages */
  g_hash_table_insert(ags_machine_generic_input_message_monitor,
		      fm_syncsynth,
		      ags_machine_generic_input_message_monitor_timeout);

  g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0,
		(GSourceFunc) ags_machine_generic_input_message_monitor_timeout,
		(gpointer) fm_syncsynth);
}

void
ags_fm_syncsynth_finalize(GObject *gobject)
{
  g_hash_table_remove(ags_machine_generic_output_message_monitor,
		      gobject);

  g_hash_table_remove(ags_machine_generic_input_message_monitor,
		      gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fm_syncsynth_parent_class)->finalize(gobject);
}

void
ags_fm_syncsynth_connect(AgsConnectable *connectable)
{
  AgsFMSyncsynth *fm_syncsynth;

  GList *list_start, *list;
  GList *child_start;
  
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_fm_syncsynth_parent_connectable_interface->connect(connectable);

  /* AgsFMSyncsynth */
  fm_syncsynth = AGS_FM_SYNCSYNTH(connectable);

  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(fm_syncsynth->fm_oscillator));

  while(list != NULL){
    child_start = gtk_container_get_children(GTK_CONTAINER(list->data));

    ags_connectable_connect(AGS_CONNECTABLE(child_start->next->data));
    
    g_signal_connect((GObject *) child_start->next->data, "control-changed",
		     G_CALLBACK(ags_fm_syncsynth_fm_oscillator_control_changed_callback), (gpointer) fm_syncsynth);
    
    g_list_free(child_start);
    
    list = list->next;
  }

  g_list_free(list_start);

  g_signal_connect((GObject *) fm_syncsynth->add, "clicked",
		   G_CALLBACK(ags_fm_syncsynth_add_callback), (gpointer) fm_syncsynth);

  g_signal_connect((GObject *) fm_syncsynth->remove, "clicked",
		   G_CALLBACK(ags_fm_syncsynth_remove_callback), (gpointer) fm_syncsynth);

  g_signal_connect((GObject *) fm_syncsynth->auto_update, "toggled",
		   G_CALLBACK(ags_fm_syncsynth_auto_update_callback), fm_syncsynth);

  g_signal_connect((GObject *) fm_syncsynth->update, "clicked",
		   G_CALLBACK(ags_fm_syncsynth_update_callback), (gpointer) fm_syncsynth);
}

void
ags_fm_syncsynth_disconnect(AgsConnectable *connectable)
{
  AgsFMSyncsynth *fm_syncsynth;

  GList *list_start, *list;
  GList *child_start;
  
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_fm_syncsynth_parent_connectable_interface->disconnect(connectable);

  /* AgsFMSyncsynth */
  fm_syncsynth = AGS_FM_SYNCSYNTH(connectable);

  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(fm_syncsynth->fm_oscillator));

  while(list != NULL){
    child_start = gtk_container_get_children(GTK_CONTAINER(list->data));

    ags_connectable_disconnect(AGS_CONNECTABLE(child_start->next->data));
    
    g_object_disconnect((GObject *) child_start->next->data,
			"any_signal::control-changed",
			G_CALLBACK(ags_fm_syncsynth_fm_oscillator_control_changed_callback),
			(gpointer) fm_syncsynth,
			NULL);
    
    g_list_free(child_start);
    
    list = list->next;
  }

  g_list_free(list_start);

  g_object_disconnect((GObject *) fm_syncsynth->add,
		      "any_signal::clicked",
		      G_CALLBACK(ags_fm_syncsynth_add_callback),
		      (gpointer) fm_syncsynth,
		      NULL);
  
  g_object_disconnect((GObject *) fm_syncsynth->remove,
		      "any_signal::clicked",
		      G_CALLBACK(ags_fm_syncsynth_remove_callback),
		      (gpointer) fm_syncsynth,
		      NULL);

  g_object_disconnect((GObject *) fm_syncsynth->auto_update,
		      "any_signal::toggled",
		      G_CALLBACK(ags_fm_syncsynth_auto_update_callback),
		      fm_syncsynth,
		      NULL);
  
  g_object_disconnect((GObject *) fm_syncsynth->update,
		      "any_signal::clicked",
		      G_CALLBACK(ags_fm_syncsynth_update_callback),
		      (gpointer) fm_syncsynth,
		      NULL);
}

void
ags_fm_syncsynth_map_recall(AgsMachine *machine)
{
  AgsWindow *window;
  AgsFMSyncsynth *fm_syncsynth;
  
  AgsAudio *audio;

  AgsDelayAudio *play_delay_audio;
  AgsDelayAudioRun *play_delay_audio_run;
  AgsCountBeatsAudio *play_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;
  AgsRecordMidiAudio *recall_record_midi_audio;
  AgsRecordMidiAudioRun *recall_record_midi_audio_run;
  AgsPlayNotationAudio *recall_notation_audio;
  AgsPlayNotationAudioRun *recall_notation_audio_run;

  GList *start_play, *play;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  fm_syncsynth = AGS_FM_SYNCSYNTH(machine);
  
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  audio = machine->audio;

  /* ags-delay */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-delay",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  g_object_get(audio,
	       "play", &start_play,
	       NULL);
  
  play = ags_recall_find_type(start_play,
			      AGS_TYPE_DELAY_AUDIO_RUN);

  if(play != NULL){
    play_delay_audio_run = AGS_DELAY_AUDIO_RUN(play->data);
    //    AGS_RECALL(play_delay_audio_run)->flags |= AGS_RECALL_PERSISTENT;
  }else{
    play_delay_audio_run = NULL;
  }
  
  g_list_free_full(start_play,
		   g_object_unref);
  
  /* ags-count-beats */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-count-beats",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  g_object_get(audio,
	       "play", &start_play,
	       NULL);
  
  play = ags_recall_find_type(start_play,
			      AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

  if(play != NULL){
    GValue value = {0,};

    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(play->data);

    /* set dependency */  
    g_object_set(G_OBJECT(play_count_beats_audio_run),
		 "delay-audio-run", play_delay_audio_run,
		 NULL);
    ags_seekable_seek(AGS_SEEKABLE(play_count_beats_audio_run),
		      (gint64) 16 * window->navigation->position_tact->adjustment->value,
		      AGS_SEEK_SET);

    /* notation loop */
    g_value_init(&value, G_TYPE_BOOLEAN);
    g_value_set_boolean(&value, gtk_toggle_button_get_active((GtkToggleButton *) window->navigation->loop));
    ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(play_count_beats_audio_run)->recall_audio)->notation_loop,
			&value);

    g_value_unset(&value);
    g_value_init(&value, G_TYPE_UINT64);

    g_value_set_uint64(&value, 16 * window->navigation->loop_left_tact->adjustment->value);
    ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(play_count_beats_audio_run)->recall_audio)->notation_loop_start,
			&value);

    g_value_reset(&value);

    g_value_set_uint64(&value, 16 * window->navigation->loop_right_tact->adjustment->value);
    ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(play_count_beats_audio_run)->recall_audio)->notation_loop_end,
			&value);
  }else{
    play_count_beats_audio_run = NULL;
  }

  g_list_free_full(start_play,
		   g_object_unref);
  
  /* ags-record-midi */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-record-midi",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  g_object_get(audio,
	       "play", &start_play,
	       NULL);

  play = ags_recall_find_type(start_play,
			      AGS_TYPE_RECORD_MIDI_AUDIO_RUN);

  if(play != NULL){
    recall_record_midi_audio_run = AGS_RECORD_MIDI_AUDIO_RUN(play->data);
    
    /* set dependency */
    g_object_set(G_OBJECT(recall_record_midi_audio_run),
		 "delay-audio-run", play_delay_audio_run,
		 NULL);

    /* set dependency */
    g_object_set(G_OBJECT(recall_record_midi_audio_run),
		 "count-beats-audio-run", play_count_beats_audio_run,
		 NULL);
  }  

  g_list_free_full(start_play,
		   g_object_unref);
  
  /* ags-play-notation */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-play-notation",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  g_object_get(audio,
	       "play", &start_play,
	       NULL);

  play = ags_recall_find_type(start_play,
			      AGS_TYPE_PLAY_NOTATION_AUDIO_RUN);

  if(play != NULL){
    recall_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(play->data);

    /* set dependency */
    g_object_set(G_OBJECT(recall_notation_audio_run),
		 "delay-audio-run", play_delay_audio_run,
		 NULL);

    /* set dependency */
    g_object_set(G_OBJECT(recall_notation_audio_run),
		 "count-beats-audio-run", play_count_beats_audio_run,
		 NULL);
  }

  g_list_free_full(start_play,
		   g_object_unref);

  /* depending on destination */
  ags_fm_syncsynth_input_map_recall(fm_syncsynth, 0);

  /* depending on destination */
  ags_fm_syncsynth_output_map_recall(fm_syncsynth, 0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_fm_syncsynth_parent_class)->map_recall(machine);  
}

gchar*
ags_fm_syncsynth_get_name(AgsPlugin *plugin)
{
  return(AGS_FM_SYNCSYNTH(plugin)->name);
}

void
ags_fm_syncsynth_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_FM_SYNCSYNTH(plugin)->name = name;
}

gchar*
ags_fm_syncsynth_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_FM_SYNCSYNTH(plugin)->xml_type);
}

void
ags_fm_syncsynth_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_FM_SYNCSYNTH(plugin)->xml_type = xml_type;
}

void
ags_fm_syncsynth_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsFMSyncsynth *gobject;
  AgsFileLookup *file_lookup;
  GList *list;

  gobject = AGS_FM_SYNCSYNTH(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));

  /* lookup */
  list = file->lookup;

  while((list = ags_file_lookup_find_by_node(list,
					     node->parent)) != NULL){
    file_lookup = AGS_FILE_LOOKUP(list->data);
    
    if(g_signal_handler_find(list->data,
			     G_SIGNAL_MATCH_FUNC,
			     0,
			     0,
			     NULL,
			     ags_file_read_machine_resolve_audio,
			     NULL) != 0){
      g_signal_connect_after(G_OBJECT(file_lookup), "resolve",
			     G_CALLBACK(ags_fm_syncsynth_read_resolve_audio), gobject);
      
      break;
    }

    list = list->next;
  }
}

void
ags_fm_syncsynth_read_resolve_audio(AgsFileLookup *file_lookup,
				    AgsMachine *machine)
{
  AgsFMSyncsynth *fm_syncsynth;

  fm_syncsynth = AGS_FM_SYNCSYNTH(machine);

  g_signal_connect_after(G_OBJECT(machine), "resize-audio-channels",
			 G_CALLBACK(ags_fm_syncsynth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(machine), "resize-pads",
			 G_CALLBACK(ags_fm_syncsynth_resize_pads), NULL);

  if((AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) == 0){
    /* ags-play-notation */
    ags_recall_factory_create(machine->audio,
			      NULL, NULL,
			      "ags-play-notation",
			      0, machine->audio->audio_channels,
			      0, 0,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_REMAP |
			       AGS_RECALL_FACTORY_RECALL),
			      0);

    ags_fm_syncsynth_output_map_recall(fm_syncsynth, 0);
    ags_fm_syncsynth_input_map_recall(fm_syncsynth, 0);
  }else{
    fm_syncsynth->mapped_output_pad = machine->audio->output_pads;
    fm_syncsynth->mapped_input_pad = machine->audio->input_pads;
  }
}

xmlNode*
ags_fm_syncsynth_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsFMSyncsynth *fm_syncsynth;
  xmlNode *node;
  gchar *id;

  fm_syncsynth = AGS_FM_SYNCSYNTH(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-fm_syncsynth");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", fm_syncsynth,
				   NULL));

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_fm_syncsynth_resize_audio_channels(AgsMachine *machine,
				       guint audio_channels, guint audio_channels_old,
				       gpointer data)
{
  AgsAudio *audio;
  AgsChannel *start_output, *start_input;
  AgsChannel *channel, *next_pad, *next_channel, *nth_channel;

  guint output_pads, input_pads;
  
  audio = machine->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       "input-pads", &input_pads,
	       "output", &start_output,
	       "input", &start_input,
	       NULL);

  if(audio_channels > audio_channels_old){
    /* AgsOutput */
    channel = start_output;

    if(channel != NULL){
      g_object_ref(channel);
    }
    
    next_channel = NULL;
    
    while(channel != NULL){
      /* get some fields */
      next_pad = ags_channel_next_pad(channel);

      nth_channel = ags_channel_nth(channel,
				    audio_channels_old);

      g_object_unref(channel);

      channel = nth_channel;
      
      while(channel != next_pad){
	AgsRecycling *recycling;
	AgsAudioSignal *audio_signal;

	GObject *output_soundcard;

	ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_NOTATION));
		
	g_object_get(audio,
		     "output-soundcard", &output_soundcard,
		     NULL);

	/* get recycling */
	g_object_get(channel,
		     "first-recycling", &recycling,
		     NULL);

	/* instantiate template audio signal */
	audio_signal = ags_audio_signal_new(output_soundcard,
					    (GObject *) recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_recycling_add_audio_signal(recycling,
				       audio_signal);

	g_object_unref(output_soundcard);
	
	g_object_unref(recycling);

	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      /* unref */
      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
      
      if(next_channel != NULL){
	g_object_unref(next_channel);
      }
    }

    /* ags-envelope */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-envelope",
			      audio_channels_old, audio_channels, 
			      0, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL | 
			       AGS_RECALL_FACTORY_ADD),
			      0);
    
    /* AgsInput */
    channel = start_input;
    
    if(channel != NULL){
      g_object_ref(channel);
    }

    next_channel = NULL;
    
    while(channel != NULL){
      /* get some fields */
      next_pad = ags_channel_next_pad(channel);

      nth_channel = ags_channel_nth(channel,
				    audio_channels_old);

      g_object_unref(channel);

      channel = nth_channel;

      while(channel != next_pad){
	AgsPort *port;
      
	GList *start_play, *play;
	GList *start_recall, *recall;

	g_object_get(channel,
		     "play", &start_play,
		     "recall", &start_recall,
		     NULL);

	/* play */
	play = ags_recall_find_type(start_play,
				    AGS_TYPE_ENVELOPE_CHANNEL);
      
	if(play != NULL){
	  GValue value = {0};
      
	  g_object_get(play->data,
		       "use-note-length", &port,
		       NULL);

	  g_value_init(&value,
		       G_TYPE_BOOLEAN);
	  g_value_set_boolean(&value,
			      TRUE);

	  ags_port_safe_write(port,
			      &value);
	
	  g_object_unref(port);
	}

	g_list_free(start_play);

	/* recall */
	recall = ags_recall_find_type(start_recall,
				      AGS_TYPE_ENVELOPE_CHANNEL);
      
	if(recall != NULL){
	  GValue value = {0};
      
	  g_object_get(recall->data,
		       "use-note-length", &port,
		       NULL);

	  g_value_init(&value,
		       G_TYPE_BOOLEAN);
	  g_value_set_boolean(&value,
			      TRUE);

	  ags_port_safe_write(port,
			      &value);
	
	  g_object_unref(port);
	}

	g_list_free(start_recall);

	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      /* unref */
      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
      
      if(next_channel != NULL){
	g_object_unref(next_channel);
      }
    }
    
    if(ags_recall_global_get_rt_safe() ||
       ags_recall_global_get_performance_mode()){
      /* ags-copy */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-copy",
				audio_channels_old, audio_channels,
				0, input_pads,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);
    }else{    
      /* ags-buffer */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-buffer",
				audio_channels_old, audio_channels,
				0, input_pads,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);
    }
    
    /* ags-play */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-play",
			      audio_channels_old, audio_channels, 
			      0, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_ADD),
			      0);

    /* ags-stream */
    if(!ags_recall_global_get_rt_safe()){
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-stream",
				audio_channels_old, audio_channels, 
				0, input_pads,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL | 
				 AGS_RECALL_FACTORY_ADD),
				0);
    }else{
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-rt-stream",
				audio_channels_old, audio_channels, 
				0, input_pads,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL | 
				 AGS_RECALL_FACTORY_ADD),
				0);
    }
    
    /* AgsOutput */
    /* ags-stream */
    if(!(ags_recall_global_get_rt_safe() ||
	 ags_recall_global_get_performance_mode())){
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-stream",
				audio_channels_old, audio_channels,
				0, output_pads,
				(AGS_RECALL_FACTORY_OUTPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_ADD),
				0);
    }
  }

  /* unref */
  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_fm_syncsynth_resize_pads(AgsMachine *machine, GType type,
			     guint pads, guint pads_old,
			     gpointer data)
{
  AgsFMSyncsynth *fm_syncsynth;

  AgsAudio *audio;
  AgsChannel *start_output;
  AgsChannel *channel, *next_channel, *nth_channel;
  AgsAudioSignal *audio_signal;

  guint i, j;
  gboolean grow;

  if(pads == pads_old){
    return;
  }

  fm_syncsynth = (AgsFMSyncsynth *) machine;

  audio = machine->audio;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_fm_syncsynth_input_map_recall(fm_syncsynth,
					  pads_old);
      }

    }else{
      fm_syncsynth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      g_object_get(audio,
		   "output", &start_output,
		   NULL);
      
      nth_channel = ags_channel_pad_nth(start_output,
					pads_old);

      channel = nth_channel;

      next_channel = NULL;
      
      while(channel != NULL){
	AgsRecycling *recycling;
	AgsAudioSignal *audio_signal;

	GObject *output_soundcard;
	
	ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_NOTATION));
	
	g_object_get(audio,
		     "output-soundcard", &output_soundcard,
		     NULL);

	/* get recycling */
	g_object_get(channel,
		     "first-recycling", &recycling,
		     NULL);

	/* instantiate template audio signal */
	audio_signal = ags_audio_signal_new(output_soundcard,
					    (GObject *) recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_recycling_add_audio_signal(recycling,
				       audio_signal);

	g_object_unref(output_soundcard);
	
	g_object_unref(recycling);
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_fm_syncsynth_output_map_recall(fm_syncsynth,
					   pads_old);
      }

      /* unref */
      if(start_output != NULL){
	g_object_unref(start_output);
      }
      
      if(next_channel != NULL){
	g_object_unref(next_channel);
      }
    }else{
      fm_syncsynth->mapped_output_pad = pads;
    }
  }
}


void
ags_fm_syncsynth_input_map_recall(AgsFMSyncsynth *fm_syncsynth,
				  guint input_pad_start)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  guint input_pads;
  guint audio_channels;

  if(fm_syncsynth->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(fm_syncsynth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       "input", &start_input,
	       NULL);
  
  /* ags-envelope */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-envelope",
			    0, audio_channels, 
			    input_pad_start, input_pads,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL | 
			     AGS_RECALL_FACTORY_ADD),
			    0);

  channel = ags_channel_pad_nth(start_input,
				input_pad_start);
    
  while(channel != NULL){
    AgsPort *port;
      
    GList *start_play, *play;
    GList *start_recall, *recall;

    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    /* play */
    play = ags_recall_find_type(start_play,
				AGS_TYPE_ENVELOPE_CHANNEL);
      
    if(play != NULL){
      GValue value = {0};
      
      g_object_get(play->data,
		   "use-note-length", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_BOOLEAN);
      g_value_set_boolean(&value,
			  TRUE);

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_play);

    /* recall */
    recall = ags_recall_find_type(start_recall,
				  AGS_TYPE_ENVELOPE_CHANNEL);
      
    if(recall != NULL){
      GValue value = {0};
      
      g_object_get(recall->data,
		   "use-note-length", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_BOOLEAN);
      g_value_set_boolean(&value,
			  TRUE);

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_recall);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
  
  /* remap for input */
  if(ags_recall_global_get_rt_safe() ||
     ags_recall_global_get_performance_mode()){
    /* ags-copy */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-copy",
			      0, audio_channels, 
			      input_pad_start, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }else{    
    /* ags-buffer */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-buffer",
			      0, audio_channels, 
			      input_pad_start, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }
  
  /* ags-play */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-play",
			    0, audio_channels, 
			    input_pad_start, input_pads,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* ags-feed */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-feed",
			    0, audio_channels, 
			    input_pad_start, input_pads,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL | 
			     AGS_RECALL_FACTORY_ADD),
			    0);
  
  /* ags-stream */
  if(!(ags_recall_global_get_rt_safe() ||
       ags_recall_global_get_performance_mode())){
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-stream",
			      0, audio_channels, 
			      input_pad_start, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL | 
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }else{
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-rt-stream",
			      0, audio_channels, 
			      input_pad_start, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL | 
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }
  
  fm_syncsynth->mapped_input_pad = input_pads;

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_fm_syncsynth_output_map_recall(AgsFMSyncsynth *fm_syncsynth,
				   guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads, input_pads;
  guint audio_channels;

  if(fm_syncsynth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(fm_syncsynth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);
  
  /* remap for input */
  if(ags_recall_global_get_rt_safe() ||
     ags_recall_global_get_performance_mode()){
    /* ags-copy */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-copy",
			      0, audio_channels, 
			      0, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }else{    
    /* ags-buffer */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-buffer",
			      0, audio_channels, 
			      0, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }

  if(!(ags_recall_global_get_rt_safe() ||
       ags_recall_global_get_performance_mode())){
    /* ags-stream */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-stream",
			      0, audio_channels,
			      output_pad_start, output_pads,
			      (AGS_RECALL_FACTORY_OUTPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }
  
  fm_syncsynth->mapped_output_pad = audio->output_pads;
}

/**
 * ags_fm_syncsynth_add_fm_oscillator:
 * @fm_syncsynth: the #AgsFMSyncsynth
 * @fm_oscillator: the #AgsFMOscillator
 * 
 * Add @fm_oscillator to @fm_syncsynth.
 * 
 * Since: 2.3.0
 */
void
ags_fm_syncsynth_add_fm_oscillator(AgsFMSyncsynth *fm_syncsynth,
				   AgsFMOscillator *fm_oscillator)
{
  AgsAudio *audio;
  
  GtkHBox *hbox;
  GtkCheckButton *check_button;

  audio = AGS_MACHINE(fm_syncsynth)->audio;
  ags_audio_add_synth_generator(audio,
				(GObject *) ags_synth_generator_new());
  
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);

  check_button = (GtkCheckButton *) gtk_check_button_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) check_button,
		     FALSE,
		     FALSE,
		     0);
  
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) fm_oscillator,
		     FALSE,
		     FALSE,
		     0);

  gtk_box_pack_start((GtkBox *) fm_syncsynth->fm_oscillator,
		     (GtkWidget *) hbox,
		     FALSE,
		     FALSE,
		     0);
  gtk_widget_show_all((GtkWidget *) hbox);
}

/**
 * ags_fm_syncsynth_remove_fm_oscillator:
 * @fm_syncsynth: the #AgsFMSyncsynth
 * @nth: the nth #AgsFMOscillator
 * 
 * Remove nth fm_oscillator.
 * 
 * Since: 2.3.0
 */
void
ags_fm_syncsynth_remove_fm_oscillator(AgsFMSyncsynth *fm_syncsynth,
				      guint nth)
{
  AgsAudio *audio;
  
  GList *list, *list_start;
  GList *start_synth_generator;
  
  audio = AGS_MACHINE(fm_syncsynth)->audio;
  g_object_get(audio,
	       "synth-generator", &start_synth_generator,
	       NULL);

  start_synth_generator = g_list_reverse(start_synth_generator);
  ags_audio_remove_synth_generator(audio,
				   g_list_nth_data(start_synth_generator,
						   nth));

  g_list_free_full(start_synth_generator,
		   g_object_unref);
  
  list_start = gtk_container_get_children(GTK_CONTAINER(fm_syncsynth->fm_oscillator));

  list = g_list_nth(list_start,
		    nth);

  if(list != NULL){
    gtk_widget_destroy(list->data);
  }

  g_list_free(list_start);
}

/**
 * ags_fm_syncsynth_reset_loop:
 * @fm_syncsynth: the #AgsFMSyncsynth
 * 
 * Reset loop spin buttons.
 * 
 * Since: 2.3.0
 */
void
ags_fm_syncsynth_reset_loop(AgsFMSyncsynth *fm_syncsynth)
{
  GList *list, *list_start;
  GList *child_start;
  
  gdouble loop_upper, tmp0, tmp1;

  loop_upper = 0.0;

  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(fm_syncsynth->fm_oscillator));
  
  while(list != NULL){
    child_start = gtk_container_get_children(GTK_CONTAINER(list->data));

    tmp0 = gtk_spin_button_get_value(AGS_FM_OSCILLATOR(child_start->next->data)->frame_count);
    tmp1 = gtk_spin_button_get_value(AGS_FM_OSCILLATOR(child_start->next->data)->attack);

    if(tmp0 + tmp1 > loop_upper){
      loop_upper = tmp0 + tmp1;
    }

    g_list_free(child_start);
    
    list = list->next;
  }

  g_list_free(list_start);
  
  gtk_spin_button_set_range(fm_syncsynth->loop_start,
			    0.0, loop_upper);
  gtk_spin_button_set_range(fm_syncsynth->loop_end,
			    0.0, loop_upper);
}

/**
 * ags_fm_syncsynth_update:
 * @fm_syncsynth: the #AgsFMSyncsynth
 * 
 * Update audio data.
 * 
 * Since: 2.3.0
 */
void
ags_fm_syncsynth_update(AgsFMSyncsynth *fm_syncsynth)
{
  AgsWindow *window;
  AgsFMOscillator *fm_oscillator;
  
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsClearAudioSignal *clear_audio_signal;
  AgsApplySynth *apply_synth;

  AgsApplicationContext *application_context;
  
  GList *list, *list_start;
  GList *start_synth_generator, *synth_generator;    
  GList *child_start;
  GList *task;
  
  guint input_lines;
  guint buffer_size;
  guint format;
  guint attack, frame_count;
  guint loop_start, loop_end;
  gdouble frequency, phase, start_frequency;
  gdouble volume;

  AgsComplex **sync_point;
  guint sync_point_count;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) fm_syncsynth);

  application_context = (AgsApplicationContext *) window->application_context;

  audio = AGS_MACHINE(fm_syncsynth)->audio;

  /*  */
  start_frequency = (gdouble) gtk_spin_button_get_value_as_float(fm_syncsynth->lower);

  /* clear input */
  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  next_channel = NULL;

  task = NULL;

  while(channel != NULL){
    AgsRecycling *first_recycling;
    AgsAudioSignal *template;

    GList *start_list;
    
    g_object_get(channel,
		 "first-recycling", &first_recycling,
		 NULL);

    g_object_get(first_recycling,
		 "audio-signal", &start_list,
		 NULL);
    
    /* clear task */
    template = ags_audio_signal_get_template(start_list);

    clear_audio_signal = ags_clear_audio_signal_new(template);
    task = g_list_prepend(task,
			  clear_audio_signal);

    g_list_free_full(start_list,
		     g_object_unref);
    
    g_object_unref(first_recycling);
    g_object_unref(template);
    
    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }

  /* write input */
  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(fm_syncsynth->fm_oscillator));

  /* get some fields */
  g_object_get(audio,
	       "input-lines", &input_lines,
	       "synth-generator", &start_synth_generator,
	       NULL);

  g_object_get(start_input,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);

  loop_start = (guint) gtk_spin_button_get_value_as_int(fm_syncsynth->loop_start);
  loop_end = (guint) gtk_spin_button_get_value_as_int(fm_syncsynth->loop_end);

  synth_generator = start_synth_generator;

  while(list != NULL){
    guint i;
    gboolean do_sync;
    
    /* do it so */
    child_start = gtk_container_get_children(GTK_CONTAINER(list->data));

    fm_oscillator = AGS_FM_OSCILLATOR(child_start->next->data);

    g_list_free(child_start);
        
    attack = (guint) gtk_spin_button_get_value_as_int(fm_oscillator->attack);
    frame_count = (guint) gtk_spin_button_get_value_as_int(fm_oscillator->frame_count);
    phase = (gdouble) gtk_spin_button_get_value_as_float(fm_oscillator->phase);
    frequency = (gdouble) gtk_spin_button_get_value_as_float(fm_oscillator->frequency);
    volume = (gdouble) gtk_spin_button_get_value_as_float(fm_oscillator->volume);

    g_object_set(synth_generator->data,
		 "format", format,
		 "delay", (gdouble) attack / buffer_size,
		 "attack", attack,
		 "frame-count", frame_count,
		 "loop-start", loop_start,
		 "loop-end", loop_end,
		 "oscillator", gtk_combo_box_get_active(fm_oscillator->wave),
		 "frequency", frequency,
		 "phase", phase,
		 "volume", volume,
		 NULL);

    do_sync = gtk_toggle_button_get_active((GtkToggleButton *) fm_oscillator->do_sync);

    if(do_sync){
      sync_point_count = fm_oscillator->sync_point_count;
      
      /* free previous sync point */
      if(AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point != NULL){
	for(i = 0; i < AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point_count; i++){
	  ags_complex_free(AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i]);
	}

	free(AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point);
      }

      /* set new sync point */
      if(sync_point_count > 0){
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point = (AgsComplex **) malloc(sync_point_count * sizeof(AgsComplex *));
      }else{
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point = NULL;
      }

      AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point_count = sync_point_count;

      for(i = 0; i < sync_point_count; i++){
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i] = ags_complex_alloc();

	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i][0][0] = gtk_spin_button_get_value(fm_oscillator->sync_point[2 * i]);
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i][0][1] = gtk_spin_button_get_value(fm_oscillator->sync_point[2 * i + 1]);
      }
    }else{
      for(i = 0; i < AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point_count; i++){
	ags_complex_free(AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i]);
      }

      free(AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point);
      
      AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point = NULL;
      AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point_count = 0;
    }
					
    apply_synth = ags_apply_synth_new(synth_generator->data,
				      start_input,
				      start_frequency, input_lines);
        
    task = g_list_prepend(task,
			  apply_synth);

    /* iterate */
    synth_generator = synth_generator->next;
    
    list = list->next;
  }

  g_list_free_full(start_synth_generator,
		   g_object_unref);
  
  g_list_free(list_start);

  ags_xorg_application_context_schedule_task_list(application_context,
						  g_list_reverse(task));
}

/**
 * ags_fm_syncsynth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsFMSyncsynth
 *
 * Returns: the new #AgsFMSyncsynth
 *
 * Since: 2.3.0
 */
AgsFMSyncsynth*
ags_fm_syncsynth_new(GObject *soundcard)
{
  AgsFMSyncsynth *fm_syncsynth;

  fm_syncsynth = (AgsFMSyncsynth *) g_object_new(AGS_TYPE_FM_SYNCSYNTH,
						 NULL);

  g_object_set(AGS_MACHINE(fm_syncsynth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(fm_syncsynth);
}
