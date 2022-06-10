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

#include <ags/app/ags_line.h>
#include <ags/app/ags_line_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>

#include <ags/config.h>

#include <ladspa.h>

#include <ags/i18n.h>

void ags_line_class_init(AgsLineClass *line);
void ags_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_line_init(AgsLine *line);
void ags_line_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_line_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_line_dispose(GObject *gobject);
void ags_line_finalize(GObject *gobject);

void ags_line_connect(AgsConnectable *connectable);
void ags_line_disconnect(AgsConnectable *connectable);

void ags_line_real_set_channel(AgsLine *line, AgsChannel *channel);

void ags_line_add_ladspa_plugin(AgsLine *line,
				GList *control_type_name,
				AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				gchar *plugin_name,
				gchar *filename,
				gchar *effect,
				guint start_audio_channel, guint stop_audio_channel,
				guint start_pad, guint stop_pad,
				gint position,
				guint create_flags, guint recall_flags);
void ags_line_add_lv2_plugin(AgsLine *line,
			     GList *control_type_name,
			     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			     gchar *plugin_name,
			     gchar *filename,
			     gchar *effect,
			     guint start_audio_channel, guint stop_audio_channel,
			     guint start_pad, guint stop_pad,
			     gint position,
			     guint create_flags, guint recall_flags);

#if defined(AGS_WITH_VST3)
void ags_line_add_vst3_plugin(AgsLine *line,
			      GList *control_type_name,
			      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			      gchar *plugin_name,
			      gchar *filename,
			      gchar *effect,
			      guint start_audio_channel, guint stop_audio_channel,
			      guint start_pad, guint stop_pad,
			      gint position,
			      guint create_flags, guint recall_flags);
#endif

void ags_line_real_add_plugin(AgsLine *line,
			      GList *control_type_name,
			      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			      gchar *plugin_name,
			      gchar *filename,
			      gchar *effect,
			      guint start_audio_channel, guint stop_audio_channel,
			      guint start_pad, guint stop_pad,
			      gint position,
			      guint create_flags, guint recall_flags);
void ags_line_real_remove_plugin(AgsLine *line,
				 guint nth);

void ags_line_real_map_recall(AgsLine *line,
			      guint output_pad_start);
GList* ags_line_real_find_port(AgsLine *line);

/**
 * SECTION:ags_line
 * @short_description: A composite widget to visualize #AgsChannel
 * @title: AgsLine
 * @section_id:
 * @include: ags/app/ags_line.h
 *
 * #AgsLine is a composite widget to visualize #AgsChannel. It should be
 * packed by an #AgsPad. It may contain #AgsLineMember to modify ports of
 * #AgsRecall.
 */

enum{
  SAMPLERATE_CHANGED,
  BUFFER_SIZE_CHANGED,
  FORMAT_CHANGED,
  SET_CHANNEL,
  GROUP_CHANGED,
  ADD_PLUGIN,
  REMOVE_PLUGIN,
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
  PROP_PARENT_PAD,
  PROP_CHANNEL,
};

static gpointer ags_line_parent_class = NULL;
static guint line_signals[LAST_SIGNAL];

GHashTable *ags_line_indicator_refresh = NULL;

GType
ags_line_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_line = 0;

    static const GTypeInfo ags_line_info = {
      sizeof(AgsLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_line = g_type_register_static(GTK_TYPE_BOX,
					   "AgsLine", &ags_line_info,
					   0);

    g_type_add_interface_static(ags_type_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_line);
  }

  return g_define_type_id__volatile;
}

void
ags_line_class_init(AgsLineClass *line)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_line_parent_class = g_type_class_peek_parent(line);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(line);

  gobject->set_property = ags_line_set_property;
  gobject->get_property = ags_line_get_property;

  gobject->dispose = ags_line_dispose;
  gobject->finalize = ags_line_finalize;
  
  /* properties */
  /**
   * AgsLine:samplerate:
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
   * AgsLine:buffer-size:
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
   * AgsLine:format:
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
   * AgsLine:parent-pad:
   *
   * The assigned parent #AgsPad.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("parent-pad",
				   i18n_pspec("parent pad"),
				   i18n_pspec("The pad which is its parent"),
				   AGS_TYPE_PAD,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PARENT_PAD,
				  param_spec);

  /**
   * AgsLine:channel:
   *
   * The assigned #AgsChannel to visualize.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("assigned channel"),
				   i18n_pspec("The channel it is assigned with"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /* AgsLineClass */
  line->samplerate_changed = NULL;
  line->buffer_size_changed = NULL;
  line->format_changed = NULL;

  line->set_channel = ags_line_real_set_channel;
  line->group_changed = NULL;
  
  line->add_plugin = ags_line_real_add_plugin;
  line->remove_plugin = ags_line_real_remove_plugin;
  
  line->map_recall = ags_line_real_map_recall;
  line->find_port = ags_line_real_find_port;

  line->stop = NULL;

  /* signals */
  /**
   * AgsLine::samplerate-changed:
   * @line: the #AgsLine
   * @samplerate: the samplerate
   * @old_samplerate: the old samplerate
   *
   * The ::samplerate-changed signal notifies about changed samplerate.
   * 
   * Since: 3.0.0
   */
  line_signals[SAMPLERATE_CHANGED] =
    g_signal_new("samplerate-changed",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, samplerate_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsLine::buffer-size-changed:
   * @line: the #AgsLine
   * @buffer_size: the buffer size
   * @old_buffer_size: the old buffer size
   *
   * The ::buffer-size-changed signal notifies about changed buffer size.
   * 
   * Since: 3.0.0
   */
  line_signals[BUFFER_SIZE_CHANGED] =
    g_signal_new("buffer-size-changed",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, buffer_size_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsLine::format-changed:
   * @line: the #AgsLine
   * @format: the format
   * @old_format: the old format
   *
   * The ::format-changed signal notifies about changed format.
   * 
   * Since: 3.0.0
   */
  line_signals[FORMAT_CHANGED] =
    g_signal_new("format-changed",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, format_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsLine::set-channel:
   * @line: the #AgsLine to modify
   * @channel: the #AgsChannel to set
   *
   * The ::set-channel signal notifies about changed channel.
   *
   * Since: 3.0.0
   */
  line_signals[SET_CHANNEL] =
    g_signal_new("set-channel",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, set_channel),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsLine::group-changed:
   * @line: the object group changed
   *
   * The ::group-changed signal notifies about changed grouping. This
   * normally happens as toggling group button in #AgsPad or #AgsLine.
   *
   * Since: 3.0.0
   */
  line_signals[GROUP_CHANGED] =
    g_signal_new("group-changed",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, group_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsLine::add-plugin:
   * @line: the #AgsLine to modify
   * @effect: the effect's name
   *
   * The ::add-plugin signal notifies about added effect.
   * 
   * Since: 3.3.0
   */
  line_signals[ADD_PLUGIN] =
    g_signal_new("add-plugin",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, add_plugin),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__POINTER_OBJECT_OBJECT_STRING_STRING_STRING_UINT_UINT_UINT_UINT_INT_UINT_UINT,
		 G_TYPE_NONE, 13,
		 G_TYPE_POINTER,
		 G_TYPE_OBJECT,
		 G_TYPE_OBJECT,
		 G_TYPE_STRING,
		 G_TYPE_STRING,
		 G_TYPE_STRING,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_INT,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsLine::remove-plugin:
   * @line: the #AgsLine to modify
   * @nth: the nth effect
   *
   * The ::remove-plugin signal notifies about removed effect.
   * 
   * Since: 3.3.0
   */
  line_signals[REMOVE_PLUGIN] =
    g_signal_new("remove-plugin",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, remove_plugin),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsLine::map-recall:
   * @line: the #AgsLine
   * @output_pad_start: the channels start pad
   *
   * The ::map-recall as recalls should be mapped.
   *
   * Since: 3.0.0
   */
  line_signals[MAP_RECALL] =
    g_signal_new("map-recall",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, map_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsLine::find-port:
   * @line: the #AgsLine 
   *
   * The ::find-port retrieves all associated ports.
   * 
   * Returns: an #GList-struct containing all related #AgsPort
   *
   * Since: 3.0.0
   */
  line_signals[FIND_PORT] =
    g_signal_new("find-port",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, find_port),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsLine::stop:
   * @line: the #AgsLine
   * @recall_id: the #GList-struct containing #AgsRecallID
   * @sound_scope: the sound scope
   *
   * The ::stop signal gets emited as audio stops playback.
   * 
   * Since: 3.0.0
   */
  line_signals[STOP] =
    g_signal_new("stop",
                 G_TYPE_FROM_CLASS(line),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, stop),
                 NULL, NULL,
                 ags_cclosure_marshal_VOID__POINTER_INT,
                 G_TYPE_NONE, 2,
		 G_TYPE_POINTER, G_TYPE_INT);
}

void
ags_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_line_connect;
  connectable->disconnect = ags_line_disconnect;
}

void
ags_line_init(AgsLine *line)
{
  AgsApplicationContext *application_context;
  AgsConfig *config;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(line),
				 GTK_ORIENTATION_VERTICAL);  

  application_context = ags_application_context_get_instance();

  g_signal_connect(application_context, "check-message",
		   G_CALLBACK(ags_line_check_message_callback), line);

  if(ags_line_indicator_refresh == NULL){
    ags_line_indicator_refresh = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							  NULL,
							  NULL);
  }
  
  line->flags = 0;
  line->connectable_flags = 0;

  line->version = AGS_VERSION;
  line->build_id = AGS_BUILD_ID;

  config = ags_config_get_instance();
  
  line->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  line->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  line->format = ags_soundcard_helper_config_get_format(config);

  line->channel = NULL;

  line->parent_pad = NULL;
  
  //  gtk_widget_set_can_focus(line,
  //			   TRUE);
  
  line->label = (GtkLabel *) gtk_label_new(NULL);
  gtk_box_append((GtkBox *) line,
		 (GtkWidget *) line->label);

  line->group = (GtkToggleButton *) gtk_toggle_button_new_with_label(i18n("group"));
  gtk_toggle_button_set_active(line->group, TRUE);
  gtk_box_append((GtkBox *) line,
		 (GtkWidget *) line->group);

  line->line_member = NULL;
  line->effect_separator = NULL;
  
  line->line_member_expander = ags_expander_new();
  gtk_box_append((GtkBox *) line,
		 (GtkWidget *) line->line_member_expander);
  
  line->indicator = NULL;

  line->plugin = NULL;

  line->queued_refresh = NULL;
  
  /* forwarded callbacks */
  g_signal_connect_after(line, "stop",
			 G_CALLBACK(ags_line_stop_callback), NULL);
}

