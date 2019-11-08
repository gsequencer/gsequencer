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

#include <ags/X/machine/ags_fm_synth.h>
#include <ags/X/machine/ags_fm_synth_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <ags/X/machine/ags_fm_synth_input_pad.h>
#include <ags/X/machine/ags_fm_synth_input_line.h>
#include <ags/X/machine/ags_fm_oscillator.h>

#include <math.h>

#include <ags/i18n.h>

void ags_fm_synth_class_init(AgsFMSynthClass *fm_synth);
void ags_fm_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fm_synth_init(AgsFMSynth *fm_synth);
void ags_fm_synth_finalize(GObject *gobject);

void ags_fm_synth_connect(AgsConnectable *connectable);
void ags_fm_synth_disconnect(AgsConnectable *connectable);

void ags_fm_synth_show(GtkWidget *widget);

void ags_fm_synth_map_recall(AgsMachine *machine);

/**
 * SECTION:ags_fm_synth
 * @short_description: fm synth
 * @title: AgsFMSynth
 * @section_id:
 * @include: ags/X/machine/ags_fm_synth.h
 *
 * The #AgsFMSynth is a composite widget to act as fm synth.
 */

static gpointer ags_fm_synth_parent_class = NULL;
static AgsConnectableInterface *ags_fm_synth_parent_connectable_interface;

extern GHashTable *ags_machine_generic_output_message_monitor;

GType
ags_fm_synth_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fm_synth = 0;

    static const GTypeInfo ags_fm_synth_info = {
      sizeof(AgsFMSynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fm_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFMSynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fm_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_fm_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_fm_synth = g_type_register_static(AGS_TYPE_MACHINE,
					       "AgsFMSynth", &ags_fm_synth_info,
					       0);
    
    g_type_add_interface_static(ags_type_fm_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fm_synth);
  }

  return g_define_type_id__volatile;
}

void
ags_fm_synth_class_init(AgsFMSynthClass *fm_synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_fm_synth_parent_class = g_type_class_peek_parent(fm_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) fm_synth;

  gobject->finalize = ags_fm_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) fm_synth;

  machine->map_recall = ags_fm_synth_map_recall;
}

void
ags_fm_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_fm_synth_connectable_parent_interface;

  ags_fm_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_fm_synth_connect;
  connectable->disconnect = ags_fm_synth_disconnect;
}

void
ags_fm_synth_init(AgsFMSynth *fm_synth)
{
  AgsAudio *audio;
  GtkHBox *hbox;
  GtkVBox *vbox;
  GtkTable *table;
  GtkLabel *label;

  g_signal_connect_after((GObject *) fm_synth, "parent_set",
			 G_CALLBACK(ags_fm_synth_parent_set_callback), (gpointer) fm_synth);

  audio = AGS_MACHINE(fm_synth)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_SYNTH));
  g_object_set(audio,
	       "min-audio-channels", 1,
	       "max-audio-channels", 1,
	       "min-output-pads", 1,
	       "max-output-pads", 128,
	       "min-input-pads", 1,
	       NULL);

  AGS_MACHINE(fm_synth)->input_pad_type = AGS_TYPE_FM_SYNTH_INPUT_PAD;
  AGS_MACHINE(fm_synth)->input_line_type = AGS_TYPE_FM_SYNTH_INPUT_LINE;
  AGS_MACHINE(fm_synth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(fm_synth)->output_line_type = G_TYPE_NONE;

  //  AGS_MACHINE(fm_synth)->flags |= AGS_MACHINE_IS_SYNTHESIZER;
  AGS_MACHINE(fm_synth)->mapping_flags |= AGS_MACHINE_MONO;
  
  /* create widgets */
  fm_synth->flags = 0;
 
  fm_synth->name = NULL;
  fm_synth->xml_type = "ags-fm-synth";

  fm_synth->mapped_input_pad = 0;
  fm_synth->mapped_output_pad = 0;
 
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) (gtk_bin_get_child((GtkBin *) fm_synth)), (GtkWidget *) hbox);

  fm_synth->input_pad = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  AGS_MACHINE(fm_synth)->input = (GtkContainer *) fm_synth->input_pad;
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) AGS_MACHINE(fm_synth)->input,
		     FALSE,
		     FALSE,
		     0);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) vbox, FALSE, FALSE, 0);

  fm_synth->auto_update = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("auto update"));
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) fm_synth->auto_update, FALSE, FALSE, 0);

  fm_synth->update = (GtkButton *) gtk_button_new_with_label(i18n("update"));
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) fm_synth->update, FALSE, FALSE, 0);

  /* table */
  table = (GtkTable *) gtk_table_new(3, 2, FALSE);
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) table, FALSE, FALSE, 0);

  /* frequency */  
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

  fm_synth->lower = (GtkSpinButton *) gtk_spin_button_new_with_range(AGS_FM_SYNTH_BASE_NOTE_MIN,
								     AGS_FM_SYNTH_BASE_NOTE_MAX,
								     1.0);
  fm_synth->lower->adjustment->value = -48.0;
  gtk_table_attach(table,
		   GTK_WIDGET(fm_synth->lower),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* output - discard messages */
  g_hash_table_insert(ags_machine_generic_output_message_monitor,
		      fm_synth,
		      ags_machine_generic_output_message_monitor_timeout);

  g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0,
		(GSourceFunc) ags_machine_generic_output_message_monitor_timeout,
		(gpointer) fm_synth);
}

