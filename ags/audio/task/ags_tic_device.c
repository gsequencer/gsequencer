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

#include <ags/audio/task/ags_tic_device.h>

#include <ags/object/ags_connectable.h>

#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

void ags_tic_device_class_init(AgsTicDeviceClass *tic_device);
void ags_tic_device_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_tic_device_init(AgsTicDevice *tic_device);
void ags_tic_device_connect(AgsConnectable *connectable);
void ags_tic_device_disconnect(AgsConnectable *connectable);
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
static AgsConnectableInterface *ags_tic_device_parent_connectable_interface;

GType
ags_tic_device_get_type()
{
  static GType ags_type_tic_device = 0;

  if(!ags_type_tic_device){
    static const GTypeInfo ags_tic_device_info = {
      sizeof (AgsTicDeviceClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_tic_device_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTicDevice),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_tic_device_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_tic_device_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_tic_device = g_type_register_static(AGS_TYPE_TASK,
						 "AgsTicDevice\0",
						 &ags_tic_device_info,
						 0);

    g_type_add_interface_static(ags_type_tic_device,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_tic_device);
}

void
ags_tic_device_class_init(AgsTicDeviceClass *tic_device)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_tic_device_parent_class = g_type_class_peek_parent(tic_device);

  /* gobject */
  gobject = (GObjectClass *) tic_device;

  gobject->finalize = ags_tic_device_finalize;

  /* task */
  task = (AgsTaskClass *) tic_device;

  task->launch = ags_tic_device_launch;
}

void
ags_tic_device_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_tic_device_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_tic_device_connect;
  connectable->disconnect = ags_tic_device_disconnect;
}

void
ags_tic_device_init(AgsTicDevice *tic_device)
{
  tic_device->device = NULL;
}

void
ags_tic_device_connect(AgsConnectable *connectable)
{
  ags_tic_device_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_tic_device_disconnect(AgsConnectable *connectable)
{
  ags_tic_device_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_tic_device_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_tic_device_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_tic_device_launch(AgsTask *task)
{
  AgsTicDevice *tic_device;

  tic_device = AGS_TIC_DEVICE(task);

  if(AGS_IS_SOUNDCARD(tic_device->device)){
    AgsSoundcardInterface *soundcard_interface;
    
    soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(AGS_SOUNDCARD(tic_device->device));
  
    soundcard_interface->tic(AGS_SOUNDCARD(tic_device->device));
  }else if(AGS_IS_SEQUENCER(AGS_SEQUENCER(tic_device->device))){
    AgsSequencerInterface *sequencer_interface;
    
    sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(AGS_SEQUENCER(tic_device->device));
  
    sequencer_interface->tic(AGS_SEQUENCER(tic_device->device));
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
 * Since: 0.7.105
 */
AgsTicDevice*
ags_tic_device_new(GObject *device)
{
  AgsTicDevice *tic_device;

  tic_device = (AgsTicDevice *) g_object_new(AGS_TYPE_TIC_DEVICE,
					     NULL);

  tic_device->device = device;

  return(tic_device);
}