void
ags_line_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsLine *line;

  line = AGS_LINE(gobject);

  switch(prop_id){
  case PROP_SAMPLERATE:
  {
    guint samplerate, old_samplerate;
      
    samplerate = g_value_get_uint(value);
    old_samplerate = line->samplerate;

    if(samplerate == old_samplerate){
      return;
    }

    line->samplerate = samplerate;

    ags_line_samplerate_changed(line,
				samplerate, old_samplerate);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size, old_buffer_size;
      
    buffer_size = g_value_get_uint(value);
    old_buffer_size = line->buffer_size;

    if(buffer_size == old_buffer_size){
      return;
    }
      
    line->buffer_size = buffer_size;

    ags_line_buffer_size_changed(line,
				 buffer_size, old_buffer_size);
  }
  break;
  case PROP_FORMAT:
  {
    guint format, old_format;
      
    format = g_value_get_uint(value);
    old_format = line->format;

    if(format == old_format){
      return;
    }

    line->format = format;

    ags_line_format_changed(line,
			    format, old_format);
  }
  break;
  case PROP_PARENT_PAD:
  {
    GtkWidget *parent_pad;

    parent_pad = (GtkWidget *) g_value_get_object(value);

    if(line->parent_pad == parent_pad){
      return;
    }

    if(line->parent_pad != NULL){
      g_object_unref(G_OBJECT(line->parent_pad));
    }

    if(parent_pad != NULL){
      g_object_ref(G_OBJECT(parent_pad));
    }
      
    line->parent_pad = parent_pad;
  }
  break;
  case PROP_CHANNEL:
  {
    AgsChannel *channel;

    channel = (AgsChannel *) g_value_get_object(value);

    ags_line_set_channel(line, channel);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsLine *line;

  line = AGS_LINE(gobject);

  switch(prop_id){
  case PROP_SAMPLERATE:
  {
    g_value_set_uint(value,
		     line->samplerate);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    g_value_set_uint(value,
		     line->buffer_size);
  }
  break;
  case PROP_FORMAT:
  {
    g_value_set_uint(value,
		     line->format);
  }
  break;
  case PROP_PARENT_PAD:
  {
    g_value_set_object(value,
		       line->parent_pad);
  }
  break;
  case PROP_CHANNEL:
  {
    g_value_set_object(value,
		       line->channel);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_dispose(GObject *gobject)
{
  AgsLine *line;

  line = AGS_LINE(gobject);

  /* channel */
  if(line->channel != NULL){
    g_object_unref(line->channel);

    line->channel = NULL;
  }  
  
  /* call parent */
  G_OBJECT_CLASS(ags_line_parent_class)->dispose(gobject);
}

void
ags_line_finalize(GObject *gobject)
{
  AgsLine *line;

  AgsApplicationContext *application_context;

  GList *list;
  
  line = AGS_LINE(gobject);

  application_context = ags_application_context_get_instance();

  g_object_disconnect(application_context,
		      "any_signal::check-message",
		      G_CALLBACK(ags_line_check_message_callback),
		      line,
		      NULL);
  
  /* remove indicator widget */
  if(line->indicator != NULL){
    g_hash_table_remove(ags_line_indicator_refresh,
			line->indicator);
  }

  /* remove of the queued drawing hash */
  list = line->queued_refresh;

  while(list != NULL){
    g_hash_table_remove(ags_line_indicator_refresh,
			list->data);

    list = list->next;
  }

  /* channel */
  if(line->channel != NULL){
    g_object_unref(line->channel);
  }  
  
  /* call parent */
  G_OBJECT_CLASS(ags_line_parent_class)->finalize(gobject);
}

void
ags_line_connect(AgsConnectable *connectable)
{
  AgsLine *line;

  GList *start_list, *list;

  line = AGS_LINE(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (line->connectable_flags)) != 0){
    return;
  }

  /* set connected flag */
  line->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

#ifdef AGS_DEBUG
  g_message("line connect");
#endif
  
  if((AGS_LINE_PREMAPPED_RECALL & (line->flags)) == 0){
    if((AGS_LINE_MAPPED_RECALL & (line->flags)) == 0){
      ags_line_map_recall(line,
			  0);
    }
  }else{
    ags_line_find_port(line);
  }

  /* connect group button */
  g_signal_connect_after((GObject *) line->group, "toggled",
			 G_CALLBACK(ags_line_group_toggled_callback), (gpointer) line);

  /* connect line members */
  list =
    start_list = ags_line_get_line_member(line);
  
  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_connect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_line_disconnect(AgsConnectable *connectable)
{
  AgsLine *line;

  GList *start_list, *list;

  line = AGS_LINE(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (line->connectable_flags)) == 0){
    return;
  }

  /* unset connected flag */
  line->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

#ifdef AGS_DEBUG
  g_message("line disconnect");
#endif

  /* disconnect group button */
  if(line->group != NULL &&
     GTK_IS_BUTTON(line->group)){
    g_object_disconnect(line->group,
			"any_signal::toggled",
			G_CALLBACK(ags_line_group_toggled_callback),
			(gpointer) line,
			NULL);
  }

  /* disconnect line members */
  list =
    start_list = ags_line_get_line_member(line);
  
  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  g_list_free(start_list);
}

/**
 * ags_line_plugin_alloc:
 * @play_container: the #AgsRecallContainer
 * @recall_container: the #AgsRecallContainer
 * @plugin_name: the plugin name
 * @filename: the filename as string
 * @effect: the effect as string
 * 
 * Allocate #AgsLinePlugin-struct.
 * 
 * Returns: the newly allocated #AgsLinePlugin-struct
 * 
 * Since: 3.3.0
 */
AgsLinePlugin*
ags_line_plugin_alloc(AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
		      gchar *plugin_name,
		      gchar *filename,
		      gchar *effect)
{
  AgsLinePlugin *line_plugin;

  line_plugin = (AgsLinePlugin *) g_malloc(sizeof(AgsLinePlugin));

  if(play_container != NULL){
    g_object_ref(play_container);
  }

  line_plugin->play_container = play_container;
  
  if(recall_container != NULL){
    g_object_ref(recall_container);
  }

  line_plugin->recall_container = recall_container;
  
  line_plugin->plugin_name = g_strdup(plugin_name);

  line_plugin->filename = g_strdup(filename);
  line_plugin->effect = g_strdup(effect);

  line_plugin->control_type_name = NULL;

  line_plugin->control_count = 0;
  
  return(line_plugin);
}

/**
 * ags_line_plugin_free:
 * @line_plugin: the #AgsLinePlugin-struct
 * 
 * Free @line_plugin.
 * 
 * Since: 3.3.0
 */
void
ags_line_plugin_free(AgsLinePlugin *line_plugin)
{
  if(line_plugin == NULL){
    return;
  }

  if(line_plugin->play_container != NULL){
    g_object_unref(line_plugin->play_container);
  }

  if(line_plugin->recall_container != NULL){
    g_object_unref(line_plugin->recall_container);
  }
  
  if(line_plugin->filename != NULL){
    g_free(line_plugin->filename);
  }

  if(line_plugin->effect != NULL){
    g_free(line_plugin->effect);
  }

  if(line_plugin->control_type_name != NULL){
    g_list_free(line_plugin->control_type_name);
  }
  
  g_free(line_plugin);
}

/**
 * ags_line_samplerate_changed:
 * @line: the #AgsLine
 * @samplerate: the samplerate
 * @old_samplerate: the old samplerate
 * 
 * Notify about samplerate changed.
 * 
 * Since: 3.0.0
 */
void
ags_line_samplerate_changed(AgsLine *line,
			    guint samplerate, guint old_samplerate)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[SAMPLERATE_CHANGED], 0,
		samplerate,
		old_samplerate);
  g_object_unref((GObject *) line);
}

/**
 * ags_line_buffer_size_changed:
 * @line: the #AgsLine
 * @buffer_size: the buffer_size
 * @old_buffer_size: the old buffer_size
 * 
 * Notify about buffer_size changed.
 * 
 * Since: 3.0.0
 */
void
ags_line_buffer_size_changed(AgsLine *line,
			     guint buffer_size, guint old_buffer_size)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[BUFFER_SIZE_CHANGED], 0,
		buffer_size,
		old_buffer_size);
  g_object_unref((GObject *) line);
}

/**
 * ags_line_format_changed:
 * @line: the #AgsLine
 * @format: the format
 * @old_format: the old format
 * 
 * Notify about format changed.
 * 
 * Since: 3.0.0
 */
void
ags_line_format_changed(AgsLine *line,
			guint format, guint old_format)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[FORMAT_CHANGED], 0,
		format,
		old_format);
  g_object_unref((GObject *) line);
}

void
ags_line_real_set_channel(AgsLine *line, AgsChannel *channel)
{
  gchar *str;
    
  if(line->channel == channel){
    return;
  }
  
  if(line->channel != NULL){    
    g_object_unref(G_OBJECT(line->channel));
  }

  if(channel != NULL){
    g_object_ref(G_OBJECT(channel));
  }

  if(line->channel != NULL){
    line->flags &= (~AGS_LINE_PREMAPPED_RECALL);
  }

  if(channel != NULL){
    line->samplerate = channel->samplerate;
    line->buffer_size = channel->buffer_size;
    line->format = channel->format;
  }
  
  line->channel = channel;

  if(channel != NULL){
    guint audio_channel;
    
    /* get audio channel */
    g_object_get(channel,
		 "audio-channel", &audio_channel,
		 NULL);
    
    /* set label */
    str = g_strdup_printf("%s %d", i18n("channel"), audio_channel + 1);
    gtk_label_set_label(line->label,
			str);

    g_free(str);
  }else{
    str = g_strdup_printf("%s (null)", i18n("channel"));
    gtk_label_set_label(line->label,
			str);

    g_free(str);
  }
}

/**
 * ags_line_set_channel:
 * @line: the #AgsLine
 * @channel: the #AgsChannel to set
 *
 * Is emitted as channel gets modified.
 *
 * Since: 3.0.0
 */
void
ags_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[SET_CHANNEL], 0,
		channel);
  g_object_unref((GObject *) line);
}

/**
 * ags_line_group_changed:
 * @line: an #AgsLine
 *
 * Is emitted as group is changed.
 *
 * Since: 3.0.0
 */
void
ags_line_group_changed(AgsLine *line)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[GROUP_CHANGED], 0);
  g_object_unref((GObject *) line);
}

/**
 * ags_line_get_line_member:
 * @line: the #AgsLine
 * 
 * Get line member of @line.
 * 
 * Returns: the #GList-struct containing #AgsLineMember
 *
 * Since: 4.0.0
 */
