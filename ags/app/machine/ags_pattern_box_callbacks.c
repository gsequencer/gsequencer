/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/machine/ags_pattern_box_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>

#include <gdk/gdkkeysyms.h>

void
ags_pattern_box_pad_callback(GtkWidget *toggle_button, AgsPatternBox *pattern_box)
{
  AgsMachine *machine;
  AgsLine *selected_line;

  GList *start_list, *list;
  GList *start_line, *line;
  
  guint i, index0, index1, offset;
  
  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pattern_box,
						   AGS_TYPE_MACHINE);

  if(machine->selected_input_pad == NULL){
    return;
  }

  /*  */
  if((AGS_PATTERN_BOX_BLOCK_PATTERN & (pattern_box->flags)) != 0){
#ifdef AGS_DEBUG
    g_message("AgsPatternBox pattern is blocked\n");
#endif

    return;
  }

  /* calculate offset */
  list =
    start_list = ags_pattern_box_get_pad(pattern_box);

  for(i = 0; i < pattern_box->n_controls && toggle_button != list->data; i++){
    list = list->next;
  }

  offset = i;
  g_list_free(start_list);

  /* retrieve indices */
  index0 = machine->bank_0;
  index1 = machine->bank_1;
  
  /* calculate offset / page */
  if(gtk_check_button_get_active(pattern_box->page_0_15)){
    i = 0;
  }else if(gtk_check_button_get_active(pattern_box->page_16_31)){
    i = 1;
  }else if(gtk_check_button_get_active(pattern_box->page_32_47)){
    i = 2;
  }else if(gtk_check_button_get_active(pattern_box->page_48_63)){
    i = 3;
  }

  offset += (i * pattern_box->n_controls);

  /**/
  line =
    start_line = ags_pad_get_line(AGS_PAD(machine->selected_input_pad));

  while((line = ags_line_find_next_grouped(line)) != NULL){
    GList *start_pattern, *pattern;
    
    selected_line = AGS_LINE(line->data);

    g_object_get(selected_line->channel,
		 "pattern", &start_pattern,
		 NULL);

    /* toggle */
    pattern = start_pattern;
    
    ags_pattern_toggle_bit(pattern->data,
			   index0, index1,
			   offset);

    g_list_free_full(start_pattern,
		     g_object_unref);

    /* iterate */
    line = line->next;
  }

  g_list_free(start_line);
}

void
ags_pattern_box_offset_callback(GtkWidget *widget, AgsPatternBox *pattern_box)
{
  ags_pattern_box_set_pattern(pattern_box);
}
