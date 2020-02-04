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

#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_automation_editor_callbacks.h>

#include <ags/X/ags_window.h>

#include <ags/X/editor/ags_scrolled_automation_edit_box.h>
#include <ags/X/editor/ags_vautomation_edit_box.h>
#include <ags/X/editor/ags_automation_edit.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <math.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_automation_editor_class_init(AgsAutomationEditorClass *automation_editor);
void ags_automation_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_editor_init(AgsAutomationEditor *automation_editor);
void ags_automation_editor_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_automation_editor_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_automation_editor_connect(AgsConnectable *connectable);
void ags_automation_editor_disconnect(AgsConnectable *connectable);
void ags_automation_editor_finalize(GObject *gobject);

void ags_automation_editor_real_machine_changed(AgsAutomationEditor *automation_editor, AgsMachine *machine);

gint ags_automation_editor_paste_automation_all(AgsAutomationEditor *automation_editor,
						AgsNotebook *notebook,
						AgsMachine *machine,
						xmlNode *automation_node,
						AgsTimestamp *timestamp,
						gboolean match_line, gboolean no_duplicates,
						guint position_x, guint position_y,
						gboolean paste_from_position,
						gint *last_x);
gint ags_automation_editor_paste_automation(AgsAutomationEditor *automation_editor,
					    AgsNotebook *notebook,
					    AgsMachine *machine,
					    xmlNode *audio_node,
					    guint position_x, guint position_y,
					    gboolean paste_from_position,
					    gint *last_x);

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
};

static gpointer ags_automation_editor_parent_class = NULL;
static guint automation_editor_signals[LAST_SIGNAL];

/**
 * SECTION:ags_automation_editor
 * @short_description: A composite widget to edit automation
 * @title: AgsAutomationEditor
 * @section_id:
 * @include: ags/X/ags_automation_editor.h
 *
 * #AgsAutomationEditor is a composite widget to edit automation. You may select machines
 * or change editor tool to do automation.
 */