GList*
ags_line_get_line_member(AgsLine *line)
{
  g_return_val_if_fail(AGS_IS_LINE(line), NULL);

  return(g_list_reverse(g_list_copy(line->line_member)));
}

/**
 * ags_line_add_line_member:
 * @line: the #AgsLine
 * @line_member: the #AgsLineMember
 * @x: the x position
 * @y: the y position
 * @width: the width
 * @height: the height
 * 
 * Add @line_member to @line.
 * 
 * Since: 4.0.0
 */
void
ags_line_add_line_member(AgsLine *line,
			 AgsLineMember *line_member,
			 guint x, guint y,
			 guint width, guint height)
{
  g_return_if_fail(AGS_IS_LINE(line));
  g_return_if_fail(AGS_IS_LINE_MEMBER(line_member));

  if(g_list_find(line->line_member, line_member) == NULL){
    line->line_member = g_list_prepend(line->line_member,
				       line_member);

    line_member->parent_line = line;
    
    gtk_widget_set_vexpand((GtkWidget *) line_member,
			   FALSE);

    gtk_widget_set_valign((GtkWidget *) line_member,
			  GTK_ALIGN_START);
    
    ags_expander_add(line->line_member_expander,
		     line_member,
		     x, y,
		     width, height);
  }
}

/**
 * ags_line_remove_line_member:
 * @line: the #AgsLine
 * @line_member: the #AgsLineMember
 * 
 * Remove @line_member from @line.
 * 
 * Since: 4.0.0
 */
void
ags_line_remove_line_member(AgsLine *line,
			    AgsLineMember *line_member)
{
  g_return_if_fail(AGS_IS_LINE(line));
  g_return_if_fail(AGS_IS_LINE_MEMBER(line_member));

  if(g_list_find(line->line_member, line_member) != NULL){
    line->line_member = g_list_remove(line->line_member,
				      line_member);

    line_member->parent_line = NULL;
    
    ags_expander_remove(line->line_member_expander,
			line_member);
  }
}

/**
 * ags_line_get_effect_separator:
 * @line: the #AgsLine
 * 
 * Get line member of @line.
 * 
 * Returns: the #GList-struct containing #AgsEffectSeparator
 *
 * Since: 4.0.0
 */
GList*
ags_line_get_effect_separator(AgsLine *line)
{
  g_return_val_if_fail(AGS_IS_LINE(line), NULL);

  return(g_list_reverse(g_list_copy(line->effect_separator)));
}

/**
 * ags_line_add_effect_separator:
 * @line: the #AgsLine
 * @effect_separator: the #AgsEffectSeparator
 * @x: the x position
 * @y: the y position
 * @width: the width
 * @height: the height
 * 
 * Add @effect_separator to @line.
 * 
 * Since: 4.0.0
 */
void
ags_line_add_effect_separator(AgsLine *line,
			      AgsEffectSeparator *effect_separator,
			      guint x, guint y,
			      guint width, guint height)
{
  g_return_if_fail(AGS_IS_LINE(line));
  g_return_if_fail(AGS_IS_EFFECT_SEPARATOR(effect_separator));

  if(g_list_find(line->effect_separator, effect_separator) == NULL){
    line->effect_separator = g_list_prepend(line->effect_separator,
					    effect_separator);
    
    ags_expander_add(line->line_member_expander,
		     effect_separator,
		     x, y,
		     width, height);
  }
}

/**
 * ags_line_remove_effect_separator:
 * @line: the #AgsLine
 * @effect_separator: the #AgsEffectSeparator
 * 
 * Remove @effect_separator from @line.
 * 
 * Since: 4.0.0
 */
void
ags_line_remove_effect_separator(AgsLine *line,
				 AgsEffectSeparator *effect_separator)
{
  g_return_if_fail(AGS_IS_LINE(line));
  g_return_if_fail(AGS_IS_EFFECT_SEPARATOR(effect_separator));

  if(g_list_find(line->effect_separator, effect_separator) != NULL){
    line->effect_separator = g_list_remove(line->effect_separator,
					   effect_separator);
    
    ags_expander_remove(line->line_member_expander,
			effect_separator);
  }
}

void
ags_line_add_ladspa_plugin(AgsLine *line,
			   GList *control_type_name,
			   AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			   gchar *plugin_name,
			   gchar *filename,
			   gchar *effect,
			   guint start_audio_channel, guint stop_audio_channel,
			   guint start_pad, guint stop_pad,
			   gint position,
			   guint create_flags, guint recall_flags)
{
  AgsLineMember *line_member;
  AgsEffectSeparator *separator;

  GtkAdjustment *adjustment;
  AgsLinePlugin *line_plugin;

  AgsAudio *audio;
  AgsChannel *channel;
  
  AgsLadspaPlugin *ladspa_plugin;

  GList *start_recall;
  GList *start_list, *list;
  GList *start_plugin_port, *plugin_port;

  guint audio_channel;
  guint pad;
  gdouble page, step;
  guint port_count;
  guint control_count;

  guint x, y;
  guint k;

  audio = NULL;
  channel = NULL;
  
  audio_channel = 0;

  pad = 0;

  g_object_get(line,
	       "channel", &channel,
	       NULL);
  
  /* alloc line plugin */
  line_plugin = ags_line_plugin_alloc(play_container, recall_container,
				      plugin_name,
				      filename,
				      effect);
  line_plugin->control_type_name = control_type_name;
  
  line->plugin = g_list_append(line->plugin,
			       line_plugin);

  g_object_get(channel,
	       "audio", &audio,
	       "audio-channel", &audio_channel,
	       "pad", &pad,
	       NULL);
  
  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);

  /* ags-fx-ladspa */
  start_recall = ags_fx_factory_create(audio,
				       line_plugin->play_container, line_plugin->recall_container,
				       plugin_name,
				       filename,
				       effect,
				       audio_channel, audio_channel + 1,
				       pad, pad + 1,
				       position,
				       create_flags | (AGS_IS_OUTPUT(channel) ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT), recall_flags);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
    
  /* retrieve position within table  */
  x = 0;
  y = 0;

  list = line->line_member_expander->children;

  while(list != NULL){
    if(y <= AGS_EXPANDER_CHILD(list->data)->y){
      y = AGS_EXPANDER_CHILD(list->data)->y + 1;
    }

    list = list->next;
  }
  
  /* add separator */
  separator = ags_effect_separator_new();

  separator->play_container = play_container;
  separator->recall_container = recall_container;
  
  g_object_set(separator,
	       "text", effect,
	       "filename", filename,
	       "effect", effect,
	       NULL);
  ags_line_add_effect_separator(line,
				separator,
				0, y,
				AGS_LINE_COLUMNS_COUNT, 1);
  gtk_widget_show(GTK_WIDGET(separator));

  y++;
  
  /* load ports */
  g_object_get(ladspa_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  plugin_port = start_plugin_port;
  
  port_count = g_list_length(start_plugin_port);
  
  control_count = 0;

  k = 0;
  
  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
      GtkWidget *child_widget;

      AgsLadspaConversion *ladspa_conversion;

      GType widget_type;

      gchar *plugin_name;
      gchar *control_port;
      gchar *port_name;

      GtkOrientation widget_orientation;      
      
      guint unique_id;
      guint scale_precision;
      gdouble step_count;
      gboolean disable_seemless;
      gboolean do_step_conversion;

      GRecMutex *plugin_port_mutex;

      control_count++;

      disable_seemless = FALSE;
      do_step_conversion = FALSE;

      widget_orientation = GTK_ORIENTATION_VERTICAL;
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	disable_seemless = TRUE;

	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_orientation = GTK_ORIENTATION_HORIZONTAL;

	  widget_type = AGS_TYPE_INDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }

      if(control_type_name != NULL){
	widget_type = g_type_from_name(control_type_name->data);

	control_type_name = control_type_name->next;
      }
      
      scale_precision = AGS_DIAL_DEFAULT_PRECISION;
      step_count = AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	guint scale_steps;
	
	g_object_get(plugin_port->data,
		     "scale-steps", &scale_steps,
		     NULL);

	step_count =
	  scale_precision = (gdouble) scale_steps;

	disable_seemless = TRUE;
      }

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port->data);
      
      /* get port name */
      g_rec_mutex_lock(plugin_port_mutex);

      port_name = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);
	
      g_rec_mutex_unlock(plugin_port_mutex);
      
      /* add line member */
      g_object_get(ladspa_plugin,
		   "unique-id", &unique_id,
		   NULL);

      plugin_name = g_strdup_printf("ladspa-%u",
				    unique_id);

      control_port = g_strdup_printf("%u/%u",
				     k + 1,
				     port_count);

      line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
						   "widget-orientation", widget_orientation,
						   "widget-type", widget_type,
						   "widget-label", port_name,
						   "margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
						   "play-container", play_container,
						   "recall-container", recall_container,
						   "plugin-name", plugin_name,
						   "filename", filename,
						   "effect", effect,
						   "specifier", port_name,
						   "control-port", control_port,
						   "scale-precision", scale_precision,
						   "step-count", step_count,
						   NULL);      
      child_widget = ags_line_member_get_widget(line_member);

      g_free(plugin_name);
      g_free(control_port);
      g_free(port_name);
      
      /* ladspa conversion */
      ladspa_conversion = NULL;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_BOUNDED_BELOW)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_BELOW;
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_BOUNDED_ABOVE)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_ABOVE;
      }
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_SAMPLERATE)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_SAMPLERATE;
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_LOGARITHMIC)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}
    
	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_LOGARITHMIC;

	do_step_conversion = TRUE;
      }

      g_object_set(line_member,
		   "conversion", ladspa_conversion,
		   NULL);
      
      /* child widget */
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	line_member->port_flags = AGS_LINE_MEMBER_PORT_BOOLEAN;
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	line_member->port_flags = AGS_LINE_MEMBER_PORT_INTEGER;
      }
      
      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	float default_value;
	gdouble lower, upper;
	gdouble control_value;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}
	
	/* add controls of ports and apply range  */
	g_rec_mutex_lock(plugin_port_mutex);

	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(do_step_conversion){
	  g_object_set(ladspa_conversion,
		       "lower", lower_bound,
		       "upper", upper_bound,
		       NULL);

	  lower = 0.0;
	  upper = AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT - 1.0;

#if 0
	  if(!disable_seemless){
	    g_object_get(ladspa_conversion,
			 "step-count", &step_count,
			 NULL);
	  }
#endif
	}else{
	  lower = lower_bound;
	  upper = upper_bound;
	}

	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper >= 0.0 && lower >= 0.0){
	  step = (upper - lower) / step_count;
	}else if(upper < 0.0 && lower < 0.0){
	  step = -1.0 * (lower - upper) / step_count;
	}else{
	  step = (upper - lower) / step_count;
	}

	if(step_count > 8){
	  if(upper >= 0.0 && lower >= 0.0){
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper < 0.0 && lower < 0.0){
	    page = -1.0 * (lower - upper) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }
	}else{
	  page = step;
	}
	
	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_page_increment(adjustment,
					  page);
	gtk_adjustment_set_lower(adjustment,
				 lower);
	gtk_adjustment_set_upper(adjustment,
				 upper);

	/* get/set default value */
	g_rec_mutex_lock(plugin_port_mutex);
	
	default_value = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	control_value = default_value;

	if(ladspa_conversion != NULL){
	  control_value = ags_conversion_convert((AgsConversion *) ladspa_conversion,
						 default_value,
						 TRUE);
	}
	
	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(GTK_IS_RANGE(child_widget)){
	GtkRange *range;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	gdouble lower, upper;
	float default_value;
	gdouble control_value;
	
	range = (GtkRange *) child_widget;
	
	/* add controls of ports and apply range  */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(do_step_conversion){
	  g_object_set(ladspa_conversion,
		       "lower", lower_bound,
		       "upper", upper_bound,
		       NULL);

	  lower = 0.0;
	  upper = AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT - 1.0;
	  
#if 0
	  if(!disable_seemless){
	    g_object_get(ladspa_conversion,
			 "step-count", &step_count,
			 NULL);
	  }
#endif
	}else{
	  lower = lower_bound;
	  upper = upper_bound;
	}

	g_object_get(range,
		     "adjustment", &adjustment,
		     NULL);

	if(upper >= 0.0 && lower >= 0.0){
	  step = (upper - lower) / step_count;
	}else if(upper < 0.0 && lower < 0.0){
	  step = -1.0 * (lower - upper) / step_count;
	}else{
	  step = (upper - lower) / step_count;
	}

	if(step_count > 8){
	  if(upper >= 0.0 && lower >= 0.0){
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper < 0.0 && lower < 0.0){
	    page = -1.0 * (lower - upper) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }
	}else{
	  page = step;
	}
	
	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_page_increment(adjustment,
					  page);
	gtk_adjustment_set_lower(adjustment,
				 lower);
	gtk_adjustment_set_upper(adjustment,
				 upper);

	/* get/set default value */
	g_rec_mutex_lock(plugin_port_mutex);
	
	default_value = (float) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	control_value = default_value;

	if(ladspa_conversion != NULL){
	  control_value = ags_conversion_convert((AgsConversion *) ladspa_conversion,
						 default_value,
						 TRUE);
	}

	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(GTK_IS_SPIN_BUTTON(child_widget)){
	GtkSpinButton *spin_button;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	gdouble lower, upper;
	float default_value;
	gdouble control_value;
	
	spin_button = (GtkSpinButton *) child_widget;
	
	/* add controls of ports and apply range  */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(do_step_conversion){
	  g_object_set(ladspa_conversion,
		       "lower", lower_bound,
		       "upper", upper_bound,
		       NULL);

	  lower = 0.0;
	  upper = AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT - 1.0;
	  
#if 0
	  if(!disable_seemless){
	    g_object_get(ladspa_conversion,
			 "step-count", &step_count,
			 NULL);
	  }
#endif
	}else{
	  lower = lower_bound;
	  upper = upper_bound;
	}

	g_object_get(spin_button,
		     "adjustment", &adjustment,
		     NULL);

	if(upper >= 0.0 && lower >= 0.0){
	  step = (upper - lower) / step_count;
	}else if(upper < 0.0 && lower < 0.0){
	  step = -1.0 * (lower - upper) / step_count;
	}else{
	  step = (upper - lower) / step_count;
	}

	if(step_count > 8){
	  if(upper >= 0.0 && lower >= 0.0){
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper < 0.0 && lower < 0.0){
	    page = -1.0 * (lower - upper) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }
	}else{
	  page = step;
	}
	
	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_page_increment(adjustment,
					  page);
	gtk_adjustment_set_lower(adjustment,
				 lower);
	gtk_adjustment_set_upper(adjustment,
				 upper);

	/* get/set default value */
	g_rec_mutex_lock(plugin_port_mutex);
	
	default_value = (float) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	control_value = default_value;

	if(ladspa_conversion != NULL){
	  control_value = ags_conversion_convert((AgsConversion *) ladspa_conversion,
						 default_value,
						 TRUE);
	}

	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_line_indicator_refresh,
			    child_widget, ags_line_indicator_refresh_timeout);
	line->queued_refresh = g_list_prepend(line->queued_refresh,
					      child_widget);
	g_timeout_add((guint) (1000.0 * AGS_UI_PROVIDER_DEFAULT_TIMEOUT),
		      (GSourceFunc) ags_line_indicator_refresh_timeout,
		      (gpointer) child_widget);
      }
      
