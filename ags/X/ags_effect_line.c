/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/ags_effect_line.h>
#include <ags/X/ags_effect_line_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_effect_pad.h>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

#include <ags/i18n.h>

void ags_effect_line_class_init(AgsEffectLineClass *effect_line);
void ags_effect_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_effect_line_plugin_interface_init(AgsPluginInterface *plugin);
void ags_effect_line_init(AgsEffectLine *effect_line);
void ags_effect_line_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_effect_line_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_effect_line_dispose(GObject *gobject);
void ags_effect_line_finalize(GObject *gobject);

void ags_effect_line_connect(AgsConnectable *connectable);
void ags_effect_line_disconnect(AgsConnectable *connectable);

gchar* ags_effect_line_get_name(AgsPlugin *plugin);
void ags_effect_line_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_effect_line_get_version(AgsPlugin *plugin);
void ags_effect_line_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_effect_line_get_build_id(AgsPlugin *plugin);
void ags_effect_line_set_build_id(AgsPlugin *plugin, gchar *build_id);

void ags_effect_line_real_set_channel(AgsEffectLine *effect_line, AgsChannel *channel);
GList* ags_effect_line_add_ladspa_effect(AgsEffectLine *effect_line,
					 GList *control_type_name,
					 gchar *filename,
					 gchar *effect);
GList* ags_effect_line_add_lv2_effect(AgsEffectLine *effect_line,
				      GList *control_type_name,
				      gchar *filename,
				      gchar *effect);
GList* ags_effect_line_real_add_effect(AgsEffectLine *effect_line,
				       GList *control_type_name,
				       gchar *filename,
				       gchar *effect);
void ags_effect_line_real_remove_effect(AgsEffectLine *effect_line,
					guint nth);
void ags_effect_line_real_map_recall(AgsEffectLine *effect_line,
				     guint output_pad_start);
GList* ags_effect_line_real_find_port(AgsEffectLine *effect_line);

/**
 * SECTION:ags_effect_line
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsEffectLine
 * @section_id:
 * @include: ags/X/ags_effect_line.h
 *
 * #AgsEffectLine is a composite widget to visualize one #AgsChannel. It should be
 * packed by an #AgsEffectLine.
 */

enum{
  SAMPLERATE_CHANGED,
  BUFFER_SIZE_CHANGED,
  FORMAT_CHANGED,
  SET_CHANNEL,
  ADD_EFFECT,
  REMOVE_EFFECT,
  MAP_RECALL,
  FIND_PORT,
  DONE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_CHANNEL,
};

static gpointer ags_effect_line_parent_class = NULL;
static guint effect_line_signals[LAST_SIGNAL];

GHashTable *ags_effect_line_indicator_queue_draw = NULL;

GType
ags_effect_line_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_effect_line = 0;

    static const GTypeInfo ags_effect_line_info = {
      sizeof(AgsEffectLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_effect_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEffectLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_effect_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_effect_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_effect_line_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_effect_line = g_type_register_static(GTK_TYPE_VBOX,
						  "AgsEffectLine", &ags_effect_line_info,
						  0);

    g_type_add_interface_static(ags_type_effect_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_effect_line,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_effect_line);
  }

  return g_define_type_id__volatile;
}

