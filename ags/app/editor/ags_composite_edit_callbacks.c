/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/editor/ags_composite_edit_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/app/editor/ags_notation_edit.h>
#include <ags/app/editor/ags_automation_edit.h>
#include <ags/app/editor/ags_wave_edit.h>
#include <ags/app/editor/ags_scrolled_automation_edit_box.h>
#include <ags/app/editor/ags_scrolled_wave_edit_box.h>

void
ags_composite_edit_vscrollbar_changed(GtkAdjustment *adjustment,
				      AgsCompositeEdit *composite_edit)
{
  GtkAdjustment *composite_adjustment;
  
  gdouble upper, lower;
  gdouble page_size;
  gdouble step_increment, page_increment;
  
  g_object_get(adjustment,
	       "upper", &upper,
	       "lower", &lower,
	       "page-size", &page_size,
	       "step-increment", &step_increment,
	       "page-increment", &page_increment,
	       NULL);

  composite_adjustment = gtk_scrollbar_get_adjustment(composite_edit->vscrollbar);
  
  g_object_set(composite_adjustment,
	       "upper", upper,
	       "lower", lower,
	       "page-size", page_size,
	       "step-increment", step_increment,
	       "page-increment", page_increment,
	       NULL);
}

void
ags_composite_edit_hscrollbar_changed(GtkAdjustment *adjustment,
				      AgsCompositeEdit *composite_edit)
{
  GtkAdjustment *composite_adjustment;
  
  gdouble upper, lower;
  gdouble page_size;
  gdouble step_increment, page_increment;
  
  g_object_get(adjustment,
	       "upper", &upper,
	       "lower", &lower,
	       "page-size", &page_size,
	       "step-increment", &step_increment,
	       "page-increment", &page_increment,
	       NULL);

  composite_adjustment = gtk_scrollbar_get_adjustment(composite_edit->hscrollbar);
  
  g_object_set(composite_adjustment,
	       "upper", upper,
	       "lower", lower,
	       "page-size", page_size,
	       "step-increment", step_increment,
	       "page-increment", page_increment,
	       NULL);

  gtk_adjustment_set_upper(composite_edit->ruler->adjustment,
			   upper);
}

void
ags_composite_edit_vscrollbar_callback(GtkAdjustment *adjustment,
				       AgsCompositeEdit *composite_edit)
{
  if(composite_edit->block_vscrollbar){
    return;
  }

  composite_edit->block_vscrollbar = TRUE;

  if(AGS_IS_NOTATION_EDIT(composite_edit->edit)){
    gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_edit->edit)->vscrollbar),
			     gtk_adjustment_get_value(adjustment));
  }
  
  //TODO:JK: implement me

  composite_edit->block_vscrollbar = FALSE;
}

void
ags_composite_edit_hscrollbar_callback(GtkAdjustment *adjustment,
				       AgsCompositeEdit *composite_edit)
{
  AgsCompositeEditor *composite_editor;
  
  AgsApplicationContext *application_context;
    
  gdouble gui_scale_factor;
  gdouble value;

  if(composite_edit->block_hscrollbar){
    return;
  }

  composite_edit->block_hscrollbar = TRUE;

  application_context = ags_application_context_get_instance();

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  if(AGS_IS_NOTATION_EDIT(composite_edit->edit)){
    gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(AGS_NOTATION_EDIT(composite_edit->edit)->hscrollbar),
			     gtk_adjustment_get_value(adjustment));
  }
  
  if(AGS_IS_SCROLLED_AUTOMATION_EDIT_BOX(composite_edit->edit)){
    GList *start_list, *list;

    /* scale factor */
    gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
    
    list =
      start_list = ags_automation_edit_box_get_automation_edit((AgsAutomationEditBox *) AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_edit->edit)->automation_edit_box);

    while(list != NULL){
      gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(AGS_AUTOMATION_EDIT(list->data)->hscrollbar),
			       gtk_adjustment_get_value(adjustment));

      list = list->next;
    }

    g_list_free(start_list);

    value = gtk_adjustment_get_value(adjustment) / (gui_scale_factor * 64.0);
    gtk_adjustment_set_value(composite_edit->ruler->adjustment,
			     gui_scale_factor * value);
  }
  
  if(AGS_IS_SCROLLED_WAVE_EDIT_BOX(composite_edit->edit)){
    GList *start_list, *list;

    list =
      start_list = ags_wave_edit_box_get_wave_edit((AgsWaveEditBox *) AGS_SCROLLED_WAVE_EDIT_BOX(composite_edit->edit)->wave_edit_box);

    while(list != NULL){
      gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(AGS_WAVE_EDIT(list->data)->hscrollbar),
			       gtk_adjustment_get_value(adjustment));

      list = list->next;
    }

    value = gtk_adjustment_get_value(adjustment) / (gui_scale_factor * 64.0);
    gtk_adjustment_set_value(composite_edit->ruler->adjustment,
			     gui_scale_factor * value);
  }

  gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(composite_editor->tempo_edit->hscrollbar),
			   gtk_adjustment_get_value(adjustment));

  composite_edit->block_hscrollbar = FALSE;
}
