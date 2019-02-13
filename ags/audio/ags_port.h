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

#ifndef __AGS_PORT_H__
#define __AGS_PORT_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>

#include <ladspa.h>

#include <ags/libags.h>

#define AGS_TYPE_PORT                (ags_port_get_type())
#define AGS_PORT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PORT, AgsPort))
#define AGS_PORT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PORT, AgsPortClass))
#define AGS_IS_PORT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PORT))
#define AGS_IS_PORT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PORT))
#define AGS_PORT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PORT, AgsPortClass))

typedef struct _AgsPort AgsPort;
typedef struct _AgsPortClass AgsPortClass;

/**
 * AgsPortFlags:
 * @AGS_PORT_ADDED_TO_REGISTRY: add to registry
 * @AGS_PORT_CONNECTED: indicates the port was connected by calling #AgsConnectable::connect()
 * @AGS_PORT_CONVERT_ALWAYS: convert always
 * @AGS_PORT_USE_LADSPA_FLOAT: use ladspa float
 * @AGS_PORT_IS_OUTPUT: is output
 * @AGS_PORT_INFINITE_RANGE: infinite range
 *
 * Enum values to control the behavior or indicate internal state of #AgsPort by
 * enable/disable as flags.
 */
typedef enum{
  AGS_PORT_ADDED_TO_REGISTRY  = 1,
  AGS_PORT_CONNECTED          = 1 <<  1,
  AGS_PORT_CONVERT_ALWAYS     = 1 <<  2,
  AGS_PORT_USE_LADSPA_FLOAT   = 1 <<  3,
  AGS_PORT_IS_OUTPUT          = 1 <<  4,
  AGS_PORT_INFINITE_RANGE     = 1 <<  5,
}AgsPortFlags;

struct _AgsPort
{
  GObject object;

  guint flags;
  
  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  AgsUUID *uuid;

  gchar *plugin_name;
  gchar *specifier;

  gchar *control_port;
  
  gboolean port_value_is_pointer;
  GType port_value_type;

  guint port_value_size;
  guint port_value_length;

  GObject *plugin_port;
  AgsConversion *conversion;

  GList *automation;
  
  union _AgsPortValue{
    gboolean ags_port_boolean;
    gint64 ags_port_int;
    guint64 ags_port_uint;
    gfloat ags_port_float;
    LADSPA_Data ags_port_ladspa;
    gdouble ags_port_double;
    AgsComplex ags_port_complex;
    gchar *ags_port_string;
    gboolean *ags_port_boolean_ptr;
    gint64 *ags_port_int_ptr;
    guint64 *ags_port_uint_ptr;
    gfloat *ags_port_float_ptr;
    gdouble *ags_port_double_ptr;
    AgsComplex *ags_port_complex_ptr;
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

pthread_mutex_t* ags_port_get_class_mutex();

gboolean ags_port_test_flags(AgsPort *port, guint flags);
void ags_port_set_flags(AgsPort *port, guint flags);
void ags_port_unset_flags(AgsPort *port, guint flags);

void ags_port_safe_read(AgsPort *port, GValue *value);
void ags_port_safe_read_raw(AgsPort *port, GValue *value);
void ags_port_safe_write(AgsPort *port, GValue *value);
void ags_port_safe_write_raw(AgsPort *port, GValue *value);

void ags_port_safe_get_property(AgsPort *port, gchar *property_name, GValue *value);
void ags_port_safe_set_property(AgsPort *port, gchar *property_name, GValue *value);

GList* ags_port_find_specifier(GList *port, gchar *specifier);

void ags_port_add_automation(AgsPort *port,
			     GObject *automation);
void ags_port_remove_automation(AgsPort *port,
				GObject *automation);

AgsPort* ags_port_new();

#endif /*__AGS_PORT_H__*/