GType
ags_automation_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_automation_editor = 0;

    static const GTypeInfo ags_automation_editor_info = {
      sizeof (AgsAutomationEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_automation_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutomationEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_automation_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_automation_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_automation_editor = g_type_register_static(GTK_TYPE_VBOX,
							"AgsAutomationEditor", &ags_automation_editor_info,
							0);
    
    g_type_add_interface_static(ags_type_automation_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_automation_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_automation_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_automation_editor_connect;
  connectable->disconnect = ags_automation_editor_disconnect;
}

void
ags_automation_editor_class_init(AgsAutomationEditorClass *automation_editor)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_automation_editor_parent_class = g_type_class_peek_parent(automation_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) automation_editor;

  gobject->set_property = ags_automation_editor_set_property;
  gobject->get_property = ags_automation_editor_get_property;

  gobject->finalize = ags_automation_editor_finalize;
  
  /* properties */
  
  /* AgsEditorClass */
  automation_editor->machine_changed = ags_automation_editor_real_machine_changed;

  /* signals */
  /**
   * AgsEditor::machine-changed:
   * @editor: the object to change machine.
   * @machine: the #AgsMachine to set
   *
   * The ::machine-changed signal notifies about changed machine.
   * 
   * Since: 3.0.0
   */
  automation_editor_signals[MACHINE_CHANGED] =
    g_signal_new("machine-changed",
                 G_TYPE_FROM_CLASS(automation_editor),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAutomationEditorClass, machine_changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_automation_editor_init(AgsAutomationEditor *automation_editor)
{
  GtkViewport *viewport;
  GtkHBox *hbox;
  GtkScrolledWindow *scrolled_window;
  GtkTable *table;

  GtkStyleContext *style_context;  

  GtkAdjustment *adjustment;

  AgsConfig *config;

  gchar *str;
  
  gdouble gui_scale_factor;
  
  automation_editor->flags = (AGS_AUTOMATION_EDITOR_PASTE_MATCH_LINE |
			      AGS_AUTOMATION_EDITOR_PASTE_NO_DUPLICATES);

  automation_editor->version = AGS_AUTOMATION_EDITOR_DEFAULT_VERSION;
  automation_editor->build_id = AGS_AUTOMATION_EDITOR_DEFAULT_BUILD_ID;

  config = ags_config_get_instance();

  /* scale factor */
  gui_scale_factor = 1.0;
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "gui-scale");

  if(str != NULL){
    gui_scale_factor = g_ascii_strtod(str,
				      NULL);

    g_free(str);
  }

  /* offset */
  automation_editor->tact_counter = 0;
  automation_editor->current_tact = 0.0;

  /* automation toolbar */
  automation_editor->automation_toolbar = ags_automation_toolbar_new();
  gtk_box_pack_start((GtkBox *) automation_editor,
		     (GtkWidget *) automation_editor->automation_toolbar,
		     FALSE, FALSE,
		     0);

  /* paned */
  automation_editor->paned = (GtkHPaned *) gtk_hpaned_new();
  gtk_box_pack_start((GtkBox *) automation_editor,
		     (GtkWidget *) automation_editor->paned,
		     TRUE, TRUE,
		     0);

  /* machine selector */
  viewport = (GtkViewport *) gtk_viewport_new(NULL,
					      NULL);
  g_object_set(viewport,
	       "shadow-type", GTK_SHADOW_NONE,
	       NULL);
  gtk_paned_pack1((GtkPaned *) automation_editor->paned,
		  (GtkWidget *) viewport,
		  FALSE, TRUE);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(viewport),
		    GTK_WIDGET(scrolled_window));
  automation_editor->machine_selector = g_object_new(AGS_TYPE_MACHINE_SELECTOR,
						     "homogeneous", FALSE,
						     "spacing", 0,
						     NULL);
  automation_editor->machine_selector->flags |= (AGS_MACHINE_SELECTOR_AUTOMATION);
  gtk_label_set_label(automation_editor->machine_selector->label,
		      i18n("automation"));
  
  automation_editor->machine_selector->popup = ags_machine_selector_popup_new(automation_editor->machine_selector);
  g_object_set(automation_editor->machine_selector->menu_button,
	       "menu", automation_editor->machine_selector->popup,
	       NULL);
  
  gtk_scrolled_window_add_with_viewport(scrolled_window,
					(GtkWidget *) automation_editor->machine_selector);

  /* selected machine */
  automation_editor->selected_machine = NULL;

  /* notebook audio, output, input */
  hbox = gtk_hbox_new(FALSE,
		      0);
  gtk_paned_pack2((GtkPaned *) automation_editor->paned,
		  (GtkWidget *) hbox,
		  TRUE,
		  TRUE);
  
  viewport = (GtkViewport *) gtk_viewport_new(NULL,
					      NULL);
  g_object_set(viewport,
	       "shadow-type", GTK_SHADOW_NONE,
	       NULL);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) viewport,
		     TRUE, TRUE,
		     0);
  
  automation_editor->notebook = (GtkNotebook *) gtk_notebook_new();
  gtk_container_add(GTK_CONTAINER(viewport),
		    GTK_WIDGET(automation_editor->notebook));
  
  /* audio */
  table = (GtkTable *) gtk_table_new(4, 3,
				     FALSE);
  gtk_notebook_append_page(automation_editor->notebook,
			   (GtkWidget *) table,
			   gtk_label_new(i18n("audio")));

  /* audio - scrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_HEIGHT), 1.0);
  automation_editor->audio_vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(adjustment);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->audio_vscrollbar,
		   2, 3,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH), 1.0);
  automation_editor->audio_hscrollbar = (GtkHScrollbar *) gtk_hscrollbar_new(adjustment);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->audio_hscrollbar,
		   1, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  /* audio - ruler */
  automation_editor->audio_ruler = ags_ruler_new();
  g_object_set(automation_editor->audio_ruler,
	       "step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_STEP),
	       "large-step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_LARGE_STEP),
	       "small-step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_SMALL_STEP),
	       NULL);
  gtk_widget_set_size_request((GtkWidget *) automation_editor->audio_ruler,
			      -1,
			      (gint) (gui_scale_factor * AGS_RULER_DEFAULT_HEIGHT));
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->audio_ruler,
		   1, 2,
		   1, 2,
		   GTK_FILL | GTK_EXPAND, GTK_FILL,
		   0, 0);

  /* audio - scale */
  automation_editor->audio_scrolled_scale_box = ags_scrolled_scale_box_new();
  gtk_widget_set_vexpand(automation_editor->audio_scrolled_scale_box, TRUE);
  g_object_set(automation_editor->audio_scrolled_scale_box,
	       "margin-top", (gint) (gui_scale_factor * AGS_RULER_DEFAULT_HEIGHT),
	       NULL);

  automation_editor->audio_scrolled_scale_box->scale_box = (AgsScaleBox *) ags_vscale_box_new();
  g_object_set(automation_editor->audio_scrolled_scale_box->scale_box,
	       "fixed-scale-width", (guint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_WIDTH),
	       "fixed-scale-height", (guint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_HEIGHT),
	       NULL);

  gtk_container_add(GTK_CONTAINER(automation_editor->audio_scrolled_scale_box->viewport),
		    GTK_WIDGET(automation_editor->audio_scrolled_scale_box->scale_box));
  gtk_widget_set_size_request((GtkWidget *) automation_editor->audio_scrolled_scale_box,
			      (gint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_WIDTH),
			      -1);
  
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->audio_scrolled_scale_box,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL | GTK_EXPAND,
		   0, 0);

  /* audio - automation edit */
  automation_editor->audio_scrolled_automation_edit_box = ags_scrolled_automation_edit_box_new();

  automation_editor->audio_scrolled_automation_edit_box->automation_edit_box = (AgsAutomationEditBox *) ags_vautomation_edit_box_new();
  g_object_set(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box,
	       "fixed-edit-height", (guint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_HEIGHT),
	       NULL);
  gtk_container_add(GTK_CONTAINER(automation_editor->audio_scrolled_automation_edit_box->viewport),
		    GTK_WIDGET(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box));

  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->audio_scrolled_automation_edit_box,
		   1, 2,
		   2, 3,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  gtk_widget_set_events(GTK_WIDGET(automation_editor->audio_scrolled_automation_edit_box->viewport), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK);
  
  /* output */
  table = (GtkTable *) gtk_table_new(4, 3,
				     FALSE);
  gtk_notebook_append_page(automation_editor->notebook,
			   (GtkWidget *) table,
			   gtk_label_new(i18n("output")));

  /* output - scrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_HEIGHT), 1.0);
  automation_editor->output_vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(adjustment);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->output_vscrollbar,
		   2, 3,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH), 1.0);
  automation_editor->output_hscrollbar = (GtkHScrollbar *) gtk_hscrollbar_new(adjustment);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->output_hscrollbar,
		   1, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  /* output - notebook */
  automation_editor->output_notebook = g_object_new(AGS_TYPE_NOTEBOOK,
						    "homogeneous", FALSE,
						    "spacing", 0,
						    "prefix", i18n("line"),
						    NULL);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->output_notebook,
		   0, 3,
		   0, 1,
		   GTK_FILL | GTK_EXPAND, GTK_FILL,
		   0, 0);

  /* output - ruler */
  automation_editor->output_ruler = ags_ruler_new();
  gtk_widget_set_size_request((GtkWidget *) automation_editor->output_ruler,
			      -1,
			      (gint) (gui_scale_factor * AGS_RULER_DEFAULT_HEIGHT));
  g_object_set(automation_editor->output_ruler,
	       "step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_STEP),
	       "large-step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_LARGE_STEP),
	       "small-step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_SMALL_STEP),
	       NULL);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->output_ruler,
		   1, 2,
		   1, 2,
		   GTK_FILL | GTK_EXPAND, GTK_FILL,
		   0, 0);

  /* output - scale */
  automation_editor->output_scrolled_scale_box = ags_scrolled_scale_box_new();
  gtk_widget_set_vexpand(automation_editor->output_scrolled_scale_box, TRUE);
  g_object_set(automation_editor->output_scrolled_scale_box,
	       "margin-top", (gint) (gui_scale_factor * AGS_RULER_DEFAULT_HEIGHT),
	       NULL);

  automation_editor->output_scrolled_scale_box->scale_box = (AgsScaleBox *) ags_vscale_box_new();
  g_object_set(automation_editor->output_scrolled_scale_box->scale_box,
	       "fixed-scale-width", (guint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_WIDTH),
	       "fixed-scale-height", (guint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_HEIGHT),
	       NULL);

  gtk_container_add(GTK_CONTAINER(automation_editor->output_scrolled_scale_box->viewport),
		    GTK_WIDGET(automation_editor->output_scrolled_scale_box->scale_box));
  gtk_widget_set_size_request((GtkWidget *) automation_editor->output_scrolled_scale_box,
			      (gint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_WIDTH),
			      -1);

  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->output_scrolled_scale_box,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL | GTK_EXPAND,
		   0, 0);

  /* output - automation edit */
  automation_editor->output_scrolled_automation_edit_box = ags_scrolled_automation_edit_box_new();

  automation_editor->output_scrolled_automation_edit_box->automation_edit_box = (AgsAutomationEditBox *) ags_vautomation_edit_box_new();
  g_object_set(automation_editor->output_scrolled_automation_edit_box->automation_edit_box,
	       "fixed-edit-height", (guint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_HEIGHT),
	       NULL);
  gtk_container_add(GTK_CONTAINER(automation_editor->output_scrolled_automation_edit_box->viewport),
		    GTK_WIDGET(automation_editor->output_scrolled_automation_edit_box->automation_edit_box));

  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->output_scrolled_automation_edit_box,
		   1, 2,
		   2, 3,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  gtk_widget_set_events(GTK_WIDGET(automation_editor->output_scrolled_automation_edit_box->viewport), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK);
  
  /* input */
  table = (GtkTable *) gtk_table_new(4, 3,
				     FALSE);
  gtk_notebook_append_page(automation_editor->notebook,
			   (GtkWidget *) table,
			   gtk_label_new(i18n("input")));

  /* input - scrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_HEIGHT), 1.0);
  automation_editor->input_vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(adjustment);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->input_vscrollbar,
		   2, 3,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH), 1.0);
  automation_editor->input_hscrollbar = (GtkHScrollbar *) gtk_hscrollbar_new(adjustment);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->input_hscrollbar,
		   1, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  /* input - notebook */
  automation_editor->input_notebook = g_object_new(AGS_TYPE_NOTEBOOK,
						   "homogeneous", FALSE,
						   "spacing", 0,
						   "prefix", i18n("line"),
						   NULL);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->input_notebook,
		   0, 3,
		   0, 1,
		   GTK_FILL | GTK_EXPAND, GTK_FILL,
		   0, 0);

  /* input - ruler */
  automation_editor->input_ruler = ags_ruler_new();
  gtk_widget_set_size_request((GtkWidget *) automation_editor->input_ruler,
			      -1,
			      (gint) (gui_scale_factor * AGS_RULER_DEFAULT_HEIGHT));
  g_object_set(automation_editor->input_ruler,
	       "step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_STEP),
	       "large-step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_LARGE_STEP),
	       "small-step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_SMALL_STEP),
	       NULL);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->input_ruler,
		   1, 2,
		   1, 2,
		   GTK_FILL | GTK_EXPAND, GTK_FILL,
		   0, 0);

  /* input - scale */
  automation_editor->input_scrolled_scale_box = ags_scrolled_scale_box_new();
  gtk_widget_set_vexpand(automation_editor->input_scrolled_scale_box, TRUE);
  g_object_set(automation_editor->input_scrolled_scale_box,
	       "margin-top", (gint) (gui_scale_factor * AGS_RULER_DEFAULT_HEIGHT),
	       NULL);

  automation_editor->input_scrolled_scale_box->scale_box = (AgsScaleBox *) ags_vscale_box_new();
  g_object_set(automation_editor->input_scrolled_scale_box->scale_box,
	       "fixed-scale-width", (guint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_WIDTH),
	       "fixed-scale-height", (guint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_HEIGHT),
	       NULL);

  gtk_container_add(GTK_CONTAINER(automation_editor->input_scrolled_scale_box->viewport),
		    GTK_WIDGET(automation_editor->input_scrolled_scale_box->scale_box));
  gtk_widget_set_size_request((GtkWidget *) automation_editor->input_scrolled_scale_box,
			      (gint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_WIDTH),
			      -1);

  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->input_scrolled_scale_box,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL | GTK_EXPAND,
		   0, 0);
  gtk_widget_show_all(GTK_WIDGET(automation_editor->input_scrolled_scale_box));
  gtk_widget_show_all(GTK_WIDGET(automation_editor->input_scrolled_scale_box->viewport));
  
  /* input automation edit */
  automation_editor->input_scrolled_automation_edit_box = ags_scrolled_automation_edit_box_new();

  automation_editor->input_scrolled_automation_edit_box->automation_edit_box = (AgsAutomationEditBox *) ags_vautomation_edit_box_new();
  g_object_set(automation_editor->input_scrolled_automation_edit_box->automation_edit_box,
	       "fixed-edit-height", (guint) (gui_scale_factor * AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_HEIGHT),
	       NULL);
  gtk_container_add(GTK_CONTAINER(automation_editor->input_scrolled_automation_edit_box->viewport),
		    GTK_WIDGET(automation_editor->input_scrolled_automation_edit_box->automation_edit_box));

  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->input_scrolled_automation_edit_box,
		   1, 2,
		   2, 3,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  gtk_widget_set_events(GTK_WIDGET(automation_editor->input_scrolled_automation_edit_box->viewport), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK);

  /* focused automation edit */
  automation_editor->focused_automation_edit = NULL;

  /* automation meta */
  automation_editor->automation_meta = ags_automation_meta_new();
  g_object_set(automation_editor->automation_meta,
	       "valign", GTK_ALIGN_START,
	       NULL);  
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) automation_editor->automation_meta,
		     FALSE, FALSE,
		     0);
  
  /* style context */
  style_context = gtk_widget_get_style_context(automation_editor);
  gtk_style_context_add_class(style_context,
			      "editor");
}

