#include "ags_synth.h"
#include "ags_synth_callbacks.h"

#include "ags_oscillator.h"

#include "../ags_window.h"

#include "../../audio/ags_devout.h"
#include "../../audio/ags_audio.h"
#include "../../audio/ags_channel.h"
#include "../../audio/ags_input.h"
#include "../../audio/ags_output.h"
#include "../../audio/ags_audio_signal.h"
#include "../../audio/ags_synths.h"

#include <math.h>

GType ags_synth_get_type(void);
void ags_synth_class_init(AgsSynthClass *synth);
void ags_synth_init(AgsSynth *synth);
void ags_synth_connect(AgsSynth *synth);
void ags_synth_destroy(GtkObject *object);
void ags_synth_show(GtkWidget *widget);

void ags_synth_set_audio_channels(AgsAudio *audio, guint audio_channels);
void ags_synth_set_pads(AgsAudio *audio, GType type, guint pads);
void ags_synth_set_lines(AgsAudio *audio, GType type, guint lines);

void ags_synth_update(AgsSynth *synth);

extern void ags_file_read_synth(AgsFile *file, AgsMachine *machine);
extern void ags_file_write_synth(AgsFile *file, AgsMachine *machine);

GType
ags_synth_get_type(void)
{
  static GType synth_type = 0;

  if (!synth_type){
    static const GtkTypeInfo synth_info = {
      "AgsSynth\0",
      sizeof(AgsSynth), /* base_init */
      sizeof(AgsSynthClass), /* base_finalize */
      (GtkClassInitFunc) ags_synth_class_init,
      (GtkObjectInitFunc) ags_synth_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    synth_type = gtk_type_unique (AGS_TYPE_MACHINE, &synth_info);
  }

  return (synth_type);
}

void
ags_synth_class_init(AgsSynthClass *synth)
{
  AgsMachineClass *machine = (AgsMachineClass *) synth;

  //  machine->read_file = ags_file_read_synth;
  //  machine->write_file = ags_file_write_synth;
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

  AGS_AUDIO_GET_CLASS(synth->machine.audio)->set_audio_channels = ags_synth_set_audio_channels;
  AGS_AUDIO_GET_CLASS(synth->machine.audio)->set_pads = ags_synth_set_pads;
  AGS_AUDIO_GET_CLASS(synth->machine.audio)->set_lines = ags_synth_set_lines;

  synth->machine.audio->flags |= (AGS_AUDIO_ASYNC | AGS_AUDIO_OUTPUT_HAS_RECYCLING | AGS_AUDIO_INPUT_HAS_RECYCLING);

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
ags_synth_destroy(GtkObject *object)
{
}

void
ags_synth_show(GtkWidget *widget)
{
}

void
ags_synth_connect(AgsSynth *synth)
{
  ags_machine_connect((AgsMachine *) synth);

  g_signal_connect((GObject *) synth, "destroy\0",
		   G_CALLBACK(ags_synth_destroy_callback), (gpointer) synth);

  g_signal_connect((GObject *) synth->lower, "value-changed\0",
		   G_CALLBACK(ags_synth_lower_callback), synth);

  g_signal_connect((GObject *) synth->auto_update, "toggled\0",
		   G_CALLBACK(ags_synth_auto_update_callback), synth);

  g_signal_connect((GObject *) synth->update, "clicked\0",
		   G_CALLBACK(ags_synth_update_callback), (gpointer) synth);
}

void
ags_synth_set_audio_channels(AgsAudio *audio, guint audio_channels)
{
  printf("AgsSynth only pads can be adjusted\n\0");
  //  _ags_audio_set_audio_channels(audio, audio_channels);
}

void
ags_synth_set_pads(AgsAudio *audio, GType type, guint pads)
{
  AgsSynth *synth;
  GtkHBox *hbox;
  GtkMenuItem *item;
  guint pads_old;
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

  pads_old = (type == AGS_TYPE_OUTPUT) ? audio->output_pads: audio->input_pads;

  if(pads == pads_old)
    return;

  ags_audio_real_set_pads(audio, type, pads);
  synth = (AgsSynth *) audio->machine;

  if(type == AGS_TYPE_INPUT)
    if(pads > pads_old){
      AgsOscillator *oscillator;
      guint i;
      
      for(i = pads_old; i < pads; i++){
	oscillator = ags_oscillator_new();
	gtk_menu_shell_append((GtkMenuShell *) gtk_option_menu_get_menu(synth->oscillator), (GtkWidget *) oscillator);

	item = (GtkMenuItem *) gtk_menu_item_new();
	hbox = ags_synth_set_pads_amplifier();
	gtk_container_add((GtkContainer *) item,
			  (GtkWidget *) hbox);
	gtk_menu_shell_append((GtkMenuShell *) gtk_option_menu_get_menu(synth->amplifier[0]), (GtkWidget *) item);

	item = (GtkMenuItem *) gtk_menu_item_new();
	hbox = ags_synth_set_pads_amplifier();
	gtk_container_add((GtkContainer *) item,
			  (GtkWidget *) hbox);
	gtk_menu_shell_append((GtkMenuShell *) gtk_option_menu_get_menu(synth->amplifier[1]), (GtkWidget *) item);

	if(GTK_WIDGET_VISIBLE((GtkWidget *) synth)){
	  gtk_widget_show_all((GtkWidget *) item);
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
}

void
ags_synth_set_lines(AgsAudio *audio, GType type, guint lines)
{
  ags_audio_real_set_lines(audio, type, lines);
}

void
ags_synth_update(AgsSynth *synth)
{
  AgsOscillator *oscillator;
  AgsDevout *devout;
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

    /* write to input */
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

    /* write to output */
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
}

AgsSynth*
ags_synth_new()
{
  AgsSynth *synth;

  synth = (AgsSynth *) g_object_new(AGS_TYPE_SYNTH, NULL);

  return(synth);
}
