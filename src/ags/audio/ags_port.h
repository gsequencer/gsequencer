/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_PORT_H__
#define __AGS_PORT_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_PORT                (ags_port_get_type())
#define AGS_PORT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PORT, AgsPort))
#define AGS_PORT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PORT, AgsPortClass))
#define AGS_IS_PORT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PORT))
#define AGS_IS_PORT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PORT))
#define AGS_PORT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PORT, AgsPortClass))

typedef struct _AgsPort AgsPort;
typedef struct _AgsPortClass AgsPortClass;

struct _AgsPort
{
  GObject object;
};

struct _AgsPortClass
{
  GObjectClass object;
};

GType ags_port_get_type();

AgsPort* ags_port_new();

#endif /*__AGS_PORT_H__*/
