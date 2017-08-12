/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/machine/ags_syncsynth.h>
#include <ags/X/machine/ags_syncsynth_callbacks.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_launch.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_play_notation_audio_run.h>

#include <ags/audio/task/ags_clear_audio_signal.h>
#include <ags/audio/task/ags_apply_synth.h>

#include <ags/widget/ags_expander_set.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <ags/X/file/ags_gui_file_xml.h>

#include <ags/X/machine/ags_syncsynth_input_pad.h>
#include <ags/X/machine/ags_syncsynth_input_line.h>
#include <ags/X/machine/ags_oscillator.h>

#include <ags/X/ags_window.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <math.h>

#include <ags/i18n.h>

void ags_syncsynth_class_init(AgsSyncsynthClass *syncsynth);
void ags_syncsynth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_syncsynth_plugin_interface_init(AgsPluginInterface *plugin);
void ags_syncsynth_init(AgsSyncsynth *syncsynth);
void ags_syncsynth_finalize(GObject *gobject);
void ags_syncsynth_connect(AgsConnectable *connectable);
void ags_syncsynth_disconnect(AgsConnectable *connectable);
void ags_syncsynth_show(GtkWidget *widget);
void ags_syncsynth_map_recall(AgsMachine *machine);
gchar* ags_syncsynth_get_name(AgsPlugin *plugin);
void ags_syncsynth_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_syncsynth_get_xml_type(AgsPlugin *plugin);
void ags_syncsynth_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_syncsynth_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
void ags_syncsynth_read_resolve_audio(AgsFileLookup *file_lookup,
				      AgsMachine *machine);
xmlNode* ags_syncsynth_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_syncsynth_set_audio_channels(AgsAudio *audio,
				      guint audio_channels, guint audio_channels_old,
				      AgsSyncsynth *syncsynth);
void ags_syncsynth_set_pads(AgsAudio *audio, GType type,
			    guint pads, guint pads_old,
			    AgsSyncsynth *syncsynth);

/**
 * SECTION:ags_syncsynth
 * @short_description: syncsynth
 * @title: AgsSyncsynth
 * @section_id:
 * @include: ags/X/machine/ags_syncsynth.h
 *
 * The #AgsSyncsynth is a composite widget to act as syncsynth.
 */

static gpointer ags_syncsynth_parent_class = NULL;

static AgsConnectableInterface *ags_syncsynth_parent_connectable_interface;

GType
ags_syncsynth_get_type(void)
{
  static GType ags_type_syncsynth = 0;

  if(!ags_type_syncsynth){
    static const GTypeInfo ags_syncsynth_info = {
      sizeof(AgsSyncsynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_syncsynth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSyncsynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_syncsynth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_syncsynth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_syncsynth_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_syncsynth = g_type_register_static(AGS_TYPE_MACHINE,
						"AgsSyncsynth", &ags_syncsynth_info,
						0);
    
    g_type_add_interface_static(ags_type_syncsynth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_syncsynth,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_syncsynth);
}

void
ags_syncsynth_class_init(AgsSyncsynthClass *syncsynth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_syncsynth_parent_class = g_type_class_peek_parent(syncsynth);

  /* GObjectClass */
  gobject = (GObjectClass *) syncsynth;

  gobject->finalize = ags_syncsynth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) syncsynth;

  machine->map_recall = ags_syncsynth_map_recall;
}

void
ags_syncsynth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_syncsynth_connectable_parent_interface;

  ags_syncsynth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_syncsynth_connect;
  connectable->disconnect = ags_syncsynth_disconnect;
}

void
ags_syncsynth_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_syncsynth_get_name;
  plugin->set_name = ags_syncsynth_set_name;
  plugin->get_xml_type = ags_syncsynth_get_xml_type;
  plugin->set_xml_type = ags_syncsynth_set_xml_type;
  plugin->read = ags_syncsynth_read;
  plugin->write = ags_syncsynth_write;
}

