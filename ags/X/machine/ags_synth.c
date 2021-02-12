/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_synth_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <ags/X/machine/ags_synth_input_pad.h>
#include <ags/X/machine/ags_synth_input_line.h>
#include <ags/X/machine/ags_oscillator.h>

#include <math.h>

#include <ags/i18n.h>

void ags_synth_class_init(AgsSynthClass *synth);
void ags_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_synth_init(AgsSynth *synth);
void ags_synth_finalize(GObject *gobject);

void ags_synth_connect(AgsConnectable *connectable);
void ags_synth_disconnect(AgsConnectable *connectable);

void ags_synth_show(GtkWidget *widget);
void ags_synth_map_recall(AgsMachine *machine);

/**
 * SECTION:ags_synth
 * @short_description: synth
 * @title: AgsSynth
 * @section_id:
 * @include: ags/X/machine/ags_synth.h
 *
 * The #AgsSynth is a composite widget to act as synth.
 */

static gpointer ags_synth_parent_class = NULL;
static AgsConnectableInterface *ags_synth_parent_connectable_interface;

GType
ags_synth_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_synth = 0;

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
					    "AgsSynth", &ags_synth_info,
					    0);
    
    g_type_add_interface_static(ags_type_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_synth);
  }

  return g_define_type_id__volatile;
}

void
ags_synth_class_init(AgsSynthClass *synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_synth_parent_class = g_type_class_peek_parent(synth);

  /* GObjectClass */
  gobject = (GObjectClass *) synth;

  gobject->finalize = ags_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) synth;

  machine->map_recall = ags_synth_map_recall;
}

void
ags_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_synth_connect;
  connectable->disconnect = ags_synth_disconnect;
}

void
ags_synth_init(AgsSynth *synth)
{
  AgsAudio *audio;
  GtkBox *hbox;
  GtkBox *vbox;
  GtkGrid *grid;
  GtkLabel *label;

  g_signal_connect_after((GObject *) synth, "parent_set",
			 G_CALLBACK(ags_synth_parent_set_callback), (gpointer) synth);

  audio = AGS_MACHINE(synth)->audio;
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

  AGS_MACHINE(synth)->input_pad_type = AGS_TYPE_SYNTH_INPUT_PAD;
  AGS_MACHINE(synth)->input_line_type = AGS_TYPE_SYNTH_INPUT_LINE;
  AGS_MACHINE(synth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(synth)->output_line_type = G_TYPE_NONE;

  //  AGS_MACHINE(synth)->flags |= AGS_MACHINE_IS_SYNTHESIZER;
  AGS_MACHINE(synth)->mapping_flags |= AGS_MACHINE_MONO;
  
  /* create widgets */
  synth->flags = 0;
 
  synth->name = NULL;
  synth->xml_type = "ags-synth";

  synth->mapped_input_pad = 0;
  synth->mapped_output_pad = 0;
 
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_container_add((GtkContainer *) (gtk_bin_get_child((GtkBin *) synth)),
		    (GtkWidget *) hbox);

  synth->input_pad = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					    0);
  AGS_MACHINE(synth)->input = (GtkContainer *) synth->input_pad;
  gtk_box_pack_start(hbox,
		     (GtkWidget *) AGS_MACHINE(synth)->input,
		     FALSE,
		     FALSE,
		     0);

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENATION_VERTICAL,
				0);
  gtk_box_pack_start(hbox,
		     (GtkWidget *) vbox,
		     FALSE, FALSE,
		     0);

  synth->auto_update = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("auto update"));
  gtk_box_pack_start(vbox,
		     (GtkWidget *) synth->auto_update,
		     FALSE, FALSE,
		     0);

  synth->update = (GtkButton *) gtk_button_new_with_label(i18n("update"));
  gtk_box_pack_start(vbox,
		     (GtkWidget *) synth->update,
		     FALSE, FALSE,
		     0);

  /* grid */
  grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start(vbox,
		     (GtkWidget *) grid,
		     FALSE, FALSE,
		     0);

  /* frequency */  
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("lower"),
				    "xalign", 0.0,
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  synth->lower = (GtkSpinButton *) gtk_spin_button_new_with_range(AGS_SYNTH_BASE_NOTE_MIN,
								  AGS_SYNTH_BASE_NOTE_MAX,
								  1.0);
  gtk_spin_button_set_digits(synth->lower,
			     2);
  gtk_spin_button_set_value(synth->lower, -48.0);


  gtk_widget_set_valign((GtkWidget *) synth->lower,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) synth->lower,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) synth->lower,
		  1, 0,
		  1, 1);
}

