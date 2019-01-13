/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/X/ags_line.h>
#include <ags/X/ags_line_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line_member.h>
#include <ags/X/ags_effect_separator.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_pad_editor.h>
#include <ags/X/ags_line_editor.h>
#include <ags/X/ags_line_member_editor.h>
#include <ags/X/ags_plugin_browser.h>
#include <ags/X/ags_ladspa_browser.h>
#include <ags/X/ags_dssi_browser.h>
#include <ags/X/ags_lv2_browser.h>

#include <ladspa.h>
#include <dlfcn.h>

#include <ags/i18n.h>

void ags_line_class_init(AgsLineClass *line);
void ags_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_line_plugin_interface_init(AgsPluginInterface *plugin);
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

gchar* ags_line_get_version(AgsPlugin *plugin);
void ags_line_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_line_get_build_id(AgsPlugin *plugin);
void ags_line_set_build_id(AgsPlugin *plugin, gchar *build_id);

void ags_line_real_set_channel(AgsLine *line, AgsChannel *channel);
GList* ags_line_add_ladspa_effect(AgsLine *line,
				  GList *control_type_name,
				  gchar *filename,
				  gchar *effect);
GList* ags_line_add_lv2_effect(AgsLine *line,
			       GList *control_type_name,
			       gchar *filename,
			       gchar *effect);
GList* ags_line_real_add_effect(AgsLine *line,
				GList *control_type_name,
				gchar *filename,
				gchar *effect);
void ags_line_real_remove_effect(AgsLine *line,
				 guint nth);
void ags_line_real_map_recall(AgsLine *line,
			      guint output_pad_start);
GList* ags_line_real_find_port(AgsLine *line);