#ifdef AGS_DEBUG
      g_message("ladspa bounds: %f %f", lower, upper);
#endif
	  
      ags_line_add_line_member(line,
			       line_member,
			       x % AGS_LINE_COLUMNS_COUNT, y,
			       1, 1);

      ags_connectable_connect(AGS_CONNECTABLE(line_member));
      gtk_widget_show((GtkWidget *) line_member);
      
      /* iterate */
      x++;

      if(x % AGS_LINE_COLUMNS_COUNT == 0){
	y++;
      }
    }

    /* iterate */
    plugin_port = plugin_port->next;
    k++;
  }

  line_plugin->control_count = control_count;

  if(audio != NULL){
    g_object_unref(audio);
  }

  if(channel != NULL){
    g_object_unref(channel);
  }
  
  g_list_free_full(start_plugin_port,
		   g_object_unref);
}

void
ags_line_add_lv2_plugin(AgsLine *line,
			GList *control_type_name,
			AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			gchar *plugin_name,
			gchar *filename,
			gchar *effect,
			guint start_audio_channel, guint stop_audio_channel,
			guint start_pad, guint stop_pad,
			gint position,
			guint create_flags, guint recall_flags)
{
  AgsLineMember *line_member;
  AgsEffectSeparator *separator;

  AgsLinePlugin *line_plugin;

  AgsAudio *audio;
  AgsChannel *channel;
  
  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  GList *start_recall;
  GList *list;
  GList *start_plugin_port, *plugin_port;

  gchar *uri;

  gboolean is_lv2_plugin;

  guint audio_channel;
  guint pad;
  gdouble page, step;
  guint port_count;
  guint control_count;

  guint x, y;
  guint k;
  
  GRecMutex *lv2_manager_mutex;
  GRecMutex *base_plugin_mutex;

  lv2_manager = ags_lv2_manager_get_instance();
    
  lv2_manager_mutex = AGS_LV2_MANAGER_GET_OBJ_MUTEX(lv2_manager);

  audio = NULL;
  channel = NULL;
  
  pad = 0;
  audio_channel = 0;

  g_object_get(line,
	       "channel", &channel,
	       NULL);

  /* make sure turtle is parsed */
  g_rec_mutex_lock(lv2_manager_mutex);
	      
  is_lv2_plugin = ((lv2_manager->quick_scan_plugin_filename != NULL &&
		    g_strv_contains(lv2_manager->quick_scan_plugin_filename,
				    filename)) ||
		   (lv2_manager->quick_scan_instrument_filename != NULL &&
		    g_strv_contains(lv2_manager->quick_scan_instrument_filename,
				    filename))) ? TRUE: FALSE;
	      
  g_rec_mutex_unlock(lv2_manager_mutex);

  if(filename != NULL &&
     effect != NULL &&
     is_lv2_plugin){
    AgsTurtle *manifest;
    AgsTurtleManager *turtle_manager;
    
    gchar *path;
    gchar *manifest_filename;

    turtle_manager = ags_turtle_manager_get_instance();
    
    path = g_path_get_dirname(filename);

    manifest_filename = g_strdup_printf("%s%c%s",
					path,
					G_DIR_SEPARATOR,
					"manifest.ttl");

    manifest = (AgsTurtle *) ags_turtle_manager_find(turtle_manager,
						     manifest_filename);

    if(manifest == NULL){
      AgsLv2TurtleParser *lv2_turtle_parser;
	
      AgsTurtle **turtle;

      guint n_turtle;

      g_message("new turtle [Manifest] - %s", manifest_filename);
	
      manifest = ags_turtle_new(manifest_filename);
      ags_turtle_load(manifest,
		      NULL);
      ags_turtle_manager_add(turtle_manager,
			     (GObject *) manifest);

      lv2_turtle_parser = ags_lv2_turtle_parser_new(manifest);

      n_turtle = 1;
      turtle = (AgsTurtle **) malloc(2 * sizeof(AgsTurtle *));

      turtle[0] = manifest;
      turtle[1] = NULL;
	
      ags_lv2_turtle_parser_parse(lv2_turtle_parser,
				  turtle, n_turtle);
    
      g_object_run_dispose((GObject *) lv2_turtle_parser);
      g_object_unref(lv2_turtle_parser);
	
      g_object_unref(manifest);
	
      free(turtle);
    }
    
    g_free(manifest_filename);
  }
  
  /* alloc line plugin */
  line_plugin = ags_line_plugin_alloc(play_container, recall_container,
				      plugin_name,
				      filename,
				      effect);
  line_plugin->control_type_name = control_type_name;
  
  line->plugin = g_list_append(line->plugin,
			       line_plugin);  

  g_object_get(channel,
	       "audio", &audio,
	       "audio-channel", &audio_channel,
	       "pad", &pad,
	       NULL);
  
  /* load plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2_plugin);

  /* get uri */
  g_rec_mutex_lock(base_plugin_mutex);

  uri = g_strdup(lv2_plugin->uri);
  
  g_rec_mutex_unlock(base_plugin_mutex);

  /* ags-fx-lv2 */
  start_recall = ags_fx_factory_create(audio,
				       line_plugin->play_container, line_plugin->recall_container,
				       plugin_name,
				       filename,
				       effect,
				       audio_channel, audio_channel + 1,
				       pad, pad + 1,
				       position,
				       create_flags | (AGS_IS_OUTPUT(channel) ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT), recall_flags);
  
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
    
  /* retrieve position within table  */
  x = 0;
  y = 0;

  list = line->line_member_expander->children;

  while(list != NULL){
    if(y <= AGS_EXPANDER_CHILD(list->data)->y){
      y = AGS_EXPANDER_CHILD(list->data)->y + 1;
    }

    list = list->next;
  }
  
  /* add separator */
  separator = ags_effect_separator_new();

  separator->play_container = play_container;
  separator->recall_container = recall_container;

  g_object_set(separator,
	       "text", effect,
	       "filename", filename,
	       "effect", effect,
	       NULL);
  ags_line_add_effect_separator(line,
				separator,
				0, y,
				AGS_LINE_COLUMNS_COUNT, 1);
  gtk_widget_show(GTK_WIDGET(separator));
  
  y++;
  
  /* load ports */
  g_object_get(lv2_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  plugin_port = start_plugin_port;
  
  port_count = g_list_length(start_plugin_port);

  control_count = 0;
  
  k = 0;
  
  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
      GtkWidget *child_widget;

      AgsLv2Conversion *lv2_conversion;
      
      GType widget_type;

      gchar *plugin_name;
      gchar *control_port;
      gchar *port_name;

      GtkOrientation widget_orientation;      

      guint scale_precision;
      gdouble step_count;
      gboolean disable_seemless;
      gboolean do_step_conversion;

      GRecMutex *plugin_port_mutex;

      control_count++;

      disable_seemless = FALSE;
      do_step_conversion = FALSE;

      widget_orientation = GTK_ORIENTATION_VERTICAL;
       
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	disable_seemless = TRUE;

	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_orientation = GTK_ORIENTATION_HORIZONTAL;
      
	  widget_type = AGS_TYPE_INDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }

      if(control_type_name != NULL){
	widget_type = g_type_from_name(control_type_name->data);

	control_type_name = control_type_name->next;
      }
      
      scale_precision = AGS_DIAL_DEFAULT_PRECISION;
      step_count = AGS_LV2_CONVERSION_DEFAULT_STEP_COUNT;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	guint scale_steps;
	
	g_object_get(plugin_port->data,
		     "scale-steps", &scale_steps,
		     NULL);

	step_count =
	  scale_precision = (gdouble) scale_steps;

	disable_seemless = TRUE;
      }

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port->data);
      
      /* get port name */
      g_rec_mutex_lock(plugin_port_mutex);

      port_name = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);
	
      g_rec_mutex_unlock(plugin_port_mutex);

      /* add line member */
      plugin_name = g_strdup_printf("lv2-<%s>",
				    uri);

      control_port = g_strdup_printf("%d/%d",
				     k + 1,
				     port_count);

      line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
						   "widget-orientation", widget_orientation,
						   "widget-type", widget_type,
						   "widget-label", port_name,
						   "margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
						   "play-container", play_container,
						   "recall-container", recall_container,
						   "plugin-name", plugin_name,
						   "filename", filename,
						   "effect", effect,
						   "specifier", port_name,
						   "control-port", control_port,
						   "scale-precision", scale_precision,
						   "step-count", step_count,
						   NULL);
      child_widget = ags_line_member_get_widget(line_member);

      g_free(plugin_name);
      g_free(control_port);
      g_free(port_name);

      /* lv2 conversion */
      lv2_conversion = NULL;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_LOGARITHMIC)){
	if(lv2_conversion == NULL ||
	   !AGS_IS_LV2_CONVERSION(lv2_conversion)){
	  lv2_conversion = ags_lv2_conversion_new();
	}
    
	lv2_conversion->flags |= AGS_LV2_CONVERSION_LOGARITHMIC;

	do_step_conversion = TRUE;
      }

      g_object_set(line_member,
		   "conversion", lv2_conversion,
		   NULL);

      /* child widget */
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	line_member->port_flags = AGS_LINE_MEMBER_PORT_BOOLEAN;
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	line_member->port_flags = AGS_LINE_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	gdouble lower, upper;
	float default_value;
	gdouble control_value;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}
	
	/* add controls of ports and apply range  */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(do_step_conversion){
	  g_object_set(lv2_conversion,
		       "lower", lower_bound,
		       "upper", upper_bound,
		       NULL);

	  lower = 0.0;
	  upper = AGS_LV2_CONVERSION_DEFAULT_STEP_COUNT - 1.0;
	  
#if 0
	  if(!disable_seemless){
	    g_object_get(lv2_conversion,
			 "step-count", &step_count,
			 NULL);
	  }
#endif
	}else{
	  lower = lower_bound;
	  upper = upper_bound;
	}

	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper >= 0.0 && lower >= 0.0){
	  step = (upper - lower) / step_count;
	}else if(upper < 0.0 && lower < 0.0){
	  step = -1.0 * (lower - upper) / step_count;
	}else{
	  step = (upper - lower) / step_count;
	}

	if(step_count > 8){
	  if(upper >= 0.0 && lower >= 0.0){
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper < 0.0 && lower < 0.0){
	    page = -1.0 * (lower - upper) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }
	}else{
	  page = step;
	}
	
	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_page_increment(adjustment,
					  page);
	gtk_adjustment_set_lower(adjustment,
				 lower);
	gtk_adjustment_set_upper(adjustment,
				 upper);

	/* get/set default value */
	g_rec_mutex_lock(plugin_port_mutex);
	
	default_value = (float) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	control_value = default_value;

	if(lv2_conversion != NULL){
	  control_value = ags_conversion_convert((AgsConversion *) lv2_conversion,
						 default_value,
						 TRUE);
	}

	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(GTK_IS_RANGE(child_widget)){
	GtkRange *range;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	gdouble lower, upper;
	float default_value;
	gdouble control_value;
	
	range = (GtkRange *) child_widget;
	
	/* add controls of ports and apply range  */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(do_step_conversion){
	  g_object_set(lv2_conversion,
		       "lower", lower_bound,
		       "upper", upper_bound,
		       NULL);

	  lower = 0.0;
	  upper = AGS_LV2_CONVERSION_DEFAULT_STEP_COUNT - 1.0;
	  
#if 0
	  if(!disable_seemless){
	    g_object_get(lv2_conversion,
			 "step-count", &step_count,
			 NULL);
	  }
#endif
	}else{
	  lower = lower_bound;
	  upper = upper_bound;
	}

	g_object_get(range,
		     "adjustment", &adjustment,
		     NULL);

	if(upper >= 0.0 && lower >= 0.0){
	  step = (upper - lower) / step_count;
	}else if(upper < 0.0 && lower < 0.0){
	  step = -1.0 * (lower - upper) / step_count;
	}else{
	  step = (upper - lower) / step_count;
	}

	if(step_count > 8){
	  if(upper >= 0.0 && lower >= 0.0){
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper < 0.0 && lower < 0.0){
	    page = -1.0 * (lower - upper) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }
	}else{
	  page = step;
	}
	
	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_page_increment(adjustment,
					  page);
	gtk_adjustment_set_lower(adjustment,
				 lower);
	gtk_adjustment_set_upper(adjustment,
				 upper);

	/* get/set default value */
	g_rec_mutex_lock(plugin_port_mutex);
	
	default_value = (float) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	control_value = default_value;

	if(lv2_conversion != NULL){
	  control_value = ags_conversion_convert((AgsConversion *) lv2_conversion,
						 default_value,
						 TRUE);
	}

	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(GTK_IS_SPIN_BUTTON(child_widget)){
	GtkSpinButton *spin_button;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	gdouble lower, upper;
	float default_value;
	gdouble control_value;
	
	spin_button = (GtkSpinButton *) child_widget;
	
	/* add controls of ports and apply range  */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(do_step_conversion){
	  g_object_set(lv2_conversion,
		       "lower", lower_bound,
		       "upper", upper_bound,
		       NULL);

	  lower = 0.0;
	  upper = AGS_LV2_CONVERSION_DEFAULT_STEP_COUNT - 1.0;
	  
#if 0
	  if(!disable_seemless){
	    g_object_get(lv2_conversion,
			 "step-count", &step_count,
			 NULL);
	  }
#endif
	}else{
	  lower = lower_bound;
	  upper = upper_bound;
	}

	g_object_get(spin_button,
		     "adjustment", &adjustment,
		     NULL);

	if(upper >= 0.0 && lower >= 0.0){
	  step = (upper - lower) / step_count;
	}else if(upper < 0.0 && lower < 0.0){
	  step = -1.0 * (lower - upper) / step_count;
	}else{
	  step = (upper - lower) / step_count;
	}

	if(step_count > 8){
	  if(upper >= 0.0 && lower >= 0.0){
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper < 0.0 && lower < 0.0){
	    page = -1.0 * (lower - upper) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }
	}else{
	  page = step;
	}
	
	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_page_increment(adjustment,
					  page);
	gtk_adjustment_set_lower(adjustment,
				 lower);
	gtk_adjustment_set_upper(adjustment,
				 upper);

	/* get/set default value */
	g_rec_mutex_lock(plugin_port_mutex);
	
	default_value = (float) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	control_value = default_value;

	if(lv2_conversion != NULL){
	  control_value = ags_conversion_convert((AgsConversion *) lv2_conversion,
						 default_value,
						 TRUE);
	}

	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(AGS_IS_INDICATOR(child_widget)){
	g_hash_table_insert(ags_line_indicator_refresh,
			    child_widget,
			    ags_line_indicator_refresh_timeout);

	line->queued_refresh = g_list_prepend(line->queued_refresh,
					      child_widget);
	g_timeout_add(1000 / 30,
		      (GSourceFunc) ags_line_indicator_refresh_timeout,
		      (gpointer) child_widget);
      }

#ifdef AGS_DEBUG
      g_message("lv2 bounds: %f %f", lower, upper);
#endif

      ags_line_add_line_member(line,
			       line_member,
			       x % AGS_LINE_COLUMNS_COUNT, y,
			       1, 1);
      ags_connectable_connect(AGS_CONNECTABLE(line_member));
      gtk_widget_show((GtkWidget *) line_member);

      /* iterate */
      x++;

      if(x % AGS_LINE_COLUMNS_COUNT == 0){
	y++;
      }
    }

    /* iterate */
    plugin_port = plugin_port->next;
    k++;
  }

  line_plugin->control_count = control_count;

  if(audio != NULL){
    g_object_unref(audio);
  }

  if(channel != NULL){
    g_object_unref(channel);
  }

  g_list_free_full(start_plugin_port,
		   g_object_unref);

  g_free(uri);
}

