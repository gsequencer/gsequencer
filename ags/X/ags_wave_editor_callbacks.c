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

#include <ags/X/ags_wave_editor_callbacks.h>

#include <ags/X/ags_ui_provider.h>

gboolean
ags_wave_editor_edit_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsWaveEditor *wave_editor)
{
  ags_wave_editor_reset_scrollbar(wave_editor);

  return(FALSE);
}

void
ags_wave_editor_vscrollbar_value_changed(GtkRange *range, AgsWaveEditor *wave_editor)
{
  GtkAdjustment *vadjustment;

  vadjustment = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(wave_editor->scrolled_wave_edit_box->viewport));
  gtk_adjustment_set_value(vadjustment,
			   gtk_range_get_value(range));

  vadjustment = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(wave_editor->scrolled_level_box->viewport));
  gtk_adjustment_set_value(vadjustment,
			   gtk_range_get_value(range));
}

void
ags_wave_editor_hscrollbar_value_changed(GtkRange *range, AgsWaveEditor *wave_editor)
{
  AgsApplicationContext *application_context;
   
  GList *list_start, *list;

  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  gtk_adjustment_set_value(wave_editor->ruler->adjustment,
			   gui_scale_factor * (gtk_range_get_value(range) / (guint) (gui_scale_factor * AGS_WAVE_EDIT_DEFAULT_CONTROL_WIDTH)));
  gtk_widget_queue_draw(GTK_WIDGET(wave_editor->ruler));
    
  /* wave edit */
  if((AGS_WAVE_EDITOR_RESET_HSCROLLBAR & (wave_editor->flags)) == 0){
    list_start =
      list = gtk_container_get_children(GTK_CONTAINER(wave_editor->scrolled_wave_edit_box->wave_edit_box));

    while(list != NULL){
      GtkAdjustment *adjustment;

      adjustment = gtk_range_get_adjustment(GTK_RANGE(AGS_WAVE_EDIT(list->data)->hscrollbar));
      
      gtk_adjustment_set_value(adjustment,
			       gtk_range_get_value(range));

      list = list->next;
    }

    g_list_free(list_start);
  }
}

void
ags_wave_editor_wave_edit_hscrollbar_value_changed(GtkRange *range, AgsWaveEditor *wave_editor)
{
  AgsApplicationContext *application_context;
  
  GList *list_start, *list;
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_adjustment_set_value(wave_editor->ruler->adjustment,
			   gtk_range_get_value(range) / (guint) (gui_scale_factor * AGS_WAVE_EDIT_DEFAULT_CONTROL_WIDTH));
  gtk_widget_queue_draw(GTK_WIDGET(wave_editor->ruler));
  
  /* wave edit */
  if((AGS_WAVE_EDITOR_RESET_HSCROLLBAR & (wave_editor->flags)) == 0){
    list_start =
      list = gtk_container_get_children(GTK_CONTAINER(wave_editor->scrolled_wave_edit_box->wave_edit_box));

    while(list != NULL){
      GtkAdjustment *adjustment;

      adjustment = gtk_range_get_adjustment(GTK_RANGE(AGS_WAVE_EDIT(list->data)->hscrollbar));
      
      gtk_adjustment_set_value(adjustment,
			       gtk_range_get_value(range));

      list = list->next;
    }

    g_list_free(list_start);
  }
}

void
ags_wave_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
					 AgsWaveEditor *wave_editor)
{
  ags_wave_editor_machine_changed(wave_editor, machine);
}

void
ags_wave_editor_resize_audio_channels_callback(AgsMachine *machine, 
					       guint audio_channels, guint audio_channels_old,
					       AgsWaveEditor *wave_editor)
{
  guint input_pads;
  guint i;

  g_object_get(machine->audio,
	       "input-pads", &input_pads,
	       NULL);
  
  if(audio_channels > audio_channels_old){
    GList *tab;
    
    for(i = audio_channels_old * input_pads; i < audio_channels * input_pads; i++){
      ags_notebook_insert_tab(wave_editor->notebook,
			      i);

      tab = wave_editor->notebook->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
    }
  }else{
    /* shrink notebook */
    for(i = audio_channels * input_pads; i < audio_channels_old * input_pads; i++){
      ags_notebook_remove_tab(wave_editor->notebook,
			      audio_channels);
    }
  }

  //TODO:JK: implement me
}

void
ags_wave_editor_resize_pads_callback(AgsMachine *machine, GType channel_type,
				     guint pads, guint pads_old,
				     AgsWaveEditor *wave_editor)
{
  AgsAudio *audio;

  GList *tab;

  guint audio_channels;
  guint i;

  /* verify pads */
  if(!g_type_is_a(channel_type,
		  AGS_TYPE_INPUT)){
    return;
  }    

  audio = machine->audio;

  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       NULL);
  
  for(i = audio_channels * pads_old; i < audio_channels * pads; i++){
    ags_notebook_insert_tab(wave_editor->notebook,
			    i);

    tab = wave_editor->notebook->tab;
    gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				 TRUE);
  }

  //TODO:JK: implement me
}
