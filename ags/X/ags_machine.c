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

#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_effect_bridge.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <ags/i18n.h>

void ags_machine_class_init(AgsMachineClass *machine);
void ags_machine_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_plugin_interface_init(AgsPluginInterface *plugin);
void ags_machine_init(AgsMachine *machine);
void ags_machine_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec);
void ags_machine_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec);
static void ags_machine_finalize(GObject *gobject);

void ags_machine_connect(AgsConnectable *connectable);
void ags_machine_disconnect(AgsConnectable *connectable);

gchar* ags_machine_get_version(AgsPlugin *plugin);
void ags_machine_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_machine_get_build_id(AgsPlugin *plugin);
void ags_machine_set_build_id(AgsPlugin *plugin, gchar *build_id);

void ags_machine_show(GtkWidget *widget);

void ags_machine_real_resize_audio_channels(AgsMachine *machine,
					    guint new_size, guint old_size);
void ags_machine_real_resize_pads(AgsMachine *machine,
				  GType channel_type,
				  guint new_size, guint old_size);
void ags_machine_real_map_recall(AgsMachine *machine);
GList* ags_machine_real_find_port(AgsMachine *machine);

GtkMenu* ags_machine_popup_new(AgsMachine *machine);

/**
 * SECTION:ags_machine
 * @short_description: visualize audio object.
 * @title: AgsMachine
 * @section_id:
 * @include: ags/X/ags_machine.h
 *
 * #AgsMachine is a composite widget to act as base class to visualize #AgsAudio.
 */

#define AGS_DEFAULT_MACHINE "ags-default-machine"

enum{
  SAMPLERATE_CHANGED,
  BUFFER_SIZE_CHANGED,
  FORMAT_CHANGED,
  RESIZE_AUDIO_CHANNELS,
  RESIZE_PADS,
  MAP_RECALL,
  FIND_PORT,
  STOP,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_AUDIO,
  PROP_MACHINE_NAME,
};

static gpointer ags_machine_parent_class = NULL;
static guint machine_signals[LAST_SIGNAL];

GHashTable *ags_machine_message_monitor = NULL;
GHashTable *ags_machine_generic_output_message_monitor = NULL;
GHashTable *ags_machine_generic_input_message_monitor = NULL;

GType
ags_machine_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine = 0;

    static const GTypeInfo ags_machine_info = {
      sizeof (AgsMachineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_machine_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine = g_type_register_static(GTK_TYPE_HANDLE_BOX,
					      "AgsMachine", &ags_machine_info,
					      0);
    
    g_type_add_interface_static(ags_type_machine,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_class_init(AgsMachineClass *machine)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_machine_parent_class = g_type_class_peek_parent(machine);

  /* GObjectClass */
  gobject = (GObjectClass *) machine;
  
  gobject->set_property = ags_machine_set_property;
  gobject->get_property = ags_machine_get_property;

  gobject->finalize = ags_machine_finalize;

  /* properties */
  /**
   * AgsMachine:samplerate:
   *
   * The samplerate.
   * 
   * Since: 2.1.35
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("samplerate"),
				 i18n_pspec("The samplerate"),
				 0,
				 G_MAXUINT32,
				 AGS_SOUNDCARD_DEFAULT_SAMPLERATE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsMachine:buffer-size:
   *
   * The buffer length.
   * 
   * Since: 2.1.35
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("buffer size"),
				 i18n_pspec("The buffer size"),
				 0,
				 G_MAXUINT32,
				 AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsMachine:format:
   *
   * The format.
   * 
   * Since: 2.1.35
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("format"),
				 i18n_pspec("The format"),
				 0,
				 G_MAXUINT32,
				 AGS_SOUNDCARD_DEFAULT_FORMAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsMachine:audio:
   *
   * The assigned #AgsAudio to visualize.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("assigned audio"),
				   i18n_pspec("The audio it is assigned to"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsMachine:machine-name:
   *
   * The machine's name.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("machine-name",
				   i18n_pspec("machine name"),
				   i18n_pspec("The machine's name"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE_NAME,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) machine;

  widget->show = ags_machine_show;

  /* AgsMachineClass */
  machine->samplerate_changed = NULL;
  machine->buffer_size_changed = NULL;
  machine->format_changed = NULL;

  machine->resize_pads = ags_machine_real_resize_pads;
  machine->resize_audio_channels = ags_machine_real_resize_audio_channels;
  machine->map_recall = ags_machine_real_map_recall;
  machine->find_port = ags_machine_real_find_port;
  machine->stop = NULL;

  /* signals */
  /**
   * AgsMachine::samplerate-changed:
   * @machine: the #AgsMachine
   * @samplerate: the samplerate
   * @old_samplerate: the old samplerate
   *
   * The ::samplerate-changed signal notifies about changed samplerate.
   * 
   * Since: 2.1.35
   */
  machine_signals[SAMPLERATE_CHANGED] =
    g_signal_new("samplerate-changed",
		 G_TYPE_FROM_CLASS(machine),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMachineClass, samplerate_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMachine::buffer-size-changed:
   * @machine: the #AgsMachine
   * @buffer_size: the buffer size
   * @old_buffer_size: the old buffer size
   *
   * The ::buffer-size-changed signal notifies about changed buffer size.
   * 
   * Since: 2.1.35
   */
  machine_signals[BUFFER_SIZE_CHANGED] =
    g_signal_new("buffer-size-changed",
		 G_TYPE_FROM_CLASS(machine),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMachineClass, buffer_size_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMachine::format-changed:
   * @machine: the #AgsMachine
   * @format: the format
   * @old_format: the old format
   *
   * The ::format-changed signal notifies about changed format.
   * 
   * Since: 2.1.35
   */
  machine_signals[FORMAT_CHANGED] =
    g_signal_new("format-changed",
		 G_TYPE_FROM_CLASS(machine),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMachineClass, format_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMachine::resize-audio-channels:
   * @machine: the #AgsMachine to modify
   * @new_size: the new size
   * @old_size: the old size
   *
   * The ::resize-audio-channels signal notifies about changed channel allocation within
   * audio.
   * 
   * Since: 2.0.0
   */
  machine_signals[RESIZE_AUDIO_CHANNELS] =
    g_signal_new("resize-audio-channels",
		 G_TYPE_FROM_CLASS(machine),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMachineClass, resize_audio_channels),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMachine::resize-pads:
   * @machine: the #AgsMachine to modify
   * @channel_type: either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
   * @new_size: the new size
   * @old_size: the old size
   *
   * The ::resize-pads signal notifies about changed channel allocation within
   * audio.
   * 
   * Since: 2.0.0
   */
  machine_signals[RESIZE_PADS] =
    g_signal_new("resize-pads",
		 G_TYPE_FROM_CLASS(machine),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMachineClass, resize_pads),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__ULONG_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_ULONG,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMachine::map-recall:
   * @machine: the #AgsMachine
   *
   * The ::map-recall should be used to add the machine's default recall.
   * 
   * Since: 2.0.0
   */
  machine_signals[MAP_RECALL] =
    g_signal_new("map-recall",
                 G_TYPE_FROM_CLASS(machine),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMachineClass, map_recall),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE, 0);

  /**
   * AgsMachine::find-port:
   * @machine: the #AgsMachine to resize
   * Returns: a #GList with associated ports
   *
   * The ::find-port signal emits as recall should be mapped.
   * 
   * Since: 2.0.0
   */
  machine_signals[FIND_PORT] =
    g_signal_new("find-port",
		 G_TYPE_FROM_CLASS(machine),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMachineClass, find_port),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsMachine::stop:
   * @machine: the #AgsMachine
   * @recall_id: the #GList-struct containing #AgsRecallID
   * @sound_scope: the sound scope
   *
   * The ::stop signal gets emited as audio stops playback.
   * 
   * Since: 2.0.0
   */
  machine_signals[STOP] =
    g_signal_new("stop",
                 G_TYPE_FROM_CLASS(machine),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMachineClass, stop),
                 NULL, NULL,
                 ags_cclosure_marshal_VOID__POINTER_INT,
                 G_TYPE_NONE, 2,
		 G_TYPE_POINTER, G_TYPE_INT);
}

void
ags_machine_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_connect;
  connectable->disconnect = ags_machine_disconnect;
}

void
ags_machine_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_machine_get_version;
  plugin->set_version = ags_machine_set_version;
  plugin->get_build_id = ags_machine_get_build_id;
  plugin->set_build_id = ags_machine_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->get_ports = NULL;
}

void
ags_machine_init(AgsMachine *machine)
{
  GtkVBox *vbox;
  GtkFrame *frame;

  if(ags_machine_message_monitor == NULL){
    ags_machine_message_monitor = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							NULL,
							NULL);
  }

  if(ags_machine_generic_output_message_monitor == NULL){
    ags_machine_generic_output_message_monitor = g_hash_table_new_full(g_direct_hash, g_direct_equal,
								       NULL,
								       NULL);
  }

  if(ags_machine_generic_input_message_monitor == NULL){
    ags_machine_generic_input_message_monitor = g_hash_table_new_full(g_direct_hash, g_direct_equal,
								      NULL,
								      NULL);
  }

  if(ags_machine_message_monitor == NULL){
    ags_machine_message_monitor = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							NULL,
							NULL);
  }
  
  g_hash_table_insert(ags_machine_message_monitor,
		      machine,
		      ags_machine_message_monitor_timeout);

  g_timeout_add(1000 / 30,
		(GSourceFunc) ags_machine_message_monitor_timeout,
		(gpointer) machine);

  machine->machine_name = NULL;

  machine->version = AGS_MACHINE_DEFAULT_VERSION;
  machine->build_id = AGS_MACHINE_DEFAULT_BUILD_ID;

  machine->flags = 0;
  machine->file_input_flags = 0;
  machine->mapping_flags = 0;
  machine->connection_flags = 0;
  
  machine->output_pad_type = G_TYPE_NONE;
  machine->input_pad_type = G_TYPE_NONE;

  machine->bank_0 = 0;
  machine->bank_1 = 0;

  vbox = (GtkVBox *) gtk_vbox_new(FALSE,
				  0);
  
  frame = (GtkFrame *) gtk_frame_new(NULL);
  gtk_container_add((GtkContainer *) machine,
		    (GtkWidget *) frame);

  machine->audio = ags_audio_new(NULL);
  g_object_ref(G_OBJECT(machine->audio));

  machine->samplerate = machine->audio->samplerate;
  machine->buffer_size = machine->audio->buffer_size;
  machine->format = machine->audio->format;
  
  machine->audio->flags |= AGS_AUDIO_CAN_NEXT_ACTIVE;
  machine->audio->machine_widget = (GObject *) machine;

  /* AgsAudio related forwarded signals */
  g_signal_connect_after(G_OBJECT(machine), "resize-audio-channels",
			 G_CALLBACK(ags_machine_resize_audio_channels_callback), NULL);

  g_signal_connect_after(G_OBJECT(machine), "resize-pads",
			 G_CALLBACK(ags_machine_resize_pads_callback), NULL);

  g_signal_connect_after(G_OBJECT(machine), "stop",
			 G_CALLBACK(ags_machine_stop_callback), NULL);
  
  machine->play = NULL;

  machine->output = NULL;
  machine->selected_output_pad = NULL;

  machine->input = NULL;
  machine->selected_input_pad = NULL;
  
  machine->bridge = NULL;

  machine->port = NULL;
  machine->enabled_automation_port = NULL;

  machine->popup = ags_machine_popup_new(machine);
  g_object_ref(machine->popup);
  
  machine->menu_tool_button = (GtkMenuToolButton *) g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
								 "label", "machine",
								 "menu", machine->popup,
								 NULL);
  gtk_frame_set_label_widget(frame,
			     (GtkWidget *) machine->menu_tool_button);
  machine->properties = NULL;
  machine->rename = NULL;
  machine->rename_audio = NULL;
  machine->reposition_audio = NULL;
  machine->connection_editor = NULL;
  machine->midi_dialog = NULL;
  machine->envelope_dialog = NULL;

  machine->application_context = NULL;
}

