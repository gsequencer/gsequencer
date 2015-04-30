/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/task/ags_set_output_device.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

void ags_set_output_device_class_init(AgsSetOutputDeviceClass *set_output_device);
void ags_set_output_device_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_set_output_device_init(AgsSetOutputDevice *set_output_device);
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
 * The #AgsSetOutputDevice task resets soundcard of #AgsDevout.
 */

static gpointer ags_set_output_device_parent_class = NULL;
static AgsConnectableInterface *ags_set_output_device_parent_connectable_interface;

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
							"AgsSetOutputDevice\0",
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

  ags_set_output_device_parent_class = g_type_class_peek_parent(set_output_device);

  /* gobject */
  gobject = (GObjectClass *) set_output_device;

  gobject->finalize = ags_set_output_device_finalize;

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
  set_output_device->devout = NULL;
  set_output_device->card_id = NULL;
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
  AgsDevout *devout;
  AgsSetOutputDevice *set_output_device;
  char *card_id;

  set_output_device = AGS_SET_OUTPUT_DEVICE(task);

  devout = AGS_DEVOUT(set_output_device->devout);
  card_id = set_output_device->card_id;

  /* perform task */
  g_object_set(G_OBJECT(devout),
	       "device\0", card_id,
	       NULL);
}

/**
 * ags_set_output_device_new:
 * @devout: the #AgsDevout to reset
 * @card_id: the new soundcard
 *
 * Creates an #AgsSetOutputDevice.
 *
 * Returns: an new #AgsSetOutputDevice.
 *
 * Since: 0.4
 */
AgsSetOutputDevice*
ags_set_output_device_new(GObject *devout,
			  char *card_id)
{
  AgsSetOutputDevice *set_output_device;

  set_output_device = (AgsSetOutputDevice *) g_object_new(AGS_TYPE_SET_OUTPUT_DEVICE,
							  NULL);

  set_output_device->devout = devout;
  set_output_device->card_id = card_id;

  return(set_output_device);
}
