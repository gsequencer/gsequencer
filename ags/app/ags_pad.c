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

#include <ags/app/ags_pad.h>
#include <ags/app/ags_pad_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/i18n.h>

void ags_pad_class_init(AgsPadClass *pad);
void ags_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pad_init(AgsPad *pad);
void ags_pad_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec);
void ags_pad_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec);

void ags_pad_connect(AgsConnectable *connectable);
void ags_pad_disconnect(AgsConnectable *connectable);

void ags_pad_real_set_channel(AgsPad *pad, AgsChannel *channel);
void ags_pad_real_resize_lines(AgsPad *pad, GType line_type,
			       guint audio_channels, guint audio_channels_old);
void ags_pad_real_map_recall(AgsPad *pad,
			     guint output_pad_start);
GList* ags_pad_real_find_port(AgsPad *pad);

/**
 * SECTION:ags_pad
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsPad
 * @section_id:
 * @include: ags/app/ags_pad.h
 *
 * #AgsPad is a composite widget to visualize a bunch of #AgsChannel. It should be
 * packed by an #AgsMachine.
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

static gpointer ags_pad_parent_class = NULL;
static guint pad_signals[LAST_SIGNAL];

GType
ags_pad_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_pad = 0;

    static const GTypeInfo ags_pad_info = {
      sizeof(AgsPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pad = g_type_register_static(GTK_TYPE_BOX,
					  "AgsPad", &ags_pad_info,
					  0);

    g_type_add_interface_static(ags_type_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_pad);
  }

  return g_define_type_id__volatile;
}

void
ags_pad_class_init(AgsPadClass *pad)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_pad_parent_class = g_type_class_peek_parent(pad);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(pad);

  gobject->set_property = ags_pad_set_property;
  gobject->get_property = ags_pad_get_property;

  //TODO:JK: add finalize

  /* properties */
  /**
   * AgsPad:samplerate:
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
   * AgsPad:buffer-size:
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
   * AgsPad:format:
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
   * AgsPad:channel:
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

  /* AgsPadClass */
  pad->samplerate_changed = NULL;
  pad->buffer_size_changed = NULL;
  pad->format_changed = NULL;

  pad->set_channel = ags_pad_real_set_channel;
  pad->resize_lines = ags_pad_real_resize_lines;
  pad->map_recall = ags_pad_real_map_recall;
  pad->find_port = ags_pad_real_find_port;

  /* signals */
  /**
   * AgsPad::samplerate-changed:
   * @pad: the #AgsPad
   * @samplerate: the samplerate
   * @old_samplerate: the old samplerate
   *
   * The ::samplerate-changed signal notifies about changed samplerate.
   * 
   * Since: 3.0.0
   */
  pad_signals[SAMPLERATE_CHANGED] =
    g_signal_new("samplerate-changed",
		 G_TYPE_FROM_CLASS(pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPadClass, samplerate_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsPad::buffer-size-changed:
   * @pad: the #AgsPad
   * @buffer_size: the buffer size
   * @old_buffer_size: the old buffer size
   *
   * The ::buffer-size-changed signal notifies about changed buffer size.
   * 
   * Since: 3.0.0
   */
  pad_signals[BUFFER_SIZE_CHANGED] =
    g_signal_new("buffer-size-changed",
		 G_TYPE_FROM_CLASS(pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPadClass, buffer_size_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsPad::format-changed:
   * @pad: the #AgsPad
   * @format: the format
   * @old_format: the old format
   *
   * The ::format-changed signal notifies about changed format.
   * 
   * Since: 3.0.0
   */
  pad_signals[FORMAT_CHANGED] =
    g_signal_new("format-changed",
		 G_TYPE_FROM_CLASS(pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPadClass, format_changed),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsPad::set-channel:
   * @pad: the #AgsPad to modify
   * @channel: the #AgsChannel to set
   *
   * The ::set-channel signal notifies about changed channel.
   * 
   * Since: 3.0.0
   */
  pad_signals[SET_CHANNEL] =
    g_signal_new("set-channel",
		 G_TYPE_FROM_CLASS(pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPadClass, set_channel),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsPad::resize-lines:
   * @pad: the #AgsPad to resize
   * @line_type: the channel type
   * @audio_channels: count of lines
   * @audio_channels_old: old count of lines
   *
   * The ::resize-lines is emitted as count of lines pack is modified.
   * 
   * Since: 3.0.0
   */
  pad_signals[RESIZE_LINES] =
    g_signal_new("resize-lines",
		 G_TYPE_FROM_CLASS(pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPadClass, resize_lines),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__ULONG_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_ULONG, G_TYPE_UINT, G_TYPE_UINT);

  
  /**
   * AgsPad::map-recall:
   * @pad: the #AgsPad to resize
   * @output_pad_start: start of output pad
   *
   * The ::map-recall as recall should be mapped
   * 
   * Since: 3.0.0
   */
  pad_signals[MAP_RECALL] =
    g_signal_new("map-recall",
		 G_TYPE_FROM_CLASS(pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPadClass, map_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsPad::find-port:
   * @pad: the #AgsPad to resize
   *
   * The ::find-port retrieves all associated ports
   * 
   * Returns: a #GList-struct with associated ports
   *
   * Since: 3.0.0
   */
  pad_signals[FIND_PORT] =
    g_signal_new("find-port",
		 G_TYPE_FROM_CLASS(pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPadClass, find_port),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
}

void
ags_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_pad_connect;
  connectable->disconnect = ags_pad_disconnect;
}

void
ags_pad_init(AgsPad *pad)
{
  GtkBox *hbox;

  AgsConfig *config;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(pad),
				 GTK_ORIENTATION_VERTICAL);  
  
  pad->flags = 0;

  pad->name = NULL;

  pad->version = AGS_VERSION;
  pad->build_id = AGS_BUILD_ID;

  config = ags_config_get_instance();
  
  pad->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  pad->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  pad->format = ags_soundcard_helper_config_get_format(config);

  pad->channel = NULL;
  
  pad->cols = 2;

  pad->line = NULL;
  
  pad->line_expander_set = ags_expander_set_new();
  gtk_box_append((GtkBox *) pad,
		 (GtkWidget *) pad->line_expander_set);

  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) hbox, FALSE, FALSE, 0);

  pad->group = (GtkToggleButton *) gtk_toggle_button_new_with_label("G");
  gtk_toggle_button_set_active(pad->group, TRUE);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) pad->group,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  pad->mute = (GtkToggleButton *) gtk_toggle_button_new_with_label("M");
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) pad->mute,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  pad->solo = (GtkToggleButton *) gtk_toggle_button_new_with_label("S");
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) pad->solo,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  pad->play = NULL;
}

void
ags_pad_set_property(GObject *gobject,
		     guint prop_id,
		     const GValue *value,
		     GParamSpec *param_spec)
{
  AgsPad *pad;

  pad = AGS_PAD(gobject);

  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      GList *start_list, *list;

      guint samplerate, old_samplerate;
      
      samplerate = g_value_get_uint(value);
      old_samplerate = pad->samplerate;

      if(samplerate == old_samplerate){
	return;
      }

      pad->samplerate = samplerate;

      ags_pad_samplerate_changed(pad,
				 samplerate, old_samplerate);

      list =
	start_list = gtk_container_get_children((GtkContainer *) pad->expander_set);

      while(list != NULL){
	if(AGS_LINE(list->data)){
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
      old_buffer_size = pad->buffer_size;

      if(buffer_size == old_buffer_size){
	return;
      }

      pad->buffer_size = buffer_size;

      ags_pad_buffer_size_changed(pad,
				  buffer_size, old_buffer_size);

      list =
	start_list = gtk_container_get_children((GtkContainer *) pad->expander_set);

      while(list != NULL){
	if(AGS_LINE(list->data)){
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
      old_format = pad->format;

      if(format == old_format){
	return;
      }

      pad->format = format;

      ags_pad_format_changed(pad,
			     format, old_format);

      list =
	start_list = gtk_container_get_children((GtkContainer *) pad->expander_set);

      while(list != NULL){
	if(AGS_LINE(list->data)){
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

      ags_pad_set_channel(pad, channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pad_get_property(GObject *gobject,
		     guint prop_id,
		     GValue *value,
		     GParamSpec *param_spec)
{
  AgsPad *pad;

  pad = AGS_PAD(gobject);

  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value,
		       pad->samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value,
		       pad->buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value,
		       pad->format);
    }
    break;
  case PROP_CHANNEL:
    {
      g_value_set_object(value, pad->channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pad_connect(AgsConnectable *connectable)
{
  AgsPad *pad;
  GList *line_list, *line_list_start;

  /* AgsPad */
  pad = AGS_PAD(connectable);

  if((AGS_PAD_CONNECTED & (pad->flags)) != 0){
    return;
  }
  
  pad->flags |= AGS_PAD_CONNECTED;

  if((AGS_PAD_PREMAPPED_RECALL & (pad->flags)) == 0){
    if((AGS_PAD_MAPPED_RECALL & (pad->flags)) == 0){
      ags_pad_map_recall(pad,
			 0);
    }
  }else{
    pad->flags &= (~AGS_PAD_PREMAPPED_RECALL);

    ags_pad_find_port(pad);
  }

  /* GtkButton */
  g_signal_connect_after((GObject *) pad->group, "clicked",
			 G_CALLBACK(ags_pad_group_clicked_callback), (gpointer) pad);

  g_signal_connect_after((GObject *) pad->mute, "clicked",
			 G_CALLBACK(ags_pad_mute_clicked_callback), (gpointer) pad);

  g_signal_connect_after((GObject *) pad->solo, "clicked",
			 G_CALLBACK(ags_pad_solo_clicked_callback), (gpointer) pad);

  /* AgsLine */
  line_list_start =  
    line_list = gtk_container_get_children(GTK_CONTAINER(pad->expander_set));

  while(line_list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(line_list->data));

    line_list = line_list->next;
  }

  g_list_free(line_list_start);
}

void
ags_pad_disconnect(AgsConnectable *connectable)
{
  AgsPad *pad;
  GList *line_list, *line_list_start;

  /* AgsPad */
  pad = AGS_PAD(connectable);

  if((AGS_PAD_CONNECTED & (pad->flags)) == 0){
    return;
  }
  
  pad->flags &= (~AGS_PAD_CONNECTED);

  /* AgsLine */
  line_list_start =  
    line_list = gtk_container_get_children(GTK_CONTAINER(pad->expander_set));

  while(line_list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(line_list->data));

    line_list = line_list->next;
  }

  g_list_free(line_list_start);

  g_signal_handlers_disconnect_by_data(pad->channel,
				       pad);
}

/**
 * ags_pad_samplerate_changed:
 * @pad: the #AgsPad
 * @samplerate: the samplerate
 * @old_samplerate: the old samplerate
 * 
 * Notify about samplerate changed.
 * 
 * Since: 3.0.0
 */
void
ags_pad_samplerate_changed(AgsPad *pad,
			   guint samplerate, guint old_samplerate)
{
  g_return_if_fail(AGS_IS_PAD(pad));

  g_object_ref((GObject *) pad);
  g_signal_emit(G_OBJECT(pad),
		pad_signals[SAMPLERATE_CHANGED], 0,
		samplerate,
		old_samplerate);
  g_object_unref((GObject *) pad);
}

/**
 * ags_pad_buffer_size_changed:
 * @pad: the #AgsPad
 * @buffer_size: the buffer_size
 * @old_buffer_size: the old buffer_size
 * 
 * Notify about buffer_size changed.
 * 
 * Since: 3.0.0
 */
void
ags_pad_buffer_size_changed(AgsPad *pad,
			    guint buffer_size, guint old_buffer_size)
{
  g_return_if_fail(AGS_IS_PAD(pad));

  g_object_ref((GObject *) pad);
  g_signal_emit(G_OBJECT(pad),
		pad_signals[BUFFER_SIZE_CHANGED], 0,
		buffer_size,
		old_buffer_size);
  g_object_unref((GObject *) pad);
}

/**
 * ags_pad_format_changed:
 * @pad: the #AgsPad
 * @format: the format
 * @old_format: the old format
 * 
 * Notify about format changed.
 * 
 * Since: 3.0.0
 */
void
ags_pad_format_changed(AgsPad *pad,
		       guint format, guint old_format)
{
  g_return_if_fail(AGS_IS_PAD(pad));

  g_object_ref((GObject *) pad);
  g_signal_emit(G_OBJECT(pad),
		pad_signals[FORMAT_CHANGED], 0,
		format,
		old_format);
  g_object_unref((GObject *) pad);
}

void
ags_pad_real_set_channel(AgsPad *pad, AgsChannel *channel)
{
  AgsChannel *current, *next_current;

  GList *line, *line_start;

  if(pad->channel == channel){
    return;
  }

  if(pad->channel != NULL){
    g_object_unref(G_OBJECT(pad->channel));
  }

  if(channel != NULL){
    g_object_ref(G_OBJECT(channel));
  }

  if(channel != NULL){
    pad->samplerate = channel->samplerate;
    pad->buffer_size = channel->buffer_size;
    pad->format = channel->format;
  }

  pad->channel = channel;

  line_start = 
    line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(pad)->expander_set));

  current = channel;

  if(current != NULL){
    g_object_ref(current);
  }

  next_current = NULL;
  
  /* set channel */
  while(line != NULL){
    g_object_set(G_OBJECT(line->data),
		 "channel", current,
		 NULL);

    /* iterate */
    if(current != NULL){
      next_current = ags_channel_next(current);

      g_object_unref(current);

      current = next_current;
    }
    
    line = line->next;
  }

  if(next_current != NULL){
    g_object_unref(next_current);
  }
  
  g_list_free(line_start);
}

/**
 * ags_pad_set_channel:
 * @pad: an #AgsPad
 * @channel: the #AgsChannel to set
 *
 * Is emitted as channel gets modified.
 *
 * Since: 3.0.0
 */
void
ags_pad_set_channel(AgsPad *pad, AgsChannel *channel)
{
  g_return_if_fail(AGS_IS_PAD(pad));

  g_object_ref((GObject *) pad);
  g_signal_emit(G_OBJECT(pad),
		pad_signals[SET_CHANNEL], 0,
		channel);
  g_object_unref((GObject *) pad);
}

/**
 * ags_pad_get_line:
 * @pad: the #AgsPad
 * 
 * Get bulk member of @pad.
 * 
 * Returns: the #GList-struct containing #AgsLine
 *
 * Since: 4.0.0
 */
GList*
ags_pad_get_line(AgsPad *pad)
{
  g_return_val_if_fail(AGS_IS_PAD(pad), NULL);

  return(g_list_reverse(g_list_copy(pad->line)));
}

/**
 * ags_pad_add_line:
 * @pad: the #AgsPad
 * @line: the #AgsLine
 * @x: the x position
 * @y: the y position
 * @width: the width
 * @height: the height
 * 
 * Add @line to @pad.
 * 
 * Since: 4.0.0
 */
void
ags_pad_add_line(AgsPad *pad,
		 AgsLine *line,
		 guint x, guint y,
		 guint width, guint height)
{
  g_return_if_fail(AGS_IS_PAD(pad));
  g_return_if_fail(AGS_IS_LINE(line));

  if(g_list_find(pad->line, line) == NULL){
    pad->line = g_list_prepend(pad->line,
			       line);
    
    ags_expander_set_add(pad->line_expander_set,
			 line,
			 x, y,
			 width, height);
  }
}

/**
 * ags_pad_remove_line:
 * @pad: the #AgsPad
 * @line: the #AgsLine
 * 
 * Remove @line from @pad.
 * 
 * Since: 4.0.0
 */
void
ags_pad_remove_line(AgsPad *pad,
		    AgsLine *line)
{
  g_return_if_fail(AGS_IS_PAD(pad));
  g_return_if_fail(AGS_IS_LINE(line));

  if(g_list_find(pad->line, line) != NULL){
    pad->line = g_list_remove(pad->line,
			      line);
    
    ags_expander_set_remove(pad->line_expander_set,
			    line);
  }
}

void
ags_pad_real_resize_lines(AgsPad *pad, GType line_type,
			  guint audio_channels, guint audio_channels_old)
{
  AgsLine *line;

  AgsAudio *audio;
  AgsChannel *channel;

  guint audio_audio_channels;
  guint i;

#ifdef AGS_DEBUG
  g_message("ags_pad_real_resize_lines: audio_channels = %u ; audio_channels_old = %u\n", audio_channels, audio_channels_old);
#endif

  audio = NULL;

  audio_audio_channels = 0;

  if(pad->channel != NULL){
    g_object_get(pad->channel,
		 "audio", &audio,
		 NULL);
  }
  
  if(audio != NULL){
    g_object_get(audio,
		 "audio-channels", &audio_audio_channels,
		 NULL);
  }
  
  /* resize */
  if(audio_channels > audio_channels_old){
    /* create AgsLine */
    for(i = audio_channels_old; i < audio_channels; i++){
      /* instantiate line */
      if(i < audio_audio_channels){
	channel = ags_channel_nth(pad->channel,
				  i);
      }else{
	channel = NULL;
      }
	
      line = (AgsLine *) g_object_new(line_type,
				      "pad", pad,
				      "channel", channel,
				      NULL);

      if(channel != NULL){
	channel->line_widget = (GObject *) line;
      }

      ags_expander_set_add(pad->expander_set,
			   (GtkWidget *) line,
			   i % pad->cols, floor(i / pad->cols),
			   1, 1);
	
      if(channel != NULL){
	g_object_unref(channel);
      }
    }
  }else if(audio_channels < audio_channels_old){
    GList *list, *list_start;

    list_start =
      list = g_list_nth(g_list_reverse(gtk_container_get_children(GTK_CONTAINER(pad->expander_set))),
			audio_channels);
    
    while(list != NULL){
      ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

      list = list->next;
    }

    list = list_start;

    while(list != NULL){
      gtk_widget_destroy(GTK_WIDGET(list->data));

      list = list->next;
    }

    g_list_free(list_start);
  }

  if(audio != NULL){
    g_object_unref(audio);
  }
}

/**
 * ags_pad_resize_lines:
 * @pad: the #AgsPad to resize
 * @line_type: channel type, either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
 * @audio_channels: count of lines
 * @audio_channels_old: old count of lines
 *
 * Resize the count of #AgsLine packe by #AgsPad.
 *
 * Since: 3.0.0
 */
void
ags_pad_resize_lines(AgsPad *pad, GType line_type,
		     guint audio_channels, guint audio_channels_old)
{
  g_return_if_fail(AGS_IS_PAD(pad));

  //  fprintf(stdout, "ags_pad_resize_lines: audio_channels = %u ; audio_channels_old = %u\n", audio_channels, audio_channels_old);

  g_object_ref((GObject *) pad);
  g_signal_emit(G_OBJECT(pad),
		pad_signals[RESIZE_LINES], 0,
		line_type,
		audio_channels, audio_channels_old);
  g_object_unref((GObject *) pad);
}

void
ags_pad_real_map_recall(AgsPad *pad, guint output_pad_start)
{
  if((AGS_PAD_MAPPED_RECALL & (pad->flags)) != 0){
    return;
  }
  
  pad->flags |= AGS_PAD_MAPPED_RECALL;

  ags_pad_find_port(pad);
}

/**
 * ags_pad_map_recall:
 * @pad: the #AgsPad to resize
 * @output_pad_start: start of output pad
 *
 * Start of output pad
 *
 * Since: 3.0.0
 */
void
ags_pad_map_recall(AgsPad *pad, guint output_pad_start)
{
  g_return_if_fail(AGS_IS_PAD(pad));

  g_object_ref((GObject *) pad);
  g_signal_emit(G_OBJECT(pad),
		pad_signals[MAP_RECALL], 0,
		output_pad_start);
  g_object_unref((GObject *) pad);
}

GList*
ags_pad_real_find_port(AgsPad *pad)
{
  GList *start_line, *line;
  
  GList *port, *tmp_port;

  port = NULL;

  /* find output ports */
  if(pad->expander_set != NULL){
    line =
      start_line = gtk_container_get_children((GtkContainer *) pad->expander_set);

    while(line != NULL){
      tmp_port = ags_line_find_port(AGS_LINE(line->data));
      
      if(port != NULL){
	port = g_list_concat(port,
			     tmp_port);
      }else{
	port = tmp_port;
      }

      line = line->next;
    }

    g_list_free(start_line);
  }

  return(port);
}

/**
 * ags_pad_find_port:
 * @pad: an #AgsPad
 *
 * Lookup ports of assigned recalls.
 *
 * Returns: an #GList containing all related #AgsPort
 *
 * Since: 3.0.0
 */
GList*
ags_pad_find_port(AgsPad *pad)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_PAD(pad),
		       NULL);

  g_object_ref((GObject *) pad);
  g_signal_emit((GObject *) pad,
		pad_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) pad);

  return(list);
}

void
ags_pad_play(AgsPad *pad)
{
  AgsMachine *machine;

  AgsChannel *channel;
  AgsChannel *next_pad, *next_channel;
  AgsPlayback *playback;
  
  GList *start_list, *list;

  gboolean play_all;

  if(!AGS_IS_PAD(pad)){
    return;
  }
  
  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pad,
						   AGS_TYPE_MACHINE);

  list = 
    start_list = gtk_container_get_children(GTK_CONTAINER(pad->expander_set));

  /*  */
  play_all = gtk_toggle_button_get_active(pad->group);
  
  if(gtk_toggle_button_get_active(pad->play)){
    if(play_all){
      channel = pad->channel;

      if(channel != NULL){
	g_object_ref(channel);
      }

      next_pad = ags_channel_next_pad(channel);
      next_channel = NULL;
      
      while(channel != next_pad){
	AgsNote *play_note;
	
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	g_object_get(playback,
		     "play-note", &play_note,
		     NULL);

	g_object_set(play_note,
		     "x0", 0,
		     "x1", 1,
		     NULL);

	ags_machine_playback_set_active(machine,
					playback,
					TRUE);
	
	g_object_unref(playback);
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      /* unref */
      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
      
      if(next_channel != NULL){
	g_object_unref(next_channel);
      }      
    }else{
      while((list = ags_line_find_next_grouped(list)) != NULL){
	AgsLine *line;
	
	line = AGS_LINE(list->data);

	channel = line->channel;
	
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	ags_machine_playback_set_active(machine,
					playback,
					TRUE);
	
	g_object_unref(playback);

	/* iterate */
	list = list->next;
      }
    }
  }else{
    if(play_all){
      channel = pad->channel;

      if(channel != NULL){
	g_object_ref(channel);
      }

      next_pad = ags_channel_next_pad(channel);
      next_channel = NULL;
      
      while(channel != next_pad){
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	ags_machine_playback_set_active(machine,
					playback,
					FALSE);
	
	g_object_unref(playback);
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      /* unref */
      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
      
      if(next_channel != NULL){
	g_object_unref(next_channel);
      }      
    }else{
      while((list = ags_line_find_next_grouped(list)) != NULL){
	AgsLine *line;
	
	line = AGS_LINE(list->data);

	channel = line->channel;
	
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	ags_machine_playback_set_active(machine,
					playback,
					FALSE);
	
	g_object_unref(playback);

	/* iterate */
	list = list->next;
      }
    }
  }
  
  g_list_free(start_list);
}
  
/**
 * ags_pad_new:
 * @channel: the bunch of channel to visualize
 *
 * Creates an #AgsPad
 *
 * Returns: a new #AgsPad
 *
 * Since: 3.0.0
 */
AgsPad*
ags_pad_new(AgsChannel *channel)
{
  AgsPad *pad;

  pad = (AgsPad *) g_object_new(AGS_TYPE_PAD, NULL);

  return(pad);
}