void
ags_effect_line_class_init(AgsEffectLineClass *effect_line)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_effect_line_parent_class = g_type_class_peek_parent(effect_line);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(effect_line);

  gobject->set_property = ags_effect_line_set_property;
  gobject->get_property = ags_effect_line_get_property;

  gobject->dispose = ags_effect_line_dispose;
  gobject->finalize = ags_effect_line_finalize;
  
  /* properties */
  /**
   * AgsEffectLine:samplerate:
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
   * AgsEffectLine:buffer-size:
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
   * AgsEffectLine:format:
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
   * AgsEffectLine:channel:
   *
   * The start of a bunch of #AgsChannel to visualize.
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

  /* AgsEffectLineClass */
  effect_line->samplerate_changed = NULL;
  effect_line->buffer_size_changed = NULL;
  effect_line->format_changed = NULL;

  effect_line->set_channel = ags_effect_line_real_set_channel;

  effect_line->add_effect = ags_effect_line_real_add_effect;
  effect_line->remove_effect = ags_effect_line_real_remove_effect;

  effect_line->map_recall = ags_effect_line_real_map_recall;
  effect_line->find_port = ags_effect_line_real_find_port;

  effect_line->done = NULL;

  /* signals */
  /**
   * AgsEffectLine::samplerate-changed:
   * @effect_line: the #AgsEffectLine
   * @samplerate: the samplerate
   * @old_samplerate: the old samplerate
   *
   * The ::samplerate-changed signal notifies about changed samplerate.
   * 
   * Since: 3.0.0
   */
  effect_line_signals[SAMPLERATE_CHANGED] =
    g_signal_new("samplerate-changed",
		 G_TYPE_FROM_CLASS(effect_line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectLineClass, samplerate_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsEffectLine::buffer-size-changed:
   * @effect_line: the #AgsEffectLine
   * @buffer_size: the buffer size
   * @old_buffer_size: the old buffer size
   *
   * The ::buffer-size-changed signal notifies about changed buffer size.
   * 
   * Since: 3.0.0
   */
  effect_line_signals[BUFFER_SIZE_CHANGED] =
    g_signal_new("buffer-size-changed",
		 G_TYPE_FROM_CLASS(effect_line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectLineClass, buffer_size_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsEffectLine::format-changed:
   * @effect_line: the #AgsEffectLine
   * @format: the format
   * @old_format: the old format
   *
   * The ::format-changed signal notifies about changed format.
   * 
   * Since: 3.0.0
   */
  effect_line_signals[FORMAT_CHANGED] =
    g_signal_new("format-changed",
		 G_TYPE_FROM_CLASS(effect_line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectLineClass, format_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsEffectLine::set-channel:
   * @effect_line: the #AgsEffectLine to modify
   * @channel: the #AgsChannel to set
   *
   * The ::set-channel signal notifies about changed channel.
   *
   * Since: 3.0.0
   */
  effect_line_signals[SET_CHANNEL] =
    g_signal_new("set-channel",
		 G_TYPE_FROM_CLASS(effect_line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectLineClass, set_channel),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsEffectLine::add-effect:
   * @effect_line: the #AgsEffectLine to modify
   * @control_type_name: the string representation of a #GType
   * @filename: the effect's filename
   * @effect: the effect's name
   *
   * The ::add-effect signal notifies about added effect.
   *
   * Returns: the #GList-struct containing the #AgsPort objects added
   *
   * Since: 3.0.0
   */
  effect_line_signals[ADD_EFFECT] =
    g_signal_new("add-effect",
		 G_TYPE_FROM_CLASS(effect_line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectLineClass, add_effect),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__POINTER_STRING_STRING,
		 G_TYPE_POINTER, 3,
		 G_TYPE_POINTER,
		 G_TYPE_STRING,
		 G_TYPE_STRING);

  /**
   * AgsEffectLine::remove-effect:
   * @effect_line: the #AgsEffectLine to modify
   * @nth: the nth effect
   *
   * The ::remove-effect signal notifies about removed effect.
   *
   * Since: 3.0.0
   */
  effect_line_signals[REMOVE_EFFECT] =
    g_signal_new("remove-effect",
		 G_TYPE_FROM_CLASS(effect_line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectLineClass, remove_effect),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsEffectLine::map-recall:
   * @effect_line: the #AgsEffectLine
   * @output_pad_start: the channel's start pad
   *
   * The ::map-recall should be used to add the effect_line's default recall. This function
   * may call ags_effect_line_find_port().
   *
   * Since: 3.0.0
   */
  effect_line_signals[MAP_RECALL] =
    g_signal_new("map-recall",
                 G_TYPE_FROM_CLASS (effect_line),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsEffectLineClass, map_recall),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__UINT,
                 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsEffectLine::find-port:
   * @effect_line: the #AgsEffectLine to resize
   *
   * The ::find-port as recall should be mapped
   *
   * Returns: an #GList-struct containing all related #AgsPort
   *
   * Since: 3.0.0
   */
  effect_line_signals[FIND_PORT] =
    g_signal_new("find-port",
		 G_TYPE_FROM_CLASS(effect_line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectLineClass, find_port),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsEffectLine::done:
   * @effect_line: the #AgsEffectLine
   * @recall_id: the #AgsRecallID
   *
   * The ::done signal gets emited as audio stops playback.
   * 
   * Since: 3.0.0
   */
  effect_line_signals[DONE] =
    g_signal_new("done",
                 G_TYPE_FROM_CLASS(effect_line),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectLineClass, done),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_effect_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_effect_line_connect;
  connectable->disconnect = ags_effect_line_disconnect;
}

void
ags_effect_line_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_effect_line_get_version;
  plugin->set_version = ags_effect_line_set_version;
  plugin->get_build_id = ags_effect_line_get_build_id;
  plugin->set_build_id = ags_effect_line_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_effect_line_init(AgsEffectLine *effect_line)
{
  AgsApplicationContext *application_context;
  AgsConfig *config;

  application_context = ags_application_context_get_instance();

  g_signal_connect(application_context, "check-message",
		   G_CALLBACK(ags_effect_line_check_message_callback), effect_line);
  
  if(ags_effect_line_indicator_queue_draw == NULL){
    ags_effect_line_indicator_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
								 NULL,
								 NULL);
  }
  
  effect_line->flags = 0;

  effect_line->name = NULL;
  
  effect_line->version = AGS_EFFECT_LINE_DEFAULT_VERSION;
  effect_line->build_id = AGS_EFFECT_LINE_DEFAULT_BUILD_ID;

  config = ags_config_get_instance();
  
  effect_line->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  effect_line->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  effect_line->format = ags_soundcard_helper_config_get_format(config);

  effect_line->channel = NULL;

  effect_line->label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
						 NULL);
  gtk_box_pack_start(GTK_BOX(effect_line),
		     GTK_WIDGET(effect_line->label),
		     FALSE, FALSE,
		     0);

  effect_line->group = (GtkToggleButton *) gtk_toggle_button_new_with_label(i18n("group"));
  gtk_box_pack_start(GTK_BOX(effect_line),
		     GTK_WIDGET(effect_line->group),
		     FALSE, FALSE,
		     0);

  effect_line->table = (GtkTable *) gtk_table_new(1, AGS_EFFECT_LINE_COLUMNS_COUNT,
						  TRUE);
  gtk_box_pack_start(GTK_BOX(effect_line),
		     GTK_WIDGET(effect_line->table),
		     FALSE, FALSE,
		     0);

  effect_line->queued_drawing = NULL;
}

void
ags_effect_line_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsEffectLine *effect_line;

  effect_line = AGS_EFFECT_LINE(gobject);

  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      guint samplerate, old_samplerate;
      
      samplerate = g_value_get_uint(value);
      old_samplerate = effect_line->samplerate;

      if(samplerate == old_samplerate){
	return;
      }

      effect_line->samplerate = samplerate;

      ags_effect_line_samplerate_changed(effect_line,
					 samplerate, old_samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size, old_buffer_size;
      
      buffer_size = g_value_get_uint(value);
      old_buffer_size = effect_line->buffer_size;

      if(buffer_size == old_buffer_size){
	return;
      }

      effect_line->buffer_size = buffer_size;

      ags_effect_line_buffer_size_changed(effect_line,
					  buffer_size, old_buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      guint format, old_format;
      
      format = g_value_get_uint(value);
      old_format = effect_line->format;

      if(format == old_format){
	return;
      }

      effect_line->format = format;

      ags_effect_line_format_changed(effect_line,
				     format, old_format);
    }
    break;
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      ags_effect_line_set_channel(effect_line, channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_line_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsEffectLine *effect_line;

  effect_line = AGS_EFFECT_LINE(gobject);

  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value,
		       effect_line->samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value,
		       effect_line->buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value,
		       effect_line->format);
    }
    break;
  case PROP_CHANNEL:
    {
      g_value_set_object(value,
			 effect_line->channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_line_dispose(GObject *gobject)
{
  AgsEffectLine *effect_line;
  GList *list;

  effect_line = AGS_EFFECT_LINE(gobject);

  if(effect_line->channel != NULL){
    g_object_unref(effect_line->channel);

    effect_line->channel = NULL;
  }
  
  /* call parent */  
  G_OBJECT_CLASS(ags_effect_line_parent_class)->dispose(gobject);
}

void
ags_effect_line_finalize(GObject *gobject)
{
  AgsEffectLine *effect_line;

  AgsApplicationContext *application_context;

  GList *list;

  effect_line = AGS_EFFECT_LINE(gobject);

  application_context = ags_application_context_get_instance();

  g_object_disconnect(application_context,
		      "any_signal::check-message",
		      G_CALLBACK(ags_effect_line_check_message_callback),
		      effect_line,
		      NULL);

  /* remove of the queued drawing hash */
  list = effect_line->queued_drawing;

  while(list != NULL){
    g_hash_table_remove(ags_effect_line_indicator_queue_draw,
			list->data);

    list = list->next;
  }

  /* channel */
  if(effect_line->channel != NULL){
    g_object_unref(effect_line->channel);
  }
  
  /* call parent */  
  G_OBJECT_CLASS(ags_effect_line_parent_class)->finalize(gobject);
}

void
ags_effect_line_connect(AgsConnectable *connectable)
{
  AgsEffectLine *effect_line;

  GList *list, *list_start;

  effect_line = AGS_EFFECT_LINE(connectable);

  if((AGS_EFFECT_LINE_CONNECTED & (effect_line->flags)) == 0){
    return;
  }

  effect_line->flags &= (~AGS_EFFECT_LINE_CONNECTED);

  if((AGS_EFFECT_LINE_PREMAPPED_RECALL & (effect_line->flags)) == 0){
    if((AGS_EFFECT_LINE_MAPPED_RECALL & (effect_line->flags)) == 0){
      ags_effect_line_map_recall(effect_line,
				 0);
    }
  }else{
    ags_effect_line_find_port(effect_line);
  }

  /* connect line members */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(effect_line->table));
  
  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_effect_line_disconnect(AgsConnectable *connectable)
{
  AgsEffectLine *effect_line;

  GList *list, *list_start;

  effect_line = AGS_EFFECT_LINE(connectable);

  if((AGS_EFFECT_LINE_CONNECTED & (effect_line->flags)) == 0){
    return;
  }

  /* unset connected flag */
  effect_line->flags &= (~AGS_EFFECT_LINE_CONNECTED);

  /* disconnect line members */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(effect_line->table));
  
  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  g_list_free(list_start);
}

gchar*
ags_effect_line_get_name(AgsPlugin *plugin)
{
  return(AGS_EFFECT_LINE(plugin)->name);
}

void
ags_effect_line_set_name(AgsPlugin *plugin, gchar *name)
{
  AgsEffectLine *effect_line;

  effect_line = AGS_EFFECT_LINE(plugin);

  effect_line->name = name;
}

gchar*
ags_effect_line_get_version(AgsPlugin *plugin)
{
  return(AGS_EFFECT_LINE(plugin)->version);
}

void
ags_effect_line_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsEffectLine *effect_line;

  effect_line = AGS_EFFECT_LINE(plugin);

  effect_line->version = version;
}

gchar*
ags_effect_line_get_build_id(AgsPlugin *plugin)
{
  return(AGS_EFFECT_LINE(plugin)->build_id);
}

void
ags_effect_line_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsEffectLine *effect_line;

  effect_line = AGS_EFFECT_LINE(plugin);

  effect_line->build_id = build_id;
}

/**
 * ags_effect_line_samplerate_changed:
 * @effect_line: the #AgsEffectLine
 * @samplerate: the samplerate
 * @old_samplerate: the old samplerate
 * 
 * Notify about samplerate changed.
 * 
 * Since: 3.0.0
 */
void
ags_effect_line_samplerate_changed(AgsEffectLine *effect_line,
				   guint samplerate, guint old_samplerate)
{
  g_return_if_fail(AGS_IS_EFFECT_LINE(effect_line));

  g_object_ref((GObject *) effect_line);
  g_signal_emit(G_OBJECT(effect_line),
		effect_line_signals[SAMPLERATE_CHANGED], 0,
		samplerate,
		old_samplerate);
  g_object_unref((GObject *) effect_line);
}

/**
 * ags_effect_line_buffer_size_changed:
 * @effect_line: the #AgsEffectLine
 * @buffer_size: the buffer_size
 * @old_buffer_size: the old buffer_size
 * 
 * Notify about buffer_size changed.
 * 
 * Since: 3.0.0
 */
void
ags_effect_line_buffer_size_changed(AgsEffectLine *effect_line,
				    guint buffer_size, guint old_buffer_size)
{
  g_return_if_fail(AGS_IS_EFFECT_LINE(effect_line));

  g_object_ref((GObject *) effect_line);
  g_signal_emit(G_OBJECT(effect_line),
		effect_line_signals[BUFFER_SIZE_CHANGED], 0,
		buffer_size,
		old_buffer_size);
  g_object_unref((GObject *) effect_line);
}

/**
 * ags_effect_line_format_changed:
 * @effect_line: the #AgsEffectLine
 * @format: the format
 * @old_format: the old format
 * 
 * Notify about format changed.
 * 
 * Since: 3.0.0
 */
void
ags_effect_line_format_changed(AgsEffectLine *effect_line,
			       guint format, guint old_format)
{
  g_return_if_fail(AGS_IS_EFFECT_LINE(effect_line));

  g_object_ref((GObject *) effect_line);
  g_signal_emit(G_OBJECT(effect_line),
		effect_line_signals[FORMAT_CHANGED], 0,
		format,
		old_format);
  g_object_unref((GObject *) effect_line);
}

void
ags_effect_line_real_set_channel(AgsEffectLine *effect_line, AgsChannel *channel)
{
  gchar *str;
    
  if(effect_line->channel == channel){
    return;
  }
  
  if(effect_line->channel != NULL){    
    g_object_unref(G_OBJECT(effect_line->channel));
  }

  if(channel != NULL){
    g_object_ref(G_OBJECT(channel));
  }

  if(effect_line->channel != NULL){
    effect_line->flags &= (~AGS_EFFECT_LINE_PREMAPPED_RECALL);
  }

  if(channel != NULL){
    effect_line->samplerate = channel->samplerate;
    effect_line->buffer_size = channel->buffer_size;
    effect_line->format = channel->format;
  }
  
  effect_line->channel = channel;

  if(channel != NULL){
    guint audio_channel;
    
    /* get audio channel */
    g_object_get(channel,
		 "audio-channel", &audio_channel,
		 NULL);
    
    /* set label */
    str = g_strdup_printf("%s %d", i18n("channel"), audio_channel + 1);
    gtk_label_set_label(effect_line->label,
			str);

    g_free(str);
  }else{
    str = g_strdup_printf("%s (null)", i18n("channel"));
    gtk_label_set_label(effect_line->label,
			str);

    g_free(str);
  }
}

/**
 * ags_effect_line_set_channel:
 * @effect_line: the #AgsEffectLine
 * @channel: the #AgsChannel to set
 *
 * Is emitted as channel gets modified.
 *
 * Since: 3.0.0
 */
void
ags_effect_line_set_channel(AgsEffectLine *effect_line, AgsChannel *channel)
{
  g_return_if_fail(AGS_IS_EFFECT_LINE(effect_line));

  g_object_ref((GObject *) effect_line);
  g_signal_emit(G_OBJECT(effect_line),
		effect_line_signals[SET_CHANNEL], 0,
		channel);
  g_object_unref((GObject *) effect_line);
}

GList*
ags_effect_line_add_ladspa_effect(AgsEffectLine *effect_line,
				  GList *control_type_name,
				  gchar *filename,
				  gchar *effect)
{
  AgsLineMember *line_member;
  AgsEffectSeparator *separator;
  GtkAdjustment *adjustment;
  
  AgsLadspaPlugin *ladspa_plugin;

  AgsRecallHandler *recall_handler;
  
  GList *start_list, *list;
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
  
  list =
    start_list = gtk_container_get_children(effect_line->table);

  while(list != NULL){
    guint top_attach;

    gtk_container_child_get(GTK_CONTAINER(effect_line->table),
			    list->data,
			    "top-attach", &top_attach,
			    NULL);
    
    if(y <= top_attach){
      y = top_attach + 1;
    }

    list = list->next;
  }

  g_list_free(start_list);
  
  /* play - find ports */
  g_object_get(effect_line->channel,
	       "play", &start_recall,
	       NULL);

  /* get by effect */
  recall = ags_recall_get_by_effect(start_recall,
				    filename,
				    effect);

  g_list_free_full(start_recall,
		   g_object_unref);
  
  start_recall = recall;
  
  if(recall == NULL){
    return(NULL);
  }

  recall = g_list_last(start_recall);
  g_object_get((GObject *) recall->data,
	       "port", &play_port,
	       NULL);

  g_list_foreach(play_port,
		 (GFunc) g_object_unref,
		 NULL);
  
  g_list_free_full(start_recall,
		   g_object_unref);
  
  /* recall - find ports */
  g_object_get(effect_line->channel,
	       "recall", &start_recall,
	       NULL);

  /* get by effect */
  recall = start_recall;
  
  start_recall = ags_recall_get_by_effect(start_recall,
					  filename,
					  effect);

  g_list_free_full(recall,
		   g_object_unref);
  
  recall = start_recall;
  
  if(recall == NULL){
    return(NULL);
  }

  recall = g_list_last(start_recall);
  g_object_get((GObject *) recall->data,
	       "port", &recall_port,
	       NULL);

  g_list_foreach(recall_port,
		 (GFunc) g_object_unref,
		 NULL);

  g_list_free_full(start_recall,
		   g_object_unref);

  /* add separator */
  separator = ags_effect_separator_new();
  g_object_set(separator,
	       "text", effect,
	       "filename", filename,
	       "effect", effect,
	       NULL);
  
  gtk_table_attach(effect_line->table,
		   (GtkWidget *) separator,
		   0, AGS_EFFECT_LINE_COLUMNS_COUNT,
		   y, y + 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
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
      guint scale_precision;
      gdouble step_count;
      gboolean disable_seemless;
      gboolean do_step_conversion;

      GRecMutex *plugin_port_mutex;

      disable_seemless = FALSE;
      do_step_conversion = FALSE;

      if(x == AGS_EFFECT_LINE_COLUMNS_COUNT){
	x = 0;
	y++;
	gtk_table_resize(effect_line->table,
			 y + 1, AGS_EFFECT_LINE_COLUMNS_COUNT);
      }
      
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
						   "widget-type", widget_type,
						   "widget-label", AGS_PLUGIN_PORT(plugin_port->data)->port_name,
						   "plugin-name", plugin_name,
						   "filename", filename,
						   "effect", effect,
						   "specifier", AGS_PLUGIN_PORT(plugin_port->data)->port_name,
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

	gtk_adjustment_set_step_increment(adjustment,
					  step);
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
	  control_value = ags_conversion_convert(ladspa_conversion,
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

	gtk_adjustment_set_step_increment(adjustment,
					  step);
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
	  control_value = ags_conversion_convert(ladspa_conversion,
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

	gtk_adjustment_set_step_increment(adjustment,
					  step);
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
	  control_value = ags_conversion_convert(ladspa_conversion,
						 default_value,
						 TRUE);
	}

	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_effect_line_indicator_queue_draw,
			    child_widget, ags_effect_line_indicator_queue_draw_timeout);

	effect_line->queued_drawing = g_list_prepend(effect_line->queued_drawing,
						     child_widget);

	g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0,
		      (GSourceFunc) ags_effect_line_indicator_queue_draw_timeout,
		      (gpointer) child_widget);
      }

#ifdef AGS_DEBUG
      g_message("ladspa bounds: %f %f", lower, upper);
#endif
	  
      gtk_table_attach(effect_line->table,
		       (GtkWidget *) line_member,
		       (x % AGS_EFFECT_LINE_COLUMNS_COUNT), (x % AGS_EFFECT_LINE_COLUMNS_COUNT) + 1,
		       y, y + 1,
		       GTK_FILL, GTK_FILL,
		       0, 0);

      ags_connectable_connect(AGS_CONNECTABLE(line_member));
      gtk_widget_show_all((GtkWidget *) line_member);
      
      /* iterate */
      x++;

      if(x % AGS_EFFECT_LINE_COLUMNS_COUNT == 0){
	y++;
      }
    }
    
    /* iterate */
    plugin_port = plugin_port->next;
    k++;
  }

  g_list_free_full(start_plugin_port,
		   g_object_unref);
  
  return(g_list_concat(play_port,
		       recall_port));
}

GList*
ags_effect_line_add_lv2_effect(AgsEffectLine *effect_line,
			       GList *control_type_name,
			       gchar *filename,
			       gchar *effect)
{
  AgsLineMember *line_member;
  AgsEffectSeparator *separator;
  GtkAdjustment *adjustment;

  AgsLv2Plugin *lv2_plugin;

  AgsRecallHandler *recall_handler;

  GList *start_list, *list;
  GList *start_recall, *recall;
  GList *play_port, *recall_port;
  GList *start_plugin_port, *plugin_port;

  gchar *uri;
  
  gdouble step;
  guint port_count;

  guint x, y;
  guint k;

  /* load plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);

  /* retrieve position within table  */
  x = 0;
  y = 0;

  list =
    start_list = gtk_container_get_children(effect_line->table);

  while(list != NULL){
    guint top_attach;

    gtk_container_child_get(GTK_CONTAINER(effect_line->table),
			    list->data,
			    "top-attach", &top_attach,
			    NULL);
    
    if(y <= top_attach){
      y = top_attach + 1;
    }

    list = list->next;
  }

  g_list_free(start_list);
  
  /* play - find ports */
  g_object_get(effect_line->channel,
	       "play", &start_recall,
	       NULL);

  /* get by effect */
  recall = ags_recall_get_by_effect(start_recall,
				    filename,
				    effect);

  g_list_free_full(start_recall,
		   g_object_unref);
  
  start_recall = recall;
  
  if(recall == NULL){
    return(NULL);
  }
    
  recall = g_list_last(start_recall);
  g_object_get((GObject *) recall->data,
	       "port", &play_port,
	       NULL);

  g_list_foreach(play_port,
		 (GFunc) g_object_unref,
		 NULL);
  
  g_list_free_full(start_recall,
		   g_object_unref);

  /* recall - find ports */
  g_object_get(effect_line->channel,
	       "recall", &start_recall,
	       NULL);

  /* get by effect */
  recall = start_recall;
  
  start_recall = ags_recall_get_by_effect(start_recall,
					  filename,
					  effect);
  
  g_list_free_full(recall,
		   g_object_unref);
  
  recall = start_recall;
  
  if(recall == NULL){
    return(NULL);
  }

  recall = g_list_last(start_recall);
  g_object_get((GObject *) recall->data,
	       "port", &recall_port,
	       NULL);

  g_list_foreach(recall_port,
		 (GFunc) g_object_unref,
		 NULL);
  
  g_list_free_full(start_recall,
		   g_object_unref);

  /* add separator */
  separator = ags_effect_separator_new();
  g_object_set(separator,
	       "text", effect,
	       "filename", filename,
	       "effect", effect,
	       NULL);
  gtk_table_attach(effect_line->table,
		   (GtkWidget *) separator,
		   0, AGS_EFFECT_LINE_COLUMNS_COUNT,
		   y, y + 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_widget_show_all(GTK_WIDGET(separator));
  
  y++;

  /* get uri */
  g_object_get(lv2_plugin,
	       "uri", &uri,
	       NULL);

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
      
      guint scale_precision;
      gdouble step_count;
      gboolean disable_seemless;
      gboolean do_step_conversion;

      GRecMutex *plugin_port_mutex;

      disable_seemless = FALSE;
      do_step_conversion = FALSE;
     
      if(x == AGS_EFFECT_LINE_COLUMNS_COUNT){
	x = 0;
	y++;
	gtk_table_resize(effect_line->table,
			 y + 1, AGS_EFFECT_LINE_COLUMNS_COUNT);
      }

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

	gtk_adjustment_set_step_increment(adjustment,
					  step);
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
	  control_value = ags_conversion_convert(lv2_conversion,
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

	gtk_adjustment_set_step_increment(adjustment,
					  step);
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
	  control_value = ags_conversion_convert(lv2_conversion,
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

	gtk_adjustment_set_step_increment(adjustment,
					  step);
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
	  control_value = ags_conversion_convert(lv2_conversion,
						 default_value,
						 TRUE);
	}

	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_effect_line_indicator_queue_draw,
			    child_widget,
			    ags_effect_line_indicator_queue_draw_timeout);
	
	effect_line->queued_drawing = g_list_prepend(effect_line->queued_drawing,
						     child_widget);
	
	g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0,
		      (GSourceFunc) ags_effect_line_indicator_queue_draw_timeout,
		      (gpointer) child_widget);
      }

#ifdef AGS_DEBUG
      g_message("lv2 bounds: %f %f", lower, upper);
#endif
	  
      gtk_table_attach(effect_line->table,
		       (GtkWidget *) line_member,
		       (x % AGS_EFFECT_LINE_COLUMNS_COUNT), (x % AGS_EFFECT_LINE_COLUMNS_COUNT) + 1,
		       y, y + 1,
		       GTK_FILL, GTK_FILL,
		       0, 0);
      
      ags_connectable_connect(AGS_CONNECTABLE(line_member));
      gtk_widget_show_all((GtkWidget *) line_member);

      /* iterate */
      x++;

      if(x % AGS_EFFECT_LINE_COLUMNS_COUNT == 0){
	y++;
      }
    }

    /* iterate */
    plugin_port = plugin_port->next;
    k++;
  }

  g_list_free_full(start_plugin_port,
		   g_object_unref);
  
  g_free(uri);
  
  return(g_list_concat(play_port,
		       recall_port));
}