void
ags_synth_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_synth_parent_class)->finalize(gobject);
}

void
ags_synth_connect(AgsConnectable *connectable)
{
  AgsSynth *synth;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_synth_parent_connectable_interface->connect(connectable);

  /* AgsSynth */
  synth = AGS_SYNTH(connectable);

  g_signal_connect((GObject *) synth->lower, "value-changed",
		   G_CALLBACK(ags_synth_lower_callback), synth);

  g_signal_connect((GObject *) synth->auto_update, "toggled",
		   G_CALLBACK(ags_synth_auto_update_callback), synth);

  g_signal_connect((GObject *) synth->update, "clicked",
		   G_CALLBACK(ags_synth_update_callback), (gpointer) synth);
}

void
ags_synth_disconnect(AgsConnectable *connectable)
{
  AgsSynth *synth;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsSynth */
  synth = AGS_SYNTH(connectable);

  g_object_disconnect((GObject *) synth->lower,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_synth_lower_callback),
		      synth,
		      NULL);

  g_object_disconnect((GObject *) synth->auto_update,
		      "any_signal::toggled",
		      G_CALLBACK(ags_synth_auto_update_callback),
		      synth,
		      NULL);
  
  g_object_disconnect((GObject *) synth->update,
		      "any_signal::clicked",
		      G_CALLBACK(ags_synth_update_callback),
		      (gpointer) synth,
		      NULL);
}

void
ags_synth_map_recall(AgsMachine *machine)
{
  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }
  
  AGS_MACHINE_CLASS(ags_synth_parent_class)->map_recall(machine);
}

/**
 * ags_synth_test_flags:
 * @synth: the #AgsSynth
 * @flags: the flags
 * 
 * Test @flags of @synth.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.15
 */
gboolean
ags_synth_test_flags(AgsSynth *synth, guint flags)
{
  gboolean success;
  
  if(!AGS_IS_SYNTH(synth)){
    return(FALSE);
  }

  success = ((flags & (synth->flags))) ? TRUE: FALSE;

  return(success);
}

/**
 * ags_synth_set_flags:
 * @synth: the #AgsSynth
 * @flags: the flags
 * 
 * Set @flags of @synth.
 * 
 * Since: 3.2.15
 */
void
ags_synth_set_flags(AgsSynth *synth, guint flags)
{
  if(!AGS_IS_SYNTH(synth)){
    return;
  }

  synth->flags |= flags;
}

/**
 * ags_synth_unset_flags:
 * @synth: the #AgsSynth
 * @flags: the flags
 * 
 * Unset @flags of @synth.
 * 
 * Since: 3.2.15
 */
void
ags_synth_unset_flags(AgsSynth *synth, guint flags)
{
  if(!AGS_IS_SYNTH(synth)){
    return;
  }

  synth->flags &= (~flags);
}

/**
 * ags_synth_update:
 * @synth: the #AgsSynth
 * 
 * Update @synth.
 * 
 * Since: 3.0.0
 */