#if defined(AGS_WITH_VST3)
void
ags_line_add_vst3_plugin(AgsLine *line,
			GList *control_type_name,
			AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			gchar *plugin_name,
			gchar *filename,
			gchar *effect,
			guint start_audio_channel, guint stop_audio_channel,
			guint start_pad, guint stop_pad,
			gint position,
			guint create_flags, guint recall_flags)
{
  AgsLineMember *line_member;
  AgsEffectSeparator *separator;

  AgsLinePlugin *line_plugin;

  AgsAudio *audio;
  AgsChannel *channel;
  
  AgsVst3Manager *vst3_manager;
  AgsVst3Plugin *vst3_plugin;

  GList *start_recall;
  GList *list;
  GList *start_plugin_port, *plugin_port;

  guint audio_channel;
  guint pad;
  gdouble page, step;
  guint port_count;
  guint control_count;

  guint x, y;
  guint k;
  
  GRecMutex *vst3_manager_mutex;
  GRecMutex *base_plugin_mutex;

  vst3_manager = ags_vst3_manager_get_instance();
    
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);

  audio = NULL;
  channel = NULL;
  
  pad = 0;
  audio_channel = 0;

  g_object_get(line,
	       "channel", &channel,
	       NULL);
  
  /* alloc line plugin */
  line_plugin = ags_line_plugin_alloc(play_container, recall_container,
				      plugin_name,
				      filename,
				      effect);
  line_plugin->control_type_name = control_type_name;
  
  line->plugin = g_list_append(line->plugin,
			       line_plugin);  

  g_object_get(channel,
	       "audio", &audio,
	       "audio-channel", &audio_channel,
	       "pad", &pad,
	       NULL);
  
  /* load plugin */
  vst3_plugin = ags_vst3_manager_find_vst3_plugin(ags_vst3_manager_get_instance(),
						  filename, effect);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);

  /* ags-fx-vst3 */
  start_recall = ags_fx_factory_create(audio,
				       line_plugin->play_container, line_plugin->recall_container,
				       plugin_name,
				       filename,
				       effect,
				       audio_channel, audio_channel + 1,
				       pad, pad + 1,
				       position,
				       create_flags | (AGS_IS_OUTPUT(channel) ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT), recall_flags);
  
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
    
  /* retrieve position within table  */
  x = 0;
  y = 0;

  list = line->line_member_expander->children;

  while(list != NULL){
    if(y <= AGS_EXPANDER_CHILD(list->data)->y){
      y = AGS_EXPANDER_CHILD(list->data)->y + 1;
    }

    list = list->next;
  }
  
  /* add separator */
  separator = ags_effect_separator_new();

  separator->play_container = play_container;
  separator->recall_container = recall_container;

  g_object_set(separator,
	       "text", effect,
	       "filename", filename,
	       "effect", effect,
	       NULL);
  ags_line_add_effect_separator(line,
				(GtkWidget *) separator,
				0, y,
				AGS_LINE_COLUMNS_COUNT, 1);
  gtk_widget_show((GtkWidget *) separator);
  
  y++;
  
  /* load ports */
  g_object_get(vst3_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  plugin_port = start_plugin_port;
  
  port_count = g_list_length(start_plugin_port);

  control_count = 0;
  
  k = 0;
  
  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL) &&
       !ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_HIDDEN)){
      GtkWidget *child_widget;

      AgsVst3Conversion *vst3_conversion;
      
      GType widget_type;

      gchar *plugin_name;
      gchar *control_port;
      gchar *port_name;

      GtkOrientation widget_orientation;      

      guint scale_precision;
      gdouble step_count;
      gboolean disable_seemless;
      gboolean do_step_conversion;

      GRecMutex *plugin_port_mutex;

      control_count++;

      disable_seemless = FALSE;
      do_step_conversion = FALSE;

      widget_orientation = GTK_ORIENTATION_VERTICAL;
       
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	disable_seemless = TRUE;

	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_orientation = GTK_ORIENTATION_HORIZONTAL;
      
	  widget_type = AGS_TYPE_INDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }

      if(control_type_name != NULL){
	widget_type = g_type_from_name(control_type_name->data);

	control_type_name = control_type_name->next;
      }
      
      scale_precision = AGS_DIAL_DEFAULT_PRECISION;
      step_count = AGS_VST3_CONVERSION_DEFAULT_STEP_COUNT;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	guint scale_steps;
	
	g_object_get(plugin_port->data,
		     "scale-steps", &scale_steps,
		     NULL);

	step_count =
	  scale_precision = (gdouble) scale_steps;

	disable_seemless = TRUE;
      }

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port->data);
      
      /* get port name */
      g_rec_mutex_lock(plugin_port_mutex);

      port_name = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);
	
      g_rec_mutex_unlock(plugin_port_mutex);

      /* add line member */
      plugin_name = g_strdup_printf("vst3-<%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x>",
				    vst3_plugin->cid[0],
				    vst3_plugin->cid[1],
				    vst3_plugin->cid[2],
				    vst3_plugin->cid[3],
				    vst3_plugin->cid[4],
				    vst3_plugin->cid[5],
				    vst3_plugin->cid[6],
				    vst3_plugin->cid[7],
				    vst3_plugin->cid[8],
				    vst3_plugin->cid[9],
				    vst3_plugin->cid[10],
				    vst3_plugin->cid[11],
				    vst3_plugin->cid[12],
				    vst3_plugin->cid[13],
				    vst3_plugin->cid[14],
				    vst3_plugin->cid[15]);

      control_port = g_strdup_printf("%d/%d",
				     k + 1,
				     port_count);

      line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
						   "widget-orientation", widget_orientation,
						   "widget-type", widget_type,
						   "widget-label", port_name,
						   "margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
						   "play-container", play_container,
						   "recall-container", recall_container,
						   "plugin-name", plugin_name,
						   "filename", filename,
						   "effect", effect,
						   "specifier", port_name,
						   "control-port", control_port,
						   "scale-precision", scale_precision,
						   "step-count", step_count,
						   NULL);
      child_widget = ags_line_member_get_widget(line_member);

      g_free(plugin_name);
      g_free(control_port);
      g_free(port_name);

      /* vst3 conversion */
      vst3_conversion = NULL;

      g_object_set(line_member,
		   "conversion", vst3_conversion,
		   NULL);

      /* child widget */
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	line_member->port_flags = AGS_LINE_MEMBER_PORT_BOOLEAN;
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	line_member->port_flags = AGS_LINE_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	gdouble lower, upper;
	float default_value;
	gdouble control_value;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}
	
	/* add controls of ports and apply range  */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(do_step_conversion){
	  g_object_set(vst3_conversion,
		       "lower", lower_bound,
		       "upper", upper_bound,
		       NULL);

	  lower = 0.0;
	  upper = AGS_VST3_CONVERSION_DEFAULT_STEP_COUNT - 1.0;
	  
#if 0
	  if(!disable_seemless){
	    g_object_get(vst3_conversion,
			 "step-count", &step_count,
			 NULL);
	  }
#endif
	}else{
	  lower = lower_bound;
	  upper = upper_bound;
	}

	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper >= 0.0 && lower >= 0.0){
	  step = (upper - lower) / step_count;
	}else if(upper < 0.0 && lower < 0.0){
	  step = -1.0 * (lower - upper) / step_count;
	}else{
	  step = (upper - lower) / step_count;
	}

	if(step_count > 8){
	  if(upper >= 0.0 && lower >= 0.0){
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper < 0.0 && lower < 0.0){
	    page = -1.0 * (lower - upper) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }
	}else{
	  page = step;
	}
	
	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_page_increment(adjustment,
					  page);
	gtk_adjustment_set_lower(adjustment,
				 lower);
	gtk_adjustment_set_upper(adjustment,
				 upper);

	/* get/set default value */
	g_rec_mutex_lock(plugin_port_mutex);
	
	default_value = (float) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	control_value = default_value;

	if(vst3_conversion != NULL){
	  control_value = ags_conversion_convert((AgsConversion *) vst3_conversion,
						 default_value,
						 TRUE);
	}

	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(GTK_IS_RANGE(child_widget)){
	GtkRange *range;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	gdouble lower, upper;
	float default_value;
	gdouble control_value;
	
	range = (GtkRange *) child_widget;
	
	/* add controls of ports and apply range  */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(do_step_conversion){
	  g_object_set(vst3_conversion,
		       "lower", lower_bound,
		       "upper", upper_bound,
		       NULL);

	  lower = 0.0;
	  upper = AGS_VST3_CONVERSION_DEFAULT_STEP_COUNT - 1.0;
	  
#if 0
	  if(!disable_seemless){
	    g_object_get(vst3_conversion,
			 "step-count", &step_count,
			 NULL);
	  }
#endif
	}else{
	  lower = lower_bound;
	  upper = upper_bound;
	}

	g_object_get(range,
		     "adjustment", &adjustment,
		     NULL);

	if(upper >= 0.0 && lower >= 0.0){
	  step = (upper - lower) / step_count;
	}else if(upper < 0.0 && lower < 0.0){
	  step = -1.0 * (lower - upper) / step_count;
	}else{
	  step = (upper - lower) / step_count;
	}

	if(step_count > 8){
	  if(upper >= 0.0 && lower >= 0.0){
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper < 0.0 && lower < 0.0){
	    page = -1.0 * (lower - upper) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }
	}else{
	  page = step;
	}
	
	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_page_increment(adjustment,
					  page);
	gtk_adjustment_set_lower(adjustment,
				 lower);
	gtk_adjustment_set_upper(adjustment,
				 upper);

	/* get/set default value */
	g_rec_mutex_lock(plugin_port_mutex);
	
	default_value = (float) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	control_value = default_value;

	if(vst3_conversion != NULL){
	  control_value = ags_conversion_convert((AgsConversion *) vst3_conversion,
						 default_value,
						 TRUE);
	}

	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(GTK_IS_SPIN_BUTTON(child_widget)){
	GtkSpinButton *spin_button;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	gdouble lower, upper;
	float default_value;
	gdouble control_value;
	
	spin_button = (GtkSpinButton *) child_widget;
	
	/* add controls of ports and apply range  */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(do_step_conversion){
	  g_object_set(vst3_conversion,
		       "lower", lower_bound,
		       "upper", upper_bound,
		       NULL);

	  lower = 0.0;
	  upper = AGS_VST3_CONVERSION_DEFAULT_STEP_COUNT - 1.0;
	  
#if 0
	  if(!disable_seemless){
	    g_object_get(vst3_conversion,
			 "step-count", &step_count,
			 NULL);
	  }
#endif
	}else{
	  lower = lower_bound;
	  upper = upper_bound;
	}

	g_object_get(spin_button,
		     "adjustment", &adjustment,
		     NULL);

	if(upper >= 0.0 && lower >= 0.0){
	  step = (upper - lower) / step_count;
	}else if(upper < 0.0 && lower < 0.0){
	  step = -1.0 * (lower - upper) / step_count;
	}else{
	  step = (upper - lower) / step_count;
	}

	if(step_count > 8){
	  if(upper >= 0.0 && lower >= 0.0){
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper < 0.0 && lower < 0.0){
	    page = -1.0 * (lower - upper) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }
	}else{
	  page = step;
	}
	
	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_page_increment(adjustment,
					  page);
	gtk_adjustment_set_lower(adjustment,
				 lower);
	gtk_adjustment_set_upper(adjustment,
				 upper);

	/* get/set default value */
	g_rec_mutex_lock(plugin_port_mutex);
	
	default_value = (float) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	control_value = default_value;

	if(vst3_conversion != NULL){
	  control_value = ags_conversion_convert((AgsConversion *) vst3_conversion,
						 default_value,
						 TRUE);
	}

	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(AGS_IS_INDICATOR(child_widget)){
	g_hash_table_insert(ags_line_indicator_refresh,
			    child_widget,
			    ags_line_indicator_refresh_timeout);

	line->queued_refresh = g_list_prepend(line->queued_refresh,
					      child_widget);
	g_timeout_add(1000 / 30,
		      (GSourceFunc) ags_line_indicator_refresh_timeout,
		      (gpointer) child_widget);
      }

#ifdef AGS_DEBUG
      g_message("vst3 bounds: %f %f", lower, upper);
#endif

      ags_line_add_line_member(line,
			       (GtkWidget *) line_member,
			       x % AGS_LINE_COLUMNS_COUNT, y,
			       1, 1);
      ags_connectable_connect(AGS_CONNECTABLE(line_member));
      gtk_widget_show((GtkWidget *) line_member);

      /* iterate */
      x++;

      if(x % AGS_LINE_COLUMNS_COUNT == 0){
	y++;
      }
    }

    /* iterate */
    plugin_port = plugin_port->next;
    k++;
  }

  line_plugin->control_count = control_count;

  if(audio != NULL){
    g_object_unref(audio);
  }

  if(channel != NULL){
    g_object_unref(channel);
  }

  g_list_free_full(start_plugin_port,
		   g_object_unref);
}
#endif