GList*
ags_effect_line_real_add_effect(AgsEffectLine *effect_line,
				GList *control_type_name,
				gchar *filename,
				gchar *effect)
{
  AgsWindow *window;

  AgsLadspaPlugin *ladspa_plugin;
  AgsLv2Plugin *lv2_plugin;
  
  GList *port;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) effect_line);

  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);
  port = NULL;
  
  if(ladspa_plugin != NULL){
    port = ags_effect_line_add_ladspa_effect(effect_line,
					     control_type_name,
					     filename,
					     effect);
  }else{
    lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
						 filename, effect);
    
    if(lv2_plugin != NULL){
      port = ags_effect_line_add_lv2_effect(effect_line,
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
 * ags_effect_line_add_effect:
 * @effect_line: the #AgsEffectLine to modify
 * @control_type_name: the #GList-struct containing string representation of a #GType
 * @filename: the effect's filename
 * @effect: the effect's name
 *
 * Add an effect by its filename and effect specifier.
 *
 * Returns: the #GList-struct containing the #AgsPort objects added
 *
 * Since: 3.0.0
 */
GList*
ags_effect_line_add_effect(AgsEffectLine *effect_line,
			   GList *control_type_name,
			   gchar *filename,
			   gchar *effect)
{
  GList *port;
  
  g_return_val_if_fail(AGS_IS_EFFECT_LINE(effect_line), NULL);

  g_object_ref((GObject *) effect_line);
  g_signal_emit(G_OBJECT(effect_line),
		effect_line_signals[ADD_EFFECT], 0,
		control_type_name,
		filename,
		effect,
		&port);
  g_object_unref((GObject *) effect_line);

  return(port);
}

void
ags_effect_line_real_remove_effect(AgsEffectLine *effect_line,
				   guint nth)
{
  AgsWindow *window;

  GList *control, *control_start;
  GList *start_recall, *recall;
  GList *start_port, *port;

  gchar *filename, *effect;

  guint nth_effect, n_bulk;
  guint i;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) effect_line);
  
  /* get nth_effect */
  g_object_get(effect_line->channel,
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
    g_list_free_full(start_recall,
		     g_object_unref);

    return;
  }

  nth_effect--;
  
  /* get filename and effect */
  g_object_get(recall->data,
	       "filename", &filename,
	       "effect", &effect,
	       NULL);

  /* destroy separator */
  control_start =
    control = gtk_container_get_children((GtkContainer *) effect_line->table);

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
      control = gtk_container_get_children((GtkContainer *) effect_line->table);
    
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
	  g_hash_table_remove(ags_effect_line_indicator_queue_draw,
			      child_widget);
	}

	gtk_widget_destroy(control->data);
	
	break;
      }
	
      /* iterate */
      control = control->next;
    }

    g_list_free(control_start);

    /* iterate */
    port = port->next;
  }

  g_list_free_full(start_recall,
		   g_object_unref);
  g_list_free_full(start_port,
		   g_object_unref);

  /* remove recalls */
  ags_channel_remove_effect(effect_line->channel,
			    nth_effect);

  /* reset automation editor */
  ags_automation_toolbar_load_port(window->automation_window->automation_editor->automation_toolbar);
}