void
ags_machine_set_property(GObject *gobject,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *param_spec)
{
  AgsWindow *window;
  AgsMachine *machine;

  machine = AGS_MACHINE(gobject);
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      GList *start_list, *list;
      
      guint samplerate, old_samplerate;
      
      samplerate = g_value_get_uint(value);
      old_samplerate = machine->samplerate;

      if(samplerate == old_samplerate){
	return;
      }
      
      machine->samplerate = samplerate;

      ags_machine_samplerate_changed(machine,
				     samplerate, old_samplerate);

      if(machine->output != NULL){
	list = 
	  start_list = gtk_container_get_children(GTK_CONTAINER(machine->output));

	while(list != NULL){
	  if(AGS_IS_PAD(list->data)){
	    g_object_set(list->data,
			 "samplerate", samplerate,
			 NULL);
	  }

	  list = list->next;
	}

	g_list_free(start_list);
      }

      if(machine->input != NULL){
	list = 
	  start_list = gtk_container_get_children(GTK_CONTAINER(machine->input));

	while(list != NULL){
	  if(AGS_IS_PAD(list->data)){
	    g_object_set(list->data,
			 "samplerate", samplerate,
			 NULL);
	  }

	  list = list->next;
	}

	g_list_free(start_list);
      }

      if(machine->bridge != NULL){
	g_object_set(machine->bridge,
		     "samplerate", samplerate,
		     NULL);
      }
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      GList *start_list, *list;
      
      guint buffer_size, old_buffer_size;
      
      buffer_size = g_value_get_uint(value);
      old_buffer_size = machine->buffer_size;

      if(buffer_size == old_buffer_size){
	return;
      }
      
      machine->buffer_size = buffer_size;

      ags_machine_buffer_size_changed(machine,
				      buffer_size, old_buffer_size);


      if(machine->output != NULL){
	list = 
	  start_list = gtk_container_get_children(GTK_CONTAINER(machine->output));

	while(list != NULL){
	  if(AGS_IS_PAD(list->data)){
	    g_object_set(list->data,
			 "buffer-size", buffer_size,
			 NULL);
	  }

	  list = list->next;
	}

	g_list_free(start_list);
      }

      if(machine->input != NULL){
	list = 
	  start_list = gtk_container_get_children(GTK_CONTAINER(machine->input));

	while(list != NULL){
	  if(AGS_IS_PAD(list->data)){
	    g_object_set(list->data,
			 "buffer-size", buffer_size,
			 NULL);
	  }

	  list = list->next;
	}

	g_list_free(start_list);
      }

      if(machine->bridge != NULL){
	g_object_set(machine->bridge,
		     "buffer-size", buffer_size,
		     NULL);
      }
    }
    break;
  case PROP_FORMAT:
    {
      GList *start_list, *list;
      
      guint format, old_format;
      
      format = g_value_get_uint(value);
      old_format = machine->format;

      if(format == old_format){
	return;
      }

      machine->format = format;

      ags_machine_format_changed(machine,
				 format, old_format);


      if(machine->output != NULL){
	list = 
	  start_list = gtk_container_get_children(GTK_CONTAINER(machine->output));

	while(list != NULL){
	  if(AGS_IS_PAD(list->data)){
	    g_object_set(list->data,
			 "format", format,
			 NULL);
	  }

	  list = list->next;
	}

	g_list_free(start_list);
      }

      if(machine->input != NULL){
	list = 
	  start_list = gtk_container_get_children(GTK_CONTAINER(machine->input));

	while(list != NULL){
	  if(AGS_IS_PAD(list->data)){
	    g_object_set(list->data,
			 "format", format,
			 NULL);
	  }

	  list = list->next;
	}

	g_list_free(start_list);
      }

      if(machine->bridge != NULL){
	g_object_set(machine->bridge,
		     "format", format,
		     NULL);
      }
    }
    break;
  case PROP_AUDIO:
    {
      AgsAudio *audio;
      gboolean reset;

      audio = (AgsAudio *) g_value_get_object(value);
      
      reset = TRUE;

      if(machine->audio != NULL){
	GList *pad;
	GList *list;

	g_object_unref(G_OBJECT(machine->audio));

	if(audio == NULL){
	  /* destroy pad */
	  pad = gtk_container_get_children(machine->output);
	  //	  pad = g_list_nth(pad, audio->output_pads);
	  
	  while(pad != NULL){
	    gtk_widget_destroy(pad->data);

	    pad = pad->next;
	  }

	  pad = gtk_container_get_children(machine->input);
	  //	  pad = g_list_nth(pad, audio->input_pads);
	  
	  while(pad != NULL){
	    gtk_widget_destroy(pad->data);
	    
	    pad = pad->next;
	  }
	  
	  reset = FALSE;
	}
      }
      
      if(audio != NULL){
	g_object_ref(G_OBJECT(audio));
	machine->audio = audio;

	if(reset){
	  AgsChannel *input, *output;
	  
	  GList *start_pad, *pad;
	  GList *start_line, *line;

	  guint audio_channels;
	  guint output_pads, input_pads;
	  guint i;

	  g_object_get(audio,
		       "audio-channels", &audio_channels,
		       "output-pads", &output_pads,
		       "input-pads", &input_pads,
		       "output", &output,
		       "input", &input,
		       NULL);
	  g_object_unref(output);
	  g_object_unref(input);

	  /* set channel and resize for AgsOutput */
	  if(machine->output_pad_type != G_TYPE_NONE){	    
	    pad =
	      start_pad = gtk_container_get_children(machine->output);

	    i = 0;

	    while(pad != NULL && output != NULL){
	      line =
		start_line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(pad->data)->expander_set));

	      ags_pad_resize_lines(AGS_PAD(pad->data), machine->output_line_type,
				   audio_channels, g_list_length(line));
	      g_object_set(G_OBJECT(pad->data),
			   "channel", output,
			   NULL);

	      g_list_free(start_line);

	      /* iterate */
	      g_object_get(output,
			   "next-pad", &output,
			   NULL);

	      if(output != NULL){
		g_object_unref(output);
	      }
	      
	      pad = pad->next;
	      i++;
	    }

	    g_list_free(start_pad);
	    
	    if(output != NULL){
	      AgsPad *pad;

	      /* add pad */
	      for(; i < audio->output_pads; i++){
		pad = g_object_new(machine->output_pad_type,
				   "channel", output,
				   NULL);
		gtk_container_add(machine->output,
				  GTK_WIDGET(pad));

		ags_pad_resize_lines(pad, machine->output_line_type,
				     audio_channels, 0);
	      }
	    }else{
	      /* destroy pad */
	      pad =
		start_pad = gtk_container_get_children(machine->output);
	      pad = g_list_nth(pad,
			       output_pads);

	      while(pad != NULL){
		gtk_widget_destroy(pad->data);

		pad = pad->next;
	      }

	      g_list_free(start_pad);
	    }
	  }

	  /* set channel and resize for AgsOutput */
	  if(machine->input_pad_type != G_TYPE_NONE){
	    pad =
	      start_pad = gtk_container_get_children(machine->input);

	    i = 0;

	    while(pad != NULL && input != NULL){
	      line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(pad->data)->expander_set));

	      ags_pad_resize_lines(AGS_PAD(pad->data), machine->input_line_type,
				   audio_channels, g_list_length(line));
	      g_object_set(G_OBJECT(pad->data),
			   "channel", input,
			   NULL);

	      g_list_free(line);

	      /* iterate */
	      g_object_get(input,
			   "next-pad", &input,
			   NULL);

	      if(input != NULL){
		g_object_unref(input);
	      }
	      
	      pad = pad->next;
	      i++;
	    }

	    g_list_free(start_pad);

	    if(input != NULL){
	      AgsPad *pad;

	      /* add pad */
	      for(; i < audio->input_pads; i++){
		pad = g_object_new(machine->input_pad_type,
				   "channel", input,
				   NULL);
		gtk_container_add(machine->input,
				  GTK_WIDGET(pad));

		ags_pad_resize_lines(pad, machine->input_line_type,
				     audio_channels, 0);
	      }
	    }else{
	      /* destroy pad */
	      pad =
		start_pad = gtk_container_get_children(machine->input);
	      pad = g_list_nth(pad, audio->input_pads);

	      while(pad != NULL){
		gtk_widget_destroy(pad->data);

		pad = pad->next;
	      }	      

	      g_list_free(start_pad);
	    }
	  }
	}else{
	  AgsPad *pad;

	  AgsChannel *input, *output;
	  AgsChannel *channel;
	  
	  guint audio_channels;
	  guint output_pads, input_pads;
	  guint i;

	  g_object_get(audio,
		       "audio-channels", &audio_channels,
		       "output-pads", &output_pads,
		       "input-pads", &input_pads,
		       "output", &output,
		       "input", &input,
		       NULL);
	  g_object_unref(output);
	  g_object_unref(input);

	  /* add pad */
	  if(machine->output_pad_type != G_TYPE_NONE){
	    channel = output;

	    for(i = 0; i < output_pads; i++){
	      pad = g_object_new(machine->output_pad_type,
				 "channel", channel,
				 NULL);
	      gtk_container_add(machine->output,
				GTK_WIDGET(pad));	  
	      ags_pad_resize_lines(pad, machine->output_line_type,
				   audio_channels, 0);

	      g_object_get(channel,
			   "next-pad", &channel,
			   NULL);

	      if(channel != NULL){
		g_object_unref(channel);
	      }
	    }
	  }

	  if(machine->input_pad_type != G_TYPE_NONE){
	    channel = input;

	    for(i = 0; i < input_pads; i++){
	      pad = g_object_new(machine->input_pad_type,
				 "channel", channel,
				 NULL);
	      gtk_container_add(machine->output,
				GTK_WIDGET(pad));
	      ags_pad_resize_lines(pad, machine->input_line_type,
				   audio_channels, 0);

	      g_object_get(channel,
			   "next-pad", &channel,
			   NULL);

	      if(channel != NULL){
		g_object_unref(channel);
	      }
	    }
	  }
	}
      }else{
	machine->audio = NULL;
      }
    }
    break;
  case PROP_MACHINE_NAME:
    {
      gchar *machine_name;
      gchar *str;
      
      machine_name = g_value_get_string(value);

      if(machine_name == machine->machine_name){
	return;
      }

      if(machine->machine_name != NULL){
	g_free(machine->machine_name);
      }

      machine->machine_name = g_strdup(machine_name);

      /* update UI */
      str = g_strdup_printf("%s: %s",
			    G_OBJECT_TYPE_NAME(machine),
			    machine_name);
      
      g_object_set(machine->menu_tool_button,
		   "label", str,
		   NULL);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_get_property(GObject *gobject,
			 guint prop_id,
			 GValue *value,
			 GParamSpec *param_spec)
{
  AgsMachine *machine;

  machine = AGS_MACHINE(gobject);

  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value,
		       machine->samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value,
		       machine->buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value,
		       machine->format);
    }
    break;
  case PROP_AUDIO:
    {
      g_value_set_object(value, machine->audio);
    }
    break;
  case PROP_MACHINE_NAME:
    {
      g_value_set_string(value, machine->machine_name);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

static void
ags_machine_finalize(GObject *gobject)
{
  AgsMachine *machine;

  AgsAudio *audio;
  
  AgsApplicationContext *application_context;
  
  GList *list, *list_start;

  char *str;

  machine = (AgsMachine *) gobject;

  application_context = ags_application_context_get_instance();
  
  /* remove from sound provider */
  list =
    list_start = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

  list_start = g_list_remove(list_start,
			     machine->audio);
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(application_context),
			       list_start);

  g_object_unref(machine->audio);
  g_list_foreach(list_start,
		 (GFunc) g_object_unref,
		 NULL);
  
  /* remove message monitor */
  g_hash_table_remove(ags_machine_message_monitor,
		      machine);

  g_list_free_full(machine->enabled_automation_port,
		   (GDestroyNotify) ags_machine_automation_port_free);
  
  //TODO:JK: better clean-up of audio
  
  if(machine->properties != NULL){
    gtk_widget_destroy((GtkWidget *) machine->properties);
  }

  if(machine->rename != NULL){
    gtk_widget_destroy((GtkWidget *) machine->rename);
  }

  if(machine->rename_audio != NULL){
    gtk_widget_destroy((GtkWidget *) machine->rename_audio);
  }

  if(machine->reposition_audio != NULL){
    gtk_widget_destroy((GtkWidget *) machine->reposition_audio);
  }
  
  if(machine->machine_name != NULL){
    g_free(machine->machine_name);
  }

  audio = machine->audio;

  /* call parent */
  G_OBJECT_CLASS(ags_machine_parent_class)->finalize(gobject);

  if(audio != NULL){
    g_object_unref(G_OBJECT(audio));
  }
}