void
ags_automation_editor_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_editor_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_editor_connect(AgsConnectable *connectable)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(connectable);

  if((AGS_AUTOMATION_EDITOR_CONNECTED & (automation_editor->flags)) != 0){
    return;
  }

  automation_editor->flags |= AGS_AUTOMATION_EDITOR_CONNECTED;

  /* audio */
  g_signal_connect_after((GObject *) automation_editor->audio_scrolled_automation_edit_box->viewport, "configure_event",
			 G_CALLBACK(ags_automation_editor_audio_edit_configure_event), (gpointer) automation_editor);

  g_signal_connect_after((GObject *) automation_editor->audio_vscrollbar, "value-changed",
			 G_CALLBACK(ags_automation_editor_audio_vscrollbar_value_changed), (gpointer) automation_editor);

  g_signal_connect_after((GObject *) automation_editor->audio_hscrollbar, "value-changed",
			 G_CALLBACK(ags_automation_editor_audio_hscrollbar_value_changed), (gpointer) automation_editor);

  /* output */
  g_signal_connect_after((GObject *) automation_editor->output_scrolled_automation_edit_box->viewport, "configure_event",
			 G_CALLBACK(ags_automation_editor_output_edit_configure_event), (gpointer) automation_editor);

  g_signal_connect_after((GObject *) automation_editor->output_vscrollbar, "value-changed",
			 G_CALLBACK(ags_automation_editor_output_vscrollbar_value_changed), (gpointer) automation_editor);

  g_signal_connect_after((GObject *) automation_editor->output_hscrollbar, "value-changed",
			 G_CALLBACK(ags_automation_editor_output_hscrollbar_value_changed), (gpointer) automation_editor);

  /* input */
  g_signal_connect_after((GObject *) automation_editor->input_scrolled_automation_edit_box->viewport, "configure_event",
			 G_CALLBACK(ags_automation_editor_input_edit_configure_event), (gpointer) automation_editor);

  g_signal_connect_after((GObject *) automation_editor->input_vscrollbar, "value-changed",
			 G_CALLBACK(ags_automation_editor_input_vscrollbar_value_changed), (gpointer) automation_editor);

  g_signal_connect_after((GObject *) automation_editor->input_hscrollbar, "value-changed",
			 G_CALLBACK(ags_automation_editor_input_hscrollbar_value_changed), (gpointer) automation_editor);

  /* machine selector */
  g_signal_connect((GObject *) automation_editor->machine_selector, "changed",
		   G_CALLBACK(ags_automation_editor_machine_changed_callback), (gpointer) automation_editor);

  /* toolbar and selector */
  ags_connectable_connect(AGS_CONNECTABLE(automation_editor->automation_toolbar));
  ags_connectable_connect(AGS_CONNECTABLE(automation_editor->machine_selector));

  ags_connectable_connect(AGS_CONNECTABLE(automation_editor->automation_meta));
}

void
ags_automation_editor_disconnect(AgsConnectable *connectable)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(connectable);

  if((AGS_AUTOMATION_EDITOR_CONNECTED & (automation_editor->flags)) == 0){
    return;
  }

  automation_editor->flags &= (~AGS_AUTOMATION_EDITOR_CONNECTED);

  /* audio */
  g_object_disconnect((GObject *) automation_editor->audio_scrolled_automation_edit_box->viewport,
		      "any_signal::configure_event",
		      G_CALLBACK(ags_automation_editor_audio_edit_configure_event),
		      automation_editor,
		      NULL);

  /* output */
  g_object_disconnect((GObject *) automation_editor->output_scrolled_automation_edit_box->viewport,
		      "any_signal::configure_event",
		      G_CALLBACK(ags_automation_editor_output_edit_configure_event),
		      automation_editor,
		      NULL);

  /* input */
  g_object_disconnect((GObject *) automation_editor->input_scrolled_automation_edit_box->viewport,
		      "any_signal::configure_event",
		      G_CALLBACK(ags_automation_editor_input_edit_configure_event),
		      automation_editor,
		      NULL);
  
  /* toolbar and selector */
  ags_connectable_disconnect(AGS_CONNECTABLE(automation_editor->automation_toolbar)); 
  ags_connectable_disconnect(AGS_CONNECTABLE(automation_editor->machine_selector));

  ags_connectable_disconnect(AGS_CONNECTABLE(automation_editor->automation_meta));
}

void
ags_automation_editor_finalize(GObject *gobject)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_automation_editor_parent_class)->finalize(gobject);
}

void
ags_automation_editor_reset_audio_scrollbar(AgsAutomationEditor *automation_editor)
{
  AgsAutomationToolbar *automation_toolbar;

  GtkAdjustment *audio_vscrollbar_adjustment;
  GtkAdjustment *audio_hscrollbar_adjustment;

  GtkAllocation automation_edit_box_allocation;
  GtkAllocation viewport_allocation;
  
  GList *list_start, *list;

  gdouble old_h_upper;
  gdouble v_upper, h_upper;
  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;

  automation_toolbar = automation_editor->automation_toolbar;

  /* audio */
  gtk_widget_get_allocation(GTK_WIDGET(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box),
			    &automation_edit_box_allocation);

  gtk_widget_get_allocation(GTK_WIDGET(automation_editor->audio_scrolled_automation_edit_box->viewport),
			    &viewport_allocation);
  
  /* reset vertical scrollbar */
  v_upper = automation_edit_box_allocation.height - viewport_allocation.height;

  if(v_upper < 0.0){
    v_upper = 0.0;
  }

  audio_vscrollbar_adjustment = gtk_range_get_adjustment(automation_editor->audio_vscrollbar);
  
  gtk_adjustment_set_upper(audio_vscrollbar_adjustment,
			   v_upper);

  gtk_adjustment_set_upper(gtk_viewport_get_vadjustment(automation_editor->audio_scrolled_automation_edit_box->viewport),
			   v_upper);
  gtk_adjustment_set_upper(gtk_viewport_get_vadjustment(automation_editor->audio_scrolled_scale_box->viewport),
			   v_upper);
  
  /* reset horizontal scrollbar */
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom) - 2.0);

  /* upper */
  audio_hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_editor->audio_hscrollbar));

  old_h_upper = gtk_adjustment_get_upper(audio_hscrollbar_adjustment);

  zoom_correction = 1.0 / 16;

  map_width = ((double) AGS_AUTOMATION_EDITOR_MAX_CONTROLS * zoom * zoom_correction);
  h_upper = map_width - automation_edit_box_allocation.width;

  if(h_upper < 0.0){
    h_upper = 0.0;
  }
  
  gtk_adjustment_set_upper(automation_editor->audio_ruler->adjustment,
			   h_upper);

  gtk_adjustment_set_upper(audio_hscrollbar_adjustment,
			   h_upper);

  /* automation edit */
  list_start =
    list = gtk_container_get_children(GTK_CONTAINER(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box));

  while(list != NULL){
    GtkAdjustment *adjustment;

    adjustment = gtk_range_get_adjustment(GTK_RANGE(AGS_AUTOMATION_EDIT(list->data)->hscrollbar));
    
    gtk_adjustment_set_upper(adjustment,
			     h_upper);    

    list = list->next;
  }

  g_list_free(list_start);

  /* reset value */
  if(old_h_upper != 0.0){
#if 0
    gtk_adjustment_set_value(audio_hscrollbar_adjustment,
			     gtk_adjustment_get_value(audio_hscrollbar_adjustment) / old_h_upper * h_upper);
#endif
  }
}

void
ags_automation_editor_reset_output_scrollbar(AgsAutomationEditor *automation_editor)
{
  AgsAutomationToolbar *automation_toolbar;

  GtkAdjustment *output_vscrollbar_adjustment;
  GtkAdjustment *output_hscrollbar_adjustment;
  
  GtkAllocation automation_edit_box_allocation;
  GtkAllocation viewport_allocation;
  
  GList *list_start, *list;
  
  gdouble old_h_upper;
  gdouble v_upper, h_upper;
  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;

  automation_toolbar = automation_editor->automation_toolbar;

  /* output */
  gtk_widget_get_allocation(GTK_WIDGET(automation_editor->output_scrolled_automation_edit_box->automation_edit_box),
			    &automation_edit_box_allocation);

  gtk_widget_get_allocation(GTK_WIDGET(automation_editor->output_scrolled_automation_edit_box->viewport),
			    &viewport_allocation);
  
  /* reset vertical scrollbar */
  v_upper = automation_edit_box_allocation.height - viewport_allocation.height;

  if(v_upper < 0.0){
    v_upper = 0.0;
  }

  output_vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_editor->output_vscrollbar));
  
  gtk_adjustment_set_upper(output_vscrollbar_adjustment,
			   v_upper);

  gtk_adjustment_set_upper(gtk_viewport_get_vadjustment(automation_editor->output_scrolled_automation_edit_box->viewport),
			   v_upper);
  gtk_adjustment_set_upper(gtk_viewport_get_vadjustment(automation_editor->output_scrolled_scale_box->viewport),
			   v_upper);

  /* reset horizontal scrollbar */
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom) - 2.0);

  /* upper */
  output_hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_editor->output_hscrollbar));
  
  old_h_upper = gtk_adjustment_get_upper(output_hscrollbar_adjustment);

  zoom_correction = 1.0 / 16;

  map_width = ((double) AGS_AUTOMATION_EDITOR_MAX_CONTROLS * zoom * zoom_correction);
  h_upper = map_width - automation_edit_box_allocation.width;

  if(h_upper < 0.0){
    h_upper = 0.0;
  }
  
  gtk_adjustment_set_upper(automation_editor->output_ruler->adjustment,
			   h_upper);

  gtk_adjustment_set_upper(output_hscrollbar_adjustment,
			   h_upper);

  /* automation edit */
  list_start =
    list = gtk_container_get_children(GTK_CONTAINER(automation_editor->output_scrolled_automation_edit_box->automation_edit_box));

  while(list != NULL){
    GtkAdjustment *adjustment;

    adjustment = gtk_range_get_adjustment(GTK_RANGE(AGS_AUTOMATION_EDIT(list->data)->hscrollbar));

    gtk_adjustment_set_upper(adjustment,
			     h_upper);
    

    list = list->next;
  }

  g_list_free(list_start);

  /* reset value */
  if(old_h_upper != 0.0){
#if 0
    gtk_adjustment_set_value(output_hscrollbar_adjustment,
			     gtk_adjustment_get_value(output_hscrollbar_adjustment) / old_h_upper * h_upper);
#endif
  }
}