/**
 * ags_effect_line_remove_effect:
 * @effect_line: the #AgsEffectLine to modify
 * @nth: the nth effect to remove
 *
 * Remove an effect by its position.
 *
 * Since: 3.0.0
 */
void
ags_effect_line_remove_effect(AgsEffectLine *effect_line,
			      guint nth)
{
  g_return_if_fail(AGS_IS_EFFECT_LINE(effect_line));

  g_object_ref((GObject *) effect_line);
  g_signal_emit(G_OBJECT(effect_line),
		effect_line_signals[REMOVE_EFFECT], 0,
		nth);
  g_object_unref((GObject *) effect_line);
}

void
ags_effect_line_real_map_recall(AgsEffectLine *effect_line,
				guint ouput_pad_start)
{
  if((AGS_EFFECT_LINE_MAPPED_RECALL & (effect_line->flags)) != 0){
    return;
  }

  effect_line->flags |= AGS_EFFECT_LINE_MAPPED_RECALL;

  ags_effect_line_find_port(effect_line);
}

/**
 * ags_effect_line_map_recall:
 * @effect_line: the #AgsEffectLine to add its default recall.
 * @output_pad_start: the start channel's index
 *
 * You may want the @effect_line to add its default recall. This function
 * may call ags_effect_line_find_port().
 *
 * Since: 3.0.0
 */