void
ags_fm_synth_finalize(GObject *gobject)
{
  g_hash_table_remove(ags_machine_generic_output_message_monitor,
		      gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fm_synth_parent_class)->finalize(gobject);
}

void
ags_fm_synth_connect(AgsConnectable *connectable)
{
  AgsFMSynth *fm_synth;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_fm_synth_parent_connectable_interface->connect(connectable);

  /* AgsFMSynth */
  fm_synth = AGS_FM_SYNTH(connectable);

  g_signal_connect((GObject *) fm_synth->lower, "value-changed",
		   G_CALLBACK(ags_fm_synth_lower_callback), fm_synth);

  g_signal_connect((GObject *) fm_synth->auto_update, "toggled",
		   G_CALLBACK(ags_fm_synth_auto_update_callback), fm_synth);

  g_signal_connect((GObject *) fm_synth->update, "clicked",
		   G_CALLBACK(ags_fm_synth_update_callback), (gpointer) fm_synth);
}

void
ags_fm_synth_disconnect(AgsConnectable *connectable)
{
  AgsFMSynth *fm_synth;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_fm_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsFMSynth */
  fm_synth = AGS_FM_SYNTH(connectable);

  g_object_disconnect((GObject *) fm_synth->lower,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_fm_synth_lower_callback),
		      fm_synth,
		      NULL);

  g_object_disconnect((GObject *) fm_synth->auto_update,
		      "any_signal::toggled",
		      G_CALLBACK(ags_fm_synth_auto_update_callback),
		      fm_synth,
		      NULL);
  
  g_object_disconnect((GObject *) fm_synth->update,
		      "any_signal::clicked",
		      G_CALLBACK(ags_fm_synth_update_callback),
		      (gpointer) fm_synth,
		      NULL);
}

void
ags_fm_synth_map_recall(AgsMachine *machine)
{
  AgsFMSynth *fm_synth;

  AgsAudio *audio;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }
  
  AGS_MACHINE_CLASS(ags_fm_synth_parent_class)->map_recall(machine);
}

