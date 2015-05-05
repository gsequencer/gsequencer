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

#ifndef __AGS_REPLICATOR_BRIDGE_H__
#define __AGS_REPLICATOR_BRIDGE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

#define AGS_TYPE_REPLICATOR_BRIDGE                (ags_replicator_bridge_get_type())
#define AGS_REPLICATOR_BRIDGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REPLICATOR_BRIDGE, AgsReplicatorBridge))
#define AGS_REPLICATOR_BRIDGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_REPLICATOR_BRIDGE, AgsReplicatorBridgeClass))
#define AGS_IS_REPLICATOR_BRIDGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_REPLICATOR_BRIDGE))
#define AGS_IS_REPLICATOR_BRIDGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_REPLICATOR_BRIDGE))
#define AGS_REPLICATOR_BRIDGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_REPLICATOR_BRIDGE, AgsReplicatorBridgeClass))

#define AGS_REPLICATOR_BRIDGE_DEFAULT_VERSION "0.4.3\0"
#define AGS_REPLICATOR_BRIDGE_DEFAULT_BUILD_ID "CEST 05-05-2015 17:30\0"

typedef struct _AgsReplicatorBridge AgsReplicatorBridge;
typedef struct _AgsReplicatorBridgeClass AgsReplicatorBridgeClass;

typedef enum{
  AGS_REPLICATOR_BRIDGE_PARSED_INPUT    = 1,
  AGS_REPLICATOR_BRIDGE_PARSED_MATRIX   = 1 <<  1,
}AgsReplicatorBridgeFlags;

struct _AgsReplicatorBridge
{
  AgsMachine machine;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;

  guint mapped_input;
  
  GtkHBox *selective_input;
  GtkTable *matrix;
};

struct _AgsReplicatorBridgeClass
{
  AgsMachineClass machine;
};

GType ags_replicator_bridge_get_type(void);

AgsReplicatorBridge* ags_replicator_bridge_new(GObject *soundcard);

#endif /*__AGS_REPLICATOR_BRIDGE_H__*/
