/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_machine.h>
#include <ags/app/ags_machine_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_effect_bridge.h>

#include <ags/i18n.h>

void ags_machine_class_init(AgsMachineClass *machine);
void ags_machine_connectable_interface_init(AgsConnectableInterface *connectable);
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

void ags_machine_real_resize_audio_channels(AgsMachine *machine,
					    guint new_size, guint old_size);
void ags_machine_real_resize_pads(AgsMachine *machine,
				  GType channel_type,
				  guint new_size, guint old_size);
void ags_machine_real_map_recall(AgsMachine *machine);
GList* ags_machine_real_find_port(AgsMachine *machine);

void ags_machine_real_refresh_port(AgsMachine *machine);

xmlNode* ags_machine_copy_pattern_to_notation(AgsMachine *machine,
					      AgsChannel *start_current,
					      guint input_pads);



/**
 * SECTION:ags_machine
 * @short_description: visualize audio object.
 * @title: AgsMachine
 * @section_id:
 * @include: ags/app/ags_machine.h
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
  REFRESH_PORT,
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

    ags_type_machine = g_type_register_static(GTK_TYPE_GRID,
					      "AgsMachine", &ags_machine_info,
					      0);
    
    g_type_add_interface_static(ags_type_machine,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_class_init(AgsMachineClass *machine)
{
  GObjectClass *gobject;

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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("assigned audio"),
				   i18n_pspec("The audio it is assigned to"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsMachine:machine-name:
   *
   * The machine's name.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("machine-name",
				   i18n_pspec("machine name"),
				   i18n_pspec("The machine's name"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE_NAME,
				  param_spec);

  /* AgsMachineClass */
  machine->samplerate_changed = NULL;
  machine->buffer_size_changed = NULL;
  machine->format_changed = NULL;

  machine->resize_pads = ags_machine_real_resize_pads;
  machine->resize_audio_channels = ags_machine_real_resize_audio_channels;
  machine->map_recall = ags_machine_real_map_recall;
  machine->find_port = ags_machine_real_find_port;
  machine->stop = NULL;
  machine->refresh_port = ags_machine_real_refresh_port;

  /* signals */
  /**
   * AgsMachine::samplerate-changed:
   * @machine: the #AgsMachine
   * @samplerate: the samplerate
   * @old_samplerate: the old samplerate
   *
   * The ::samplerate-changed signal notifies about changed samplerate.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
   */
  machine_signals[RESIZE_PADS] =
    g_signal_new("resize-pads",
		 G_TYPE_FROM_CLASS(machine),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMachineClass, resize_pads),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__POINTER_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_POINTER,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMachine::map-recall:
   * @machine: the #AgsMachine
   *
   * The ::map-recall should be used to add the machine's default recall.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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

  /**
   * AgsMachine::refresh-port:
   * @machine: the #AgsMachine
   *
   * The ::refresh-port signal.
   * 
   * Since: 4.2.1
   */
  machine_signals[REFRESH_PORT] =
    g_signal_new("refresh-port",
                 G_TYPE_FROM_CLASS(machine),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMachineClass, refresh_port),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE, 0);
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
ags_machine_init(AgsMachine *machine)
{
  GtkBox *vbox;
  GtkBox *context_box;

  GtkBuilder *builder;

  AgsApplicationContext *application_context;

  GSimpleActionGroup *action_group;
  GSimpleAction *action;
  
  GList *start_machine;

  application_context = ags_application_context_get_instance();
  
  start_machine = ags_ui_provider_get_machine(AGS_UI_PROVIDER(application_context));
  
  start_machine = g_list_prepend(start_machine,
				 machine);
  ags_ui_provider_set_machine(AGS_UI_PROVIDER(application_context),
			      start_machine);
  
  g_signal_connect(application_context, "check-message",
		   G_CALLBACK(ags_machine_check_message_callback), machine);
  
  action_group = g_simple_action_group_new();
  gtk_widget_insert_action_group((GtkWidget *) machine,
				 "machine",
				 action_group);

  /* move up */
  action = g_simple_action_new("move_up",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_move_up_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* move down */
  action = g_simple_action_new("move_down",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_move_down_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* show */
  action = g_simple_action_new("show",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_show_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* hide */
  action = g_simple_action_new("hide",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_hide_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* destroy */
  action = g_simple_action_new("destroy",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_destroy_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* rename */
  action = g_simple_action_new("rename",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_rename_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* rename audio */
  action = g_simple_action_new("rename_audio",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_rename_audio_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* reposition audio */
  action = g_simple_action_new("reposition_audio",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_reposition_audio_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* properties */
  action = g_simple_action_new("properties",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_properties_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* sticky controls */
  action = g_simple_action_new_stateful("sticky_controls",
					NULL,
					g_variant_new_boolean(FALSE));
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_sticky_controls_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* copy pattern */
  action = g_simple_action_new("copy_pattern",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_copy_pattern_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* paste pattern */
  action = g_simple_action_new("paste_pattern",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_paste_pattern_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* envelope */
  action = g_simple_action_new("envelope",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_envelope_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* preset */
  action = g_simple_action_new("preset",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_preset_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* fresh ports */
  action = g_simple_action_new_stateful("refresh_port",
					NULL,
					g_variant_new_boolean(FALSE));
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_refresh_port_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
  
  /* audio connection */
  action = g_simple_action_new("audio_connection",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_audio_connection_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* MIDI connection */
  action = g_simple_action_new("midi_connection",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_midi_connection_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* audio_export */
  action = g_simple_action_new("audio_export",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_audio_export_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* MIDI export */
  action = g_simple_action_new("midi_export",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_midi_export_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* audio import */
  action = g_simple_action_new("audio_import",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_audio_import_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* MIDI import */
  action = g_simple_action_new("midi_import",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_machine_midi_import_callback), machine);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
  
  machine->uid = g_uuid_string_random();
  machine->machine_name = NULL;

  machine->version = AGS_MACHINE_DEFAULT_VERSION;
  machine->build_id = AGS_MACHINE_DEFAULT_BUILD_ID;

  machine->flags = 0;
  machine->connectable_flags = 0;
  machine->file_input_flags = 0;
  machine->mapping_flags = 0;
  machine->connection_flags = 0;
  
  machine->output_pad_orientation = GTK_ORIENTATION_HORIZONTAL;
  machine->output_pad_type = G_TYPE_NONE;
  machine->output_line_type = G_TYPE_NONE;

  machine->input_pad_orientation = GTK_ORIENTATION_HORIZONTAL;
  machine->input_pad_type = G_TYPE_NONE;
  machine->input_line_type = G_TYPE_NONE;

  machine->bank_0 = 0;
  machine->bank_1 = 0;

  gtk_widget_set_vexpand((GtkWidget *) machine,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) machine,
			 TRUE);

  gtk_widget_set_valign((GtkWidget *) machine,
			 GTK_ALIGN_START);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  
  machine->frame = (GtkFrame *) gtk_frame_new(NULL);

  gtk_widget_set_hexpand((GtkWidget *) machine->frame,
			 TRUE);

  gtk_grid_attach((GtkGrid *) machine,
		  machine->frame,
		  0, 0,
		  1, 1);

  machine->audio = ags_audio_new(NULL);
  g_object_ref(G_OBJECT(machine->audio));

  machine->samplerate = machine->audio->samplerate;
  machine->buffer_size = machine->audio->buffer_size;
  machine->format = machine->audio->format;
  
  machine->audio_channels = machine->audio->audio_channels;

  machine->output_pads = machine->audio->output_pads;
  machine->input_pads = machine->audio->input_pads;

  machine->audio->flags |= AGS_AUDIO_CAN_NEXT_ACTIVE;
  machine->audio->machine_widget = (GObject *) machine;

  machine->base_note = NULL;
  machine->base_key_code = 0;
  
  machine->active_playback = NULL;
  
  /* AgsAudio related forwarded signals */
  g_signal_connect_after(G_OBJECT(machine), "resize-audio-channels",
			 G_CALLBACK(ags_machine_resize_audio_channels_callback), NULL);

  g_signal_connect_after(G_OBJECT(machine), "resize-pads",
			 G_CALLBACK(ags_machine_resize_pads_callback), NULL);

  g_signal_connect_after(G_OBJECT(machine), "stop",
			 G_CALLBACK(ags_machine_stop_callback), NULL);
  
  machine->play = NULL;

  machine->output_pad = NULL;

  machine->output_pad_grid = NULL;

  machine->selected_output_pad = NULL;

  machine->input_pad = NULL;

  machine->input_pad_grid = NULL;

  machine->selected_input_pad = NULL;

  machine->machine_input_line = NULL;
  
  machine->bridge = NULL;

  machine->port = NULL;
  machine->enabled_automation_port = NULL;

  context_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				       AGS_UI_PROVIDER_DEFAULT_PADDING);
  gtk_frame_set_label_widget(machine->frame,
			     (GtkWidget *) context_box);

  machine->context_label = (GtkLabel *) gtk_label_new(NULL);
  gtk_box_append(context_box,
		 (GtkWidget *) machine->context_label);
  
  machine->context_menu_button = (GtkMenuButton *) gtk_menu_button_new();
  gtk_box_append(context_box,
		 (GtkWidget *) machine->context_menu_button);

  builder = gtk_builder_new_from_resource("/org/nongnu/gsequencer/ags/app/ui/ags_machine_menu.ui");

  machine->context_menu = gtk_builder_get_object(builder,
						 "ags-machine-menu");

  gtk_menu_button_set_menu_model(machine->context_menu_button,
				 machine->context_menu);
  
  machine->editor_model = NULL;
  machine->dialog_model = NULL;

  machine->machine_editor_dialog = NULL;
  machine->rename = NULL;
  machine->rename_audio = NULL;
  machine->reposition_audio = NULL;
  machine->connection_editor_dialog = NULL;
  machine->midi_dialog = NULL;
  machine->envelope_dialog = NULL;

  machine->preset_dialog = NULL;

  machine->midi_export_dialog = NULL;
  machine->wave_export_dialog = NULL;
  machine->midi_import_dialog = NULL;
  machine->wave_import_dialog = NULL;
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

      if(machine->output_pad != NULL){
	list = 
	  start_list = ags_machine_get_output_pad(machine);

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

      if(machine->input_pad != NULL){
	list = 
	  start_list = ags_machine_get_input_pad(machine);

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

      if(machine->output_pad != NULL){
	list = 
	  start_list = ags_machine_get_output_pad(machine);

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

      if(machine->input_pad != NULL){
	list = 
	  start_list = ags_machine_get_input_pad(machine);

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

      if(machine->output_pad != NULL){
	list = 
	  start_list = ags_machine_get_output_pad(machine);

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

      if(machine->input_pad != NULL){
	list = 
	  start_list = ags_machine_get_input_pad(machine);

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

      gtk_label_set_text(machine->context_label,
			 str);
      gtk_widget_show(machine->context_label);
      
      g_free(str);
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
  
  GList *start_list;

  application_context = ags_application_context_get_instance();

  machine = (AgsMachine *) gobject;

  g_object_disconnect(application_context,
		      "any_signal::check-message",
		      G_CALLBACK(ags_machine_check_message_callback),
		      machine,
		      NULL);

  g_free(machine->uid);

  /* remove from sound provider */
  start_list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

  start_list = g_list_remove(start_list,
			     machine->audio);
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(application_context),
			       start_list);

  g_object_unref(machine->audio);
  g_list_foreach(start_list,
		 (GFunc) g_object_unref,
		 NULL);
  
  g_list_free_full(machine->enabled_automation_port,
		   (GDestroyNotify) ags_machine_automation_port_free);  
  
  //TODO:JK: better clean-up of audio

  g_free(machine->base_note);
  
  if(machine->machine_editor_dialog != NULL){
    gtk_window_destroy((GtkWindow *) machine->machine_editor_dialog);
  }

  if(machine->rename != NULL){
    gtk_window_destroy((GtkWindow *) machine->rename);
  }

  if(machine->rename_audio != NULL){
    gtk_window_destroy((GtkWindow *) machine->rename_audio);
  }

  if(machine->reposition_audio != NULL){
    gtk_window_destroy((GtkWindow *) machine->reposition_audio);
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

  GList *start_list, *list;

  /* AgsMachine */
  machine = AGS_MACHINE(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine->connectable_flags)) != 0){
    return;
  }

  machine->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect_after(G_OBJECT(machine), "map-recall",
			 G_CALLBACK(ags_machine_map_recall_callback), NULL);

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
  if(machine->input_pad != NULL){
    list =
      start_list = ags_machine_get_input_pad(machine);

    while(list != NULL){
      ags_connectable_connect(AGS_CONNECTABLE(list->data));
      
      list = list->next;
    }

    g_list_free(start_list);
  }

  /* AgsPad - output */
  if(machine->output_pad != NULL){
    list =
      start_list = ags_machine_get_output_pad(machine);
    
    while(list != NULL){
      ags_connectable_connect(AGS_CONNECTABLE(list->data));
      
      list = list->next;
    }

    g_list_free(start_list);
  }
}

void
ags_machine_disconnect(AgsConnectable *connectable)
{
  AgsMachine *machine;

  GList *start_list, *list;

  /* AgsMachine */
  machine = AGS_MACHINE(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine->connectable_flags)) == 0){
    return;
  }

  machine->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(G_OBJECT(machine),
		      "any_signal::map-recall",
		      G_CALLBACK(ags_machine_map_recall_callback),
		      NULL,
		      NULL);

  if(machine->bridge != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(machine->bridge));
  }

  /* AgsPad - input */
  if(machine->input_pad != NULL){
    list =
      start_list = ags_machine_get_input_pad(machine);

    while(list != NULL){
      ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
      
      list = list->next;
    }

    g_list_free(start_list);
  }

  /* AgsPad - output */
  if(machine->output_pad != NULL){
    list =
      start_list = ags_machine_get_output_pad(machine);
    
    while(list != NULL){
      ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
      
      list = list->next;
    }

    g_list_free(start_list);
  }

  //TODO:JK: implement me
  g_signal_handlers_disconnect_by_data(machine->audio,
				       machine);
}

/**
 * ags_machine_input_line_sort_func:
 * @a: the #AgsMachineInputLine-struct
 * @b: another #AgsMachineInputLine-struct
 * 
 * Sort machine input line.
 * 
 * Returns: 0 if equal, -1 if smaller and 1 if bigger offset
 *
 * Since: 3.16.0
 */
gint
ags_machine_input_line_sort_func(gconstpointer a,
				 gconstpointer b)
{  
  if(a == NULL || b == NULL){
    return(0);
  }

  if(AGS_MACHINE_INPUT_LINE(a)->line == AGS_MACHINE_INPUT_LINE(b)->line){
    return(0);
  }

  if(AGS_MACHINE_INPUT_LINE(a)->line < AGS_MACHINE_INPUT_LINE(b)->line){
    return(-1);
  }else{
    return(1);
  }
}

/**
 * ags_machine_get_machine_name:
 * @machine: the #AgsMachine
 * 
 * Get machine name of @machine.
 * 
 * Returns: the machine name
 * 
 * Since: 4.0.0
 */
gchar*
ags_machine_get_machine_name(AgsMachine *machine)
{
  gchar *machine_name;

  g_return_val_if_fail(AGS_IS_MACHINE(machine), NULL);

  machine_name = NULL;

  g_object_get(machine,
	       "machine-name", &machine_name,
	       NULL);

  return(machine_name);
}

/**
 * ags_machine_get_machine_name:
 * @machine: the #AgsMachine
 * @machine_name: the machine name
 * 
 * Get machine name of @machine.
 * 
 * Since: 4.0.0
 */
void
ags_machine_set_machine_name(AgsMachine *machine,
			     gchar *machine_name)
{

  g_return_if_fail(AGS_IS_MACHINE(machine));

  g_object_set(machine,
	       "machine-name", machine_name,
	       NULL);
}

/**
 * ags_machine_input_line_alloc:
 * 
 * Allocate #AgsMachineInputLine-struct.
 * 
 * Returns: the newly allocated struct
 * 
 * Since: 3.16.0
 */
AgsMachineInputLine*
ags_machine_input_line_alloc()
{
  AgsMachineInputLine *ptr;

  ptr = (AgsMachineInputLine *) g_new(AgsMachineInputLine,
				      1);

  ptr->pad = 0;
  ptr->audio_channel = 0;

  ptr->line = 0;

  ptr->mapped_recall = FALSE;
  
  return(ptr);
}

/**
 * ags_machine_reset_pattern_envelope:
 * @machine: the #AgsMachine
 * 
 * Reset pattern envelope from preset.
 * 
 * Since: 3.6.20
 */
void
ags_machine_reset_pattern_envelope(AgsMachine *machine)
{
  AgsAudio *audio;
  AgsChannel *start_input, *input, *next_input;

  GList *start_preset, *preset;

  guint audio_channels;

  if(!AGS_IS_MACHINE(machine)){
    return;
  }
  
  audio = machine->audio;
  
  start_input = NULL;
  
  start_preset = NULL;

  audio_channels = 0;
  
  g_object_get(audio,
	       "input", &start_input,
	       "preset", &start_preset,
	       "audio-channels", &audio_channels,
	       NULL);

  /* unset all envelope */
  if(start_input != NULL){
    input = start_input;
    g_object_ref(input);

    while(input != NULL){
      GList *start_pattern, *pattern;
      
      start_pattern = NULL;
	      
      g_object_get(input,
		   "pattern", &start_pattern,
		   NULL);

      pattern = start_pattern;

      while(pattern != NULL){
	guint length;
	guint k;
	
	ags_pattern_get_dim(pattern->data,
			    NULL,
			    NULL,
			    &length);

	for(k = 0; k < length; k++){
	  AgsNote *note;

	  note = ags_pattern_get_note(pattern->data,
				      k);
		  
	  if(note != NULL){
	    ags_note_unset_flags(note,
				 AGS_NOTE_ENVELOPE);
	  }
	}
	
	pattern = pattern->next;
      }

      /* iterate */
      next_input = ags_channel_next(input);

      g_object_unref(input);

      input = next_input;
    }
  }

  /* apply envelope */
  preset = start_preset;

  while(preset != NULL){
    AgsPreset *current_preset;

    gchar *scope;

    guint pad_start;
    guint pad_end;
    guint audio_channel_start;
    guint audio_channel_end;
    guint x_start;
    guint x_end;

    GValue attack = G_VALUE_INIT;
    GValue decay = G_VALUE_INIT;
    GValue sustain = G_VALUE_INIT;
    GValue release = G_VALUE_INIT;
    GValue ratio = G_VALUE_INIT;

    GError *error;
    
    current_preset = preset->data;

    scope = NULL;
    
    g_object_get(current_preset,
		 "scope", &scope,
		 NULL);

    if(!g_strcmp0(scope,
		  "ags-envelope")){
      gboolean success;

      success = TRUE;

      audio_channel_start = 0;
      audio_channel_end = 0;

      pad_start = 0;
      pad_end = 0;

      x_start = 0;
      x_end = 0;
      
      g_object_get(current_preset,
		   "audio-channel-start", &audio_channel_start,
		   "audio-channel-end", &audio_channel_end,
		   "pad-start", &pad_start,
		   "pad-end", &pad_end,
		   "x-start", &x_start,
		   "x-end", &x_end,
		   NULL);
      
      g_value_init(&attack, AGS_TYPE_COMPLEX);
	
      error = NULL;
      ags_preset_get_parameter(current_preset,
			       "attack", &attack,
			       &error);

      if(error != NULL){
	success = FALSE;

	g_error_free(error);
      }

      g_value_init(&decay, AGS_TYPE_COMPLEX);

      error = NULL;
      ags_preset_get_parameter(current_preset,
			       "decay", &decay,
			       &error);

      if(error != NULL){
	success = FALSE;

	g_error_free(error);
      }

      g_value_init(&sustain, AGS_TYPE_COMPLEX);
	
      error = NULL;
      ags_preset_get_parameter(current_preset,
			       "sustain", &sustain,
			       &error);

      if(error != NULL){
	success = FALSE;

	g_error_free(error);
      }
	
      g_value_init(&release, AGS_TYPE_COMPLEX);
	
      error = NULL;
      ags_preset_get_parameter(current_preset,
			       "release", &release,
			       &error);

      if(error != NULL){
	success = FALSE;

	g_error_free(error);
      }

      g_value_init(&ratio, AGS_TYPE_COMPLEX);
	
      error = NULL;
      ags_preset_get_parameter(current_preset,
			       "ratio", &ratio,
			       &error);

      if(error != NULL){
	success = FALSE;

	g_error_free(error);
      }

      if(success){
	guint i, i_start, i_stop;
	guint j, j_start, j_stop;
	guint k, k_start, k_stop;
	  
	i_start = pad_start;
	i_stop = pad_end + 1;
	  
	j_start = audio_channel_start;
	j_stop = audio_channel_end + 1;
	  
	k_start = x_start;
	k_stop = x_end + 1;
	  
	for(i = i_start; i < i_stop; i++){
	  for(j = j_start; j < j_stop; j++){
	    AgsChannel *channel;
	  
	    GList *start_pattern, *pattern;
	      
	    channel = ags_channel_nth(start_input,
				      i * audio_channels + j);

	    start_pattern = NULL;
	      
	    g_object_get(channel,
			 "pattern", &start_pattern,
			 NULL);

	    pattern = start_pattern;

	    while(pattern != NULL){
	      for(k = k_start; k < k_stop; k++){
		AgsNote *note;
		  		  
		note = ags_pattern_get_note(pattern->data,
					    k);
		  
		if(note != NULL){
		  g_object_set(note,
			       "attack", g_value_get_boxed(&attack),
			       "decay", g_value_get_boxed(&decay),
			       "sustain", g_value_get_boxed(&sustain),
			       "release", g_value_get_boxed(&release),
			       "ratio", g_value_get_boxed(&ratio),
			       NULL);
		    
		  ags_note_set_flags(note,
				     AGS_NOTE_ENVELOPE);

		  g_object_unref(note);
		}
	      }
		
	      pattern = pattern->next;
	    }

	    if(channel != NULL){
	      g_object_unref(channel);
	    }
	      
	    g_list_free_full(start_pattern,
			     g_object_unref);
	  }
	}
      }

      g_value_unset(&attack);
      g_value_unset(&decay);
      g_value_unset(&sustain);
      g_value_unset(&release);
      g_value_unset(&ratio);
    }
    
    g_free(scope);

    preset = preset->next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
  
  g_list_free_full(start_preset,
		   g_object_unref);      
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
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * ags_machine_get_output_pad:
 * @machine: the #AgsMachine
 * 
 * Get line member of @machine.
 * 
 * Returns: the #GList-struct containing #AgsPad
 *
 * Since: 4.0.0
 */
GList*
ags_machine_get_output_pad(AgsMachine *machine)
{
  g_return_val_if_fail(AGS_IS_MACHINE(machine), NULL);

  return(g_list_reverse(g_list_copy(machine->output_pad)));
}

/**
 * ags_machine_add_output_pad:
 * @machine: the #AgsMachine
 * @output_pad: the #AgsPad
 * @x: the x position
 * @y: the y position
 * @width: the width
 * @height: the height
 * 
 * Add @output_pad to @machine.
 * 
 * Since: 4.0.0
 */
void
ags_machine_add_output_pad(AgsMachine *machine,
			   AgsPad *output_pad,
			   guint x, guint y,
			   guint width, guint height)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));
  g_return_if_fail(AGS_IS_PAD(output_pad));

  if(g_list_find(machine->output_pad, output_pad) == NULL){
    machine->output_pad = g_list_prepend(machine->output_pad,
					 output_pad);

    output_pad->parent_machine = machine;
    
    gtk_grid_attach(machine->output_pad_grid,
		    output_pad,
		    x, y,
		    width, height);
  }
}

/**
 * ags_machine_remove_output_pad:
 * @machine: the #AgsMachine
 * @output_pad: the #AgsPad
 * 
 * Remove @output_pad from @machine.
 * 
 * Since: 4.0.0
 */
void
ags_machine_remove_output_pad(AgsMachine *machine,
			      AgsPad *output_pad)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));
  g_return_if_fail(AGS_IS_PAD(output_pad));

  if(g_list_find(machine->output_pad, output_pad) != NULL){
    machine->output_pad = g_list_remove(machine->output_pad,
					output_pad);

    output_pad->parent_machine = NULL;
    
    gtk_grid_remove(machine->output_pad_grid,
		    output_pad);
  }
}

/**
 * ags_machine_get_input_pad:
 * @machine: the #AgsMachine
 * 
 * Get line member of @machine.
 * 
 * Returns: the #GList-struct containing #AgsPad
 *
 * Since: 4.0.0
 */
GList*
ags_machine_get_input_pad(AgsMachine *machine)
{
  g_return_val_if_fail(AGS_IS_MACHINE(machine), NULL);

  return(g_list_reverse(g_list_copy(machine->input_pad)));
}

/**
 * ags_machine_add_input_pad:
 * @machine: the #AgsMachine
 * @input_pad: the #AgsPad
 * @x: the x position
 * @y: the y position
 * @width: the width
 * @height: the height
 * 
 * Add @input_pad to @machine.
 * 
 * Since: 4.0.0
 */
void
ags_machine_add_input_pad(AgsMachine *machine,
			  AgsPad *input_pad,
			  guint x, guint y,
			  guint width, guint height)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));
  g_return_if_fail(AGS_IS_PAD(input_pad));

  if(g_list_find(machine->input_pad, input_pad) == NULL){
    machine->input_pad = g_list_prepend(machine->input_pad,
					input_pad);

    input_pad->parent_machine = machine;
    
    gtk_grid_attach(machine->input_pad_grid,
		    input_pad,
		    x, y,
		    width, height);
  }
}

/**
 * ags_machine_remove_input_pad:
 * @machine: the #AgsMachine
 * @input_pad: the #AgsPad
 * 
 * Remove @input_pad from @machine.
 * 
 * Since: 4.0.0
 */
void
ags_machine_remove_input_pad(AgsMachine *machine,
			     AgsPad *input_pad)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));
  g_return_if_fail(AGS_IS_PAD(input_pad));

  if(g_list_find(machine->input_pad, input_pad) != NULL){
    machine->input_pad = g_list_remove(machine->input_pad,
				       input_pad);

    input_pad->parent_machine = NULL;
    
    gtk_grid_remove(machine->input_pad_grid,
		    input_pad);
  }
}

/**
 * ags_machine_samplerate_changed:
 * @machine: the #AgsMachine
 * @samplerate: the samplerate
 * @old_samplerate: the old samplerate
 * 
 * Notify about samplerate changed.
 * 
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * Since: 3.0.0
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
  AgsChannel *start_channel;
  AgsChannel *channel;

  GList *list_output_pad, *list_output_pad_start;
  GList *list_input_pad, *list_input_pad_start;

  guint i;
  guint j;

  audio = machine->audio;

  /* reset existing input line */
  if(audio_channels_old < audio_channels){
    for(i = 0; i < machine->input_pads; i++){
      for(j = 0; j < audio_channels; j++){
	if(j < audio_channels_old){
	  AgsMachineInputLine* input_line;

	  input_line = g_list_nth_data(machine->machine_input_line,
				       (i * audio_channels_old) + j);

	  if(input_line != NULL){
	    input_line->line = (i * audio_channels) + j;
	  }
	}
      }
    }
  }else{
    for(i = 0; i < machine->input_pads; i++){
      for(j = 0; j < audio_channels_old; j++){
	if(j >= audio_channels){
	  AgsMachineInputLine* input_line;

	  input_line = g_list_nth_data(machine->machine_input_line,
				       i * audio_channels);

	  if(input_line->audio_channel >= audio_channels){
	    machine->machine_input_line = g_list_remove(machine->machine_input_line,
							input_line);
	    
	    g_free(input_line);
	  }
	}
      }
    }
  }
  
  /* insert new input line */
  if(audio_channels_old < audio_channels){
    for(i = 0; i < machine->input_pads; i++){
      for(j = 0; j < audio_channels; j++){
	if(j >= audio_channels_old){
	  AgsMachineInputLine* input_line;

	  gboolean success;
	
	  input_line = g_list_nth_data(machine->machine_input_line,
				       (i * audio_channels_old) + j);

	  success = FALSE;
	
	  if(input_line == NULL){	
	    input_line = ags_machine_input_line_alloc();
	  }else{
	    success = TRUE;
	  }
	
	  input_line->pad = i;
	  input_line->audio_channel = j;
	
	  input_line->line = (i * machine->audio_channels) + j;
	
	  if(!success){
	    machine->machine_input_line = g_list_insert_sorted(machine->machine_input_line,
							       input_line,
							       (GCompareFunc) ags_machine_input_line_sort_func);
	  }
	}
      }
    }
  }
  
  list_output_pad =
    list_output_pad_start = NULL;

  list_input_pad =
    list_input_pad_start = NULL;
  
  if(audio_channels > audio_channels_old){
    /* grow lines */
    AgsPad *pad;

    guint input_pads, output_pads;
    guint audio_audio_channels;
    guint audio_input_pads, audio_output_pads;

    output_pads = machine->output_pads;
    input_pads = machine->input_pads;

    if(machine->input_pad_grid != NULL){
      list_input_pad =
	list_input_pad_start = ags_machine_get_input_pad(machine);
    }

    if(machine->output_pad_grid != NULL){
      list_output_pad =
	list_output_pad_start = ags_machine_get_output_pad(machine);
    }
    
    /* AgsInput */
    if(machine->input_pad_grid != NULL){
      /* get input */
      start_channel = NULL;

      audio_audio_channels = 0;

      audio_input_pads = 0;
      audio_output_pads = 0;
      
      g_object_get(audio,
		   "input", &start_channel,
		   "audio-channels", &audio_audio_channels,
		   "input-pads", &audio_input_pads,
		   "output-pads", &audio_output_pads,
		   NULL);
      
      for(i = 0; i < input_pads; i++){
	/* create AgsPad's if necessary or resize */
	if(audio_audio_channels > 0 &&
	   i < audio_input_pads){
	  channel = ags_channel_nth(start_channel, i * audio_audio_channels);
	}else{
	  channel = NULL;
	}
	
	if(audio_channels_old == 0){
	  pad = g_object_new(machine->input_pad_type,
			     "channel", channel,
			     NULL);

	  if(machine->input_pad_orientation == GTK_ORIENTATION_HORIZONTAL){
	    ags_machine_add_input_pad(machine,
				      (GtkWidget *) pad,
				      i, 0,
				      1, 1);
	  }else{
	    ags_machine_add_input_pad(machine,
				      (GtkWidget *) pad,
				      0, i,
				      1, 1);
	  }

	  ags_pad_resize_lines((AgsPad *) pad, machine->input_line_type,
			       audio_channels, 0);
	}else{
	  pad = AGS_PAD(list_input_pad->data);

	  ags_pad_resize_lines((AgsPad *) pad, machine->input_line_type,
			       audio_channels, audio_channels_old);
	}

	if(channel != NULL){
	  g_object_unref(channel);
	}
	
	/* iterate */
	if(audio_channels_old != 0){
	  list_input_pad = list_input_pad->next;
	}
      }

      if(start_channel != NULL){
	g_object_unref(start_channel);
      }
    }

    /* AgsOutput */
    if(machine->output_pad_grid != NULL){
      /* get output */
      start_channel = NULL;

      g_object_get(audio,
		   "output", &start_channel,
		   NULL);

      for(i = 0; i < output_pads; i++){
	/* create AgsPad's if necessary or resize */
	if(audio_audio_channels > 0 &&
	   i < audio_output_pads){
	  channel = ags_channel_nth(start_channel, i * audio_audio_channels);
	}else{
	  channel = NULL;
	}

	if(audio_channels_old == 0){
	  pad = g_object_new(machine->output_pad_type,
			     "channel", channel,
			     NULL);

	  if(machine->output_pad_orientation == GTK_ORIENTATION_HORIZONTAL){
	    ags_machine_add_output_pad(machine,
				       (GtkWidget *) pad,
				       i, 0,
				       1, 1);
	  }else{
	    ags_machine_add_output_pad(machine,
				       (GtkWidget *) pad,
				       0, i,
				       1, 1);
	  }
	  
	  ags_pad_resize_lines((AgsPad *) pad, machine->output_line_type,
			       audio_channels, 0);
	}else{
	  pad = AGS_PAD(list_output_pad->data);

	  ags_pad_resize_lines((AgsPad *) pad, machine->output_line_type,
			       audio_channels, audio_channels_old);
	}

	if(channel != NULL){
	  g_object_unref(channel);
	}

	/* iterate */
	if(audio_channels_old != 0){
	  list_output_pad = list_output_pad->next;
	}
      }

      if(start_channel != NULL){
	g_object_unref(start_channel);
      }
    }

    /* show all */
    if(audio_channels_old == 0){
      if(machine->input_pad_grid != NULL){
	list_input_pad_start = 
	  list_input_pad = ags_machine_get_input_pad(machine);
      }

      if(machine->output_pad_grid != NULL){
	list_output_pad_start = 
	  list_output_pad = ags_machine_get_output_pad(machine);
      }
    }
    
    if(gtk_widget_get_visible((GtkWidget *) machine)){
      if(audio_channels_old == 0){
	/* AgsInput */
	if(machine->input_pad_grid != NULL){
	  list_input_pad = list_input_pad_start;

	  while(list_input_pad != NULL){
	    gtk_widget_show(GTK_WIDGET(list_input_pad->data));

	    list_input_pad = list_input_pad->next;
	  }
	}
	
	/* AgsOutput */
	if(machine->output_pad_grid != NULL){
	  list_output_pad = list_output_pad_start;
	  
	  while(list_input_pad != NULL){
	    gtk_widget_show(GTK_WIDGET(list_input_pad->data));
	    
	    list_input_pad = list_input_pad->next;
	  }
	}
      }else{
	if(machine->input_pad_grid != NULL){
	  GList *start_list_input_line, *list_input_line;

	  list_input_pad = list_input_pad_start;

	  while(list_input_pad != NULL){
	    start_list_input_line = ags_pad_get_line(AGS_PAD(list_input_pad->data));
	    
	    list_input_line = g_list_nth(start_list_input_line,
					 audio_channels_old);
	    
	    while(list_input_line != NULL){
	      gtk_widget_show(GTK_WIDGET(list_input_line->data));

	      list_input_line = list_input_line->next;
	    }

	    g_list_free(start_list_input_line);
	    
	    list_input_pad = list_input_pad->next;
	  }
	}
	
	/* AgsOutput */
	if(machine->output_pad_grid != NULL){
	  GList *start_list_output_line, *list_output_line;
	  
	  list_output_pad = list_output_pad_start;
	  
	  while(list_output_pad != NULL){
	    start_list_output_line = ags_pad_get_line(AGS_PAD(list_output_pad->data));
	    
	    list_output_line = g_list_nth(start_list_output_line,
					  audio_channels_old);
	    
	    while(list_output_line != NULL){
	      gtk_widget_show(GTK_WIDGET(list_output_line->data));

	      list_output_line = list_output_line->next;
	    }	    

	    g_list_free(start_list_output_line);
	    
	    list_output_pad = list_output_pad->next;
	  }
	}	
      }
    }
    
    g_list_free(list_input_pad_start);
    g_list_free(list_output_pad_start);
  }else if(audio_channels < audio_channels_old){
    /* shrink lines */
    if(machine->output_pad != NULL){
      list_output_pad_start = 
	list_output_pad = ags_machine_get_output_pad(machine);
    }

    if(machine->input_pad_grid != NULL){
      list_input_pad_start = 
	list_input_pad = ags_machine_get_input_pad(machine);
    }
	
    if(audio_channels == 0){
      /* AgsInput */
      if(machine->input_pad_grid != NULL){
	while(list_input_pad != NULL){
	  ags_machine_remove_input_pad(machine,
				       list_input_pad->data);

	  g_object_run_dispose(list_input_pad->data);
	  g_object_unref(list_input_pad->data);

	  list_input_pad = list_input_pad->next;
	}
      }
      
      /* AgsOutput */
      if(machine->output_pad_grid != NULL){
	while(list_output_pad != NULL){
	  ags_machine_remove_output_pad(machine,
					list_output_pad->data);
	  
	  g_object_run_dispose(list_output_pad->data);
	  g_object_unref(list_output_pad->data);
	  
	  list_output_pad = list_output_pad->next;
	}
      }
    }else{
      /* AgsInput */
      if(machine->input_pad_grid != NULL){
	for(i = 0; list_input_pad != NULL; i++){
	  ags_pad_resize_lines(AGS_PAD(list_input_pad->data), machine->input_pad_type,
			       audio_channels, audio_channels_old);
	  
	  list_input_pad = list_input_pad->next;
	}
      }
      
      /* AgsOutput */
      if(machine->output_pad_grid != NULL){
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

  machine->audio_channels = audio_channels;
}

/**
 * ags_machine_resize_audio_channels:
 * @machine: the #AgsMachine
 * @new_size: new allocation
 * @old_size: old allocation
 *
 * Resize audio channel allocation.
 *
 * Since: 3.0.0
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
  AgsChannel *start_input, *start_output;
  AgsChannel *input, *output;
  
  GList *list_pad_start, *list_pad;

  guint audio_channels;
  guint audio_audio_channels;
  guint audio_input_pads, audio_output_pads;
  guint i;
  guint j;

  audio = machine->audio;

  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(pads_old < pads){
      for(i = 0; i < pads; i++){
	for(j = 0; j < machine->audio_channels; j++){
	  if(i >= pads_old){
	    AgsMachineInputLine* input_line;

	    input_line = ags_machine_input_line_alloc();

	    input_line->pad = i;
	    input_line->audio_channel = j;

	    input_line->line = (i * machine->audio_channels) + j;

	    machine->machine_input_line = g_list_insert_sorted(machine->machine_input_line,
							       input_line,
							       (GCompareFunc) ags_machine_input_line_sort_func);
	  }
	}
      }
    }else{
      for(i = 0; i < pads_old; i++){
	for(j = 0; j < machine->audio_channels; j++){
	  if(i >= pads){
	    AgsMachineInputLine* input_line;

	    input_line = g_list_nth_data(machine->machine_input_line,
					 pads * machine->audio_channels);

	    if(input_line->pad >= pads){
	      machine->machine_input_line = g_list_remove(machine->machine_input_line,
							  input_line);
	    
	      g_free(input_line);
	    }
	  }
	}
      }
    }
  }
  
  audio_channels = machine->audio_channels;
  
  if(pads_old < pads){
    start_output = NULL;
    start_input = NULL;
    
    audio_audio_channels = 0;

    audio_input_pads = 0;
    audio_output_pads = 0;
      
    g_object_get(audio,
		 "output", &start_output,
		 "input", &start_input,
		 "audio-channels", &audio_audio_channels,
		 "input-pads", &audio_input_pads,
		 "output-pads", &audio_output_pads,
		 NULL);

    /* grow input */
    if(machine->input_pad_grid != NULL){
      if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
	for(i = pads_old; i < pads; i++){
	  /* instantiate pad */
	  if(audio_audio_channels > 0 &&
	     i < audio_input_pads){
	    input = ags_channel_nth(start_input,
				    i * audio_audio_channels);
	  }else{
	    input = NULL;
	  }
	  
	  pad = g_object_new(machine->input_pad_type,
			     "channel", input,
			     NULL);

	  if(machine->input_pad_orientation == GTK_ORIENTATION_HORIZONTAL){
	    ags_machine_add_input_pad(machine,
				      (GtkWidget *) pad,
				      i, 0,
				      1, 1);
	  }else{
	    ags_machine_add_input_pad(machine,
				      (GtkWidget *) pad,
				      0, i,
				      1, 1);
	  }
	  
	  /* resize lines */
	  ags_pad_resize_lines((AgsPad *) pad, machine->input_line_type,
			       audio_channels, 0);	  

	  if(input != NULL){
	    g_object_unref(input);
	  }
	}

	/* show all */
	list_pad_start = 
	  list_pad = ags_machine_get_input_pad(machine);
	list_pad = g_list_nth(list_pad,
			      pads_old);

	while(list_pad != NULL){
	  gtk_widget_show(GTK_WIDGET(list_pad->data));

	  list_pad = list_pad->next;
	}

	g_list_free(list_pad_start);
      }
    }

    if(start_input != NULL){
      g_object_unref(start_input);
    }
    
    /* grow output */
    if(machine->output_pad_grid != NULL){
      if(g_type_is_a(channel_type, AGS_TYPE_OUTPUT)){
	for(i = pads_old; i < pads; i++){
	  /* instantiate pad */
	  if(audio_audio_channels > 0 &&
	     i < audio_output_pads){
	    output = ags_channel_nth(start_output,
				     i * audio_audio_channels);
	  }else{
	    output = NULL;
	  }

	  pad = g_object_new(machine->output_pad_type,
			     "channel", output,
			     NULL);

	  if(machine->output_pad_orientation == GTK_ORIENTATION_HORIZONTAL){
	    ags_machine_add_output_pad(machine,
				       (GtkWidget *) pad,
				       i, 0,
				       1, 1);
	  }else{
	    ags_machine_add_output_pad(machine,
				       (GtkWidget *) pad,
				       0, i,
				       1, 1);
	  }

	  /* resize lines */
	  ags_pad_resize_lines((AgsPad *) pad, machine->output_line_type,
			       audio_channels, 0);

	  if(output != NULL){
	    g_object_unref(output);
	  }
	}

	/* show all */
	list_pad_start = 
	  list_pad = ags_machine_get_output_pad(machine);

	list_pad = g_list_nth(list_pad,
			      pads_old);

	while(list_pad != NULL){
	  gtk_widget_show(GTK_WIDGET(list_pad->data));

	  list_pad = list_pad->next;
	}

	g_list_free(list_pad_start);
      }
    }

    if(start_output != NULL){
      g_object_unref(start_output);
    }
  }else if(pads_old > pads){
    GList *list, *list_start;
    
    /* input - destroy AgsPad's */
    if(channel_type == AGS_TYPE_INPUT &&
       machine->input_pad_grid != NULL){
      for(i = 0; i < pads_old - pads; i++){
	list_start = ags_machine_get_input_pad(machine);
	list = g_list_nth(list_start, pads);

	if(list != NULL){
	  ags_machine_remove_input_pad(machine,
				       list->data);

	  g_object_run_dispose(list->data);
	  g_object_unref(list->data);
	}
	
	g_list_free(list_start);
      }
    }
    
    /* output - destroy AgsPad's */
    if(channel_type == AGS_TYPE_OUTPUT &&
       machine->output_pad_grid != NULL){
      for(i = 0; i < pads_old - pads; i++){
	list_start = ags_machine_get_output_pad(machine);
	list = g_list_nth(list_start, pads);

	if(list != NULL){
	  ags_machine_remove_output_pad(machine,
					list->data);

	  g_object_run_dispose(list->data);
	  g_object_unref(list->data);
	}
	
	g_list_free(list_start);
      }
    }
  }

  if(g_type_is_a(channel_type, AGS_TYPE_OUTPUT)){
    machine->output_pads = pads;
  }else{
    machine->input_pads = pads;
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
 * Since: 3.0.0
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
 * Since: 3.0.0
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
  if(machine->output_pad_grid != NULL){
    pad_start = 
      pad = ags_machine_get_output_pad(machine);

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
  if(machine->input_pad_grid != NULL){
    pad_start = 
      pad = ags_machine_get_input_pad(machine);

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
 * Since: 3.0.0
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
 * Since: 3.0.0
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

void
ags_machine_real_refresh_port(AgsMachine *machine)
{
  GList *start_pad, *pad;

  if(machine->bridge != NULL){
    ags_effect_bridge_refresh_port(machine->bridge);
  }
  
  /* output */
  pad =
    start_pad = ags_machine_get_output_pad(machine);

  while(pad != NULL){
    ags_pad_refresh_port(pad->data);

    pad = pad->next;
  }

  g_list_free(start_pad);

  /* input */
  pad =
    start_pad = ags_machine_get_input_pad(machine);

  while(pad != NULL){
    ags_pad_refresh_port(pad->data);

    pad = pad->next;
  }

  g_list_free(start_pad);
}

/**
 * ags_machine_refresh_port:
 * @machine: the #AgsMachine
 *
 * Notify about to refresh ports.
 * 
 * Since: 4.2.1
 */
void
ags_machine_refresh_port(AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));

  g_object_ref((GObject *) machine);
  g_signal_emit((GObject *) machine,
		machine_signals[REFRESH_PORT], 0);
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
 * Since: 3.0.0
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
 * ags_machine_playback_set_active:
 * @machine: the #AgsMachine
 * @playback: the #AgsPlayback
 * @is_active: if %TRUE playback is started, otherwise stopped
 *
 * Start/stop @playback of @machine.
 *
 * Since: 3.0.0
 */
void
ags_machine_playback_set_active(AgsMachine *machine,
				AgsPlayback *playback,
				gboolean is_active)
{
  AgsChannel *channel;
  AgsNote *play_note;

  AgsStartSoundcard *start_soundcard;
  AgsStartChannel *start_channel;
  AgsCancelChannel *cancel_channel;
  AgsResetNote *reset_note;

  AgsApplicationContext *application_context;

  GList *start_task;
  
  if(!AGS_IS_MACHINE(machine) ||
     !AGS_IS_PLAYBACK(playback)){
    return;
  }

  application_context = ags_application_context_get_instance();

  start_task = NULL;
  
  if(is_active){
    if(g_list_find(machine->active_playback, playback) != NULL){
      return;
    }

    machine->active_playback = g_list_prepend(machine->active_playback,
					      playback);
    g_object_ref(playback);

    /* start playback of channel */
    g_object_get(playback,
		 "channel", &channel,
		 NULL);

    start_channel = ags_start_channel_new(channel,
					  AGS_SOUND_SCOPE_PLAYBACK);
    g_signal_connect_after(G_OBJECT(start_channel), "launch",
			   G_CALLBACK(ags_machine_active_playback_start_channel_launch_callback), playback);
    start_task = g_list_prepend(start_task,
				start_channel);

    /* start soundcard */
    start_soundcard = ags_start_soundcard_new();
    start_task = g_list_prepend(start_task,
				start_soundcard);

    /* launch task */
    start_task = g_list_reverse(start_task);

    ags_ui_provider_schedule_task_all(AGS_UI_PROVIDER(application_context),
				      start_task);

    /* feed note */
    g_object_get(playback,
		 "play-note", &play_note,
		 NULL);

    if(ags_note_test_flags(play_note, AGS_NOTE_FEED)){
      reset_note = ags_reset_note_get_instance();
      ags_reset_note_add(reset_note,
			 play_note);
    }

    /* unref */
    g_object_unref(channel);

    g_object_unref(play_note);
  }else{
    if(g_list_find(machine->active_playback, playback) == NULL){
      return;
    }

    machine->active_playback = g_list_remove(machine->active_playback,
					     playback);
    g_object_unref(playback);

    /* cancel playback of channel */
    g_object_get(playback,
		 "channel", &channel,
		 NULL);
    
    cancel_channel = ags_cancel_channel_new(channel,
					    AGS_SOUND_SCOPE_PLAYBACK);
    start_task = g_list_prepend(start_task,
				cancel_channel);

    /* launch task */
    start_task = g_list_reverse(start_task);

    ags_ui_provider_schedule_task_all(AGS_UI_PROVIDER(application_context),
				      start_task);
    
    /* feed note */
    g_object_get(playback,
		 "play-note", &play_note,
		 NULL);

    if(ags_note_test_flags(play_note, AGS_NOTE_FEED)){
      reset_note = ags_reset_note_get_instance();
      ags_reset_note_remove(reset_note,
			    play_note);      
    }

    /* unref */
    g_object_unref(channel);

    g_object_unref(play_note);
  }
}

/**
 * ags_machine_set_run:
 * @machine: the #AgsMachine
 * @run: if %TRUE playback is started, otherwise stopped
 *
 * Start/stop playback of @machine.
 *
 * Since: 3.0.0
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
 * Since: 3.0.0
 */
void
ags_machine_set_run_extended(AgsMachine *machine,
			     gboolean run,
			     gboolean sequencer, gboolean notation, gboolean wave, gboolean midi)
{
  AgsApplicationContext *application_context;

  GList *start_list;
  
  gboolean no_soundcard;

  if(!AGS_IS_MACHINE(machine)){
    return;
  }
  
  application_context = ags_application_context_get_instance();

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
      start_soundcard = ags_start_soundcard_new();
      list = g_list_prepend(list,
			    start_soundcard);

      /* start sequencer */
      start_sequencer = ags_start_sequencer_new();
      list = g_list_prepend(list,
			    start_sequencer);
      
      /* append AgsStartSoundcard and AgsStartSequencer */
      list = g_list_reverse(list);
      
      ags_ui_provider_schedule_task_all(AGS_UI_PROVIDER(application_context),
					list);
    }
  }else{
    AgsCancelAudio *cancel_audio;

    if(sequencer){
      /* create cancel task */
      cancel_audio = ags_cancel_audio_new(machine->audio,
					  AGS_SOUND_SCOPE_SEQUENCER);
    
      /* append AgsCancelAudio */
      ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				    (AgsTask *) cancel_audio);
    }

    if(notation){
      /* create cancel task */
      cancel_audio = ags_cancel_audio_new(machine->audio,
					  AGS_SOUND_SCOPE_NOTATION);
    
      /* append AgsCancelAudio */
      ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				    (AgsTask *) cancel_audio);
    }

    if(wave){
      /* create cancel task */
      cancel_audio = ags_cancel_audio_new(machine->audio,
					  AGS_SOUND_SCOPE_WAVE);
    
      /* append AgsCancelAudio */
      ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				    (AgsTask *) cancel_audio);
    }

    if(midi){
      /* create cancel task */
      cancel_audio = ags_cancel_audio_new(machine->audio,
					  AGS_SOUND_SCOPE_MIDI);
    
      /* append AgsCancelAudio */
      ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				    (AgsTask *) cancel_audio);
    }
  }
}

/**
 * ags_machine_get_editor_model:
 * @machine: the #AgsMachine
 * 
 * Get editor model.
 * 
 * Returns: the #GList-struct containing #xmlNode-struct
 * 
 * Since: 4.4.0
 */
GList*
ags_machine_get_editor_model(AgsMachine *machine)
{  
  g_return_val_if_fail(AGS_IS_MACHINE(machine), NULL);

  return(g_list_reverse(g_list_copy(machine->editor_model)));
}

/**
 * ags_machine_add_editor_model:
 * @machine: the #AgsMachine
 * @node: the #xmlNode-struct
 * 
 * Add @node to @machine.
 * 
 * Since: 4.4.0
 */
void
ags_machine_add_editor_model(AgsMachine *machine,
			     xmlNode *node)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));
  g_return_if_fail(node != NULL);

  if(g_list_find(machine->editor_model, node) == NULL){
    machine->editor_model = g_list_prepend(machine->editor_model,
					   node);
  }
}

