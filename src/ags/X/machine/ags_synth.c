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

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
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

#include <ags/widget/ags_expander_set.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <ags/X/machine/ags_synth_input_pad.h>
#include <ags/X/machine/ags_synth_input_line.h>
#include <ags/X/machine/ags_oscillator.h>

#include <ags/X/ags_window.h>

#include <math.h>

void ags_synth_class_init(AgsSynthClass *synth);
void ags_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_synth_plugin_interface_init(AgsPluginInterface *plugin);
void ags_synth_init(AgsSynth *synth);
void ags_synth_finalize(GObject *gobject);
void ags_synth_connect(AgsConnectable *connectable);
void ags_synth_disconnect(AgsConnectable *connectable);
void ags_synth_show(GtkWidget *widget);
gchar* ags_synth_get_name(AgsPlugin *plugin);
void ags_synth_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_synth_get_xml_type(AgsPlugin *plugin);
void ags_synth_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_synth_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_synth_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_synth_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old,
				  gpointer data);
void ags_synth_set_pads(AgsAudio *audio, GType type,
			guint pads, guint pads_old,
			gpointer data);

void ags_synth_update(AgsSynth *synth);

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

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_synth_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_synth = g_type_register_static(AGS_TYPE_MACHINE,
					    "AgsSynth\0", &ags_synth_info,
					    0);
    
    g_type_add_interface_static(ags_type_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_synth,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
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
ags_synth_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_synth_get_name;
  plugin->set_name = ags_synth_set_name;
  plugin->get_xml_type = ags_synth_get_xml_type;
  plugin->set_xml_type = ags_synth_set_xml_type;
  plugin->read = ags_synth_read;
  plugin->write = ags_synth_write;
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
  GtkHBox *hbox;
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

  //TODO:JK: uncomment me
  //  AGS_MACHINE(synth)->flags |= AGS_MACHINE_IS_SYNTHESIZER;

  /* create widgets */
  synth->flags = 0;
 
  synth->name = NULL;
  synth->xml_type = "ags-synth\0";
 
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) (gtk_bin_get_child((GtkBin *) synth)), (GtkWidget *) hbox);

  synth->input_pad = (GtkHBox *) gtk_vbox_new(FALSE, 0);
  AGS_MACHINE(synth)->input = synth->input_pad;
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) AGS_MACHINE(synth)->input,
		     FALSE,
		     FALSE,
		     0);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) vbox, FALSE, FALSE, 0);

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
				    "xalign\0", 0.0,
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
  g_signal_connect_after(G_OBJECT(AGS_MACHINE(synth)->audio), "set_audio_channels\0",
			 G_CALLBACK(ags_synth_set_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(AGS_MACHINE(synth)->audio), "set_pads\0",
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

gchar*
ags_synth_get_name(AgsPlugin *plugin)
{
  return(AGS_SYNTH(plugin)->name);
}

void
ags_synth_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_SYNTH(plugin)->name = name;
}

gchar*
ags_synth_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_SYNTH(plugin)->xml_type);
}

void
ags_synth_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_SYNTH(plugin)->xml_type = xml_type;
}

void
ags_synth_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsSynth *gobject;
  GList *list;

  gobject = AGS_SYNTH(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gtk_spin_button_set_value(gobject->lower,
			    g_ascii_strtod(xmlGetProp(node,
						      "lower\0"),
					   NULL));

  gtk_spin_button_set_value(gobject->loop_start,
			    g_ascii_strtod(xmlGetProp(node,
						      "loop-begin\0"),
					   NULL));

  gtk_spin_button_set_value(gobject->loop_end,
			    g_ascii_strtod(xmlGetProp(node,
						      "loop-end\0"),
					   NULL));
}

