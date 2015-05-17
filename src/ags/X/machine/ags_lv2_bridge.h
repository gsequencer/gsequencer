/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#ifndef __AGS_LV2_BRIDGE_H__
#define __AGS_LV2_BRIDGE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_machine.h>

#define AGS_TYPE_LV2_BRIDGE                (ags_lv2_bridge_get_type())
#define AGS_LV2_BRIDGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2_BRIDGE, AgsLv2Bridge))
#define AGS_LV2_BRIDGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2_BRIDGE, AgsLv2BridgeClass))
#define AGS_IS_LV2_BRIDGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_LV2_BRIDGE))
#define AGS_IS_LV2_BRIDGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_LV2_BRIDGE))
#define AGS_LV2_BRIDGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_LV2_BRIDGE, AgsLv2BridgeClass))

#define AGS_LV2_BRIDGE_DEFAULT_VERSION "0.4.3\0"
#define AGS_LV2_BRIDGE_DEFAULT_BUILD_ID "CEST 13-05-2015 13:40\0"

typedef struct _AgsLv2Bridge AgsLv2Bridge;
typedef struct _AgsLv2BridgeClass AgsLv2BridgeClass;

typedef enum{
  AGS_LV2_BRIDGE_DISPLAY_INPUT    = 1,
  AGS_LV2_BRIDGE_BULK_OUTPUT      = 1 <<  1,
  AGS_LV2_BRIDGE_DISPLAY_OUTPUT   = 1 <<  2,
  AGS_LV2_BRIDGE_BULK_INPUT       = 1 <<  3,
}AgsLv2BridgeFlags;

struct _AgsLv2Bridge
{
  AgsMachine machine;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;

  guint mapped_output;
  guint mapped_input;

  gchar *filename;
  gchar *uri;

  LV2UI_Descriptor *lv2_ui_descriptor;
  LV2UI_Handle *lv2_ui_handle;
};

struct _AgsLv2BridgeClass
{
  AgsMachineClass machine;
};

GType ags_lv2_bridge_get_type(void);

void ags_lv2_bridge_load(AgsLv2Bridge *lv2_bridge);

AgsLv2Bridge* ags_lv2_bridge_new(GObject *soundcard,
				 gchar *filename,
				 gchar *uri);

#endif /*__AGS_LV2_BRIDGE_H__*/
