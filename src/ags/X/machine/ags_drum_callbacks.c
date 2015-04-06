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

#include <ags/X/machine/ags_drum_callbacks.h>
#include <ags/X/machine/ags_drum_input_pad.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_callbacks.h>

#include <ags/object/ags_application_context.h>

#include <ags/widget/ags_led.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/task/ags_toggle_pattern_bit.h>
#include <ags/audio/task/ags_toggle_led.h>

#include <ags/audio/task/recall/ags_apply_bpm.h>
#include <ags/audio/task/recall/ags_apply_sequencer_length.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>
#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_play_audio_signal.h>

#include <ags/audio/file/ags_audio_file.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_navigation.h>

#include <stdlib.h>
#include <math.h>

#define AGS_AUDIO_FILE_DEVOUT "AgsAudioFileDevout\0"
#define AGS_DRUM_PLAY_RECALL "AgsDrumPlayRecall\0"

extern AgsApplicationContext *ags_application_context;
extern const char *AGS_DRUM_INDEX;

void
ags_drum_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsDrum *drum)
{
  AgsWindow *window;

  if(old_parent != NULL)
    return;

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) drum, AGS_TYPE_WINDOW));

  AGS_MACHINE(drum)->name = g_strdup_printf("Default %d\0",
					    ags_window_find_machine_counter(window, AGS_TYPE_DRUM)->counter);
  ags_window_increment_machine_counter(window,
				       AGS_TYPE_DRUM);
}

void
ags_drum_open_callback(GtkWidget *toggle_button, AgsDrum *drum)
{
  GtkFileChooserDialog *file_chooser;
  GtkCheckButton *check_button;

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(g_strdup("open audio files\0"),
								      (GtkWindow *) gtk_widget_get_toplevel((GtkWidget *) drum),
								      GTK_FILE_CHOOSER_ACTION_OPEN,
								      GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
								      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, 
								      NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), TRUE);

  check_button = (GtkCheckButton *) gtk_check_button_new_with_label(g_strdup("open in new channel\0"));
  gtk_toggle_button_set_active((GtkToggleButton *) check_button, TRUE);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(file_chooser)->vbox, (GtkWidget *) check_button, FALSE, FALSE, 0);
  g_object_set_data(G_OBJECT(file_chooser), "create\0", (gpointer) check_button);

  check_button = (GtkCheckButton *) gtk_check_button_new_with_label(g_strdup("overwrite existing links\0"));
  gtk_toggle_button_set_active((GtkToggleButton *) check_button, TRUE);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(file_chooser)->vbox, (GtkWidget *) check_button, FALSE, FALSE, 0);
  g_object_set_data(G_OBJECT(file_chooser), "overwrite\0", (gpointer) check_button);

  gtk_widget_show_all(GTK_WIDGET(file_chooser));

  g_signal_connect(G_OBJECT(file_chooser), "response\0",
		   G_CALLBACK(ags_machine_open_response_callback), drum);
}

void
ags_drum_loop_button_callback(GtkWidget *button, AgsDrum *drum)
{
  AgsCountBeatsAudio *count_beats_audio;
  GList *list;
  gboolean loop;

  loop = (GTK_TOGGLE_BUTTON(button)->active) ? TRUE: FALSE;

  /* AgsCopyPatternAudio */
  list = AGS_MACHINE(drum)->audio->play;

  while((list = ags_recall_find_type(list,
				     AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->loop->port_value.ags_port_boolean = loop;

    list = list->next;
  }

  list = AGS_MACHINE(drum)->audio->recall;

  while((list = ags_recall_find_type(list,
				     AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->loop->port_value.ags_port_boolean = loop;

    list = list->next;
  }

}

void
ags_drum_length_spin_callback(GtkWidget *spin_button, AgsDrum *drum)
{
  AgsWindow *window;
  AgsApplySequencerLength *apply_sequencer_length;
  AgsThread *main_loop, *current;
  AgsTaskThread *task_thread;
  gdouble length;
  
  task_thread = NULL;

  main_loop = ags_application_context->main_loop;
  current = main_loop->children;

  while(current != NULL){
    if(AGS_IS_TASK_THREAD(current)){
      task_thread = (AgsTaskThread *) current;

      break;
    }

    current = current->next;
  }

  window = (AgsWindow *) gtk_widget_get_toplevel(GTK_WIDGET(drum));

  length = GTK_SPIN_BUTTON(spin_button)->adjustment->value;

  apply_sequencer_length = ags_apply_sequencer_length_new(G_OBJECT(AGS_MACHINE(drum)->audio),
							  length);

  ags_task_thread_append_task(task_thread,
			      AGS_TASK(apply_sequencer_length));
}

void
ags_drum_index0_callback(GtkWidget *widget, AgsDrum *drum)
{
  if(drum->selected0 != NULL){
    GtkToggleButton *toggle_button;

    if(GTK_TOGGLE_BUTTON(widget) != drum->selected0){
      AgsCopyPatternAudio *copy_pattern_audio;
      GList *list;

      toggle_button = drum->selected0;
      drum->selected0 = NULL;
      gtk_toggle_button_set_active(toggle_button, FALSE);
      drum->selected0 = (GtkToggleButton*) widget;

      list = ags_recall_find_type(AGS_MACHINE(drum)->audio->play,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	GValue value = {0,};

	g_value_init(&value, G_TYPE_UINT);
	g_value_set_uint(&value, GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_DRUM_INDEX)));

	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	ags_port_safe_write(copy_pattern_audio->bank_index_0, &value);

	g_value_unset(&value);
      }

      list = ags_recall_find_type(AGS_MACHINE(drum)->audio->recall,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	GValue value = {0,};

	g_value_init(&value, G_TYPE_UINT);
	g_value_set_uint(&value, GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_DRUM_INDEX)));

	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	ags_port_safe_write(copy_pattern_audio->bank_index_0, &value);

	g_value_unset(&value);
      }
    }else if(! gtk_toggle_button_get_active(drum->selected0)){
      toggle_button = drum->selected0;
      drum->selected0 = NULL;
      gtk_toggle_button_set_active((GtkToggleButton *) widget, TRUE);
      drum->selected0 = (GtkToggleButton*) widget;
    }

    ags_drum_set_pattern(drum);
  }
}

