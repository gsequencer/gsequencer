/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/task/ags_set_output_device.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_devout.h>

void ags_set_output_device_class_init(AgsSetOutputDeviceClass *set_output_device);
void ags_set_output_device_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_set_output_device_init(AgsSetOutputDevice *set_output_device);
void ags_set_output_device_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_set_output_device_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_set_output_device_connect(AgsConnectable *connectable);
void ags_set_output_device_disconnect(AgsConnectable *connectable);
void ags_set_output_device_finalize(GObject *gobject);

void ags_set_output_device_launch(AgsTask *task);

/**
 * SECTION:ags_set_output_device
 * @short_description: resets soundcard
 * @title: AgsSetOutputDevice
 * @section_id:
 * @include: ags/audio/task/ags_set_output_device.h
 *
 * The #AgsSetOutputDevice task resets soundcard of #AgsSoundcard.
 */

static gpointer ags_set_output_device_parent_class = NULL;
static AgsConnectableInterface *ags_set_output_device_parent_connectable_interface;

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_DEVICE,
};

GType
ags_set_output_device_get_type()
{
  static GType ags_type_set_output_device = 0;

  if(!ags_type_set_output_device){
    static const GTypeInfo ags_set_output_device_info = {
      sizeof (AgsSetOutputDeviceClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_set_output_device_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSetOutputDevice),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_set_output_device_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_set_output_device_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_set_output_device = g_type_register_static(AGS_TYPE_TASK,
							"AgsSetOutputDevice",
							&ags_set_output_device_info,
							0);
    
    g_type_add_interface_static(ags_type_set_output_device,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_set_output_device);
}

void
ags_set_output_device_class_init(AgsSetOutputDeviceClass *set_output_device)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_set_output_device_parent_class = g_type_class_peek_parent(set_output_device);

  /* gobject */
  gobject = (GObjectClass *) set_output_device;

  gobject->set_property = ags_set_output_device_set_property;
  gobject->get_property = ags_set_output_device_get_property;

  gobject->finalize = ags_set_output_device_finalize;

  /* properties */
  /**
   * AgsSetOutputDevice:soundcard:
   *
   * The assigned #AgsSoundcard instance.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("soundcard",
				   "soundcard of set audio channels",
				   "The soundcard of set audio channels",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsSetOutputDevice:device:
   *
   * The soundcard indentifier
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_string("device",
				   "device identifier",
				   "The device identifier to set",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) set_output_device;

  task->launch = ags_set_output_device_launch;
}

void
ags_set_output_device_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_set_output_device_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_set_output_device_connect;
  connectable->disconnect = ags_set_output_device_disconnect;
}

void
ags_set_output_device_init(AgsSetOutputDevice *set_output_device)
{
  set_output_device->soundcard = NULL;
  set_output_device->device = NULL;
}

void
ags_set_output_device_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsSetOutputDevice *set_output_device;

  set_output_device = AGS_SET_OUTPUT_DEVICE(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      if(set_output_device->soundcard == (GObject *) soundcard){
	return;
      }

      if(set_output_device->soundcard != NULL){
	g_object_unref(set_output_device->soundcard);
      }

      if(soundcard != NULL){
	g_object_ref(soundcard);
      }

      set_output_device->soundcard = (GObject *) soundcard;
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      if(device == set_output_device->device){
	return;
      }

      if(set_output_device->device != NULL){
	g_free(set_output_device->device);
      }

      set_output_device->device = g_strdup(device);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_output_device_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsSetOutputDevice *set_output_device;

  set_output_device = AGS_SET_OUTPUT_DEVICE(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, set_output_device->soundcard);
    }
    break;
  case PROP_DEVICE:
    {
      g_value_set_string(value, set_output_device->device);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_output_device_connect(AgsConnectable *connectable)
{
  ags_set_output_device_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_set_output_device_disconnect(AgsConnectable *connectable)
{
  ags_set_output_device_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_set_output_device_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_set_output_device_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_set_output_device_launch(AgsTask *task)
{
  GObject *soundcard;
  AgsSetOutputDevice *set_output_device;
  char *device;

  set_output_device = AGS_SET_OUTPUT_DEVICE(task);

  soundcard = set_output_device->soundcard;
  device = set_output_device->device;

  if(AGS_IS_DEVOUT(soundcard) &&
     (AGS_DEVOUT_ALSA & (AGS_DEVOUT(soundcard)->flags)) != 0){
    if(index(device, ',') == NULL){
      gchar *tmp;
    
      tmp = g_strdup_printf("%s,0",
			    device);
      
      g_free(device);
      device = tmp;
    }
  }
  
  /* perform task */
  ags_soundcard_set_device(AGS_SOUNDCARD(soundcard),
			   device);
}

/**
 * ags_set_output_device_new:
 * @soundcard: the #AgsSoundcard to reset
 * @device: the new soundcard
 *
 * Creates an #AgsSetOutputDevice.
 *
 * Returns: an new #AgsSetOutputDevice.
 *
 * Since: 0.4
 */
AgsSetOutputDevice*
ags_set_output_device_new(GObject *soundcard,
			  char *device)
{
  AgsSetOutputDevice *set_output_device;

  set_output_device = (AgsSetOutputDevice *) g_object_new(AGS_TYPE_SET_OUTPUT_DEVICE,
							  NULL);

  set_output_device->soundcard = soundcard;
  set_output_device->device = device;

  return(set_output_device);
}
