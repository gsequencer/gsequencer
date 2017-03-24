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

#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_matrix_callbacks.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_portlet.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_seekable.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_launch.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_thread-posix.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_loop_channel.h>
#include <ags/audio/recall/ags_loop_channel_run.h>
#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_stream_channel_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>
#include <ags/audio/recall/ags_play_notation_audio.h>
#include <ags/audio/recall/ags_play_notation_audio_run.h>
#include <ags/audio/recall/ags_buffer_channel.h>
#include <ags/audio/recall/ags_buffer_channel_run.h>
#include <ags/audio/recall/ags_record_midi_audio.h>
#include <ags/audio/recall/ags_record_midi_audio_run.h>
#include <ags/audio/recall/ags_play_notation_audio.h>
#include <ags/audio/recall/ags_play_notation_audio_run.h>

#include <ags/X/ags_menu_bar.h>

#include <ags/X/file/ags_gui_file_xml.h>

#include <math.h>

#define AGS_MATRIX_INPUT_LINE_MAPPED_KEY "AGS_MATRIX_INPUT_LINE_MAPPED_KEY"
#define AGS_MATRIX_INPUT_LINE_MAPPED_DATA "AGS_MATRIX_INPUT_LINE_MAPPED_DATA"

void ags_matrix_class_init(AgsMatrixClass *matrix);
void ags_matrix_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_matrix_plugin_interface_init(AgsPluginInterface *plugin);
void ags_matrix_init(AgsMatrix *matrix);
void ags_matrix_finalize(GObject *gobject);
void ags_matrix_connect(AgsConnectable *connectable);
void ags_matrix_disconnect(AgsConnectable *connectable);
void ags_matrix_map_recall(AgsMachine *machine);
gchar* ags_matrix_get_name(AgsPlugin *plugin);
void ags_matrix_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_matrix_get_xml_type(AgsPlugin *plugin);
void ags_matrix_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_matrix_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
void ags_matrix_read_resolve_audio(AgsFileLookup *file_lookup,
				   AgsMachine *machine);
void ags_matrix_launch_task(AgsFileLaunch *file_launch, AgsMatrix *matrix);
xmlNode* ags_matrix_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_matrix_set_audio_channels(AgsAudio *audio,
				   guint audio_channels, guint audio_channels_old,
				   gpointer data);
void ags_matrix_set_pads(AgsAudio *audio, GType type,
			 guint pads, guint pads_old,
			 gpointer data);

/**
 * SECTION:ags_matrix
 * @short_description: matrix sequencer
 * @title: AgsMatrix
 * @section_id:
 * @include: ags/X/machine/ags_matrix.h
 *
 * The #AgsMatrix is a composite widget to act as matrix sequencer.
 */

static gpointer ags_matrix_parent_class = NULL;

static AgsConnectableInterface *ags_matrix_parent_connectable_interface;

const char *AGS_MATRIX_INDEX = "AgsMatrixIndex\0";

GType
ags_matrix_get_type(void)
{
  static GType ags_type_matrix = 0;

  if(!ags_type_matrix){
    static const GTypeInfo ags_matrix_info = {
      sizeof(AgsMatrixClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_matrix_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMatrix),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_matrix_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_matrix_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_matrix_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_matrix = g_type_register_static(AGS_TYPE_MACHINE,
					    "AgsMatrix\0", &ags_matrix_info,
					    0);
    
    g_type_add_interface_static(ags_type_matrix,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_matrix,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_matrix);
}

void
ags_matrix_class_init(AgsMatrixClass *matrix)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_matrix_parent_class = g_type_class_peek_parent(matrix);

  /* GObjectClass */
  gobject = (GObjectClass *) matrix;

  gobject->finalize = ags_matrix_finalize;

  /* AgsMachine */
  machine = (AgsMachineClass *) matrix;

  machine->map_recall = ags_matrix_map_recall;
}

void
ags_matrix_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_matrix_connectable_parent_interface;

  ags_matrix_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_matrix_connect;
  connectable->disconnect = ags_matrix_disconnect;
}

