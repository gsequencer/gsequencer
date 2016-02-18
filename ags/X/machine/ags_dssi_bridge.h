/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_DSSI_BRIDGE_H__
#define __AGS_DSSI_BRIDGE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_machine.h>

#define AGS_TYPE_DSSI_BRIDGE                (ags_dssi_bridge_get_type())
#define AGS_DSSI_BRIDGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DSSI_BRIDGE, AgsDssiBridge))
#define AGS_DSSI_BRIDGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DSSI_BRIDGE, AgsDssiBridgeClass))
#define AGS_IS_DSSI_BRIDGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DSSI_BRIDGE))
#define AGS_IS_DSSI_BRIDGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DSSI_BRIDGE))
#define AGS_DSSI_BRIDGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DSSI_BRIDGE, AgsDssiBridgeClass))

#define AGS_DSSI_BRIDGE_DEFAULT_VERSION "0.7.4\0"
#define AGS_DSSI_BRIDGE_DEFAULT_BUILD_ID "CEST 16-02-2016 23:36\0"

typedef struct _AgsDssiBridge AgsDssiBridge;
typedef struct _AgsDssiBridgeClass AgsDssiBridgeClass;

typedef enum{
  AGS_DSSI_BRIDGE_DISPLAY_INPUT    = 1,
  AGS_DSSI_BRIDGE_BULK_OUTPUT      = 1 <<  1,
  AGS_DSSI_BRIDGE_DISPLAY_OUTPUT   = 1 <<  2,
  AGS_DSSI_BRIDGE_BULK_INPUT       = 1 <<  3,
}AgsDssiBridgeFlags;

struct _AgsDssiBridge
{
  AgsMachine machine;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;

  guint mapped_output;
  guint mapped_input;

  gchar *filename;
  gchar *effect;
  unsigned long effect_index;
};

struct _AgsDssiBridgeClass
{
  AgsMachineClass machine;
};

GType ags_dssi_bridge_get_type(void);

void ags_dssi_bridge_load(AgsDssiBridge *dssi_bridge);

AgsDssiBridge* ags_dssi_bridge_new(GObject *soundcard,
				   gchar *filename,
				   gchar *effect);

#endif /*__AGS_DSSI_BRIDGE_H__*/
