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

#include <ags/audio/recall/ags_copy_input_to_output.h>

void ags_copy_input_to_output_class_init(AgsCopyInputToOutput *copy_input_to_output);
void ags_copy_input_to_output_init(AgsCopyInputToOutput *copy_input_to_output);

GType
ags_copy_input_to_output_get_type()
{
  static GType ags_type_copy_input_to_output = 0;

  if(!ags_type_copy_input_to_output){
    static const GTypeInfo ags_copy_input_to_output_info = {
      sizeof (AgsCopyInputToOutputClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_input_to_output_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyInputToOutput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_input_to_output_init,
    };
    ags_type_copy_input_to_output = g_type_register_static(AGS_TYPE_RECALL, "AgsCopyInputToOutput\0", &ags_copy_input_to_output_info, 0);
  }
  return (ags_type_copy_input_to_output);
}

void
ags_copy_input_to_output_class_init(AgsCopyInputToOutput *copy_input_to_output)
{
  //  AGS_RECALL_GET_CLASS(copy_input_to_output)->run = ags_copy_input_to_output;
}

void
ags_copy_input_to_output_init(AgsCopyInputToOutput *copy_input_to_output)
{
}

void
ags_copy_input_to_output(AgsRecall *recall)
{
  AgsCopyInputToOutput *copy_input_to_output;
  AgsChannel *output, *input;
  AgsRecycling *recycling;
  GList *list;
  gboolean stream_done;
  void ags_copy_input_to_output_copy_stream_to_stream_done(AgsRecall *recall){
    GList *list_prev, *list_next;

    list_prev = list->prev;
    list_next = list->next;

    if(list_prev != NULL)
      list_prev->next = list_next;
    else
      recall->recall = list_next;

    if(list_next != NULL)
      list_next->prev = list_prev;

    free(list);
    list = list_next;
    stream_done = TRUE;

    ags_recall_destroy(recall);
  }

  output = copy_input_to_output->output;
  input = copy_input_to_output->input;

  list = recall->recall;

  while(input != NULL){
    recycling = input->first_recycling;

    while(recycling != input->last_recycling->next){
      AGS_RECALL_GET_CLASS(list->data)->run((AgsRecall *) list->data);

      recycling = recycling->next;
    }

    if(!stream_done)
      list = list->next;
    else
      stream_done = FALSE;

    input = input->next_pad;
  }
}

AgsCopyInputToOutput*
ags_copy_input_to_output_new()
{
  AgsCopyInputToOutput *copy_input_to_output;

  copy_input_to_output = (AgsCopyInputToOutput *) g_object_new(AGS_TYPE_COPY_INPUT_TO_OUTPUT, NULL);

  return(copy_input_to_output);
}