void
ags_automation_editor_reset_input_scrollbar(AgsAutomationEditor *automation_editor)
{
  AgsAutomationToolbar *automation_toolbar;

  GtkAdjustment *input_vscrollbar_adjustment;
  GtkAdjustment *input_hscrollbar_adjustment;
  
  GtkAllocation automation_edit_box_allocation;
  GtkAllocation viewport_allocation;

  GList *list_start, *list;
  
  gdouble old_h_upper;
  gdouble v_upper, h_upper;
  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;
  
  automation_toolbar = automation_editor->automation_toolbar;

  /* input */
  gtk_widget_get_allocation(GTK_WIDGET(automation_editor->input_scrolled_automation_edit_box->automation_edit_box),
			    &automation_edit_box_allocation);

  gtk_widget_get_allocation(GTK_WIDGET(automation_editor->input_scrolled_automation_edit_box->viewport),
			    &viewport_allocation);

  /* reset vertical scrollbar */
  v_upper = automation_edit_box_allocation.height - viewport_allocation.height;

  if(v_upper < 0.0){
    v_upper = 0.0;
  }

  input_vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_editor->input_vscrollbar));

  gtk_adjustment_set_upper(input_vscrollbar_adjustment,
			   v_upper);

  gtk_adjustment_set_upper(gtk_viewport_get_vadjustment(automation_editor->input_scrolled_automation_edit_box->viewport),
			   v_upper);
  gtk_adjustment_set_upper(gtk_viewport_get_vadjustment(automation_editor->input_scrolled_scale_box->viewport),
			   v_upper);

  /* reset horizontal scrollbar */
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom) - 2.0);

  /* upper */
  input_hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_editor->input_hscrollbar));

  old_h_upper = gtk_adjustment_get_upper(input_hscrollbar_adjustment);

  zoom_correction = 1.0 / 16;

  map_width = ((double) AGS_AUTOMATION_EDITOR_MAX_CONTROLS * zoom * zoom_correction);
  h_upper = map_width - automation_edit_box_allocation.width;

  if(h_upper < 0.0){
    h_upper = 0.0;
  }
  
  gtk_adjustment_set_upper(automation_editor->input_ruler->adjustment,
			   h_upper);

  gtk_adjustment_set_upper(input_hscrollbar_adjustment,
			   h_upper);

  /* automation edit */
  list_start =
    list = gtk_container_get_children(GTK_CONTAINER(automation_editor->input_scrolled_automation_edit_box->automation_edit_box));

  while(list != NULL){
    GtkAdjustment *adjustment;

    adjustment = gtk_range_get_adjustment(GTK_RANGE(AGS_AUTOMATION_EDIT(list->data)->hscrollbar));

    gtk_adjustment_set_upper(adjustment,
			     h_upper);
    

    list = list->next;
  }

  g_list_free(list_start);

  /* reset value */
  if(old_h_upper != 0.0){
#if 0
    gtk_adjustment_set_value(input_hscrollbar_adjustment,
			     gtk_adjustment_get_value(input_hscrollbar_adjustment) / old_h_upper * h_upper);
#endif
  }
}