void
ags_effect_line_map_recall(AgsEffectLine *effect_line,
			   guint output_pad_start)
{
  g_return_if_fail(AGS_IS_EFFECT_LINE(effect_line));

  g_object_ref((GObject *) effect_line);
  g_signal_emit((GObject *) effect_line,
		effect_line_signals[MAP_RECALL], 0,
		output_pad_start);
  g_object_unref((GObject *) effect_line);
}

GList*
ags_effect_line_real_find_port(AgsEffectLine *effect_line)
{
  GList *port, *tmp_port;
  GList *line_member, *line_member_start;

  if(effect_line == NULL || effect_line->table == NULL){
    return(NULL);
  }

  line_member_start = 
    line_member = gtk_container_get_children(GTK_CONTAINER(effect_line->table));
  
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
 * ags_effect_line_find_port:
 * @effect_line: the #AgsEffectLine
 *
 * Lookup ports of associated recalls.
 *
 * Returns: a #GList-struct containing all related #AgsPort
 *
 * Since: 3.0.0
 */
GList*
ags_effect_line_find_port(AgsEffectLine *effect_line)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_EFFECT_LINE(effect_line),
		       NULL);

  g_object_ref((GObject *) effect_line);
  g_signal_emit((GObject *) effect_line,
		effect_line_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) effect_line);

  return(list);
}