/**
 * SECTION:ags_line
 * @short_description: A composite widget to visualize #AgsChannel
 * @title: AgsLine
 * @section_id:
 * @include: ags/X/ags_line.h
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
  ADD_EFFECT,
  REMOVE_EFFECT,
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
  PROP_PAD,
  PROP_CHANNEL,
};

static gpointer ags_line_parent_class = NULL;
static guint line_signals[LAST_SIGNAL];

GHashTable *ags_line_message_monitor = NULL;
GHashTable *ags_line_indicator_queue_draw = NULL;

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

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_line_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_line = g_type_register_static(GTK_TYPE_VBOX,
					   "AgsLine", &ags_line_info,
					   0);

    g_type_add_interface_static(ags_type_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_line,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

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
   * AgsLine:buffer-size:
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
   * AgsLine:format:
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
   * AgsLine:pad:
   *
   * The assigned #AgsPad.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("pad",
				   i18n_pspec("parent pad"),
				   i18n_pspec("The pad which is its parent"),
				   AGS_TYPE_PAD,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PAD,
				  param_spec);

  /**
   * AgsLine:channel:
   *
   * The assigned #AgsChannel to visualize.
   * 
   * Since: 2.0.0
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
  
  line->add_effect = ags_line_real_add_effect;
  line->remove_effect = ags_line_real_remove_effect;
  
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
   * Since: 2.1.35
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
   * Since: 2.1.35
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
   * Since: 2.1.35
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * AgsLine::add-effect:
   * @line: the #AgsLine to modify
   * @control_type_name: the control #GType string representation
   * @filename: the effect's filename
   * @effect: the effect's name
   *
   * The ::add-effect signal notifies about added effect.
   *
   * Returns: a #GList-struct containing new #AgsPort objects
   *
   * Since: 2.0.0
   */
  line_signals[ADD_EFFECT] =
    g_signal_new("add-effect",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, add_effect),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__POINTER_STRING_STRING,
		 G_TYPE_POINTER, 3,
		 G_TYPE_POINTER,
		 G_TYPE_STRING,
		 G_TYPE_STRING);

  /**
   * AgsLine::remove-effect:
   * @line: the #AgsLine to modify
   * @nth: the nth effect
   *
   * The ::remove-effect signal notifies about removed effect.
   *
   * Since: 2.0.0
   */
  line_signals[REMOVE_EFFECT] =
    g_signal_new("remove-effect",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, remove_effect),
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
ags_line_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_line_get_version;
  plugin->set_version = ags_line_set_version;
  plugin->get_build_id = ags_line_get_build_id;
  plugin->set_build_id = ags_line_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_line_init(AgsLine *line)
{
  if(ags_line_message_monitor == NULL){
    ags_line_message_monitor = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						     NULL,
						     NULL);
  }

  g_hash_table_insert(ags_line_message_monitor,
		      line, ags_line_message_monitor_timeout);
  
  g_timeout_add(1000 / 30,
		(GSourceFunc) ags_line_message_monitor_timeout,
		(gpointer) line);

  if(ags_line_indicator_queue_draw == NULL){
    ags_line_indicator_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							  NULL,
							  NULL);
  }
  
  line->flags = 0;

  line->version = AGS_VERSION;
  line->build_id = AGS_BUILD_ID;

  line->channel = NULL;

  //  gtk_widget_set_can_focus(line,
  //			   TRUE);

  line->pad = NULL;
  
  line->label = (GtkLabel *) gtk_label_new(NULL);
  gtk_box_pack_start(GTK_BOX(line),
		     GTK_WIDGET(line->label),
		     FALSE, FALSE,
		     0);

  line->group = (GtkToggleButton *) gtk_toggle_button_new_with_label(i18n("group"));
  gtk_toggle_button_set_active(line->group, TRUE);
  gtk_box_pack_start(GTK_BOX(line),
		     GTK_WIDGET(line->group),
		     FALSE, FALSE,
		     0);

  line->expander = ags_expander_new(1, 1);
  gtk_table_set_row_spacings(line->expander->table,
			     2);
  gtk_table_set_col_spacings(line->expander->table,
			     2);
  gtk_box_pack_start(GTK_BOX(line),
		     GTK_WIDGET(line->expander),
		     TRUE, TRUE,
		     0);
  
  line->indicator = NULL;

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
  case PROP_PAD:
    {
      GtkWidget *pad;

      pad = (GtkWidget *) g_value_get_object(value);

      if(line->pad == pad){
	return;
      }

      if(line->pad != NULL){
	g_object_unref(G_OBJECT(line->pad));
      }

      if(pad != NULL){
	g_object_ref(G_OBJECT(pad));
      }
      
      line->pad = pad;
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
  case PROP_PAD:
    {
      g_value_set_object(value,
			 line->pad);
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

  GList *list;
  
  line = AGS_LINE(gobject);

  /* remove message monitor */
  g_hash_table_remove(ags_line_message_monitor,
		      line);
  
  /* remove indicator widget */
  if(line->indicator != NULL){
    g_hash_table_remove(ags_line_indicator_queue_draw,
			line->indicator);
  }

  /* remove of the queued drawing hash */
  list = line->queued_drawing;

  while(list != NULL){
    g_hash_table_remove(ags_line_indicator_queue_draw,
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

  GList *list, *list_start;

  line = AGS_LINE(connectable);

  if((AGS_LINE_CONNECTED & (line->flags)) != 0){
    return;
  }

  /* set connected flag */
  line->flags |= AGS_LINE_CONNECTED;

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
  g_signal_connect_after((GObject *) line->group, "clicked",
			 G_CALLBACK(ags_line_group_clicked_callback), (gpointer) line);

  /* connect line members */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(line->expander->table));
  
  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_connect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_line_disconnect(AgsConnectable *connectable)
{
  AgsLine *line;

  GList *list, *list_start;

  line = AGS_LINE(connectable);

  if((AGS_LINE_CONNECTED & (line->flags)) == 0){
    return;
  }

  /* unset connected flag */
  line->flags &= (~AGS_LINE_CONNECTED);

#ifdef AGS_DEBUG
  g_message("line disconnect");
#endif

  /* disconnect group button */
  if(line->group != NULL &&
     GTK_IS_BUTTON(line->group)){
    g_object_disconnect(line->group,
			"any_signal::clicked",
			G_CALLBACK(ags_line_group_clicked_callback),
			(gpointer) line,
			NULL);
  }

  /* disconnect line members */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(line->expander->table));
  
  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  g_list_free(list_start);
}

gchar*
ags_line_get_version(AgsPlugin *plugin)
{
  return(AGS_LINE(plugin)->version);
}

void
ags_line_set_version(AgsPlugin *plugin, gchar *version)
{
  AGS_LINE(plugin)->version = version;
}

gchar*
ags_line_get_build_id(AgsPlugin *plugin)
{
  return(AGS_LINE(plugin)->build_id);
}

void
ags_line_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AGS_LINE(plugin)->build_id = build_id;
}

/**
 * ags_line_samplerate_changed:
 * @line: the #AgsLine
 * @samplerate: the samplerate
 * @old_samplerate: the old samplerate
 * 
 * Notify about samplerate changed.
 * 
 * Since: 2.1.35
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
 * Since: 2.1.35
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
 * Since: 2.1.35
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
 * Since: 2.0.0
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
 * Since: 2.0.0
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

GList*
ags_line_add_ladspa_effect(AgsLine *line,
			   GList *control_type_name,
			   gchar *filename,
			   gchar *effect)
{
  AgsLineMember *line_member;
  AgsEffectSeparator *separator;
  GtkAdjustment *adjustment;

  AgsLadspaPlugin *ladspa_plugin;

  GList *list;
  GList *start_recall, *recall;
  GList *play_port, *recall_port;
  GList *start_plugin_port, *plugin_port;

  gdouble step;
  guint port_count;

  guint x, y;
  guint k;
  
  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);

  play_port = NULL;
  recall_port = NULL;
    
  /* retrieve position within table  */
  x = 0;
  y = 0;

  list = line->expander->children;

  while(list != NULL){
    if(y <= AGS_EXPANDER_CHILD(list->data)->y){
      y = AGS_EXPANDER_CHILD(list->data)->y + 1;
    }

    list = list->next;
  }

  /* play - find ports */
  g_object_get(line->channel,
	       "play", &start_recall,
	       NULL);

  /* get by effect */
  recall = ags_recall_get_by_effect(start_recall,
				    filename,
				    effect);

  g_list_free(start_recall);
  
  start_recall = recall;
  
  if(recall == NULL){
    return(NULL);
  }

  recall = g_list_last(start_recall);
  g_object_get((GObject *) recall->data,
	       "port", &play_port,
	       NULL);
  
  /* check has output port */
  g_list_free(start_recall);
  
  /* recall - find ports */
  g_object_get(line->channel,
	       "recall", &start_recall,
	       NULL);

  /* get by effect */
  recall = start_recall;
  
  start_recall = ags_recall_get_by_effect(start_recall,
					  filename,
					  effect);

  g_list_free(recall);
  
  recall = start_recall;
  
  if(recall == NULL){
    return(NULL);
  }

  recall = g_list_last(start_recall);
  g_object_get((GObject *) recall->data,
	       "port", &recall_port,
	       NULL);

  g_list_free(start_recall);
  
  /* add separator */
  separator = ags_effect_separator_new();
  g_object_set(separator,
	       "text", effect,
	       "filename", filename,
	       "effect", effect,
	       NULL);
  ags_expander_add(line->expander,
  		   (GtkWidget *) separator,
  		   0, y,
  		   AGS_LINE_COLUMNS_COUNT, 1);
  gtk_widget_show_all(GTK_WIDGET(separator));

  y++;
  
  /* load ports */
  g_object_get(ladspa_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  plugin_port = start_plugin_port;
  
  port_count = g_list_length(start_plugin_port);
  
  k = 0;
  
  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
      GtkWidget *child_widget;

      AgsLadspaConversion *ladspa_conversion;

      GType widget_type;

      gchar *plugin_name;
      gchar *control_port;
      gchar *port_name;
      
      guint unique_id;
      guint step_count;
      gboolean disable_seemless;

      pthread_mutex_t *plugin_port_mutex;

      disable_seemless = FALSE;
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	disable_seemless = TRUE;

	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_HINDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }

      if(control_type_name != NULL){
	widget_type = g_type_from_name(control_type_name->data);

	control_type_name = control_type_name->next;
      }
      
      step_count = AGS_DIAL_DEFAULT_PRECISION;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	g_object_get(plugin_port->data,
		     "scale-steps", &step_count,
		     NULL);

	disable_seemless = TRUE;
      }

      /* get plugin port mutex */
      pthread_mutex_lock(ags_plugin_port_get_class_mutex());

      plugin_port_mutex = AGS_PLUGIN_PORT(plugin_port->data)->obj_mutex;
      
      pthread_mutex_unlock(ags_plugin_port_get_class_mutex());
      
      /* get port name */
      pthread_mutex_lock(plugin_port_mutex);

      port_name = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);
	
      pthread_mutex_unlock(plugin_port_mutex);
      
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
						   "widget-type", widget_type,
						   "widget-label", port_name,
						   "plugin-name", plugin_name,
						   "filename", filename,
						   "effect", effect,
						   "specifier", port_name,
						   "control-port", control_port,
						   "steps", step_count,
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
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}
	
	/* add controls of ports and apply range  */
	pthread_mutex_lock(plugin_port_mutex);

	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	pthread_mutex_unlock(plugin_port_mutex);

	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper_bound >= 0.0 && lower_bound >= 0.0){
	  step = (upper_bound - lower_bound) / step_count;
	}else if(upper_bound < 0.0 && lower_bound < 0.0){
	  step = -1.0 * (lower_bound - upper_bound) / step_count;
	}else{
	  step = (upper_bound - lower_bound) / step_count;
	}

	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_lower(adjustment,
				 lower_bound);
	gtk_adjustment_set_upper(adjustment,
				 upper_bound);

	/* get/set default value */
	pthread_mutex_lock(plugin_port_mutex);
	
	default_value = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	pthread_mutex_unlock(plugin_port_mutex);

	gtk_adjustment_set_value(adjustment,
				 default_value);
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_line_indicator_queue_draw,
			    child_widget, ags_line_indicator_queue_draw_timeout);
	line->queued_drawing = g_list_prepend(line->queued_drawing,
					      child_widget);
	g_timeout_add(1000 / 30,
		      (GSourceFunc) ags_line_indicator_queue_draw_timeout,
		      (gpointer) child_widget);
      }
      
