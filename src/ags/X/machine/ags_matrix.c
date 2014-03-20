/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_matrix_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_portlet.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_loop_channel.h>
#include <ags/audio/recall/ags_loop_channel_run.h>
#include <ags/audio/recall/ags_copy_channel.h>
#include <ags/audio/recall/ags_copy_channel_run.h>
#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_stream_channel_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>
#include <ags/audio/recall/ags_play_notation_audio_run.h>

#include <ags/widget/ags_led.h>

#include <ags/X/ags_menu_bar.h>

#include <math.h>

#define AGS_MATRIX_INPUT_LINE_MAPPED_KEY "AGS_MATRIX_INPUT_LINE_MAPPED_KEY"
#define AGS_MATRIX_INPUT_LINE_MAPPED_DATA "AGS_MATRIX_INPUT_LINE_MAPPED_DATA"

#define AGS_MATRIX_CELL_WIDTH   12
#define AGS_MATRIX_CELL_HEIGHT  10

void ags_matrix_class_init(AgsMatrixClass *matrix);
void ags_matrix_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_matrix_init(AgsMatrix *matrix);
void ags_matrix_finalize(GObject *gobject);
void ags_matrix_connect(AgsConnectable *connectable);
void ags_matrix_disconnect(AgsConnectable *connectable);
void ags_matrix_show(GtkWidget *widget);

void ags_matrix_set_audio_channels(AgsAudio *audio,
				   guint audio_channels, guint audio_channels_old,
				   gpointer data);
void ags_matrix_set_pads(AgsAudio *audio, GType type,
			 guint pads, guint pads_old,
			 gpointer data);

static gpointer ags_matrix_parent_class = NULL;

static AgsConnectableInterface *ags_matrix_parent_connectable_interface;

GtkStyle *matrix_style;

