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

#include <ags/app/ags_composite_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/app/editor/ags_composite_edit.h>
#include <ags/app/editor/ags_composite_edit_callbacks.h>
#include <ags/app/editor/ags_scrolled_automation_edit_box.h>
#include <ags/app/editor/ags_vautomation_edit_box.h>
#include <ags/app/editor/ags_automation_edit.h>
#include <ags/app/editor/ags_scrolled_wave_edit_box.h>
#include <ags/app/editor/ags_vwave_edit_box.h>
#include <ags/app/editor/ags_wave_edit.h>

#include <ags/app/machine/ags_audiorec.h>

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
  AgsApplicationContext *application_context;
  
  GList *start_list, *list;
  
  guint output_pads, input_pads;
  guint i;
  guint j;

  application_context = ags_application_context_get_instance();

  output_pads = 0;
  input_pads = 0;
  
  g_object_get(machine->audio,
	       "output-pads", &output_pads,
	       "input-pads", &input_pads,
	       NULL);
  
  if(audio_channels > audio_channels_old){
    GList *tab;

    gdouble gui_scale_factor;

    /* scale factor */
    gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
      
    for(i = audio_channels_old; i < audio_channels; i++){
      ags_notebook_insert_tab(composite_editor->notation_edit->channel_selector,
			      i);

      tab = composite_editor->notation_edit->channel_selector->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
    }

    for(i = audio_channels_old; i < audio_channels; i++){
      ags_notebook_insert_tab(composite_editor->wave_edit->channel_selector,
			      i);

      tab = composite_editor->wave_edit->channel_selector->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
    }

    if(composite_editor->automation_edit->focused_edit != NULL){
      if(g_type_is_a(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_TYPE_OUTPUT)){
	for(i = 0; i < output_pads; i++){
	  for(j = audio_channels_old; j < audio_channels; j++){
	    ags_notebook_insert_tab(composite_editor->automation_edit->channel_selector,
				    output_pads * audio_channels_old + i * (audio_channels - audio_channels_old) + j);

	    tab = composite_editor->automation_edit->channel_selector->tab;
	    gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
					 TRUE);
	  }
	}
      }else if(g_type_is_a(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_TYPE_INPUT)){
	for(i = 0; i < input_pads; i++){
	  for(j = audio_channels_old; j < audio_channels; j++){
	    ags_notebook_insert_tab(composite_editor->automation_edit->channel_selector,
				    input_pads * audio_channels_old + i * (audio_channels - audio_channels_old) + j);

	    tab = composite_editor->automation_edit->channel_selector->tab;
	    gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
					 TRUE);
	  }
	}	
      }
    }
    
    if(AGS_IS_AUDIOREC(machine)){
      for(i = 0; i < input_pads; i++){
	for(j = audio_channels_old; j < audio_channels; j++){
	  AgsWaveEdit *wave_edit;
	  AgsLevel *level;
	  
	  /* level */
	  level = ags_level_new();
	  g_object_set(level,
		       "level-width", (guint) (gui_scale_factor * AGS_LEVEL_DEFAULT_LEVEL_WIDTH),
		       "level-height", (guint) (gui_scale_factor * AGS_LEVEL_DEFAULT_LEVEL_HEIGHT),
		       NULL);
	  gtk_box_pack_start(GTK_BOX(AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->level_box),
			     GTK_WIDGET(level),
			     FALSE, TRUE,
			     AGS_WAVE_EDIT_DEFAULT_PADDING);

	  gtk_box_reorder_child(GTK_BOX(AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->level_box),
				GTK_WIDGET(level),
				i * audio_channels + j);
	
	  gtk_widget_show(GTK_WIDGET(level));

	  /* wave edit */
	  wave_edit = ags_wave_edit_new(i * audio_channels + j);
	  gtk_box_pack_start(GTK_BOX(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box),
			     GTK_WIDGET(wave_edit),
			     FALSE, FALSE,
			     AGS_WAVE_EDIT_DEFAULT_PADDING);

	  gtk_box_reorder_child(GTK_BOX(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box),
				GTK_WIDGET(wave_edit),
				i * audio_channels + j);	
	
	  ags_connectable_connect(AGS_CONNECTABLE(wave_edit));
	  gtk_widget_show(GTK_WIDGET(wave_edit));

	  g_signal_connect(gtk_range_get_adjustment((GtkRange *) wave_edit->hscrollbar), "changed",
			   G_CALLBACK(ags_composite_editor_wave_edit_hadjustment_changed_callback), (gpointer) composite_editor);
      
	  g_signal_connect(gtk_range_get_adjustment(wave_edit->hscrollbar), "changed",
			   G_CALLBACK(ags_composite_edit_hscrollbar_changed), composite_editor->wave_edit);
      
	  g_signal_connect((GObject *) wave_edit->hscrollbar, "value-changed",
			   G_CALLBACK(ags_composite_editor_wave_edit_hscrollbar_value_changed), (gpointer) composite_editor);      
	}
      }
    }
  }else{
    /* shrink notebook */
    for(i = audio_channels; i < audio_channels_old; i++){
      ags_notebook_remove_tab(composite_editor->notation_edit->channel_selector,
			      audio_channels);
    }

    if(composite_editor->automation_edit->focused_edit != NULL){
      if(g_type_is_a(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_TYPE_OUTPUT)){
	for(i = 0; i < output_pads; i++){
	  for(j = audio_channels_old; j < audio_channels; j++){
	    ags_notebook_remove_tab(composite_editor->wave_edit->channel_selector,
				    output_pads * audio_channels);
	  }
	}
      }else if(g_type_is_a(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_TYPE_INPUT)){
	for(i = 0; i < input_pads; i++){
	  for(j = audio_channels_old; j < audio_channels; j++){
	    ags_notebook_remove_tab(composite_editor->wave_edit->channel_selector,
				    output_pads * audio_channels);
	  }
	}	
      }
    }
    
    if(AGS_IS_AUDIOREC(machine)){      
      for(i = audio_channels; i < audio_channels_old; i++){
	ags_notebook_remove_tab(composite_editor->wave_edit->channel_selector,
				audio_channels);
      }
      
      list = 
	start_list = gtk_container_get_children(GTK_CONTAINER(AGS_SCROLLED_LEVEL_BOX(composite_editor->wave_edit->edit_control)->level_box));
      
      for(i = 0; i < input_pads && list != NULL; i++){
	for(j = 0; j < audio_channels && list != NULL; j++){
	  list = list->next;
	}

	for(; j < audio_channels_old && list != NULL; j++){
	  gtk_widget_destroy(list->data);
	  
	  list = list->next;
	}
      }

      g_list_free(start_list);

      list = 
	start_list = gtk_container_get_children(GTK_CONTAINER(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box));
      
      for(i = 0; i < input_pads && list != NULL; i++){
	for(j = 0; j < audio_channels && list != NULL; j++){
	  list = list->next;
	}

	for(; j < audio_channels_old && list != NULL; j++){
	  gtk_widget_destroy(list->data);
	  
	  list = list->next;
	}
      }
      
      g_list_free(start_list);      
    }
  }
}