#ifdef AGS_DEBUG
      g_message("ladspa bounds: %f %f", lower_bound, upper_bound);
#endif
	  
      ags_expander_add(line->expander,
		       (GtkWidget *) line_member,
		       x % AGS_LINE_COLUMNS_COUNT, y,
		       1, 1);

      ags_connectable_connect(AGS_CONNECTABLE(line_member));
      gtk_widget_show_all((GtkWidget *) line_member);
      
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

  g_list_free(start_plugin_port);

  return(g_list_concat(play_port,
		       recall_port));
}

GList*
ags_line_add_lv2_effect(AgsLine *line,
			GList *control_type_name,
			gchar *filename,
			gchar *effect)
{
  AgsLineMember *line_member;
  AgsEffectSeparator *separator;
  GtkAdjustment *adjustment;

  AgsLv2Plugin *lv2_plugin;

  GList *list;
  GList *start_recall, *recall;
  GList *play_port, *recall_port;
  GList *start_plugin_port, *plugin_port;

  gchar *uri;
  gchar *port_type_0, *port_type_1;
  gchar *plugin_name;
  gchar *control_port;

  gdouble step;
  guint port_count;

  guint x, y;
  guint k;
  
  pthread_mutex_t *base_plugin_mutex;

  /* load plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = AGS_BASE_PLUGIN(lv2_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  play_port = NULL;
  recall_port = NULL;
  
  /* retrieve position within table  */
  x = 0;
  y = 0;

  list = line->expander->children;

  while(list != NULL){
    if(y <= AGS_EXPANDER_CHILD(list->data)->y){
      y = AGS_EXPANDER_CHILD(list->data)->y + 1;
    }

    list = list->next;
  }

  /* play - find ports */
  g_object_get(line->channel,
	       "play", &start_recall,
	       NULL);

  /* get by effect */
  recall = ags_recall_get_by_effect(start_recall,
				    filename,
				    effect);

  g_list_free(start_recall);
  
  start_recall = recall;
  
  if(recall == NULL){
    return(NULL);
  }

  recall = g_list_last(start_recall);
  g_object_get((GObject *) recall->data,
	       "port", &play_port,
	       NULL);
  
  g_list_free(start_recall);
  
  /* recall - find ports */
  g_object_get(line->channel,
	       "recall", &start_recall,
	       NULL);

  /* get by effect */
  recall = start_recall;
  
  start_recall = ags_recall_get_by_effect(start_recall,
					  filename,
					  effect);

  g_list_free(recall);
  
  recall = start_recall;
  
  if(recall == NULL){
    return(NULL);
  }

  recall = g_list_last(start_recall);
  g_object_get((GObject *) recall->data,
	       "port", &recall_port,
	       NULL);

  g_list_free(start_recall);
  
  /* add separator */
  separator = ags_effect_separator_new();
  g_object_set(separator,
	       "text", effect,
	       "filename", filename,
	       "effect", effect,
	       NULL);
  ags_expander_add(line->expander,
  		   (GtkWidget *) separator,
  		   0, y,
  		   AGS_LINE_COLUMNS_COUNT, 1);
  gtk_widget_show_all(GTK_WIDGET(separator));
  
  y++;
  
  /* get uri */
  pthread_mutex_lock(base_plugin_mutex);

  uri = g_strdup(lv2_plugin->uri);
	
  pthread_mutex_unlock(base_plugin_mutex);
  
  /* load ports */
  g_object_get(lv2_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  plugin_port = start_plugin_port;
  
  port_count = g_list_length(start_plugin_port);
  
  k = 0;
  
  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
      GtkWidget *child_widget;

      AgsLv2Conversion *lv2_conversion;
      
      GType widget_type;

      gchar *plugin_name;
      gchar *control_port;
      gchar *port_name;

      guint step_count;
      gboolean disable_seemless;

      pthread_mutex_t *plugin_port_mutex;

      disable_seemless = FALSE;
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	disable_seemless = TRUE;

	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_HINDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }

      if(control_type_name != NULL){
	widget_type = g_type_from_name(control_type_name->data);

	control_type_name = control_type_name->next;
      }
      
      step_count = AGS_DIAL_DEFAULT_PRECISION;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	g_object_get(plugin_port->data,
		     "scale-steps", &step_count,
		     NULL);

	disable_seemless = TRUE;
      }

      /* get plugin port mutex */
      pthread_mutex_lock(ags_plugin_port_get_class_mutex());

      plugin_port_mutex = AGS_PLUGIN_PORT(plugin_port->data)->obj_mutex;
      
      pthread_mutex_unlock(ags_plugin_port_get_class_mutex());
      
      /* get port name */
      pthread_mutex_lock(plugin_port_mutex);

      port_name = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);
	
      pthread_mutex_unlock(plugin_port_mutex);

      /* add line member */
      plugin_name = g_strdup_printf("lv2-<%s>",
				    uri);

      control_port = g_strdup_printf("%d/%d",
				     k + 1,
				     port_count);

      line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
						   "widget-type", widget_type,
						   "widget-label", port_name,
						   "plugin-name", plugin_name,
						   "filename", filename,
						   "effect", effect,
						   "specifier", port_name,
						   "control-port", control_port,
						   "steps", step_count,
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
	float default_value;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}
	
	/* add controls of ports and apply range  */
	pthread_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	pthread_mutex_unlock(plugin_port_mutex);

	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper_bound >= 0.0 && lower_bound >= 0.0){
	  step = (upper_bound - lower_bound) / step_count;
	}else if(upper_bound < 0.0 && lower_bound < 0.0){
	  step = -1.0 * (lower_bound - upper_bound) / step_count;
	}else{
	  step = (upper_bound - lower_bound) / step_count;
	}

	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_lower(adjustment,
				 lower_bound);
	gtk_adjustment_set_upper(adjustment,
				 upper_bound);

	/* get/set default value */
	pthread_mutex_lock(plugin_port_mutex);
	
	default_value = (float) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	pthread_mutex_unlock(plugin_port_mutex);

	gtk_adjustment_set_value(adjustment,
				 default_value);
      }else if(AGS_IS_INDICATOR(child_widget)){
	g_hash_table_insert(ags_line_indicator_queue_draw,
			    child_widget,
			    ags_line_indicator_queue_draw_timeout);

	line->queued_drawing = g_list_prepend(line->queued_drawing,
					      child_widget);
	g_timeout_add(1000 / 30,
		      (GSourceFunc) ags_line_indicator_queue_draw_timeout,
		      (gpointer) child_widget);
      }

