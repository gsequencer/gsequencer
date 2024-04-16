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

#include <ags/app/editor/ags_scrolled_automation_edit_box.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/app/editor/ags_composite_toolbar.h>
#include <ags/app/editor/ags_composite_edit.h>
#include <ags/app/editor/ags_automation_edit.h>

void ags_scrolled_automation_edit_box_class_init(AgsScrolledAutomationEditBoxClass *scrolled_automation_edit_box);
void ags_scrolled_automation_edit_box_init(AgsScrolledAutomationEditBox *scrolled_automation_edit_box);
void ags_scrolled_automation_edit_box_finalize(GObject *gobject);

void ags_scrolled_automation_edit_box_hadjustment_callback(GtkAdjustment *adjustment,
							   AgsScrolledAutomationEditBox *scrolled_automation_edit_box);
void ags_scrolled_automation_edit_box_vadjustment_callback(GtkAdjustment *adjustment,
							   AgsScrolledAutomationEditBox *scrolled_automation_edit_box);

/**
 * SECTION:ags_scrolled_automation_edit_box
 * @short_description: scrolled automation_edit box widget
 * @title: AgsScrolledAutomationEditBox
 * @section_id:
 * @include: ags/widget/ags_scrolled_automation_edit_box.h
 *
 * The #AgsScrolledAutomationEditBox lets you to have a scrolled automation_edit box widget.
 */

static gpointer ags_scrolled_automation_edit_box_parent_class = NULL;

GType
ags_scrolled_automation_edit_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_scrolled_automation_edit_box = 0;

    static const GTypeInfo ags_scrolled_automation_edit_box_info = {
      sizeof (AgsScrolledAutomationEditBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_scrolled_automation_edit_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScrolledAutomationEditBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_scrolled_automation_edit_box_init,
    };

    ags_type_scrolled_automation_edit_box = g_type_register_static(GTK_TYPE_GRID,
								   "AgsScrolledAutomationEditBox", &ags_scrolled_automation_edit_box_info,
								   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_scrolled_automation_edit_box);
  }

  return g_define_type_id__volatile;
}

void
ags_scrolled_automation_edit_box_class_init(AgsScrolledAutomationEditBoxClass *scrolled_automation_edit_box)
{
  GObjectClass *gobject;

  ags_scrolled_automation_edit_box_parent_class = g_type_class_peek_parent(scrolled_automation_edit_box);

  /* GObjectClass */
  gobject = (GObjectClass *) scrolled_automation_edit_box;

  gobject->finalize = ags_scrolled_automation_edit_box_finalize;
}

void
ags_scrolled_automation_edit_box_init(AgsScrolledAutomationEditBox *scrolled_automation_edit_box)
{
  /* scrolled window */
  scrolled_automation_edit_box->scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_grid_attach((GtkGrid *) scrolled_automation_edit_box,
		  (GtkWidget *) scrolled_automation_edit_box->scrolled_window,
		  0, 0,
		  1, 1);

  gtk_scrolled_window_set_policy(scrolled_automation_edit_box->scrolled_window,
  				 GTK_POLICY_EXTERNAL,
  				 GTK_POLICY_EXTERNAL);

  gtk_widget_set_vexpand((GtkWidget *) scrolled_automation_edit_box->scrolled_window,
			 TRUE);

  g_signal_connect_after(gtk_scrolled_window_get_hadjustment(scrolled_automation_edit_box->scrolled_window), "value-changed",
			 G_CALLBACK(ags_scrolled_automation_edit_box_hadjustment_callback), scrolled_automation_edit_box);

  g_signal_connect_after(gtk_scrolled_window_get_vadjustment(scrolled_automation_edit_box->scrolled_window), "value-changed",
			 G_CALLBACK(ags_scrolled_automation_edit_box_vadjustment_callback), scrolled_automation_edit_box);

  /* automation_edit box */
  scrolled_automation_edit_box->automation_edit_box = (GtkBox *) ags_automation_edit_box_new(GTK_ORIENTATION_VERTICAL);
  gtk_scrolled_window_set_child(scrolled_automation_edit_box->scrolled_window,
				(GtkWidget *) scrolled_automation_edit_box->automation_edit_box);
}

void
ags_scrolled_automation_edit_box_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_scrolled_automation_edit_box_parent_class)->finalize(gobject);
}