extern const char *AGS_COPY_PATTERN;
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
    
    ags_type_matrix = g_type_register_static(AGS_TYPE_MACHINE,
					    "AgsMatrix\0", &ags_matrix_info,
					    0);
    
    g_type_add_interface_static(ags_type_matrix,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
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

  /* GtkWidget */
  widget = (GtkWidgetClass *) matrix;

  //  widget->show = ags_matrix_show;

  /* AgsMachine */
  machine = (AgsMachineClass *) matrix;

  //  machine->read_file = ags_file_read_matrix;
  //  machine->write_file = ags_file_write_matrix;
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
ags_matrix_init(AgsMatrix *matrix)
{
  GtkFrame *frame;
  GtkTable *table;
  GtkToggleButton *button;
  GtkVScrollbar *vscrollbar;
  AgsLed *led;
  GtkVBox *vbox;
  GtkHBox *hbox;

  AgsAudio *audio;

  AgsDelayAudio *play_delay_audio;
  AgsDelayAudioRun *play_delay_audio_run;
  AgsCountBeatsAudio *play_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;
  AgsCopyPatternAudio *recall_copy_pattern_audio;
  AgsCopyPatternAudioRun *recall_copy_pattern_audio_run;

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
		   AGS_AUDIO_HAS_NOTATION);
  //  audio->audio_channels = 1;

  AGS_MACHINE(matrix)->flags |= AGS_MACHINE_IS_SEQUENCER;
  matrix->flags = 0;

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

  list = ags_recall_find_type(audio->play, AGS_TYPE_DELAY_AUDIO_RUN);

  if(list != NULL){
    play_delay_audio_run = AGS_DELAY_AUDIO_RUN(list->data);
    AGS_RECALL(play_delay_audio_run)->flags |= AGS_RECALL_PERSISTENT;
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
  
  list = ags_recall_find_type(audio->play, AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

  if(list != NULL){
    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);

    /* set dependency */  
    g_object_set(G_OBJECT(play_count_beats_audio_run),
		 "delay-audio-run\0", play_delay_audio_run,
		 NULL);
  }

  /* ags-copy-pattern */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-copy-pattern\0",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_RECALL),
			    0);

  list = ags_recall_find_type(audio->recall, AGS_TYPE_COPY_PATTERN_AUDIO_RUN);

  if(list != NULL){
    recall_copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(list->data);

    /* set dependency */
    g_object_set(G_OBJECT(recall_copy_pattern_audio_run),
		 "count-beats-audio-run\0", play_count_beats_audio_run,
		 NULL);
  }

  /* create widgets */
  frame = (GtkFrame *) (gtk_container_get_children((GtkContainer *) matrix))->data;

  matrix->table = (GtkTable *) gtk_table_new(4, 4, FALSE);
  gtk_container_add((GtkContainer *) frame, (GtkWidget *) matrix->table);

  matrix->run = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup("run\0"));
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
      matrix->index[i * 3 + j] =
	button = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup_printf("%d\0", i * 3 + j +1));
      g_object_set_data((GObject *) button, AGS_MATRIX_INDEX, GUINT_TO_POINTER(i * 3 + j));
      gtk_table_attach(table, (GtkWidget *) button,
		       j, j +1, i, i +1,
		       GTK_FILL, GTK_FILL,
		       0, 0);
    }
  }

  matrix->selected = matrix->index[0];
  gtk_toggle_button_set_active(matrix->selected, TRUE);

  /* sequencer */
  table = (GtkTable *) gtk_table_new(2, 2, FALSE);
  gtk_table_attach(matrix->table, (GtkWidget *) table,
		   2, 3, 0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  matrix->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_size_request((GtkWidget *) matrix->drawing_area, 32 * AGS_MATRIX_CELL_WIDTH +1, 8 * AGS_MATRIX_CELL_HEIGHT +1);
  gtk_widget_set_style((GtkWidget *) matrix->drawing_area, matrix_style);
  gtk_table_attach(table, (GtkWidget *) matrix->drawing_area,
		   0, 1, 0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_widget_set_events ((GtkWidget *) matrix->drawing_area,
                         GDK_EXPOSURE_MASK
                         | GDK_LEAVE_NOTIFY_MASK
                         | GDK_BUTTON_PRESS_MASK
                         | GDK_POINTER_MOTION_MASK
                         | GDK_POINTER_MOTION_HINT_MASK);

  matrix->adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 77.0, 1.0, 1.0, 8.0);

  vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(matrix->adjustment);
  gtk_widget_set_style((GtkWidget *) vscrollbar, matrix_style);
  gtk_table_attach(table, (GtkWidget *) vscrollbar,
		   1, 2, 0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* led */
  matrix->active_led = 0;

  matrix->led =
    hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_table_attach(matrix->table, (GtkWidget *) hbox,
		   2, 3, 1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  for(i = 0; i < 32; i++){
    led = ags_led_new();
    gtk_widget_set_size_request((GtkWidget *) led, AGS_MATRIX_CELL_WIDTH, AGS_MATRIX_CELL_WIDTH / 2);
    gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) led, FALSE, FALSE, 0);
  }

  /*  */
  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_table_attach(matrix->table, (GtkWidget *) vbox,
		   3, 4, 0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) hbox, FALSE, FALSE, 0);

  gtk_box_pack_start((GtkBox *) hbox, gtk_label_new(g_strdup("length\0")), FALSE, FALSE, 0);

  matrix->length_spin = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 32.0, 1.0);
  matrix->length_spin->adjustment->value = 16.0;
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) matrix->length_spin, FALSE, FALSE, 0);

  matrix->tact = (GtkOptionMenu *) gtk_option_menu_new();
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) matrix->tact, FALSE, FALSE, 0);

  gtk_option_menu_set_menu(matrix->tact, (GtkWidget *) ags_tact_menu_new());
  gtk_option_menu_set_history(matrix->tact, 4);

  matrix->loop_button = (GtkCheckButton *) gtk_check_button_new_with_label(g_strdup("loop\0"));
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) matrix->loop_button, FALSE, FALSE, 0);
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
  AgsDelayAudioRun *play_delay_audio_run;
  AgsRecallHandler *recall_handler;
  GList *list;
  int i;

  ags_matrix_parent_connectable_interface->connect(connectable);

  matrix = AGS_MATRIX(connectable);

  /* recalls */
  list = ags_recall_template_find_type(AGS_AUDIO(AGS_MACHINE(matrix)->audio)->play, AGS_TYPE_DELAY_AUDIO_RUN);

  if(list != NULL){
    play_delay_audio_run = AGS_DELAY_AUDIO_RUN(list->data);

    recall_handler = (AgsRecallHandler *) malloc(sizeof(AgsRecallHandler));

    recall_handler->signal_name = "sequencer-count\0";
    recall_handler->callback = G_CALLBACK(ags_matrix_sequencer_count_callback);
    recall_handler->data = (gpointer) matrix;

    //TODO:JK: uncomment me
    ags_recall_add_handler(AGS_RECALL(play_delay_audio_run), recall_handler);
  }

  /* AgsMatrix */
  g_signal_connect(G_OBJECT(matrix->run), "clicked\0",
		   G_CALLBACK(ags_matrix_run_callback), (gpointer) matrix);

  for(i  = 0; i < 9; i++){
    g_signal_connect (G_OBJECT (matrix->index[i]), "clicked\0",
		      G_CALLBACK (ags_matrix_index_callback), (gpointer) matrix);
  }

  g_signal_connect (G_OBJECT (matrix->drawing_area), "expose_event\0",
                    G_CALLBACK (ags_matrix_drawing_area_expose_callback), (gpointer) matrix);

  g_signal_connect (G_OBJECT (matrix->drawing_area), "button_press_event\0",
                    G_CALLBACK (ags_matrix_drawing_area_button_press_callback), (gpointer) matrix);

  g_signal_connect (G_OBJECT (matrix->adjustment), "value_changed\0",
                    G_CALLBACK (ags_matrix_adjustment_value_changed_callback), (gpointer) matrix);

  g_signal_connect_after((GObject *) matrix->length_spin, "value-changed\0",
			 G_CALLBACK(ags_matrix_length_spin_callback), (gpointer) matrix);

  g_signal_connect((GObject *) matrix->tact, "changed\0",
		   G_CALLBACK(ags_matrix_tact_callback), (gpointer) matrix);

  g_signal_connect((GObject *) matrix->loop_button, "clicked\0",
		   G_CALLBACK(ags_matrix_loop_button_callback), (gpointer) matrix);

  /* AgsAudio */
  g_signal_connect_after(G_OBJECT(matrix->machine.audio), "set_audio_channels\0",
			 G_CALLBACK(ags_matrix_set_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(matrix->machine.audio), "set_pads\0",
			 G_CALLBACK(ags_matrix_set_pads), NULL);
}