#ifdef AGS_DEBUG
      g_message("lv2 bounds: %f %f", lower_bound, upper_bound);
#endif

      ags_expander_add(line->expander,
		       (GtkWidget *) line_member,
		       x % AGS_LINE_COLUMNS_COUNT, y,
		       1, 1);      
      ags_connectable_connect(AGS_CONNECTABLE(line_member));
      gtk_widget_show_all((GtkWidget *) line_member);

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

  g_free(uri);
  
  return(g_list_concat(play_port,
		       recall_port));
}

GList*
ags_line_real_add_effect(AgsLine *line,
			 GList *control_type_name,
			 gchar *filename,
			 gchar *effect)
{
  AgsWindow *window;

  AgsLadspaPlugin *ladspa_plugin;
  AgsLv2Plugin *lv2_plugin;
  
  GList *port;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) line);
  
  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);
  port = NULL;
  
  if(ladspa_plugin != NULL){
    port = ags_line_add_ladspa_effect(line,
				      control_type_name,
				      filename,
				      effect);
  }else{
    lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
						 filename, effect);
    
    if(lv2_plugin != NULL){
      port = ags_line_add_lv2_effect(line,
				     control_type_name,
				     filename,
				     effect);
    }
  }

  /*  */
  ags_automation_toolbar_load_port(window->automation_window->automation_editor->automation_toolbar);

  return(port);
}