void
ags_matrix_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_matrix_get_name;
  plugin->set_name = ags_matrix_set_name;
  plugin->get_xml_type = ags_matrix_get_xml_type;
  plugin->set_xml_type = ags_matrix_set_xml_type;
  plugin->read = ags_matrix_read;
  plugin->write = ags_matrix_write;
}

void
ags_matrix_init(AgsMatrix *matrix)
{
  GtkFrame *frame;
  GtkTable *table;
  GtkToggleButton *button;
  GtkVBox *vbox;
  GtkHBox *hbox;

  AgsAudio *audio;

  GList *list;
  int i, j;

  g_signal_connect_after((GObject *) matrix, "parent_set\0",
			 G_CALLBACK(ags_matrix_parent_set_callback), (gpointer) matrix);

  audio = AGS_MACHINE(matrix)->audio;
  audio->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		   AGS_AUDIO_INPUT_HAS_RECYCLING |
		   AGS_AUDIO_SYNC |
		   AGS_AUDIO_ASYNC |
		   AGS_AUDIO_NOTATION_DEFAULT |
		   AGS_AUDIO_HAS_NOTATION |
		   AGS_AUDIO_PATTERN_MODE);
  g_object_set(audio,
	       "audio-start-mapping\0", 0,
	       "audio-end-mapping\0", 128,
	       "midi-start-mapping\0", 0,
	       "midi-end-mapping\0", 128,
	       NULL);

  AGS_MACHINE(matrix)->flags |= (AGS_MACHINE_REVERSE_NOTATION);

  ags_machine_popup_add_connection_options((AgsMachine *) matrix,
					   (AGS_MACHINE_POPUP_MIDI_DIALOG));
  
  AGS_MACHINE(matrix)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(matrix)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(matrix)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(matrix)->output_line_type = G_TYPE_NONE;

  g_signal_connect_after(G_OBJECT(audio), "set_audio_channels\0",
			 G_CALLBACK(ags_matrix_set_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(audio), "set_pads\0",
			 G_CALLBACK(ags_matrix_set_pads), NULL);

  /*  */
  AGS_MACHINE(matrix)->flags |= (AGS_MACHINE_IS_SEQUENCER |
				 AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(matrix)->mapping_flags |= AGS_MACHINE_MONO;
  
  matrix->flags = 0;

  matrix->name = NULL;
  matrix->xml_type = "ags-matrix\0";

  matrix->mapped_input_pad = 0;
  matrix->mapped_output_pad = 0;

  ags_machine_popup_add_edit_options((AgsMachine *) matrix,
				     (AGS_MACHINE_POPUP_COPY_PATTERN));
  
  /* create widgets */
  frame = (GtkFrame *) (gtk_bin_get_child((GtkBin *) matrix));

  matrix->table = (GtkTable *) gtk_table_new(4, 4, FALSE);
  gtk_container_add((GtkContainer *) frame,
		    (GtkWidget *) matrix->table);

  AGS_MACHINE(matrix)->play = 
    matrix->run = (GtkToggleButton *) gtk_toggle_button_new_with_label("run\0");
  gtk_table_attach(matrix->table, (GtkWidget *) matrix->run,
		   0, 1, 0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  table = (GtkTable *) gtk_table_new(3, 3, FALSE);
  gtk_table_attach(matrix->table, (GtkWidget *) table,
		   1, 2, 0, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  matrix->selected = NULL;

  for(i = 0; i < 3; i++){
    for(j = 0; j < 3; j++){
      //TODO:JK: verify leak of string
      matrix->index[i * 3 + j] =
	button = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup_printf("%d\0", i * 3 + j +1));
      g_object_set_data((GObject *) button, AGS_MATRIX_INDEX, GUINT_TO_POINTER(i * 3 + j));
      gtk_table_attach(table,
		       (GtkWidget *) button,
		       j, j +1,
		       i, i +1,
		       GTK_FILL, GTK_FILL,
		       0, 0);
    }
  }

  matrix->selected = matrix->index[0];
  gtk_toggle_button_set_active(matrix->selected, TRUE);

  /* sequencer */
  matrix->cell_pattern = ags_cell_pattern_new();
  gtk_table_attach(matrix->table,
		   (GtkWidget *) matrix->cell_pattern,
		   2, 3,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* length and loop */
  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_table_attach(matrix->table,
		   (GtkWidget *) vbox,
		   3, 4,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  gtk_box_pack_start((GtkBox *) hbox,
		     gtk_label_new("length\0"),
		     FALSE, FALSE,
		     0);

  matrix->length_spin = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 32.0, 1.0);
  matrix->length_spin->adjustment->value = 16.0;
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) matrix->length_spin,
		     FALSE, FALSE,
		     0);

  matrix->loop_button = (GtkCheckButton *) gtk_check_button_new_with_label("loop\0");
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) matrix->loop_button,
		     FALSE, FALSE,
		     0);
}

