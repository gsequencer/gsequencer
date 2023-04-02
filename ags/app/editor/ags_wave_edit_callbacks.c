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

#include <ags/app/editor/ags_wave_edit_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/app/editor/ags_composite_edit.h>

#include <math.h>

void
ags_wave_edit_draw_callback(GtkWidget *drawing_area,
			    cairo_t *cr,
			    gint width, gint height,
			    AgsWaveEdit *wave_edit)
{
  ags_wave_edit_draw(wave_edit, cr);
}

void
ags_wave_edit_drawing_area_resize_callback(GtkWidget *drawing_area,
					   gint width, gint height,
					   AgsWaveEdit *wave_edit)
{
  ags_wave_edit_reset_vscrollbar(wave_edit);
  ags_wave_edit_reset_hscrollbar(wave_edit);
  
  gtk_widget_queue_draw((GtkWidget *) wave_edit);
}

void
ags_wave_edit_vscrollbar_value_changed(GtkAdjustment *adjustment, AgsWaveEdit *wave_edit)
{
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) wave_edit->drawing_area);
}

void
ags_wave_edit_hscrollbar_value_changed(GtkAdjustment *adjustment, AgsWaveEdit *wave_edit)
{
  GtkWidget *editor;
  
  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;
  gdouble value;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  value = gtk_adjustment_get_value(adjustment) / (gui_scale_factor * 64.0);
  gtk_adjustment_set_value(wave_edit->ruler->adjustment,
			   gui_scale_factor * value);
  gtk_widget_queue_draw((GtkWidget *) wave_edit->ruler);

  editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);
    
  gtk_adjustment_set_value(AGS_COMPOSITE_EDITOR(editor)->wave_edit->ruler->adjustment,
			   gui_scale_factor * value);
  gtk_widget_queue_draw((GtkWidget *) AGS_COMPOSITE_EDITOR(editor)->wave_edit->ruler);
  
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) wave_edit->drawing_area);
}