void
ags_drum_index1_callback(GtkWidget *widget, AgsDrum *drum)
{
  if(drum->selected1 != NULL){
    GtkToggleButton *toggle_button;

    if(GTK_TOGGLE_BUTTON(widget) != drum->selected1){
      AgsCopyPatternAudio *copy_pattern_audio;
      GList *list;

      toggle_button = drum->selected1;
      drum->selected1 = NULL;
      gtk_toggle_button_set_active(toggle_button, FALSE);
      drum->selected1 = (GtkToggleButton*) widget;

      list = ags_recall_find_type(AGS_MACHINE(drum)->audio->play,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	GValue value = {0,};

	g_value_init(&value, G_TYPE_UINT);
	g_value_set_uint(&value, GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_DRUM_INDEX)));

	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	ags_port_safe_write(copy_pattern_audio->bank_index_1, &value);

	g_value_unset(&value);
      }

      list = ags_recall_find_type(AGS_MACHINE(drum)->audio->recall,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	GValue value = {0,};

	g_value_init(&value, G_TYPE_UINT);
	g_value_set_uint(&value, GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_DRUM_INDEX)));

	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	ags_port_safe_write(copy_pattern_audio->bank_index_1, &value);

	g_value_unset(&value);
      }
    }else if(!gtk_toggle_button_get_active(drum->selected1)){
      toggle_button = drum->selected1;
      drum->selected1 = NULL;
      gtk_toggle_button_set_active((GtkToggleButton *) widget, TRUE);
      drum->selected1 = (GtkToggleButton*) widget;
    }

    ags_drum_set_pattern(drum);
  }
}

void
ags_drum_pad_callback(GtkWidget *toggle_button, AgsDrum *drum)
{
  AgsLine *selected_line;
  AgsPattern *pattern;
  AgsTogglePatternBit *toggle_pattern_bit;
  AgsThread *main_loop, *current;
  AgsTaskThread *task_thread;
  GList *list, *list_start;
  GList *line, *line_start;
  GList *tasks;
  guint i, index0, index1, offset;

  if(drum->selected_pad == NULL){
    return;
  }

  if((AGS_DRUM_BLOCK_PATTERN & (drum->flags)) != 0){
#ifdef AGS_DEBUG
    g_message("AgsDrum pattern is blocked\n\0");
#endif

    return;
  }

  task_thread = NULL;

  main_loop = ags_application_context->main_loop;
  current = main_loop->children;

  while(current != NULL){
    if(AGS_IS_TASK_THREAD(current)){
      task_thread = (AgsTaskThread *) current;

      break;
    }

    current = current->next;
  }

  /* calculate offset */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) drum->pattern);

  for(i = 0; i < 16 && toggle_button != list->data; i++)
    list = list->next;

  offset = i;
  g_list_free(list_start);

  /* calculate index 0 */
  for(i = 0; i < 4 && drum->selected0 != drum->index0[i]; i++);

  index0 = i;

  /* calculate index 1 */
  for(i = 0; i < 12 && drum->selected1 != drum->index1[i]; i++);
  
  index1 = i;

  /* calculate offset / page */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) drum->offset);

  for(i = 0; i < 4 && ! GTK_TOGGLE_BUTTON(list->data)->active; i++)
    list = list->next;

  offset += (i * 16);

  g_list_free(list_start);

  /**/
  line_start = 
    line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(drum->selected_pad)->expander_set));
  tasks = NULL;

  while((line = ags_line_find_next_grouped(line)) != NULL){
    selected_line = AGS_LINE(line->data);

    toggle_pattern_bit = ags_toggle_pattern_bit_new(selected_line->channel->pattern->data,
						    selected_line->channel->line,
						    index0, index1,
						    offset);
    g_message("found %d %x\0", selected_line->channel, selected_line->channel->pattern->data);

    tasks = g_list_prepend(tasks,
			   toggle_pattern_bit);

    line = line->next;
  }

  g_list_free(line_start);

  /* append AgsTogglePatternBit */
  ags_task_thread_append_tasks(task_thread,
			       tasks);
}

