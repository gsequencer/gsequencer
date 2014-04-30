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

#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_synth_callbacks.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synths.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_play_notation_audio_run.h>

#include <ags/audio/task/ags_apply_synth.h>

#include <ags/X/machine/ags_oscillator.h>

#include <ags/X/ags_window.h>

#include <math.h>

void ags_synth_class_init(AgsSynthClass *synth);
void ags_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_synth_init(AgsSynth *synth);
void ags_synth_finalize(GObject *gobject);
void ags_synth_connect(AgsConnectable *connectable);
void ags_synth_disconnect(AgsConnectable *connectable);
void ags_synth_show(GtkWidget *widget);

void ags_file_read_synth(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_file_write_synth(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_synth_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old,
				  gpointer data);
void ags_synth_set_pads(AgsAudio *audio, GType type,
			guint pads, guint pads_old,
			gpointer data);

void ags_synth_update(AgsSynth *synth);

static gpointer ags_synth_parent_class = NULL;

static AgsConnectableInterface *ags_synth_parent_connectable_interface;

GType
ags_synth_get_type(void)
{
  static GType ags_type_synth = 0;

  if(!ags_type_synth){
    static const GTypeInfo ags_synth_info = {
      sizeof(AgsSynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_synth = g_type_register_static(AGS_TYPE_MACHINE,
					    "AgsSynth\0", &ags_synth_info,
					    0);
    
    g_type_add_interface_static(ags_type_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_synth);
}

void
ags_synth_class_init(AgsSynthClass *synth)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_synth_parent_class = g_type_class_peek_parent(synth);

  /* GObjectClass */
  gobject = (GObjectClass *) synth;

  gobject->finalize = ags_synth_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) synth;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) synth;

  //  machine->read_file = ags_file_read_synth;
  //  machine->write_file = ags_file_write_synth;
}

void
ags_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_synth_connectable_parent_interface;

  ags_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_synth_connect;
  connectable->disconnect = ags_synth_disconnect;
}

void
ags_synth_init(AgsSynth *synth)
{
  AgsAudio *audio;
  AgsRecallContainer *recall_container;
  AgsDelayAudio *delay_audio;
  AgsDelayAudioRun *play_delay_audio_run, *recall_delay_audio_run;
  AgsCountBeatsAudio *play_count_beats_audio, *recall_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run, *recall_count_beats_audio_run;
  AgsPlayNotationAudioRun *play_notation, *recall_notation;
  AgsRecallAudio *play_audio, *recall_audio;
  GtkMenu *menu;
  GtkVBox *vbox;
  GtkTable *table;
  GtkLabel *label;
  GtkFrame *frame;

  g_signal_connect_after((GObject *) synth, "parent_set\0",
			 G_CALLBACK(ags_synth_parent_set_callback), (gpointer) synth);

  audio = AGS_MACHINE(synth)->audio;
  audio->flags |= (AGS_AUDIO_ASYNC |
		   AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		   AGS_AUDIO_INPUT_HAS_RECYCLING |
		   AGS_AUDIO_HAS_NOTATION);

  AGS_MACHINE(synth)->flags |= AGS_MACHINE_IS_SYNTHESIZER;

  /* create widgets */
  synth->hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) (gtk_container_get_children((GtkContainer *) synth))->data, (GtkWidget *) synth->hbox);

  synth->oscillator = (GtkOptionMenu *) gtk_option_menu_new();
  gtk_box_pack_start((GtkBox *) synth->hbox, (GtkWidget *) synth->oscillator, FALSE, FALSE, 0);

  menu = (GtkMenu *) gtk_menu_new();
  gtk_option_menu_set_menu(synth->oscillator, (GtkWidget *) menu);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) synth->hbox, (GtkWidget *) vbox, FALSE, FALSE, 0);

  synth->auto_update = (GtkCheckButton *) gtk_check_button_new_with_label(g_strdup("auto update\0"));
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) synth->auto_update, FALSE, FALSE, 0);

  synth->update = (GtkButton *) gtk_button_new_with_label(g_strdup("update\0"));
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) synth->update, FALSE, FALSE, 0);


  table = (GtkTable *) gtk_table_new(3, 2, FALSE);
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) table, FALSE, FALSE, 0);

  
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "lower\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  synth->lower = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(synth->lower),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);


  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "loop start\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  synth->loop_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(synth->loop_start),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);


  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "loop end\0",
				    "xalign\0", 0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  synth->loop_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(synth->loop_end),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  frame = (GtkFrame *) gtk_frame_new(g_strdup("amplifier \0"));
  gtk_box_pack_start((GtkBox *) synth->hbox, (GtkWidget *) frame, FALSE, FALSE, 0);

  synth->amplifier[0] = (GtkOptionMenu *) gtk_option_menu_new();
  gtk_container_add((GtkContainer *) frame, (GtkWidget *) synth->amplifier[0]);

  menu = (GtkMenu *) gtk_menu_new();
  gtk_option_menu_set_menu(synth->amplifier[0], (GtkWidget *) menu);

  frame = (GtkFrame *) gtk_frame_new(g_strdup("amplifier 1\0"));
  gtk_box_pack_start((GtkBox *) synth->hbox, (GtkWidget *) frame, FALSE, FALSE, 0);

  synth->amplifier[1] = (GtkOptionMenu *) gtk_option_menu_new();
  gtk_container_add((GtkContainer *) frame, (GtkWidget *) synth->amplifier[1]);

  menu = (GtkMenu *) gtk_menu_new();
  gtk_option_menu_set_menu(synth->amplifier[1], (GtkWidget *) menu);
}

