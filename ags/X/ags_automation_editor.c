/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

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

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SOUNDCARD,
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
  static GType ags_type_automation_editor = 0;

  if(!ags_type_automation_editor){
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
  }

  return(ags_type_automation_editor);
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
  /**
   * AgsAutomationEditor:soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("assigned soundcard"),
				   i18n_pspec("The soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

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
   * Since: 1.0.0
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
  GtkScrolledWindow *scrolled_window;
  GtkTable *table;

  GtkAdjustment *adjustment;
  
  automation_editor->flags = 0;

  automation_editor->version = AGS_AUTOMATION_EDITOR_DEFAULT_VERSION;
  automation_editor->build_id = AGS_AUTOMATION_EDITOR_DEFAULT_BUILD_ID;

  /* offset */
  automation_editor->tact_counter = 0;
  automation_editor->current_tact = 0.0;

  /* soundcard */
  automation_editor->soundcard = NULL;

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
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_paned_pack1((GtkPaned *) automation_editor->paned,
		  (GtkWidget *) scrolled_window,
		  FALSE, TRUE);

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
  automation_editor->notebook = (GtkNotebook *) gtk_notebook_new();
  gtk_paned_pack2((GtkPaned *) automation_editor->paned,
		  (GtkWidget *) automation_editor->notebook,
		  TRUE, FALSE);
  
  /* audio */
  table = (GtkTable *) gtk_table_new(4, 3,
				     FALSE);
  gtk_notebook_append_page(automation_editor->notebook,
			   (GtkWidget *) table,
			   gtk_label_new(i18n("audio")));

  /* audio - scrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_HEIGHT, 1.0);
  automation_editor->audio_vscrollbar = gtk_vscrollbar_new(adjustment);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->audio_vscrollbar,
		   2, 3,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH, 1.0);
  automation_editor->audio_hscrollbar = gtk_hscrollbar_new(adjustment);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->audio_hscrollbar,
		   1, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  /* audio - ruler */
  automation_editor->audio_ruler = ags_ruler_new();
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->audio_ruler,
		   1, 2,
		   1, 2,
		   GTK_FILL | GTK_EXPAND, GTK_FILL,
		   0, 0);

  /* audio - scale */
  automation_editor->audio_scrolled_scale_box = ags_scrolled_scale_box_new();

  automation_editor->audio_scrolled_scale_box->scale_box = ags_vscale_box_new();
  gtk_container_add(automation_editor->audio_scrolled_scale_box->viewport,
		    automation_editor->audio_scrolled_scale_box->scale_box);

  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->audio_scrolled_scale_box,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* audio - automation edit */
  automation_editor->audio_scrolled_automation_edit_box = ags_scrolled_automation_edit_box_new();

  automation_editor->audio_scrolled_automation_edit_box->automation_edit_box = ags_vautomation_edit_box_new();
  gtk_container_add(automation_editor->audio_scrolled_automation_edit_box->viewport,
		    automation_editor->audio_scrolled_automation_edit_box->automation_edit_box);

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
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_HEIGHT, 1.0);
  automation_editor->output_vscrollbar = gtk_vscrollbar_new(adjustment);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->output_vscrollbar,
		   2, 3,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH, 1.0);
  automation_editor->output_hscrollbar = gtk_hscrollbar_new(adjustment);
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
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->output_ruler,
		   1, 2,
		   1, 2,
		   GTK_FILL | GTK_EXPAND, GTK_FILL,
		   0, 0);

  /* output - scale */
  automation_editor->output_scrolled_scale_box = ags_scrolled_scale_box_new();

  automation_editor->output_scrolled_scale_box->scale_box = ags_vscale_box_new();
  gtk_container_add(automation_editor->output_scrolled_scale_box->viewport,
		    automation_editor->output_scrolled_scale_box->scale_box);

  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->output_scrolled_scale_box,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* output - automation edit */
  automation_editor->output_scrolled_automation_edit_box = ags_scrolled_automation_edit_box_new();

  automation_editor->output_scrolled_automation_edit_box->automation_edit_box = ags_vautomation_edit_box_new();
  gtk_container_add(automation_editor->output_scrolled_automation_edit_box->viewport,
		    automation_editor->output_scrolled_automation_edit_box->automation_edit_box);

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
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_HEIGHT, 1.0);
  automation_editor->input_vscrollbar = gtk_vscrollbar_new(adjustment);
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->input_vscrollbar,
		   2, 3,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH, 1.0);
  automation_editor->input_hscrollbar = gtk_hscrollbar_new(adjustment);
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
  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->input_ruler,
		   1, 2,
		   1, 2,
		   GTK_FILL | GTK_EXPAND, GTK_FILL,
		   0, 0);

  /* input - scale */
  automation_editor->input_scrolled_scale_box = ags_scrolled_scale_box_new();

  automation_editor->input_scrolled_scale_box->scale_box = ags_vscale_box_new();
  gtk_container_add(automation_editor->input_scrolled_scale_box->viewport,
		    automation_editor->input_scrolled_scale_box->scale_box);

  gtk_table_attach(table,
		   (GtkWidget *) automation_editor->input_scrolled_scale_box,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_widget_show_all(automation_editor->input_scrolled_scale_box);
  gtk_widget_show_all(automation_editor->input_scrolled_scale_box->viewport);
  
  /* input automation edit */
  automation_editor->input_scrolled_automation_edit_box = ags_scrolled_automation_edit_box_new();

  automation_editor->input_scrolled_automation_edit_box->automation_edit_box = ags_vautomation_edit_box_new();
  gtk_container_add(automation_editor->input_scrolled_automation_edit_box->viewport,
		    automation_editor->input_scrolled_automation_edit_box->automation_edit_box);

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
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = g_value_get_object(value);

      if(automation_editor->soundcard == soundcard){
	return;
      }

      if(automation_editor->soundcard != NULL){
	g_object_unref(automation_editor->soundcard);
      }
      
      if(soundcard != NULL){
	g_object_ref(soundcard);
      }
      
      automation_editor->soundcard = soundcard;
    }
    break;
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
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, automation_editor->soundcard);
    }
    break;
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
  g_signal_connect_after((GObject *) automation_editor->audio_scrolled_automation_edit_box->viewport, "expose_event",
			 G_CALLBACK(ags_automation_editor_audio_edit_expose_event), (gpointer) automation_editor);

  g_signal_connect_after((GObject *) automation_editor->audio_scrolled_automation_edit_box->viewport, "configure_event",
			 G_CALLBACK(ags_automation_editor_audio_edit_configure_event), (gpointer) automation_editor);

  g_signal_connect_after((GObject *) automation_editor->audio_vscrollbar, "value-changed",
			 G_CALLBACK(ags_automation_editor_audio_vscrollbar_value_changed), (gpointer) automation_editor);

  g_signal_connect_after((GObject *) automation_editor->audio_hscrollbar, "value-changed",
			 G_CALLBACK(ags_automation_editor_audio_hscrollbar_value_changed), (gpointer) automation_editor);

  /* output */
  g_signal_connect_after((GObject *) automation_editor->output_scrolled_automation_edit_box->viewport, "expose_event",
			 G_CALLBACK(ags_automation_editor_output_edit_expose_event), (gpointer) automation_editor);

  g_signal_connect_after((GObject *) automation_editor->output_scrolled_automation_edit_box->viewport, "configure_event",
			 G_CALLBACK(ags_automation_editor_output_edit_configure_event), (gpointer) automation_editor);

  g_signal_connect_after((GObject *) automation_editor->output_vscrollbar, "value-changed",
			 G_CALLBACK(ags_automation_editor_output_vscrollbar_value_changed), (gpointer) automation_editor);

  g_signal_connect_after((GObject *) automation_editor->output_hscrollbar, "value-changed",
			 G_CALLBACK(ags_automation_editor_output_hscrollbar_value_changed), (gpointer) automation_editor);

  /* input */
  g_signal_connect_after((GObject *) automation_editor->input_scrolled_automation_edit_box->viewport, "expose_event",
			 G_CALLBACK(ags_automation_editor_input_edit_expose_event), (gpointer) automation_editor);

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
		      "any_signal::expose_event",
		      G_CALLBACK(ags_automation_editor_audio_edit_expose_event),
		      automation_editor,
		      "any_signal::configure_event",
		      G_CALLBACK(ags_automation_editor_audio_edit_configure_event),
		      automation_editor,
		      NULL);

  /* output */
  g_object_disconnect((GObject *) automation_editor->output_scrolled_automation_edit_box->viewport,
		      "any_signal::expose_event",
		      G_CALLBACK(ags_automation_editor_output_edit_expose_event),
		      automation_editor,
		      "any_signal::configure_event",
		      G_CALLBACK(ags_automation_editor_output_edit_configure_event),
		      automation_editor,
		      NULL);

  /* input */
  g_object_disconnect((GObject *) automation_editor->input_scrolled_automation_edit_box->viewport,
		      "any_signal::expose_event",
		      G_CALLBACK(ags_automation_editor_input_edit_expose_event),
		      automation_editor,
		      "any_signal::configure_event",
		      G_CALLBACK(ags_automation_editor_input_edit_configure_event),
		      automation_editor,
		      NULL);
  
  /* toolbar and selector */
  ags_connectable_disconnect(AGS_CONNECTABLE(automation_editor->automation_toolbar)); 
  ags_connectable_disconnect(AGS_CONNECTABLE(automation_editor->machine_selector));
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

  GList *list_start, *list;
  
  gdouble v_upper, h_upper;
  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;

  automation_toolbar = automation_editor->automation_toolbar;

  /* reset vertical scrollbar */
  v_upper = GTK_WIDGET(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box)->allocation.height - GTK_WIDGET(automation_editor->audio_scrolled_automation_edit_box->viewport)->allocation.height;

  if(v_upper < 0.0){
    v_upper = 0.0;
  }
  
  gtk_adjustment_set_upper(GTK_RANGE(automation_editor->audio_vscrollbar)->adjustment,
			   v_upper);

  gtk_adjustment_set_upper(gtk_viewport_get_vadjustment(automation_editor->audio_scrolled_automation_edit_box->viewport),
			   v_upper);
  gtk_adjustment_set_upper(gtk_viewport_get_vadjustment(automation_editor->audio_scrolled_scale_box->viewport),
			   v_upper);
  
  /* reset horizontal scrollbar */
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom) - 2.0);
  zoom_correction = 1.0 / 16;

  map_width = ((double) AGS_AUTOMATION_EDITOR_MAX_CONTROLS * zoom * zoom_correction);
  h_upper = map_width - GTK_WIDGET(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box)->allocation.width;

  if(h_upper < 0.0){
    h_upper = 0.0;
  }
  
  gtk_adjustment_set_upper(GTK_RANGE(automation_editor->audio_hscrollbar)->adjustment,
			   h_upper);

  /* automation edit */
  list_start =
    list = gtk_container_get_children(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box);

  while(list != NULL){
    gtk_adjustment_set_upper(GTK_RANGE(AGS_AUTOMATION_EDIT(list->data)->hscrollbar)->adjustment,
			     h_upper);
    

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_automation_editor_reset_output_scrollbar(AgsAutomationEditor *automation_editor)
{
  AgsAutomationToolbar *automation_toolbar;

  GList *list_start, *list;
  
  gdouble v_upper, h_upper;
  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;

  automation_toolbar = automation_editor->automation_toolbar;

  /* reset vertical scrollbar */
  v_upper = GTK_WIDGET(automation_editor->output_scrolled_automation_edit_box->automation_edit_box)->allocation.height - GTK_WIDGET(automation_editor->output_scrolled_automation_edit_box->viewport)->allocation.height;

  if(v_upper < 0.0){
    v_upper = 0.0;
  }
  
  gtk_adjustment_set_upper(GTK_RANGE(automation_editor->output_vscrollbar)->adjustment,
			   v_upper);

  gtk_adjustment_set_upper(gtk_viewport_get_vadjustment(automation_editor->output_scrolled_automation_edit_box->viewport),
			   v_upper);
  gtk_adjustment_set_upper(gtk_viewport_get_vadjustment(automation_editor->output_scrolled_scale_box->viewport),
			   v_upper);

  /* reset horizontal scrollbar */
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom) - 2.0);
  zoom_correction = 1.0 / 16;

  map_width = ((double) AGS_AUTOMATION_EDITOR_MAX_CONTROLS * zoom * zoom_correction);
  h_upper = map_width - GTK_WIDGET(automation_editor->output_scrolled_automation_edit_box->automation_edit_box)->allocation.width;

  if(h_upper < 0.0){
    h_upper = 0.0;
  }
  
  gtk_adjustment_set_upper(GTK_RANGE(automation_editor->output_hscrollbar)->adjustment,
			   h_upper);

  /* automation edit */
  list_start =
    list = gtk_container_get_children(automation_editor->output_scrolled_automation_edit_box->automation_edit_box);

  while(list != NULL){
    gtk_adjustment_set_upper(GTK_RANGE(AGS_AUTOMATION_EDIT(list->data)->hscrollbar)->adjustment,
			     h_upper);
    

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_automation_editor_reset_input_scrollbar(AgsAutomationEditor *automation_editor)
{
  AgsAutomationToolbar *automation_toolbar;

  GList *list_start, *list;
  
  gdouble v_upper, h_upper;
  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;
  
  automation_toolbar = automation_editor->automation_toolbar;

  /* reset vertical scrollbar */
  v_upper = GTK_WIDGET(automation_editor->input_scrolled_automation_edit_box->automation_edit_box)->allocation.height - GTK_WIDGET(automation_editor->input_scrolled_automation_edit_box->viewport)->allocation.height;

  if(v_upper < 0.0){
    v_upper = 0.0;
  }
  
  gtk_adjustment_set_upper(GTK_RANGE(automation_editor->input_vscrollbar)->adjustment,
			   v_upper);

  gtk_adjustment_set_upper(gtk_viewport_get_vadjustment(automation_editor->input_scrolled_automation_edit_box->viewport),
			   v_upper);
  gtk_adjustment_set_upper(gtk_viewport_get_vadjustment(automation_editor->input_scrolled_scale_box->viewport),
			   v_upper);

  /* reset horizontal scrollbar */
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom) - 2.0);
  zoom_correction = 1.0 / 16;

  map_width = ((double) AGS_AUTOMATION_EDITOR_MAX_CONTROLS * zoom * zoom_correction);
  h_upper = map_width - GTK_WIDGET(automation_editor->input_scrolled_automation_edit_box->automation_edit_box)->allocation.width;

  if(h_upper < 0.0){
    h_upper = 0.0;
  }
  
  gtk_adjustment_set_upper(GTK_RANGE(automation_editor->input_hscrollbar)->adjustment,
			   h_upper);

  /* automation edit */
  list_start =
    list = gtk_container_get_children(automation_editor->input_scrolled_automation_edit_box->automation_edit_box);

  while(list != NULL){
    gtk_adjustment_set_upper(GTK_RANGE(AGS_AUTOMATION_EDIT(list->data)->hscrollbar)->adjustment,
			     h_upper);
    

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_automation_editor_real_machine_changed(AgsAutomationEditor *automation_editor, AgsMachine *machine)
{  
  AgsMachine *old_machine;

  AgsMutexManager *mutex_manager;

  GList *list_start, *list;
  GList *tab;
  
  guint length;
  guint output_pads, input_pads;
  guint audio_channels;
  guint i;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

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

  /* get audio mutex */
  if(machine != NULL){
    pthread_mutex_lock(application_mutex);
  
    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) machine->audio);
  
    pthread_mutex_unlock(application_mutex);
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
    pthread_mutex_lock(audio_mutex);

    output_pads = machine->audio->output_pads;
    input_pads = machine->audio->input_pads;
    
    audio_channels = machine->audio->audio_channels;
    
    pthread_mutex_unlock(audio_mutex);

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
    list_start = gtk_container_get_children(automation_editor->audio_scrolled_scale_box->scale_box);

  while(list != NULL){
    gtk_widget_destroy(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);

  list =
    list_start = gtk_container_get_children(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box);

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
    list_start = gtk_container_get_children(automation_editor->output_scrolled_scale_box->scale_box);

  while(list != NULL){
    gtk_widget_destroy(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);

  list =
    list_start = gtk_container_get_children(automation_editor->output_scrolled_automation_edit_box->automation_edit_box);

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
    list_start = gtk_container_get_children(automation_editor->input_scrolled_scale_box->scale_box);

  while(list != NULL){
    gtk_widget_destroy(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);

  list =
    list_start = gtk_container_get_children(automation_editor->input_scrolled_automation_edit_box->automation_edit_box);

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
    GList *automation_port;
    
    /* audio */
    automation_port = machine->enabled_automation_port;

    while(automation_port != NULL){
      AgsAutomationEdit *automation_edit;
      AgsScale *scale;
      
      GList *automation;
	
      gboolean contains_specifier;

      pthread_mutex_lock(audio_mutex);
      
      contains_specifier = ((automation = ags_automation_find_specifier_with_type_and_line(machine->audio->automation,
											   AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->control_name,
											   AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type,
											   0)) != NULL) ? TRUE: FALSE;
      
      pthread_mutex_unlock(audio_mutex);

      if(contains_specifier){
	/* scale */
	scale = ags_scale_new();

	pthread_mutex_lock(audio_mutex);

	g_object_set(scale,
		     "control-name", AGS_AUTOMATION(automation->data)->control_name,
		     "upper", AGS_AUTOMATION(automation->data)->upper,
		     "lower", AGS_AUTOMATION(automation->data)->lower,
		     "default-value", AGS_AUTOMATION(automation->data)->default_value,
		     NULL);

	pthread_mutex_unlock(audio_mutex);

	if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == G_TYPE_NONE){
	  gtk_box_pack_start(automation_editor->audio_scrolled_scale_box->scale_box,
			     scale,
			     FALSE, FALSE,
			     AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
	}else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_OUTPUT){
	  gtk_box_pack_start(automation_editor->output_scrolled_scale_box->scale_box,
			     scale,
			     FALSE, FALSE,
			     AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
	}else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_INPUT){
	  gtk_box_pack_start(automation_editor->input_scrolled_scale_box->scale_box,
			     scale,
			     FALSE, FALSE,
			     AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
	}
	
	gtk_widget_show(scale);
	  
	/* automation edit */
	automation_edit = ags_automation_edit_new();

	pthread_mutex_lock(audio_mutex);

	g_object_set(automation_edit,
		     "channel-type", G_TYPE_NONE,
		     "control-specifier", AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->control_name,
		     "control-name", AGS_AUTOMATION(automation->data)->control_name,
		     "upper", AGS_AUTOMATION(automation->data)->upper,
		     "lower", AGS_AUTOMATION(automation->data)->lower,
		     "default-value", AGS_AUTOMATION(automation->data)->default_value,
		     NULL);

	pthread_mutex_unlock(audio_mutex);

	if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == G_TYPE_NONE){
	  gtk_box_pack_start(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box,
			     automation_edit,
			     FALSE, FALSE,
			     AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
	}else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_OUTPUT){
	  gtk_box_pack_start(automation_editor->output_scrolled_automation_edit_box->automation_edit_box,
			     automation_edit,
			     FALSE, FALSE,
			     AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
	}else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_INPUT){
	  gtk_box_pack_start(automation_editor->input_scrolled_automation_edit_box->automation_edit_box,
			     automation_edit,
			     FALSE, FALSE,
			     AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
	}

	ags_connectable_connect(AGS_CONNECTABLE(automation_edit));
	gtk_widget_show(automation_edit);

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
      }
      
      automation_port = automation_port->next;
    }
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
 * Since: 1.0.0
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
    AgsMutexManager *mutex_manager;

    pthread_mutex_t *application_mutex;
    pthread_mutex_t *audio_mutex;

    machine = automation_editor->selected_machine;

    notebook = NULL;

    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }
    
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) machine->audio);
  
    pthread_mutex_unlock(application_mutex);

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
      
      pthread_mutex_lock(audio_mutex);
      
      list_automation = machine->audio->automation;
      list_automation = ags_automation_find_near_timestamp_extended(list_automation, i,
								    automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name,
								    timestamp);

      pthread_mutex_unlock(audio_mutex);

      if(list_automation != NULL){
	pthread_mutex_lock(audio_mutex);
	
	automation = list_automation->data;

	pthread_mutex_unlock(audio_mutex);
      }else{
	//NOTE:JK: doesn't support segmentation, yet
	automation = NULL;
      }

      pthread_mutex_lock(audio_mutex);
      
      new_acceleration = ags_acceleration_duplicate(acceleration);
      ags_automation_add_acceleration(automation,
				      new_acceleration,
				      FALSE);

      pthread_mutex_unlock(audio_mutex);

      if(notebook == NULL){
	break;
      }
      
      i++;
    }
    
    g_object_unref(timestamp);
    
    gtk_widget_queue_draw(automation_editor->focused_automation_edit);
  }
}