/**
 * ags_effect_line_done:
 * @effect_line: the #AgsEffectLine
 * @recall_id: the #AgsRecallID
 *
 * Notify about to stop playback of @recall_id.
 * 
 * Since: 3.0.0
 */
void
ags_effect_line_done(AgsEffectLine *effect_line, GObject *recall_id)
{
  g_return_if_fail(AGS_IS_EFFECT_LINE(effect_line));

  g_object_ref((GObject *) effect_line);
  g_signal_emit((GObject *) effect_line,
		effect_line_signals[DONE], 0,
		recall_id);
  g_object_unref((GObject *) effect_line);
}

/**
 * ags_effect_line_find_next_grouped:
 * @effect_line: a #GList-struct of #AgsEffectLine objects
 *
 * Retrieve next grouped effect_line.
 *
 * Returns: next matching #GList-struct containing #AgsEffectLine
 * 
 * Since: 3.0.0
 */
GList*
ags_effect_line_find_next_grouped(GList *effect_line)
{
  while(effect_line != NULL && !gtk_toggle_button_get_active(AGS_EFFECT_LINE(effect_line->data)->group)){
    effect_line = effect_line->next;
  }

  return(effect_line);
}

/**
 * ags_effect_line_check_message:
 * @effect_line: the #AgsEffectLine
 *
 * Check message queue for message envelopes.
 *
 * Since: 3.0.0
 */
