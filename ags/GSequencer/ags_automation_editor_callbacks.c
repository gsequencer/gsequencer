/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/GSequencer/ags_automation_editor_callbacks.h>

#include <ags/GSequencer/ags_ui_provider.h>

gboolean
ags_automation_editor_audio_edit_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsAutomationEditor *automation_editor)
{
  ags_automation_editor_reset_audio_scrollbar(automation_editor);
  
  return(FALSE);
}

gboolean
ags_automation_editor_output_edit_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsAutomationEditor *automation_editor)
{  
  ags_automation_editor_reset_output_scrollbar(automation_editor);

  return(FALSE);
}

gboolean
ags_automation_editor_input_edit_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsAutomationEditor *automation_editor)
{
  ags_automation_editor_reset_input_scrollbar(automation_editor);

  return(FALSE);
}

void
ags_automation_editor_audio_vscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor)
{
  GtkAdjustment *vadjustment;

  vadjustment = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(automation_editor->audio_scrolled_automation_edit_box->viewport));
  gtk_adjustment_set_value(vadjustment,
			   gtk_range_get_value(range));

  vadjustment = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(automation_editor->audio_scrolled_scale_box->viewport));
  gtk_adjustment_set_value(vadjustment,
			   gtk_range_get_value(range));
}

void
ags_automation_editor_audio_hscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor)
{
  AgsApplicationContext *application_context;
   
  GList *list_start, *list;  
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_adjustment_set_value(automation_editor->audio_ruler->adjustment,
			   gui_scale_factor * (gtk_range_get_value(range) / (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH)));
  gtk_widget_queue_draw(GTK_WIDGET(automation_editor->audio_ruler));
    
  /* automation edit */
  if((AGS_AUTOMATION_EDITOR_RESET_AUDIO_HSCROLLBAR & (automation_editor->flags)) == 0){
    list_start =
      list = gtk_container_get_children(GTK_CONTAINER(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box));

    while(list != NULL){
      gtk_range_set_value(GTK_RANGE(AGS_AUTOMATION_EDIT(list->data)->hscrollbar),
			  gtk_range_get_value(range));

      list = list->next;
    }

    g_list_free(list_start);
  }
}

void
ags_automation_editor_output_vscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor)
{
  GtkAdjustment *vadjustment;

  vadjustment = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(automation_editor->output_scrolled_automation_edit_box->viewport));
  gtk_adjustment_set_value(vadjustment,
			   gtk_range_get_value(range));

  vadjustment = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(automation_editor->output_scrolled_scale_box->viewport));
  gtk_adjustment_set_value(vadjustment,
			   gtk_range_get_value(range));
}


void
ags_automation_editor_output_hscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor)
{
  AgsApplicationContext *application_context; 

  GList *list_start, *list;
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_adjustment_set_value(automation_editor->output_ruler->adjustment,
			   gui_scale_factor * (gtk_range_get_value(range) / (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH)));
  gtk_widget_queue_draw(GTK_WIDGET(automation_editor->output_ruler));
  
  /* automation edit */
  if((AGS_AUTOMATION_EDITOR_RESET_OUTPUT_HSCROLLBAR & (automation_editor->flags)) == 0){
    list_start =
      list = gtk_container_get_children(GTK_CONTAINER(automation_editor->output_scrolled_automation_edit_box->automation_edit_box));

    while(list != NULL){
      gtk_range_set_value(GTK_RANGE(AGS_AUTOMATION_EDIT(list->data)->hscrollbar),
			  gtk_range_get_value(range));

      list = list->next;
    }

    g_list_free(list_start);
  }
}

void
ags_automation_editor_input_vscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor)
{
  GtkAdjustment *vadjustment;

  vadjustment = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(automation_editor->input_scrolled_automation_edit_box->viewport));
  gtk_adjustment_set_value(vadjustment,
			   gtk_range_get_value(range));

  vadjustment = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(automation_editor->input_scrolled_scale_box->viewport));
  gtk_adjustment_set_value(vadjustment,
			   gtk_range_get_value(range));
}