void
ags_automation_editor_delete_acceleration(AgsAutomationEditor *automation_editor,
					  guint x, gdouble y)
{
  AgsMachine *machine;
  AgsNotebook *notebook;

  AgsAutomation *automation;

  AgsTimestamp *timestamp;

  GList *list_automation;

  gint i;

  if(!AGS_IS_AUTOMATION_EDITOR(automation_editor) ||
     automation_editor->focused_automation_edit == NULL){
    return;
  }

  if(automation_editor->selected_machine != NULL){
    AgsMutexManager *mutex_manager;

    pthread_mutex_t *application_mutex;
    pthread_mutex_t *audio_mutex;

    machine = automation_editor->selected_machine;

    notebook = NULL;

    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }
  
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) machine->audio);
  
    pthread_mutex_unlock(application_mutex);

    /* check all active tabs */
    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
    timestamp->timer.ags_offset.offset = AGS_AUTOMATION_DEFAULT_OFFSET * floor(x / AGS_AUTOMATION_DEFAULT_OFFSET);

    pthread_mutex_lock(audio_mutex);

    i = 0;

    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(automation_editor->notebook,
					    i)) != -1){
      AgsAcceleration *new_acceleration;
      
      list_automation = machine->audio->automation;
      list_automation = ags_automation_find_near_timestamp_extended(list_automation, i,
								    automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name,
								    timestamp);

      if(list_automation != NULL){
	automation = list_automation->data;
      }else{
	i++;
	
	continue;
      }

      ags_automation_remove_acceleration_at_position(automation,
					   x, y);

      if(notebook == NULL){
	break;
      }

      i++;
    }
    
    pthread_mutex_unlock(audio_mutex);

    gtk_widget_queue_draw(automation_editor->focused_automation_edit);
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

  GList *list_automation;

  gint i;

  if(!AGS_IS_AUTOMATION_EDITOR(automation_editor) ||
     automation_editor->focused_automation_edit == NULL){
    return;
  }
  
  if(automation_editor->selected_machine != NULL){
    AgsMutexManager *mutex_manager;

    pthread_mutex_t *application_mutex;
    pthread_mutex_t *audio_mutex;

    machine = automation_editor->selected_machine;
  
    notebook = NULL;

    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }

    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) machine->audio);
  
    pthread_mutex_unlock(application_mutex);

    /* swap values if needed */
    if(x0 > x1){
      guint tmp;

      tmp = x0;
      
      x0 = x1;
      x1 = tmp;
    }

    if(y0 > y1){
      guint tmp;

      tmp = y0;
      
      y0 = y1;
      y1 = tmp;
    }

    /* check all active tabs */
    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
    pthread_mutex_lock(audio_mutex);

    i = 0;

    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){      
      list_automation = machine->audio->automation;
      
      timestamp->timer.ags_offset.offset = AGS_AUTOMATION_DEFAULT_OFFSET * floor(x0 / AGS_AUTOMATION_DEFAULT_OFFSET);
      
      while((list_automation = ags_automation_find_near_timestamp_extended(list_automation, i,
									   automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name,
									   timestamp)) != NULL &&
	    timestamp->timer.ags_offset.offset < (AGS_AUTOMATION_DEFAULT_OFFSET * floor(x1 / AGS_AUTOMATION_DEFAULT_OFFSET)) + AGS_AUTOMATION_DEFAULT_OFFSET){
	ags_automation_add_region_to_selection(list_automation->data,
					       x0, y0,
					       x1, y1,
					       TRUE);
	
	/* iterate */
	timestamp->timer.ags_offset.offset += AGS_AUTOMATION_DEFAULT_OFFSET;
	
	list_automation = list_automation->next;
      }

      if(notebook == NULL){
	break;
      }
      
      i++;
    }
    
    pthread_mutex_unlock(audio_mutex);

    gtk_widget_queue_draw(automation_editor->focused_automation_edit);
  }
}