/**
 * ags_machine_remove_editor_model:
 * @machine: the #AgsMachine
 * @node: the #xmlNode-struct
 * 
 * Get @node from @machine.
 * 
 * Since: 4.4.0
 */
void
ags_machine_remove_editor_model(AgsMachine *machine,
				xmlNode *node)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));
  g_return_if_fail(node != NULL);

  if(g_list_find(machine->editor_model, node) != NULL){
    machine->editor_model = g_list_remove(machine->editor_model,
					  node);
  }
}

/**
 * ags_machine_get_dialog_model:
 * @machine: the #AgsMachine
 * 
 * Get dialog model.
 * 
 * Returns: the #GList-struct containing #xmlNode-struct
 * 
 * Since: 4.0.0
 */
GList*
ags_machine_get_dialog_model(AgsMachine *machine)
{  
  g_return_val_if_fail(AGS_IS_MACHINE(machine), NULL);

  return(g_list_reverse(g_list_copy(machine->dialog_model)));
}

/**
 * ags_machine_add_dialog_model:
 * @machine: the #AgsMachine
 * @node: the #xmlNode-struct
 * 
 * Add @node to @machine.
 * 
 * Since: 4.0.0
 */
void
ags_machine_add_dialog_model(AgsMachine *machine,
			     xmlNode *node)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));
  g_return_if_fail(node != NULL);

  if(g_list_find(machine->dialog_model, node) == NULL){
    machine->dialog_model = g_list_prepend(machine->dialog_model,
					   node);
  }
}

