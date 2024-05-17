/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/editor/ags_automation_edit_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/app/editor/ags_composite_edit.h>
#include <ags/app/editor/ags_scrolled_automation_edit_box.h>

#include <math.h>

void
ags_automation_edit_update_ui_callback(GObject *ui_provider,
				       AgsAutomationEdit *automation_edit)
{
  AgsCompositeEditor *composite_editor;

  GtkAdjustment *hscrollbar_adjustment;
    
  AgsAudio *audio;
  
  AgsApplicationContext *application_context;

  GObject *output_soundcard;
    
  double zoom_factor;
  double width;
  double x;

  if((AGS_AUTOMATION_EDIT_AUTO_SCROLL & (automation_edit->flags)) == 0){
    return;
  }
    
  application_context = ags_application_context_get_instance();
    
  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));
    
  if(composite_editor->selected_machine == NULL){
    return;
  }

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(composite_editor->toolbar)->zoom));

  audio = composite_editor->selected_machine->audio;      

  output_soundcard = ags_audio_get_output_soundcard(audio);
    
  /* reset offset */
  automation_edit->note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
  automation_edit->note_offset_absolute = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(output_soundcard));

  /* 256th */
  automation_edit->note_offset_256th = 16 * automation_edit->note_offset;
  automation_edit->note_offset_256th_absolute = 16 * automation_edit->note_offset_absolute;

  /* reset scrollbar */
  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->hscrollbar);
  x = (double) automation_edit->note_offset * (double) automation_edit->control_width / zoom_factor;
    
  width = (double) gtk_widget_get_width(automation_edit->drawing_area);
  
  if(x < gtk_adjustment_get_value(hscrollbar_adjustment) ||
     x > gtk_adjustment_get_value(hscrollbar_adjustment) + (width * (3.0 / 4.0))){
    gtk_adjustment_set_value(hscrollbar_adjustment,
			     x);

    gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(composite_editor->automation_edit->hscrollbar),
			     x);
  }else{
    gtk_widget_queue_draw(automation_edit->drawing_area);
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }
}

void
ags_automation_edit_draw_callback(GtkWidget *drawing_area,
				  cairo_t *cr,
				  int width, int height,
				  AgsAutomationEdit *automation_edit)
{
  ags_automation_edit_draw(automation_edit, cr);
}

