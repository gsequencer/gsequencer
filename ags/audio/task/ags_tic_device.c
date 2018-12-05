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

#include <ags/audio/task/ags_tic_device.h>

#include <ags/i18n.h>

void ags_tic_device_class_init(AgsTicDeviceClass *tic_device);
void ags_tic_device_init(AgsTicDevice *tic_device);
void ags_tic_device_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_tic_device_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_tic_device_dispose(GObject *gobject);
void ags_tic_device_finalize(GObject *gobject);

void ags_tic_device_launch(AgsTask *task);

/**
 * SECTION:ags_tic_device
 * @short_description: tic device object
 * @title: AgsTicDevice
 * @section_id:
 * @include: ags/audio/task/ags_tic_device.h
 *
 * The #AgsTicDevice task tics #GObject.
 */

static gpointer ags_tic_device_parent_class = NULL;

enum{
  PROP_0,
  PROP_DEVICE,
};

GType
ags_tic_device_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_tic_device = 0;

    static const GTypeInfo ags_tic_device_info = {
      sizeof(AgsTicDeviceClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_tic_device_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsTicDevice),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_tic_device_init,
    };

    ags_type_tic_device = g_type_register_static(AGS_TYPE_TASK,
						 "AgsTicDevice",
						 &ags_tic_device_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_tic_device);
  }

  return g_define_type_id__volatile;
}

void
ags_tic_device_class_init(AgsTicDeviceClass *tic_device)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_tic_device_parent_class = g_type_class_peek_parent(tic_device);

  /* gobject */
  gobject = (GObjectClass *) tic_device;

  gobject->set_property = ags_tic_device_set_property;
  gobject->get_property = ags_tic_device_get_property;

  gobject->dispose = ags_tic_device_dispose;
  gobject->finalize = ags_tic_device_finalize;

  /* properties */
  /**
   * AgsTicDevice:device:
   *
   * The assigned #GObject as device.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("device",
				   i18n_pspec("device to tic"),
				   i18n_pspec("The device to tic"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);  

  /* task */
  task = (AgsTaskClass *) tic_device;

  task->launch = ags_tic_device_launch;
}

void
ags_tic_device_init(AgsTicDevice *tic_device)
{
  tic_device->device = NULL;
}

void
ags_tic_device_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsTicDevice *tic_device;

  tic_device = AGS_TIC_DEVICE(gobject);

  switch(prop_id){
  case PROP_DEVICE:
    {
      GObject *device;

      device = (GObject *) g_value_get_object(value);

      if(tic_device->device == (GObject *) device){
	return;
      }

      if(tic_device->device != NULL){
	g_object_unref(tic_device->device);
      }

      if(device != NULL){
	g_object_ref(device);
      }

      tic_device->device = (GObject *) device;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_tic_device_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsTicDevice *tic_device;

  tic_device = AGS_TIC_DEVICE(gobject);

  switch(prop_id){
  case PROP_DEVICE:
    {
      g_value_set_object(value, tic_device->device);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_tic_device_dispose(GObject *gobject)
{
  AgsTicDevice *tic_device;

  tic_device = AGS_TIC_DEVICE(gobject);

  if(tic_device->device != NULL){
    g_object_unref(tic_device->device);

    tic_device->device = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_tic_device_parent_class)->dispose(gobject);
}

void
ags_tic_device_finalize(GObject *gobject)
{
  AgsTicDevice *tic_device;

  tic_device = AGS_TIC_DEVICE(gobject);

  if(tic_device->device != NULL){
    g_object_unref(tic_device->device);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_tic_device_parent_class)->finalize(gobject);
}

void
ags_tic_device_launch(AgsTask *task)
{
  AgsTicDevice *tic_device;

  tic_device = AGS_TIC_DEVICE(task);

  //FIXME:JK: configure realtime disable event
  if(AGS_IS_SOUNDCARD(tic_device->device)){
    AgsSoundcardInterface *soundcard_interface;
    
    soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(tic_device->device));
  
    //soundcard_interface->tic(AGS_SOUNDCARD(tic_device->device));
    ags_soundcard_tic(AGS_SOUNDCARD(tic_device->device));
  }else if(AGS_IS_SEQUENCER(AGS_SEQUENCER(tic_device->device))){
    AgsSequencerInterface *sequencer_interface;
    
    sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(tic_device->device));
  
    //sequencer_interface->tic(AGS_SEQUENCER(tic_device->device));
    ags_sequencer_tic(AGS_SEQUENCER(tic_device->device));
  }
}

/**
 * ags_tic_device_new:
 * @device: the #GObject to tic
 *
 * Creates an #AgsTicDevice.
 *
 * Returns: an new #AgsTicDevice.
 *
 * Since: 2.0.0
 */
AgsTicDevice*
ags_tic_device_new(GObject *device)
{
  AgsTicDevice *tic_device;

  tic_device = (AgsTicDevice *) g_object_new(AGS_TYPE_TIC_DEVICE,
					     "device", device,
					     NULL);

  return(tic_device);
}