/**
 * ags_machine_remove_dialog_model:
 * @machine: the #AgsMachine
 * @node: the #xmlNode-struct
 * 
 * Get @node from @machine.
 * 
 * Since: 4.0.0
 */
void
ags_machine_remove_dialog_model(AgsMachine *machine,
				xmlNode *node)
{
  g_return_if_fail(AGS_IS_MACHINE(machine));
  g_return_if_fail(node != NULL);

  if(g_list_find(machine->dialog_model, node) != NULL){
    machine->dialog_model = g_list_remove(machine->dialog_model,
					  node);
  }
}

/**
 * ags_machine_find_dialog_model:
 * @machine: the #AgsMachine
 * @dialog_model: the #GList-struct
 * @node_name: the node name
 * @attribute: the attribute
 * @value: the attribute value
 * 
 * Find dialog model.
 * 
 * Returns: the matching #GList-struct with data #xmlNode-struct, or %NULL
 * 
 * Since: 4.0.0
 */
GList*
ags_machine_find_dialog_model(AgsMachine *machine,
			      GList *dialog_model,
			      gchar *node_name,
			      gchar *attribute,
			      gchar *value)
{
  g_return_val_if_fail(AGS_IS_MACHINE(machine), NULL);

  while(dialog_model != NULL){
    xmlNode *current_node;

    current_node = dialog_model->data;

    if(!g_strcmp0(current_node->name, node_name)){
      if(attribute != NULL){
	if(!g_strcmp0(xmlGetProp(current_node, attribute),
		      value)){	
	  return(dialog_model);
	}
      }else{
	return(dialog_model);
      }
    }
    
    dialog_model = dialog_model->next;
  }

  return(NULL);
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
 * Since: 3.0.0
 */
GtkListStore*
ags_machine_get_possible_audio_output_connections(AgsMachine *machine)
{
  AgsApplicationContext *application_context;

  GtkListStore *model;

  GList *start_list, *list;
  GtkTreeIter iter;

  application_context = ags_application_context_get_instance();
  
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
	gchar *tmp_device;

	tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
	
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
			   0, g_strdup_printf("%s: %s", 
					      G_OBJECT_TYPE_NAME(G_OBJECT(list->data)),
					      (tmp_device != NULL ? tmp_device: "(null)")),
			   1, list->data,
			   -1);

	g_free(tmp_device);
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
 * Since: 3.0.0
 */
GtkListStore*
ags_machine_get_possible_audio_input_connections(AgsMachine *machine)
{
  AgsApplicationContext *application_context;

  GtkListStore *model;

  GList *start_list, *list;
  GtkTreeIter iter;

  application_context = ags_application_context_get_instance();
  
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
	gchar *tmp_device;

	tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
	
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
			   0, g_strdup_printf("%s: %s", 
					      G_OBJECT_TYPE_NAME(G_OBJECT(list->data)),
					      (tmp_device != NULL ? tmp_device: "(null)")),
			   1, list->data,
			   -1);

	g_free(tmp_device);
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
 * Since: 3.0.0
 */
GtkListStore*
ags_machine_get_possible_links(AgsMachine *machine)
{
  AgsWindow *window;
  
  GtkListStore *model;

  GList *start_list, *list;
  GtkTreeIter iter;
  
  window = gtk_widget_get_ancestor(machine,
				   AGS_TYPE_WINDOW);

  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "NULL",
		     1, NULL,
		     -1);

  list =
    start_list = ags_window_get_machine(window);
  
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

  g_list_free(start_list);
  
  return(model);
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
 * Since: 3.0.0
 */
void
ags_machine_open_files(AgsMachine *machine,
		       GSList *filenames,
		       gboolean overwrite_channels,
		       gboolean create_channels)
{
  AgsOpenFile *open_file;

  AgsApplicationContext *application_context;
    
  application_context = ags_application_context_get_instance();

  /* instantiate open file task */
  open_file = ags_open_file_new(machine->audio,
				filenames,
				overwrite_channels,
				create_channels);

  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) open_file);
}