void
ags_automation_edit_drawing_area_resize_callback(GtkWidget *drawing_area,
						 gint width, gint height,
						 AgsAutomationEdit *automation_edit)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;
  AgsCompositeEdit *composite_edit;
  AgsAutomationEditBox *automation_edit_box;
 
  AgsApplicationContext *application_context;

  GtkAdjustment *adjustment;
  GtkAdjustment *external_adjustment;
  GtkAdjustment *edit_control_adjustment;

  GtkAllocation allocation;
  GtkAllocation scrolled_window_allocation;

  GList *start_list, *list;

  guint list_length;
  
  guint key_count;
  double varea_height;

  gdouble gui_scale_factor;
  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;
  
  application_context = ags_application_context_get_instance();

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));
  composite_edit = composite_editor->automation_edit;

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  composite_toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);

  zoom_correction = 1.0 / 16;

  /* map width */
  map_width = ((64.0) * (16.0 * 16.0 * 1200.0) * zoom * zoom_correction);

  if(AGS_IS_SCROLLED_AUTOMATION_EDIT_BOX(composite_edit->edit)){
    automation_edit_box = ags_scrolled_automation_edit_box_get_automation_edit_box(composite_edit->edit);

    list =
      start_list = ags_automation_edit_box_get_automation_edit(automation_edit_box);

    while(list != NULL){
      ags_automation_edit_reset_hscrollbar(list->data);

      list = list->next;
    }

    list = start_list;
  
    if(list != NULL){
      /* adjustment and allocation */
      gtk_widget_get_allocation(GTK_WIDGET(AGS_AUTOMATION_EDIT(list->data)->drawing_area),
				&allocation);

      external_adjustment = gtk_scrollbar_get_adjustment(composite_edit->hscrollbar);

      gtk_adjustment_set_upper(external_adjustment,
			       (map_width - allocation.width > 0.0) ? (gdouble) map_width - allocation.width: 1.0);

      if(map_width - allocation.width > 0){
	if(gtk_adjustment_get_value(external_adjustment) + allocation.width > gtk_adjustment_get_upper(external_adjustment)){
	  gtk_adjustment_set_value(external_adjustment,
				   map_width - allocation.width);
	}
      }else{
	gtk_adjustment_set_value(external_adjustment,
				 0.0);
      }
    }
  
    g_list_free(start_list);
  }

  if(AGS_IS_SCROLLED_AUTOMATION_EDIT_BOX(composite_edit->edit)){
    /* automation edit box */
    automation_edit_box = ags_scrolled_automation_edit_box_get_automation_edit_box(composite_edit->edit);

    list =
      start_list = ags_automation_edit_box_get_automation_edit(automation_edit_box);

    list_length = g_list_length(start_list);

    varea_height = 0;
    
    if(list_length > 0){
      varea_height = (list_length * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT) + (list_length - 1) * AGS_AUTOMATION_EDIT_DEFAULT_PADDING;
    }
    
    if(list != NULL){
      /* adjustment and allocation */
      gtk_widget_get_allocation(GTK_WIDGET(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_edit->edit)->scrolled_window),
				&scrolled_window_allocation);

      adjustment = gtk_scrolled_window_get_vadjustment(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_edit->edit)->scrolled_window);

      external_adjustment = gtk_scrollbar_get_adjustment(composite_edit->vscrollbar);

      edit_control_adjustment = gtk_scrolled_window_get_vadjustment(AGS_SCROLLED_SCALE_BOX(composite_edit->edit_control)->scrolled_window);

      gtk_adjustment_configure(adjustment,
			       gtk_adjustment_get_value(adjustment),
			       0.0,
			       (varea_height - scrolled_window_allocation.height > 0) ? (gdouble) varea_height - scrolled_window_allocation.height: 1.0,
			       1.0,
			       (gdouble) automation_edit->control_height,
			       0.0);

      gtk_adjustment_configure(external_adjustment,
			       gtk_adjustment_get_value(external_adjustment),
			       0.0,
			       (varea_height - scrolled_window_allocation.height > 0) ? (gdouble) varea_height - scrolled_window_allocation.height: 1.0,
			       1.0,
			       (gdouble) automation_edit->control_height,
			       0.0);

      gtk_adjustment_configure(edit_control_adjustment,
			       gtk_adjustment_get_value(adjustment),
			       0.0,
			       (varea_height - scrolled_window_allocation.height > 0) ? (gdouble) varea_height - scrolled_window_allocation.height: 1.0,
			       1.0,
			       (gdouble) automation_edit->control_height,
			       0.0);

      if(list_length > 0){
	if(gtk_adjustment_get_value(external_adjustment) + scrolled_window_allocation.height < gtk_adjustment_get_upper(external_adjustment)){
	  gtk_adjustment_set_value(external_adjustment,
				   gtk_adjustment_get_value(external_adjustment));
	}
      }
    }

    g_list_free(start_list);
  }

  gtk_widget_queue_draw((GtkWidget *) automation_edit);
}

void
ags_automation_edit_vscrollbar_value_changed(GtkAdjustment *adjustment, AgsAutomationEdit *automation_edit)
{  
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
}

void
ags_automation_edit_hscrollbar_value_changed(GtkAdjustment *adjustment, AgsAutomationEdit *automation_edit)
{
  AgsCompositeEditor *composite_editor;  
  GtkWidget *editor;
  
  AgsApplicationContext *application_context;
     
  gdouble gui_scale_factor;
  gdouble value;

  application_context = ags_application_context_get_instance();

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  value = gtk_adjustment_get_value(adjustment) / (gui_scale_factor * 64.0);
  gtk_adjustment_set_value(automation_edit->ruler->adjustment,
			   gui_scale_factor * value);
  gtk_widget_queue_draw((GtkWidget *) automation_edit->ruler);

  editor = gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);
    
  gtk_adjustment_set_value(AGS_COMPOSITE_EDITOR(editor)->automation_edit->ruler->adjustment,
			   gui_scale_factor * value);
  gtk_widget_queue_draw((GtkWidget *) AGS_COMPOSITE_EDITOR(editor)->automation_edit->ruler);

  gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(composite_editor->tempo_edit->hscrollbar),
			   gtk_adjustment_get_value(adjustment));
  
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
}
