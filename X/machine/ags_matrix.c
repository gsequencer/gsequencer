#include "ags_matrix.h"
#include "ags_matrix_callbacks.h"

#include "../../audio/ags_audio.h"
#include "../../audio/ags_channel.h"
#include "../../audio/ags_input.h"
#include "../../audio/ags_output.h"
#include "../../audio/ags_pattern.h"
#include "../../audio/ags_recall.h"

#include "../../audio/recall/ags_delay.h"
#include "../../audio/recall/ags_play_channel.h"
#include "../../audio/recall/ags_copy_pattern.h"

#define AGS_MATRIX_CELL_WIDTH   12
#define AGS_MATRIX_CELL_HEIGHT  10

GType ags_matrix_get_type(void);
void ags_matrix_class_init(AgsMatrixClass *matrix);
void ags_matrix_init(AgsMatrix *matrix);
void ags_matrix_connect(AgsMatrix *matrix);
void ags_matrix_destroy(GtkObject *object);
void ags_matrix_show(GtkWidget *widget);

void ags_matrix_set_audio_channels(AgsAudio *audio, guint audio_channels);
void ags_matrix_set_pads(AgsAudio *audio, GType type, guint pads);
void ags_matrix_set_lines(AgsAudio *audio, GType type, guint lines);

extern void ags_file_read_matrix(AgsFile *file, AgsMachine *machine);
extern void ags_file_write_matrix(AgsFile *file, AgsMachine *machine);

GtkStyle *matrix_style;

extern const char *AGS_COPY_PATTERN;
const char *AGS_MATRIX_INDEX = "AgsMatrixIndex\0";

GType
ags_matrix_get_type(void)
{
  static GType matrix_type = 0;

  if (!matrix_type){
    static const GtkTypeInfo matrix_info = {
      "AgsMatrix\0",
      sizeof(AgsMatrix), /* base_init */
      sizeof(AgsMatrixClass), /* base_finalize */
      (GtkClassInitFunc) ags_matrix_class_init,
      (GtkObjectInitFunc) ags_matrix_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    matrix_type = gtk_type_unique (AGS_TYPE_MACHINE, &matrix_info);
  }

  return (matrix_type);
}

void
ags_matrix_class_init(AgsMatrixClass *matrix)
{
  GtkObjectClass *object = (GtkObjectClass *) matrix;
  GtkWidgetClass *widget = (GtkWidgetClass *) matrix;
  AgsMachineClass *machine = (AgsMachineClass *) matrix;

  //  object->destroy = ags_matrix_destroy;

  //  widget->show = ags_matrix_show;

  //  machine->read_file = ags_file_read_matrix;
  //  machine->write_file = ags_file_write_matrix;
}

void
ags_matrix_init(AgsMatrix *matrix)
{
  AgsChannel *channel;
  AgsDelay *delay;
  GtkFrame *frame;
  GtkTable *table;
  GtkToggleButton *button;
  GtkVScrollbar *vscrollbar;
  GtkVBox *vbox;
  GtkHBox *hbox;
  int i, j;

  g_signal_connect_after((GObject *) matrix, "parent_set\0",
			 G_CALLBACK(ags_matrix_parent_set_callback), (gpointer) matrix);


  AGS_AUDIO_GET_CLASS(matrix->machine.audio)->set_audio_channels = ags_matrix_set_audio_channels;
  AGS_AUDIO_GET_CLASS(matrix->machine.audio)->set_pads = ags_matrix_set_pads;
  AGS_AUDIO_GET_CLASS(matrix->machine.audio)->set_lines = ags_matrix_set_lines;

  matrix->machine.audio->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING | AGS_AUDIO_SYNC | AGS_AUDIO_ASYNC | AGS_AUDIO_HAS_NOTATION);
  matrix->machine.audio->audio_channels = 1;

  matrix->flags = 0;

  /* create an AgsDelaySharedAudio */
  matrix->delay_shared_audio = ags_delay_shared_audio_new(0);

  /* create an AgsDelay in audio->play */
  delay = ags_delay_new(matrix->delay_shared_audio);
  delay->recall.flags |= AGS_RECALL_TEMPLATE;

  ags_delay_connect(delay);
  g_signal_connect((GObject *) delay, "done\0",
		   G_CALLBACK(ags_matrix_run_delay_done), matrix);

  matrix->machine.audio->play = g_list_prepend(matrix->machine.audio->play, (gpointer) delay);

  /* create an AgsDelay in audio->recall */
  delay = ags_delay_new(matrix->delay_shared_audio);
  delay->recall.flags = AGS_RECALL_TEMPLATE;

  ags_delay_connect(delay);
  g_signal_connect((GObject *) delay, "done\0",
		   G_CALLBACK(ags_matrix_run_delay_done), matrix);

  matrix->machine.audio->recall = g_list_prepend(matrix->machine.audio->recall, (gpointer) delay);

  /* create an AgsCopyPatternSharedData */
  matrix->copy_pattern_shared_audio = ags_copy_pattern_shared_audio_new(NULL,
									0, 0,
									16, FALSE,
									0);

  matrix->copy_pattern_shared_audio_run = ags_copy_pattern_shared_audio_run_new(delay, 0);

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
		   1, 2, 0, 1,
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

  matrix->tic = (GtkOptionMenu *) gtk_option_menu_new();
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) matrix->tic, FALSE, FALSE, 0);

  gtk_option_menu_set_menu(matrix->tic, (GtkWidget *) ags_tic_menu_new());
  gtk_option_menu_set_history(matrix->tic, 6);

  matrix->loop_button = (GtkCheckButton *) gtk_check_button_new_with_label(g_strdup("loop\0"));
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) matrix->loop_button, FALSE, FALSE, 0);
}