void
ags_syncsynth_init(AgsSyncsynth *syncsynth)
{
  GtkHBox *hbox;
  GtkVBox *vbox;
  GtkTable *table;
  GtkLabel *label;

  AgsAudio *audio;
  AgsRecallContainer *recall_container;
  AgsDelayAudio *delay_audio;
  AgsDelayAudioRun *play_delay_audio_run, *recall_delay_audio_run;
  AgsCountBeatsAudio *play_count_beats_audio, *recall_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run, *recall_count_beats_audio_run;
  AgsPlayNotationAudioRun *play_notation, *recall_notation;
  AgsRecallAudio *play_audio, *recall_audio;

  g_signal_connect_after((GObject *) syncsynth, "parent_set",
			 G_CALLBACK(ags_syncsynth_parent_set_callback), (gpointer) syncsynth);

  audio = AGS_MACHINE(syncsynth)->audio;
  audio->flags |= (AGS_AUDIO_ASYNC |
		   AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		   AGS_AUDIO_INPUT_HAS_RECYCLING |
		   AGS_AUDIO_INPUT_TAKES_SYNTH);

  AGS_MACHINE(syncsynth)->input_pad_type = AGS_TYPE_SYNCSYNTH_INPUT_PAD;
  AGS_MACHINE(syncsynth)->input_line_type = AGS_TYPE_SYNCSYNTH_INPUT_LINE;
  AGS_MACHINE(syncsynth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(syncsynth)->output_line_type = G_TYPE_NONE;

  g_signal_connect_after(G_OBJECT(AGS_MACHINE(syncsynth)->audio), "set_audio_channels",
			 G_CALLBACK(ags_syncsynth_set_audio_channels), syncsynth);

  g_signal_connect_after(G_OBJECT(AGS_MACHINE(syncsynth)->audio), "set_pads",
			 G_CALLBACK(ags_syncsynth_set_pads), syncsynth);

  AGS_MACHINE(syncsynth)->flags |= AGS_MACHINE_IS_SYNTHESIZER;
  AGS_MACHINE(syncsynth)->mapping_flags |= AGS_MACHINE_MONO;
  
  /* create widgets */
  syncsynth->flags = 0;
 
  syncsynth->name = NULL;
  syncsynth->xml_type = "ags-syncsynth";

  syncsynth->mapped_input_pad = 0;
  syncsynth->mapped_output_pad = 0;
 
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) (gtk_bin_get_child((GtkBin *) syncsynth)), (GtkWidget *) hbox);

  syncsynth->input_pad = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  AGS_MACHINE(syncsynth)->input = (GtkContainer *) syncsynth->input_pad;
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) AGS_MACHINE(syncsynth)->input,
		     FALSE,
		     FALSE,
		     0);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) vbox, FALSE, FALSE, 0);

  syncsynth->auto_update = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("auto update"));
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) syncsynth->auto_update, FALSE, FALSE, 0);

  syncsynth->update = (GtkButton *) gtk_button_new_with_label(i18n("update"));
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) syncsynth->update, FALSE, FALSE, 0);

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

  syncsynth->lower = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(syncsynth->lower),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* loop start */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("loop start"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  syncsynth->loop_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(syncsynth->loop_start),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* loop end */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("loop end"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  syncsynth->loop_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(syncsynth->loop_end),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_syncsynth_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_syncsynth_parent_class)->finalize(gobject);
}

void
ags_syncsynth_connect(AgsConnectable *connectable)
{
  AgsSyncsynth *syncsynth;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_syncsynth_parent_connectable_interface->connect(connectable);

  /* AgsSyncsynth */
  syncsynth = AGS_SYNCSYNTH(connectable);

  g_signal_connect((GObject *) syncsynth->lower, "value-changed",
		   G_CALLBACK(ags_syncsynth_lower_callback), syncsynth);

  g_signal_connect((GObject *) syncsynth->auto_update, "toggled",
		   G_CALLBACK(ags_syncsynth_auto_update_callback), syncsynth);

  g_signal_connect((GObject *) syncsynth->update, "clicked",
		   G_CALLBACK(ags_syncsynth_update_callback), (gpointer) syncsynth);
}

void
ags_syncsynth_disconnect(AgsConnectable *connectable)
{
  AgsSyncsynth *syncsynth;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_syncsynth_parent_connectable_interface->disconnect(connectable);

  /* AgsSyncsynth */
  syncsynth = AGS_SYNCSYNTH(connectable);

  g_object_disconnect((GObject *) syncsynth->lower,
		      "value-changed",
		      G_CALLBACK(ags_syncsynth_lower_callback),
		      syncsynth,
		      NULL);

  g_object_disconnect((GObject *) syncsynth->auto_update,
		      "toggled",
		      G_CALLBACK(ags_syncsynth_auto_update_callback),
		      syncsynth,
		      NULL);
  
  g_object_disconnect((GObject *) syncsynth->update,
		      "clicked",
		      G_CALLBACK(ags_syncsynth_update_callback),
		      (gpointer) syncsynth,
		      NULL);
}

void
ags_syncsynth_map_recall(AgsMachine *machine)
{
  AgsSyncsynth *syncsynth;

  AgsAudio *audio;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }
  
  AGS_MACHINE_CLASS(ags_syncsynth_parent_class)->map_recall(machine);
}

gchar*
ags_syncsynth_get_name(AgsPlugin *plugin)
{
  return(AGS_SYNCSYNTH(plugin)->name);
}

void
ags_syncsynth_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_SYNCSYNTH(plugin)->name = name;
}

gchar*
ags_syncsynth_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_SYNCSYNTH(plugin)->xml_type);
}

void
ags_syncsynth_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_SYNCSYNTH(plugin)->xml_type = xml_type;
}

void
ags_syncsynth_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsSyncsynth *gobject;
  AgsFileLookup *file_lookup;
  GList *list;

  gobject = AGS_SYNCSYNTH(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));
}

xmlNode*
ags_syncsynth_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsSyncsynth *syncsynth;
  xmlNode *node;
  gchar *id;

  syncsynth = AGS_SYNCSYNTH(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-syncsynth");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", syncsynth,
				   NULL));

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_syncsynth_set_audio_channels(AgsAudio *audio,
				 guint audio_channels, guint audio_channels_old,
				 AgsSyncsynth *syncsynth)
{
  /* empty */
}

void
ags_syncsynth_set_pads(AgsAudio *audio, GType type,
		       guint pads, guint pads_old,
		       AgsSyncsynth *syncsynth)
{
  /* empty */
}

void
ags_syncsynth_update(AgsSyncsynth *syncsynth)
{
}

/**
 * ags_syncsynth_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsSyncsynth
 *
 * Returns: a new #AgsSyncsynth
 *
 * Since: 0.9.7
 */
AgsSyncsynth*
ags_syncsynth_new(GObject *soundcard)
{
  AgsSyncsynth *syncsynth;

  syncsynth = (AgsSyncsynth *) g_object_new(AGS_TYPE_SYNCSYNTH,
					    NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(syncsynth)->audio),
	       "soundcard", soundcard,
	       NULL);

  return(syncsynth);
}
