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

#include <ags/X/ags_effect_pad.h>
#include <ags/X/ags_effect_pad_callbacks.h>

#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_line.h>

#include <ags/i18n.h>

void ags_effect_pad_class_init(AgsEffectPadClass *effect_pad);
void ags_effect_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_effect_pad_init(AgsEffectPad *effect_pad);
void ags_effect_pad_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_effect_pad_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);

void ags_effect_pad_connect(AgsConnectable *connectable);
void ags_effect_pad_disconnect(AgsConnectable *connectable);

void ags_effect_pad_real_set_channel(AgsEffectPad *effect_pad, AgsChannel *channel);

void ags_effect_pad_real_resize_lines(AgsEffectPad *effect_pad, GType line_type,
				      guint audio_channels, guint audio_channels_old);
void ags_effect_pad_real_map_recall(AgsEffectPad *effect_pad);
GList* ags_effect_pad_real_find_port(AgsEffectPad *effect_pad);

/**
 * SECTION:ags_effect_pad
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsEffectPad
 * @section_id:
 * @include: ags/X/ags_effect_pad.h
 *
 * #AgsEffectPad is a composite widget to visualize a bunch of #AgsChannel. It should be
 * packed by an #AgsEffectBridge.
 */

enum{
  SAMPLERATE_CHANGED,
  BUFFER_SIZE_CHANGED,
  FORMAT_CHANGED,
  SET_CHANNEL,
  RESIZE_LINES,
  MAP_RECALL,
  FIND_PORT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_CHANNEL,
};

static gpointer ags_effect_pad_parent_class = NULL;
static guint effect_pad_signals[LAST_SIGNAL];

GType
ags_effect_pad_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_effect_pad = 0;

    static const GTypeInfo ags_effect_pad_info = {
      sizeof(AgsEffectPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_effect_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEffectPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_effect_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_effect_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_effect_pad = g_type_register_static(GTK_TYPE_VBOX,
						 "AgsEffectPad", &ags_effect_pad_info,
						 0);

    g_type_add_interface_static(ags_type_effect_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_effect_pad);
  }

  return g_define_type_id__volatile;
}