void
ags_automation_editor_real_machine_changed(AgsAutomationEditor *automation_editor, AgsMachine *machine)
{  
  AgsMachine *old_machine;
  
  AgsConfig *config;

  GList *list_start, *list;
  GList *tab;
  
  gchar *str;
  
  gdouble gui_scale_factor;
  guint length;
  guint output_pads, input_pads;
  guint audio_channels;
  guint i;

  GRecMutex *audio_mutex;
  GRecMutex *automation_mutex;

  /* disconnect set pads - old */
  old_machine = automation_editor->selected_machine;

  if(old_machine != NULL){
    g_object_disconnect(old_machine,
			"any_signal::resize-audio-channels",
			G_CALLBACK(ags_automation_editor_resize_audio_channels_callback),
			(gpointer) automation_editor,
			"any_signal::resize-pads",
			G_CALLBACK(ags_automation_editor_resize_pads_callback),
			(gpointer) automation_editor,
			NULL);
  }

  config = ags_config_get_instance();

  /* scale factor */
  gui_scale_factor = 1.0;
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "gui-scale");

  if(str != NULL){
    gui_scale_factor = g_ascii_strtod(str,
				      NULL);

    g_free(str);
  }

  /* get audio mutex */
  audio_mutex = NULL;
  
  if(machine != NULL){
    audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(machine->audio);
  }
  
  /* notebook - remove tabs */
  length = g_list_length(automation_editor->output_notebook->tab);
  
  for(i = 0; i < length; i++){
    ags_notebook_remove_tab(automation_editor->output_notebook,
			    0);
  }

  length = g_list_length(automation_editor->input_notebook->tab);
  
  for(i = 0; i < length; i++){
    ags_notebook_remove_tab(automation_editor->input_notebook,
			    0);
  }

  /* notebook - add tabs */
  if(machine != NULL){
    g_rec_mutex_lock(audio_mutex);

    output_pads = machine->audio->output_pads;
    input_pads = machine->audio->input_pads;
    
    audio_channels = machine->audio->audio_channels;
    
    g_rec_mutex_unlock(audio_mutex);

    for(i = 0; i < output_pads * audio_channels; i++){
      ags_notebook_insert_tab(automation_editor->output_notebook,
			      i);

      tab = automation_editor->output_notebook->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
    }

    for(i = 0; i < input_pads * audio_channels; i++){
      ags_notebook_insert_tab(automation_editor->input_notebook,
			      i);

      tab = automation_editor->input_notebook->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
    }
  }

  /*
   * destroy old
   */
  /* audio */
  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(automation_editor->audio_scrolled_scale_box->scale_box));

  while(list != NULL){
    gtk_widget_destroy(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);

  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box));

  while(list != NULL){
    g_object_disconnect(AGS_AUTOMATION_EDIT(list->data)->hscrollbar,
			"any_signal::value-changed",
			G_CALLBACK(ags_automation_editor_audio_automation_edit_hscrollbar_value_changed),
			(gpointer) automation_editor,
			NULL);
    
    gtk_widget_destroy(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);
  
  /* output */
  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(automation_editor->output_scrolled_scale_box->scale_box));

  while(list != NULL){
    gtk_widget_destroy(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);

  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(automation_editor->output_scrolled_automation_edit_box->automation_edit_box));

  while(list != NULL){
    g_object_disconnect(AGS_AUTOMATION_EDIT(list->data)->hscrollbar,
			"any_signal::value-changed",
			G_CALLBACK(ags_automation_editor_output_automation_edit_hscrollbar_value_changed),
			(gpointer) automation_editor,
			NULL);

    gtk_widget_destroy(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);
  
  /* input */
  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(automation_editor->input_scrolled_scale_box->scale_box));

  while(list != NULL){
    gtk_widget_destroy(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);

  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(automation_editor->input_scrolled_automation_edit_box->automation_edit_box));

  while(list != NULL){
    g_object_disconnect(AGS_AUTOMATION_EDIT(list->data)->hscrollbar,
			"any_signal::value-changed",
			G_CALLBACK(ags_automation_editor_input_automation_edit_hscrollbar_value_changed),
			(gpointer) automation_editor,
			NULL);

    gtk_widget_destroy(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);
  
  /*
   * add new
   */
  if(machine != NULL){  
    AgsChannel *start_channel, *channel;

    GList *automation_port;
    GList *start_audio_port;
    GList *start_output_port;
    GList *start_input_port;
    GList *start_port, *port;
    
    /* audio */
    automation_port = machine->enabled_automation_port;

    start_audio_port = ags_audio_collect_all_audio_ports(machine->audio);
    
    g_object_get(machine->audio,
		 "output", &start_channel,
		 NULL);

    channel = start_channel;

    start_output_port = NULL;
    
    while(channel != NULL){
      AgsChannel *next_channel;

      GList *list;
      
      list = ags_channel_collect_all_channel_ports(channel);

      if(start_output_port == NULL){
	start_output_port = list;
      }else{
	start_output_port = g_list_concat(start_output_port,
					  list);
      }
      
      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    g_object_get(machine->audio,
		 "input", &start_channel,
		 NULL);

    channel = start_channel;

    start_input_port = NULL;
    
    while(channel != NULL){
      AgsChannel *next_channel;

      GList *list;
      
      list = ags_channel_collect_all_channel_ports(channel);

      if(start_input_port == NULL){
	start_input_port = list;
      }else{
	start_input_port = g_list_concat(start_input_port,
					 list);
      }
      
      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }
    
    while(automation_port != NULL){
      AgsAutomationEdit *automation_edit;
      AgsScale *scale;

      AgsPluginPort *plugin_port;
      
      gchar *control_name;

      gdouble upper, lower;
      gdouble default_value;

      g_message("%s", AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->control_name);
      
      /* scale */
      scale = ags_scale_new();
      g_object_set(scale,
		   "scale-width", (guint) (gui_scale_factor * AGS_SCALE_DEFAULT_SCALE_WIDTH),
		   "scale-height", (guint) (gui_scale_factor * AGS_SCALE_DEFAULT_SCALE_HEIGHT),
		   NULL);

      control_name = g_strdup(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->control_name);

      start_port = NULL;
      
      if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == G_TYPE_NONE){
	start_port = start_audio_port;
      }else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_OUTPUT){
	start_port = start_output_port;
      }else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_INPUT){
	start_port = start_input_port;
      }

      upper = 0.0;
      lower = 0.0;

      default_value = 0.0;

      port = start_port;

      while(port != NULL){
	AgsPluginPort *plugin_port;

	gchar *specifier;

	gboolean success;

	g_object_get(port->data,
		     "specifier", &specifier,
		     "plugin-port", &plugin_port,
		     NULL);

	success = FALSE;	
	
	if(!g_strcmp0(specifier,
		      control_name)){
	  GValue *upper_value;
	  GValue *lower_value;
	  GValue *value;
	  
	  g_object_get(plugin_port,
		       "upper-value", &upper_value,
		       "lower-value", &lower_value,
		       "default-value", &value,
		       NULL);

	  upper = g_value_get_float(upper_value);
	  lower = g_value_get_float(lower_value);

	  default_value = g_value_get_float(value);

	  g_value_unset(upper_value);
	  g_free(upper_value);

	  g_value_unset(lower_value);
	  g_free(lower_value);

	  g_value_unset(value);
	  g_free(value);
	  
	  success = TRUE; 
	}
	
	if(plugin_port != NULL){
	  g_object_unref(plugin_port);
	}

	if(success){
	  break;
	}
	
	port = port->next;
      }

      g_object_set(scale,
		   "control-name", control_name,
		   "upper", upper,
		   "lower", lower,
		   "default-value", default_value,
		   NULL);

      if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == G_TYPE_NONE){
	gtk_box_pack_start(GTK_BOX(automation_editor->audio_scrolled_scale_box->scale_box),
			   GTK_WIDGET(scale),
			   FALSE, TRUE,
			   AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
      }else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_OUTPUT){
	gtk_box_pack_start(GTK_BOX(automation_editor->output_scrolled_scale_box->scale_box),
			   GTK_WIDGET(scale),
			   FALSE, TRUE,
			   AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
      }else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_INPUT){
	gtk_box_pack_start(GTK_BOX(automation_editor->input_scrolled_scale_box->scale_box),
			   GTK_WIDGET(scale),
			   FALSE, TRUE,
			   AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
      }

      {
	GtkAllocation allocation;
	  
	gtk_widget_get_allocation(scale, &allocation);

	g_message("%d|%d %d,%d", allocation.x, allocation.y, allocation.width, allocation.height);
      }
	
      gtk_widget_show(GTK_WIDGET(scale));
	  
      /* automation edit */
      automation_edit = ags_automation_edit_new();

      g_object_set(automation_edit,
		   "channel-type", AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type,
		   "control-specifier", AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->control_name,
		   "control-name", control_name,
		   "upper", upper,
		   "lower", lower,
		   "default-value", default_value,
		   NULL);

      if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == G_TYPE_NONE){
	gtk_box_pack_start(GTK_BOX(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box),
			   GTK_WIDGET(automation_edit),
			   FALSE, TRUE,
			   AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
      }else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_OUTPUT){
	gtk_box_pack_start(GTK_BOX(automation_editor->output_scrolled_automation_edit_box->automation_edit_box),
			   GTK_WIDGET(automation_edit),
			   FALSE, TRUE,
			   AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
      }else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_INPUT){
	gtk_box_pack_start(GTK_BOX(automation_editor->input_scrolled_automation_edit_box->automation_edit_box),
			   GTK_WIDGET(automation_edit),
			   FALSE, TRUE,
			   AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
      }

      ags_connectable_connect(AGS_CONNECTABLE(automation_edit));
      gtk_widget_show(GTK_WIDGET(automation_edit));

      if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == G_TYPE_NONE){
	g_signal_connect_after((GObject *) automation_edit->hscrollbar, "value-changed",
			       G_CALLBACK(ags_automation_editor_audio_automation_edit_hscrollbar_value_changed), (gpointer) automation_editor);
      }else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_OUTPUT){
	g_signal_connect_after((GObject *) automation_edit->hscrollbar, "value-changed",
			       G_CALLBACK(ags_automation_editor_output_automation_edit_hscrollbar_value_changed), (gpointer) automation_editor);
      }else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_INPUT){
	g_signal_connect_after((GObject *) automation_edit->hscrollbar, "value-changed",
			       G_CALLBACK(ags_automation_editor_input_automation_edit_hscrollbar_value_changed), (gpointer) automation_editor);
      }
      
      g_free(control_name);      
      
      /* iterate */
      automation_port = automation_port->next;
    }

    g_list_free_full(start_audio_port,
		     (GDestroyNotify) g_object_unref);

    g_list_free_full(start_output_port,
		     (GDestroyNotify) g_object_unref);

    g_list_free_full(start_input_port,
		     (GDestroyNotify) g_object_unref);
  }
  
  /* selected machine */
  automation_editor->selected_machine = machine;
  
  /* connect set-pads - new */
  if(machine != NULL){
    g_signal_connect_after(machine, "resize-audio-channels",
			   G_CALLBACK(ags_automation_editor_resize_audio_channels_callback), automation_editor);

    g_signal_connect_after(machine, "resize-pads",
			   G_CALLBACK(ags_automation_editor_resize_pads_callback), automation_editor);
  }  
}
 
/**
 * ags_automation_editor_machine_changed:
 * @automation_editor: an #AgsAutomationEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine changed of automation editor.
 *
 * Since: 3.0.0
 */
void
ags_automation_editor_machine_changed(AgsAutomationEditor *automation_editor, AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_AUTOMATION_EDITOR(automation_editor));

  g_object_ref((GObject *) automation_editor);
  g_signal_emit((GObject *) automation_editor,
		automation_editor_signals[MACHINE_CHANGED], 0,
		machine);
  g_object_unref((GObject *) automation_editor);
}

