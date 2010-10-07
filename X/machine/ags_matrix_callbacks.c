#include "ags_matrix_callbacks.h"

#include "../ags_window.h"
#include "../ags_navigation.h"

#include "../../audio/ags_channel.h"
#include "../../audio/ags_recycling.h"
#include "../../audio/ags_pattern.h"
#include "../../audio/ags_recall.h"

#include "../../audio/recall/ags_delay_audio.h"
#include "../../audio/recall/ags_delay_audio_run.h"
#include "../../audio/recall/ags_copy_pattern_audio.h"
#include "../../audio/recall/ags_copy_pattern_audio_run.h"
#include "../../audio/recall/ags_copy_pattern_channel.h"
#include "../../audio/recall/ags_copy_pattern_channel_run.h"

#include <math.h>

extern const char *AGS_MATRIX_INDEX;

void
ags_matrix_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMatrix *matrix)
{
  AgsWindow *window;
  AgsDelayAudio *delay_audio;
  AgsCopyPatternAudio *copy_pattern_audio;
  double tic;

  if(old_parent != NULL)
    return;

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);
  matrix->machine.name = g_strdup_printf("Default %d\0", window->counter->matrix);
  window->counter->matrix++;

  /* AgsDelay related */
  delay_audio = matrix->delay_audio;

  tic = exp2(4.0 - (double) gtk_option_menu_get_history((GtkOptionMenu *) matrix->tic));
  delay_audio->delay = (guint) round(((double)window->devout->frequency / (double)window->devout->buffer_size) * (60.0 / gtk_adjustment_get_value(window->navigation->bpm->adjustment)) * tic);

  /* AgsCopyPattern related */
  copy_pattern_audio = matrix->copy_pattern_audio;

  copy_pattern_audio->devout = window->devout;

  copy_pattern_audio->stream_length = (guint)matrix->length_spin->adjustment->value * (guint)(delay_audio->delay + 1) + 1;

  fprintf(stdout, "ags_matrix_parent_set_callback: delay_audio->delay = %d\n\0", delay_audio->delay);
}

gboolean
ags_matrix_destroy_callback(GtkObject *object, AgsMatrix *matrix)
{
  ags_matrix_destroy(object);

  return(TRUE);
}

void
ags_matrix_run_callback(GtkWidget *toggle_button, AgsMatrix *matrix)
{
  AgsDevout *devout;
  guint group_id;
  GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  devout = AGS_DEVOUT(matrix->machine.audio->devout);

  if(GTK_TOGGLE_BUTTON(toggle_button)->active){
    matrix->machine.audio->devout_play->flags &= (~AGS_DEVOUT_PLAY_REMOVE);

    /* do init stuff here */
    group_id = ags_audio_recursive_play_init(matrix->machine.audio);
    matrix->machine.audio->devout_play->group_id = group_id;

    g_static_mutex_lock(&mutex);
    devout->play_audio_ref++;
    devout->play_audio = g_list_append(devout->play_audio, matrix->machine.audio->devout_play);
    g_static_mutex_unlock(&mutex);

    if((AGS_DEVOUT_PLAY_AUDIO & devout->flags) == 0)
      devout->flags |= AGS_DEVOUT_PLAY_AUDIO;

    AGS_DEVOUT_GET_CLASS(devout)->run(devout);
  }else{
    if((AGS_DEVOUT_PLAY_DONE & (matrix->machine.audio->devout_play->flags)) == 0)
      matrix->machine.audio->devout_play->flags |= AGS_DEVOUT_PLAY_CANCEL;
    else{
      //      AgsDelay *delay;

      matrix->machine.audio->devout_play->flags |= AGS_DEVOUT_PLAY_REMOVE;
      matrix->machine.audio->devout_play->flags &= (~AGS_DEVOUT_PLAY_DONE);

      //      delay = AGS_DELAY(ags_recall_find_by_effect(matrix->machine.audio->play, (char *) g_type_name(AGS_TYPE_DELAY))->data);
      //      delay->recall_ref = matrix->machine.audio->input_lines;
    }
  }
}

void
ags_matrix_index_callback(GtkWidget *widget, AgsMatrix *matrix)
{
  GtkToggleButton *toggle;

  if(matrix->selected != NULL){
    if(GTK_TOGGLE_BUTTON(widget) != matrix->selected){
      toggle = matrix->selected;
      matrix->selected = NULL;
      gtk_toggle_button_set_active(toggle, FALSE);
      matrix->selected = (GtkToggleButton*) widget;
      ags_matrix_draw_matrix(matrix);

      matrix->copy_pattern_audio->j = GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_MATRIX_INDEX));
    }else{
      toggle = matrix->selected;
      matrix->selected = NULL;
      gtk_toggle_button_set_active(toggle, TRUE);
      matrix->selected = toggle;
    }
  }
}

gboolean
ags_matrix_drawing_area_expose_callback(GtkWidget *widget, GdkEventExpose *event, AgsMatrix *matrix)
{
  ags_matrix_draw_gutter(matrix);

  return(FALSE);
}

gboolean
ags_matrix_drawing_area_button_press_callback(GtkWidget *widget, GdkEventButton *event, AgsMatrix *matrix)
{
  if (event->button == 1){
    AgsChannel *channel;
    guint i, j;

    i = (guint) floor((double) event->y / 10.0);
    j = (guint) floor((double) event->x / 12.0);

    channel = ags_channel_nth(matrix->machine.audio->input, i + (guint) matrix->adjustment->value);

    ags_pattern_toggle_bit((AgsPattern *) channel->pattern->data, 0, strtol(matrix->selected->button.label_text, NULL, 10) -1, j);
    ags_matrix_redraw_gutter_point(matrix, channel, j, i);
  }else if (event->button == 3){
  }

  return(FALSE);
}