void
ags_synth_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_synth_parent_class)->finalize(gobject);
}

void
ags_synth_connect(AgsConnectable *connectable)
{
  AgsSynth *synth;

  ags_synth_parent_connectable_interface->connect(connectable);

  /* AgsSynth */
  synth = AGS_SYNTH(connectable);

  g_signal_connect((GObject *) synth->lower, "value-changed\0",
		   G_CALLBACK(ags_synth_lower_callback), synth);

  g_signal_connect((GObject *) synth->auto_update, "toggled\0",
		   G_CALLBACK(ags_synth_auto_update_callback), synth);

  g_signal_connect((GObject *) synth->update, "clicked\0",
		   G_CALLBACK(ags_synth_update_callback), (gpointer) synth);

  /* AgsAudio */
  g_signal_connect_after(G_OBJECT(synth->machine.audio), "set_audio_channels\0",
			 G_CALLBACK(ags_synth_set_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(synth->machine.audio), "set_pads\0",
			 G_CALLBACK(ags_synth_set_pads), NULL);
}

void
ags_synth_disconnect(AgsConnectable *connectable)
{
  AgsSynth *synth;

  ags_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsSynth */
  synth = AGS_SYNTH(connectable);
}

void
ags_synth_set_audio_channels(AgsAudio *audio,
			     guint audio_channels, guint audio_channels_old,
			     gpointer data)
{
  printf("AgsSynth only pads can be adjusted\n\0");
  //  _ags_audio_set_audio_channels(audio, audio_channels);
}

void
ags_synth_set_pads(AgsAudio *audio, GType type,
		   guint pads, guint pads_old,
		   gpointer data)
{
  AgsSynth *synth;
  GtkHBox *hbox;
  GtkHBox* ags_synth_set_pads_amplifier(){
    GtkHBox *hbox;
    GtkVScale *scale;

    hbox = (GtkHBox *) gtk_hbox_new(TRUE, 0);

    scale = (GtkVScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
    gtk_range_set_inverted((GtkRange *) scale, TRUE);
    gtk_scale_set_draw_value((GtkScale *) scale, FALSE);
    gtk_widget_set_size_request((GtkWidget *) scale, -1, 100);
    gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) scale, FALSE, FALSE, 0);

    scale = (GtkVScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
    gtk_range_set_inverted((GtkRange *) scale, TRUE);
    gtk_scale_set_draw_value((GtkScale *) scale, FALSE);
    gtk_widget_set_size_request((GtkWidget *) scale, -1, 100);
    gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) scale, FALSE, FALSE, 0);

    scale = (GtkVScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
    gtk_range_set_inverted((GtkRange *) scale, TRUE);
    gtk_scale_set_draw_value((GtkScale *) scale, FALSE);
    gtk_widget_set_size_request((GtkWidget *) scale, -1, 100);
    gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) scale, FALSE, FALSE, 0);

    scale = (GtkVScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
    gtk_range_set_inverted((GtkRange *) scale, TRUE);
    gtk_scale_set_draw_value((GtkScale *) scale, FALSE);
    gtk_widget_set_size_request((GtkWidget *) scale, -1, 100);
    gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) scale, FALSE, FALSE, 0);

    return(hbox);
  }

  synth = (AgsSynth *) audio->machine;

  if(type == AGS_TYPE_INPUT){
    if(pads > pads_old){
      AgsOscillator *oscillator;
      GtkMenuItem *item0, *item1;
      guint i;
      
      for(i = pads_old; i < pads; i++){
	oscillator = ags_oscillator_new();
	gtk_menu_shell_append((GtkMenuShell *) gtk_option_menu_get_menu(synth->oscillator), (GtkWidget *) oscillator);

	item0 = (GtkMenuItem *) gtk_menu_item_new();
	hbox = ags_synth_set_pads_amplifier();
	gtk_container_add((GtkContainer *) item0,
			  (GtkWidget *) hbox);
	gtk_menu_shell_append((GtkMenuShell *) gtk_option_menu_get_menu(synth->amplifier[0]), (GtkWidget *) item0);

	item1 = (GtkMenuItem *) gtk_menu_item_new();
	hbox = ags_synth_set_pads_amplifier();
	gtk_container_add((GtkContainer *) item1,
			  (GtkWidget *) hbox);
	gtk_menu_shell_append((GtkMenuShell *) gtk_option_menu_get_menu(synth->amplifier[1]), (GtkWidget *) item1);

	if(GTK_WIDGET_VISIBLE((GtkWidget *) synth)){
	  gtk_widget_show_all((GtkWidget *) item0);
	  gtk_widget_show_all((GtkWidget *) item1);

	  gtk_widget_show_all((GtkWidget *) oscillator);
	  ags_oscillator_connect(oscillator);
	}
      }

      if(pads_old == 0){
	gtk_option_menu_set_history(synth->oscillator, 0);
	gtk_option_menu_set_history(synth->amplifier[0], 0);
	gtk_option_menu_set_history(synth->amplifier[1], 0);
      }
    }else{
      GList *list0, *list1, *list2, *list3, *list4, *list5;
      
      list0 = g_list_nth(gtk_container_get_children((GtkContainer *) synth->oscillator->menu), pads);
      list2 = g_list_nth(gtk_container_get_children((GtkContainer *) synth->amplifier[0]->menu), pads);
      list4 = g_list_nth(gtk_container_get_children((GtkContainer *) synth->amplifier[1]->menu), pads);

      while(list0 != NULL){
	list1 = list0->next;
	list3 = list2->next;
	list5 = list4->next;

	gtk_widget_destroy((GtkWidget *) list0->data);
	gtk_widget_destroy((GtkWidget *) list2->data);
	gtk_widget_destroy((GtkWidget *) list4->data);

	list0 = list1;
	list2 = list3;
	list4 = list5;
      }
    }
  }else{
  }
}

