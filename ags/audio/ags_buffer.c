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

#include <ags/object/ags_connectable.h>

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