void
ags_automation_editor_select_all(AgsAutomationEditor *automation_editor)
{
  AgsMachine *machine;
  AgsNotebook *notebook;
  
  GList *list_automation;

  gint i;

  if(!AGS_IS_AUTOMATION_EDITOR(automation_editor)){
    return;
  }
  
  if(automation_editor->selected_machine != NULL){
    AgsMutexManager *mutex_manager;

    pthread_mutex_t *application_mutex;
    pthread_mutex_t *audio_mutex;

    machine = automation_editor->selected_machine;
  
    notebook = NULL;

    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }

    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) machine->audio);
  
    pthread_mutex_unlock(application_mutex);

    /* check all active tabs */
    pthread_mutex_lock(audio_mutex);

    i = 0;
    
    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      list_automation = machine->audio->automation;
      
      while((list_automation = ags_automation_find_near_timestamp_extended(list_automation, i,
									   automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name,
									   NULL)) != NULL){
	ags_automation_add_all_to_selection(AGS_AUTOMATION(list_automation->data));
	
	list_automation = list_automation->next;
      }

      if(notebook == NULL){
	break;
      }

      i++;
    }
    
    pthread_mutex_unlock(audio_mutex);

    gtk_widget_queue_draw(automation_editor->focused_automation_edit);
  }
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

  auto gint ags_automation_editor_paste_automation(xmlNode *audio_node);
  
  gint ags_automation_editor_paste_automation(xmlNode *audio_node){
    AgsTimestamp *timestamp;
    
    GList *list_automation;

    guint first_x;

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
		AgsAutomation *automation;
		
		guint offset;
		guint current_x;
		gint i;

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
		
		
		/*  */
		i = 0;
		
		while(notebook == NULL ||
		      (i = ags_notebook_next_active_tab(automation_editor->notebook,
							i)) != -1){
		  timestamp->timer.ags_offset.offset = offset;
		  
		  list_automation = ags_automation_find_near_timestamp_extended(machine->audio->automation, i,
										automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name,
										timestamp);

		  if(list_automation == NULL){
		    automation = ags_automation_new(machine->audio,
						    i,
						    automation_editor->focused_automation_edit->channel_type,
						    automation_editor->focused_automation_edit->control_name);
		    AGS_TIMESTAMP(automation->timestamp)->timer.ags_offset.offset = offset;
		    machine->audio->automation = ags_automation_add(machine->audio->automation,
								    automation);
		  }else{
		    automation = AGS_AUTOMATION(list_automation->data);
		  }
		  
		  if(paste_from_position){
		    xmlNode *child;

		    guint x_boundary;
	  
		    ags_automation_insert_from_clipboard(automation,
							 automation_node,
							 TRUE, position_x,
							 TRUE, position_y);

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
	  
		    if(current_x > last_x){
		      last_x = current_x;
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

		    if(current_x > last_x){
		      last_x = current_x;
		    }
		  }

		  if(notebook == NULL){
		    break;
		  }
		  
		  i++;
		}
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

  if(!AGS_IS_AUTOMATION_EDITOR(automation_editor) ||
     automation_editor->focused_automation_edit == NULL){
    return;
  }
  
  if((machine = automation_editor->selected_machine) != NULL){
    AgsMutexManager *mutex_manager;

    pthread_mutex_t *application_mutex;
    pthread_mutex_t *audio_mutex;


    machine = automation_editor->selected_machine;

    automation_edit = NULL;
    notebook = NULL;

    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      GList *list_start, *list;
      
      notebook = automation_editor->output_notebook;

      list =
	list_start = gtk_container_get_children(automation_editor->output_scrolled_automation_edit_box->automation_edit_box);

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
	list_start = gtk_container_get_children(automation_editor->input_scrolled_automation_edit_box->automation_edit_box);

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
	list_start = gtk_container_get_children(automation_editor->audio_scrolled_automation_edit_box->automation_edit_box);

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
    
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
    /* get audio mutex */
    pthread_mutex_lock(application_mutex);  
    
    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) machine->audio);
  
    pthread_mutex_unlock(application_mutex);

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
    pthread_mutex_lock(audio_mutex);

    while(audio_node != NULL){
      if(audio_node->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp("audio", audio_node->name, 6)){
	  automation_node = audio_node->children;
	
	  first_x = ags_automation_editor_paste_automation(audio_node);
	
	  break;
	}
      }
      
      audio_node = audio_node->next;
    }

    if(first_x == -1){
      first_x = 0;
    }
    
    xmlFreeDoc(clipboard); 

    pthread_mutex_unlock(audio_mutex);

    if(paste_from_position){
      gint big_step, small_step;

      //TODO:JK: implement me
      big_step = (guint) ceil((double) last_x / 16.0) * 16.0 + (automation_edit->cursor_position_x % (guint) 16);
      small_step = (guint) big_step - 16;
	
      if(small_step < last_x){
	automation_editor->focused_automation_edit->cursor_position_x = big_step;
      }else{
	automation_editor->focused_automation_edit->cursor_position_x = small_step;
      }
    }

    gtk_widget_queue_draw(automation_edit);
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

  GList *list_automation;

  xmlChar *buffer;

  int size;
  gint i;

  if(!AGS_IS_AUTOMATION_EDITOR(automation_editor) ||
     automation_editor->focused_automation_edit == NULL){
    return;
  }
  
  if(automation_editor->selected_machine != NULL){
    AgsMutexManager *mutex_manager;

    pthread_mutex_t *application_mutex;
    pthread_mutex_t *audio_mutex;

    machine = automation_editor->selected_machine;

    notebook = NULL;

    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }
  
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) machine->audio);
  
    pthread_mutex_unlock(application_mutex);

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
    list_automation = machine->audio->automation;
    i = 0;

    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      pthread_mutex_lock(audio_mutex);

      list_automation = machine->audio->automation;

      /* copy */
      while((list_automation = ags_automation_find_near_timestamp_extended(list_automation, i,
									   automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name,
									   NULL)) != NULL){
	automation_node = ags_automation_copy_selection(AGS_AUTOMATION(list_automation->data));
	xmlAddChild(automation_list_node,
		    automation_node);
	
	list_automation = list_automation->next;
      }

      pthread_mutex_unlock(audio_mutex);

      if(notebook == NULL){
	break;
      }
      
      i++;
    }
    
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

  GList *list_automation;

  xmlChar *buffer;
  int size;
  gint i;

  if(!AGS_IS_AUTOMATION_EDITOR(automation_editor) ||
     automation_editor->focused_automation_edit == NULL){
    return;
  }
  
  if(automation_editor->selected_machine != NULL){
    AgsMutexManager *mutex_manager;

    pthread_mutex_t *application_mutex;
    pthread_mutex_t *audio_mutex;

    machine = automation_editor->selected_machine;

    notebook = NULL;

    if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(automation_editor->focused_automation_edit->channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }

    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) machine->audio);
  
    pthread_mutex_unlock(application_mutex);

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
    pthread_mutex_lock(audio_mutex);

    list_automation = machine->audio->automation;

    pthread_mutex_unlock(audio_mutex);

    i = 0;
    
    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      pthread_mutex_lock(audio_mutex);
      
      list_automation = machine->audio->automation;

      /* cut */
      while((list_automation = ags_automation_find_near_timestamp_extended(list_automation, i,
									   automation_editor->focused_automation_edit->channel_type, automation_editor->focused_automation_edit->control_name,
									   NULL)) != NULL){
	automation_node = ags_automation_cut_selection(AGS_AUTOMATION(list_automation->data));
	xmlAddChild(automation_list_node,
		    automation_node);
	
	list_automation = list_automation->next;
      }
      
      pthread_mutex_unlock(audio_mutex);

      if(notebook == NULL){
	break;
      }

      i++;
    }

    gtk_widget_queue_draw(automation_editor->focused_automation_edit);

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
 * Create a new #AgsAutomationEditor.
 *
 * Returns: a new #AgsAutomationEditor
 *
 * Since: 1.0.0
 */
AgsAutomationEditor*
ags_automation_editor_new()
{
  AgsAutomationEditor *automation_editor;

  automation_editor = (AgsAutomationEditor *) g_object_new(AGS_TYPE_AUTOMATION_EDITOR,
							   NULL);

  return(automation_editor);
}