void
ags_machine_connect(AgsConnectable *connectable)
{
  AgsMachine *machine;

  GList *list_start, *list;

  /* AgsMachine */
  machine = AGS_MACHINE(connectable);

  if((AGS_MACHINE_CONNECTED & (machine->flags)) != 0){
    return;
  }

  machine->flags |= AGS_MACHINE_CONNECTED;

  if((AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) == 0){
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) == 0){
      ags_machine_map_recall(machine);
    }
  }else{
    //    machine->flags &= ~AGS_MACHINE_PREMAPPED_RECALL;
#ifdef AGS_DEBUG
    g_message("find port");
#endif
    
    ags_machine_find_port(machine);
  }

  if(machine->bridge != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(machine->bridge));
  }
  
  if(machine->play != NULL){
    g_signal_connect(G_OBJECT(machine->play), "clicked",
		     G_CALLBACK(ags_machine_play_callback), (gpointer) machine);
  }

  /* GtkWidget */
  //  g_signal_connect(G_OBJECT (machine), "button_press_event",
  //		   G_CALLBACK(ags_machine_button_press_callback), (gpointer) machine);

  /* AgsPad - input */
  if(machine->input != NULL){
    list_start =
      list = gtk_container_get_children(GTK_CONTAINER(machine->input));

    while(list != NULL){
      ags_connectable_connect(AGS_CONNECTABLE(list->data));
      
      list = list->next;
    }

    g_list_free(list_start);
  }

  /* AgsPad - output */
  if(machine->output != NULL){
    list_start =
      list = gtk_container_get_children(GTK_CONTAINER(machine->output));
    
    while(list != NULL){
      ags_connectable_connect(AGS_CONNECTABLE(list->data));
      
      list = list->next;
    }

    g_list_free(list_start);
  }
}

void
ags_machine_disconnect(AgsConnectable *connectable)
{
  AgsMachine *machine;

  GList *list_start, *list;

  /* AgsMachine */
  machine = AGS_MACHINE(connectable);

  if((AGS_MACHINE_CONNECTED & (machine->flags)) == 0){
    return;
  }

  machine->flags &= (~AGS_MACHINE_CONNECTED);

  if(machine->bridge != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(machine->bridge));
  }

  /* AgsPad - input */
  if(machine->input != NULL){
    list_start =
      list = gtk_container_get_children(GTK_CONTAINER(machine->input));

    while(list != NULL){
      ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
      
      list = list->next;
    }

    g_list_free(list_start);
  }

  /* AgsPad - output */
  if(machine->output != NULL){
    list_start =
      list = gtk_container_get_children(GTK_CONTAINER(machine->output));
    
    while(list != NULL){
      ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
      
      list = list->next;
    }

    g_list_free(list_start);
  }

  //TODO:JK: implement me
  g_signal_handlers_disconnect_by_data(machine->audio,
				       machine);
}


gchar*
ags_machine_get_version(AgsPlugin *plugin)
{
  return(AGS_MACHINE(plugin)->version);
}

void
ags_machine_set_version(AgsPlugin *plugin, gchar *version)
{
  AGS_MACHINE(plugin)->version = version;

  //TODO:JK: implement me
}

gchar*
ags_machine_get_build_id(AgsPlugin *plugin)
{
  return(AGS_MACHINE(plugin)->build_id);
}

void
ags_machine_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AGS_MACHINE(plugin)->build_id = build_id;

  //TODO:JK: implement me
}

void
ags_machine_show(GtkWidget *widget)
{
  AgsMachine *machine;
  AgsWindow *window;
  GtkFrame *frame;

  GTK_WIDGET_CLASS(ags_machine_parent_class)->show(widget);

  machine = (AgsMachine *) widget;

  GTK_WIDGET_CLASS(ags_machine_parent_class)->show(widget);

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) widget);

  frame = (GtkFrame *) gtk_container_get_children((GtkContainer *) machine)->data;
  gtk_widget_show_all((GtkWidget *) frame);
}

/**
 * ags_machine_automation_port_alloc:
 * @channel_type: the #GType of channel
 * @control_name: the control name as string
 * 
 * Allocate #AgsMachineAutomationPort
 * 
 * Returns: the new allocated #AgsMachineAutomationPort
 * 
 * Since: 2.0.0
 */