void
ags_matrix_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_matrix_parent_class)->finalize(gobject);
}

void
ags_matrix_connect(AgsConnectable *connectable)
{
  AgsMatrix *matrix;

  int i;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_matrix_parent_connectable_interface->connect(connectable);

  /* AgsMatrix */
  matrix = AGS_MATRIX(connectable);

  for(i  = 0; i < 9; i++){
    g_signal_connect(G_OBJECT (matrix->index[i]), "clicked\0",
		     G_CALLBACK (ags_matrix_index_callback), (gpointer) matrix);
  }

  ags_connectable_connect(AGS_CONNECTABLE(matrix->cell_pattern));
  
  g_signal_connect_after((GObject *) matrix->length_spin, "value-changed\0",
			 G_CALLBACK(ags_matrix_length_spin_callback), (gpointer) matrix);

  g_signal_connect((GObject *) matrix->loop_button, "clicked\0",
		   G_CALLBACK(ags_matrix_loop_button_callback), (gpointer) matrix);

  g_signal_connect_after(G_OBJECT(AGS_MACHINE(matrix)->audio), "tact\0",
			 G_CALLBACK(ags_matrix_tact_callback), matrix);

  g_signal_connect_after(G_OBJECT(AGS_MACHINE(matrix)->audio), "done\0",
			 G_CALLBACK(ags_matrix_done_callback), matrix);
}

void
ags_matrix_disconnect(AgsConnectable *connectable)
{
  AgsMatrix *matrix;

  int i;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  /* AgsMatrix */
  matrix = AGS_MATRIX(connectable);

  for(i  = 0; i < 9; i++){
    g_object_disconnect(G_OBJECT (matrix->index[i]),
			"clicked\0",
			G_CALLBACK (ags_matrix_index_callback),
			(gpointer) matrix,
			NULL);
  }

  ags_connectable_disconnect(AGS_CONNECTABLE(matrix->cell_pattern));
  
  g_object_disconnect((GObject *) matrix->length_spin,
		      "value-changed\0",
		      G_CALLBACK(ags_matrix_length_spin_callback),
		      (gpointer) matrix,
		      NULL);

  g_object_disconnect((GObject *) matrix->loop_button,
		      "clicked\0",
		      G_CALLBACK(ags_matrix_loop_button_callback),
		      (gpointer) matrix,
		      NULL);

  g_object_disconnect(G_OBJECT(AGS_MACHINE(matrix)->audio),
		      "tact\0",
		      G_CALLBACK(ags_matrix_tact_callback),
		      matrix,
		      NULL);

  g_object_disconnect(G_OBJECT(AGS_MACHINE(matrix)->audio),
		      "done\0",
		      G_CALLBACK(ags_matrix_done_callback),
		      matrix,
		      NULL);

  /* call parent */
  ags_matrix_parent_connectable_interface->disconnect(connectable);
}

void
ags_matrix_set_audio_channels(AgsAudio *audio,
			      guint audio_channels, guint audio_channels_old,
			      gpointer data)
{
  g_message("AgsMatrix only pads can be adjusted\0");
  //  _ags_audio_set_audio_channels(audio, audio_channels);
}

