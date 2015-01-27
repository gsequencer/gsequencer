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

#include <pthread.h>

#include <ladspa.h>

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

  gchar *plugin_name;
  gchar *specifier;

  gchar *control_port;

  gboolean port_value_is_pointer;
  GType port_value_type;

  guint port_value_size;
  guint port_value_length;

  pthread_mutex_t mutex;

  union _AgsPortValue{
    gboolean ags_port_boolean;
    gint64 ags_port_int;
    guint64 ags_port_uint;
    gfloat ags_port_float;
    gdouble ags_port_double;
    gboolean *ags_port_boolean_ptr;
    gint64 *ags_port_int_ptr;
    guint64 *ags_port_uint_ptr;
    gfloat *ags_port_float_ptr;
    gdouble *ags_port_double_ptr;
    gpointer ags_port_pointer;
    GObject *ags_port_object;
  }port_value;
};

struct _AgsPortClass
{
  GObjectClass object;

  void (*safe_read)(AgsPort *port, GValue *value);
  void (*safe_write)(AgsPort *port, GValue *value);

  void (*safe_get_property)(AgsPort *port, gchar *property_name, GValue *value);
  void (*safe_set_property)(AgsPort *port, gchar *property_name, GValue *value);
};

GType ags_port_get_type();

void ags_port_safe_read(AgsPort *port, GValue *value);
void ags_port_safe_write(AgsPort *port, GValue *value);

void ags_port_safe_get_property(AgsPort *port, gchar *property_name, GValue *value);
void ags_port_safe_set_property(AgsPort *port, gchar *property_name, GValue *value);

GList* ags_port_find_specifier(GList *port, gchar *specifier);

AgsPort* ags_port_new();

#endif /*__AGS_PORT_H__*/
