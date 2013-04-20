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

#include <ags/audio/ags_recall_channel_run.h>

void ags_run_order_class_init(AgsRunOrderClass *run_order);
void ags_run_order_init(AgsRunOrder *run_order);
void ags_run_order_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_run_order_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_run_order_finalize(GObject *gobject);

void ags_run_order_changed_output(AgsRunOrder *run_order, AgsChannel *input,
				  guint new_position, guint old_position);
void ags_run_order_changed_input(AgsRunOrder *run_order, AgsChannel *input,
				 guint new_position, guint old_position,
				 AgsGroupId group_id, gboolean do_play);

enum{
  PROP_0,
  PROP_RECALL_ID,
};

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
  GParamSpec *param_spec;

  ags_run_order_parent_class = g_type_class_peek_parent(run_order);

  gobject = (GObjectClass *) run_order;

  gobject->set_property = ags_run_order_set_property;
  gobject->get_property = ags_run_order_get_property;

  gobject->finalize = ags_run_order_finalize;

  /* properties */
  param_spec = g_param_spec_object("recall_id\0",
				   "recall id of AgsRunOrder\0",
				   "The recall id of the AgsRunOrder\0",
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);
}

void
ags_run_order_init(AgsRunOrder *run_order)
{
  run_order->recall_id = NULL;

  run_order->run_count = 0;
  run_order->run_order = NULL;
}


void
ags_run_order_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsRunOrder *run_order;

  run_order = AGS_RUN_ORDER(gobject);

  switch(prop_id){
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_object(value);

      if(run_order->recall_id == recall_id)
	return;

      if(run_order->recall_id != NULL){
	g_object_unref(run_order->recall_id);
      }

      if(recall_id != NULL){
	g_object_ref(recall_id);
      }

      run_order->recall_id = recall_id;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_run_order_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsRunOrder *run_order;

  run_order = AGS_RUN_ORDER(gobject);

  switch(prop_id){
  case PROP_RECALL_ID:
    {
      g_value_set_object(value, run_order->recall_id);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_run_order_finalize(GObject *gobject)
{
  AgsRunOrder *run_order;

  run_order = AGS_RUN_ORDER(gobject);

  if(run_order->recall_id != NULL)
    g_object_unref(run_order->recall_id);

  ags_list_free_and_unref_link(run_order->run_order);

  G_OBJECT_CLASS(ags_run_order_parent_class)->finalize(gobject);
}

void
ags_run_order_changed_input(AgsRunOrder *run_order, AgsChannel *input,
			    guint new_position, guint old_position,
			    AgsGroupId group_id, gboolean do_play)
{
  GList *list;

  if(do_play){
    list = input->play;
  }else{
    list = input->recall;
  }

  while(list != NULL){
    if(AGS_IS_RECALL_CHANNEL_RUN(list->data) &&
       AGS_RECALL(list->data)->recall_id != NULL &&
       AGS_RECALL(list->data)->recall_id->group_id == group_id &&
       AGS_RECALL_CHANNEL_RUN(list->data)->run_order == old_position){
      //      g_message("ags_run_order_changed_input\n\0");
      ags_recall_channel_run_run_order_changed(AGS_RECALL_CHANNEL_RUN(list->data),
					       new_position);
    }
    
    list = list->next;
  }
}

void
ags_run_order_changed_output(AgsRunOrder *run_order, AgsChannel *output,
			     guint new_position, guint old_position)
{
  AgsAudio *audio;
  AgsChannel *input;
  GList *list;
  AgsGroupId output_group_id, group_id;
  gboolean output_do_play, input_do_play;

  audio = AGS_AUDIO(output->audio);

  /* get some parameters */
  group_id = run_order->recall_id->group_id;
  output_do_play = FALSE;
  input_do_play = FALSE;

  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
    output_group_id = run_order->recall_id->group_id;

    if(run_order->recall_id->parent_group_id == 0){
      input_do_play = TRUE;
      output_do_play = TRUE;
    }
  }else{
    AgsRecallID *output_recall_id;

    output_group_id = run_order->recall_id->parent_group_id;

    output_recall_id = ags_recall_id_find_group_id(output->recall_id, output_group_id);

    if(output_recall_id->parent_group_id == 0){
      output_do_play = TRUE;
    }
  }

  /* set input */
  if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
    input = ags_channel_nth(audio->input, output->audio_channel);

    while(input != NULL){
      ags_run_order_changed_input(run_order, input,
				  new_position, old_position,
				  group_id, input_do_play);

      input = input->next_pad;
    }
  }else{
    input = ags_channel_nth(audio->input, output->line);

    ags_run_order_changed_input(run_order, input,
				new_position, old_position,
				group_id, input_do_play);
  }

  /* set output */
  if(output_do_play){
    list = output->play;
  }else{
    list = output->recall;
  }
  
  while(list != NULL){
    if(AGS_IS_RECALL_CHANNEL_RUN(list->data) &&
       AGS_RECALL(list->data)->recall_id != NULL &&
       AGS_RECALL(list->data)->recall_id->group_id == output_group_id &&
       AGS_RECALL_CHANNEL_RUN(list->data)->run_order == old_position){
      ags_recall_channel_run_run_order_changed(AGS_RECALL_CHANNEL_RUN(list->data),
					       new_position);
    }
    
    list = list->next;
  }
}

void
ags_run_order_add_channel(AgsRunOrder *run_order, AgsChannel *channel)
{
  run_order->run_order = g_list_append(run_order->run_order, channel);
  run_order->run_count++;

  ags_run_order_changed_output(run_order, channel, run_order->run_count, 0);
}

void
ags_run_order_insert_channel(AgsRunOrder *run_order, AgsChannel *channel, guint position)
{
  GList *list;
  guint i;

  run_order->run_order = g_list_insert(run_order->run_order, channel, (gint) position);
  run_order->run_count++;

  ags_run_order_changed_output(run_order, channel, position, 0);
  list = g_list_nth(run_order->run_order, position + 1);

  for(i = position + 1; list != NULL; i++){
    channel = AGS_CHANNEL(list->data);

    ags_run_order_changed_output(run_order, channel, i, i - 1);

    list = list->next;
  }
}

void
ags_run_order_remove_channel(AgsRunOrder *run_order, AgsChannel *channel)
{
  int index;

  index = g_list_index(run_order->run_order, channel);

  if(index != -1){
    GList *list;
    guint position;
    guint i;

    position = (guint) index;

    run_order->run_order = g_list_remove(run_order->run_order, channel);
    run_order->run_count--;

    ags_run_order_changed_output(run_order, channel, 0, position + 1);
    list = g_list_nth(run_order->run_order, position);
    
    for(i = position; list != NULL; i++){
      channel = AGS_CHANNEL(list->data);
      
      ags_run_order_changed_output(run_order, channel, i, i + 1);
      
      list = list->next;
    }
  }
}

AgsRunOrder*
ags_run_order_find_group_id(GList *run_order_i, AgsGroupId group_id)
{
  while(run_order_i != NULL){
    if(AGS_RUN_ORDER(run_order_i->data)->recall_id->group_id == group_id){
      return(AGS_RUN_ORDER(run_order_i->data));
    }

    run_order_i = run_order_i->next;
  }
  
  return(NULL);
}

AgsRunOrder*
ags_run_order_new(AgsRecallID *recall_id)
{
  AgsRunOrder *run_order;

  run_order = (AgsRunOrder *) g_object_new(AGS_TYPE_RUN_ORDER,
					   "recall_id\0", recall_id,
					   NULL);

  return(run_order);
}