void
ags_matrix_set_pads(AgsAudio *audio, GType type,
		    guint pads, guint pads_old,
		    gpointer data)
{
  AgsMachine *machine;
  AgsMatrix *matrix;

  AgsChannel *channel, *source;
  AgsAudioSignal *audio_signal;

  AgsMutexManager *mutex_manager;

  guint i, j;
  gboolean grow;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *soundcard_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *source_mutex;

  if(pads == pads_old){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* lookup audio mutex */
  pthread_mutex_lock(application_mutex);
    
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get machine */
  pthread_mutex_lock(audio_mutex);
  
  matrix = (AgsMatrix *) audio->machine;

  pthread_mutex_unlock(audio_mutex);

  machine = AGS_MACHINE(matrix);

  /* set size request if needed */
  if(g_type_is_a(type, AGS_TYPE_INPUT)){
    gtk_adjustment_set_upper(GTK_RANGE(matrix->cell_pattern->vscrollbar)->adjustment,
			     (double) pads);

    
    if(pads < AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
      //      gtk_widget_set_size_request((GtkWidget *) matrix->cell_pattern,
      //			  -1,
      //			  pads * matrix->cell_pattern->cell_height + AGS_LED_DEFAULT_HEIGHT + 2);
    }else if(pads_old < AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
      //      gtk_widget_set_size_request((GtkWidget *) matrix->cell_pattern,
      //				  -1,
      //				  AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY * matrix->cell_pattern->cell_height + AGS_LED_DEFAULT_HEIGHT + 2);
    }
  }

  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(type, AGS_TYPE_INPUT)){
    AgsPattern *pattern;

    GList *list, *notation;

    pthread_mutex_lock(audio_mutex);

    source = audio->input;

    pthread_mutex_unlock(audio_mutex);

    if(grow){
      /* create pattern */
      source = ags_channel_nth(source, pads_old);
      
      while(source != NULL){
	/* lookup source mutex */
	pthread_mutex_lock(application_mutex);

	source_mutex = ags_mutex_manager_lookup(mutex_manager,
						(GObject *) source);
  
	pthread_mutex_unlock(application_mutex);

	/* instantiate pattern */
	pthread_mutex_lock(source_mutex);

	if(source->pattern == NULL){
	  source->pattern = g_list_alloc();
	  source->pattern->data = (gpointer) ags_pattern_new();
	  ags_pattern_set_dim((AgsPattern *) source->pattern->data, 1, 9, 32);
	}
	
	source = source->next;

	pthread_mutex_unlock(source_mutex);
      }

      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_matrix_input_map_recall(matrix,
				    pads_old);
      }
    }else{
      matrix->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      AgsChannel *current, *output;

      GList *recall;
      GList *list;

      guint stop;

      pthread_mutex_lock(audio_mutex);

      source = audio->output;

      pthread_mutex_unlock(audio_mutex);

      source = ags_channel_nth(source,
			       pads_old);

      if(source != NULL){
	GObject *soundcard;
	AgsRecycling *recycling;
	AgsAudioSignal *audio_signal;

	pthread_mutex_lock(audio_mutex);
	
	soundcard = audio->soundcard;

	pthread_mutex_unlock(audio_mutex);

	/* lookup source mutex */
	pthread_mutex_lock(application_mutex);

	source_mutex = ags_mutex_manager_lookup(mutex_manager,
						(GObject *) source);
  
	pthread_mutex_unlock(application_mutex);

	/* get recycling */
	pthread_mutex_lock(source_mutex);

	recycling = source->first_recycling;

	pthread_mutex_unlock(source_mutex);

	/* instantiate template audio signal */
	audio_signal = ags_audio_signal_new(soundcard,
					    (GObject *) recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_recycling_add_audio_signal(recycling,
				       audio_signal);

	ags_matrix_output_map_recall(matrix,
				     pads_old);
      }
    }else{
      matrix->mapped_output_pad = pads;
    }
  }
}

