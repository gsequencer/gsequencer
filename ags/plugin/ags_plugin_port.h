/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_PLUGIN_PORT_H__
#define __AGS_PLUGIN_PORT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_PLUGIN_PORT                (ags_plugin_port_get_type())
#define AGS_PLUGIN_PORT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLUGIN_PORT, AgsPluginPort))
#define AGS_PLUGIN_PORT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLUGIN_PORT, AgsPluginPortClass))
#define AGS_IS_PLUGIN_PORT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLUGIN_PORT))
#define AGS_IS_PLUGIN_PORT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLUGIN_PORT))
#define AGS_PLUGIN_PORT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLUGIN_PORT, AgsPluginPortClass))

#define AGS_PLUGIN_PORT_GET_OBJ_MUTEX(obj) (&(((AgsPluginPort *) obj)->obj_mutex))

typedef struct _AgsPluginPort AgsPluginPort;
typedef struct _AgsPluginPortClass AgsPluginPortClass;

/**
 * AgsPluginPortFlags:
 * @AGS_PLUGIN_PORT_ATOM: atom port
 * @AGS_PLUGIN_PORT_AUDIO: audio port
 * @AGS_PLUGIN_PORT_CONTROL: control port
 * @AGS_PLUGIN_PORT_MIDI: MIDI port
 * @AGS_PLUGIN_PORT_EVENT: event port
 * @AGS_PLUGIN_PORT_OUTPUT: is output
 * @AGS_PLUGIN_PORT_INPUT: is input
 * @AGS_PLUGIN_PORT_TOGGLED: toggle control
 * @AGS_PLUGIN_PORT_ENUMERATION: enumerated
 * @AGS_PLUGIN_PORT_LOGARITHMIC: logarithmic
 * @AGS_PLUGIN_PORT_INTEGER: integer
 * @AGS_PLUGIN_PORT_SAMPLERATE: samplerate
 * @AGS_PLUGIN_PORT_BOUNDED_BELOW: bounded below
 * @AGS_PLUGIN_PORT_BOUNDED_ABOVE: bounded above
 * @AGS_PLUGIN_PORT_UI_NOTIFICATION: ui notification
 * 
 * Common port attributes.
 */
typedef enum{
  AGS_PLUGIN_PORT_ATOM            = 1,
  AGS_PLUGIN_PORT_AUDIO           = 1 <<  1,
  AGS_PLUGIN_PORT_CONTROL         = 1 <<  2,
  AGS_PLUGIN_PORT_MIDI            = 1 <<  3,
  AGS_PLUGIN_PORT_EVENT           = 1 <<  4,
  AGS_PLUGIN_PORT_OUTPUT          = 1 <<  5,
  AGS_PLUGIN_PORT_INPUT           = 1 <<  6,
  AGS_PLUGIN_PORT_TOGGLED         = 1 <<  7,
  AGS_PLUGIN_PORT_ENUMERATION     = 1 <<  8,
  AGS_PLUGIN_PORT_LOGARITHMIC     = 1 <<  9,
  AGS_PLUGIN_PORT_INTEGER         = 1 << 10,
  AGS_PLUGIN_PORT_SAMPLERATE      = 1 << 11,
  AGS_PLUGIN_PORT_BOUNDED_BELOW   = 1 << 12,
  AGS_PLUGIN_PORT_BOUNDED_ABOVE   = 1 << 13,
  AGS_PLUGIN_PORT_UI_NOTIFICATION = 1 << 14,
}AgsPluginPortFlags;

struct _AgsPluginPort
{
  GObject gobject;
  
  guint flags;
  
  GRecMutex obj_mutex;

  guint port_index;

  gchar *port_name;
  gchar *port_symbol;

  gint scale_steps;
  gchar **scale_point;
  gdouble *scale_value;
  
  GValue *lower_value;
  GValue *upper_value;

  GValue *default_value;  
  
  gpointer user_data;
};

struct _AgsPluginPortClass
{
  GObjectClass gobject;
};

GType ags_plugin_port_get_type(void);
GType ags_plugin_port_flags_get_type();

GRecMutex* ags_plugin_port_get_obj_mutex(AgsPluginPort *plugin_port);

gboolean ags_plugin_port_test_flags(AgsPluginPort *plugin_port, guint flags);
void ags_plugin_port_set_flags(AgsPluginPort *plugin_port, guint flags);
void ags_plugin_port_unset_flags(AgsPluginPort *plugin_port, guint flags);

guint ags_plugin_port_get_port_index(AgsPluginPort *plugin_port);
void ags_plugin_port_set_port_index(AgsPluginPort *plugin_port,
				    guint port_index);

gchar* ags_plugin_port_get_port_name(AgsPluginPort *plugin_port);
void ags_plugin_port_set_port_name(AgsPluginPort *plugin_port,
				   gchar *port_name);

gchar* ags_plugin_port_get_port_symbol(AgsPluginPort *plugin_port);
void ags_plugin_port_set_port_symbol(AgsPluginPort *plugin_port,
				     gchar *port_symbol);

gint ags_plugin_port_get_scale_steps(AgsPluginPort *plugin_port);
void ags_plugin_port_set_scale_steps(AgsPluginPort *plugin_port,
				     gint scale_steps);

gchar** ags_plugin_port_get_scale_point(AgsPluginPort *plugin_port);
void ags_plugin_port_set_scale_point(AgsPluginPort *plugin_port,
				     gchar **scale_point);

gdouble* ags_plugin_port_get_scale_value(AgsPluginPort *plugin_port);
void ags_plugin_port_set_scale_value(AgsPluginPort *plugin_port,
				     gdouble *scale_value);

GValue* ags_plugin_port_get_lower_value(AgsPluginPort *plugin_port);
void ags_plugin_port_set_lower_value(AgsPluginPort *plugin_port,
				     GValue *lower_value);

GValue* ags_plugin_port_get_upper_value(AgsPluginPort *plugin_port);
void ags_plugin_port_set_upper_value(AgsPluginPort *plugin_port,
				     GValue *upper_value);

GValue* ags_plugin_port_get_default_value(AgsPluginPort *plugin_port);
void ags_plugin_port_set_default_value(AgsPluginPort *plugin_port,
				       GValue *default_value);

GList* ags_plugin_port_find_symbol(GList *plugin_port,
				   gchar *port_symbol);
GList* ags_plugin_port_find_port_index(GList *plugin_port,
				       guint port_index);

AgsPluginPort* ags_plugin_port_new();

G_END_DECLS

#endif /*__AGS_PLUGIN_PORT_H__*/