void
ags_line_real_add_plugin(AgsLine *line,
			 GList *control_type_name,
			 AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			 gchar *plugin_name,
			 gchar *filename,
			 gchar *effect,
			 guint start_audio_channel, guint stop_audio_channel,
			 guint start_pad, guint stop_pad,
			 gint position,
			 guint create_flags, guint recall_flags)
{
  AgsBasePlugin *base_plugin;

  gchar *fallback_filename;

  base_plugin = NULL;

  fallback_filename = NULL;
  
  if(!g_ascii_strncasecmp(plugin_name,
			  "ags-fx-ladspa",
			  14)){   
    base_plugin = (AgsBasePlugin *) ags_ladspa_manager_find_ladspa_plugin_with_fallback(ags_ladspa_manager_get_instance(),
											filename, effect);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-lv2",
				11)){
    base_plugin = (AgsBasePlugin *) ags_lv2_manager_find_lv2_plugin_with_fallback(ags_lv2_manager_get_instance(),
										  filename, effect);
  }

  if(base_plugin != NULL){
    g_object_get(base_plugin,
		 "filename", &fallback_filename,
		 NULL);
  }else{
    fallback_filename = g_strdup(filename);
  }

  if((AGS_FX_FACTORY_ADD & (create_flags)) != 0){
    if(!g_ascii_strncasecmp(plugin_name,
			    "ags-fx-ladspa",
			    14)){
      ags_line_add_ladspa_plugin(line,
				 control_type_name,
				 play_container, recall_container,
				 plugin_name,
				 fallback_filename,
				 effect,
				 start_audio_channel, stop_audio_channel,
				 start_pad, stop_pad,
				 position,
				 create_flags, recall_flags);
    }else if(!g_ascii_strncasecmp(plugin_name,
				  "ags-fx-lv2",
				  11)){
      ags_line_add_lv2_plugin(line,
			      control_type_name,
			      play_container, recall_container,
			      plugin_name,
			      fallback_filename,
			      effect,
			      start_audio_channel, stop_audio_channel,
			      start_pad, stop_pad,
			      position,
			      create_flags, recall_flags);
    }else if(!g_ascii_strncasecmp(plugin_name,
				  "ags-fx-vst3",
				  12)){
#if defined(AGS_WITH_VST3)
      ags_line_add_vst3_plugin(line,
			       control_type_name,
			       play_container, recall_container,
			       plugin_name,
			       fallback_filename,
			       effect,
			       start_audio_channel, stop_audio_channel,
			       start_pad, stop_pad,
			       position,
			       create_flags, recall_flags);
#endif
    }
  }

  g_free(fallback_filename);
}