void
ags_matrix_map_recall(AgsMachine *machine)
{
  AgsWindow *window;
  AgsMatrix *matrix;

  AgsAudio *audio;
  AgsChannel *channel;
  AgsPattern *pattern;

  AgsDelayAudio *play_delay_audio;
  AgsDelayAudioRun *play_delay_audio_run;
  AgsCountBeatsAudio *play_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;
  AgsCopyPatternAudio *recall_copy_pattern_audio;
  AgsCopyPatternAudioRun *recall_copy_pattern_audio_run;
  AgsPlayNotationAudio  *play_notation;
  AgsCopyPatternChannel *copy_pattern_channel;
  AgsRecordMidiAudio *recall_record_midi_audio;
  AgsRecordMidiAudioRun *recall_record_midi_audio_run;
  AgsPlayNotationAudio *recall_notation_audio;
  AgsPlayNotationAudioRun *recall_notation_audio_run;

  GList *notation;
  GList *list;

  guint i, j;

  GValue value = {0,};

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  matrix = AGS_MATRIX(machine);
  audio = machine->audio;

  /* ags-delay */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-delay\0",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_DELAY_AUDIO_RUN);

  if(list != NULL){
    play_delay_audio_run = AGS_DELAY_AUDIO_RUN(list->data);
    //    AGS_RECALL(play_delay_audio_run)->flags |= AGS_RECALL_PERSISTENT;
  }else{
    play_delay_audio_run = NULL;
  }
  
  /* ags-count-beats */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-count-beats\0",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);
  
  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

  if(list != NULL){
    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);

    /* set dependency */  
    g_object_set(G_OBJECT(play_count_beats_audio_run),
		 "delay-audio-run\0", play_delay_audio_run,
		 NULL);
    ags_seekable_seek(AGS_SEEKABLE(play_count_beats_audio_run),
		      window->navigation->position_tact->adjustment->value,
		      TRUE);

    g_value_init(&value, G_TYPE_BOOLEAN);
    g_value_set_boolean(&value, gtk_toggle_button_get_active((GtkToggleButton *) window->navigation->loop));
    ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(play_count_beats_audio_run)->recall_audio)->notation_loop,
			&value);
  }else{
    play_count_beats_audio_run = NULL;
  }

  /* ags-copy-pattern */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-copy-pattern\0",
			    0, audio->audio_channels,
			    0, audio->input_lines,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_RECALL),
			    0);

  list = ags_recall_find_type(audio->recall,
			      AGS_TYPE_COPY_PATTERN_AUDIO_RUN);

  if(list != NULL){
    recall_copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(list->data);

    /* set dependency */
    g_object_set(G_OBJECT(recall_copy_pattern_audio_run),
		 "delay-audio-run\0", play_delay_audio_run,
		 "count-beats-audio-run\0", play_count_beats_audio_run,
		 NULL);

  }

  /* set pattern object on port */
  channel = ags_channel_pad_nth(audio->input, 0);
      
  for(i = 0; i < audio->input_pads; i++){
    for(j = 0; j < audio->audio_channels; j++){
      list = ags_recall_template_find_type(channel->recall,
					   AGS_TYPE_COPY_PATTERN_CHANNEL);

      if(list != NULL){
	copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(list->data);

	list = channel->pattern;
	pattern = AGS_PATTERN(list->data);

	copy_pattern_channel->pattern->port_value.ags_port_object = (GObject *) pattern;
	  
	ags_portlet_set_port(AGS_PORTLET(pattern),
			     (GObject *) copy_pattern_channel->pattern);
      }
      
      channel = channel->next;
    }
  }

  /* ags-record-midi */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-record-midi\0",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_RECALL),
			    0);

  list = ags_recall_find_type(audio->recall, AGS_TYPE_RECORD_MIDI_AUDIO_RUN);

  if(list != NULL){
    recall_record_midi_audio_run = AGS_RECORD_MIDI_AUDIO_RUN(list->data);
    
    /* set dependency */
    g_object_set(G_OBJECT(recall_record_midi_audio_run),
		 "delay-audio-run\0", play_delay_audio_run,
		 NULL);

    /* set dependency */
    g_object_set(G_OBJECT(recall_record_midi_audio_run),
		 "count-beats-audio-run\0", play_count_beats_audio_run,
		 NULL);
  }  

  /* ags-play-notation */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-play-notation\0",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_RECALL),
			    0);

  list = ags_recall_find_type(audio->recall, AGS_TYPE_PLAY_NOTATION_AUDIO_RUN);

  if(list != NULL){
    recall_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(list->data);

    /* set dependency */
    g_object_set(G_OBJECT(recall_notation_audio_run),
		 "delay-audio-run\0", play_delay_audio_run,
		 NULL);

    /* set dependency */
    g_object_set(G_OBJECT(recall_notation_audio_run),
		 "count-beats-audio-run\0", play_count_beats_audio_run,
		 NULL);
  }

  /* depending on destination */
  ags_matrix_input_map_recall(matrix, 0);

  /* depending on destination */
  ags_matrix_output_map_recall(matrix, 0);

  AGS_MACHINE_CLASS(ags_matrix_parent_class)->map_recall(machine);
}

