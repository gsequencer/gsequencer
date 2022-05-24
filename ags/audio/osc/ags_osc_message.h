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

#ifndef __AGS_OSC_MESSAGE_H__
#define __AGS_OSC_MESSAGE_H__

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

#include <ags/libags.h>

#include <ags/audio/osc/ags_osc_connection.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_MESSAGE                (ags_osc_message_get_type ())
#define AGS_OSC_MESSAGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_MESSAGE, AgsOscMessage))
#define AGS_OSC_MESSAGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OSC_MESSAGE, AgsOscMessageClass))
#define AGS_IS_OSC_MESSAGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_MESSAGE))
#define AGS_IS_OSC_MESSAGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_MESSAGE))
#define AGS_OSC_MESSAGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OSC_MESSAGE, AgsOscMessageClass))

#define AGS_OSC_MESSAGE_GET_OBJ_MUTEX(obj) (&(((AgsOscMessage *) obj)->obj_mutex))

typedef struct _AgsOscMessage AgsOscMessage;
typedef struct _AgsOscMessageClass AgsOscMessageClass;

struct _AgsOscMessage
{
  GObject gobject;

  guint flags;
  guint connectable_flags;
  
  GRecMutex obj_mutex;

  AgsOscConnection *osc_connection;
  
  gint32 tv_sec;
  gint32 tv_fraction;
  gboolean immediately;

  guint message_size;

  guchar *message;
};

struct _AgsOscMessageClass
{
  GObjectClass gobject;
};

GType ags_osc_message_get_type(void);

gint ags_osc_message_sort_func(gconstpointer a,
			       gconstpointer b);

/* instance */
AgsOscMessage* ags_osc_message_new();

G_END_DECLS

#endif /*__AGS_OSC_MESSAGE_H__*/
