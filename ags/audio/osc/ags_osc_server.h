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

#ifndef __AGS_OSC_SERVER_H__
#define __AGS_OSC_SERVER_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_OSC_SERVER                (ags_osc_server_get_type ())
#define AGS_OSC_SERVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_SERVER, AgsOscServer))
#define AGS_OSC_SERVER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OSC_SERVER, AgsOscServerClass))
#define AGS_IS_OSC_SERVER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_SERVER))
#define AGS_IS_OSC_SERVER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_SERVER))
#define AGS_OSC_SERVER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OSC_SERVER, AgsOscServerClass))

typedef struct _AgsOscServer AgsOscServer;
typedef struct _AgsOscServerClass AgsOscServerClass;

struct _AgsOscServer
{
  GObject gobject;

  guint flags;

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;
};

struct _AgsOscServerClass
{
  GObjectClass gobject;

  void (*start)(AgsOscServer *osc_server);
};

GType ags_osc_server_get_type(void);

pthread_mutex_t* ags_osc_server_get_class_mutex();

void ags_osc_server_start(AgsOscServer *osc_server);

AgsOscServer* ags_osc_server_new();

#endif /*__AGS_OSC_SERVER_H__*/