AgsMachineAutomationPort*
ags_machine_automation_port_alloc(GType channel_type, gchar *control_name)
{
  AgsMachineAutomationPort *automation_port;

  automation_port = (AgsMachineAutomationPort *) malloc(sizeof(AgsMachineAutomationPort));

  automation_port->channel_type = channel_type;
  automation_port->control_name = g_strdup(control_name);

  return(automation_port);
}

/**
 * ags_machine_automation_port_free:
 * @automation_port: the #AgsAutomationPort
 * 
 * Free @automation_port
 * 
 * Since: 2.0.0
 */
void
ags_machine_automation_port_free(AgsMachineAutomationPort *automation_port)
{
  if(automation_port == NULL){
    return;
  }
  
  g_free(automation_port->control_name);

  g_free(automation_port);
}

/**
 * ags_machine_automation_port_find_channel_type_with_control_name:
 * @list: a #GList-struct containing #AgsAutomationPort
 * @channel_type: the #GType to match
 * @control_name: the control name as string to match
 * 
 * Find #AgsAutomationPort by specifying channel type and control name.
 * 
 * Returns: the matching #AgsAutomationPort or %NULL
 * 
 * Since: 2.0.0
 */
GList*
ags_machine_automation_port_find_channel_type_with_control_name(GList *list,
								GType channel_type, gchar *control_name)
{
  while(list != NULL){
    if(AGS_MACHINE_AUTOMATION_PORT(list->data)->channel_type == channel_type &&
       !g_strcmp0(AGS_MACHINE_AUTOMATION_PORT(list->data)->control_name,
		  control_name)){
      return(list);
    }
    
    list = list->next;
  }

  return(NULL);
}

/**
 * ags_machine_samplerate_changed:
 * @machine: the #AgsMachine
 * @samplerate: the samplerate
 * @old_samplerate: the old samplerate
 * 
 * Notify about samplerate changed.
 * 
 * Since: 2.1.35
 */
void
ags_machine_samplerate_changed(AgsMachine *machine,
			       guint samplerate, guint old_samplerate)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));

  g_object_ref((GObject *) machine);
  g_signal_emit(G_OBJECT(machine),
		machine_signals[SAMPLERATE_CHANGED], 0,
		samplerate,
		old_samplerate);
  g_object_unref((GObject *) machine);
}

/**
 * ags_machine_buffer_size_changed:
 * @machine: the #AgsMachine
 * @buffer_size: the buffer_size
 * @old_buffer_size: the old buffer_size
 * 
 * Notify about buffer_size changed.
 * 
 * Since: 2.1.35
 */
void
ags_machine_buffer_size_changed(AgsMachine *machine,
				guint buffer_size, guint old_buffer_size)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));

  g_object_ref((GObject *) machine);
  g_signal_emit(G_OBJECT(machine),
		machine_signals[BUFFER_SIZE_CHANGED], 0,
		buffer_size,
		old_buffer_size);
  g_object_unref((GObject *) machine);
}

/**
 * ags_machine_format_changed:
 * @machine: the #AgsMachine
 * @format: the format
 * @old_format: the old format
 * 
 * Notify about format changed.
 * 
 * Since: 2.1.35
 */
void
ags_machine_format_changed(AgsMachine *machine,
			   guint format, guint old_format)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));

  g_object_ref((GObject *) machine);
  g_signal_emit(G_OBJECT(machine),
		machine_signals[FORMAT_CHANGED], 0,
		format,
		old_format);
  g_object_unref((GObject *) machine);
}

void
ags_machine_real_resize_audio_channels(AgsMachine *machine,
				       guint audio_channels, guint audio_channels_old)
{
  AgsAudio *audio;
  AgsChannel *channel;

  GList *list_output_pad, *list_output_pad_start;
  GList *list_input_pad, *list_input_pad_start;
  GList *list_output_pad_next, *list_output_pad_next_start;
  GList *list_input_pad_next, *list_input_pad_next_start;

  guint i, j;

  audio = machine->audio;
  
  if(audio_channels > audio_channels_old){
    /* grow lines */
    AgsPad *pad;

    GList *list, *list_start;

    guint input_pads, output_pads;

    g_object_get(audio,
		 "output-pads", &output_pads,
		 "input-pads", &input_pads,
		 NULL);

    if(machine->input != NULL){
      list_input_pad_start = 
	list_input_pad = g_list_reverse(gtk_container_get_children((GtkContainer *) machine->input));
    }else{
      list_input_pad_start = NULL;
    }

    if(machine->output != NULL){
      list_output_pad_start = 
	list_output_pad = g_list_reverse(gtk_container_get_children((GtkContainer *) machine->output));
    }else{
      list_output_pad_start = NULL;
    }
    
    /* AgsInput */
    if(machine->input != NULL){
      /* get input */
      g_object_get(audio,
		   "input", &channel,
		   NULL);
      g_object_unref(channel);

      for(i = 0; i < input_pads; i++){
	/* create AgsPad's if necessary or resize */
	if(audio_channels_old == 0){
	  pad = g_object_new(machine->input_pad_type,
			     "channel", channel,
			     NULL);
	  gtk_box_pack_start((GtkBox *) machine->input,
			     (GtkWidget *) pad,
			     FALSE, FALSE,
			     0);

	  ags_pad_resize_lines((AgsPad *) pad, machine->input_line_type,
			       audio_channels, 0);
	}else{
	  pad = AGS_PAD(list_input_pad->data);

	  ags_pad_resize_lines((AgsPad *) pad, machine->input_line_type,
			       audio_channels, audio_channels_old);
	}

	/* iterate */
	g_object_get(channel,
		     "next-pad", &channel,
		     NULL);

	if(channel != NULL){
	  g_object_unref(channel);
	}
	
	if(audio_channels_old != 0){
	  list_input_pad = list_input_pad->next;
	}
      }
    }

    /* AgsOutput */
    if(machine->output != NULL){
      /* get output */
      g_object_get(audio,
		   "output", &channel,
		   NULL);

      g_object_unref(channel);

      for(i = 0; i < output_pads; i++){
	/* create AgsPad's if necessary or resize */
	if(audio_channels_old == 0){
	  pad = g_object_new(machine->output_pad_type,
			     "channel", channel,
			     NULL);
	  gtk_box_pack_start((GtkBox *) machine->output,
			     (GtkWidget *) pad,
			     FALSE, FALSE,
			     0);
	  ags_pad_resize_lines((AgsPad *) pad, machine->output_line_type,
			       audio_channels, 0);
	}else{
	  pad = AGS_PAD(list_output_pad->data);

	  ags_pad_resize_lines((AgsPad *) pad, machine->output_line_type,
			       audio_channels, audio_channels_old);
	}

	/* iterate */
	g_object_get(channel,
		     "next-pad", &channel,
		     NULL);

	if(channel != NULL){
	  g_object_unref(channel);
	}

	if(audio_channels_old != 0){
	  list_output_pad = list_output_pad->next;
	}
      }
    }

    /* show all */
    if(audio_channels_old == 0){
      if(machine->input != NULL){
	list_input_pad_start = 
	  list_input_pad = g_list_reverse(gtk_container_get_children((GtkContainer *) machine->input));
      }else{
	list_input_pad_start = NULL;
      }

      if(machine->output != NULL){
	list_output_pad_start = 
	  list_output_pad = g_list_reverse(gtk_container_get_children((GtkContainer *) machine->output));
      }else{
	list_output_pad_start = NULL;
      }
    }
    
    if(gtk_widget_get_visible((GtkWidget *) machine)){
      if(audio_channels_old == 0){
	/* AgsInput */
	if(machine->input != NULL){
	  list_input_pad = list_input_pad_start;

	  while(list_input_pad != NULL){
	    gtk_widget_show_all(GTK_WIDGET(list_input_pad->data));

	    list_input_pad = list_input_pad->next;
	  }
	}
	
	/* AgsOutput */
	if(machine->output != NULL){
	  list_output_pad = list_output_pad_start;
	  
	  while(list_input_pad != NULL){
	    gtk_widget_show_all(GTK_WIDGET(list_input_pad->data));
	    
	    list_input_pad = list_input_pad->next;
	  }
	}
      }else{
	if(machine->input != NULL){
	  GList *list_input_line, *list_input_line_start;

	  list_input_pad = list_input_pad_start;

	  while(list_input_pad != NULL){
	    list_input_line_start =
	      list_input_line = g_list_reverse(gtk_container_get_children(GTK_CONTAINER(AGS_PAD(list_input_pad->data)->expander_set)));
	    list_input_line = g_list_nth(list_input_line,
					 audio_channels_old);
	    
	    while(list_input_line != NULL){
	      gtk_widget_show_all(GTK_WIDGET(list_input_line->data));

	      list_input_line = list_input_line->next;
	    }

	    g_list_free(list_input_line_start);
	    
	    list_input_pad = list_input_pad->next;
	  }
	}
	
	/* AgsOutput */
	if(machine->output != NULL){
	  GList *list_output_line, *list_output_line_start;
	  
	  list_output_pad = list_output_pad_start;
	  
	  while(list_output_pad != NULL){
	    list_output_line_start = 
	      list_output_line = g_list_reverse(gtk_container_get_children(GTK_CONTAINER(AGS_PAD(list_output_pad->data)->expander_set)));
	    list_output_line = g_list_nth(list_output_line,
					 audio_channels_old);
	    
	    while(list_output_line != NULL){
	      gtk_widget_show_all(GTK_WIDGET(list_output_line->data));

	      list_output_line = list_output_line->next;
	    }	    
	    
	    list_output_pad = list_output_pad->next;
	  }
	}	
      }
    }
    
    g_list_free(list_input_pad_start);
    g_list_free(list_output_pad_start);
  }else if(audio_channels < audio_channels_old){
    /* shrink lines */
    if(machine->output != NULL){
      list_output_pad_start = 
	list_output_pad = gtk_container_get_children((GtkContainer *) machine->output);
    }else{
      list_output_pad_start = NULL;
    }

    if(machine->input != NULL){
      list_input_pad_start = 
	list_input_pad = gtk_container_get_children((GtkContainer *) machine->input);
    }else{
      list_input_pad_start = NULL;
    }
	
    if(audio_channels == 0){
      /* AgsInput */
      if(machine->input != NULL){
	while(list_input_pad != NULL){
	  list_input_pad_next = list_input_pad->next;

	  gtk_widget_destroy(GTK_WIDGET(list_input_pad->data));

	  list_input_pad = list_input_pad_next;
	}
      }
      
      /* AgsOutput */
      if(machine->output != NULL){
	while(list_output_pad != NULL){
	  list_output_pad_next = list_output_pad->next;
	  
	  gtk_widget_destroy(GTK_WIDGET(list_output_pad->data));
	  
	  list_output_pad = list_output_pad_next;
	}
      }
    }else{
      /* AgsInput */
      if(machine->input != NULL){
	for(i = 0; list_input_pad != NULL; i++){
	  ags_pad_resize_lines(AGS_PAD(list_input_pad->data), machine->input_pad_type,
			       audio_channels, audio_channels_old);
	  
	  list_input_pad = list_input_pad->next;
	}
      }
      
      /* AgsOutput */
      if(machine->output != NULL){
	for(i = 0; list_output_pad != NULL; i++){
	  ags_pad_resize_lines(AGS_PAD(list_output_pad->data), machine->output_pad_type,
			       audio_channels, audio_channels_old);
	  
	  list_output_pad = list_output_pad->next;
	}
      }
    }

    g_list_free(list_output_pad_start);    
    g_list_free(list_input_pad_start);
  }
}