void
ags_automation_editor_input_hscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor)
{
  AgsApplicationContext *application_context;
   
  GList *list_start, *list;
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_adjustment_set_value(automation_editor->input_ruler->adjustment,
			   gui_scale_factor * (gtk_range_get_value(range) / (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH)));
  gtk_widget_queue_draw(GTK_WIDGET(automation_editor->input_ruler));
  
  /* automation edit */
  if((AGS_AUTOMATION_EDITOR_RESET_INPUT_HSCROLLBAR & (automation_editor->flags)) == 0){
    list_start =
      list = gtk_container_get_children(GTK_CONTAINER(automation_editor->input_scrolled_automation_edit_box->automation_edit_box));

    while(list != NULL){
      gtk_range_set_value(GTK_RANGE(AGS_AUTOMATION_EDIT(list->data)->hscrollbar),
			  gtk_range_get_value(range));

      list = list->next;
    }

    g_list_free(list_start);
  }
}

void
ags_automation_editor_audio_automation_edit_hscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor)
{
  AgsAutomationEdit *automation_edit;
  
  GList *list_start, *list;
  
  if((AGS_AUTOMATION_EDITOR_RESET_AUDIO_HSCROLLBAR & (automation_editor->flags)) != 0){
    return;
  }
  
  automation_edit = (AgsAutomationEdit *) gtk_widget_get_ancestor(GTK_WIDGET(range),
								  AGS_TYPE_AUTOMATION_EDIT);

  automation_editor->flags |= AGS_AUTOMATION_EDITOR_RESET_AUDIO_HSCROLLBAR;

  gtk_range_set_value(GTK_RANGE(automation_editor->audio_hscrollbar),
		      gtk_range_get_value(range));
  
  /* automation edit */
  list_start =
    list = gtk_container_get_children(GTK_CONTAINER(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box));

  while(list != NULL){
    if(automation_edit != list->data){
      gtk_range_set_value(GTK_RANGE(AGS_AUTOMATION_EDIT(list->data)->hscrollbar),
			  gtk_range_get_value(range));
    }
    
    list = list->next;
  }

  g_list_free(list_start);

  automation_editor->flags &= (~AGS_AUTOMATION_EDITOR_RESET_AUDIO_HSCROLLBAR);
}

void
ags_automation_editor_output_automation_edit_hscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor)
{
  AgsAutomationEdit *automation_edit;
  
  GList *list_start, *list;

  if((AGS_AUTOMATION_EDITOR_RESET_OUTPUT_HSCROLLBAR & (automation_editor->flags)) != 0){
    return;
  }

  automation_edit = (AgsAutomationEdit *) gtk_widget_get_ancestor(GTK_WIDGET(range),
								  AGS_TYPE_AUTOMATION_EDIT);
  
  automation_editor->flags |= AGS_AUTOMATION_EDITOR_RESET_OUTPUT_HSCROLLBAR;

  gtk_range_set_value(GTK_RANGE(automation_editor->output_hscrollbar),
		      gtk_range_get_value(range));

  /* automation edit */
  list_start =
    list = gtk_container_get_children(GTK_CONTAINER(automation_editor->output_scrolled_automation_edit_box->automation_edit_box));

  while(list != NULL){
    if(automation_edit != list->data){
      gtk_range_set_value(GTK_RANGE(AGS_AUTOMATION_EDIT(list->data)->hscrollbar),
			  gtk_range_get_value(range));
    }
    
    list = list->next;
  }

  g_list_free(list_start);

  automation_editor->flags &= (~AGS_AUTOMATION_EDITOR_RESET_OUTPUT_HSCROLLBAR);
}

