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

#include <ags/audio/ags_buffer.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>

#include <ags/i18n.h>

#include <stdlib.h>

void ags_buffer_class_init(AgsBufferClass *buffer);
void ags_buffer_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_buffer_init(AgsBuffer *buffer);
void ags_buffer_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_buffer_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_buffer_connect(AgsConnectable *connectable);
void ags_buffer_disconnect(AgsConnectable *connectable);
void ags_buffer_finalize(GObject *gobject);

/**
 * SECTION:ags_buffer
 * @short_description: Buffer class.
 * @title: AgsBuffer
 * @section_id:
 * @include: ags/audio/ags_buffer.h
 *
 * #AgsBuffer represents a tone.
 */

static gpointer ags_buffer_parent_class = NULL;

enum{
  PROP_0,
  PROP_X,
  PROP_SELECTION_X0,
  PROP_SELECTION_X1,
  PROP_FORMAT,
  PROP_BUFFER_LENGTH,
  PROP_DATA,
};

GType
ags_buffer_get_type()
{
  static GType ags_type_buffer = 0;

  if(!ags_type_buffer){
    static const GTypeInfo ags_buffer_info = {
      sizeof(AgsBufferClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_buffer_class_init,
      NULL,
      NULL,
      sizeof(AgsBuffer),
      0,
      (GInstanceInitFunc) ags_buffer_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_buffer_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_buffer = g_type_register_static(G_TYPE_OBJECT,
					     "AgsBuffer",
					     &ags_buffer_info,
					     0);
    
    g_type_add_interface_static(ags_type_buffer,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_buffer);
}

void 
ags_buffer_class_init(AgsBufferClass *buffer)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_buffer_parent_class = g_type_class_peek_parent(buffer);

  gobject = (GObjectClass *) buffer;

  gobject->set_property = ags_buffer_set_property;
  gobject->get_property = ags_buffer_get_property;

  gobject->finalize = ags_buffer_finalize;

  /* properties */
  /**
   * AgsBuffer:x:
   *
   * Buffer's x offset.
   * 
   * Since: 1.4.0
   */
  param_spec = g_param_spec_uint64("x",
				   i18n_pspec("offset x"),
				   i18n_pspec("The x offset"),
				   0,
				   G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X,
				  param_spec);

  /**
   * AgsBuffer:selection-x0:
   *
   * Buffer's selection x0 offset.
   * 
   * Since: 1.4.0
   */
  param_spec = g_param_spec_uint64("selection-x0",
				   i18n_pspec("selection offset x0"),
				   i18n_pspec("The selection's x0 offset"),
				   0,
				   G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SELECTION_X0,
				  param_spec);

  /**
   * AgsBuffer:selection-x1:
   *
   * Buffer's selection x1 offset.
   * 
   * Since: 1.4.0
   */
  param_spec = g_param_spec_uint64("selection-x1",
				   i18n_pspec("selection offset x1"),
				   i18n_pspec("The selection's x1 offset"),
				   0,
				   G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SELECTION_X1,
				  param_spec);

  /**
   * AgsBuffer:format:
   *
   * Buffer's audio data format.
   * 
   * Since: 1.4.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("audio data format"),
				 i18n_pspec("The format of audio data"),
				 0,
				 G_MAXUINT,
				 AGS_SOUNDCARD_DEFAULT_FORMAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsBuffer:buffer-length:
   *
   * Buffer's audio data buffer length.
   * 
   * Since: 1.4.0
   */
  param_spec = g_param_spec_uint("buffer-length",
				 i18n_pspec("audio data's buffer length"),
				 i18n_pspec("The buffer length of audio data"),
				 0,
				 G_MAXUINT,
				 AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_LENGTH,
				  param_spec);

  /**
   * AgsBuffer:data:
   *
   * Buffer's audio data.
   * 
   * Since: 1.4.0
   */
  param_spec = g_param_spec_pointer("data",
				    i18n_pspec("audio data"),
				    i18n_pspec("The audio data"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DATA,
				  param_spec);
}

void
ags_buffer_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_buffer_connect;
  connectable->disconnect = ags_buffer_disconnect;
}

void
ags_buffer_init(AgsBuffer *buffer)
{  
  buffer->flags = 0;

  buffer->x = 0;

  buffer->selection_x0 = 0;
  buffer->selection_x1 = 0;

  buffer->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  buffer->buffer_length = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  buffer->data = ags_stream_alloc(buffer->buffer_length,
				  buffer->format);
}

void
ags_buffer_connect(AgsConnectable *connectable)
{
  AgsBuffer *buffer;

  buffer = AGS_BUFFER(connectable);

  if((AGS_BUFFER_CONNECTED & (buffer->flags)) != 0){
    return;
  }

  buffer->flags |= AGS_BUFFER_CONNECTED;
}

void
ags_buffer_disconnect(AgsConnectable *connectable)
{
  AgsBuffer *buffer;

  buffer = AGS_BUFFER(connectable);

  if((AGS_BUFFER_CONNECTED & (buffer->flags)) == 0){
    return;
  }

  buffer->flags &= (~AGS_BUFFER_CONNECTED);
}

void
ags_buffer_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsBuffer *buffer;

  buffer = AGS_BUFFER(gobject);

  switch(prop_id){
  case PROP_X:
    {
      buffer->x = g_value_get_uint64(value);
    }
    break;
  case PROP_SELECTION_X0:
    {
      buffer->selection_x0 = g_value_get_uint64(value);
    }
    break;
  case PROP_SELECTION_X1:
    {
      buffer->selection_x1 = g_value_get_uint64(value);
    }
    break;
  case PROP_FORMAT:
    {
      buffer->format = g_value_get_uint(value);
    }
    break;
  case PROP_BUFFER_LENGTH:
    {
      buffer->buffer_length = g_value_get_uint(value);
    }
    break;
  case PROP_DATA:
    {
      buffer->data = g_value_get_pointer(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_buffer_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsBuffer *buffer;

  buffer = AGS_BUFFER(gobject);

  switch(prop_id){
  case PROP_X:
    {
      g_value_set_uint64(value,
			 buffer->x);
    }
    break;
  case PROP_SELECTION_X0:
    {
      g_value_set_uint64(value,
			 buffer->selection_x0);
    }
    break;
  case PROP_SELECTION_X1:
    {
      g_value_set_uint64(value,
			 buffer->selection_x1);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value,
		       buffer->format);
    }
    break;
  case PROP_BUFFER_LENGTH:
    {
      g_value_set_uint(value,
		       buffer->buffer_length);
    }
    break;
  case PROP_DATA:
    {
      g_value_set_pointer(value,
			  buffer->data);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_buffer_finalize(GObject *gobject)
{
  AgsBuffer *buffer;

  buffer = AGS_BUFFER(gobject);

  if(buffer->data != NULL){
    free(buffer->data);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_buffer_parent_class)->finalize(gobject);
}

/**
 * ags_buffer_new:
 *
 * Creates an #AgsBuffer
 *
 * Returns: a new #AgsBuffer
 *
 * Since: 1.4.0
 */
AgsBuffer*
ags_buffer_new()
{
  AgsBuffer *buffer;

  buffer = (AgsBuffer *) g_object_new(AGS_TYPE_BUFFER,
				      NULL);

  return(buffer);
}