void
ags_synth_update(AgsSynth *synth)
{
  AgsOscillator *oscillator;
  AgsDevout *devout;
  AgsChannel *channel;
  AgsApplySynth *apply_synth;
  GList *list_oscillator;
  guint wave;
  guint attack, frame_count;
  guint frequency, phase, start;
  guint loop_start, loop_end;
  gdouble volume;

  devout = (AgsDevout *) synth->machine.audio->devout;

  start = (guint) gtk_spin_button_get_value_as_int(synth->lower);

  loop_start = (guint) gtk_spin_button_get_value_as_int(synth->loop_start);
  loop_end = (guint) gtk_spin_button_get_value_as_int(synth->loop_end);

  /* write input */
  channel = synth->machine.audio->input;

  list_oscillator = gtk_container_get_children(GTK_CONTAINER(gtk_option_menu_get_menu(synth->oscillator)));

  while(list_oscillator != NULL){
    oscillator = AGS_OSCILLATOR(list_oscillator->data);

    wave = (guint) gtk_combo_box_get_active(oscillator->wave) + 1;
    attack = (guint) gtk_spin_button_get_value_as_int(oscillator->attack);
    frame_count = (guint) gtk_spin_button_get_value_as_int(oscillator->frame_count);
    phase = (guint) gtk_spin_button_get_value_as_int(oscillator->phase);
    frequency = (guint) gtk_spin_button_get_value_as_int(oscillator->frequency);
    volume = (gdouble) gtk_spin_button_get_value_as_float(oscillator->volume);

    apply_synth = ags_apply_synth_new(channel, 1,
				      wave,
				      attack, frame_count,
				      frequency, phase, start,
				      volume,
				      loop_start, loop_end);

    ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread),
				AGS_TASK(apply_synth));

    channel = channel->next;
    list_oscillator = list_oscillator->next;
  }

  /* write output */
  channel = synth->machine.audio->output;
  list_oscillator = gtk_container_get_children(GTK_CONTAINER(gtk_option_menu_get_menu(synth->oscillator)));

  while(list_oscillator != NULL){
    oscillator = AGS_OSCILLATOR(list_oscillator->data);

    wave = (guint) gtk_combo_box_get_active(oscillator->wave) + 1;
    attack = (guint) gtk_spin_button_get_value_as_int(oscillator->attack);
    frame_count = (guint) gtk_spin_button_get_value_as_int(oscillator->frame_count);
    phase = (guint) gtk_spin_button_get_value_as_int(oscillator->phase);
    frequency = (guint) gtk_spin_button_get_value_as_int(oscillator->frequency);
    volume = (gdouble) gtk_spin_button_get_value_as_float(oscillator->volume);

    apply_synth = ags_apply_synth_new(channel, synth->machine.audio->output_lines,
				      wave,
				      attack, frame_count,
				      frequency, phase, start,
				      volume,
				      loop_start, loop_end);

    ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread),
				AGS_TASK(apply_synth));

    list_oscillator = list_oscillator->next;
  }
}

AgsSynth*
ags_synth_new(GObject *devout)
{
  AgsSynth *synth;
  GValue value = {0,};

  synth = (AgsSynth *) g_object_new(AGS_TYPE_SYNTH,
				    NULL);

  if(devout != NULL){
    g_value_init(&value, G_TYPE_OBJECT);
    g_value_set_object(&value, devout);
    g_object_set_property(G_OBJECT(synth->machine.audio),
			  "devout\0", &value);
    g_value_unset(&value);
  }

  return(synth);
}