/**
 * ags_machine_resize_audio_channels:
 * @machine: the #AgsMachine
 * @new_size: new allocation
 * @old_size: old allocation
 *
 * Resize audio channel allocation.
 *
 * Since: 2.0.0
 */
void
ags_machine_resize_audio_channels(AgsMachine *machine,
				  guint new_size,
				  guint old_size)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));

  g_object_ref((GObject *) machine);
  g_signal_emit(G_OBJECT(machine),
		machine_signals[RESIZE_AUDIO_CHANNELS], 0,
		new_size,
		old_size);
  g_object_unref((GObject *) machine);
}

void
ags_machine_real_resize_pads(AgsMachine *machine, GType channel_type,
			     guint pads, guint pads_old)
{
  AgsPad *pad;

  AgsAudio *audio;
  AgsChannel *channel;
  AgsChannel *input, *output;
  
  GList *list_pad_start, *list_pad;

  guint audio_channels;
  guint i, j;

  audio = machine->audio;
  
  if(pads_old < pads){
    g_object_get(audio,
		 "audio-channels", &audio_channels,
		 "output", &output,
		 "input", &input,
		 NULL);

    g_object_unref(output);
    g_object_unref(input);

    /* grow input */
    if(machine->input != NULL){
      if(channel_type == AGS_TYPE_INPUT){
	channel = ags_channel_nth(input,
				  pads_old * audio_channels);
      
	for(i = pads_old; i < pads; i++){
	  /* instantiate pad */
	  pad = g_object_new(machine->input_pad_type,
			     "channel", channel,
			     NULL);
	  gtk_box_pack_start((GtkBox *) machine->input,
			     (GtkWidget *) pad, FALSE, FALSE, 0);

	  /* resize lines */
	  ags_pad_resize_lines((AgsPad *) pad, machine->input_line_type,
			       audio_channels, 0);
	  
	  /* iterate */
	  g_object_get(channel,
		       "next-pad", &channel,
		       NULL);	

	  if(channel != NULL){
	    g_object_unref(channel);
	  }
	}

	/* show all */
	list_pad_start = 
	  list_pad = gtk_container_get_children(GTK_CONTAINER(machine->input));
	list_pad = g_list_nth(list_pad,
			      pads_old);

	while(list_pad != NULL){
	  gtk_widget_show_all(GTK_WIDGET(list_pad->data));

	  list_pad = list_pad->next;
	}

	g_list_free(list_pad_start);
      }
    }
    
    /* grow output */
    if(machine->output != NULL){
      if(channel_type == AGS_TYPE_OUTPUT){
	channel = ags_channel_nth(output,
				  pads_old * audio_channels);
    
	for(i = pads_old; i < pads; i++){
	  /* instantiate pad */
	  pad = g_object_new(machine->output_pad_type,
			     "channel", channel,
			     NULL);
	  gtk_box_pack_start((GtkBox *) machine->output, (GtkWidget *) pad, FALSE, FALSE, 0);

	  /* resize lines */
	  ags_pad_resize_lines((AgsPad *) pad, machine->output_line_type,
			       audio_channels, 0);

	  /* iterate */
	  g_object_get(channel,
		       "next-pad", &channel,
		       NULL);

	  if(channel != NULL){
	    g_object_unref(channel);
	  }
	}

	/* show all */
	list_pad_start = 
	  list_pad = gtk_container_get_children(GTK_CONTAINER(machine->output));
	list_pad = g_list_nth(list_pad,
			      pads_old);

	while(list_pad != NULL){
	  gtk_widget_show_all(GTK_WIDGET(list_pad->data));

	  list_pad = list_pad->next;
	}

	g_list_free(list_pad_start);
      }
    }
  }else if(pads_old > pads){
    GList *list, *list_start;
    
    /* input - destroy AgsPad's */
    if(channel_type == AGS_TYPE_INPUT &&
       machine->input != NULL){
      for(i = 0; i < pads_old - pads; i++){
	list_start = gtk_container_get_children(GTK_CONTAINER(machine->input));
	list = g_list_nth(list_start, pads);

	if(list != NULL){
	  gtk_widget_destroy(GTK_WIDGET(list->data));
	}
	
	g_list_free(list_start);
      }
    }
    
    /* output - destroy AgsPad's */
    if(channel_type == AGS_TYPE_OUTPUT &&
       machine->output != NULL){
      for(i = 0; i < pads_old - pads; i++){
	list_start = gtk_container_get_children(GTK_CONTAINER(machine->output));
	list = g_list_nth(list_start, pads);

	if(list != NULL){
	  gtk_widget_destroy(GTK_WIDGET(list->data));
	}
	
	g_list_free(list_start);
      }
    }
  }
}

/**
 * ags_machine_resize_pads:
 * @machine: the #AgsMachine
 * @channel_type: the channel #GType
 * @new_size: new allocation
 * @old_size: old allocation
 *
 * Resize pad allocation.
 *
 * Since: 2.0.0
 */
void
ags_machine_resize_pads(AgsMachine *machine,
			GType channel_type,
			guint new_size,
			guint old_size)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));

  g_object_ref((GObject *) machine);
  g_signal_emit(G_OBJECT(machine),
		machine_signals[RESIZE_PADS], 0,
		channel_type,
		new_size,
		old_size);
  g_object_unref((GObject *) machine);
}

void
ags_machine_real_map_recall(AgsMachine *machine)
{
  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  machine->flags |= AGS_MACHINE_MAPPED_RECALL;

  ags_machine_find_port(machine);
}

/**
 * ags_machine_map_recall:
 * @machine: the #AgsMachine to add its default recall.
 *
 * You may want the @machine to add its default recall.
 * 
 * Since: 2.0.0
 */
void
ags_machine_map_recall(AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));

  g_object_ref((GObject *) machine);
  g_signal_emit((GObject *) machine,
		machine_signals[MAP_RECALL], 0);
  g_object_unref((GObject *) machine);
}

GList*
ags_machine_real_find_port(AgsMachine *machine)
{
  GList *pad, *pad_start;
  
  GList *port, *tmp_port;

  port = NULL;

  /* find output ports */
  if(machine->output != NULL){
    pad_start = 
      pad = gtk_container_get_children(machine->output);

    while(pad != NULL){
      tmp_port = ags_pad_find_port(AGS_PAD(pad->data));
      
      if(port != NULL){
	port = g_list_concat(port,
			     tmp_port);
      }else{
	port = tmp_port;
      }

      pad = pad->next;
    }

    g_list_free(pad_start);
  }

  /* find input ports */
  if(machine->input != NULL){
    pad_start = 
      pad = gtk_container_get_children(machine->input);

    while(pad != NULL){
      tmp_port = ags_pad_find_port(AGS_PAD(pad->data));
      
      if(port != NULL){
	port = g_list_concat(port,
			     tmp_port);
      }else{
	port = tmp_port;
      }

      pad = pad->next;
    }

    g_list_free(pad_start);
  }

  /* find bridge ports */
  if(machine->bridge != NULL){
    tmp_port = ags_effect_bridge_find_port((AgsEffectBridge *) machine->bridge);

    if(port != NULL){
      port = g_list_concat(port,
			   tmp_port);
    }else{
      port = tmp_port;
    }
  }
    
  return(port);
}

/**
 * ags_machine_find_port:
 * @machine: the #AgsMachine
 * Returns: an #GList containing all related #AgsPort
 *
 * Lookup ports of associated recalls.
 *
 * Since: 2.0.0
 */
GList*
ags_machine_find_port(AgsMachine *machine)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_MACHINE(machine),
		       NULL);

  g_object_ref((GObject *) machine);
  g_signal_emit((GObject *) machine,
		machine_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) machine);

  return(list);
}