void
ags_matrix_input_map_recall(AgsMatrix *matrix, guint input_pad_start)
{
  AgsAudio *audio;
  AgsChannel *source, *current;

  AgsConfig *config;
  
  GList *list;

  gchar *str;

  gboolean performance_mode;

  audio = AGS_MACHINE(matrix)->audio;

  if(matrix->mapped_input_pad > input_pad_start){
    return;
  }

  config = ags_config_get_instance();

  source = ags_channel_nth(audio->input,
			   input_pad_start * audio->audio_channels);

  /* map dependending on output */
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "engine-mode\0");
  performance_mode = FALSE;

  /* remap for input */
  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "performance\0",
			  12)){
    current = source;

    while(current != NULL){
      /* ags-copy */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-copy\0",
				0, audio->audio_channels, 
				current->pad, current->pad + 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);

      current = current->next_pad;
    }
    
    /* set performance mode */
    performance_mode = TRUE;
  }else{    
    current = source;

    while(current != NULL){
      /* ags-buffer */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-buffer\0",
				0, audio->audio_channels, 
				current->pad, current->pad + 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);

      current = current->next_pad;
    }
  }
  
  /*  */
  current = source;

  while(current != NULL){
    /* ags-play */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-play\0",
			      current->audio_channel, current->audio_channel + 1, 
			      current->pad, current->pad + 1,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_ADD),
			      0);

    current = current->next_pad;
  }

  /*  */
  current = source;

  while(current != NULL){
    /* ags-stream */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-stream\0",
			      current->audio_channel, current->audio_channel + 1, 
			      current->pad, current->pad + 1,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL | 
			       AGS_RECALL_FACTORY_ADD),
			      0);

    current = current->next_pad;
  }
  
  matrix->mapped_input_pad = audio->input_pads;
}

void
ags_matrix_output_map_recall(AgsMatrix *matrix, guint output_pad_start)
{
  AgsAudio *audio;
  AgsChannel *input, *current;

  AgsConfig *config;

  gchar *str;

  gboolean performance_mode;

  audio = AGS_MACHINE(matrix)->audio;

  if(matrix->mapped_output_pad > output_pad_start){
    return;
  }

  config = ags_config_get_instance();

  /* map dependending on output */
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "engine-mode\0");
  performance_mode = FALSE;

  /* remap for input */
  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "performance\0",
			  12)){
    input = audio->input;

    while(input != NULL){
      /* ags-copy */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-copy\0",
				0, audio->audio_channels, 
				input->pad, input->pad + 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);

      input = input->next_pad;
    }
    
    /* set performance mode */
    performance_mode = TRUE;
  }else{    
    input = audio->input;

    while(input != NULL){
      /* ags-buffer */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-buffer\0",
				0, audio->audio_channels, 
				input->pad, input->pad + 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);

      input = input->next_pad;
    }
  }

  if(!performance_mode){
    current = ags_channel_nth(audio->output,
			      output_pad_start * audio->audio_channels);

    while(current != NULL){
      /* ags-stream */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-stream\0",
				current->audio_channel, current->audio_channel + 1,
				current->pad, current->pad + 1,
				(AGS_RECALL_FACTORY_OUTPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_ADD),
				0);

      current = current->next;
    }
  }
  
  matrix->mapped_output_pad = audio->output_pads;
}

gchar*
ags_matrix_get_name(AgsPlugin *plugin)
{
  return(AGS_MATRIX(plugin)->name);
}