void
ags_matrix_adjustment_value_changed_callback(GtkWidget *widget, AgsMatrix *matrix)
{
  ags_matrix_draw_matrix(matrix);
}

void
ags_matrix_bpm_callback(GtkWidget *spin_button, AgsMatrix *matrix)
{
  AgsWindow *window;
  AgsChannel *channel;
  AgsDelayAudio *delay_audio;
  double tic;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) matrix, AGS_TYPE_WINDOW);

  tic = exp2(4.0 - (double) gtk_option_menu_get_history((GtkOptionMenu *) matrix->tic));

  delay_audio = matrix->delay_audio;
  delay_audio->delay = (guint) round(((double)AGS_DEVOUT(matrix->machine.audio->devout)->frequency / (double)AGS_DEVOUT(matrix->machine.audio->devout)->buffer_size) * (60.0 / gtk_adjustment_get_value(window->navigation->bpm->adjustment)) * tic);

  matrix->copy_pattern_audio->stream_length = ((guint)matrix->length_spin->adjustment->value) * (delay_audio->delay + 1) + 1;

  channel = matrix->machine.audio->output;

  while(channel != NULL){
    ags_channel_resize_audio_signal(channel, matrix->copy_pattern_audio->stream_length);

    channel = channel->next;
  }
}

void
ags_matrix_length_spin_callback(GtkWidget *spin_button, AgsMatrix *matrix)
{
  AgsChannel *channel;
  AgsDelayAudio *delay_audio;

  channel = matrix->machine.audio->output;

  delay_audio = matrix->delay_audio;

  matrix->copy_pattern_audio->length = (guint) GTK_SPIN_BUTTON(spin_button)->adjustment->value;
  matrix->copy_pattern_audio->stream_length = ((guint)matrix->length_spin->adjustment->value) * (delay_audio->delay + 1) + 1;

  while(channel != NULL){
    ags_channel_resize_audio_signal(channel, matrix->copy_pattern_audio->length);

    channel = channel->next;
  }
}

void
ags_matrix_tic_callback(GtkWidget *option_menu, AgsMatrix *matrix)
{
  AgsWindow *window;
  AgsChannel *channel;
  AgsDelayAudio *delay_audio;
  double tic;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) matrix);

  tic = exp2(4.0 - (double) gtk_option_menu_get_history((GtkOptionMenu *) matrix->tic));

  delay_audio = matrix->delay_audio;
  delay_audio->delay = (guint) round(((double)AGS_DEVOUT(matrix->machine.audio->devout)->frequency / (double)AGS_DEVOUT(matrix->machine.audio->devout)->buffer_size) * (60.0 / gtk_adjustment_get_value(window->navigation->bpm->adjustment)) * tic);

  matrix->copy_pattern_audio->stream_length = ((guint)matrix->length_spin->adjustment->value) * (delay_audio->delay + 1) + 1;

  channel = matrix->machine.audio->output;

  while(channel != NULL){
    ags_channel_resize_audio_signal(channel, matrix->copy_pattern_audio->stream_length);

    channel = channel->next;
  }

  channel = matrix->machine.audio->input;
}

void
ags_matrix_loop_button_callback(GtkWidget *button, AgsMatrix *matrix)
{
  matrix->copy_pattern_audio->loop = (GTK_TOGGLE_BUTTON(button)->active) ? TRUE: FALSE;
}

void
ags_matrix_run_delay_done(AgsRecall *recall, AgsMatrix *matrix)
{
  fprintf(stdout, "ags_matrix_run_delay_done\n\0");

  //  delay = AGS_DELAY(recall);
  //  matrix = AGS_MATRIX(AGS_AUDIO(delay->recall.parent)->machine);
  //  matrix->block_run = TRUE;
  matrix->machine.audio->devout_play->flags |= AGS_DEVOUT_PLAY_DONE;
  gtk_toggle_button_set_active(matrix->run, FALSE);

}

void
ags_matrix_copy_pattern_done(AgsRecall *recall, AgsMatrix *matrix)
{
  //  AgsCopyPattern *copy_pattern;

  //  copy_pattern = AGS_COPY_PATTERN(recall);
  recall->flags |= AGS_RECALL_HIDE;
  /*
  g_list_free(copy_pattern->destination);
  copy_pattern->destination = NULL;
  */
}

void
ags_matrix_copy_pattern_cancel(AgsRecall *recall, AgsMatrix *matrix)
{
}

void
ags_matrix_copy_pattern_loop(AgsRecall *recall, AgsMatrix *matrix)
{

}

void
ags_matrix_play_done(AgsRecall *recall, AgsMatrix *matrix)
{
  /*
  matrix->play_ref++;

  if(matrix->play_ref == matrix->machine.audio->output_lines){
    matrix->play_ref = 0;
    matrix->machine.audio->devout_play->flags |= AGS_DEVOUT_PLAY_DONE;
    gtk_toggle_button_set_active(matrix->run, FALSE);
  }
  */
}

void
ags_matrix_play_cancel(AgsRecall *recall, AgsMatrix *matrix)
{
}

