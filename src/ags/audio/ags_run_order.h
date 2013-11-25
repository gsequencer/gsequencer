/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#ifndef __AGS_RUN_ORDER_H__
#define __AGS_RUN_ORDER_H__

#include <glib-object.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_channel.h>

#define AGS_TYPE_RUN_ORDER                (ags_run_order_get_type ())
#define AGS_RUN_ORDER(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), AGS_TYPE_RUN_ORDER, AgsRunOrder))
#define AGS_RUN_ORDER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST ((class), AGS_TYPE_RUN_ORDER, AgsRunOrderClass))
#define AGS_IS_RUN_ORDER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RUN_ORDER))
#define AGS_IS_RUN_ORDER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RUN_ORDER))
#define AGS_RUN_ORDER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RUN_ORDER, AgsRunOrderClass))

typedef struct _AgsRunOrder AgsRunOrder;
typedef struct _AgsRunOrderClass AgsRunOrderClass;

struct _AgsRunOrder
{
  GObject object;

  AgsRecallID *recall_id;

  guint run_count;
  GList *run_order; // AgsChannel
};

struct _AgsRunOrderClass
{
  GObjectClass object;
};

GType ags_run_order_get_type(void);

void ags_run_order_add_channel(AgsRunOrder *run_order, AgsChannel *channel);
void ags_run_order_insert_channel(AgsRunOrder *run_order, AgsChannel *channel, guint position);
void ags_run_order_remove_channel(AgsRunOrder *run_order, AgsChannel *channel);
AgsRunOrder* ags_run_order_find_channel(AgsRunOrder *run_order, AgsChannel *channel);

AgsRunOrder* ags_run_order_find_group_id(GList *run_order,
					 AgsGroupId group_id); //TODO:JK: add AgsRecycling *first_recycling, AgsRecycling *last_recycling

AgsRunOrder* ags_run_order_new(AgsRecallID *recall_id);

#endif /*__AGS_RUN_ORDER_H__*/