/**
 * ags_machine_stop:
 * @machine: the #AgsMachine
 * @recall_id: the #GList-struct containing #AgsRecallID
 * @sound_scope: the sound scope
 *
 * Notify about to stop playback of @recall_id.
 * 
 * Since: 2.0.0
 */
void
ags_machine_stop(AgsMachine *machine, GList *recall_id, gint sound_scope)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));

  g_object_ref((GObject *) machine);
  g_signal_emit((GObject *) machine,
		machine_signals[STOP], 0,
		recall_id, sound_scope);
  g_object_unref((GObject *) machine);
}

/**
 * ags_machine_find_by_name:
 * @list: a #GList of #AgsMachine
 * @name: the name of machine
 *
 * Find the specified by @name machine.
 *
 * Returns: the matching #AgsMachine, or %NULL
 *
 * Since: 2.0.0
 */
AgsMachine*
ags_machine_find_by_name(GList *list, char *name)
{
  while(list != NULL){
    if(!g_strcmp0(AGS_MACHINE(list->data)->machine_name, name))
      return((AgsMachine *) list->data);

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_machine_set_run:
 * @machine: the #AgsMachine
 * @run: if %TRUE playback is started, otherwise stopped
 *
 * Start/stop playback of @machine.
 *
 * Since: 2.0.0
 */
void
ags_machine_set_run(AgsMachine *machine,
		    gboolean run)
{
  ags_machine_set_run_extended(machine,
			       run,
			       TRUE, TRUE, TRUE, TRUE);
}

/**
 * ags_machine_set_run_extended:
 * @machine: the #AgsMachine
 * @run: if %TRUE playback is started, otherwise stopped
 * @sequencer: if doing sequencer
 * @notation: if doing notation
 * @wave: if doing wave
 * @midi: if doing midi
 *
 * Start/stop playback of @machine.
 *
 * Since: 2.0.0
 */
void
ags_machine_set_run_extended(AgsMachine *machine,
			     gboolean run,
			     gboolean sequencer, gboolean notation, gboolean wave, gboolean midi)
{
  AgsWindow *window;

  AgsThread *gui_thread;

  AgsApplicationContext *application_context;

  GList *start_list;
  
  gboolean no_soundcard;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) machine);

  application_context = (AgsApplicationContext *) window->application_context;

  no_soundcard = FALSE;

  if((start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context))) == NULL){
    no_soundcard = TRUE;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  if(no_soundcard){
    g_message("No soundcard available");
    
    return;
  }
  
  /* get gui thread */
  gui_thread = ags_ui_provider_get_gui_thread(AGS_UI_PROVIDER(application_context));

  if(run){
    AgsStartAudio *start_audio;
    AgsStartSoundcard *start_soundcard;
    AgsStartSequencer *start_sequencer;
    GList *list;

    list = NULL;

    if(sequencer){
      /* create start task */
      start_audio = ags_start_audio_new(machine->audio,
					AGS_SOUND_SCOPE_SEQUENCER);
      list = g_list_prepend(list,
			    start_audio);
    }

    if(notation){
      /* create start task */
      start_audio = ags_start_audio_new(machine->audio,
					AGS_SOUND_SCOPE_NOTATION);
      list = g_list_prepend(list,
			    start_audio);
    }
    
    if(wave){
      /* create start task */
      start_audio = ags_start_audio_new(machine->audio,
					AGS_SOUND_SCOPE_WAVE);
      list = g_list_prepend(list,
			    start_audio);
    }

    if(midi){
      /* create start task */
      start_audio = ags_start_audio_new(machine->audio,
					AGS_SOUND_SCOPE_MIDI);
      list = g_list_prepend(list,
			    start_audio);
    }

    /* create start task */
    if(list != NULL){
      /* start soundcard */
      start_soundcard = ags_start_soundcard_new((AgsApplicationContext *) window->application_context);
      list = g_list_prepend(list,
			    start_soundcard);

      /* start sequencer */
      start_sequencer = ags_start_sequencer_new((AgsApplicationContext *) window->application_context);
      list = g_list_prepend(list,
			    start_sequencer);
      
      /* append AgsStartSoundcard and AgsStartSequencer */
      list = g_list_reverse(list);
      
      ags_gui_thread_schedule_task_list((AgsGuiThread *) gui_thread,
					list);
    }
  }else{
    AgsCancelAudio *cancel_audio;

    if(sequencer){
      /* create cancel task */
      cancel_audio = ags_cancel_audio_new(machine->audio,
					  AGS_SOUND_SCOPE_SEQUENCER);
    
      /* append AgsCancelAudio */
      ags_gui_thread_schedule_task((AgsGuiThread *) gui_thread,
				   (GObject *) cancel_audio);
    }

    if(notation){
      /* create cancel task */
      cancel_audio = ags_cancel_audio_new(machine->audio,
					  AGS_SOUND_SCOPE_NOTATION);
    
      /* append AgsCancelAudio */
      ags_gui_thread_schedule_task((AgsGuiThread *) gui_thread,
				   (GObject *) cancel_audio);
    }

    if(wave){
      /* create cancel task */
      cancel_audio = ags_cancel_audio_new(machine->audio,
					  AGS_SOUND_SCOPE_WAVE);
    
      /* append AgsCancelAudio */
      ags_gui_thread_schedule_task((AgsGuiThread *) gui_thread,
				   (GObject *) cancel_audio);
    }

    if(midi){
      /* create cancel task */
      cancel_audio = ags_cancel_audio_new(machine->audio,
					  AGS_SOUND_SCOPE_MIDI);
    
      /* append AgsCancelAudio */
      ags_gui_thread_schedule_task((AgsGuiThread *) gui_thread,
				   (GObject *) cancel_audio);
    }
  }
}

/**
 * ags_machine_get_possible_audio_output_connections:
 * @machine: the #AgsMachine
 *
 * Find audio output connections suitable for @machine.
 *
 * Returns: a #GtkListStore containing one column with a string representing
 * machines by its type and name.
 *
 * Since: 2.0.0
 */
GtkListStore*
ags_machine_get_possible_audio_output_connections(AgsMachine *machine)
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  GtkListStore *model;

  GList *start_list, *list;
  GtkTreeIter iter;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  if(window != NULL){
    application_context = (AgsApplicationContext *) window->application_context;
  }else{
    application_context = NULL;
  }
  
  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "NULL",
		     1, NULL,
		     -1);

  if(application_context != NULL){
    list =
      start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

    while(list != NULL){
      guint soundcard_capability;

      soundcard_capability = ags_soundcard_get_capability(AGS_SOUNDCARD(list->data));
      
      if((AGS_SOUNDCARD_CAPABILITY_PLAYBACK & (soundcard_capability)) != 0 ||
	 (AGS_SOUNDCARD_CAPABILITY_DUPLEX & (soundcard_capability)) != 0){
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
			   0, g_strdup_printf("%s: %s", 
					      G_OBJECT_TYPE_NAME(G_OBJECT(list->data)),
					      ags_soundcard_get_device(AGS_SOUNDCARD(list->data))),
			   1, list->data,
			   -1);
      }

      list = list->next;
    }

    g_list_free_full(start_list,
		     g_object_unref);
  }
  
  return(model);
}

/**
 * ags_machine_get_possible_audio_input_connections:
 * @machine: the #AgsMachine
 *
 * Find audio input connections suitable for @machine.
 *
 * Returns: a #GtkListStore containing one column with a string representing
 * machines by its type and name.
 *
 * Since: 2.0.0
 */
GtkListStore*
ags_machine_get_possible_audio_input_connections(AgsMachine *machine)
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;

  GtkListStore *model;

  GList *start_list, *list;
  GtkTreeIter iter;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  if(window != NULL){
    application_context = (AgsApplicationContext *) window->application_context;
  }else{
    application_context = NULL;
  }
  
  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "NULL",
		     1, NULL,
		     -1);

  if(application_context != NULL){
    list =
      start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

    while(list != NULL){
      guint soundcard_capability;

      soundcard_capability = ags_soundcard_get_capability(AGS_SOUNDCARD(list->data));

      if((AGS_SOUNDCARD_CAPABILITY_CAPTURE & (soundcard_capability)) != 0 ||
	 (AGS_SOUNDCARD_CAPABILITY_DUPLEX & (soundcard_capability)) != 0){
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
			   0, g_strdup_printf("%s: %s", 
					      G_OBJECT_TYPE_NAME(G_OBJECT(list->data)),
					      ags_soundcard_get_device(AGS_SOUNDCARD(list->data))),
			   1, list->data,
			   -1);
      }

      list = list->next;
    }

    g_list_free_full(start_list,
		     g_object_unref);
  }
  
  return(model);
}

/**
 * ags_machine_get_possible_links:
 * @machine: the #AgsMachine
 *
 * Find links suitable for @machine.
 *
 * Returns: a #GtkListStore containing one column with a string representing
 * machines by its type and name.
 *
 * Since: 2.0.0
 */
GtkListStore*
ags_machine_get_possible_links(AgsMachine *machine)
{
  GtkListStore *model;

  GList *list;
  GtkTreeIter iter;

  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "NULL",
		     1, NULL,
		     -1);

  if(GTK_WIDGET(machine)->parent != NULL){
    list = gtk_container_get_children(GTK_CONTAINER(GTK_WIDGET(machine)->parent));

    while(list != NULL){
      if(list->data != machine){
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
			   0, g_strdup_printf("%s: %s", 
					      G_OBJECT_TYPE_NAME(G_OBJECT(list->data)),
					      AGS_MACHINE(list->data)->machine_name),
			   1, list->data,
			   -1);
      }

      list = list->next;
    }
  }
  
  return(model);
}

