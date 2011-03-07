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

void ags_run_order_changed(AgsRunOrder *run_order, AgsChannel *channel, guint position);

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

void
ags_run_order_changed(AgsRunOrder *run_order, AgsChannel *channel, guint position)
{
  GList *list;

  if(run_order->recall_id->parent_group_id == 0){
    list = channel->play;
  }else{
    list = channel->recall;
  }

  while(list != NULL){
    if(AGS_IS_RECALL_CHANNEL_RUN(list->data) &&
       AGS_RECALL(list->data)->recall_id != NULL &&
       AGS_RECALL(list->data)->recall_id->group_id == run_order->recall_id->group_id){
      ags_recall_channel_run_run_order_changed(AGS_RECALL_CHANNEL_RUN(list->data),
					       position);
    }
    
    list = list->next;
  }
}

void
ags_run_order_add_channel(AgsRunOrder *run_order, AgsChannel *channel)
{
  run_order->run_order = g_list_append(run_order->run_order, channel);
  run_order->run_count++;

  ags_run_order_changed(run_order, channel, run_order->run_count - 1);
}

void
ags_run_order_insert_channel(AgsRunOrder *run_order, AgsChannel *channel, guint position)
{
  AgsChannel *channel;
  GList *run_order_i;
  guint i;

  run_order->run_order = g_list_insert(run_order->run_order, channel, (gint) position);
  run_order->run_count++;

  run_order_i = g_list_nth(run_order->run_order, position + 1);

  for(i = run_order->run_count - 1; run_order_i != NULL; i++){
    channel = AGS_CHANNEL(run_order_i->data);

    ags_run_order_changed(run_order, channel, i);

    run_order_i = run_order_i->next;
  }
}

void
ags_run_order_remove_channel(AgsRunOrder *run_order, AgsChannel *channel)
{
}

AgsRunOrder*
ags_run_order_find_group_id(GList *run_order_i, guint group_id)
{
}

AgsRunOrder*
ags_run_order_new()
{
  AgsRunOrder *run_order;

  run_order = (AgsRunOrder *) g_object_new(AGS_TYPE_RUN_ORDER, NULL);

  return(run_order);
}