xmlNode*
ags_machine_copy_pattern_to_notation(AgsMachine *machine,
				     AgsChannel *start_current,
				     guint input_pads)
{
  AgsChannel *current, *next_current;    
  AgsPattern *pattern;

  xmlNode *notation_node, *current_note;

  GList *start_list;

  gchar *str;
  
  guint audio_channel;
  guint x_boundary, y_boundary;
  guint bank_0, bank_1, k;

  current = start_current;

  audio_channel = 0;
  
  if(current != NULL){
    g_object_ref(current);

    g_object_get(current,
		 "audio-channel", &audio_channel,
		 NULL);
  }

  next_current = NULL;
    
  /* create root node */
  notation_node = xmlNewNode(NULL, BAD_CAST "notation");

  xmlNewProp(notation_node, BAD_CAST "program", BAD_CAST "ags");
  xmlNewProp(notation_node, BAD_CAST "type", BAD_CAST AGS_NOTATION_CLIPBOARD_TYPE);
  xmlNewProp(notation_node, BAD_CAST "version", BAD_CAST AGS_NOTATION_CLIPBOARD_VERSION);
  xmlNewProp(notation_node, BAD_CAST "format", BAD_CAST AGS_NOTATION_CLIPBOARD_FORMAT);
  xmlNewProp(notation_node, BAD_CAST "base_frequency", BAD_CAST "0");

  str = g_strdup_printf("%u",
			audio_channel);
  xmlNewProp(notation_node, BAD_CAST "audio-channel", BAD_CAST str);

  g_free(str);

  bank_0 = machine->bank_0;
  bank_1 = machine->bank_1;
    
  x_boundary = G_MAXUINT;
  y_boundary = G_MAXUINT;

  while(current != NULL){
    guint length;
      
    GRecMutex *pattern_mutex;
      
    g_object_get(current,
		 "pattern", &start_list,
		 NULL);

    pattern = start_list->data;
    g_object_ref(pattern);
    
    g_list_free_full(start_list,
		     g_object_unref);

    /* get pattern mutex */
    pattern_mutex = AGS_PATTERN_GET_OBJ_MUTEX(pattern);

    /* get length */
    g_rec_mutex_lock(pattern_mutex);

    length = pattern->dim[2];
      
    g_rec_mutex_unlock(pattern_mutex);
      
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

    g_object_unref(pattern);
    
    /* iterate */
    next_current = ags_channel_next(current);

    g_object_unref(current);

    current = next_current;
  }

  xmlNewProp(notation_node, BAD_CAST "x_boundary", BAD_CAST g_strdup_printf("%u", x_boundary));
  xmlNewProp(notation_node, BAD_CAST "y_boundary", BAD_CAST g_strdup_printf("%u", y_boundary));

  return(notation_node);
}