void
ags_automation_editor_add_acceleration(AgsAutomationEditor *automation_editor,
				       AgsAcceleration *acceleration)
{
  AgsMachine *machine;
  AgsNotebook *notebook;

  AgsAutomation *automation;

  AgsTimestamp *timestamp;

  GList *list_automation;

  gint i;
  
  if(!AGS_IS_AUTOMATION_EDITOR(automation_editor) ||
     !AGS_IS_ACCELERATION(acceleration) ||
     automation_editor->focused_automation_edit == NULL){
    return;
  }

  if(automation_editor->selected_machine != NULL){
    AgsChannel *start_output, *start_input;
    AgsChannel *channel, *nth_channel;

    GRecMutex *audio_mutex;

    machine = automation_editor->selected_machine;

    notebook = NULL;

    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }

    /* get audio mutex */
    audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(machine->audio);

    /* get some fields */
    g_rec_mutex_lock(audio_mutex);

    start_output = machine->audio->output;

    if(start_output != NULL){
      g_object_ref(start_output);
    }
    
    start_input = machine->audio->input;

    if(start_input != NULL){
      g_object_ref(start_input);
    }
    
    g_rec_mutex_unlock(audio_mutex);
    
    /* check all active tabs */
    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
    timestamp->timer.ags_offset.offset = AGS_AUTOMATION_DEFAULT_OFFSET * floor(acceleration->x / AGS_AUTOMATION_DEFAULT_OFFSET);

    i = 0;

    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){      
      AgsAcceleration *new_acceleration;

      GList *start_play_port, *play_port;
      GList *start_recall_port, *recall_port;
      GList *start_list, *list;

      play_port = NULL;
      recall_port = NULL;
      
      if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
	nth_channel = ags_channel_nth(start_output,
				      i);

	play_port =
	  start_play_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											   automation_editor->focused_automation_edit->control_name,
											   TRUE);

	recall_port =
	  start_recall_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											     automation_editor->focused_automation_edit->control_name,
											     FALSE);

	if(nth_channel != NULL){
	  g_object_unref(nth_channel);
	}
      }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
	nth_channel = ags_channel_nth(start_input,
				      i);

	play_port =
	  start_play_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											   automation_editor->focused_automation_edit->control_name,
											   TRUE);

	recall_port =
	  start_recall_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											     automation_editor->focused_automation_edit->control_name,
											     FALSE);

	if(nth_channel != NULL){
	  g_object_unref(nth_channel);
	}
      }else{
	play_port =
	  start_play_port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
										       automation_editor->focused_automation_edit->control_name,
										       TRUE);
	
	recall_port =
	  start_recall_port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
											 automation_editor->focused_automation_edit->control_name,
											 FALSE);
      }

      /* play port */
      while(play_port != NULL){
	AgsPort *current_port;
	
	current_port = play_port->data;

	g_object_get(current_port,
		     "automation", &start_list,
		     NULL);
	
	list = ags_automation_find_near_timestamp(start_list, i,
						  timestamp);
	
	if(list == NULL){
	  automation = ags_automation_new(G_OBJECT(machine->audio),
					  i,
					  automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name);
	  automation->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;

	  g_object_set(automation,
		       "port", current_port,
		       NULL);

	  /* add to audio */
	  ags_audio_add_automation(machine->audio,
				   (GObject *) automation);

	  /* add to port */
	  ags_port_add_automation(current_port,
				  (GObject *) automation);
	}else{
	  automation = list->data;
	}
	
	new_acceleration = ags_acceleration_duplicate(acceleration);
	ags_automation_add_acceleration(automation,
					new_acceleration,
					FALSE);

	g_list_free_full(start_list,
			 g_object_unref);
	
	/* iterate */
	play_port = play_port->next;
      }

      g_list_free_full(start_play_port,
		       g_object_unref);
      
      /* recall port */
      if(recall_port != NULL){
	AgsPort *current_port;
	
	current_port = recall_port->data;

	g_object_get(current_port,
		     "automation", &start_list,
		     NULL);

	list = ags_automation_find_near_timestamp(start_list, i,
						  timestamp);
	
	if(list == NULL){
	  automation = ags_automation_new(G_OBJECT(machine->audio),
					  i,
					  automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name);
	  automation->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;
	  g_object_set(automation,
		       "port", current_port,
		       NULL);
	  
	  /* add to audio */
	  ags_audio_add_automation(machine->audio,
				   (GObject *) automation);

	  /* add to port */
	  ags_port_add_automation(current_port,
				  (GObject *) automation);
	}else{
	  automation = list->data;
	}
	
	new_acceleration = ags_acceleration_duplicate(acceleration);
	ags_automation_add_acceleration(automation,
					new_acceleration,
					FALSE);

	g_list_free_full(start_list,
			 g_object_unref);
	
	/* iterate */
	recall_port = recall_port->next;
      }      

      g_list_free_full(start_recall_port,
		       g_object_unref);

      if(notebook == NULL){
	break;
      }
      
      i++;
    }
    
    /* unref */
    if(start_output != NULL){
      g_object_unref(start_output);
    }

    if(start_input != NULL){
      g_object_unref(start_input);
    }

    g_object_unref(timestamp);
    
    gtk_widget_queue_draw(GTK_WIDGET(automation_editor->focused_automation_edit));
  }
}

void
ags_automation_editor_delete_acceleration(AgsAutomationEditor *automation_editor,
					  guint x, gdouble y)
{
  AgsMachine *machine;
  AgsNotebook *notebook;

  GtkAdjustment *automation_edit_vscrollbar_adjustment;

  AgsAutomation *automation;

  AgsTimestamp *timestamp;

  GtkAllocation automation_edit_allocation;

  GList *start_list_automation, *list_automation;

  gdouble c_range;
  guint g_range;
  guint scan_x;
  gdouble scan_y;
  gboolean success;
  gint i, j, j_step, j_stop;

  if(!AGS_IS_AUTOMATION_EDITOR(automation_editor) ||
     automation_editor->focused_automation_edit == NULL){
    return;
  }

  if(automation_editor->selected_machine != NULL){
    machine = automation_editor->selected_machine;

    notebook = NULL;

    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }
  
    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_editor->focused_automation_edit->flags)) != 0){
      c_range = exp(automation_editor->focused_automation_edit->upper) - exp(automation_editor->focused_automation_edit->lower);
    }else{
      c_range = automation_editor->focused_automation_edit->upper - automation_editor->focused_automation_edit->lower;
    }

    gtk_widget_get_allocation(GTK_WIDGET(automation_editor->focused_automation_edit->drawing_area),
			      &automation_edit_allocation);
    
    automation_edit_vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(automation_editor->focused_automation_edit->vscrollbar));
    
    g_range = gtk_adjustment_get_upper(automation_edit_vscrollbar_adjustment) + automation_edit_allocation.height;

    /* check all active tabs */
    g_object_get(machine->audio,
		 "automation", &start_list_automation,
		 NULL);

    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
    timestamp->timer.ags_offset.offset = AGS_AUTOMATION_DEFAULT_OFFSET * floor(x / AGS_AUTOMATION_DEFAULT_OFFSET);

    i = 0;

    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      AgsAcceleration *new_acceleration;
      
      list_automation = start_list_automation;
      
      while((list_automation = ags_automation_find_near_timestamp_extended(list_automation, i,
									   automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name,
									   timestamp)) != NULL){
      
	if(list_automation != NULL){
	  automation = list_automation->data;
	}else{
	  if(notebook == NULL){
	    break;
	  }

	  list_automation = list_automation->next;
	  
	  i++;
	
	  continue;
	}

	success = FALSE;
      
	j = 0;
	j_step = 1;
	j_stop = 4;

	while(!success &&
	      exp2(j_step) <= AGS_AUTOMATION_EDIT_DEFAULT_SCAN_WIDTH){
	  scan_x = (-1 * j_step + floor(j / (2 * j_step)));

	  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_editor->focused_automation_edit->flags)) != 0){
	    scan_y = log((-1 * j_step + floor(j % (2 * j_step))) / g_range) * c_range;	
	  }else{
	    scan_y = ((-1 * j_step + floor(j % (2 * j_step))) / g_range) * c_range;	
	  }

	  success = ags_automation_remove_acceleration_at_position(automation,
								   x - scan_x, y - scan_y);
	
	  j++;
	
	  if(j >= j_stop){
	    j_step++;
	    j_stop = exp2(j_step + 1);
	  }
	}

	
	list_automation = list_automation->next;
      }
      
      if(notebook == NULL){
	break;
      }

      i++;
    }

    g_list_free_full(start_list_automation,
		     g_object_unref);

    /* queue draw */
    gtk_widget_queue_draw(GTK_WIDGET(automation_editor->focused_automation_edit));
  }
}

void
ags_automation_editor_select_region(AgsAutomationEditor *automation_editor,
				    guint x0, gdouble y0,
				    guint x1, gdouble y1)
{
  AgsMachine *machine;
  AgsNotebook *notebook;
  
  AgsAutomation *automation;

  AgsTimestamp *timestamp;

  GList *start_list_automation, *list_automation;

  gint i;
  
  if(!AGS_IS_AUTOMATION_EDITOR(automation_editor) ||
     automation_editor->focused_automation_edit == NULL){
    return;
  }
  
  if(automation_editor->selected_machine != NULL){
    machine = automation_editor->selected_machine;
  
    notebook = NULL;

    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }

    /* swap values if needed */
    if(x0 > x1){
      guint tmp;

      tmp = x0;
      
      x0 = x1;
      x1 = tmp;
    }

    if(y0 > y1){
      gdouble tmp;

      tmp = y0;
      
      y0 = y1;
      y1 = tmp;
    }

    /* check all active tabs */
    g_object_get(machine->audio,
		 "automation", &start_list_automation,
		 NULL);

    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
    i = 0;

    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){      
      list_automation = start_list_automation;
      
      timestamp->timer.ags_offset.offset = AGS_AUTOMATION_DEFAULT_OFFSET * floor(x0 / AGS_AUTOMATION_DEFAULT_OFFSET);

      while(timestamp->timer.ags_offset.offset < (AGS_AUTOMATION_DEFAULT_OFFSET * floor(x1 / AGS_AUTOMATION_DEFAULT_OFFSET)) + AGS_AUTOMATION_DEFAULT_OFFSET){
	while((list_automation = ags_automation_find_near_timestamp_extended(list_automation, i,
									     automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name,
									     timestamp)) != NULL){
	  ags_automation_add_region_to_selection(list_automation->data,
						 x0, y0,
						 x1, y1,
						 TRUE);
	      
	  /* iterate */
	      
	  list_automation = list_automation->next;
	}

	timestamp->timer.ags_offset.offset += AGS_AUTOMATION_DEFAULT_OFFSET;
      }
      
      if(notebook == NULL){
	break;
      }
      
      i++;
    }
    
    g_list_free_full(start_list_automation,
		     g_object_unref);

    /* queue draw */
    gtk_widget_queue_draw(GTK_WIDGET(automation_editor->focused_automation_edit));
  }
}

