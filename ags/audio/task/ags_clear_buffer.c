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

#include <ags/audio/task/ags_clear_buffer.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_midiin.h>

#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/audio/pulse/ags_pulse_devout.h>

#include <ags/i18n.h>

void ags_clear_buffer_class_init(AgsClearBufferClass *clear_buffer);
void ags_clear_buffer_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_clear_buffer_init(AgsClearBuffer *clear_buffer);
void ags_clear_buffer_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_clear_buffer_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_clear_buffer_connect(AgsConnectable *connectable);
void ags_clear_buffer_disconnect(AgsConnectable *connectable);
void ags_clear_buffer_dispose(GObject *gobject);
void ags_clear_buffer_finalize(GObject *gobject);

void ags_clear_buffer_launch(AgsTask *task);

/**
 * SECTION:ags_clear_buffer
 * @short_description: switch buffer flag of device
 * @title: AgsClearBuffer
 * @section_id:
 * @include: ags/audio/task/ags_clear_buffer.h
 *
 * The #AgsClearBuffer task switches the buffer flag of device.
 */

static gpointer ags_clear_buffer_parent_class = NULL;
static AgsConnectableInterface *ags_clear_buffer_parent_connectable_interface;

enum{
  PROP_0,
  PROP_DEVICE,
};

GType
ags_clear_buffer_get_type()
{
  static GType ags_type_clear_buffer = 0;

  if(!ags_type_clear_buffer){
    static const GTypeInfo ags_clear_buffer_info = {
      sizeof (AgsClearBufferClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_clear_buffer_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsClearBuffer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_clear_buffer_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_clear_buffer_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_clear_buffer = g_type_register_static(AGS_TYPE_TASK,
							 "AgsClearBuffer",
							 &ags_clear_buffer_info,
							 0);

    g_type_add_interface_static(ags_type_clear_buffer,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_clear_buffer);
}

void
ags_clear_buffer_class_init(AgsClearBufferClass *clear_buffer)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_clear_buffer_parent_class = g_type_class_peek_parent(clear_buffer);

  /* gobject */
  gobject = (GObjectClass *) clear_buffer;

  gobject->set_property = ags_clear_buffer_set_property;
  gobject->get_property = ags_clear_buffer_get_property;

  gobject->dispose = ags_clear_buffer_dispose;
  gobject->finalize = ags_clear_buffer_finalize;

  /* properties */
  /**
   * AgsClearBuffer:device:
   *
   * The assigned #AgsSoundcard or #AgsSequencer
   * 
   * Since: 0.7.124
   */
  param_spec = g_param_spec_object("device",
				   i18n_pspec("device of change device"),
				   i18n_pspec("The device of change device task"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) clear_buffer;

  task->launch = ags_clear_buffer_launch;
}

void
ags_clear_buffer_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_clear_buffer_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_clear_buffer_connect;
  connectable->disconnect = ags_clear_buffer_disconnect;
}

void
ags_clear_buffer_init(AgsClearBuffer *clear_buffer)
{
  clear_buffer->device = NULL;
}

void
ags_clear_buffer_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsClearBuffer *clear_buffer;

  clear_buffer = AGS_CLEAR_BUFFER(gobject);

  switch(prop_id){
  case PROP_DEVICE:
    {
      GObject *device;

      device = (GObject *) g_value_get_object(value);

      if(clear_buffer->device == (GObject *) device){
	return;
      }

      if(clear_buffer->device != NULL){
	g_object_unref(clear_buffer->device);
      }

      if(device != NULL){
	g_object_ref(device);
      }

      clear_buffer->device = (GObject *) device;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_clear_buffer_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsClearBuffer *clear_buffer;

  clear_buffer = AGS_CLEAR_BUFFER(gobject);

  switch(prop_id){
  case PROP_DEVICE:
    {
      g_value_set_object(value, clear_buffer->device);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_clear_buffer_connect(AgsConnectable *connectable)
{
  ags_clear_buffer_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_clear_buffer_disconnect(AgsConnectable *connectable)
{
  ags_clear_buffer_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_clear_buffer_dispose(GObject *gobject)
{
  AgsClearBuffer *clear_buffer;

  clear_buffer = AGS_CLEAR_BUFFER(gobject);

  if(clear_buffer->device != NULL){
    g_object_unref(clear_buffer->device);

    clear_buffer->device = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_clear_buffer_parent_class)->dispose(gobject);
}

void
ags_clear_buffer_finalize(GObject *gobject)
{
  AgsClearBuffer *clear_buffer;

  clear_buffer = AGS_CLEAR_BUFFER(gobject);

  if(clear_buffer->device != NULL){
    g_object_unref(clear_buffer->device);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_clear_buffer_parent_class)->finalize(gobject);
}

void
ags_clear_buffer_launch(AgsTask *task)
{
  AgsClearBuffer *clear_buffer;

  guint nth_buffer;
  guint word_size;
  
  clear_buffer = AGS_CLEAR_BUFFER(task);

  if(AGS_IS_DEVOUT(clear_buffer->device)){
    AgsDevout *devout;

    devout = clear_buffer->device;

    /* retrieve word size */
    switch(devout->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	word_size = sizeof(signed char);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	word_size = sizeof(signed short);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	word_size = sizeof(signed long);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	word_size = sizeof(signed long);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_64_BIT:
      {
	word_size = sizeof(signed long long);
      }
      break;
    default:
      g_warning("ags_clear_buffer_launch(): unsupported word size");

      return;
    }
    
    if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
      nth_buffer = 0;
    }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
      nth_buffer = 1;
    }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
      nth_buffer = 2;
    }else if((AGS_DEVOUT_BUFFER3 & (devout->flags)) != 0){
      nth_buffer = 3;
    }
    
    memset(devout->buffer[nth_buffer], 0, (size_t) devout->pcm_channels * devout->buffer_size * word_size);
  }else if(AGS_IS_JACK_DEVOUT(clear_buffer->device)){
    AgsJackDevout *jack_devout;
    
    jack_devout = clear_buffer->device;

    switch(jack_devout->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	word_size = sizeof(signed char);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	word_size = sizeof(signed short);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {      
	//NOTE:JK: The 24-bit linear samples use 32-bit physical space
	word_size = sizeof(signed long);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	word_size = sizeof(signed long);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_64_BIT:
      {
	word_size = sizeof(signed long long);
      }
      break;
    default:    
      g_warning("ags_clear_buffer_launch(): unsupported word size");
    
      return;
    }
    
    if((AGS_JACK_DEVOUT_BUFFER0 & (jack_devout->flags)) != 0){
      nth_buffer = 3;
    }else if((AGS_JACK_DEVOUT_BUFFER1 & (jack_devout->flags)) != 0){
      nth_buffer = 0;
    }else if((AGS_JACK_DEVOUT_BUFFER2 & (jack_devout->flags)) != 0){
      nth_buffer = 1;
    }else if((AGS_JACK_DEVOUT_BUFFER3 & (jack_devout->flags)) != 0){
      nth_buffer = 2;
    }
      
    memset(jack_devout->buffer[nth_buffer], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  }else if(AGS_IS_PULSE_DEVOUT(clear_buffer->device)){
    AgsPulseDevout *pulse_devout;
    
    pulse_devout = clear_buffer->device;

    switch(pulse_devout->format){
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	word_size = sizeof(signed short);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {      
	//NOTE:JK: The 24-bit linear samples use 32-bit physical space
	word_size = sizeof(signed long);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	word_size = sizeof(signed long);
      }
      break;
    default:    
      g_warning("ags_clear_buffer_launch(): unsupported word size");
    
      return;
    }
    
    if((AGS_PULSE_DEVOUT_BUFFER0 & (pulse_devout->flags)) != 0){
      nth_buffer = 2;
    }else if((AGS_PULSE_DEVOUT_BUFFER1 & (pulse_devout->flags)) != 0){
      nth_buffer = 3;
    }else if((AGS_PULSE_DEVOUT_BUFFER2 & (pulse_devout->flags)) != 0){
      nth_buffer = 4;
    }else if((AGS_PULSE_DEVOUT_BUFFER3 & (pulse_devout->flags)) != 0){
      nth_buffer = 5;
    }else if((AGS_PULSE_DEVOUT_BUFFER4 & (pulse_devout->flags)) != 0){
      nth_buffer = 6;
    }else if((AGS_PULSE_DEVOUT_BUFFER5 & (pulse_devout->flags)) != 0){
      nth_buffer = 7;
    }else if((AGS_PULSE_DEVOUT_BUFFER6 & (pulse_devout->flags)) != 0){
      nth_buffer = 0;
    }else if((AGS_PULSE_DEVOUT_BUFFER7 & (pulse_devout->flags)) != 0){
      nth_buffer = 1;
    }
      
    memset(pulse_devout->buffer[nth_buffer], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  }else if(AGS_IS_MIDIIN(clear_buffer->device)){
    //TODO:JK: implement me
  }else if(AGS_IS_JACK_MIDIIN(clear_buffer->device)){
    //TODO:JK: implement me
  }
}

/**
 * ags_clear_buffer_new:
 * @device: the #AgsSoundcard or #AgsSequencer
 *
 * Creates an #AgsClearBuffer.
 *
 * Returns: an new #AgsClearBuffer.
 *
 * Since: 0.7.124
 */
AgsClearBuffer*
ags_clear_buffer_new(GObject *device)
{
  AgsClearBuffer *clear_buffer;

  clear_buffer = (AgsClearBuffer *) g_object_new(AGS_TYPE_CLEAR_BUFFER,
						 "device", device,
						 NULL);

  return(clear_buffer);
}
