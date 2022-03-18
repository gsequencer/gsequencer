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

#include <ags/app/editor/ags_automation_edit_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_automation_editor.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/app/editor/ags_composite_edit.h>

#include <math.h>

void
ags_automation_edit_draw_callback(GtkWidget *drawing_area,
				  cairo_t *cr,
				  int width, int height,
				  AgsAutomationEdit *automation_edit)
{
  ags_automation_edit_draw(automation_edit, cr);
}

void
ags_automation_edit_vscrollbar_value_changed(GtkRange *range, AgsAutomationEdit *automation_edit)
{  
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
}

void
ags_automation_edit_hscrollbar_value_changed(GtkRange *range, AgsAutomationEdit *automation_edit)
{
  GtkWidget *editor;
  
  AgsApplicationContext *application_context;
     
  gboolean use_composite_editor;
  gdouble gui_scale_factor;
  gdouble value;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  value = gtk_range_get_value(GTK_RANGE(automation_edit->hscrollbar)) / (gui_scale_factor * 64.0);
  gtk_adjustment_set_value(automation_edit->ruler->adjustment,
			   gui_scale_factor * value);
  gtk_widget_queue_draw((GtkWidget *) automation_edit->ruler);

  if(use_composite_editor){
    editor = gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
				     AGS_TYPE_COMPOSITE_EDITOR);
    
    gtk_adjustment_set_value(AGS_COMPOSITE_EDITOR(editor)->automation_edit->ruler->adjustment,
			     gui_scale_factor * value);
    gtk_widget_queue_draw((GtkWidget *) AGS_COMPOSITE_EDITOR(editor)->automation_edit->ruler);
  }
  
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
}