void
ags_automation_editor_select_all(AgsAutomationEditor *automation_editor)
{
  AgsMachine *machine;
  AgsNotebook *notebook;

  GType channel_type;
  
  GList *start_list_automation, *list_automation;

  gchar *control_name;

  guint line;
  gint i;

  if(!AGS_IS_AUTOMATION_EDITOR(automation_editor)){
    return;
  }
  
  if(automation_editor->selected_machine != NULL){
    machine = automation_editor->selected_machine;
  
    notebook = NULL;

    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }

    /* check all active tabs */
    g_object_get(machine->audio,
		 "automation", &start_list_automation,
		 NULL);

    i = 0;
    
    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      list_automation = start_list_automation;
      
      while(list_automation != NULL){
	g_object_get(list_automation->data,
		     "line", &line,
		     "channel-type", &channel_type,
		     "control-name", &control_name,
		     NULL);

	if(i != line ||
	   channel_type != automation_editor->focused_automation_edit->channel_type ||
	   !g_strcmp0(control_name,
		      automation_editor->focused_automation_edit->control_name) != TRUE){
	  list_automation = list_automation->next;

	  continue;
	}
	
	ags_automation_add_all_to_selection(AGS_AUTOMATION(list_automation->data));
	
	list_automation = list_automation->next;
      }

      if(notebook == NULL){
	break;
      }

      i++;
    }
    
    g_list_free_full(start_list_automation,
		     g_object_unref);

    /* queue draw */
    gtk_widget_queue_draw(GTK_WIDGET(automation_editor->focused_automation_edit));
  }
}

gint
ags_automation_editor_paste_automation_all(AgsAutomationEditor *automation_editor,
					   AgsNotebook *notebook,
					   AgsMachine *machine,
					   xmlNode *automation_node,
					   AgsTimestamp *timestamp,
					   gboolean match_line, gboolean no_duplicates,
					   guint position_x, guint position_y,
					   gboolean paste_from_position,
					   gint *last_x)
{    
  AgsChannel *start_output, *start_input;
  AgsChannel *channel, *nth_channel;
  AgsAutomation *automation;
  
  GList *start_list_automation, *list_automation;
    
  guint first_x;
  guint current_x;
  gint i;

  GRecMutex *audio_mutex;
    
  first_x = -1;

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(machine->audio);

  /* get some fields */
  g_rec_mutex_lock(audio_mutex);

  start_output = machine->audio->output;

  if(start_output != NULL){
    g_object_ref(start_output);
  }
    
  start_input = machine->audio->input;

  if(start_input != NULL){
    g_object_ref(start_input);
  }
    
  g_rec_mutex_unlock(audio_mutex);
    
  /*  */
  i = 0;
		
  while(notebook == NULL ||
	(i = ags_notebook_next_active_tab(notebook,
					  i)) != -1){		  
    g_object_get(machine->audio,
		 "automation", &start_list_automation,
		 NULL);
      
    list_automation = ags_automation_find_near_timestamp_extended(start_list_automation, i,
								  automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name,
								  timestamp);

    if(list_automation == NULL){
      GList *start_play_port, *play_port;
      GList *start_recall_port, *recall_port;
	
      AgsPort *current_port;

      play_port = NULL;
      recall_port = NULL;

      if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
	nth_channel = ags_channel_nth(start_output,
				      i);
	  
	play_port =
	  start_play_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											   automation_editor->focused_automation_edit->control_name,
											   TRUE);

	recall_port =
	  start_recall_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											     automation_editor->focused_automation_edit->control_name,
											     FALSE);

	if(nth_channel != NULL){
	  g_object_unref(nth_channel);
	}
      }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
	nth_channel = ags_channel_nth(start_input,
				      i);

	play_port =
	  start_play_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											   automation_editor->focused_automation_edit->control_name,
											   TRUE);

	recall_port =
	  start_recall_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(nth_channel,
											     automation_editor->focused_automation_edit->control_name,
											     FALSE);

	if(nth_channel != NULL){
	  g_object_unref(nth_channel);
	}
      }else{
	play_port =
	  start_play_port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
										       automation_editor->focused_automation_edit->control_name,
										       TRUE);
	
	recall_port =
	  start_recall_port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
											 automation_editor->focused_automation_edit->control_name,
											 FALSE);
      }

      /* play port */
      while(play_port != NULL){
	AgsPort *current_port;
	
	current_port = play_port->data;

	automation = ags_automation_new(G_OBJECT(machine->audio),
					i,
					automation_editor->focused_automation_edit->channel_type,
					automation_editor->focused_automation_edit->control_name);
	automation->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;

	g_object_set(automation,
		     "port", current_port,
		     NULL);
	
	/* add to audio */
	ags_audio_add_automation(machine->audio,
				 (GObject *) automation);

	/* add to port */
	ags_port_add_automation(current_port,
				(GObject *) automation);
	  
	/* iterate */
	play_port = play_port->next;
      }

      g_list_free_full(start_play_port,
		       g_object_unref);
      
      /* recall port */
      if(recall_port != NULL){
	AgsPort *current_port;
	
	current_port = recall_port->data;

	automation = ags_automation_new(G_OBJECT(machine->audio),
					i,
					automation_editor->focused_automation_edit->channel_type,
					automation_editor->focused_automation_edit->control_name);
	automation->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;

	g_object_set(automation,
		     "port", current_port,
		     NULL);
	
	/* add to audio */
	ags_audio_add_automation(machine->audio,
				 (GObject *) automation);

	/* add to port */
	ags_port_add_automation(current_port,
				(GObject *) automation);
	  
	/* iterate */
	recall_port = recall_port->next;
      }

      g_list_free_full(start_recall_port,
		       g_object_unref);
    }else{
      automation = AGS_AUTOMATION(list_automation->data);
    }

    g_list_free_full(start_list_automation,
		     g_object_unref);
      
    g_object_get(machine->audio,
		 "automation", &start_list_automation,
		 NULL);

    list_automation = start_list_automation;
      
    while((list_automation = ags_automation_find_near_timestamp_extended(list_automation, i,
									 automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name,
									 timestamp)) != NULL){
      automation = list_automation->data;
	
      if(paste_from_position){
	xmlNode *child;

	guint x_boundary;
	  
	ags_automation_insert_from_clipboard_extended(automation,
						      automation_node,
						      TRUE, position_x,
						      TRUE, position_y,
						      match_line, no_duplicates);

	/* get boundaries */
	child = automation_node->children;
	current_x = 0;
	  
	while(child != NULL){
	  if(child->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(child->name,
			   "note",
			   5)){
	      guint tmp;

	      tmp = g_ascii_strtoull(xmlGetProp(child,
						"x"),
				     NULL,
				     10);

	      if(tmp > current_x){
		current_x = tmp;
	      }
	    }
	  }

	  child = child->next;
	}

	x_boundary = g_ascii_strtoull(xmlGetProp(automation_node,
						 "x_boundary"),
				      NULL,
				      10);


	if(first_x == -1 || x_boundary < first_x){
	  first_x = x_boundary;
	}
	  
	if(position_x > x_boundary){
	  current_x += (position_x - x_boundary);
	}else{
	  current_x -= (x_boundary - position_x);
	}
	  
	if(current_x > last_x[0]){
	  last_x[0] = current_x;
	}	
      }else{
	xmlNode *child;

	ags_automation_insert_from_clipboard(automation,
					     automation_node,
					     FALSE, 0,
					     FALSE, 0);

	/* get boundaries */
	child = automation_node->children;
	current_x = 0;
	  
	while(child != NULL){
	  if(child->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(child->name,
			   "note",
			   5)){
	      guint tmp;

	      tmp = g_ascii_strtoull(xmlGetProp(child,
						"x"),
				     NULL,
				     10);

	      if(tmp > current_x){
		current_x = tmp;
	      }
	    }
	  }

	  child = child->next;
	}

	if(current_x > last_x[0]){
	  last_x[0] = current_x;
	}
      }

      list_automation = list_automation->next;
    }

    g_list_free_full(start_list_automation,
		     g_object_unref);
      
    if(notebook == NULL){
      break;
    }
		  
    i++;
  }

  /* unref */
  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(first_x);
}
  
gint
ags_automation_editor_paste_automation(AgsAutomationEditor *automation_editor,
				       AgsNotebook *notebook,
				       AgsMachine *machine,
				       xmlNode *audio_node,
				       guint position_x, guint position_y,
				       gboolean paste_from_position,
				       gint *last_x)
{
  AgsTimestamp *timestamp;

  xmlNode *automation_list_node, *automation_node;
  xmlNode *timestamp_node;

  guint first_x;
  gboolean match_line, no_duplicates;

  first_x = -1;

  match_line = ((AGS_AUTOMATION_EDITOR_PASTE_MATCH_LINE & (automation_editor->flags)) != 0) ? TRUE: FALSE;
  no_duplicates = ((AGS_AUTOMATION_EDITOR_PASTE_NO_DUPLICATES & (automation_editor->flags)) != 0) ? TRUE: FALSE;
    
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
  timestamp->timer.ags_offset.offset = 0;
    
  /* paste automation */
  automation_list_node = audio_node->children;

  while(automation_list_node != NULL){
    if(automation_list_node->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(automation_list_node->name,
		     "automation-list",
		     14)){
	automation_node = automation_list_node->children;
	  
	while(automation_node != NULL){
	  if(automation_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(automation_node->name,
			   "automation",
			   9)){
	      guint64 offset;
		
	      timestamp_node = automation_node->children;
	      offset = 0;
	  
	      while(timestamp_node != NULL){
		if(timestamp_node->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(timestamp_node->name,
				 "timestamp",
				 10)){
		    offset = g_ascii_strtoull(xmlGetProp(timestamp_node,
							 "offset"),
					      NULL,
					      10);
		      
		    break;
		  }
		}

		timestamp_node = timestamp_node->next;
	      }     
		
	      /* 1st attempt */
	      timestamp->timer.ags_offset.offset = (guint64) AGS_AUTOMATION_DEFAULT_OFFSET * floor((double) position_x / (double) AGS_AUTOMATION_DEFAULT_OFFSET);
		
	      first_x = ags_automation_editor_paste_automation_all(automation_editor,
								   notebook,
								   machine,
								   automation_node,
								   timestamp,
								   match_line, no_duplicates,
								   position_x, position_y,
								   paste_from_position,
								   last_x);

	      /* 2nd attempt */
	      timestamp->timer.ags_offset.offset += AGS_AUTOMATION_DEFAULT_OFFSET;

	      ags_automation_editor_paste_automation_all(automation_editor,
							 notebook,
							 machine,
							 automation_node,
							 timestamp,
							 match_line, no_duplicates,
							 position_x, position_y,
							 paste_from_position,
							 last_x);
		
	    }
	  }

	  automation_node = automation_node->next;
	}	  
      }
    }

    automation_list_node = automation_list_node->next;
  }    

  g_object_unref(timestamp);

  return(first_x);
}