void
ags_matrix_connect(AgsMatrix *matrix)
{
  int i;

  g_signal_connect((GObject *) matrix, "destroy\0",
		   G_CALLBACK(ags_matrix_destroy_callback), (gpointer) matrix);

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

  g_signal_connect((GObject *) matrix->tic, "changed\0",
		   G_CALLBACK(ags_matrix_tic_callback), (gpointer) matrix);

  g_signal_connect((GObject *) matrix->loop_button, "clicked\0",
		   G_CALLBACK(ags_matrix_loop_button_callback), (gpointer) matrix);
}

void
ags_matrix_destroy(GtkObject *object)
{
}

void
ags_matrix_show(GtkWidget *widget)
{
}

void
ags_matrix_set_audio_channels(AgsAudio *audio, guint audio_channels)
{
  printf("AgsMatrix only pads can be adjusted\n\0");
  //  _ags_audio_set_audio_channels(audio, audio_channels);
}

void
ags_matrix_set_pads(AgsAudio *audio, GType type, guint pads)
{
  AgsMatrix *matrix;
  AgsChannel *channel, *destination;
  AgsAudioSignal *audio_signal;
  AgsDelaySharedAudio *delay_shared_audio;
  AgsCopyPatternSharedChannel *copy_pattern_shared_channel;
  guint pads_old;
  guint stream_length;

  pads_old = (type == AGS_TYPE_OUTPUT) ? audio->output_pads: audio->input_pads;

  if(pads == pads_old)
    return;

  if(type == AGS_TYPE_INPUT && pads < 8){
    printf("AgsMatrix minimum input pad count 8\n\0");
    pads = 8;
  }

  ags_audio_real_set_pads(audio, type, pads);

  matrix = (AgsMatrix *) audio->machine;
  delay_shared_audio = matrix->delay_shared_audio;

  stream_length = ((guint)matrix->length_spin->adjustment->value) * (delay_shared_audio->delay + 1);

  if(type == AGS_TYPE_INPUT){
    AgsCopyPattern *copy_pattern;

    if(pads_old < pads){
      channel = ags_channel_nth(audio->input, pads_old);
      //delay->recall_ref = audio->input_lines;

      while(channel != NULL){
	channel->pattern = g_list_alloc();
	channel->pattern->data = (gpointer) ags_pattern_new();
	ags_pattern_set_dim((AgsPattern *) channel->pattern->data, 1, 9, 32);

	destination = ags_channel_nth(audio->output, channel->audio_channel);

	while(destination != NULL){
	  /* AgsCopyPattern */
	  copy_pattern_shared_channel = ags_copy_pattern_shared_channel_new(destination,
									    channel, (AgsPattern *) channel->pattern->data);

	  copy_pattern = ags_copy_pattern_new(matrix->copy_pattern_shared_audio,
					      matrix->copy_pattern_shared_audio_run,
					      copy_pattern_shared_channel);

	  copy_pattern->recall.flags |= AGS_RECALL_TEMPLATE;

	  //	  ags_copy_pattern_connect(copy_pattern);
	  
	  g_signal_connect((GObject *) copy_pattern, "done\0",
			   G_CALLBACK(ags_matrix_copy_pattern_done), matrix);
	
	  g_signal_connect((GObject *) copy_pattern, "cancel\0",
			   G_CALLBACK(ags_matrix_copy_pattern_cancel), matrix);
	  
	  g_signal_connect((GObject *) copy_pattern, "loop\0",
			   G_CALLBACK(ags_matrix_copy_pattern_loop), matrix);

	  channel->recall = g_list_append(channel->recall, copy_pattern);

	  destination = destination->next_pad;
	}

	channel = channel->next;
      }
    }else if(pads_old > pads){
    }
  }else{
    AgsPlayChannel *play_channel;

    if(pads_old < pads){
      channel = ags_channel_nth(audio->output, pads_old);

      while(channel != NULL){
	audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
	ags_audio_signal_stream_resize(audio_signal, stream_length);

	/* AgsPlayChannel * /
	play_channel = ags_play_channel_new(channel);

	ags_play_channel_connect(play_channel);

	g_signal_connect((GObject *) play_channel, "done\0",
			 G_CALLBACK(ags_matrix_play_done), matrix);
	
	g_signal_connect((GObject *) play_channel, "cancel\0",
			 G_CALLBACK(ags_matrix_play_cancel), matrix);

	channel->play = g_list_append(channel->play, play_channel);
	*/

	channel = channel->next;
      }
    }else if(pads_old > pads){
    }
  }
}

void
ags_matrix_set_lines(AgsAudio *audio, GType type, guint lines)
{
  ags_audio_real_set_lines(audio, type, lines);
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
  if(ags_pattern_get_bit((AgsPattern *) channel->pattern->data, 0, strtol(matrix->selected->button.label_text, NULL, 10) -1, j))
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
ags_matrix_new()
{
  AgsMatrix *matrix;

  matrix = (AgsMatrix *) g_object_new(AGS_TYPE_MATRIX, NULL);

  return(matrix);
}