xmlNode*
ags_synth_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsSynth *synth;
  xmlNode *node;
  gchar *id;

  synth = AGS_SYNTH(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-synth\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", synth,
				   NULL));

  xmlNewProp(node,
	     "lower\0",
	     g_strdup_printf("%f\0", gtk_spin_button_get_value(synth->lower)));

  xmlNewProp(node,
	     "loop-begin\0",
	     g_strdup_printf("%f\0", gtk_spin_button_get_value(synth->loop_start)));

  xmlNewProp(node,
	     "loop-end\0",
	     g_strdup_printf("%f\0", gtk_spin_button_get_value(synth->loop_end)));

  xmlAddChild(parent,
	      node);

  return(node);
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
  AgsChannel *channel;
  GList *list, *list_next, *list_start;
  guint i;

  synth = (AgsSynth *) audio->machine;

  if(type == AGS_TYPE_INPUT){
    AgsSynthInputPad *synth_input_pad;

    if(pads_old < pads){
      /*  */
      channel = ags_channel_nth(audio->input, pads_old * audio->audio_channels);

      for(i = pads_old; i < pads; i++){
	synth_input_pad = ags_synth_input_pad_new(channel);
	gtk_box_pack_start((GtkBox *) synth->input_pad,
			   (GtkWidget *) synth_input_pad, FALSE, FALSE, 0);
	ags_pad_resize_lines((AgsPad *) synth_input_pad, AGS_TYPE_SYNTH_INPUT_LINE,
			     AGS_AUDIO(channel->audio)->audio_channels, 0);

	if(GTK_WIDGET_VISIBLE((GtkWidget *) synth)){
	  ags_connectable_connect(AGS_CONNECTABLE(synth_input_pad));
	  gtk_widget_show_all((GtkWidget *) synth_input_pad);
	}

	channel = channel->next_pad;
      }
    }else{
      /* destroy AgsPad's */
      list_start = 
	list = gtk_container_get_children((GtkContainer *) synth->input_pad);
      list = g_list_nth(list, pads);

      while(list != NULL){
	list_next = list->next;

	gtk_widget_destroy((GtkWidget *) list->data);

	list = list_next;
      }

      g_list_free(list_start);
    }
  }else{
    //TODO:JK: implement me
  }  
}

void
ags_synth_update(AgsSynth *synth)
{
  AgsOscillator *oscillator;
  AgsDevout *devout;
  AgsChannel *channel;
  AgsApplySynth *apply_synth;
  GList *input_pad, *input_pad_start;
  GList *input_line, *input_line_start;
  guint wave;
  guint attack, frame_count;
  guint frequency, phase, start;
  guint loop_start, loop_end;
  gdouble volume;

  devout = AGS_DEVOUT(AGS_MACHINE(synth)->audio->devout);

  start = (guint) gtk_spin_button_get_value_as_int(synth->lower);

  loop_start = (guint) gtk_spin_button_get_value_as_int(synth->loop_start);
  loop_end = (guint) gtk_spin_button_get_value_as_int(synth->loop_end);

  /* write input */
  channel = AGS_MACHINE(synth)->audio->input;
  input_pad_start = 
    input_pad = gtk_container_get_children(synth->input_pad);

  while(input_pad != NULL){
    input_line = gtk_container_get_children(AGS_PAD(input_pad->data)->expander_set);
    oscillator = AGS_OSCILLATOR(gtk_container_get_children(AGS_LINE(input_line->data)->expander->table)->data);

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
    input_pad = input_pad->next;
  }
  
  g_list_free(input_pad_start);

  /* write output */
  channel = AGS_MACHINE(synth)->audio->output;
  input_pad_start = 
    input_pad = gtk_container_get_children(synth->input_pad);

  while(input_pad != NULL){
    input_line = gtk_container_get_children(AGS_PAD(input_pad->data)->expander_set);
    oscillator = AGS_OSCILLATOR(gtk_container_get_children(AGS_LINE(input_line->data)->expander->table)->data);

    wave = (guint) gtk_combo_box_get_active(oscillator->wave) + 1;
    attack = (guint) gtk_spin_button_get_value_as_int(oscillator->attack);
    frame_count = (guint) gtk_spin_button_get_value_as_int(oscillator->frame_count);
    phase = (guint) gtk_spin_button_get_value_as_int(oscillator->phase);
    frequency = (guint) gtk_spin_button_get_value_as_int(oscillator->frequency);
    volume = (gdouble) gtk_spin_button_get_value_as_float(oscillator->volume);

    apply_synth = ags_apply_synth_new(channel, AGS_MACHINE(synth)->audio->output_lines,
				      wave,
				      attack, frame_count,
				      frequency, phase, start,
				      volume,
				      loop_start, loop_end);

    ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread),
				AGS_TASK(apply_synth));

    input_pad = input_pad->next;
  }
  
  g_list_free(input_pad_start);
}

/**
 * ags_synth_new:
 * @devout: the assigned devout.
 *
 * Creates an #AgsSynth
 *
 * Returns: a new #AgsSynth
 *
 * Since: 0.3
 */
AgsSynth*
ags_synth_new(GObject *devout)
{
  AgsSynth *synth;

  synth = (AgsSynth *) g_object_new(AGS_TYPE_SYNTH,
				    NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(synth)->audio),
	       "devout\0", devout,
	       NULL);

  return(synth);
}