void
ags_matrix_disconnect(AgsConnectable *connectable)
{
  AgsMatrix *matrix;

  ags_matrix_parent_connectable_interface->disconnect(connectable);

  /* AgsMatrix */
  matrix = AGS_MATRIX(connectable);

  //TODO:JK: implement me
}

void
ags_matrix_show(GtkWidget *widget)
{
}

void
ags_matrix_set_audio_channels(AgsAudio *audio,
			      guint audio_channels, guint audio_channels_old,
			      gpointer data)
{
  printf("AgsMatrix only pads can be adjusted\n\0");
  //  _ags_audio_set_audio_channels(audio, audio_channels);
}

void
ags_matrix_set_pads(AgsAudio *audio, GType type,
		    guint pads, guint pads_old,
		    gpointer data)
{
  AgsMatrix *matrix;
  AgsChannel *channel, *source;
  AgsAudioSignal *audio_signal;
  guint i, j;
  gboolean grow;

  if(type == AGS_TYPE_INPUT && pads < 8){
    printf("AgsMatrix minimum input pad count 8\n\0");
    pads = 8;
  }

  matrix = (AgsMatrix *) audio->machine;

  if(pads_old == pads)
    return;
  if(pads_old < pads)
    grow = TRUE;
  else
    grow = FALSE;

  if(type == AGS_TYPE_INPUT){
    AgsPlayNotationAudioRun  *play_notation;
    AgsCopyPatternChannel *copy_pattern_channel;
    AgsPattern *pattern;
    GList *list, *notation;

    list = audio->recall;

    while((list = ags_recall_find_type(list,
				       AGS_TYPE_PLAY_NOTATION_AUDIO_RUN)) != NULL){
      play_notation = AGS_PLAY_NOTATION_AUDIO_RUN(list->data);

      notation = audio->notation;
	
      while(notation != NULL){
	g_object_set(G_OBJECT(play_notation),
		     "notation\0", notation->data,
		     NULL);
	
	notation = notation->next;
      }

      list = list->next;
    }

    if(grow){
      /* ags-copy-pattern */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-copy-pattern\0",
				0, audio->audio_channels,
				pads_old, pads,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_REMAP |
				 AGS_RECALL_FACTORY_RECALL),
				0);
      /* create pattern */
      source = ags_channel_nth(audio->input, pads_old);

      while(source != NULL){
	source->pattern = g_list_alloc();
	source->pattern->data = (gpointer) ags_pattern_new();
	ags_pattern_set_dim((AgsPattern *) source->pattern->data, 1, 9, 32);
	
	source = source->next;
      }

      /* set pattern object on port */
      channel = ags_channel_pad_nth(audio->input, pads_old);
      
      for(i = pads_old; i < pads; i++){
	for(j = 0; j < audio->audio_channels; j++){
	  list = ags_recall_template_find_type(channel->recall, AGS_TYPE_COPY_PATTERN_CHANNEL);
	  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(list->data);

	  list = channel->pattern;
	  pattern = AGS_PATTERN(list->data);

	  copy_pattern_channel->pattern->port_value.ags_port_object = (GObject *) pattern;
	  
	  ags_portlet_set_port(AGS_PORTLET(pattern), copy_pattern_channel->pattern);
	  
	  channel = channel->next;
	}
      }
    }

    /* depending on destination */
    ags_matrix_input_map_recall(matrix, 0);
  }else{
    if(grow){
      AgsChannel *current, *output;
      AgsDelayAudio *delay_audio;
      GList *recall;
      GList *list;
      guint stop;

      source = ags_channel_nth(audio->output, pads_old);

      if(source != NULL){
	AgsDevout *devout;
	AgsAudioSignal *audio_signal;
	guint stop;
	
	devout = AGS_DEVOUT(AGS_AUDIO(source->audio)->devout);
	
	stop = (guint) ceil(16.0 * AGS_DEVOUT_DEFAULT_DELAY * exp2(8.0 - 4.0) + 1.0);
	
	audio_signal = ags_audio_signal_new(devout,
					    source->first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_audio_signal_stream_resize(audio_signal,
				   stop);
	ags_recycling_add_audio_signal(source->first_recycling,
				       audio_signal);

	/* depending on destination */
       	ags_matrix_output_map_recall(matrix, 0);


	/* map recalls */
	ags_matrix_output_map_recall(matrix, pads_old);

	output = audio->output;

	while(output != NULL){
	  current = ags_channel_nth(audio->input,
				    output->audio_channel);

	  while(current != NULL){
	    recall = current->play;

	    while(recall != NULL){
	      if(AGS_IS_RECALL_CHANNEL_RUN(recall->data)){
		g_object_set(G_OBJECT(recall->data),
			     "destination\0", output,
			     NULL);
		ags_connectable_connect(AGS_CONNECTABLE(recall->data));
	      }

	      recall = recall->next;
	    }

	    recall = current->recall;

	    while(recall != NULL){
	      if(AGS_IS_RECALL_CHANNEL_RUN(recall->data)){
		g_object_set(G_OBJECT(recall->data),
			     "destination\0", output,
			     NULL);

		ags_connectable_connect(AGS_CONNECTABLE(recall->data));
	      }

	      recall = recall->next;
	    }

	    current = current->next_pad;
	  }

	  output = output->next;
	}
      }      
    }
  }
}

