/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_LV2_PRESET_H__
#define __AGS_LV2_PRESET_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_turtle.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/ext/presets/presets.h>

#define AGS_TYPE_LV2_PRESET                (ags_lv2_preset_get_type())
#define AGS_LV2_PRESET(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2_PRESET, AgsLv2Preset))
#define AGS_LV2_PRESET_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2_PRESET, AgsLv2PresetClass))
#define AGS_IS_LV2_PRESET(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2_PRESET))
#define AGS_IS_LV2_PRESET_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2_PRESET))
#define AGS_LV2_PRESET_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2_PRESET, AgsLv2PresetClass))

#define AGS_LV2_PORT_PRESET(ptr) ((AgsLv2PortPreset*)(ptr))

typedef struct _AgsLv2Preset AgsLv2Preset;
typedef struct _AgsLv2PresetClass AgsLv2PresetClass;
typedef struct _AgsLv2PortPreset AgsLv2PortPreset;

typedef enum{
  AGS_LV2_PRESET_CONNECTED    = 1,
}AgsLv2PresetFlags;

struct _AgsLv2Preset
{
  GObject gobject;

  guint flags;
  
  GObject *lv2_plugin;

  gchar *uri;

  gchar *bank;
  gchar *preset_label;
  
  AgsTurtle *turtle;

  GList *port_preset;
};

struct _AgsLv2PresetClass
{
  GObjectClass gobject;
};

struct _AgsLv2PortPreset
{
  gchar *port_symbol;

  GValue *port_value;
};

GType ags_lv2_preset_get_type(void);

AgsLv2PortPreset* ags_lv2_port_preset_alloc(gchar *port_symobl,
					    GType port_type);
void ags_lv2_port_preset_free(AgsLv2PortPreset *lv2_port_preset);

void ags_lv2_preset_parse_turtle(AgsLv2Preset *lv2_preset);

GList* ags_lv2_preset_find_preset_label(GList *lv2_preset,
					gchar *preset_label);

AgsLv2Preset* ags_lv2_preset_new(GObject *lv2_plugin,
				 AgsTurtle *turtle,
				 gchar *uri);

#endif /*__AGS_LV2_PRESET_H__*/
