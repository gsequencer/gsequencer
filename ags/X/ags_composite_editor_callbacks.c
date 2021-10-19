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

#include <ags/X/ags_composite_editor_callbacks.h>

#include <ags/X/editor/ags_scrolled_automation_edit_box.h>
#include <ags/X/editor/ags_vautomation_edit_box.h>
#include <ags/X/editor/ags_automation_edit.h>
#include <ags/X/editor/ags_scrolled_wave_edit_box.h>
#include <ags/X/editor/ags_vwave_edit_box.h>
#include <ags/X/editor/ags_wave_edit.h>

void
ags_composite_editor_edit_viewport_vadjustment_changed_callback(GtkAdjustment *adjustment,
								AgsCompositeEditor *composite_editor)
{
  gdouble lower, upper;
  gdouble page_increment, step_increment;
  gdouble page_size;
  gdouble value;

  g_object_get(adjustment,
	       "lower", &lower,
	       "upper", &upper,
	       "page-increment", &page_increment,
	       "step-increment", &step_increment,
	       "page-size", &page_size,
	       "value", &value,
	       NULL);

  if(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->viewport->vadjustment == adjustment){
    g_object_set(gtk_range_get_adjustment((GtkRange *) composite_editor->automation_edit->vscrollbar),
		 "lower", lower,
		 "upper", upper,
		 "page-increment", page_increment,
		 "step-increment", step_increment,
		 "page-size", page_size,
		 "value", value,
		 NULL);
  }

  if(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->viewport->vadjustment == adjustment){
    g_object_set(gtk_range_get_adjustment((GtkRange *) composite_editor->wave_edit->vscrollbar),
		 "lower", lower,
		 "upper", upper,
		 "page-increment", page_increment,
		 "step-increment", step_increment,
		 "page-size", page_size,
		 "value", value,
		 NULL);
  }
}

void
ags_composite_editor_edit_vadjustment_value_changed_callback(GtkAdjustment *adjustment,
							     AgsCompositeEditor *composite_editor)
{
  gdouble value;

  value = gtk_adjustment_get_value(adjustment);

  if(gtk_range_get_adjustment(composite_editor->automation_edit->vscrollbar) == adjustment){
    /* edit */
    gtk_adjustment_set_value(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->viewport->vadjustment,
			     value);
    
    /* edit control */
    gtk_adjustment_set_value(AGS_SCROLLED_SCALE_BOX(composite_editor->automation_edit->edit_control)->viewport->vadjustment,
			     value);
  }
  
  if(gtk_range_get_adjustment(composite_editor->wave_edit->vscrollbar) == adjustment){
    /* edit */
    gtk_adjustment_set_value(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->viewport->vadjustment,
			     value);
    
    /* edit control */
    gtk_adjustment_set_value(AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->viewport->vadjustment,
			     value);
  }
}

void
ags_composite_editor_automation_edit_hadjustment_changed_callback(GtkAdjustment *adjustment,
								  AgsCompositeEditor *composite_editor)
{
  gdouble lower, upper;
  gdouble page_increment, step_increment;
  gdouble page_size;
  gdouble value;

  g_object_get(adjustment,
	       "lower", &lower,
	       "upper", &upper,
	       "page-increment", &page_increment,
	       "step-increment", &step_increment,
	       "page-size", &page_size,
	       "value", &value,
	       NULL);

  g_object_set(gtk_range_get_adjustment((GtkRange *) composite_editor->automation_edit->hscrollbar),
	       "lower", lower,
	       "upper", upper,
	       "page-increment", page_increment,
	       "step-increment", step_increment,
	       "page-size", page_size,
	       "value", value,
	       NULL);
}