void
ags_matrix_input_map_recall(AgsMatrix *matrix, guint input_pad_start)
{
  AgsAudio *audio;
  AgsChannel *source, *current, *destination;
  AgsCopyChannel *copy_channel;
  AgsCopyChannelRun *copy_channel_run;

  GList *list;

  audio = AGS_MACHINE(matrix)->audio;

  source = ags_channel_nth(audio->input,
			   input_pad_start * audio->audio_channels);

  current = source;

  while(current != NULL){
    /* ags-copy */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-copy\0",
			      current->audio_channel, current->audio_channel + 1, 
			      current->pad, current->pad + 1,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);


    destination = ags_channel_nth(audio->output,
				  current->audio_channel);

    while(destination != NULL){
      /* recall */
      list = current->recall;

      while((list = ags_recall_find_type(list, AGS_TYPE_COPY_CHANNEL)) != NULL){
	copy_channel = AGS_COPY_CHANNEL(list->data);

	g_object_set(G_OBJECT(copy_channel),
		     "destination\0", destination,
		     NULL);

	list = list->next;
      }

      list = current->recall;
    
      while((list = ags_recall_find_type(list, AGS_TYPE_COPY_CHANNEL_RUN)) != NULL){
	copy_channel_run = AGS_COPY_CHANNEL_RUN(list->data);

	g_object_set(G_OBJECT(copy_channel_run),
		     "destination\0", destination,
		     NULL);

	list = list->next;
      }

      destination = destination->next_pad;
    }

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
}