void
ags_effect_pad_class_init(AgsEffectPadClass *effect_pad)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_effect_pad_parent_class = g_type_class_peek_parent(effect_pad);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(effect_pad);

  gobject->set_property = ags_effect_pad_set_property;
  gobject->get_property = ags_effect_pad_get_property;

  
  /* properties */
  /**
   * AgsEffectPad:samplerate:
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
   * AgsEffectPad:buffer-size:
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
   * AgsEffectPad:format:
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
   * AgsEffectPad:channel:
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


  /* AgsEffectPadClass */
  effect_pad->samplerate_changed = NULL;
  effect_pad->buffer_size_changed = NULL;
  effect_pad->format_changed = NULL;

  effect_pad->set_channel = ags_effect_pad_real_set_channel;
  
  effect_pad->resize_lines = ags_effect_pad_real_resize_lines;

  effect_pad->map_recall = ags_effect_pad_real_map_recall;
  effect_pad->find_port = ags_effect_pad_real_find_port;

  /* signals */
  /**
   * AgsEffectPad::samplerate-changed:
   * @effect_pad: the #AgsEffectPad
   * @samplerate: the samplerate
   * @old_samplerate: the old samplerate
   *
   * The ::samplerate-changed signal notifies about changed samplerate.
   * 
   * Since: 3.0.0
   */
  effect_pad_signals[SAMPLERATE_CHANGED] =
    g_signal_new("samplerate-changed",
		 G_TYPE_FROM_CLASS(effect_pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectPadClass, samplerate_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsEffectPad::buffer-size-changed:
   * @effect_pad: the #AgsEffectPad
   * @buffer_size: the buffer size
   * @old_buffer_size: the old buffer size
   *
   * The ::buffer-size-changed signal notifies about changed buffer size.
   * 
   * Since: 3.0.0
   */
  effect_pad_signals[BUFFER_SIZE_CHANGED] =
    g_signal_new("buffer-size-changed",
		 G_TYPE_FROM_CLASS(effect_pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectPadClass, buffer_size_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsEffectPad::format-changed:
   * @effect_pad: the #AgsEffectPad
   * @format: the format
   * @old_format: the old format
   *
   * The ::format-changed signal notifies about changed format.
   * 
   * Since: 3.0.0
   */
  effect_pad_signals[FORMAT_CHANGED] =
    g_signal_new("format-changed",
		 G_TYPE_FROM_CLASS(effect_pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectPadClass, format_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsEffectPad::set-channel:
   * @effect_pad: the #AgsEffectPad to modify
   * @channel: the #AgsChannel to set
   *
   * The ::set-channel signal notifies about changed channel.
   *
   * Since: 3.0.0
   */
  effect_pad_signals[SET_CHANNEL] =
    g_signal_new("set-channel",
		 G_TYPE_FROM_CLASS(effect_pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectPadClass, set_channel),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsPad::resize-lines:
   * @effect_pad: the #AgsEffectPad to resize
   * @line_type: the channel type
   * @audio_channels: count of lines
   * @audio_channels_old: old count of lines
   *
   * The ::resize-lines is emitted as count of lines pack is modified.
   * 
   * Since: 3.0.0
   */
  effect_pad_signals[RESIZE_LINES] =
    g_signal_new("resize-lines",
		 G_TYPE_FROM_CLASS(effect_pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectPadClass, resize_lines),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__ULONG_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_ULONG,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsEffectPad::map-recall:
   * @effect_pad: the #AgsEffectPad
   *
   * The ::map-recall should be used to add the effect_pad's default recall.
   * 
   * Since: 3.0.0
   */
  effect_pad_signals[MAP_RECALL] =
    g_signal_new("map-recall",
                 G_TYPE_FROM_CLASS (effect_pad),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsEffectPadClass, map_recall),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE, 0);

  /**
   * AgsEffectPad::find-port:
   * @effect_pad: the #AgsEffectPad to resize
   *
   * The ::find-port as recall should be mapped
   * 
   * Returns: a #GList-struct with associated ports
   *
   * Since: 3.0.0
   */
  effect_pad_signals[FIND_PORT] =
    g_signal_new("find-port",
		 G_TYPE_FROM_CLASS(effect_pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectPadClass, find_port),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
}

void
ags_effect_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_effect_pad_connect;
  connectable->disconnect = ags_effect_pad_disconnect;
}

void
ags_effect_pad_init(AgsEffectPad *effect_pad)
{  
  AgsConfig *config;

  effect_pad->flags = 0;

  effect_pad->name = NULL;
  
  effect_pad->version = AGS_EFFECT_PAD_DEFAULT_VERSION;
  effect_pad->build_id = AGS_EFFECT_PAD_DEFAULT_BUILD_ID;

  config = ags_config_get_instance();
  
  effect_pad->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  effect_pad->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  effect_pad->format = ags_soundcard_helper_config_get_format(config);

  effect_pad->channel = NULL;

  effect_pad->cols = AGS_EFFECT_PAD_COLUMNS_COUNT;
  effect_pad->table = (GtkTable *) gtk_table_new(1, AGS_EFFECT_PAD_COLUMNS_COUNT,
						 TRUE);
  gtk_box_pack_start(GTK_BOX(effect_pad),
		     GTK_WIDGET(effect_pad->table),
		     FALSE, TRUE,
		     0);
}

void
ags_effect_pad_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsEffectPad *effect_pad;

  effect_pad = AGS_EFFECT_PAD(gobject);

  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      GList *start_list, *list;

      guint samplerate, old_samplerate;
      
      samplerate = g_value_get_uint(value);
      old_samplerate = effect_pad->samplerate;

      if(samplerate == old_samplerate){
	return;
      }

      effect_pad->samplerate = samplerate;

      ags_effect_pad_samplerate_changed(effect_pad,
					samplerate, old_samplerate);

      list = 
	start_list = gtk_container_get_children(GTK_CONTAINER(effect_pad->table));

      while(list != NULL){
	if(AGS_IS_EFFECT_LINE(list->data)){
	  g_object_set(list->data,
		       "samplerate", samplerate,
		       NULL);
	}

	list = list->next;
      }

      g_list_free(start_list);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      GList *start_list, *list;

      guint buffer_size, old_buffer_size;
      
      buffer_size = g_value_get_uint(value);
      old_buffer_size = effect_pad->buffer_size;

      if(buffer_size == old_buffer_size){
	return;
      }

      effect_pad->buffer_size = buffer_size;

      ags_effect_pad_buffer_size_changed(effect_pad,
					 buffer_size, old_buffer_size);

      list = 
	start_list = gtk_container_get_children(GTK_CONTAINER(effect_pad->table));

      while(list != NULL){
	if(AGS_IS_EFFECT_LINE(list->data)){
	  g_object_set(list->data,
		       "buffer-size", buffer_size,
		       NULL);
	}

	list = list->next;
      }

      g_list_free(start_list);
    }
    break;
  case PROP_FORMAT:
    {
      GList *start_list, *list;

      guint format, old_format;
      
      format = g_value_get_uint(value);
      old_format = effect_pad->format;

      if(format == old_format){
	return;
      }

      effect_pad->format = format;

      ags_effect_pad_format_changed(effect_pad,
				    format, old_format);

      list = 
	start_list = gtk_container_get_children(GTK_CONTAINER(effect_pad->table));

      while(list != NULL){
	if(AGS_IS_EFFECT_LINE(list->data)){
	  g_object_set(list->data,
		       "format", format,
		       NULL);
	}

	list = list->next;
      }

      g_list_free(start_list);
    }
    break;
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      ags_effect_pad_set_channel(effect_pad, channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_pad_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsEffectPad *effect_pad;

  effect_pad = AGS_EFFECT_PAD(gobject);

  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value,
		       effect_pad->samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value,
		       effect_pad->buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value,
		       effect_pad->format);
    }
    break;
  case PROP_CHANNEL:
    {
      g_value_set_object(value,
			 effect_pad->channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_pad_connect(AgsConnectable *connectable)
{
  AgsEffectPad *effect_pad;

  GList *effect_line_list, *effect_line_list_start;

  /* AgsEffect_Pad */
  effect_pad = AGS_EFFECT_PAD(connectable);

  if((AGS_EFFECT_PAD_CONNECTED & (effect_pad->flags)) != 0){
    return;
  }
  
  effect_pad->flags |= AGS_EFFECT_PAD_CONNECTED;

  if((AGS_EFFECT_PAD_PREMAPPED_RECALL & (effect_pad->flags)) == 0){
    if((AGS_EFFECT_PAD_MAPPED_RECALL & (effect_pad->flags)) == 0){
      //TODO:JK: implement me
    }
  }else{
    effect_pad->flags &= (~AGS_EFFECT_PAD_PREMAPPED_RECALL);
  }

  /* AgsEffectLine */
  effect_line_list_start =  
    effect_line_list = gtk_container_get_children(GTK_CONTAINER(effect_pad->table));

  while(effect_line_list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(effect_line_list->data));

    effect_line_list = effect_line_list->next;
  }

  g_list_free(effect_line_list_start);
}

void
ags_effect_pad_disconnect(AgsConnectable *connectable)
{
  AgsEffectPad *effect_pad;

  GList *effect_line_list, *effect_line_list_start;

  /* AgsEffect_Pad */
  effect_pad = AGS_EFFECT_PAD(connectable);

  if((AGS_EFFECT_PAD_CONNECTED & (effect_pad->flags)) == 0){
    return;
  }
  
  effect_pad->flags &= (~AGS_EFFECT_PAD_CONNECTED);

  /* AgsEffectLine */
  effect_line_list_start =  
    effect_line_list = gtk_container_get_children(GTK_CONTAINER(effect_pad->table));

  while(effect_line_list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(effect_line_list->data));

    effect_line_list = effect_line_list->next;
  }

  g_list_free(effect_line_list_start);
}

/**
 * ags_effect_pad_samplerate_changed:
 * @effect_pad: the #AgsEffectPad
 * @samplerate: the samplerate
 * @old_samplerate: the old samplerate
 * 
 * Notify about samplerate changed.
 * 
 * Since: 3.0.0
 */
void
ags_effect_pad_samplerate_changed(AgsEffectPad *effect_pad,
				  guint samplerate, guint old_samplerate)
{
  g_return_if_fail(AGS_IS_EFFECT_PAD(effect_pad));

  g_object_ref((GObject *) effect_pad);
  g_signal_emit(G_OBJECT(effect_pad),
		effect_pad_signals[SAMPLERATE_CHANGED], 0,
		samplerate,
		old_samplerate);
  g_object_unref((GObject *) effect_pad);
}

/**
 * ags_effect_pad_buffer_size_changed:
 * @effect_pad: the #AgsEffectPad
 * @buffer_size: the buffer_size
 * @old_buffer_size: the old buffer_size
 * 
 * Notify about buffer_size changed.
 * 
 * Since: 3.0.0
 */
void
ags_effect_pad_buffer_size_changed(AgsEffectPad *effect_pad,
				   guint buffer_size, guint old_buffer_size)
{
  g_return_if_fail(AGS_IS_EFFECT_PAD(effect_pad));

  g_object_ref((GObject *) effect_pad);
  g_signal_emit(G_OBJECT(effect_pad),
		effect_pad_signals[BUFFER_SIZE_CHANGED], 0,
		buffer_size,
		old_buffer_size);
  g_object_unref((GObject *) effect_pad);
}

/**
 * ags_effect_pad_format_changed:
 * @effect_pad: the #AgsEffectPad
 * @format: the format
 * @old_format: the old format
 * 
 * Notify about format changed.
 * 
 * Since: 3.0.0
 */
void
ags_effect_pad_format_changed(AgsEffectPad *effect_pad,
			      guint format, guint old_format)
{
  g_return_if_fail(AGS_IS_EFFECT_PAD(effect_pad));

  g_object_ref((GObject *) effect_pad);
  g_signal_emit(G_OBJECT(effect_pad),
		effect_pad_signals[FORMAT_CHANGED], 0,
		format,
		old_format);
  g_object_unref((GObject *) effect_pad);
}

void
ags_effect_pad_real_set_channel(AgsEffectPad *effect_pad, AgsChannel *channel)
{
  GList *start_effect_line, *effect_line;
      
  gchar *str;
    
  if(effect_pad->channel == channel){
    return;
  }
  
  if(effect_pad->channel != NULL){    
    g_object_unref(G_OBJECT(effect_pad->channel));
  }

  if(channel != NULL){
    g_object_ref(G_OBJECT(channel));
  }

  if(effect_pad->channel != NULL){
    effect_pad->flags &= (~AGS_EFFECT_PAD_PREMAPPED_RECALL);
  }

  if(channel != NULL){
    effect_pad->samplerate = channel->samplerate;
    effect_pad->buffer_size = channel->buffer_size;
    effect_pad->format = channel->format;
  }
  
  effect_pad->channel = channel;

  start_effect_line =
    effect_line = gtk_container_get_children((GtkContainer *) effect_pad->table);

  while(effect_line != NULL){
    g_object_set(G_OBJECT(effect_line->data),
		 "channel", channel,
		 NULL);

    effect_line = effect_line->next;
  }

  g_list_free(start_effect_line);
}

/**
 * ags_effect_pad_set_channel:
 * @effect_pad: the #AgsEffectPad
 * @channel: the #AgsChannel to set
 *
 * Is emitted as channel gets modified.
 *
 * Since: 3.0.0
 */
void
ags_effect_pad_set_channel(AgsEffectPad *effect_pad, AgsChannel *channel)
{
  g_return_if_fail(AGS_IS_EFFECT_PAD(effect_pad));

  g_object_ref((GObject *) effect_pad);
  g_signal_emit(G_OBJECT(effect_pad),
		effect_pad_signals[SET_CHANNEL], 0,
		channel);
  g_object_unref((GObject *) effect_pad);
}

void
ags_effect_pad_real_resize_lines(AgsEffectPad *effect_pad, GType effect_line_type,
				 guint audio_channels, guint audio_channels_old)
{
  AgsEffectLine *effect_line;

  AgsAudio *audio;
  AgsChannel *channel;

  GList *start_list, *list;

  guint audio_audio_channels;
  guint i, j;  

  audio = NULL;

  audio_audio_channels = 0;

  if(effect_pad->channel != NULL){
    g_object_get(effect_pad->channel,
		 "audio", &audio,
		 NULL);
  }
  
  if(audio != NULL){
    g_object_get(audio,
		 "audio-channels", &audio_audio_channels,
		 NULL);
  }
  
  if(audio_channels > audio_channels_old){    
    for(i = audio_channels_old; i < audio_channels;){
      for(j = audio_channels_old % effect_pad->cols; j < effect_pad->cols && i < audio_channels; j++, i++){
	if(i < audio_audio_channels){
	  channel = ags_channel_nth(effect_pad->channel,
				    i);
	}else{
	  channel = NULL;
	}

	effect_line = (AgsEffectLine *) g_object_new(effect_line_type,
						     "channel", channel,
						     NULL);
	gtk_table_attach(effect_pad->table,
			 (GtkWidget *) effect_line,
			 j, j + 1,
			 i / effect_pad->cols, i / effect_pad->cols + 1,
			 FALSE, FALSE,
			 0, 0);

	/* iterate */
	if(channel != NULL){
	  g_object_unref(channel);
	}
      }
    }
  }else{
    start_list = gtk_container_get_children((GtkContainer *) effect_pad->table);
    list = g_list_nth(start_list,
		      audio_channels);

    while(list != NULL){
      gtk_widget_destroy(list->data);

      list = list->next;
    }

    g_list_free(start_list);
  }

  if(audio != NULL){
    g_object_unref(audio);
  }    
}

/**
 * ags_effect_pad_resize_lines:
 * @effect_pad: the #AgsEffectPad to resize
 * @line_type: channel type, either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
 * @audio_channels: count of lines
 * @audio_channels_old: old count of lines
 *
 * Resize the count of #AgsEffectLine packe by #AgsEffectPad.
 *
 * Since: 3.0.0
 */
void
ags_effect_pad_resize_lines(AgsEffectPad *effect_pad, GType line_type,
			    guint audio_channels, guint audio_channels_old)
{
  g_return_if_fail(AGS_IS_EFFECT_PAD(effect_pad));
  
  g_object_ref((GObject *) effect_pad);
  g_signal_emit(G_OBJECT(effect_pad),
		effect_pad_signals[RESIZE_LINES], 0,
		line_type,
		audio_channels, audio_channels_old);
  g_object_unref((GObject *) effect_pad);
}

void
ags_effect_pad_real_map_recall(AgsEffectPad *effect_pad)
{
  if((AGS_EFFECT_PAD_MAPPED_RECALL & (effect_pad->flags)) != 0){
    return;
  }

  effect_pad->flags |= AGS_EFFECT_PAD_MAPPED_RECALL;

  ags_effect_pad_find_port(effect_pad);
}

/**
 * ags_effect_pad_map_recall:
 * @effect_pad: the #AgsEffectPad to add its default recall.
 *
 * You may want the @effect_pad to add its default recall.
 *
 * Since: 3.0.0
 */
void
ags_effect_pad_map_recall(AgsEffectPad *effect_pad)
{
  g_return_if_fail(AGS_IS_EFFECT_PAD(effect_pad));

  g_object_ref((GObject *) effect_pad);
  g_signal_emit((GObject *) effect_pad,
		effect_pad_signals[MAP_RECALL], 0);
  g_object_unref((GObject *) effect_pad);
}

GList*
ags_effect_pad_real_find_port(AgsEffectPad *effect_pad)
{
  GList *effect_line, *effect_line_start;
  
  GList *port, *tmp_port;

  port = NULL;

  /* find output ports */
  if(effect_pad->table != NULL){
    effect_line_start = 
      effect_line = gtk_container_get_children((GtkContainer *) effect_pad->table);

    while(effect_line != NULL){
      tmp_port = ags_effect_line_find_port(AGS_EFFECT_LINE(effect_line->data));
      
      if(port != NULL){
	port = g_list_concat(port,
			     tmp_port);
      }else{
	port = tmp_port;
      }

      effect_line = effect_line->next;
    }

    g_list_free(effect_line_start);
  }

  return(port);
}

/**
 * ags_effect_pad_find_port:
 * @effect_pad: the #AgsEffectPad
 *
 * Lookup ports of associated recalls.
 *
 * Returns: an #GList-struct containing all related #AgsPort
 * 
 * Since: 3.0.0
 */
GList*
ags_effect_pad_find_port(AgsEffectPad *effect_pad)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_EFFECT_PAD(effect_pad),
		       NULL);

  g_object_ref((GObject *) effect_pad);
  g_signal_emit((GObject *) effect_pad,
		effect_pad_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) effect_pad);

  return(list);
}

/**
 * ags_effect_pad_new:
 * @channel: the #AgsChannel to visualize
 *
 * Create a new instance of #AgsEffectPad
 *
 * Returns: the new #AgsEffectPad
 *
 * Since: 3.0.0
 */
AgsEffectPad*
ags_effect_pad_new(AgsChannel *channel)
{
  AgsEffectPad *effect_pad;

  effect_pad = (AgsEffectPad *) g_object_new(AGS_TYPE_EFFECT_PAD,
					     "channel", channel,
					     NULL);

  return(effect_pad);
}
