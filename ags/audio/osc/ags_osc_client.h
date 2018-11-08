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

#ifndef __AGS_OSC_CLIENT_H__
#define __AGS_OSC_CLIENT_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_OSC_CLIENT                (ags_osc_client_get_type ())
#define AGS_OSC_CLIENT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_CLIENT, AgsOscClient))
#define AGS_OSC_CLIENT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OSC_CLIENT, AgsOscClientClass))
#define AGS_IS_OSC_CLIENT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_CLIENT))
#define AGS_IS_OSC_CLIENT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_CLIENT))
#define AGS_OSC_CLIENT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OSC_CLIENT, AgsOscClientClass))

typedef struct _AgsOscClient AgsOscClient;
typedef struct _AgsOscClientClass AgsOscClientClass;

struct _AgsOscClient
{
  GObject gobject;

  guint flags;

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;
};

struct _AgsOscClientClass
{
  GObjectClass gobject;

  void (*connect)(AgsOscClient *osc_client);
};

GType ags_osc_client_get_type(void);

pthread_mutex_t* ags_osc_client_get_class_mutex();

void ags_osc_client_connect(AgsOscClient *osc_client);

AgsOscClient* ags_osc_client_new();

#endif /*__AGS_OSC_CLIENT_H__*/
