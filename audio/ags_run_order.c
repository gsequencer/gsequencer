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

#include <ags/audio/ags_run_order.h>

void ags_run_order_class_init(AgsRunOrderClass *run_order);
void ags_run_order_init(AgsRunOrder *run_order);
void ags_run_order_finalize(GObject *gobject);

static gpointer ags_run_order_parent_class = NULL;

GType
ags_run_order_get_type(void)
{
  static GType ags_type_run_order = 0;

  if(!ags_type_run_order){
    static const GTypeInfo ags_run_order_info = {
      sizeof (AgsRunOrderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_run_order_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRunOrder),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_run_order_init,
    };

    ags_type_run_order = g_type_register_static(G_TYPE_OBJECT,
						"AgsRunOrder\0",
						&ags_run_order_info,
						0);
  }

  return (ags_type_run_order);
}

void
ags_run_order_class_init(AgsRunOrderClass *run_order)
{
  GObjectClass *gobject;

  ags_run_order_parent_class = g_type_class_peek_parent(run_order);

  gobject = (GObjectClass *) run_order;
  gobject->finalize = ags_run_order_finalize;
}

void
ags_run_order_init(AgsRunOrder *run_order)
{
  run_order->recall_id = NULL;

  run_order->run_count = 0;
  run_order->run_order = NULL;
}

void
ags_run_order_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_run_order_parent_class)->finalize(gobject);
}

AgsRunOrder*
ags_run_order_new()
{
  AgsRunOrder *run_order;

  run_order = (AgsRunOrder *) g_object_new(AGS_TYPE_RUN_ORDER, NULL);

  return(run_order);
}
