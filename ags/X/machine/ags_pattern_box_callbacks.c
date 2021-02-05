/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/machine/ags_pattern_box_callbacks.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <gdk/gdkkeysyms.h>

gboolean
ags_pattern_box_focus_in_callback(GtkWidget *widget, GdkEvent *event, AgsPatternBox *pattern_box)
{
  GList *start_list;

  start_list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);
  gtk_widget_set_state_flags((GtkWidget *) g_list_nth_data(start_list,
							   pattern_box->cursor_x),
			     GTK_STATE_FLAG_PRELIGHT);
  g_list_free(start_list);
  
  return(TRUE);
}

gboolean
ags_pattern_box_focus_out_callback(GtkWidget *widget, GdkEvent *event, AgsPatternBox *pattern_box)
{
  GList *start_list;

  start_list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

  if(!gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(start_list,
								       pattern_box->cursor_x - 1))){
    gtk_widget_set_state_flags((GtkWidget *) g_list_nth_data(start_list,
							     pattern_box->cursor_x),
			       GTK_STATE_FLAG_NORMAL);
  }else{
    gtk_widget_set_state_flags((GtkWidget *) g_list_nth_data(start_list,
							     pattern_box->cursor_x),
			       GTK_STATE_FLAG_ACTIVE);
  }
  
  g_list_free(list);
  
  return(TRUE);
}

void
ags_pattern_box_pad_callback(GtkWidget *toggle_button, AgsPatternBox *pattern_box)
{
  AgsMachine *machine;
  AgsLine *selected_line;

  GList *list, *list_start;
  GList *line, *line_start;
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
  list_start = 
    list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

  for(i = 0; i < pattern_box->n_controls && toggle_button != list->data; i++){
    list = list->next;
  }

  offset = i;
  g_list_free(list_start);

  /* retrieve indices */
  index0 = machine->bank_0;
  index1 = machine->bank_1;
  
  /* calculate offset / page */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) pattern_box->offset);

  for(i = 0; i < 4 && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(list->data)); i++){
    list = list->next;
  }

  offset += (i * pattern_box->n_controls);

  g_list_free(list_start);

  /**/
  line_start = 
    line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(machine->selected_input_pad)->expander_set));

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

  g_list_free(line_start);
}

void
ags_pattern_box_offset_callback(GtkWidget *widget, AgsPatternBox *pattern_box)
{
  ags_pattern_box_set_pattern(pattern_box);
}

gboolean
ags_pattern_box_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsPatternBox *pattern_box)
{
  if(event->keyval == GDK_KEY_Tab){
    return(FALSE);
  }

  switch(event->keyval){
  case GDK_KEY_Control_L:
    {
      pattern_box->key_mask |= AGS_PATTERN_BOX_KEY_L_CONTROL;
    }
    break;
  case GDK_KEY_Control_R:
    {
      pattern_box->key_mask |= AGS_PATTERN_BOX_KEY_R_CONTROL;
    }
    break;
  case GDK_KEY_c:
    {
      /* copy notes */
      if((AGS_PATTERN_BOX_KEY_L_CONTROL & (pattern_box->key_mask)) != 0 || (AGS_PATTERN_BOX_KEY_R_CONTROL & (pattern_box->key_mask)) != 0){
	AgsMachine *machine;

	machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pattern_box,
							 AGS_TYPE_MACHINE);
	
	ags_machine_copy_pattern(machine);
      }
    }
    break;
  }
  
  return(TRUE);
}