/**
 * ags_line_add_plugin:
 * @line: the #AgsLine to modify
 * @control_type_name: the #GList-struct containing string representation of a #GType
 * @play_container: an #AgsRecallContainer to indetify what recall to use
 * @recall_container: an #AgsRecallContainer to indetify what recall to use
 * @plugin_name: the plugin identifier
 * @filename: the effect's filename
 * @effect: the effect's name
 * @start_audio_channel: the first audio channel to apply
 * @stop_audio_channel: the last audio channel to apply
 * @start_pad: the first pad to apply
 * @stop_pad: the last pad to apply
 * @position: the position to insert the recall
 * @create_flags: modify the behaviour of this function
 * @recall_flags: flags to be set for #AgsRecall
 *
 * Add an effect by its filename and effect specifier.
 *
 * Since: 3.0.0
 */
void
ags_line_add_plugin(AgsLine *line,
		    GList *control_type_name,
		    AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
		    gchar *plugin_name,
		    gchar *filename,
		    gchar *effect,
		    guint start_audio_channel, guint stop_audio_channel,
		    guint start_pad, guint stop_pad,
		    gint position,
		    guint create_flags, guint recall_flags)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[ADD_PLUGIN], 0,
		control_type_name,
		play_container, recall_container,
		plugin_name,
		filename,
		effect,
		start_audio_channel, stop_audio_channel,
		start_pad, stop_pad,
		position,
		create_flags, recall_flags);
  g_object_unref((GObject *) line);
}

void
ags_line_real_remove_plugin(AgsLine *line,
			    guint nth)
{
  AgsLinePlugin *line_plugin;

  AgsAudio *audio;

  GList *start_list, *list;
  GList *start_recall, *recall;
  
  if(!AGS_IS_LINE(line)){
    return;
  }

  audio = NULL;
  
  list = g_list_nth(line->plugin,
		    nth);

  if(list == NULL){
    return;
  }
  
  line_plugin = list->data;

  g_object_get(line->channel,
	       "audio", &audio,
	       NULL);

  /*  */  
  line->plugin = g_list_remove(line->plugin,
			       line_plugin);

  /* AgsRecallAudio */
  ags_audio_remove_recall(audio, (GObject *) ags_recall_container_get_recall_audio(line_plugin->play_container),
			  TRUE);

  ags_audio_remove_recall(audio, (GObject *) ags_recall_container_get_recall_audio(line_plugin->recall_container),
			  FALSE);

  /* AgsRecallAudioRun - play context */
  g_object_get(line_plugin->play_container,
	       "recall-audio-run", &start_recall,
	       NULL);
  
  recall = start_recall;

  while(recall != NULL){
    ags_audio_remove_recall(audio, (GObject *) recall->data,
			    TRUE);

    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* AgsRecallAudioRun - recall context */
  g_object_get(line_plugin->recall_container,
	       "recall-audio-run", &start_recall,
	       NULL);
  
  recall = start_recall;

  while(recall != NULL){
    ags_audio_remove_recall(audio, (GObject *) recall->data,
			    FALSE);
    
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* AgsRecallChannel - play context */
  g_object_get(line_plugin->play_container,
	       "recall-channel", &start_recall,
	       NULL);
  
  recall = start_recall;

  while(recall != NULL){
    AgsChannel *channel;

    g_object_get(recall->data,
		 "source", &channel,
		 NULL);
    
    ags_channel_remove_recall(channel, (GObject *) recall->data,
			      TRUE);

    if(channel != NULL){
      g_object_unref(channel);
    }

    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* AgsRecallChannel - recall context */
  g_object_get(line_plugin->recall_container,
	       "recall-channel", &start_recall,
	       NULL);
  
  recall = start_recall;

  while(recall != NULL){
    AgsChannel *channel;

    g_object_get(recall->data,
		 "source", &channel,
		 NULL);
    
    ags_channel_remove_recall(channel, (GObject *) recall->data,
			      FALSE);
    

    if(channel != NULL){
      g_object_unref(channel);
    }

    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  /* AgsRecallChannelRun - play context */
  g_object_get(line_plugin->play_container,
	       "recall-channel-run", &start_recall,
	       NULL);
  
  recall = start_recall;

  while(recall != NULL){
    AgsChannel *channel;

    g_object_get(recall->data,
		 "source", &channel,
		 NULL);
    
    ags_channel_remove_recall(channel, (GObject *) recall->data,
			      TRUE);    

    if(channel != NULL){
      g_object_unref(channel);
    }

    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* AgsRecallChannelRun - recall context */
  g_object_get(line_plugin->recall_container,
	       "recall-channel-run", &start_recall,
	       NULL);
  
  recall = start_recall;

  while(recall != NULL){
    AgsChannel *channel;

    g_object_get(recall->data,
		 "source", &channel,
		 NULL);
    
    ags_channel_remove_recall(channel, (GObject *) recall->data,
			      FALSE);    

    if(channel != NULL){
      g_object_unref(channel);
    }

    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* recall container */
  ags_audio_remove_recall_container(audio, (GObject *) line_plugin->play_container);
  ags_audio_remove_recall_container(audio, (GObject *) line_plugin->recall_container);

  ags_channel_remove_recall_container(line->channel, (GObject *) line_plugin->play_container);
  ags_channel_remove_recall_container(line->channel, (GObject *) line_plugin->recall_container);

  /* destroy controls - line member */
  start_list = ags_line_get_line_member(line);

  list = start_list;
  
  while(list != NULL){
    if(AGS_IS_LINE_MEMBER(list->data) &&
       AGS_LINE_MEMBER(list->data)->play_container == line_plugin->play_container){
      GtkWidget *child_widget;

      child_widget = ags_line_member_get_widget(list->data);
      
      if(AGS_IS_INDICATOR(child_widget) ||
	 AGS_IS_LED(child_widget)){
	g_hash_table_remove(ags_line_indicator_refresh,
			    child_widget);
      }

      ags_line_remove_line_member(line,
				  list->data);
    }
    
    list = list->next;
  }
  
  g_list_free(start_list);

  /* destroy controls - effect separator */
  start_list = ags_line_get_effect_separator(line);

  list = start_list;
  
  while(list != NULL){
    if(AGS_IS_EFFECT_SEPARATOR(list->data) &&
       AGS_EFFECT_SEPARATOR(list->data)->play_container == line_plugin->play_container){
      ags_line_remove_effect_separator(line,
				       list->data);
    }
    
    list = list->next;
  }
  
  g_list_free(start_list);

  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  /* free AgsLinePlugin */
  ags_line_plugin_free(line_plugin);
}

/**
 * ags_line_remove_plugin:
 * @line: the #AgsLine to modify
 * @nth: the nth effect to remove
 *
 * Remove an effect by its position.
 *
 * Since: 3.0.0
 */
void
ags_line_remove_plugin(AgsLine *line,
		       guint nth)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[REMOVE_PLUGIN], 0,
		nth);
  g_object_unref((GObject *) line);
}

void
ags_line_real_map_recall(AgsLine *line,
			 guint output_pad_start)
{
  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0){
    return;
  }
  
  line->flags |= AGS_LINE_MAPPED_RECALL;

  ags_line_find_port(line);
}

/**
 * ags_line_map_recall:
 * @line: an #AgsLine
 * @output_pad_start: the start pad
 *
 * Is emitted as group is changed.
 *
 * Since: 3.0.0
 */
void
ags_line_map_recall(AgsLine *line,
		    guint output_pad_start)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[MAP_RECALL], 0,
		output_pad_start);
  g_object_unref((GObject *) line);
}

GList*
ags_line_real_find_port(AgsLine *line)
{
  GList *port, *tmp_port;
  GList *start_line_member, *line_member;

  if(line == NULL || line->line_member == NULL){
    return(NULL);
  }

  line_member =
    start_line_member = ags_line_get_line_member(line);
  
  port = NULL;

  while(line_member != NULL){
    if(AGS_IS_LINE_MEMBER(line_member->data)){
      tmp_port = ags_line_member_find_port(AGS_LINE_MEMBER(line_member->data));

      if(port != NULL){
	port = g_list_concat(port,
			     tmp_port);
      }else{
	port = tmp_port;
      }
    }

    line_member = line_member->next;
  }

  g_list_free(start_line_member);
  
  return(port);
}

/**
 * ags_line_find_port:
 * @line: the #AgsLine
 *
 * Lookup ports of assigned recalls.
 *
 * Returns: a #GList-struct containing all related #AgsPort
 *
 * Since: 3.0.0
 */
GList*
ags_line_find_port(AgsLine *line)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_LINE(line),
		       NULL);

  g_object_ref((GObject *) line);
  g_signal_emit((GObject *) line,
		line_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) line);

  return(list);
}