void
ags_composite_editor_wave_edit_hadjustment_changed_callback(GtkAdjustment *adjustment,
							    AgsCompositeEditor *composite_editor)
{
  gdouble lower, upper;
  gdouble page_increment, step_increment;
  gdouble page_size;
  gdouble value;

  g_object_get(adjustment,
	       "lower", &lower,
	       "upper", &upper,
	       "page-increment", &page_increment,
	       "step-increment", &step_increment,
	       "page-size", &page_size,
	       "value", &value,
	       NULL);

  g_object_set(gtk_range_get_adjustment((GtkRange *) composite_editor->wave_edit->hscrollbar),
	       "lower", lower,
	       "upper", upper,
	       "page-increment", page_increment,
	       "step-increment", step_increment,
	       "page-size", page_size,
	       "value", value,
	       NULL);
}

void
ags_composite_editor_edit_hadjustment_value_changed_callback(GtkAdjustment *adjustment,
							     AgsCompositeEditor *composite_editor)
{
  gdouble value;

  value = gtk_adjustment_get_value(adjustment);

  if(gtk_range_get_adjustment(composite_editor->automation_edit->hscrollbar) == adjustment){
    GList *start_list, *list;

    list = 
      start_list = gtk_container_get_children(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->automation_edit_box);

    while(list != NULL){
      gtk_range_set_value(AGS_AUTOMATION_EDIT(list->data)->hscrollbar,
			  value);
      
      list = list->next;
    }

    g_list_free(start_list);
  }
  
  if(gtk_range_get_adjustment(composite_editor->wave_edit->hscrollbar) == adjustment){
    GList *start_list, *list;

    list = 
      start_list = gtk_container_get_children(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box);

    while(list != NULL){
      gtk_range_set_value(AGS_WAVE_EDIT(list->data)->hscrollbar,
			  value);
      
      list = list->next;
    }

    g_list_free(start_list);
  }
}

void
ags_composite_editor_machine_selector_changed_callback(AgsMachineSelector *machine_selector,
						       AgsMachine *machine,
						       AgsCompositeEditor *composite_editor)
{
  ags_composite_editor_machine_changed(composite_editor,
				       machine);
}

void
ags_composite_editor_resize_audio_channels_callback(AgsMachine *machine, 
						    guint audio_channels, guint audio_channels_old,
						    AgsCompositeEditor *composite_editor)
{
  guint i;
  
  if(audio_channels > audio_channels_old){
    GList *tab;
    
    for(i = audio_channels_old; i < audio_channels; i++){
      ags_notebook_insert_tab(composite_editor->notation_edit->channel_selector,
			      i);

      tab = composite_editor->notation_edit->channel_selector->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
    }
  }else{
    /* shrink notebook */
    for(i = audio_channels; i < audio_channels_old; i++){
      ags_notebook_remove_tab(composite_editor->notation_edit->channel_selector,
			      audio_channels);
    }
  }
}

void
ags_composite_editor_resize_pads_callback(AgsMachine *machine, GType channel_type,
					  guint pads, guint pads_old,
					  AgsCompositeEditor *composite_editor)
{
  AgsAudio *audio;

  audio = machine->audio;

  /* verify pads */
  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT)){
    if(!g_type_is_a(channel_type,
		    AGS_TYPE_INPUT)){
      return;
    }    
  }else{
    if(!g_type_is_a(channel_type,
		    AGS_TYPE_OUTPUT)){
      return;
    }
  }

  /*  */
  g_object_set(AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano,
	       "key-count", pads,
	       NULL);
  gtk_widget_queue_resize((GtkWidget *) AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano);
  gtk_widget_queue_draw((GtkWidget *) AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano);
  
  /*  */
  gtk_widget_queue_draw((GtkWidget *) composite_editor->notation_edit);
}

void
ags_composite_editor_automation_edit_hscrollbar_value_changed(GtkRange *range,
							      AgsCompositeEditor *composite_editor)
{
  //TODO:JK: implement me
}

void
ags_composite_editor_wave_edit_hscrollbar_value_changed(GtkRange *range,
							AgsCompositeEditor *composite_editor)
{
  //TODO:JK: implement me
}