void
ags_matrix_output_map_recall(AgsMatrix *matrix, guint output_pad_start)
{
  AgsAudio *audio;
  AgsChannel *source;

  AgsDelayAudio *recall_delay_audio;
  AgsCountBeatsAudioRun *recall_count_beats_audio_run;
  AgsLoopChannel *recall_loop_channel;
  AgsLoopChannelRun *recall_loop_channel_run;

  GList *list;
  
  audio = AGS_MACHINE(matrix)->audio;

  source = ags_channel_nth(audio->output,
			   output_pad_start * audio->audio_channels);

  /* get some recalls */
  list = ags_recall_find_type(audio->play, AGS_TYPE_DELAY_AUDIO);

  if(list != NULL){
    recall_delay_audio = AGS_DELAY_AUDIO(list->data);
  }else{
    recall_delay_audio = NULL;
  }

  list = ags_recall_find_type(audio->play, AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

  if(list != NULL){
    recall_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);
  }else{
    recall_count_beats_audio_run = NULL;
  }

  /* ags-loop */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-loop\0",
			    source->audio_channel, source->audio_channel + 1,
			    output_pad_start, audio->output_pads,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_PLAY | 
			     AGS_RECALL_FACTORY_ADD),
			    0);

  list = ags_recall_find_type(source->play, AGS_TYPE_LOOP_CHANNEL);

  if(list != NULL){
    recall_loop_channel = AGS_LOOP_CHANNEL(list->data);

    /* set dependency */
    g_object_set(G_OBJECT(recall_loop_channel),
		 "delay-audio\0", recall_delay_audio,
		 NULL);
  }else{
    recall_loop_channel = NULL;
  }

  list = ags_recall_find_type(source->play, AGS_TYPE_LOOP_CHANNEL_RUN);

  if(list != NULL){
    recall_loop_channel_run = AGS_LOOP_CHANNEL_RUN(list->data);

    /* set dependency */
    g_object_set(G_OBJECT(recall_loop_channel_run),
		 "count-beats-audio-run\0", recall_count_beats_audio_run,
		 NULL);
  }else{
    recall_loop_channel_run = NULL;
  }

  /* ags-stream */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-stream\0",
			    source->audio_channel, source->audio_channel + 1,
			    output_pad_start, audio->output_pads,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL | 
			     AGS_RECALL_FACTORY_ADD),
			    0);
}

void
ags_matrix_draw_gutter(AgsMatrix *matrix)
{
  AgsChannel *channel;
  int i, j;

  gdk_draw_rectangle (GTK_WIDGET (matrix->drawing_area)->window,
                      GTK_WIDGET (matrix->drawing_area)->style->bg_gc[0],
                      TRUE,
                      0, 0,
                      288, 80);

  channel = ags_channel_nth(matrix->machine.audio->input, (guint) matrix->adjustment->value);

  for (i = 0; i < 8; i++){
    for (j = 0; j < 32; j++){
      gdk_draw_rectangle (GTK_WIDGET (matrix->drawing_area)->window,
                          GTK_WIDGET (matrix->drawing_area)->style->fg_gc[0],
                          FALSE,
                          j * 12, i * 10,
                          12, 10);

      ags_matrix_redraw_gutter_point (matrix, channel, j, i);
    }

    channel = channel->next;
  }
}

void
ags_matrix_draw_matrix(AgsMatrix *matrix)
{
  AgsChannel *channel;
  int i, j;

  channel = ags_channel_nth(matrix->machine.audio->input, (guint) matrix->adjustment->value);

  for (i = 0; i < 8; i++){
    for (j = 0; j < 32; j++)
      ags_matrix_redraw_gutter_point (matrix, channel, j, i);

    channel = channel->next;
  }
}

void
ags_matrix_redraw_gutter_point (AgsMatrix *matrix, AgsChannel *channel, guint j, guint i)
{
  if(channel->pattern == NULL)
    return;

  if(ags_pattern_get_bit((AgsPattern *) channel->pattern->data, 0, strtol(matrix->selected->button.label_text, NULL, 10) - 1, j))
    ags_matrix_highlight_gutter_point(matrix, j, i);
  else
    ags_matrix_unpaint_gutter_point(matrix, j, i);
}

void
ags_matrix_highlight_gutter_point(AgsMatrix *matrix, guint j, guint i)
{
  gdk_draw_rectangle (GTK_WIDGET (matrix->drawing_area)->window,
		      GTK_WIDGET (matrix->drawing_area)->style->fg_gc[0],
		      TRUE,
		      j * 12 +1, i * 10 +1,
		      11, 9);
}

void
ags_matrix_unpaint_gutter_point(AgsMatrix *matrix, guint j, guint i)
{
  gdk_draw_rectangle (GTK_WIDGET (matrix->drawing_area)->window,
		      GTK_WIDGET (matrix->drawing_area)->style->bg_gc[0],
		      TRUE,
		      j * 12 +1, i * 10 +1,
		      11, 9);
}

AgsMatrix*
ags_matrix_new(GObject *devout)
{
  AgsMatrix *matrix;
  GValue value = {0,};

  matrix = (AgsMatrix *) g_object_new(AGS_TYPE_MATRIX,
				      NULL);

  if(devout != NULL){
    g_value_init(&value, G_TYPE_OBJECT);
    g_value_set_object(&value, devout);
    g_object_set_property(G_OBJECT(matrix->machine.audio),
			  "devout\0", &value);
    g_value_unset(&value);
  }

  return(matrix);
}