void
ags_fm_synth_update(AgsFMSynth *fm_synth)
{
  AgsWindow *window;
  AgsFMOscillator *fm_oscillator;
  
  AgsAudio *audio;
  AgsChannel *start_output, *start_input;
  AgsChannel *channel, *next_channel;
  AgsRecycling *first_recycling;
  
  AgsClearAudioSignal *clear_audio_signal;
  AgsApplySynth *apply_synth;

  AgsApplicationContext *application_context;
  
  GList *input_pad, *input_pad_start;
  GList *input_line, *input_line_start;
  GList *rt_template;
  GList *task;
  
  guint output_lines;
  guint buffer_size;
  guint attack, frame_count;
  gdouble frequency, phase, start_frequency;
  gdouble volume;
  gdouble fm_lfo_frequency, fm_lfo_depth;
  gdouble fm_tuning;
  
  AgsComplex **sync_point;
  guint sync_point_count;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) fm_synth);

  audio = AGS_MACHINE(fm_synth)->audio;

  /*  */
  start_frequency = (gdouble) gtk_spin_button_get_value_as_float(fm_synth->lower);

  /* clear output */
  input_pad_start = 
    input_pad = gtk_container_get_children((GtkContainer *) fm_synth->input_pad);

  g_object_get(audio,
	       "output", &start_output,
	       "input", &start_input,
	       "output-lines", &output_lines,
	       "buffer-size", &buffer_size,
	       NULL);

  task = NULL;

  channel = start_output;

  if(channel != NULL){
    g_object_ref(channel);
  }

  next_channel = NULL;
  
  while(channel != NULL){
    AgsAudioSignal *template;

    GList *start_list;
    
    g_object_get(channel,
		 "first-recycling", &first_recycling,
		 NULL);

    g_object_get(first_recycling,
		 "audio-signal", &start_list,
		 NULL);
    
    /*  */
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
  
  /* write output */  
  while(input_pad != NULL){
    AgsChannel *input;

    GList *start_synth_generator, *synth_generator;

    guint format;
    guint i;
    gboolean do_sync;
    
    input_line = gtk_container_get_children((GtkContainer *) AGS_PAD(input_pad->data)->expander_set);
    fm_oscillator = AGS_FM_OSCILLATOR(gtk_container_get_children((GtkContainer *) AGS_LINE(input_line->data)->expander->table)->data);

    g_object_get(AGS_LINE(input_line->data),
		 "channel", &input,
		 NULL);

    g_object_get(input,
		 "synth-generator", &start_synth_generator,
		 "format", &format,
		 NULL);
    
    synth_generator = start_synth_generator;

    /* do it so */    
    attack = (guint) gtk_spin_button_get_value_as_int(fm_oscillator->attack);
    frame_count = (guint) gtk_spin_button_get_value_as_int(fm_oscillator->frame_count);
    phase = gtk_spin_button_get_value(fm_oscillator->phase);
    frequency = gtk_spin_button_get_value(fm_oscillator->frequency);
    volume = gtk_spin_button_get_value(fm_oscillator->volume);

    fm_lfo_frequency = gtk_spin_button_get_value(fm_oscillator->fm_lfo_frequency);
    fm_lfo_depth = gtk_spin_button_get_value(fm_oscillator->fm_lfo_depth);

    fm_tuning = gtk_spin_button_get_value(fm_oscillator->fm_tuning);

    g_object_set(synth_generator->data,
		 "format", format,
		 "delay", (gdouble) attack / (gdouble) buffer_size,
		 "attack", attack,
		 "frame-count", frame_count,
		 "oscillator", gtk_combo_box_get_active(fm_oscillator->wave),
		 "frequency", frequency,
		 "phase", phase,
		 "volume", volume,
		 "do-fm-synth", TRUE,
		 "fm-lfo-oscillator", gtk_combo_box_get_active(fm_oscillator->fm_lfo_wave),
		 "fm-lfo-frequency", fm_lfo_frequency,
		 "fm-lfo-depth", fm_lfo_depth,
		 "fm-tuning", fm_tuning,
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
				      start_output,
				      start_frequency, output_lines);
    task = g_list_prepend(task,
			  apply_synth);

    g_list_free_full(start_synth_generator,
		     g_object_unref);

    /* iterate */
    input_pad = input_pad->next;
  }

  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }
  
  g_list_free(input_pad_start);
  
  ags_xorg_application_context_schedule_task_list(application_context,
						  g_list_reverse(task));
}

/**
 * ags_fm_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsFMSynth
 *
 * Returns: the new #AgsFMSynth
 *
 * Since: 2.3.0
 */
AgsFMSynth*
ags_fm_synth_new(GObject *soundcard)
{
  AgsFMSynth *fm_synth;

  fm_synth = (AgsFMSynth *) g_object_new(AGS_TYPE_FM_SYNTH,
					 NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(fm_synth)->audio),
	       "output-soundcard", soundcard,
	       NULL);

  return(fm_synth);
}