/**
 * ags_machine_file_chooser_dialog_new:
 * @machine: the #AgsMachine
 *
 * Creates a new machine file chooser dialog in order to
 * open audio files.
 *
 * Returns: a new #GtkFileChooserDialog
 *
 * Since: 2.0.0
 */
GtkFileChooserDialog*
ags_machine_file_chooser_dialog_new(AgsMachine *machine)
{
  GtkFileChooserDialog *file_chooser;
  GtkCheckButton *check_button;

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(i18n("Open audio files"),
								      (GtkWindow *) gtk_widget_get_toplevel((GtkWidget *) machine),
								      GTK_FILE_CHOOSER_ACTION_OPEN,
								      GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
								      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
								      NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), TRUE);

  check_button = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("open in new channel"));
  gtk_toggle_button_set_active((GtkToggleButton *) check_button, TRUE);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(file_chooser)->vbox, (GtkWidget *) check_button, FALSE, FALSE, 0);
  g_object_set_data((GObject *) file_chooser, "create", (gpointer) check_button);

  check_button = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("overwrite existing links"));
  gtk_toggle_button_set_active((GtkToggleButton *) check_button, TRUE);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(file_chooser)->vbox, (GtkWidget *) check_button, FALSE, FALSE, 0);
  g_object_set_data((GObject *) file_chooser, "overwrite", (gpointer) check_button);

  return(file_chooser);
}

/**
 * ags_machine_open_files:
 * @machine: the #AgsMachine
 * @filenames: the filenames
 * @overwrite_channels: reset channels
 * @create_channels: instantiate new channels
 *
 * Opens audio files and modifies or creates new channels if wished.
 *
 * Since: 2.0.0
 */
void
ags_machine_open_files(AgsMachine *machine,
		       GSList *filenames,
		       gboolean overwrite_channels,
		       gboolean create_channels)
{
  AgsWindow *window;

  AgsOpenFile *open_file;

  AgsThread *gui_thread;

  AgsApplicationContext *application_context;
  
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) machine);
  
  application_context = (AgsApplicationContext *) window->application_context;

  /* get gui thread */
  gui_thread = ags_ui_provider_get_gui_thread(AGS_UI_PROVIDER(application_context));

  /* instantiate open file task */
  open_file = ags_open_file_new(machine->audio,
				filenames,
				overwrite_channels,
				create_channels);

  ags_gui_thread_schedule_task((AgsGuiThread *) gui_thread,
			       (GObject *) open_file);
}

void
ags_machine_copy_pattern(AgsMachine *machine)
{
  AgsAudio *audio;
  AgsChannel *channel;
  
  xmlDoc *clipboard;
  xmlNode *audio_node, *notation_list_node, *notation_node;

  xmlChar *buffer;

  guint audio_channels;
  guint input_pads;
  int size;
  gint i;

  auto xmlNode* ags_machine_copy_pattern_to_notation(AgsChannel *current);

  xmlNode* ags_machine_copy_pattern_to_notation(AgsChannel *current){
    AgsPattern *pattern;

    xmlNode *notation_node, *current_note;

    GList *start_list;
    
    guint x_boundary, y_boundary;
    guint bank_0, bank_1, k;
    
    /* create root node */
    notation_node = xmlNewNode(NULL, BAD_CAST "notation");

    xmlNewProp(notation_node, BAD_CAST "program", BAD_CAST "ags");
    xmlNewProp(notation_node, BAD_CAST "type", BAD_CAST AGS_NOTATION_CLIPBOARD_TYPE);
    xmlNewProp(notation_node, BAD_CAST "version", BAD_CAST AGS_NOTATION_CLIPBOARD_VERSION);
    xmlNewProp(notation_node, BAD_CAST "format", BAD_CAST AGS_NOTATION_CLIPBOARD_FORMAT);
    xmlNewProp(notation_node, BAD_CAST "base_frequency", BAD_CAST g_strdup("0"));
    xmlNewProp(notation_node, BAD_CAST "audio-channel", BAD_CAST g_strdup_printf("%u", current->audio_channel));

    bank_0 = machine->bank_0;
    bank_1 = machine->bank_1;
    
    x_boundary = G_MAXUINT;
    y_boundary = G_MAXUINT;

    while(current != NULL){
      guint length;
      
      pthread_mutex_t *pattern_mutex;
      
      g_object_get(current,
		   "pattern", &start_list,
		   NULL);

      pattern = start_list->data;
      g_list_free_full(start_list,
		       g_object_unref);

      /* get pattern mutex */
      pthread_mutex_lock(ags_pattern_get_class_mutex());

      pattern_mutex = pattern->obj_mutex;
      
      pthread_mutex_unlock(ags_pattern_get_class_mutex());

      /* get length */
      pthread_mutex_lock(pattern_mutex);

      length = pattern->dim[2];
      
      pthread_mutex_unlock(pattern_mutex);
      
      for(k = 0; k < length; k++){
	guint current_pad;

	g_object_get(current,
		     "pad", &current_pad,
		     NULL);
	
	if(ags_pattern_get_bit(pattern, bank_0, bank_1, k)){
	  current_note = xmlNewChild(notation_node, NULL, BAD_CAST "note", NULL);
	  
	  xmlNewProp(current_note, BAD_CAST "x", BAD_CAST g_strdup_printf("%u", k));
	  xmlNewProp(current_note, BAD_CAST "x1", BAD_CAST g_strdup_printf("%u", k + 1));

	  if((AGS_MACHINE_REVERSE_NOTATION & (machine->flags)) != 0){
	    xmlNewProp(current_note, BAD_CAST "y", BAD_CAST g_strdup_printf("%u", input_pads - current_pad - 1));
	  }else{
	    xmlNewProp(current_note, BAD_CAST "y", BAD_CAST g_strdup_printf("%u", current_pad));
	  }
	  
	  if(x_boundary > k){
	    x_boundary = k;
	  }
      
	  if((AGS_MACHINE_REVERSE_NOTATION & (machine->flags)) != 0){
	    guint tmp;

	    tmp = input_pads - current_pad - 1;
	    
	    if(y_boundary > tmp){
	      y_boundary = tmp;
	    }
	  }else{
	    if(y_boundary > current->pad){
	      y_boundary = current->pad;
	    }
	  }
	}
      }

      /* iterate */
      g_object_get(current,
		   "next", &current,
		   NULL);
 
      if(current != NULL){
	g_object_unref(current);
      }
    }

    xmlNewProp(notation_node, BAD_CAST "x_boundary", BAD_CAST g_strdup_printf("%u", x_boundary));
    xmlNewProp(notation_node, BAD_CAST "y_boundary", BAD_CAST g_strdup_printf("%u", y_boundary));

    return(notation_node);
  }
  
  /* create document */
  clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

  /* create root node */
  audio_node = xmlNewNode(NULL,
			  BAD_CAST "audio");
  xmlDocSetRootElement(clipboard, audio_node);

  notation_list_node = xmlNewNode(NULL,
				  BAD_CAST "notation-list");
  xmlAddChild(audio_node,
	      notation_list_node);
  
  audio = machine->audio;

  /* copy to clipboard */
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       "input-pads", &input_pads,
	       "input", &channel,
	       NULL);

  g_object_unref(channel);

  for(i = 0; i < audio_channels; i++){
    /* do it so */
    notation_node = ags_machine_copy_pattern_to_notation(channel);
    xmlAddChild(notation_list_node,
		notation_node);

    /* iterate */
    g_object_get(channel,
		 "next", &channel,
		 NULL);

    if(channel != NULL){
      g_object_unref(channel);
    }
  }
  
  /* write to clipboard */
  xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
  gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			 buffer, size);
  gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
  
  xmlFreeDoc(clipboard);
}