void
ags_effect_line_check_message(AgsEffectLine *effect_line)
{
  AgsChannel *channel;
    
  AgsMessageDelivery *message_delivery;

  GList *start_message_envelope, *message_envelope;

  if(!AGS_IS_EFFECT_LINE(effect_line)){
    return;
  }

  /* retrieve message */
  message_delivery = ags_message_delivery_get_instance();

  channel = effect_line->channel;
	  
  message_envelope =
    start_message_envelope = ags_message_delivery_find_sender(message_delivery,
							      "libgsequencer",
							      (GObject *) channel);
  
  while(message_envelope != NULL){
    xmlNode *root_node;

    root_node = xmlDocGetRootElement(AGS_MESSAGE_ENVELOPE(message_envelope->data)->doc);
      
    if(!xmlStrncmp(root_node->name,
		   "ags-command",
		   12)){
      if(!xmlStrncmp(xmlGetProp(root_node,
				"method"),
		     "AgsChannel::set-samplerate",
		     27)){
	guint samplerate;
	gint position;
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "samplerate");
	samplerate = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	/* set samplerate */
	g_object_set(effect_line,
		     "samplerate", samplerate,
		     NULL);
      }else if(!xmlStrncmp(xmlGetProp(root_node,
				      "method"),
			   "AgsChannel::set-buffer-size",
			   28)){
	guint buffer_size;
	gint position;
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "buffer-size");
	buffer_size = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	/* set buffer size */
	g_object_set(effect_line,
		     "buffer-size", buffer_size,
		     NULL);
      }else if(!xmlStrncmp(xmlGetProp(root_node,
				      "method"),
			   "AgsChannel::set-format",
			   23)){
	guint format;
	gint position;
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "format");
	format = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	/* set format */
	g_object_set(effect_line,
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
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "filename");
	filename = g_value_get_string(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "effect");
	effect = g_value_get_string(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	/* get machine and machine editor */
	machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) effect_line,
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

	      port_control = 
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
	ags_effect_line_add_effect(effect_line,
				   control_type_name,
				   filename,
				   effect);
      }else if(!xmlStrncmp(xmlGetProp(root_node,
				      "method"),
			   "AgsChannel::done",
			   16)){
	AgsRecallID *recall_id;

	gint position;

	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "recall-id");
	  
	recall_id = g_value_get_object(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	/* done */
	ags_effect_line_done(effect_line,
			     (GObject *) recall_id);
      }
    }
    
    message_envelope = message_envelope->next;
  }

  g_list_free_full(start_message_envelope,
		   g_object_unref);
}

