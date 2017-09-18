/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/thread/ags_gui_thread.h>

#include <ags/X/file/ags_gui_file_xml.h>

#include <ags/X/machine/ags_synth_input_pad.h>
#include <ags/X/machine/ags_synth_input_line.h>
#include <ags/X/machine/ags_oscillator.h>

#include <ags/X/ags_window.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <math.h>

#include <ags/i18n.h>

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
void ags_synth_read_resolve_audio(AgsFileLookup *file_lookup,
				  AgsMachine *machine);
xmlNode* ags_synth_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_synth_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old,
				  AgsSynth *synth);
void ags_synth_set_pads(AgsAudio *audio, GType type,
			guint pads, guint pads_old,
			AgsSynth *synth);

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
					    "AgsSynth", &ags_synth_info,
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
  GtkHBox *hbox;
  GtkVBox *vbox;
  GtkTable *table;
  GtkLabel *label;

  g_signal_connect_after((GObject *) synth, "parent_set",
			 G_CALLBACK(ags_synth_parent_set_callback), (gpointer) synth);

  audio = AGS_MACHINE(synth)->audio;
  audio->flags |= (AGS_AUDIO_ASYNC |
		   AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		   AGS_AUDIO_INPUT_HAS_RECYCLING |
		   AGS_AUDIO_INPUT_TAKES_SYNTH);

  AGS_MACHINE(synth)->input_pad_type = AGS_TYPE_SYNTH_INPUT_PAD;
  AGS_MACHINE(synth)->input_line_type = AGS_TYPE_SYNTH_INPUT_LINE;
  AGS_MACHINE(synth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(synth)->output_line_type = G_TYPE_NONE;

  g_signal_connect_after(G_OBJECT(AGS_MACHINE(synth)->audio), "set_audio_channels",
			 G_CALLBACK(ags_synth_set_audio_channels), synth);

  g_signal_connect_after(G_OBJECT(AGS_MACHINE(synth)->audio), "set_pads",
			 G_CALLBACK(ags_synth_set_pads), synth);

  //  AGS_MACHINE(synth)->flags |= AGS_MACHINE_IS_SYNTHESIZER;
  AGS_MACHINE(synth)->mapping_flags |= AGS_MACHINE_MONO;
  
  /* create widgets */
  synth->flags = 0;
 
  synth->name = NULL;
  synth->xml_type = "ags-synth";

  synth->mapped_input_pad = 0;
  synth->mapped_output_pad = 0;
 
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) (gtk_bin_get_child((GtkBin *) synth)), (GtkWidget *) hbox);

  synth->input_pad = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  AGS_MACHINE(synth)->input = (GtkContainer *) synth->input_pad;
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) AGS_MACHINE(synth)->input,
		     FALSE,
		     FALSE,
		     0);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) vbox, FALSE, FALSE, 0);

  synth->auto_update = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("auto update"));
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) synth->auto_update, FALSE, FALSE, 0);

  synth->update = (GtkButton *) gtk_button_new_with_label(i18n("update"));
  gtk_box_pack_start((GtkBox *) vbox, (GtkWidget *) synth->update, FALSE, FALSE, 0);

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

  synth->lower = (GtkSpinButton *) gtk_spin_button_new_with_range(AGS_SYNTH_BASE_NOTE_MIN,
								  AGS_SYNTH_BASE_NOTE_MAX,
								  1.0);
  synth->lower->adjustment->value = -48.0;
  gtk_table_attach(table,
		   GTK_WIDGET(synth->lower),
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

  synth->loop_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(synth->loop_start),
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
		      "value-changed",
		      G_CALLBACK(ags_synth_lower_callback),
		      synth,
		      NULL);

  g_object_disconnect((GObject *) synth->auto_update,
		      "toggled",
		      G_CALLBACK(ags_synth_auto_update_callback),
		      synth,
		      NULL);
  
  g_object_disconnect((GObject *) synth->update,
		      "clicked",
		      G_CALLBACK(ags_synth_update_callback),
		      (gpointer) synth,
		      NULL);
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
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));

  /* fix wrong flag */
  AGS_MACHINE(gobject)->flags &= (~AGS_MACHINE_IS_SYNTHESIZER);
  
  list = file->lookup;

  while((list = ags_file_lookup_find_by_node(list,
					     node->parent)) != NULL){
    file_lookup = AGS_FILE_LOOKUP(list->data);
    
    if(g_signal_handler_find(list->data,
			     G_SIGNAL_MATCH_FUNC,
			     0,
			     0,
			     NULL,
			     ags_file_read_machine_resolve_audio,
			     NULL) != 0){
      g_signal_connect_after(G_OBJECT(file_lookup), "resolve",
			     G_CALLBACK(ags_synth_read_resolve_audio), gobject);
      
      break;
    }

    list = list->next;
  }

  /*  */
  gtk_spin_button_set_value(gobject->lower,
			    g_ascii_strtod(xmlGetProp(node,
						      "lower"),
					   NULL));

  gtk_spin_button_set_value(gobject->loop_start,
			    g_ascii_strtod(xmlGetProp(node,
						      "loop-begin"),
					   NULL));

  gtk_spin_button_set_value(gobject->loop_end,
			    g_ascii_strtod(xmlGetProp(node,
						      "loop-end"),
					   NULL));
}

