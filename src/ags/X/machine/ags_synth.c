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

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_launch.h>
#include <ags/file/ags_file_gui.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synths.h>
#include <ags/audio/ags_recall_factory.h>
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

#include <ags/X/machine/ags_synth_bridge.h>
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
void ags_synth_map_recall(AgsMachine *machine);
gchar* ags_synth_get_name(AgsPlugin *plugin);
void ags_synth_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_synth_get_xml_type(AgsPlugin *plugin);
void ags_synth_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_synth_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_synth_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);
void ags_synth_read_resolve_audio(AgsFileLookup *file_lookup,
				  AgsMachine *machine);

void ags_synth_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old,
				  AgsSynth *synth);
void ags_synth_set_pads(AgsAudio *audio, GType type,
			guint pads, guint pads_old,
			AgsSynth *synth);

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
extern AgsApplicationContext *ags_application_context;

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

  machine->map_recall = ags_synth_map_recall;
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

  AGS_MACHINE(synth)->input_pad_type = AGS_TYPE_SYNTH_INPUT_PAD;
  AGS_MACHINE(synth)->input_line_type = AGS_TYPE_SYNTH_INPUT_LINE;
  AGS_MACHINE(synth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(synth)->output_line_type = G_TYPE_NONE;

  g_signal_connect_after(G_OBJECT(AGS_MACHINE(synth)->audio), "set_audio_channels\0",
			 G_CALLBACK(ags_synth_set_audio_channels), synth);

  g_signal_connect_after(G_OBJECT(AGS_MACHINE(synth)->audio), "set_pads\0",
			 G_CALLBACK(ags_synth_set_pads), synth);

  //TODO:JK: uncomment me
  //  AGS_MACHINE(synth)->flags |= AGS_MACHINE_IS_SYNTHESIZER;

  /* create widgets */
  synth->flags = 0;
 
  synth->name = NULL;
  synth->xml_type = "ags-synth\0";

  synth->mapped_input_pad = 0;
  synth->mapped_output_pad = 0;
 
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


  table = (GtkTable *) gtk_table_new(4, 2, FALSE);
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

  AGS_MACHINE(synth)->bridge = ags_synth_bridge_new(audio);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) AGS_MACHINE(synth)->bridge,
		     FALSE, FALSE,
		     0);
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

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_synth_parent_connectable_interface->connect(connectable);

  /* AgsSynth */
  synth = AGS_SYNTH(connectable);

  g_signal_connect((GObject *) synth->lower, "value-changed\0",
		   G_CALLBACK(ags_synth_lower_callback), synth);

  g_signal_connect((GObject *) synth->auto_update, "toggled\0",
		   G_CALLBACK(ags_synth_auto_update_callback), synth);

  g_signal_connect((GObject *) synth->update, "clicked\0",
		   G_CALLBACK(ags_synth_update_callback), (gpointer) synth);
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
ags_synth_map_recall(AgsMachine *machine)
{
  AgsSynth *synth;

  AgsAudio *audio;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  audio = machine->audio;
  synth = AGS_SYNTH(machine);

  /* ags-delay */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-stream\0",
			    0, audio->audio_channels,
			    0, audio->output_pads,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL),
			    0);

  AGS_MACHINE_CLASS(ags_synth_parent_class)->map_recall(machine);
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
  AgsFileLookup *file_lookup;
  GList *list;

  gobject = AGS_SYNTH(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", ags_application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  list = file->lookup;

  while((file_lookup = ags_file_lookup_find_by_node(list,
						    node->parent)) != NULL){
    if(g_signal_handler_find(list->data,
			     G_SIGNAL_MATCH_FUNC,
			     0,
			     0,
			     NULL,
			     ags_file_read_machine_resolve_audio,
			     NULL) != 0){
      g_signal_connect_after(G_OBJECT(file_lookup), "resolve\0",
			     G_CALLBACK(ags_synth_read_resolve_audio), gobject);
      
      break;
    }

    list = list->next;
  }

  /*  */
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

void
ags_synth_read_resolve_audio(AgsFileLookup *file_lookup,
			     AgsMachine *machine)
{
  AgsSynth *synth;

  synth = AGS_SYNTH(machine);

  g_signal_connect_after(G_OBJECT(machine->audio), "set_audio_channels\0",
			 G_CALLBACK(ags_synth_set_audio_channels), synth);

  g_signal_connect_after(G_OBJECT(machine->audio), "set_pads\0",
			 G_CALLBACK(ags_synth_set_pads), synth);
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
				   "application-context\0", ags_application_context,
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
			     AgsSynth *synth)
{
  /* empty */
}

void
ags_synth_set_pads(AgsAudio *audio, GType type,
		   guint pads, guint pads_old,
		   AgsSynth *synth)
{
  /* empty */
}

void
ags_synth_update(AgsSynth *synth)
{
  AgsOscillator *oscillator;
  AgsDevout *devout;
  AgsChannel *channel;
  AgsApplySynth *apply_synth;
  AgsThread *main_loop, *current;
  AgsTaskThread *task_thread;
  GList *input_pad, *input_pad_start;
  GList *input_line, *input_line_start;
  guint wave;
  guint attack, frame_count;
  guint frequency, phase, start;
  guint loop_start, loop_end;
  gdouble volume;

  main_loop = ags_application_context->main_loop;
  task_thread = NULL;

  current = main_loop->children;

  while(current != NULL){
    if(AGS_IS_TASK_THREAD(current)){
      task_thread = (AgsTaskThread *) current;

      break;
    }

    current = current->next;
  }

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

    ags_task_thread_append_task(task_thread,
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

    ags_task_thread_append_task(task_thread,
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