gboolean
ags_pattern_box_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsPatternBox *pattern_box)
{
  AgsMachine *machine;
  
  if(event->keyval == GDK_KEY_Tab){
    return(FALSE);
  }
  
  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pattern_box,
						   AGS_TYPE_MACHINE);
    
  switch(event->keyval){
  case GDK_KEY_Control_L:
    {
      pattern_box->key_mask &= (~AGS_PATTERN_BOX_KEY_L_CONTROL);
    }
    break;
  case GDK_KEY_Control_R:
    {
      pattern_box->key_mask &= (~AGS_PATTERN_BOX_KEY_R_CONTROL);
    }
    break;
  case GDK_KEY_Left:
  case GDK_KEY_leftarrow:
    {
      if(pattern_box->cursor_x > 0){
	GList *start_list;
	
	pattern_box->cursor_x -= 1;

	start_list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);
	
	gtk_widget_set_state_flags((GtkWidget *) g_list_nth_data(start_list,
								 pattern_box->cursor_x),
				   GTK_STATE_FLAG_PRELIGHT);

	if(!gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(start_list,
									     pattern_box->cursor_x + 1))){
	  gtk_widget_set_state_flags((GtkWidget *) g_list_nth_data(start_list,
								   pattern_box->cursor_x + 1),
				     GTK_STATE_FLAG_NORMAL);
	}else{
	  gtk_widget_set_state_flags((GtkWidget *) g_list_nth_data(start_list,
								   pattern_box->cursor_x + 1),
				     GTK_STATE_FLAG_ACTIVE);
	}

	if(gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(start_list,
									    pattern_box->cursor_y * pattern_box->n_controls + pattern_box->cursor_x))){
	  /* give audible feedback */
	  ags_pad_play((AgsPad *) machine->selected_input_pad);
	}
	
	g_list_free(start_list);
      }
    }
    break;
  case GDK_KEY_Right:
  case GDK_KEY_rightarrow:
    {
      if(pattern_box->cursor_x + 1 < pattern_box->n_controls){
	GList *start_list;
	
	pattern_box->cursor_x += 1;

	start_list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

      	gtk_widget_set_state_flags((GtkWidget *) g_list_nth_data(start_list,
								 pattern_box->cursor_x),
				   GTK_STATE_FLAG_PRELIGHT);
	
	if(!gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(start_list,
									     pattern_box->cursor_x - 1))){
	  gtk_widget_set_state_flags((GtkWidget *) g_list_nth_data(start_list,
								   pattern_box->cursor_x - 1),
				     GTK_STATE_FLAG_NORMAL);
	}else{	  
	  gtk_widget_set_state_flags((GtkWidget *) g_list_nth_data(start_list,
								   pattern_box->cursor_x - 1),
				     GTK_STATE_FLAG_ACTIVE);
	}

	if(gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(start_list,
									    pattern_box->cursor_y * pattern_box->n_controls + pattern_box->cursor_x))){
	  /* give audible feedback */
	  ags_pad_play((AgsPad *) machine->selected_input_pad);
	}
	
	g_list_free(start_list);
      }
    }
    break;
  case GDK_KEY_Up:
  case GDK_KEY_uparrow:
    {
      if(pattern_box->cursor_y > 0){
	GList *start_pad;
	GList *start_list;

	pattern_box->cursor_y -= 1;

	start_list = gtk_container_get_children((GtkContainer *) pattern_box->offset);

      	gtk_button_clicked(g_list_nth_data(start_list,
					   pattern_box->cursor_y));

	g_list_free(start_list);

	/* give audible feedback */
	start_pad = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

	if(gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(start_pad,
									    pattern_box->cursor_y * pattern_box->n_controls + pattern_box->cursor_x))){
	  ags_pad_play((AgsPad *) machine->selected_input_pad);
	}
	
	g_list_free(start_pad);	
      }
    }
    break;
  case GDK_KEY_Down:
  case GDK_KEY_downarrow:
    {
      if(pattern_box->cursor_y + 1 < pattern_box->n_indices){
	GList *start_pad;
	GList *start_list;

	start_list = gtk_container_get_children((GtkContainer *) pattern_box->offset);
	pattern_box->cursor_y += 1;

	gtk_button_clicked(g_list_nth_data(start_list,
					   pattern_box->cursor_y));

	g_list_free(start_list);
	
	/* give audible feedback */
	start_pad = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

	if(gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(start_pad,
									    pattern_box->cursor_y * pattern_box->n_controls + pattern_box->cursor_x))){
	  ags_pad_play((AgsPad *) machine->selected_input_pad);
	}
	
	g_list_free(start_pad);	
      }
    }
    break;
  case GDK_KEY_space:
    {
      AgsLine *selected_line;

      AgsChannel *channel;
      
      GList *line, *line_start;
      
      guint i, j;
      guint offset;
      guint index0, index1;
            
      i = pattern_box->cursor_y;
      j = pattern_box->cursor_x;

      offset = (i * pattern_box->n_controls) + j;
      
      index0 = machine->bank_0;
      index1 = machine->bank_1;

      line_start = 
	line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(machine->selected_input_pad)->expander_set));

      while((line = ags_line_find_next_grouped(line)) != NULL){
	GList *start_pattern, *pattern;
	
	selected_line = AGS_LINE(line->data);

	channel = selected_line->channel;
	
	g_object_get(channel,
		     "pattern", &start_pattern,
		     NULL);
	
	/* toggle pattern */
	pattern = start_pattern;
    
	ags_pattern_toggle_bit(pattern->data,
			       index0, index1,
			       offset);

	g_list_free_full(start_pattern,
			 g_object_unref);

	/* iterate */
	line = line->next;
      }

      g_list_free(line_start);

      /* give audible feedback */
      ags_pad_play((AgsPad *) machine->selected_input_pad);
    }
    break;
  }

  return(TRUE);
}