void
ags_automation_editor_paste(AgsAutomationEditor *automation_editor)
{
  AgsMachine *machine;
  AgsAutomationEdit *automation_edit;
  AgsNotebook *notebook;
  
  AgsAutomation *automation;

  xmlDoc *clipboard;
  xmlNode *audio_node;
  xmlNode *automation_list_node, *automation_node;
  xmlNode *timestamp_node;
  
  gchar *buffer;

  guint position_x, position_y;
  gint first_x, last_x;
  gboolean paste_from_position;

  if(!AGS_IS_AUTOMATION_EDITOR(automation_editor) ||
     automation_editor->focused_automation_edit == NULL){
    return;
  }
  
  if((machine = automation_editor->selected_machine) != NULL){
    machine = automation_editor->selected_machine;

    automation_edit = NULL;
    notebook = NULL;

    position_x = 0;
    position_y = 0;
    
    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      GList *list_start, *list;
      
      notebook = automation_editor->output_notebook;

      list =
	list_start = gtk_container_get_children(GTK_CONTAINER(automation_editor->output_scrolled_automation_edit_box->automation_edit_box));

      while(list != NULL){
	if(!g_strcmp0(automation_editor->focused_automation_edit->control_name,
		      AGS_AUTOMATION_EDIT(list->data)->control_name)){
	  automation_edit = list->data;
	  
	  break;
	}
	
	list = list->next;
      }

      g_list_free(list_start);
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      GList *list_start, *list;

      notebook = automation_editor->input_notebook;

      list =
	list_start = gtk_container_get_children(GTK_CONTAINER(automation_editor->input_scrolled_automation_edit_box->automation_edit_box));

      while(list != NULL){
	if(!g_strcmp0(automation_editor->focused_automation_edit->control_name,
		      AGS_AUTOMATION_EDIT(list->data)->control_name)){
	  automation_edit = list->data;
	  
	  break;
	}
	
	list = list->next;
      }

      g_list_free(list_start);
    }else{
      GList *list_start, *list;

      list =
	list_start = gtk_container_get_children(GTK_CONTAINER(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box));

      while(list != NULL){
	if(!g_strcmp0(automation_editor->focused_automation_edit->control_name,
		      AGS_AUTOMATION_EDIT(list->data)->control_name)){
	  automation_edit = list->data;
	  
	  break;
	}
	
	list = list->next;
      }

      g_list_free(list_start);
    }

    /* get clipboard */
    buffer = gtk_clipboard_wait_for_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    
    if(buffer == NULL){
      return;
    }

    /* get position */
    if(automation_editor->automation_toolbar->selected_edit_mode == automation_editor->automation_toolbar->position){
      last_x = 0;
      paste_from_position = TRUE;

      position_x = automation_editor->focused_automation_edit->cursor_position_x;
      position_y = automation_editor->focused_automation_edit->cursor_position_y;
      
#ifdef DEBUG
      printf("pasting at position: [%u,%u]\n", position_x, position_y);
#endif
    }else{
      paste_from_position = FALSE;
    }

    /* get xml tree */
    clipboard = xmlReadMemory(buffer, strlen(buffer),
			      NULL, "UTF-8",
			      0);
    audio_node = xmlDocGetRootElement(clipboard);

    first_x = -1;
    
    /* iterate xml tree */
    while(audio_node != NULL){
      if(audio_node->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp("audio", audio_node->name, 6)){
	  automation_node = audio_node->children;
	
	  first_x = ags_automation_editor_paste_automation(automation_editor,
							   notebook,
							   machine,
							   audio_node,
							   position_x, position_y,
							   paste_from_position,
							   &last_x);
	
	  break;
	}
      }
      
      audio_node = audio_node->next;
    }

    if(first_x == -1){
      first_x = 0;
    }
    
    xmlFreeDoc(clipboard); 

    if(paste_from_position){
      gint big_step, small_step;

      //TODO:JK: implement me
      big_step = (guint) ceil((double) last_x / 16.0) * 16.0 + (automation_editor->focused_automation_edit->cursor_position_x % (guint) 16);
      small_step = (guint) big_step - 16;
	
      if(small_step < last_x){
	automation_editor->focused_automation_edit->cursor_position_x = big_step;
      }else{
	automation_editor->focused_automation_edit->cursor_position_x = small_step;
      }
    }

    gtk_widget_queue_draw(GTK_WIDGET(automation_edit));
  }
}

void
ags_automation_editor_copy(AgsAutomationEditor *automation_editor)
{
  AgsMachine *machine;
  AgsNotebook *notebook;
  
  AgsAutomation *automation;

  xmlDoc *clipboard;
  xmlNode *audio_node, *automation_list_node, *automation_node;

  GList *start_list_automation, *list_automation;

  xmlChar *buffer;

  int size;
  gint i;

  if(!AGS_IS_AUTOMATION_EDITOR(automation_editor) ||
     automation_editor->focused_automation_edit == NULL){
    return;
  }
  
  if(automation_editor->selected_machine != NULL){
    machine = automation_editor->selected_machine;

    notebook = NULL;

    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }
  
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL,
			    BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);

    automation_list_node = xmlNewNode(NULL,
				    BAD_CAST "automation-list");
    xmlAddChild(audio_node,
		automation_list_node);

    /* create automation nodes */
    g_object_get(machine->audio,
		 "automation", &start_list_automation,
		 NULL);
    
    i = 0;

    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      list_automation = start_list_automation;

      /* copy */
      while((list_automation = ags_automation_find_near_timestamp_extended(list_automation, i,
									   automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name,
									   NULL)) != NULL){
	automation_node = ags_automation_copy_selection(AGS_AUTOMATION(list_automation->data));
	xmlAddChild(automation_list_node,
		    automation_node);
	
	list_automation = list_automation->next;
      }

      if(notebook == NULL){
	break;
      }
      
      i++;
    }
    
    g_list_free_full(start_list_automation,
		     g_object_unref);
          
    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
  }
}

void
ags_automation_editor_cut(AgsAutomationEditor *automation_editor)
{
  AgsMachine *machine;
  AgsNotebook *notebook;
  
  AgsAutomation *automation;

  xmlDoc *clipboard;
  xmlNode *audio_node;
  xmlNode *automation_list_node, *automation_node;

  GList *start_list_automation, *list_automation;

  xmlChar *buffer;
  int size;
  gint i;

  if(!AGS_IS_AUTOMATION_EDITOR(automation_editor) ||
     automation_editor->focused_automation_edit == NULL){
    return;
  }
  
  if(automation_editor->selected_machine != NULL){
    machine = automation_editor->selected_machine;

    notebook = NULL;

    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }

    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL,
			    BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);

    automation_list_node = xmlNewNode(NULL,
				      BAD_CAST "automation-list");
    xmlAddChild(audio_node,
		automation_list_node);

    /* create automation nodes */
    g_object_get(machine->audio,
		 "automation", &start_list_automation,
		 NULL);
    
    i = 0;
    
    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      list_automation = start_list_automation;

      /* cut */
      while((list_automation = ags_automation_find_near_timestamp_extended(list_automation, i,
									   automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name,
									   NULL)) != NULL){
	automation_node = ags_automation_cut_selection(AGS_AUTOMATION(list_automation->data));
	xmlAddChild(automation_list_node,
		    automation_node);
	
	list_automation = list_automation->next;
      }
      
      if(notebook == NULL){
	break;
      }

      i++;
    }
    
    g_list_free_full(start_list_automation,
		     g_object_unref);
      
    gtk_widget_queue_draw(GTK_WIDGET(automation_editor->focused_automation_edit));

    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
  }
}

void
ags_automation_editor_invert(AgsAutomationEditor *automation_editor)
{
  g_message("ags_automation_editor_invert() - not implemented");
}

/**
 * ags_automation_editor_new:
 *
 * Create a new instance of #AgsAutomationEditor.
 *
 * Returns: the new #AgsAutomationEditor
 *
 * Since: 3.0.0
 */
AgsAutomationEditor*
ags_automation_editor_new()
{
  AgsAutomationEditor *automation_editor;

  automation_editor = (AgsAutomationEditor *) g_object_new(AGS_TYPE_AUTOMATION_EDITOR,
							   NULL);

  return(automation_editor);
}