void
ags_matrix_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_MATRIX(plugin)->name = name;
}

gchar*
ags_matrix_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_MATRIX(plugin)->xml_type);
}

void
ags_matrix_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_MATRIX(plugin)->xml_type = xml_type;
}

void
ags_matrix_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsMatrix *gobject;
  AgsFileLookup *file_lookup;
  AgsFileLaunch *file_launch;
  GList *list;

  gobject = AGS_MATRIX(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
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
      g_signal_connect_after(G_OBJECT(file_lookup), "resolve\0",
			     G_CALLBACK(ags_matrix_read_resolve_audio), gobject);
      
      break;
    }

    list = list->next;
  }

  /* launch */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node\0", node,
					       "file\0", file,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_matrix_launch_task), gobject);
  ags_file_add_launch(file,
		      (GObject *) file_launch);
}

void
ags_matrix_read_resolve_audio(AgsFileLookup *file_lookup,
				AgsMachine *machine)
{
  AgsMatrix *matrix;

  matrix = AGS_MATRIX(machine);

  g_signal_connect_after(G_OBJECT(machine->audio), "set_audio_channels\0",
			 G_CALLBACK(ags_matrix_set_audio_channels), matrix);

  g_signal_connect_after(G_OBJECT(machine->audio), "set_pads\0",
			 G_CALLBACK(ags_matrix_set_pads), matrix);

  if((AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) == 0){
    ags_matrix_output_map_recall(matrix, 0);
    ags_matrix_input_map_recall(matrix, 0);
  }else{
    matrix->mapped_output_pad = machine->audio->output_pads;
    matrix->mapped_input_pad = machine->audio->input_pads;
  }
}

void
ags_matrix_launch_task(AgsFileLaunch *file_launch, AgsMatrix *matrix)
{
  xmlNode *node;
  guint64 length, index;

  node = file_launch->node;

  /* length */
  length = (gdouble) g_ascii_strtod(xmlGetProp(node,
					       "length\0"),
				    NULL);
  gtk_spin_button_set_value(matrix->length_spin,
			    length);

  /* loop */
  if(!g_strcmp0(xmlGetProp(node,
			   "loop\0"),
		AGS_FILE_TRUE)){
    gtk_toggle_button_set_active((GtkToggleButton *) matrix->loop_button,
				 TRUE);
  }

  /* index */
  index = g_ascii_strtoull(xmlGetProp(node,
				      "bank-index-0\0"),
			   NULL,
			   10);

  if(index != 0){
    gtk_toggle_button_set_active(matrix->index[0],
				 FALSE);
    gtk_toggle_button_set_active(matrix->index[index],
				 TRUE);
    matrix->selected = matrix->index[index];
  }
}

xmlNode*
ags_matrix_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsMatrix *matrix;
  xmlNode *node;
  GList *list;
  gchar *id;
  guint i;
  gint history;

  matrix = AGS_MATRIX(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-matrix\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", matrix,
				   NULL));

  xmlNewProp(node,
	     "length\0",
	     g_strdup_printf("%d\0", (gint) gtk_spin_button_get_value(matrix->length_spin)));

  for(i = 0; matrix->selected != matrix->index[i]; i++);

  xmlNewProp(node,
	     "bank-index-0\0",
	     g_strdup_printf("%d\0", i));

  xmlNewProp(node,
	     "loop\0",
	     g_strdup_printf("%s\0", ((gtk_toggle_button_get_active((GtkToggleButton *) matrix->loop_button)) ? AGS_FILE_TRUE: AGS_FILE_FALSE)));

  xmlAddChild(parent,
	      node);

  return(node);
}

/**
 * ags_matrix_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsMatrix
 *
 * Returns: a new #AgsMatrix
 *
 * Since: 0.3
 */
AgsMatrix*
ags_matrix_new(GObject *soundcard)
{
  AgsMatrix *matrix;

  matrix = (AgsMatrix *) g_object_new(AGS_TYPE_MATRIX,
				      NULL);
  
  g_object_set(AGS_MACHINE(matrix)->audio,
	       "soundcard\0", soundcard,
	       NULL);
  
  return(matrix);
}