/**
 * ags_line_stop:
 * @line: the #AgsLine
 * @recall_id: the #GList-struct containing #AgsRecallID
 * @sound_scope: the sound scope
 *
 * Notify about to stop playback of @recall_id.
 * 
 * Since: 3.0.0
 */
void
ags_line_stop(AgsLine *line,
	      GList *recall_id, gint sound_scope)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit((GObject *) line,
		line_signals[STOP], 0,
		recall_id, sound_scope);
  g_object_unref((GObject *) line);
}

/**
 * ags_line_find_next_grouped:
 * @line: a #GList-struct of #AgsLine objects
 *
 * Retrieve next grouped line.
 *
 * Returns: next matching #GList-struct containing #AgsLine
 * 
 * Since: 3.0.0
 */
GList*
ags_line_find_next_grouped(GList *line)
{
  while(line != NULL && !gtk_toggle_button_get_active(AGS_LINE(line->data)->group)){
    line = line->next;
  }

  return(line);
}

/**
 * ags_line_check_message:
 * @line: the #AgsLine
 *
 * Check message queue for message envelopes.
 * 
 * Since: 3.0.0
 */
void
ags_line_check_message(AgsLine *line)
{
  AgsChannel *channel;
    
  AgsMessageDelivery *message_delivery;

  GList *start_message_envelope, *message_envelope;

  if(!AGS_LINE(line)){
    return;
  }
  
  /* retrieve message delivery */
  message_delivery = ags_message_delivery_get_instance();

  channel = line->channel;
  
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
		     BAD_CAST "AgsChannel::set-samplerate",
		     27)){
	guint samplerate;
	gint position;
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "samplerate");
	samplerate = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	/* set samplerate */
	g_object_set(line,
		     "samplerate", samplerate,
		     NULL);
      }else if(!xmlStrncmp(xmlGetProp(root_node,
				      "method"),
			   BAD_CAST "AgsChannel::set-buffer-size",
			   28)){
	guint buffer_size;
	gint position;
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "buffer-size");
	buffer_size = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	/* set buffer size */
	g_object_set(line,
		     "buffer-size", buffer_size,
		     NULL);
      }else if(!xmlStrncmp(xmlGetProp(root_node,
				      "method"),
			   BAD_CAST "AgsChannel::set-format",
			   23)){
	guint format;
	gint position;
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "format");
	format = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	/* set format */
	g_object_set(line,
		     "format", format,
		     NULL);
      }else if(!xmlStrncmp(xmlGetProp(root_node,
				      "method"),
			   BAD_CAST "AgsChannel::stop",
			   18)){
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
	ags_line_stop(line,
		      recall_id, sound_scope);
      }
    }
      
    message_envelope = message_envelope->next;
  }
    
  g_list_free_full(start_message_envelope,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_line_indicator_refresh_timeout:
 * @widget: the widget
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_line_indicator_refresh_timeout(GtkWidget *widget)
{
  AgsLine *line;

  if(g_hash_table_lookup(ags_line_indicator_refresh,
			 widget) != NULL){      
    GList *start_list, *list;
    
    line = (AgsLine *) gtk_widget_get_ancestor(widget,
					       AGS_TYPE_LINE);

    if(line == NULL){
      return(TRUE);
    }
    
    list =
      start_list = ags_line_get_line_member(line);
    
    /* check members */
    while(list != NULL){
      if(AGS_IS_LINE_MEMBER(list->data) &&
	 (AGS_LINE_MEMBER(list->data)->widget_type == AGS_TYPE_INDICATOR ||
	  AGS_LINE_MEMBER(list->data)->widget_type == AGS_TYPE_LED)){
	AgsLineMember *line_member;
	GtkAdjustment *adjustment;
	GtkWidget *child;

	AgsPort *current;
	
	AgsPluginPort *plugin_port;
	
	gdouble average_peak;
	gdouble lower, upper;
	gdouble range;
	gdouble peak;
	gboolean success;
	
	GValue value = {0,};

	GRecMutex *port_mutex;
	GRecMutex *plugin_port_mutex;
	
	line_member = AGS_LINE_MEMBER(list->data);
	child = ags_line_member_get_widget(line_member);
      
	average_peak = 0.0;
      
	/* play port */
	current = line_member->port;

	if(current == NULL){
	  list = list->next;
	
	  continue;
	}
	
	/* check if output port and specifier matches */
	if(!ags_port_test_flags(current, AGS_PORT_IS_OUTPUT)){
	  list = list->next;

	  continue;
	}

	plugin_port = NULL;
	
	g_object_get(current,
		     "plugin-port", &plugin_port,
		     NULL);
	
	if(plugin_port == NULL){
	  list = list->next;

	  continue;
	}

	/* get port mutex */
	port_mutex = AGS_PORT_GET_OBJ_MUTEX(current);

	/* match specifier */
	g_rec_mutex_lock(port_mutex);

	success = (!g_ascii_strcasecmp(current->specifier,
				       line_member->specifier)) ? TRUE: FALSE;
	
	g_rec_mutex_unlock(port_mutex);

	if(!success){
	  list = list->next;

	  g_object_unref(plugin_port);
	
	  continue;
	}

	/* get plugin port mutex */
	plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port);
	
	/* lower and upper */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower = (gdouble) g_value_get_float(plugin_port->lower_value);
	upper = (gdouble) g_value_get_float(plugin_port->upper_value);
      
	g_rec_mutex_unlock(plugin_port_mutex);
	
	/* get range */
	if(line_member->conversion != NULL){
	  lower = ags_conversion_convert(line_member->conversion,
					 lower,
					 TRUE);

	  upper = ags_conversion_convert(line_member->conversion,
					 upper,
					 TRUE);
	}
      
	range = upper - lower;
      
	/* play port - read value */
	g_value_init(&value, G_TYPE_FLOAT);
	ags_port_safe_read(current,
			   &value);
      
	peak = (gdouble) g_value_get_float(&value);
	g_value_unset(&value);

	if(line_member->conversion != NULL){
	  peak = ags_conversion_convert(line_member->conversion,
					peak,
					TRUE);
	}
      
	/* calculate peak */
	if(range == 0.0 ||
	   current->port_value_type == G_TYPE_BOOLEAN){
	  if(peak != 0.0){
	    average_peak = 1.0;
	  }
	}else{
	  average_peak += peak;
	}

	/* recall port */
	current = line_member->recall_port;

	/* recall port - read value */
	g_value_init(&value, G_TYPE_FLOAT);
	ags_port_safe_read(current,
			   &value);
      
	peak = (gdouble) g_value_get_float(&value);
	g_value_unset(&value);

	if(line_member->conversion != NULL){
	  peak = ags_conversion_convert(line_member->conversion,
					peak,
					TRUE);
	}

	/* calculate peak */
	if(range == 0.0 ||
	   current->port_value_type == G_TYPE_BOOLEAN){
	  if(peak != 0.0){
	    average_peak = 1.0;
	  }
	}else{
	  average_peak += peak;
	}
      
	/* apply */
	if(AGS_IS_LED(child)){
	  if(average_peak != 0.0){
	    ags_led_set_active(AGS_LED(child),
			       TRUE);
	  }
	}else{
	  adjustment = NULL;
	  
	  g_object_get(child,
		       "adjustment", &adjustment,
		       NULL);

//	  g_message("%f", average_peak);
	  
	  gtk_adjustment_set_value(adjustment,
				   10.0 * average_peak);
	}

	g_object_unref(plugin_port);
      }
    
      list = list->next;
    }

    g_list_free(start_list);
    
    return(TRUE);
  }
  
  return(FALSE);
}

/**
 * ags_line_new:
 * @parent_pad: the parent pad
 * @channel: the #AgsChannel to visualize
 *
 * Create a new instance of #AgsLine
 *
 * Returns: the new #AgsLine
 *
 * Since: 3.0.0
 */
AgsLine*
ags_line_new(GtkWidget *parent_pad, AgsChannel *channel)
{
  AgsLine *line;

  line = (AgsLine *) g_object_new(AGS_TYPE_LINE,
				  "parent-pad", parent_pad,
				  "channel", channel,
				  NULL);

  return(line);
}
