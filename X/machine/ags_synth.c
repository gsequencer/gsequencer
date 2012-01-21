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

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synths.h>

#include <ags/audio/task/ags_apply_synth.h>
#include <ags/audio/recall/ags_stream_channel.h>

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

void ags_synth_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old,
				  gpointer data);
void ags_synth_set_pads(AgsAudio *audio, GType type,
			guint pads, guint pads_old,
			gpointer data);

void ags_synth_update(AgsSynth *synth);

extern void ags_file_read_synth(AgsFile *file, AgsMachine *machine);
extern void ags_file_write_synth(AgsFile *file, AgsMachine *machine);

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
  GtkMenu *menu;
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;
  GtkFrame *frame;

  g_signal_connect_after((GObject *) synth, "parent_set\0",
			 G_CALLBACK(ags_synth_parent_set_callback), (gpointer) synth);

  synth->machine.audio->flags |= (AGS_AUDIO_ASYNC |
				  AGS_AUDIO_OUTPUT_HAS_RECYCLING |
				  AGS_AUDIO_INPUT_HAS_RECYCLING);

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

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) hbox, FALSE, FALSE, 0);
  
  label = (GtkLabel *) gtk_label_new("lower\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 0);

  synth->lower = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) synth->lower, FALSE, FALSE, 0);

  frame = (GtkFrame *) gtk_frame_new(g_strdup("amplifier 0\0"));
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
  gdouble volume;

  devout = (AgsDevout *) synth->machine.audio->devout;

  start = (guint) gtk_spin_button_get_value_as_int(synth->lower);

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
				      volume);

    ags_devout_append_task(devout,
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
				      volume);

    ags_devout_append_task(devout,
			   AGS_TASK(apply_synth));

    list_oscillator = list_oscillator->next;
  }


  /*
  AgsChannel *input, *output;
  AgsAudioSignal *audio_signal;
  GList *list0 ,*list1;
  gint wave;
  guint attack, length, stop, phase, phase_tmp, frequency;
  double volume;
  guint attack0, attack1, length0, frequency0, phase0, phase1;
  guint i, j;
  void ags_synth_update_write(guint offset, guint frequency, guint phase, guint length, double volume){
    if(wave == 0){
      ags_synth_sin(audio_signal->devout, (short *) list1->data, offset, frequency, phase, length, volume);
    }else if(wave == 1){
      ags_synth_sin(audio_signal->devout, (short *) list1->data, offset, frequency, phase, length, volume);
    }else if(wave == 2){
      ags_synth_saw(audio_signal->devout, (short *) list1->data, offset, frequency, phase, length, volume);
    }else if(wave == 3){
      ags_synth_square(audio_signal->devout, (short *) list1->data, offset, frequency, phase, length, volume);
    }else if(wave == 4){
      ags_synth_triangle(audio_signal->devout, (short *) list1->data, offset, frequency, phase, length, volume);
    }else{
      fprintf(stdout, "no wave selected\n\0");
    }
  }

  input = synth->machine.audio->input;
  list0 = gtk_container_get_children(GTK_CONTAINER(gtk_option_menu_get_menu(synth->oscillator)));

  while(list0 != NULL){
    oscillator = AGS_OSCILLATOR(list0->data);
    devout = (AgsDevout *) synth->machine.audio->devout;

    wave = gtk_option_menu_get_history(GTK_OPTION_MENU(oscillator->wave));
    fprintf(stdout, "wave = %d\n\0", wave);

    attack = (guint) oscillator->attack->adjustment->value;
    length = (guint) oscillator->length->adjustment->value;
    stop = (guint) ceil((double)(attack + length) / (double)devout->buffer_size);
    phase = (guint) oscillator->phase->adjustment->value;
    frequency = (guint) oscillator->frequency->adjustment->value;
    volume = (double) oscillator->volume->adjustment->value;

    attack0 = (guint) ceil((double)attack / (double)devout->buffer_size);
    attack1 = (guint) floor((double)attack / (double)devout->buffer_size);
    attack = attack % (guint) devout->buffer_size;

    /* write to input * /
    audio_signal = ags_audio_signal_get_template(input->first_recycling->audio_signal);
    ags_audio_signal_stream_resize(audio_signal, stop);

    list1 = g_list_nth(audio_signal->stream_beginning, attack1);

    if(list1 != NULL){
      if(list1->next != NULL)
	ags_synth_update_write(attack, frequency, phase, AGS_DEVOUT(audio_signal->devout)->buffer_size - attack, volume);
      else{
	ags_synth_update_write(attack, frequency, phase, length, volume);

	goto ags_synth_update0;
      }

      list1 = list1->next;
    }else
      continue;

    for(i = 0; list1->next != NULL; i++){
      phase0 = (phase + (AGS_DEVOUT(audio_signal->devout)->buffer_size - attack) + i * AGS_DEVOUT(audio_signal->devout)->buffer_size) % (devout->frequency / frequency);

      ags_synth_update_write(0, frequency, phase0, AGS_DEVOUT(audio_signal->devout)->buffer_size, volume);

      list1 = list1->next;
    }

    length0 = (length - (AGS_DEVOUT(audio_signal->devout)->buffer_size - attack)) % AGS_DEVOUT(audio_signal->devout)->buffer_size;

    phase0 = (phase + (AGS_DEVOUT(audio_signal->devout)->buffer_size - attack) + i * AGS_DEVOUT(audio_signal->devout)->buffer_size) % frequency;
    ags_synth_update_write(0, frequency, phase0, length0, volume);

    /* write to output * /
  ags_synth_update0:
    output = synth->machine.audio->output;

    for(i = 0; output != NULL; i++){
      frequency0 = (guint) ((double) frequency * exp2((double)((synth->lower->adjustment->value * -1.0) + (double)i) / 12.0));
      phase0 = (guint) ((double) phase * ((double) frequency / (double) frequency0));

      audio_signal = ags_audio_signal_get_template(output->first_recycling->audio_signal);

      ags_audio_signal_stream_resize(audio_signal, stop);

      list1 = g_list_nth(audio_signal->stream_beginning, attack1);

      if(list1->next != NULL){
       	fprintf(stdout, "freq = %u, phase = %u\n\0", frequency0, phase0);
	ags_synth_update_write(attack, frequency0, phase0, AGS_DEVOUT(audio_signal->devout)->buffer_size - attack, volume);
      }else{
	//	fprintf(stdout, "freq = %d, phase = %d\n\0", frequency0, phase0);
	ags_synth_update_write(attack, frequency0, phase0, length, volume);

	continue;
      }

      list1 = list1->next;


      for(j = 1; list1->next != NULL; j++){
	phase1 = (j * AGS_DEVOUT(audio_signal->devout)->buffer_size + phase0) % (devout->frequency / frequency0);
	fprintf(stdout, "freq = %u, phase = %u\n\0", frequency0, phase1);

	ags_synth_update_write(0, frequency, phase1, AGS_DEVOUT(audio_signal->devout)->buffer_size, volume);

	list1 = list1->next;
      }

      phase1 = (phase0 + (AGS_DEVOUT(audio_signal->devout)->buffer_size - attack) + j * AGS_DEVOUT(audio_signal->devout)->buffer_size) % frequency0;
      ags_synth_update_write(0, frequency0, phase1, length0, volume);


      output = output->next;
    }

    input = input->next;
    list0 = list0->next;
  }
*/
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