/**
 * ags_effect_line_indicator_queue_draw_timeout:
 * @widget: the indicator widgt
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_effect_line_indicator_queue_draw_timeout(GtkWidget *widget)
{
  AgsEffectLine *effect_line;

  if(g_hash_table_lookup(ags_effect_line_indicator_queue_draw,
			 widget) != NULL){
    GList *list, *list_start;
    
    effect_line = (AgsEffectLine *) gtk_widget_get_ancestor(widget,
							    AGS_TYPE_EFFECT_LINE);

    list_start = 
      list = gtk_container_get_children((GtkContainer *) AGS_EFFECT_LINE(effect_line)->table);

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

	GRecMutex *port_mutex;
	GRecMutex *plugin_port_mutex;
	
	line_member = AGS_LINE_MEMBER(list->data);
	child = gtk_bin_get_child(GTK_BIN(line_member));
      
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

	g_object_unref(plugin_port);
	
	/* get port mutex */
	port_mutex = AGS_PORT_GET_OBJ_MUTEX(current);

	/* match specifier */
	g_rec_mutex_lock(port_mutex);

	success = (!g_ascii_strcasecmp(current->specifier,
				       line_member->specifier)) ? TRUE: FALSE;
	
	g_rec_mutex_unlock(port_mutex);

	if(!success){
	  list = list->next;
	
	  continue;
	}

	/* get plugin port mutex */
	plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port);
	
	/* lower and upper */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower = g_value_get_float(plugin_port->lower_value);
	upper = g_value_get_float(plugin_port->upper_value);
      
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
	    ags_led_set_active((AgsLed *) child);
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
 * ags_effect_line_new:
 * @channel: the #AgsChannel to visualize
 *
 * Create a new instance of #AgsEffectLine
 *
 * Returns: the new #AgsEffectLine
 *
 * Since: 3.0.0
 */
AgsEffectLine*
ags_effect_line_new(AgsChannel *channel)
{
  AgsEffectLine *effect_line;

  effect_line = (AgsEffectLine *) g_object_new(AGS_TYPE_EFFECT_LINE,
					       "channel", channel,
					       NULL);

  return(effect_line);
}