/**
 * ags_line_add_effect:
 * @line: the #AgsLine
 * @control_type_name: the control #GType string representation
 * @filename: the filename of the plugin
 * @effect: the effect's name
 *
 * Add a line member.
 *
 * Returns: a #GList-struct containing new #AgsPort objects
 *
 * Since: 2.0.0 
 */
GList*
ags_line_add_effect(AgsLine *line,
		    GList *control_type_name,
		    gchar *filename,
		    gchar *effect)
{
  GList *port;
  
  g_return_val_if_fail(AGS_IS_LINE(line), NULL);

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[ADD_EFFECT], 0,
		control_type_name,
		filename,
		effect,
		&port);
  g_object_unref((GObject *) line);
  
  return(port);
}

void
ags_line_real_remove_effect(AgsLine *line,
			    guint nth)
{
  AgsWindow *window;
  
  GList *control, *control_start;
  GList *start_recall, *recall;
  GList *start_port, *port;

  gchar *filename, *effect;
  
  guint nth_effect, n_bulk;
  guint i;
  
  pthread_mutex_t *recall_mutex;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) line);
    
  /* get nth_effect */
  g_object_get(line->channel,
	       "play", &start_recall,
	       NULL);
  
  recall = start_recall;
  
  nth_effect = 0;
  n_bulk = 0;
  
  while((recall = ags_recall_template_find_all_type(recall,
						    AGS_TYPE_RECALL_LADSPA,
						    AGS_TYPE_RECALL_LV2,
						    G_TYPE_NONE)) != NULL){
    if(ags_recall_test_flags(recall->data, AGS_RECALL_TEMPLATE)){
      nth_effect++;
    }

    if(ags_recall_test_behaviour_flags(recall->data, AGS_SOUND_BEHAVIOUR_BULK_MODE)){
      n_bulk++;
    }

    if(nth_effect - n_bulk == nth + 1){
      break;
    }
    
    recall = recall->next;
  }

  if(recall == NULL){
    g_list_free(start_recall);
    
    return;
  }
  
  nth_effect--;

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = AGS_RECALL(recall->data)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());
  
  /* get filename and effect */
  pthread_mutex_lock(recall_mutex);

  filename = g_strdup(AGS_RECALL(recall->data)->filename);
  effect = g_strdup(AGS_RECALL(recall->data)->effect);

  pthread_mutex_unlock(recall_mutex);

  /* destroy separator */
  control_start =
    control = gtk_container_get_children((GtkContainer *) line->expander->table);

  while(control != NULL){
    gchar *separator_filename;
    gchar *separator_effect;
    
    if(AGS_IS_EFFECT_SEPARATOR(control->data)){
      g_object_get(control->data,
		   "filename", &separator_filename,
		   "effect", &separator_effect,
		   NULL);
      
      if(separator_filename != NULL &&
	 separator_effect != NULL &&
	 !g_strcmp0(filename,
		    separator_filename) &&
	 !g_strcmp0(effect,
		    separator_effect)){
	gtk_widget_destroy(control->data);
      
	break;
      }
    }
    
    control = control->next;
  }
  
  g_list_free(control_start);
  
  /* destroy controls */
  g_object_get(recall->data,
	       "port", &start_port,
	       NULL);
  
  port = start_port;
  i = 0;
  
  while(port != NULL){
    control_start =
      control = gtk_container_get_children((GtkContainer *) line->expander->table);
      
    while(control != NULL){
      if(AGS_IS_LINE_MEMBER(control->data) &&
	 AGS_LINE_MEMBER(control->data)->port == port->data){
	GtkWidget *child_widget;
	
	child_widget = gtk_bin_get_child(control->data);

	/* collect specifier */
	i++;

	/* remove widget */
	if(AGS_IS_LED(child_widget) ||
	   AGS_IS_INDICATOR(child_widget)){
	  g_hash_table_remove(ags_line_indicator_queue_draw,
			      child_widget);
	}
	
	ags_expander_remove(line->expander,
			    control->data);

	break;
      }
	
      /* iterate */
      control = control->next;
    }

    g_list_free(control_start);
    
    /* iterate */
    port = port->next;
  }

  g_list_free(start_recall);
  g_list_free(start_port);
  
  /* remove recalls */
  ags_channel_remove_effect(line->channel,
			    nth_effect);

  /* reset automation editor */
  ags_automation_toolbar_load_port(window->automation_window->automation_editor->automation_toolbar);
}