/**
 * ags_machine_message_monitor_timeout:
 * @machine: the #AgsMachine
 *
 * Monitor messages.
 *
 * Returns: %TRUE if proceed with monitor, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_machine_message_monitor_timeout(AgsMachine *machine)
{
  if(g_hash_table_lookup(ags_machine_message_monitor,
			 machine) != NULL){
    AgsMessageDelivery *message_delivery;

    GList *message_start, *message;
    
    /* retrieve message */
    message_delivery = ags_message_delivery_get_instance();

    message_start = 
      message = ags_message_delivery_find_sender(message_delivery,
						 "libags-audio",
						 (GObject *) machine->audio);
    
    while(message != NULL){
      xmlNode *root_node;

      root_node = xmlDocGetRootElement(AGS_MESSAGE_ENVELOPE(message->data)->doc);
      
      if(!xmlStrncmp(root_node->name,
		     "ags-command",
		     12)){
	if(!xmlStrncmp(xmlGetProp(root_node,
				  "method"),
		       "AgsAudio::set-samplerate",
		       25)){
	  guint samplerate;
	  gint position;
	  
	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				    "samplerate");
	  samplerate = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));

	  /* set samplerate */
	  g_object_set(machine,
		       "samplerate", samplerate,
		       NULL);
	}else if(!xmlStrncmp(xmlGetProp(root_node,
				  "method"),
		       "AgsAudio::set-buffer-size",
		       26)){
	  guint buffer_size;
	  gint position;
	  
	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				    "buffer-size");
	  buffer_size = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));

	  /* set buffer size */
	  g_object_set(machine,
		       "buffer-size", buffer_size,
		       NULL);
	}else if(!xmlStrncmp(xmlGetProp(root_node,
				  "method"),
		       "AgsAudio::set-format",
		       21)){
	  guint format;
	  gint position;
	  
	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				    "format");
	  format = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));

	  /* set format */
	  g_object_set(machine,
		       "format", format,
		       NULL);
	}else if(!xmlStrncmp(xmlGetProp(root_node,
				  "method"),
		       "AgsAudio::set-audio-channels",
		       29)){
	  guint audio_channels, audio_channels_old;
	  gint position;
	  
	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				    "audio-channels");
	  audio_channels = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));

	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				    "audio-channels-old");
	  audio_channels_old = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));

	  /* resize audio channels */
	  ags_machine_resize_audio_channels(machine,
					    audio_channels, audio_channels_old);
	}else if(!xmlStrncmp(xmlGetProp(root_node,
				  "method"),
		       "AgsAudio::set-pads",
		       19)){
	  GType channel_type;
	  
	  guint pads, pads_old;
	  gint position;

	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				     "channel-type");
	  channel_type = g_value_get_ulong(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));
	  
	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				     "pads");
	  pads = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));

	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				     "pads-old");
	  pads_old = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));

	  /* resize pads */
	  ags_machine_resize_pads(machine,
				  channel_type,
				  pads, pads_old);
	}else if(!xmlStrncmp(xmlGetProp(root_node,
				  "method"),
		       "AgsAudio::stop",
		       15)){
	  GList *recall_id;

	  gint sound_scope;
	  gint position;
	  
	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				     "recall-id");
	  recall_id = g_value_get_pointer(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));

	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				    "sound-scope");
	  sound_scope = g_value_get_int(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));
	  
	  /* stop */
	  ags_machine_stop(machine,
			   recall_id, sound_scope);
	}
      }
      
      ags_message_delivery_remove_message(message_delivery,
					  "libags-audio",
					  message->data);

      message = message->next;
    }
    
    g_list_free_full(message_start,
		     (GDestroyNotify) ags_message_envelope_free);

    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_machine_generic_output_message_monitor_timeout:
 * @machine: the #AgsMachine
 *
 * Monitor messages and discard them.
 *
 * Returns: %TRUE if proceed with monitor, otherwise %FALSE
 *
 * Since: 2.1.35
 */
gboolean
ags_machine_generic_output_message_monitor_timeout(AgsMachine *machine)
{
  if(g_hash_table_lookup(ags_machine_generic_output_message_monitor,
			 machine) != NULL){
    AgsMessageDelivery *message_delivery;

    AgsChannel *output;
    
    GList *message_start, *message;
    
    /* retrieve message */
    message_delivery = ags_message_delivery_get_instance();

    g_object_get(machine->audio,
		 "output", &output,
		 NULL);

    g_object_unref(output);

    while(output != NULL){
      message_start = 
	message = ags_message_delivery_find_sender(message_delivery,
						   "libags-audio",
						   (GObject *) output);

      while(message != NULL){
	ags_message_delivery_remove_message(message_delivery,
					    "libags-audio",
					    message->data);

	message = message->next;
      }
      
      g_list_free_full(message_start,
		       (GDestroyNotify) ags_message_envelope_free);
      
      g_object_get(output,
		   "next", &output,
		   NULL);

      if(output != NULL){
	g_object_unref(output);
      }
    }
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_machine_generic_input_message_monitor_timeout:
 * @machine: the #AgsMachine
 *
 * Monitor messages and discard them.
 *
 * Returns: %TRUE if proceed with monitor, otherwise %FALSE
 *
 * Since: 2.1.35
 */
gboolean
ags_machine_generic_input_message_monitor_timeout(AgsMachine *machine)
{
  if(g_hash_table_lookup(ags_machine_generic_input_message_monitor,
			 machine) != NULL){
    AgsMessageDelivery *message_delivery;

    AgsChannel *input;
    
    GList *message_start, *message;
    
    /* retrieve message */
    message_delivery = ags_message_delivery_get_instance();
    
    g_object_get(machine->audio,
		 "input", &input,
		 NULL);

    g_object_unref(input);

    while(input != NULL){
      message_start = 
	message = ags_message_delivery_find_sender(message_delivery,
						   "libags-audio",
						   (GObject *) input);

      while(message != NULL){
	ags_message_delivery_remove_message(message_delivery,
					    "libags-audio",
					    message->data);

	message = message->next;
      }
      
      g_list_free_full(message_start,
		       (GDestroyNotify) ags_message_envelope_free);
      
      g_object_get(input,
		   "next", &input,
		   NULL);

      if(input != NULL){
	g_object_unref(input);
      }
    }
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_machine_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsMachine
 *
 * Returns: a new #AgsMachine
 *
 * Since: 2.0.0
 */
AgsMachine*
ags_machine_new(GObject *soundcard)
{
  AgsMachine *machine;
  GValue value;

  machine = (AgsMachine *) g_object_new(AGS_TYPE_MACHINE,
					NULL);
  
  g_value_init(&value, G_TYPE_OBJECT);
  g_value_set_object(&value, soundcard);
  g_object_set_property(G_OBJECT(machine->audio),
			"soundcard", &value);
  g_value_unset(&value);

  return(machine);
}

/**
 * ags_machine_popup_new:
 * @machine: the assigned machine.
 *
 * Creates #GtkMenu to use as @machine's popup context menu.
 *
 * Returns: a new #GtkMenu containing basic actions.
 *
 * Since: 2.0.0
 */
GtkMenu*
ags_machine_popup_new(AgsMachine *machine)
{
  GtkMenu *popup;
  GtkMenuItem *item;
  GList *list, *list_start;

  popup = (GtkMenu *) gtk_menu_new();

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("move up"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("move down"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("hide"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("show"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("destroy"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("rename"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);
  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("rename audio"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("reposition audio"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("properties"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_check_menu_item_new_with_label(i18n("sticky controls"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  list_start = 
    list = gtk_container_get_children((GtkContainer *) popup);

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_move_up_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_move_down_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_hide_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_show_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_destroy_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_rename_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_rename_audio_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_reposition_audio_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_properties_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect_after((GObject*) list->data, "toggled",
			 G_CALLBACK(ags_machine_popup_sticky_controls_toggled_callback), (gpointer) machine);

  g_list_free(list_start);
  gtk_widget_show_all((GtkWidget*) popup);

  return(popup);
}

/**
 * ags_machine_popup_add_edit_options:
 * @machine: the assigned machine.
 * @edit_options: the options to set
 *
 * Add options to edit submenu
 *
 * Since: 2.0.0
 */
void
ags_machine_popup_add_edit_options(AgsMachine *machine, guint edit_options)
{
  GtkMenu *edit;
  GtkMenuItem *item;
  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("edit"));
  gtk_menu_shell_append((GtkMenuShell *) machine->popup, (GtkWidget*) item);
  gtk_widget_show((GtkWidget *) item);

  edit = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu(item,
			    (GtkWidget *) edit);

  gtk_widget_show((GtkWidget *) edit);

  if((AGS_MACHINE_POPUP_COPY_PATTERN & edit_options) != 0){
    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("copy pattern"));
    gtk_menu_shell_append((GtkMenuShell *) edit, (GtkWidget*) item);
    
    g_signal_connect((GObject*) item, "activate",
		     G_CALLBACK(ags_machine_popup_copy_pattern_callback), (gpointer) machine);

    gtk_widget_show((GtkWidget *) item);
  }
  
  if((AGS_MACHINE_POPUP_PASTE_PATTERN & edit_options) != 0){
    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("paste pattern"));
    gtk_menu_shell_append((GtkMenuShell *) edit, (GtkWidget*) item);

    g_signal_connect((GObject*) item, "activate",
		     G_CALLBACK(ags_machine_popup_paste_pattern_callback), (gpointer) machine);

    gtk_widget_show((GtkWidget *) item);
  }

  if((AGS_MACHINE_POPUP_ENVELOPE & edit_options) != 0){
    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("envelope"));
    gtk_menu_shell_append((GtkMenuShell *) edit, (GtkWidget*) item);
    
    g_signal_connect((GObject*) item, "activate",
		     G_CALLBACK(ags_machine_popup_envelope_callback), (gpointer) machine);

    gtk_widget_show((GtkWidget *) item);
  }

  gtk_widget_show_all((GtkWidget *) machine->popup);
}

/**
 * ags_machine_popup_add_connection_options:
 * @machine: the assigned machine.
 * @connection_options: the options to set
 *
 * Add options to connection submenu
 *
 * Since: 2.0.0
 */
void
ags_machine_popup_add_connection_options(AgsMachine *machine, guint connection_options)
{
  GtkMenu *connection;
  GtkMenuItem *item;

  gchar *str;

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("connection"));
  gtk_menu_shell_append((GtkMenuShell *) machine->popup, (GtkWidget*) item);
  gtk_widget_show((GtkWidget *) item);

  connection = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu(item,
			    (GtkWidget *) connection);

  gtk_widget_show((GtkWidget *) connection);

  if((AGS_MACHINE_POPUP_CONNECTION_EDITOR & connection_options) != 0){
    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("audio connection"));
    gtk_menu_shell_append((GtkMenuShell *) connection, (GtkWidget*) item);
    
    g_signal_connect((GObject*) item, "activate",
		     G_CALLBACK(ags_machine_popup_connection_editor_callback), (gpointer) machine);

    gtk_widget_show((GtkWidget *) item);
  }

  if((AGS_MACHINE_POPUP_MIDI_DIALOG & connection_options) != 0){
    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("MIDI dialog"));
    gtk_menu_shell_append((GtkMenuShell *) connection, (GtkWidget*) item);
    
    g_signal_connect((GObject*) item, "activate",
		     G_CALLBACK(ags_machine_popup_midi_dialog_callback), (gpointer) machine);

    gtk_widget_show((GtkWidget *) item);
  }

  gtk_widget_show_all((GtkWidget *) machine->popup);
}