void
ags_composite_editor_resize_pads_callback(AgsMachine *machine, GType channel_type,
					  guint pads, guint pads_old,
					  AgsCompositeEditor *composite_editor)
{
  AgsAudio *audio;

  guint audio_channels;
  guint i;
  guint j;
  
  audio = machine->audio;

  audio_channels = 0;
  
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       NULL);
  
  if(composite_editor->automation_edit->focused_edit != NULL){
    GList *tab;

    if(pads > pads_old){
      if(g_type_is_a(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_TYPE_OUTPUT) &&
	 g_type_is_a(channel_type, AGS_TYPE_OUTPUT)){
	for(i = pads_old; i < pads; i++){
	  for(j = 0; j < audio_channels; j++){
	    ags_notebook_insert_tab(composite_editor->automation_edit->channel_selector,
				    (pads_old * audio_channels) + ((i - pads_old) * audio_channels) + j);

	    tab = composite_editor->automation_edit->channel_selector->tab;
	    gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
					 TRUE);
	  }
	}
      }else if(g_type_is_a(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_TYPE_INPUT) &&
	       g_type_is_a(channel_type, AGS_TYPE_INPUT)){
	for(i = pads_old; i < pads; i++){
	  for(j = 0; j < audio_channels; j++){
	    ags_notebook_insert_tab(composite_editor->automation_edit->channel_selector,
				    (pads_old * audio_channels) + ((i - pads_old) * audio_channels) + j);

	    tab = composite_editor->automation_edit->channel_selector->tab;
	    gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
					 TRUE);
	  }
	}	
      }
    }else{
      if(g_type_is_a(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_TYPE_OUTPUT) &&
	 g_type_is_a(channel_type, AGS_TYPE_OUTPUT)){
	for(i = pads; i < pads_old; i++){
	  for(j = 0; j < audio_channels; j++){
	    ags_notebook_remove_tab(composite_editor->automation_edit->channel_selector,
				    pads * audio_channels);
	  }
	}
      }else if(g_type_is_a(AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type, AGS_TYPE_INPUT) &&
	       g_type_is_a(channel_type, AGS_TYPE_INPUT)){
	for(i = pads; i < pads_old; i++){
	  for(j = 0; j < audio_channels; j++){
	    ags_notebook_remove_tab(composite_editor->automation_edit->channel_selector,
				    pads * audio_channels);
	  }
	}	
      }
    }
  }
  
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