void
ags_scrolled_automation_edit_box_hadjustment_callback(GtkAdjustment *adjustment,
						      AgsScrolledAutomationEditBox *scrolled_automation_edit_box)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;
  AgsAutomationEditBox *automation_edit_box;
  
  AgsApplicationContext *application_context;

  GtkAdjustment *composite_adjustment;

  GtkAllocation allocation;

  GList *start_list, *list;

  gdouble gui_scale_factor;
  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;

  application_context = ags_application_context_get_instance();

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  composite_toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);

  zoom_correction = 1.0 / 16;

  map_width = ((64.0) * (16.0 * 16.0 * 1200.0) * zoom * zoom_correction);

  /* automation edit box */
  automation_edit_box = ags_scrolled_automation_edit_box_get_automation_edit_box(composite_editor->automation_edit->edit);

  list =
    start_list = ags_automation_edit_box_get_automation_edit(automation_edit_box);

  if(list != NULL){
    /* adjustment and allocation */
    gtk_widget_get_allocation(GTK_WIDGET(AGS_AUTOMATION_EDIT(list->data)->drawing_area),
			      &allocation);

    composite_adjustment = gtk_scrollbar_get_adjustment(composite_editor->automation_edit->hscrollbar);

    if(map_width - allocation.width > 0){
      if(gtk_adjustment_get_value(adjustment) + allocation.width < gtk_adjustment_get_upper(composite_adjustment)){
	gtk_adjustment_set_value(composite_adjustment,
				 gtk_adjustment_get_value(adjustment));
      }
    }
  }

  g_list_free(start_list);
}

void
ags_scrolled_automation_edit_box_vadjustment_callback(GtkAdjustment *adjustment,
						      AgsScrolledAutomationEditBox *scrolled_automation_edit_box)
{
  AgsCompositeEditor *composite_editor;
  AgsAutomationEditBox *automation_edit_box;
  
  GtkAdjustment *composite_adjustment;

  GtkAllocation scrolled_window_allocation;

  AgsApplicationContext *application_context;

  GList *start_list, *list;

  guint list_length;
  guint key_count;
  double varea_height;

  application_context = ags_application_context_get_instance();

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  /* automation edit box */
  automation_edit_box = ags_scrolled_automation_edit_box_get_automation_edit_box(composite_editor->automation_edit->edit);

  list =
    start_list = ags_automation_edit_box_get_automation_edit(automation_edit_box);

  list_length = g_list_length(start_list);
  
  if(list != NULL){
    /* adjustment and allocation */
    gtk_widget_get_allocation(GTK_WIDGET(scrolled_automation_edit_box->scrolled_window),
			      &scrolled_window_allocation);

    composite_adjustment = gtk_scrollbar_get_adjustment(composite_editor->automation_edit->vscrollbar);

    if(list_length > 0){
      if(gtk_adjustment_get_value(adjustment) + scrolled_window_allocation.height < gtk_adjustment_get_upper(composite_adjustment)){
	gtk_adjustment_set_value(composite_adjustment,
				 gtk_adjustment_get_value(adjustment));
      }
    }
  }

  g_list_free(start_list);
}

/**
 * ags_scrolled_automation_edit_box_get_scrolled_window:
 * @scrolled_automation_edit_box: the #AgsScrolledAutomationEdit_Box
 * 
 * Get scrolled window of @scrolled_automation_edit_box.
 * 
 * Returns: (transfer none): the #GtkScrolledWindow
 *
 * Since: 4.0.0
 */
GtkScrolledWindow*
ags_scrolled_automation_edit_box_get_scrolled_window(AgsScrolledAutomationEditBox *scrolled_automation_edit_box)
{
  if(!AGS_IS_SCROLLED_AUTOMATION_EDIT_BOX(scrolled_automation_edit_box)){
    return(NULL);
  }
  
  return(scrolled_automation_edit_box->scrolled_window);
}

/**
 * ags_scrolled_automation_edit_box_get_automation_edit_box:
 * @scrolled_automation_edit_box: the #AgsScrolledAutomationEdit_Box
 * 
 * Get automation_edit box of @scrolled_automation_edit_box.
 * 
 * Returns: (transfer none): the #AgsAutomationEditBox
 *
 * Since: 4.0.0
 */
AgsAutomationEditBox*
ags_scrolled_automation_edit_box_get_automation_edit_box(AgsScrolledAutomationEditBox *scrolled_automation_edit_box)
{
  if(!AGS_IS_SCROLLED_AUTOMATION_EDIT_BOX(scrolled_automation_edit_box)){
    return(NULL);
  }
  
  return((AgsAutomationEditBox *) scrolled_automation_edit_box->automation_edit_box);
}

/**
 * ags_scrolled_automation_edit_box_new:
 *
 * Create a new #AgsScrolledAutomationEditBox.
 *
 * Returns: a new #AgsScrolledAutomationEditBox
 *
 * Since: 3.0.0
 */
AgsScrolledAutomationEditBox*
ags_scrolled_automation_edit_box_new()
{
  AgsScrolledAutomationEditBox *scrolled_automation_edit_box;

  scrolled_automation_edit_box = (AgsScrolledAutomationEditBox *) g_object_new(AGS_TYPE_SCROLLED_AUTOMATION_EDIT_BOX,
									       NULL);

  return(scrolled_automation_edit_box);
}