void
ags_automation_editor_input_automation_edit_hscrollbar_value_changed(GtkRange *range, AgsAutomationEditor *automation_editor)
{
  AgsAutomationEdit *automation_edit;
  
  GList *list_start, *list;

  if((AGS_AUTOMATION_EDITOR_RESET_INPUT_HSCROLLBAR & (automation_editor->flags)) != 0){
    return;
  }

  automation_edit = (AgsAutomationEdit *) gtk_widget_get_ancestor(GTK_WIDGET(range),
								  AGS_TYPE_AUTOMATION_EDIT);
  
  automation_editor->flags |= AGS_AUTOMATION_EDITOR_RESET_INPUT_HSCROLLBAR;

  gtk_range_set_value(GTK_RANGE(automation_editor->input_hscrollbar),
		      gtk_range_get_value(range));

  /* automation edit */
  list_start =
    list = gtk_container_get_children(GTK_CONTAINER(automation_editor->input_scrolled_automation_edit_box->automation_edit_box));

  while(list != NULL){
    if(automation_edit != list->data){
      gtk_range_set_value(GTK_RANGE(AGS_AUTOMATION_EDIT(list->data)->hscrollbar),
			  gtk_range_get_value(range));
    }
    
    list = list->next;
  }

  g_list_free(list_start);

  automation_editor->flags &= (~AGS_AUTOMATION_EDITOR_RESET_INPUT_HSCROLLBAR);
}

void
ags_automation_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
					       AgsAutomationEditor *automation_editor)
{
  ags_automation_editor_machine_changed(automation_editor, machine);
}

void
ags_automation_editor_resize_audio_channels_callback(AgsMachine *machine,
						     guint audio_channels, guint audio_channels_old,
						     AgsAutomationEditor *automation_editor)
{
  AgsAudio *audio;
  
  guint output_pads, input_pads;
  guint i;

  GRecMutex *audio_mutex;
  
  audio = machine->audio;

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get some fields */
  g_rec_mutex_lock(audio_mutex);

  output_pads = audio->output_pads;
  input_pads = audio->input_pads;
  
  g_rec_mutex_unlock(audio_mutex);

  if(audio_channels > audio_channels_old){
    GList *tab;
    
    /* output line */
    for(i = audio_channels_old * output_pads; i < audio_channels * output_pads; i++){
      ags_notebook_insert_tab(automation_editor->output_notebook,
			      i);

      tab = automation_editor->output_notebook->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
    }
    
    /* input line */
    for(i = audio_channels_old * input_pads; i < audio_channels * input_pads; i++){
      ags_notebook_insert_tab(automation_editor->output_notebook,
			      i);

      tab = automation_editor->output_notebook->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
    }
  }else{
    /* output line */
    for(i = audio_channels * output_pads; i < audio_channels_old * output_pads; i++){
      ags_notebook_remove_tab(automation_editor->output_notebook,
			      audio_channels * output_pads);
    }
    
    /* input line */
    for(i = audio_channels * input_pads; i < audio_channels_old * input_pads; i++){
      ags_notebook_remove_tab(automation_editor->input_notebook,
			      audio_channels * input_pads);
    }
  }
}

void
ags_automation_editor_resize_pads_callback(AgsMachine *machine, GType channel_type,
					   guint pads, guint pads_old,
					   AgsAutomationEditor *automation_editor)
{
  AgsAudio *audio;
  
  guint audio_channels;
  guint i;
  
  GRecMutex *audio_mutex;

  audio = machine->audio;

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get some fields */
  g_rec_mutex_lock(audio_mutex);

  audio_channels = audio->audio_channels;
  
  g_rec_mutex_unlock(audio_mutex);

  if(pads > pads_old){
    GList *tab;

    if(channel_type == AGS_TYPE_OUTPUT){
      for(i = audio_channels * pads_old; i < audio_channels * pads; i++){
	ags_notebook_insert_tab(automation_editor->output_notebook,
				i);

	tab = automation_editor->output_notebook->tab;
	gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				     TRUE);
      }
    }else if(channel_type == AGS_TYPE_INPUT){
      for(i = audio_channels * pads_old; i < audio_channels * pads; i++){
	ags_notebook_insert_tab(automation_editor->input_notebook,
				i);

	tab = automation_editor->input_notebook->tab;
	gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				     TRUE);
      }
    }
  }else{
    if(channel_type == AGS_TYPE_OUTPUT){
      for(i = audio_channels * pads; i < audio_channels * pads_old; i++){
	ags_notebook_remove_tab(automation_editor->output_notebook,
				audio_channels * pads);
      }
    }else if(channel_type == AGS_TYPE_INPUT){
      for(i = audio_channels * pads; i < audio_channels * pads_old; i++){
	ags_notebook_remove_tab(automation_editor->input_notebook,
				audio_channels * pads);
      }
    }
  }
}