void
ags_drum_offset_callback(GtkWidget *widget, AgsDrum *drum)
{
  ags_drum_set_pattern(drum);
}

void
ags_drum_tact_callback(AgsAudio *audio,
		       AgsRecallID *recall_id,
		       AgsDrum *drum)
{
  AgsWindow *window;
  AgsCountBeatsAudio *play_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;
  AgsToggleLed *toggle_led;
  AgsThread *main_loop, *current;
  AgsTaskThread *task_thread;
  GList *list, *tmp;
  guint counter, active_led;
  gdouble active_led_old, active_led_new;
  GValue value = {0,};
  
  task_thread = NULL;

  main_loop = ags_application_context->main_loop;
  current = main_loop->children;

  while(current != NULL){
    if(AGS_IS_TASK_THREAD(current)){
      task_thread = (AgsTaskThread *) current;

      break;
    }

    current = current->next;
  }

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) drum, AGS_TYPE_WINDOW));

  /* get some recalls */
  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_COUNT_BEATS_AUDIO);
  
  if(list != NULL){
    play_count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);
  }

  list = ags_recall_find_type_with_recycling_container(audio->play,
						       AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
						       (GObject *) recall_id->recycling_container);
  
  if(list != NULL){
    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);
  }

  /* set optical feedback */
  active_led_new = (guint) play_count_beats_audio_run->sequencer_counter % AGS_DRUM_PATTERN_CONTROLS;
  drum->active_led = (guint) active_led_new;

  if(play_count_beats_audio_run->sequencer_counter == 0){
    g_value_init(&value, G_TYPE_DOUBLE);
    ags_port_safe_read(play_count_beats_audio->sequencer_loop_end,
		       &value);

    active_led_old = (guint) (g_value_get_double(&value) - 1.0) % AGS_DRUM_PATTERN_CONTROLS;
  }else{
    active_led_old = (guint) (drum->active_led - 1.0) % AGS_DRUM_PATTERN_CONTROLS;
  }

  tmp = gtk_container_get_children(GTK_CONTAINER(drum->led));
  toggle_led = ags_toggle_led_new(tmp,
				  (guint) active_led_new,
				  (guint) active_led_old);

  ags_task_thread_append_task(task_thread,
			      AGS_TASK(toggle_led));
}

void
ags_drum_done_callback(AgsAudio *audio,
		       AgsRecallID *recall_id,
		       AgsDrum *drum)
{
  GList *devout_play;
  gboolean all_done;

  devout_play = AGS_DEVOUT_PLAY_DOMAIN(audio->devout_play_domain)->devout_play;

  /* check unset */
  all_done = TRUE;

  while(devout_play != NULL){
    if(AGS_DEVOUT_PLAY(devout_play->data)->recall_id[1] != NULL){
      all_done = FALSE;
      break;
    }

    devout_play = devout_play->next;
  }

  if(all_done){
    GList *list;
    guint active_led;

    /* get active led */
    if(drum->active_led == 0){
      AgsCountBeatsAudio *play_count_beats_audio;
      GValue value = {0,};

      /* get some recalls */
      list = ags_recall_find_type(audio->play,
				  AGS_TYPE_COUNT_BEATS_AUDIO);
  
      if(list != NULL){
	play_count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);
      }
  
      g_value_init(&value, G_TYPE_DOUBLE);
      ags_port_safe_read(play_count_beats_audio->sequencer_loop_end,
			 &value);
      
      active_led = g_value_get_double(&value) - 1.0;
    }else{
      active_led = drum->active_led - 1;
    }

    /* unset led */
    list = gtk_container_get_children(GTK_CONTAINER(drum->led));
    ags_led_unset_active(AGS_LED(g_list_nth(list,
					    active_led)->data));

    g_list_free(list);
  }
}