/**
 * ags_line_remove_effect:
 * @line: the #AgsLine
 * @nth: nth effect to remove
 *
 * Remove a line member.
 *
 * Since: 2.0.0
 */
void
ags_line_remove_effect(AgsLine *line,
		       guint nth)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[REMOVE_EFFECT], 0,
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
 * Since: 2.0.0
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
  GList *line_member, *line_member_start;

  if(line == NULL || line->expander == NULL){
    return(NULL);
  }

  line_member_start = 
    line_member = gtk_container_get_children(GTK_CONTAINER(line->expander->table));
  
  port = NULL;

  if(line_member != NULL){
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

    g_list_free(line_member_start);
  }  
  
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
 * Since: 2.0.0
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
 * Since: 2.0.0
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
 * Since: 2.0.0
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
 * ags_line_message_monitor_timeout:
 * @line: the #AgsLine
 *
 * Monitor messages.
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_line_message_monitor_timeout(AgsLine *line)
{
  if(g_hash_table_lookup(ags_line_message_monitor,
			 line) != NULL){
    AgsChannel *channel;
    
    AgsMessageDelivery *message_delivery;

    GList *message_start, *message;
    
    /* retrieve message */
    message_delivery = ags_message_delivery_get_instance();

    channel = line->channel;
	  
    message_start = 
      message = ags_message_delivery_find_sender(message_delivery,
						 "libags-audio",
						 (GObject *) channel);
    
    while(message != NULL){
      xmlNode *root_node;

      root_node = xmlDocGetRootElement(AGS_MESSAGE_ENVELOPE(message->data)->doc);
      
      if(!xmlStrncmp(root_node->name,
		     "ags-command",
		     12)){
	if(!xmlStrncmp(xmlGetProp(root_node,
				  "method"),
		       "AgsChannel::set-samplerate",
		       27)){
	  guint samplerate;
	  gint position;
	  
	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				    "samplerate");
	  samplerate = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));

	  /* set samplerate */
	  g_object_set(line,
		       "samplerate", samplerate,
		       NULL);
	}else if(!xmlStrncmp(xmlGetProp(root_node,
				  "method"),
		       "AgsChannel::set-buffer-size",
		       28)){
	  guint buffer_size;
	  gint position;
	  
	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				    "buffer-size");
	  buffer_size = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));

	  /* set buffer size */
	  g_object_set(line,
		       "buffer-size", buffer_size,
		       NULL);
	}else if(!xmlStrncmp(xmlGetProp(root_node,
				  "method"),
		       "AgsChannel::set-format",
		       23)){
	  guint format;
	  gint position;
	  
	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				    "format");
	  format = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));

	  /* set format */
	  g_object_set(line,
		       "format", format,
		       NULL);
	}else if(!xmlStrncmp(xmlGetProp(root_node,
				  "method"),
		       "AgsChannel::add-effect",
		       22)){
	  AgsMachine *machine;
	  AgsMachineEditor *machine_editor;
	  AgsLineMemberEditor *line_member_editor;
	  AgsPluginBrowser *plugin_browser;
	  
	  GList *pad_editor, *pad_editor_start;
	  GList *line_editor, *line_editor_start;
	  GList *control_type_name;
	  
	  gchar *filename, *effect;

	  gint position;
	  
	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				    "filename");
	  filename = g_value_get_string(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));

	  position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message->data)->parameter_name,
				    "effect");
	  effect = g_value_get_string(&(AGS_MESSAGE_ENVELOPE(message->data)->value[position]));

	  /* get machine and machine editor */
	  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) line,
							   AGS_TYPE_MACHINE);
	  machine_editor = (AgsMachineEditor *) machine->properties;

	  /* get control type */
	  control_type_name = NULL;  

	  pad_editor_start = NULL;
	  line_editor_start = NULL;
  
	  if(machine_editor != NULL){
	    pad_editor_start = 
	      pad_editor = gtk_container_get_children((GtkContainer *) machine_editor->input_editor->child);
	    pad_editor = g_list_nth(pad_editor,
				    channel->pad);
    
	    if(pad_editor != NULL){
	      line_editor_start =
		line_editor = gtk_container_get_children((GtkContainer *) AGS_PAD_EDITOR(pad_editor->data)->line_editor);
	      line_editor = g_list_nth(line_editor,
				       channel->audio_channel);
	    }else{
	      line_editor = NULL;
	    }

	    if(line_editor != NULL){
	      line_member_editor = AGS_LINE_EDITOR(line_editor->data)->member_editor;

	      plugin_browser = line_member_editor->plugin_browser;

	      if(plugin_browser != NULL &&
		 plugin_browser->active_browser != NULL){
		GList *description, *description_start;
		GList *port_control, *port_control_start;

		gchar *controls;

		/* get plugin browser */
		description = 
		  description_start = NULL;
		port_control_start = NULL;
		
		if(AGS_IS_LADSPA_BROWSER(plugin_browser->active_browser)){
		  description_start = 
		    description = gtk_container_get_children((GtkContainer *) AGS_LADSPA_BROWSER(plugin_browser->active_browser)->description);
		}else if(AGS_IS_DSSI_BROWSER(plugin_browser->active_browser)){
		  description_start = 
		    description = gtk_container_get_children((GtkContainer *) AGS_DSSI_BROWSER(plugin_browser->active_browser)->description);
		}else if(AGS_IS_LV2_BROWSER(plugin_browser->active_browser)){
		  description_start = 
		    description = gtk_container_get_children((GtkContainer *) AGS_LV2_BROWSER(plugin_browser->active_browser)->description);
		}else{
		  g_message("ags_line_callbacks.c unsupported plugin browser");
		}

		/* get port description */
		if(description != NULL){
		  description = g_list_last(description);
	  
		  port_control_start =
		    port_control = gtk_container_get_children(GTK_CONTAINER(description->data));
	  
		  if(port_control != NULL){
		    while(port_control != NULL){
		      controls = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(port_control->data));

		      if(!g_ascii_strncasecmp(controls,
					      "led",
					      4)){
			control_type_name = g_list_prepend(control_type_name,
							   "AgsLed");
		      }else if(!g_ascii_strncasecmp(controls,
						    "vertical indicator",
						    19)){
			control_type_name = g_list_prepend(control_type_name,
							   "AgsVIndicator");
		      }else if(!g_ascii_strncasecmp(controls,
						    "horizontal indicator",
						    19)){
			control_type_name = g_list_prepend(control_type_name,
							   "AgsHIndicator");
		      }else if(!g_ascii_strncasecmp(controls,
						    "spin button",
						    12)){
			control_type_name = g_list_prepend(control_type_name,
							   "GtkSpinButton");
		      }else if(!g_ascii_strncasecmp(controls,
						    "dial",
						    5)){
			control_type_name = g_list_prepend(control_type_name,
							   "AgsDial");
		      }else if(!g_ascii_strncasecmp(controls,
						    "vertical scale",
						    15)){
			control_type_name = g_list_prepend(control_type_name,
							   "GtkVScale");
		      }else if(!g_ascii_strncasecmp(controls,
						    "horizontal scale",
						    17)){
			control_type_name = g_list_prepend(control_type_name,
							   "GtkHScale");
		      }else if(!g_ascii_strncasecmp(controls,
						    "check-button",
						    13)){
			control_type_name = g_list_prepend(control_type_name,
							   "GtkCheckButton");
		      }else if(!g_ascii_strncasecmp(controls,
						    "toggle button",
						    14)){
			control_type_name = g_list_prepend(control_type_name,
							   "GtkToggleButton");
		      }
	      
		      port_control = port_control->next;
		      port_control = port_control->next;
		    }
		  }

		  /* free lists */
		  g_list_free(description_start);
		  g_list_free(port_control_start);
		}
	      }
      
	      //      line_member_editor->plugin_browser;
	    }
	  }else{
	    control_type_name = NULL;
	  }
	  
	  /* free lists */
	  g_list_free(pad_editor_start);
	  g_list_free(line_editor_start);
	  
	  /* add effect */
	  ags_line_add_effect(line,
			      control_type_name,
			      filename,
			      effect);
	}else if(!xmlStrncmp(xmlGetProp(root_node,
					"method"),
			     "AgsChannel::stop",
			     16)){
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
	  ags_line_stop(line,
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
 * ags_line_indicator_queue_draw_timeout:
 * @widget: the widget
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_line_indicator_queue_draw_timeout(GtkWidget *widget)
{
  AgsLine *line;

  if(g_hash_table_lookup(ags_line_indicator_queue_draw,
			 widget) != NULL){      
    GList *list, *list_start;
    
    line = (AgsLine *) gtk_widget_get_ancestor(widget,
					       AGS_TYPE_LINE);
  
    list_start = 
      list = gtk_container_get_children((GtkContainer *) AGS_LINE(line)->expander->table);
    
    /* check members */
    while(list != NULL){
      if(AGS_IS_LINE_MEMBER(list->data) &&
	 (AGS_LINE_MEMBER(list->data)->widget_type == AGS_TYPE_VINDICATOR ||
	  AGS_LINE_MEMBER(list->data)->widget_type == AGS_TYPE_HINDICATOR ||
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

	pthread_mutex_t *port_mutex;
	pthread_mutex_t *plugin_port_mutex;
	
	line_member = AGS_LINE_MEMBER(list->data);
	child = GTK_BIN(line_member)->child;
      
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

	g_object_get(current,
		     "plugin-port", &plugin_port,
		     NULL);

	if(plugin_port == NULL){
	  list = list->next;

	  continue;
	}

	/* get port mutex */
	pthread_mutex_lock(ags_port_get_class_mutex());
	
	port_mutex = current->obj_mutex;
	
	pthread_mutex_unlock(ags_port_get_class_mutex());

	/* match specifier */
	pthread_mutex_lock(port_mutex);

	success = (!g_ascii_strcasecmp(current->specifier,
				       line_member->specifier)) ? TRUE: FALSE;
	
	pthread_mutex_unlock(port_mutex);

	if(!success){
	  list = list->next;
	
	  continue;
	}

	/* get plugin port mutex */
	pthread_mutex_lock(ags_plugin_port_get_class_mutex());
	
	plugin_port_mutex = plugin_port->obj_mutex;
	
	pthread_mutex_unlock(ags_plugin_port_get_class_mutex());
	
	/* lower and upper */
	pthread_mutex_lock(plugin_port_mutex);
	
	lower = g_value_get_float(plugin_port->lower_value);
	upper = g_value_get_float(plugin_port->upper_value);
      
	pthread_mutex_unlock(plugin_port_mutex);
	
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
      
	peak = g_value_get_float(&value);
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
	    average_peak = 10.0;
	  }
	}else{
	  average_peak += ((1.0 / (range / peak)) * 10.0);
	}

	/* recall port */
	current = line_member->recall_port;

	/* recall port - read value */
	g_value_init(&value, G_TYPE_FLOAT);
	ags_port_safe_read(current,
			   &value);
      
	peak = g_value_get_float(&value);
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
	    average_peak = 10.0;
	  }
	}else{
	  average_peak += ((1.0 / (range / peak)) * 10.0);
	}
      
	/* apply */
	if(AGS_IS_LED(child)){
	  if(average_peak != 0.0){
	    ags_led_set_active(AGS_LED(child));
	  }
	}else{
	  g_object_get(child,
		       "adjustment", &adjustment,
		       NULL);
	
	  gtk_adjustment_set_value(adjustment,
				   average_peak);
	}
      }
    
      list = list->next;
    }

    g_list_free(list_start);

    /* queue draw */
    gtk_widget_queue_draw(widget);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_line_new:
 * @pad: the parent pad
 * @channel: the #AgsChannel to visualize
 *
 * Create a new instance of #AgsLine
 *
 * Returns: the new #AgsLine
 *
 * Since: 2.0.0
 */
AgsLine*
ags_line_new(GtkWidget *pad, AgsChannel *channel)
{
  AgsLine *line;

  line = (AgsLine *) g_object_new(AGS_TYPE_LINE,
				  "pad", pad,
				  "channel", channel,
				  NULL);

  return(line);
}