void
ags_synth_read_resolve_audio(AgsFileLookup *file_lookup,
			     AgsMachine *machine)
{
  AgsSynth *synth;

  synth = AGS_SYNTH(machine);

  g_signal_connect_after(G_OBJECT(machine->audio), "set_audio_channels",
			 G_CALLBACK(ags_synth_set_audio_channels), synth);

  g_signal_connect_after(G_OBJECT(machine->audio), "set_pads",
			 G_CALLBACK(ags_synth_set_pads), synth);

  if((AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) == 0){
    synth->mapped_output_pad = machine->audio->output_pads;
    synth->mapped_input_pad = machine->audio->input_pads;
  }else{
    synth->mapped_output_pad = machine->audio->output_pads;
    synth->mapped_input_pad = machine->audio->input_pads;
  }
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
		    "ags-synth");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", synth,
				   NULL));

  xmlNewProp(node,
	     "lower",
	     g_strdup_printf("%f", gtk_spin_button_get_value(synth->lower)));

  xmlNewProp(node,
	     "loop-begin",
	     g_strdup_printf("%f", gtk_spin_button_get_value(synth->loop_start)));

  xmlNewProp(node,
	     "loop-end",
	     g_strdup_printf("%f", gtk_spin_button_get_value(synth->loop_end)));

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
  AgsWindow *window;
  AgsOscillator *oscillator;
  
  AgsAudio *audio;
  AgsChannel *channel;

  AgsClearAudioSignal *clear_audio_signal;
  AgsApplySynth *apply_synth;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsGuiThread *gui_thread;

  AgsApplicationContext *application_context;
  
  GList *input_pad, *input_pad_start;
  GList *input_line, *input_line_start;
  GList *task;
  
  guint output_lines;
  guint wave;
  guint attack, frame_count;
  gdouble frequency, phase, start_frequency;
  guint loop_start, loop_end;
  gdouble volume;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *application_mutex;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) synth);
  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  audio = AGS_MACHINE(synth)->audio;

  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  gui_thread = (AgsGuiThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_GUI_THREAD);

  /* lookup audio mutex */
  pthread_mutex_lock(application_mutex);
    
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  start_frequency = (gdouble) gtk_spin_button_get_value_as_float(synth->lower);

  loop_start = (guint) gtk_spin_button_get_value_as_int(synth->loop_start);
  loop_end = (guint) gtk_spin_button_get_value_as_int(synth->loop_end);

  /* write input */
  input_pad_start = 
    input_pad = gtk_container_get_children((GtkContainer *) synth->input_pad);

  /* get soundcard */
  pthread_mutex_lock(audio_mutex);

  channel = audio->input;
  
  pthread_mutex_unlock(audio_mutex);

  task = NULL;

  while(input_pad != NULL){
    /* lookup channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);

    /* do it so */
    input_line = gtk_container_get_children((GtkContainer *) AGS_PAD(input_pad->data)->expander_set);
    oscillator = AGS_OSCILLATOR(gtk_container_get_children((GtkContainer *) AGS_LINE(input_line->data)->expander->table)->data);
    
    wave = (guint) gtk_combo_box_get_active(oscillator->wave) + 1;
    attack = (guint) gtk_spin_button_get_value_as_int(oscillator->attack);
    frame_count = (guint) gtk_spin_button_get_value_as_int(oscillator->frame_count);
    phase = (gdouble) gtk_spin_button_get_value_as_float(oscillator->phase);
    frequency = (gdouble) gtk_spin_button_get_value_as_float(oscillator->frequency);
    volume = (gdouble) gtk_spin_button_get_value_as_float(oscillator->volume);

    apply_synth = ags_apply_synth_new(channel, 1,
				      wave,
				      attack, frame_count,
				      frequency, phase, start_frequency,
				      volume,
				      loop_start, loop_end);

    task = g_list_prepend(task,
			  apply_synth);

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next;

    pthread_mutex_unlock(channel_mutex);

    input_pad = input_pad->next;
  }
  
  g_list_free(input_pad_start);

  /* clear output */
  input_pad_start = 
    input_pad = gtk_container_get_children((GtkContainer *) synth->input_pad);

  pthread_mutex_lock(audio_mutex);

  channel = audio->output;
  output_lines = audio->output_lines;

  pthread_mutex_unlock(audio_mutex);

  while(channel != NULL){
    AgsAudioSignal *template;
    
    /* lookup channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);

    /* clear task */
    pthread_mutex_lock(channel_mutex);

    template = ags_audio_signal_get_template(channel->first_recycling->audio_signal);

    pthread_mutex_unlock(channel_mutex);

    clear_audio_signal = ags_clear_audio_signal_new(template);
    task = g_list_prepend(task,
			  clear_audio_signal);
    
    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next;

    pthread_mutex_unlock(channel_mutex);
  }
  
  /* write output */
  pthread_mutex_lock(audio_mutex);
  
  channel = audio->output;

  pthread_mutex_unlock(audio_mutex);

  while(input_pad != NULL){
    guint sync_mode;
    gboolean do_sync;

    /* do it so */
    input_line = gtk_container_get_children((GtkContainer *) AGS_PAD(input_pad->data)->expander_set);
    oscillator = AGS_OSCILLATOR(gtk_container_get_children((GtkContainer *) AGS_LINE(input_line->data)->expander->table)->data);

    wave = (guint) gtk_combo_box_get_active(oscillator->wave) + 1;
    attack = (guint) gtk_spin_button_get_value_as_int(oscillator->attack);
    frame_count = (guint) gtk_spin_button_get_value_as_int(oscillator->frame_count);
    phase = (gdouble) gtk_spin_button_get_value_as_float(oscillator->phase);
    frequency = (gdouble) gtk_spin_button_get_value_as_float(oscillator->frequency);
    volume = (gdouble) gtk_spin_button_get_value_as_float(oscillator->volume);

    do_sync = gtk_toggle_button_get_active(oscillator->do_sync);

    sync_mode = 1 << gtk_combo_box_get_active(oscillator->sync_mode);
    
    g_object_set(apply_synth,
		 "do-sync", do_sync,
		 "sync-mode", sync_mode,
		 "base-note", synth->lower->adjustment->value,
		 NULL);

    apply_synth = ags_apply_synth_new(channel, output_lines,
				      wave,
				      attack, frame_count,
				      frequency, phase, start_frequency,
				      volume,
				      loop_start, loop_end);
    task = g_list_prepend(task,
			  apply_synth);

    input_pad = input_pad->next;
  }
  
  g_list_free(input_pad_start);
  
  ags_gui_thread_schedule_task_list(gui_thread,
				    g_list_reverse(task));
}

/**
 * ags_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsSynth
 *
 * Returns: a new #AgsSynth
 *
 * Since: 0.3
 */
AgsSynth*
ags_synth_new(GObject *soundcard)
{
  AgsSynth *synth;

  synth = (AgsSynth *) g_object_new(AGS_TYPE_SYNTH,
				    NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(synth)->audio),
	       "soundcard", soundcard,
	       NULL);

  return(synth);
}