void
ags_synth_update(AgsSynth *synth)
{
  AgsOscillator *oscillator;
  
  AgsAudio *audio;
  AgsChannel *start_output, *start_input;
  AgsChannel *channel, *next_channel;
  AgsRecycling *first_recycling;
  
  AgsClearAudioSignal *clear_audio_signal;
  AgsApplySynth *apply_synth;

  AgsApplicationContext *application_context;
  
  GList *start_input_pad, *input_pad;
  GList *start_input_line;
  GList *task;
  
  guint output_lines;
  guint requested_frame_count;
  guint buffer_size;
  guint attack, frame_count;
  gdouble frequency, phase, start_frequency;
  gdouble volume;

  guint sync_point_count;

  if(!AGS_IS_SYNTH(synth)){
    return;
  }
  
  application_context = ags_application_context_get_instance();

  audio = AGS_MACHINE(synth)->audio;

  /*  */
  start_frequency = (gdouble) gtk_spin_button_get_value(synth->lower);

  /* clear output */
  start_input_pad = gtk_container_get_children((GtkContainer *) synth->input_pad);

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

  input_pad = start_input_pad;

  requested_frame_count = 0;
      
  while(input_pad != NULL){
    guint current_frame_count;
    
    start_input_line = gtk_container_get_children((GtkContainer *) AGS_PAD(input_pad->data)->expander_set);
    oscillator = AGS_OSCILLATOR(gtk_container_get_children((GtkContainer *) AGS_LINE(start_input_line->data)->expander->table)->data);

    current_frame_count = gtk_spin_button_get_value(oscillator->attack) + gtk_spin_button_get_value(oscillator->frame_count);
    
    if(requested_frame_count < current_frame_count){
      requested_frame_count = current_frame_count;
    }

    g_list_free(start_input_line);
    
    /* iterate */
    input_pad = input_pad->next;
  }
  
  /* write output */  
  input_pad = start_input_pad;

  while(input_pad != NULL){
    AgsChannel *input;

    GList *start_synth_generator, *synth_generator;

    guint format;
    guint i;
    gboolean do_sync;
    
    start_input_line = gtk_container_get_children((GtkContainer *) AGS_PAD(input_pad->data)->expander_set);
    oscillator = AGS_OSCILLATOR(gtk_container_get_children((GtkContainer *) AGS_LINE(start_input_line->data)->expander->table)->data);

    g_object_get(AGS_LINE(start_input_line->data),
		 "channel", &input,
		 NULL);

    g_object_get(input,
		 "synth-generator", &start_synth_generator,
		 "format", &format,
		 NULL);
    
    synth_generator = start_synth_generator;

    /* do it so */    
    attack = (guint) gtk_spin_button_get_value_as_int(oscillator->attack);
    frame_count = (guint) gtk_spin_button_get_value_as_int(oscillator->frame_count);
    phase = gtk_spin_button_get_value(oscillator->phase);
    frequency = gtk_spin_button_get_value(oscillator->frequency);
    volume = gtk_spin_button_get_value(oscillator->volume);

    g_object_set(synth_generator->data,
		 "format", format,
		 "delay", (gdouble) attack / (gdouble) buffer_size,
		 "attack", attack,
		 "frame-count", frame_count,
		 "oscillator", gtk_combo_box_get_active(oscillator->wave),
		 "frequency", frequency,
		 "phase", phase,
		 "volume", volume,
		 NULL);

    do_sync = gtk_toggle_button_get_active((GtkToggleButton *) oscillator->do_sync);
    
    if(do_sync){
      sync_point_count = oscillator->sync_point_count;

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
	
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i][0].real = gtk_spin_button_get_value(oscillator->sync_point[2 * i]);
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i][0].imag = gtk_spin_button_get_value(oscillator->sync_point[2 * i + 1]);
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
    g_object_set(apply_synth,
		 "requested-frame-count", requested_frame_count,
		 NULL);
    
    task = g_list_prepend(task,
			  apply_synth);

    g_list_free_full(start_synth_generator,
		     g_object_unref);

    g_list_free(start_input_line);

    /* iterate */
    input_pad = input_pad->next;
  }

  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }
  
  g_list_free(start_input_pad);
  
  ags_ui_provider_schedule_task_all(AGS_UI_PROVIDER(application_context),
				    g_list_reverse(task));
}

/**
 * ags_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsSynth
 *
 * Returns: the new #AgsSynth
 *
 * Since: 3.0.0
 */
AgsSynth*
ags_synth_new(GObject *soundcard)
{
  AgsSynth *synth;

  synth = (AgsSynth *) g_object_new(AGS_TYPE_SYNTH,
				    NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(synth)->audio),
	       "output-soundcard", soundcard,
	       NULL);

  return(synth);
}