void
ags_machine_copy_pattern(AgsMachine *machine)
{
  AgsAudio *audio;
  AgsChannel *start_channel;
  AgsChannel *channel, *next_channel;
  
  xmlDoc *clipboard;
  xmlNode *audio_node, *notation_list_node, *notation_node;

  xmlChar *buffer;

  guint audio_channels;
  guint input_pads;
  int size;
  gint i;
  
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
	       "input", &start_channel,
	       NULL);

  channel = start_channel;

  if(channel != NULL){
    g_object_ref(channel);
  }

  next_channel = NULL;
  
  for(i = 0; i < audio_channels; i++){
    /* do it so */
    notation_node = ags_machine_copy_pattern_to_notation(machine,
							 channel,
							 input_pads);
    xmlAddChild(notation_list_node,
		notation_node);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
  
  /* write to clipboard */
  xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);

  gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()),
			 buffer);
  
  xmlFreeDoc(clipboard);

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }
}

/**
 * ags_machine_check_message:
 * @machine: the #AgsMachine
 *
 * Check message queue for message envelopes.
 *
 * Since: 3.0.0
 */
void
ags_machine_check_message(AgsMachine *machine)
{
  AgsMessageDelivery *message_delivery;

  GList *start_message_envelope, *message_envelope;
  GList *start_active_playback, *active_playback;

  if(!AGS_IS_MACHINE(machine)){
    return;
  }

  /* retrieve message */
  message_delivery = ags_message_delivery_get_instance();

  message_envelope =
    start_message_envelope = ags_message_delivery_find_sender(message_delivery,
							      "libgsequencer",
							      (GObject *) machine->audio);
  
  while(message_envelope != NULL){
    xmlNode *root_node;

    root_node = xmlDocGetRootElement(AGS_MESSAGE_ENVELOPE(message_envelope->data)->doc);
      
    if(!xmlStrncmp(root_node->name,
		   BAD_CAST "ags-command",
		   12)){
      if(!xmlStrncmp(xmlGetProp(root_node,
				"method"),
		     BAD_CAST "AgsAudio::set-samplerate",
		     25)){
	guint samplerate;
	gint position;
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "samplerate");
	samplerate = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	/* set samplerate */
	g_object_set(machine,
		     "samplerate", samplerate,
		     NULL);
      }else if(!xmlStrncmp(xmlGetProp(root_node,
				      "method"),
			   BAD_CAST "AgsAudio::set-buffer-size",
			   26)){
	guint buffer_size;
	gint position;
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "buffer-size");
	buffer_size = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	/* set buffer size */
	g_object_set(machine,
		     "buffer-size", buffer_size,
		     NULL);
      }else if(!xmlStrncmp(xmlGetProp(root_node,
				      "method"),
			   BAD_CAST "AgsAudio::set-format",
			   21)){
	guint format;
	gint position;
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "format");
	format = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	/* set format */
	g_object_set(machine,
		     "format", format,
		     NULL);
      }else if(!xmlStrncmp(xmlGetProp(root_node,
				      "method"),
			   BAD_CAST "AgsAudio::set-audio-channels",
			   29)){
	guint audio_channels, audio_channels_old;
	gint position;
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "audio-channels");
	audio_channels = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "audio-channels-old");
	audio_channels_old = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	/* resize audio channels */
	ags_machine_resize_audio_channels(machine,
					  audio_channels, audio_channels_old);
      }else if(!xmlStrncmp(xmlGetProp(root_node,
				      "method"),
			   BAD_CAST "AgsAudio::set-pads",
			   19)){
	GType channel_type;
	  
	guint pads, pads_old;
	gint position;

	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "channel-type");
	channel_type = (GType) g_value_get_pointer(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "pads");
	pads = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "pads-old");
	pads_old = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	/* resize pads */
	ags_machine_resize_pads(machine,
				channel_type,
				pads, pads_old);
      }else if(!xmlStrncmp(xmlGetProp(root_node,
				      "method"),
			   BAD_CAST "AgsAudio::stop",
			   15)){
	GList *recall_id;

	gint sound_scope;
	gint position;
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "recall-id");
	recall_id = g_value_get_pointer(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "sound-scope");
	sound_scope = g_value_get_int(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));
	  
	/* stop */
	ags_machine_stop(machine,
			 recall_id, sound_scope);
      }
    }
    
    message_envelope = message_envelope->next;
  }
      
  g_list_free_full(start_message_envelope,
		   (GDestroyNotify) g_object_unref);

  /*  */
  active_playback =
    start_active_playback = g_list_copy(machine->active_playback);

  while(active_playback != NULL){
    AgsChannel *channel;
    AgsPlayback *playback;

    playback = active_playback->data;
    g_object_ref(playback);
    
    g_object_get(playback,
		 "channel", &channel,
		 NULL);
    
    message_envelope =
      start_message_envelope = ags_message_delivery_find_sender(message_delivery,
								"libgsequencer",
								(GObject *) channel);

    while(message_envelope != NULL){
      xmlNode *root_node;

      root_node = xmlDocGetRootElement(AGS_MESSAGE_ENVELOPE(message_envelope->data)->doc);
      
      if(!xmlStrncmp(root_node->name,
		     BAD_CAST "ags-command",
		     12)){
	if(!xmlStrncmp(xmlGetProp(root_node,
				  "method"),
		       BAD_CAST "AgsChannel::stop",
		       18)){
	  ags_machine_playback_set_active(machine,
					  playback,
					  FALSE);
	}
      }
	
      message_envelope = message_envelope->next;
    }
      
    g_list_free_full(start_message_envelope,
		     (GDestroyNotify) g_object_unref);

    g_object_unref(channel);
    g_object_unref(playback);
      
    active_playback = active_playback->next;
  }

  g_list_free(start_active_playback);
}

/**
 * ags_machine_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsMachine
 *
 * Returns: a new #AgsMachine
 *
 * Since: 3.0.0
 */
AgsMachine*
ags_machine_new(GObject *soundcard)
{
  AgsMachine *machine;

  machine = (AgsMachine *) g_object_new(AGS_TYPE_MACHINE,
					NULL);
  
  g_object_set(machine->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(machine);
}
