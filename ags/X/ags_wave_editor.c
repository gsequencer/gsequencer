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

#include <ags/X/ags_wave_editor.h>
#include <ags/X/ags_wave_editor_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

#include <ags/X/editor/ags_scrolled_wave_edit_box.h>
#include <ags/X/editor/ags_vwave_edit_box.h>
#include <ags/X/editor/ags_wave_edit.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <math.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_wave_editor_class_init(AgsWaveEditorClass *wave_editor);
void ags_wave_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_wave_editor_init(AgsWaveEditor *wave_editor);
void ags_wave_editor_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_wave_editor_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_wave_editor_finalize(GObject *gobject);

void ags_wave_editor_connect(AgsConnectable *connectable);
void ags_wave_editor_disconnect(AgsConnectable *connectable);

void ags_wave_editor_show(GtkWidget *widget);
void ags_wave_editor_show_all(GtkWidget *widget);

gint ags_wave_editor_paste_wave_all(AgsWaveEditor *wave_editor,
				    AgsMachine *machine,
				    AgsNotebook *notebook,
				    xmlNode *wave_node,
				    AgsTimestamp *timestamp,
				    gboolean match_line,
				    gboolean paste_from_position,
				    guint64 position_x,
				    gint64 *last_x);
gint ags_wave_editor_paste_wave(AgsWaveEditor *wave_editor,
				AgsMachine *machine,
				AgsNotebook *notebook,
				xmlNode *audio_node,
				gboolean paste_from_position,
				guint64 position_x,
				guint64 relative_offset);

void ags_wave_editor_real_machine_changed(AgsWaveEditor *wave_editor, AgsMachine *machine);

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
};

static gpointer ags_wave_editor_parent_class = NULL;
static guint wave_editor_signals[LAST_SIGNAL];

/**
 * SECTION:ags_wave_editor
 * @short_description: A composite widget to edit wave
 * @title: AgsWaveEditor
 * @section_id:
 * @include: ags/X/ags_wave_editor.h
 *
 * #AgsWaveEditor is a composite widget to edit wave. You may select machines
 * or change editor tool to do wave.
 */

GType
ags_wave_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_wave_editor = 0;

    static const GTypeInfo ags_wave_editor_info = {
      sizeof (AgsWaveEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_wave_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsWaveEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_wave_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_wave_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_wave_editor = g_type_register_static(GTK_TYPE_BOX,
						  "AgsWaveEditor", &ags_wave_editor_info,
						  0);
    
    g_type_add_interface_static(ags_type_wave_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_wave_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_wave_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_wave_editor_connect;
  connectable->disconnect = ags_wave_editor_disconnect;
}

void
ags_wave_editor_class_init(AgsWaveEditorClass *wave_editor)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  
  GParamSpec *param_spec;

  ags_wave_editor_parent_class = g_type_class_peek_parent(wave_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) wave_editor;

  gobject->set_property = ags_wave_editor_set_property;
  gobject->get_property = ags_wave_editor_get_property;

  gobject->finalize = ags_wave_editor_finalize;
  
  /* properties */

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) wave_editor;
  
  widget->show = ags_wave_editor_show;
  widget->show_all = ags_wave_editor_show_all;
  
  /* AgsEditorClass */
  wave_editor->machine_changed = ags_wave_editor_real_machine_changed;

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
  wave_editor_signals[MACHINE_CHANGED] =
    g_signal_new("machine-changed",
                 G_TYPE_FROM_CLASS(wave_editor),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsWaveEditorClass, machine_changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_wave_editor_init(AgsWaveEditor *wave_editor)
{
  GtkViewport *viewport;
  GtkBox *hbox;
  GtkScrolledWindow *scrolled_window;  
  GtkGrid *grid;
  
  GtkStyleContext *style_context;
  
  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(wave_editor),
				 GTK_ORIENTATION_VERTICAL);  

  wave_editor->flags = AGS_WAVE_EDITOR_PASTE_MATCH_LINE;

  wave_editor->version = AGS_WAVE_EDITOR_DEFAULT_VERSION;
  wave_editor->build_id = AGS_WAVE_EDITOR_DEFAULT_BUILD_ID;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* offset */
  wave_editor->tact_counter = 0;
  wave_editor->current_tact = 0.0;

  /* soundcard */
  wave_editor->wave_toolbar = ags_wave_toolbar_new();
  gtk_box_pack_start((GtkBox *) wave_editor,
		     (GtkWidget *) wave_editor->wave_toolbar,
		     FALSE, FALSE, 0);

  wave_editor->paned = (GtkPaned *) gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start((GtkBox *) wave_editor,
		     (GtkWidget *) wave_editor->paned,
		     TRUE, TRUE, 0);

  /* machine selector */
  viewport = (GtkViewport *) gtk_viewport_new(NULL,
					      NULL);
  g_object_set(viewport,
	       "shadow-type", GTK_SHADOW_NONE,
	       NULL);
  gtk_paned_pack1((GtkPaned *) wave_editor->paned,
		  (GtkWidget *) viewport,
		  FALSE, TRUE);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(viewport),
		    GTK_WIDGET(scrolled_window));

  wave_editor->machine_selector = g_object_new(AGS_TYPE_MACHINE_SELECTOR,
					       "homogeneous", FALSE,
					       "spacing", 0,
					       NULL);
  ags_machine_selector_set_edit(wave_editor->machine_selector,
				AGS_MACHINE_SELECTOR_EDIT_WAVE);
  gtk_label_set_label(wave_editor->machine_selector->label,
		      i18n("wave"));
  
  wave_editor->machine_selector->popup = ags_machine_selector_popup_new(wave_editor->machine_selector);
  g_object_set(wave_editor->machine_selector->menu_button,
	       "menu", wave_editor->machine_selector->popup,
	       NULL);
  
  gtk_container_add((GtkContainer *) scrolled_window,
		    (GtkWidget *) wave_editor->machine_selector);

  /* selected machine */
  wave_editor->selected_machine = NULL;

  /* grid */
  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
		     0);
  gtk_paned_pack2((GtkPaned *) wave_editor->paned,
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

  grid = (GtkGrid *) gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(viewport),
		    GTK_WIDGET(grid));
  
  /* notebook */
  wave_editor->notebook = g_object_new(AGS_TYPE_NOTEBOOK,
				       "homogeneous", FALSE,
				       "spacing", 0,
				       "prefix", i18n("line"),
				       NULL);

  gtk_widget_set_valign((GtkWidget *) wave_editor->notebook,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) wave_editor->notebook,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) wave_editor->notebook,
			 TRUE);
  
  gtk_grid_attach(grid,
		   (GtkWidget *) wave_editor->notebook,
		   0, 0,
		   3, 1);

  /* ruler */
  wave_editor->ruler = ags_ruler_new();
  g_object_set(wave_editor->ruler,
	       "height-request", (gint) (gui_scale_factor * AGS_RULER_DEFAULT_HEIGHT),
	       "font-size",  (guint) (gui_scale_factor * wave_editor->ruler->font_size),
	       "step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_STEP),
	       "large-step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_LARGE_STEP),
	       "small-step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_SMALL_STEP),
	       NULL);

  gtk_widget_set_valign((GtkWidget *) wave_editor->ruler,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) wave_editor->ruler,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) wave_editor->ruler,
			 TRUE);

  gtk_grid_attach(grid,
		  (GtkWidget *) wave_editor->ruler,
		  1, 1,
		  1, 1);

  /* level */
  wave_editor->scrolled_level_box = ags_scrolled_level_box_new();

  gtk_widget_set_vexpand((GtkWidget *) wave_editor->scrolled_level_box->viewport,
			 TRUE);
  
  g_object_set(wave_editor->scrolled_level_box,
	       "margin-top", (gint) (gui_scale_factor * AGS_RULER_DEFAULT_HEIGHT),
	       NULL);

  wave_editor->scrolled_level_box->level_box = (AgsLevelBox *) ags_vlevel_box_new();
  g_object_set(wave_editor->scrolled_level_box->level_box,
	       "fixed-level-width", (guint) (gui_scale_factor * AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_WIDTH),
	       "fixed-level-height", (guint) (gui_scale_factor * AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_HEIGHT),
	       NULL);
  
  gtk_container_add(GTK_CONTAINER(wave_editor->scrolled_level_box->viewport),
		    GTK_WIDGET(wave_editor->scrolled_level_box->level_box));
  gtk_widget_set_size_request((GtkWidget *) wave_editor->scrolled_level_box,
			      (gint) (gui_scale_factor * AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_WIDTH),
			      -1);

  gtk_widget_set_valign((GtkWidget *) wave_editor->scrolled_level_box,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) wave_editor->scrolled_level_box,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) wave_editor->scrolled_level_box,
			 TRUE);

  gtk_grid_attach(grid,
		  (GtkWidget *) wave_editor->scrolled_level_box,
		  0, 2,
		  1, 1);

  /* wave edit */
  wave_editor->scrolled_wave_edit_box = ags_scrolled_wave_edit_box_new();

  wave_editor->scrolled_wave_edit_box->wave_edit_box = (AgsWaveEditBox *) ags_vwave_edit_box_new();
  g_object_set(wave_editor->scrolled_wave_edit_box->wave_edit_box,
	       "fixed-edit-height", (guint) (gui_scale_factor * AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_HEIGHT),
	       NULL);
  gtk_container_add(GTK_CONTAINER(wave_editor->scrolled_wave_edit_box->viewport),
		    GTK_WIDGET(wave_editor->scrolled_wave_edit_box->wave_edit_box));

  gtk_widget_set_valign((GtkWidget *) wave_editor->scrolled_wave_edit_box,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) wave_editor->scrolled_wave_edit_box,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) wave_editor->scrolled_wave_edit_box,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) wave_editor->scrolled_wave_edit_box,
			 TRUE);

  gtk_grid_attach(grid,
		  (GtkWidget *) wave_editor->scrolled_wave_edit_box,
		  1, 2,
		  1, 1);

  gtk_widget_set_events(GTK_WIDGET(wave_editor->scrolled_wave_edit_box->viewport), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK);
  
  /* scrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (guint) (gui_scale_factor * AGS_WAVE_EDIT_DEFAULT_CONTROL_HEIGHT), 1.0);

  wave_editor->vscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,
							       adjustment);

  gtk_widget_set_valign((GtkWidget *) wave_editor->vscrollbar,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) wave_editor->vscrollbar,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		   (GtkWidget *) wave_editor->vscrollbar,
		   2, 2,
		   1, 1);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (guint) (gui_scale_factor * AGS_WAVE_EDIT_DEFAULT_CONTROL_WIDTH), 1.0);
  
  wave_editor->hscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL,
							       adjustment);

  gtk_widget_set_valign((GtkWidget *) wave_editor->hscrollbar,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) wave_editor->hscrollbar,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		   (GtkWidget *) wave_editor->hscrollbar,
		   1, 3,
		   1, 1);

  /* focused wave edit */
  wave_editor->focused_wave_edit = NULL;

  /* wave meta */
  wave_editor->wave_meta = ags_wave_meta_new();

  gtk_widget_set_valign(wave_editor->wave_meta,
			GTK_ALIGN_START);  

  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) wave_editor->wave_meta,
		     FALSE, FALSE,
		     0);

  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) wave_editor);
  gtk_style_context_add_class(style_context,
			      "editor");
}

void
ags_wave_editor_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsWaveEditor *wave_editor;

  wave_editor = AGS_WAVE_EDITOR(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_editor_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsWaveEditor *wave_editor;

  wave_editor = AGS_WAVE_EDITOR(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_editor_connect(AgsConnectable *connectable)
{
  AgsWaveEditor *wave_editor;

  wave_editor = AGS_WAVE_EDITOR(connectable);

  if((AGS_WAVE_EDITOR_CONNECTED & (wave_editor->flags)) != 0){
    return;
  }

  wave_editor->flags |= AGS_WAVE_EDITOR_CONNECTED;

  /* edit */
  g_signal_connect_after((GObject *) wave_editor->scrolled_wave_edit_box->viewport, "configure_event",
			 G_CALLBACK(ags_wave_editor_edit_configure_event), (gpointer) wave_editor);

  g_signal_connect_after((GObject *) wave_editor->vscrollbar, "value-changed",
			 G_CALLBACK(ags_wave_editor_vscrollbar_value_changed), (gpointer) wave_editor);

  g_signal_connect_after((GObject *) wave_editor->hscrollbar, "value-changed",
			 G_CALLBACK(ags_wave_editor_hscrollbar_value_changed), (gpointer) wave_editor);
  
  /*  */
  g_signal_connect((GObject *) wave_editor->machine_selector, "changed",
		   G_CALLBACK(ags_wave_editor_machine_changed_callback), (gpointer) wave_editor);

  /*  */
  ags_connectable_connect(AGS_CONNECTABLE(wave_editor->wave_toolbar));
  ags_connectable_connect(AGS_CONNECTABLE(wave_editor->machine_selector));

  ags_connectable_connect(AGS_CONNECTABLE(wave_editor->wave_meta));
}

void
ags_wave_editor_disconnect(AgsConnectable *connectable)
{
  AgsWaveEditor *wave_editor;

  wave_editor = AGS_WAVE_EDITOR(connectable);

  if((AGS_WAVE_EDITOR_CONNECTED & (wave_editor->flags)) == 0){
    return;
  }

  wave_editor->flags &= (~AGS_WAVE_EDITOR_CONNECTED);

  /* edit */
  g_object_disconnect((GObject *) wave_editor->scrolled_wave_edit_box->viewport,
		      "any_signal::configure_event",
		      G_CALLBACK(ags_wave_editor_edit_configure_event),
		      wave_editor,
		      NULL);

  /*  */
  g_object_disconnect((GObject *) wave_editor->machine_selector,
		      "changed",
		      G_CALLBACK(ags_wave_editor_machine_changed_callback),
		      (gpointer) wave_editor,
		      NULL);

  ags_connectable_disconnect(AGS_CONNECTABLE(wave_editor->wave_toolbar)); 
  ags_connectable_disconnect(AGS_CONNECTABLE(wave_editor->machine_selector));

  ags_connectable_disconnect(AGS_CONNECTABLE(wave_editor->wave_meta));
}

void
ags_wave_editor_finalize(GObject *gobject)
{
  AgsWaveEditor *wave_editor;

  wave_editor = AGS_WAVE_EDITOR(gobject);
  
  G_OBJECT_CLASS(ags_wave_editor_parent_class)->finalize(gobject);
}

void
ags_wave_editor_reset_scrollbar(AgsWaveEditor *wave_editor)
{
  AgsWaveToolbar *wave_toolbar;

  GtkAdjustment *vscrollbar_adjustment, *hscrollbar_adjustment;

  AgsApplicationContext *application_context;

  GtkAllocation wave_edit_box_allocation;
  GtkAllocation viewport_allocation;
  
  GList *list_start, *list;
  
  gdouble gui_scale_factor;
  gdouble old_h_upper;
  gdouble v_upper, h_upper;
  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;
  
  wave_toolbar = wave_editor->wave_toolbar;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* reset vertical scrollbar */
  gtk_widget_get_allocation(GTK_WIDGET(wave_editor->scrolled_wave_edit_box->wave_edit_box),
			    &wave_edit_box_allocation);

  gtk_widget_get_allocation(GTK_WIDGET(wave_editor->scrolled_wave_edit_box->viewport),
			    &viewport_allocation);
  
  v_upper = wave_edit_box_allocation.height - viewport_allocation.height;

  if(v_upper < 0.0){
    v_upper = 0.0;
  }

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_editor->vscrollbar));
  
  gtk_adjustment_set_upper(vscrollbar_adjustment,
			   v_upper);

  gtk_adjustment_set_upper(wave_editor->scrolled_wave_edit_box->viewport->vadjustment,
			   v_upper);
  gtk_adjustment_set_upper(wave_editor->scrolled_level_box->viewport->vadjustment,
			   v_upper);

  /* reset horizontal scrollbar */
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom) - 2.0);

  /* upper */
  hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_editor->hscrollbar));
  
  old_h_upper = gtk_adjustment_get_upper(hscrollbar_adjustment);

  zoom_correction = 1.0 / 16;

//  map_width = (gui_scale_factor * (double) AGS_WAVE_EDITOR_MAX_CONTROLS * zoom * zoom_correction);
  map_width = (gui_scale_factor * 64.0) * (16.0 * 16.0 * 1200.0) * zoom * zoom_correction;
  h_upper = map_width - wave_edit_box_allocation.width;

  if(h_upper < 0.0){
    h_upper = 0.0;
  }
  
  gtk_adjustment_set_upper(wave_editor->ruler->adjustment,
			   h_upper);

  gtk_adjustment_set_upper(hscrollbar_adjustment,
			   h_upper);

  /* wave edit */
  list_start =
    list = gtk_container_get_children(GTK_CONTAINER(wave_editor->scrolled_wave_edit_box->wave_edit_box));

  while(list != NULL){
    gtk_adjustment_set_upper(hscrollbar_adjustment,
			     h_upper);
    

    list = list->next;
  }

  g_list_free(list_start);

  /* reset value */
  if(old_h_upper != 0.0){
#if 0
    gtk_adjustment_set_value(hscrollbar_adjustment,
			     gtk_adjustment_get_value(hscrollbar_adjustment) / old_h_upper * h_upper);
#endif
  }
}

void
ags_wave_editor_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_wave_editor_parent_class)->show(widget);

  gtk_widget_hide((GtkWidget *) AGS_WAVE_EDITOR(widget)->wave_meta);
}

void
ags_wave_editor_show_all(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_wave_editor_parent_class)->show_all(widget);

  gtk_widget_hide(AGS_WAVE_EDITOR(widget)->wave_meta);
}

void
ags_wave_editor_real_machine_changed(AgsWaveEditor *wave_editor, AgsMachine *machine)
{
  AgsMachine *old_machine;
  AgsWaveEdit *wave_edit;
  AgsLevel *level;

  AgsApplicationContext *application_context;
  
  GList *list_start, *list;
  GList *tab;

  gdouble gui_scale_factor;
  guint length;
  guint output_lines, input_lines;
  guint pads;
  guint i;
  
  if(wave_editor->selected_machine == machine){
    return;
  }

  old_machine = wave_editor->selected_machine;

  if(old_machine != NULL){
    g_object_disconnect(old_machine,
			"any_signal::resize-audio-channels",
			G_CALLBACK(ags_wave_editor_resize_audio_channels_callback),
			(gpointer) wave_editor,
			"any_signal::resize-pads",
			G_CALLBACK(ags_wave_editor_resize_pads_callback),
			(gpointer) wave_editor,
			NULL);
  }

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* notebook - remove tabs */
  length = g_list_length(wave_editor->notebook->tab);
  
  for(i = 0; i < length; i++){
    ags_notebook_remove_tab(wave_editor->notebook,
			    0);
  }

  /*  */
  if(machine != NULL){
    g_object_get(machine->audio,
		 "input-lines", &input_lines,
		 NULL);
    
    for(i = 0; i < input_lines; i++){
      ags_notebook_insert_tab(wave_editor->notebook,
			      i);

      tab = wave_editor->notebook->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
    }
  }

  /* destroy edit */
  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(wave_editor->scrolled_level_box->level_box));

  while(list != NULL){
    gtk_widget_destroy(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);

  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(wave_editor->scrolled_wave_edit_box->wave_edit_box));

  while(list != NULL){
    g_object_disconnect(AGS_WAVE_EDIT(list->data)->hscrollbar,
			"any_signal::value-changed",
			G_CALLBACK(ags_wave_editor_wave_edit_hscrollbar_value_changed),
			(gpointer) wave_editor,
			NULL);

    gtk_widget_destroy(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);

  /*
   * add new
   */
  if(machine != NULL){
    guint input_lines;

    g_object_get(machine->audio,
		 "input-lines", &input_lines,
		 NULL);

    for(i = 0; i < input_lines; i++){
      AgsWaveEdit *wave_edit;
      AgsLevel *level;

      /* level */
      level = ags_level_new();
      g_object_set(level,
		   "level-width", (guint) (gui_scale_factor * AGS_LEVEL_DEFAULT_LEVEL_WIDTH),
		   "level-height", (guint) (gui_scale_factor * AGS_LEVEL_DEFAULT_LEVEL_HEIGHT),
		   NULL);
      gtk_box_pack_start(GTK_BOX(wave_editor->scrolled_level_box->level_box),
			 GTK_WIDGET(level),
			 FALSE, TRUE,
			 AGS_WAVE_EDIT_DEFAULT_PADDING);
	
      gtk_widget_show(GTK_WIDGET(level));

      /* wave edit */
      wave_edit = ags_wave_edit_new(i);
      gtk_box_pack_start(GTK_BOX(wave_editor->scrolled_wave_edit_box->wave_edit_box),
			 GTK_WIDGET(wave_edit),
			 FALSE, FALSE,
			 AGS_WAVE_EDIT_DEFAULT_PADDING);

      ags_connectable_connect(AGS_CONNECTABLE(wave_edit));
      gtk_widget_show(GTK_WIDGET(wave_edit));

      g_signal_connect_after((GObject *) wave_edit->hscrollbar, "value-changed",
			     G_CALLBACK(ags_wave_editor_wave_edit_hscrollbar_value_changed), (gpointer) wave_editor);
    }
  }

  /* connect set-pads - new */
  if(machine != NULL){
    g_signal_connect_after(machine, "resize-audio-channels",
			   G_CALLBACK(ags_wave_editor_resize_audio_channels_callback), wave_editor);

    g_signal_connect_after(machine, "resize-pads",
			   G_CALLBACK(ags_wave_editor_resize_pads_callback), wave_editor);
  }  

  /* selected machine */
  wave_editor->selected_machine = machine;
}
 
/**
 * ags_wave_editor_machine_changed:
 * @wave_editor: an #AgsWaveEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine changed of wave editor.
 *
 * Since: 3.0.0
 */
void
ags_wave_editor_machine_changed(AgsWaveEditor *wave_editor, AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_WAVE_EDITOR(wave_editor));

  g_object_ref((GObject *) wave_editor);
  g_signal_emit((GObject *) wave_editor,
		wave_editor_signals[MACHINE_CHANGED], 0,
		machine);
  g_object_unref((GObject *) wave_editor);
}

void
ags_wave_editor_select_region(AgsWaveEditor *wave_editor,
			      guint x0, gdouble y0,
			      guint x1, gdouble y1)
{
  AgsWindow *window;
  AgsWaveWindow *wave_window;
  AgsWaveToolbar *wave_toolbar;
  AgsNotebook *notebook;  
  AgsMachine *machine;
  
  AgsWave *wave;

  AgsTimestamp *timestamp;

  GObject *soundcard;

  GList *start_wave_edit, *wave_edit;
  GList *start_list_wave, *list_wave;

  gdouble bpm;
  guint samplerate;
  guint buffer_size;
  guint64 relative_offset;
  guint64 x0_offset, x1_offset;
  double zoom, zoom_factor;
  gdouble delay_factor;
  gint i;
  
  if(!AGS_IS_WAVE_EDITOR(wave_editor) ||
     wave_editor->focused_wave_edit == NULL){
    return;
  }
  
  if(wave_editor->selected_machine != NULL){
    machine = wave_editor->selected_machine;

    wave_window = (AgsWaveWindow *) gtk_widget_get_ancestor(GTK_WIDGET(wave_editor),
							    AGS_TYPE_WAVE_WINDOW);
    window = (AgsWindow *) wave_window->parent_window;  

    wave_toolbar = wave_editor->wave_toolbar;

    notebook = wave_editor->notebook;

    bpm = gtk_spin_button_get_value(window->navigation->bpm);
  
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
    
    /* zoom */
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom) - 2.0);
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom));
    
    /* check all active tabs */
    g_object_get(machine->audio,
		 "output-soundcard", &soundcard,
		 "wave", &start_list_wave,
		 "samplerate", &samplerate,
		 "buffer-size", &buffer_size,
		 NULL);
    
    delay_factor = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(soundcard));

    relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;
    
    x0_offset = (x0 / 64.0) * delay_factor / (bpm / 60.0) * samplerate;
    x1_offset = (x1 / 64.0) * delay_factor / (bpm / 60.0) * samplerate;

    //TODO:JK: improve me
    x0_offset = buffer_size * floor(x0_offset / buffer_size);
    x1_offset = buffer_size * ceil(x1_offset / buffer_size);
    
    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;

    start_wave_edit = gtk_container_get_children(GTK_CONTAINER(wave_editor->scrolled_wave_edit_box->wave_edit_box));
    i = 0;

    while((i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      wave_edit = g_list_nth(start_wave_edit,
			     i);
      
      list_wave = start_list_wave;
      
      timestamp->timer.ags_offset.offset = relative_offset * floor(x0 / relative_offset);

      while(timestamp->timer.ags_offset.offset < (relative_offset * floor(x1 / relative_offset)) + relative_offset){
	while((list_wave = ags_wave_find_near_timestamp(list_wave, i,
							timestamp)) != NULL){
	  ags_wave_add_region_to_selection(list_wave->data,
					   x0_offset,
					   x1_offset,
					   TRUE);
	  
	  /* iterate */
	  list_wave = list_wave->next;
	}

	timestamp->timer.ags_offset.offset += relative_offset;
      }

      /* queue draw */
      gtk_widget_queue_draw(GTK_WIDGET(wave_edit->data));
      
      i++;
    }

    if(soundcard != NULL){
      g_object_unref(soundcard);
    }

    g_list_free(start_wave_edit);
    g_list_free_full(start_list_wave,
		     g_object_unref);
  }
}

void
ags_wave_editor_select_all(AgsWaveEditor *wave_editor)
{
  AgsMachine *machine;
  AgsNotebook *notebook;
  
  GList *start_list_wave, *list_wave;

  guint samplerate;
  guint64 relative_offset;
  guint line;
  gint i;

  if(!AGS_IS_WAVE_EDITOR(wave_editor)){
    return;
  }
  
  if(wave_editor->selected_machine != NULL){
    machine = wave_editor->selected_machine;
  
    notebook = wave_editor->notebook;

    /* check all active tabs */
    g_object_get(machine->audio,
		 "wave", &start_list_wave,
		 "samplerate", &samplerate,
		 NULL);

    relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

    i = 0;
    
    while(notebook == NULL ||
	  (i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      list_wave = start_list_wave;
      
      while(list_wave != NULL){
	g_object_get(list_wave->data,
		     "line", &line,
		     NULL);

	if(i != line){	
	  list_wave = list_wave->next;

	  continue;
	}

	ags_wave_add_all_to_selection(AGS_WAVE(list_wave->data));
	
	list_wave = list_wave->next;
      }

      i++;
    }
    
    g_list_free_full(start_list_wave,
		     g_object_unref);

    /* queue draw */
    gtk_widget_queue_draw(GTK_WIDGET(wave_editor->focused_wave_edit));
  }
}

gint
ags_wave_editor_paste_wave_all(AgsWaveEditor *wave_editor,
			       AgsMachine *machine,
			       AgsNotebook *notebook,
			       xmlNode *wave_node,
			       AgsTimestamp *timestamp,
			       gboolean match_line,
			       gboolean paste_from_position,
			       guint64 position_x,
			       gint64 *last_x)
{    
  AgsWave *wave;
		
  GList *start_list_wave, *list_wave;
    
  guint64 first_x;
  guint64 current_x;
  gint i;

  first_x = -1;

  /*  */
  i = 0;
		
  while((i = ags_notebook_next_active_tab(notebook,
					  i)) != -1){		  
    g_object_get(machine->audio,
		 "wave", &start_list_wave,
		 NULL);
      
    list_wave = ags_wave_find_near_timestamp(start_list_wave, i,
					     timestamp);

    if(list_wave == NULL){
      wave = ags_wave_new((GObject *) machine->audio,
			  i);
      wave->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;
	
      /* add to audio */
      ags_audio_add_wave(machine->audio,
			 (GObject *) wave);
    }else{
      wave = AGS_WAVE(list_wave->data);
    }

    if(paste_from_position){
      xmlNode *child;

      guint64 x_boundary;
	  
      ags_wave_insert_from_clipboard_extended(wave,
					      wave_node,
					      TRUE, position_x,
					      0.0, 0,
					      match_line, FALSE);

      /* get boundaries */
      child = wave_node->children;
      current_x = 0;
	  
      while(child != NULL){
	if(child->type == XML_ELEMENT_NODE){
	  if(!xmlStrncmp(child->name,
			 BAD_CAST "buffer",
			 5)){
	    guint64 tmp;

	    tmp = g_ascii_strtoull(xmlGetProp(child,
					      BAD_CAST "x"),
				   NULL,
				   10);

	    if(tmp > current_x){
	      current_x = tmp;
	    }
	  }
	}

	child = child->next;
      }

      x_boundary = g_ascii_strtoull(xmlGetProp(wave_node,
					       BAD_CAST "x-boundary"),
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

      ags_wave_insert_from_clipboard_extended(wave,
					      wave_node,
					      FALSE, 0,
					      0.0, 0,
					      match_line, FALSE);

      /* get boundaries */
      child = wave_node->children;
      current_x = 0;
	  
      while(child != NULL){
	if(child->type == XML_ELEMENT_NODE){
	  if(!xmlStrncmp(child->name,
			 BAD_CAST "buffer",
			 5)){
	    guint64 tmp;

	    tmp = g_ascii_strtoull(xmlGetProp(child,
					      BAD_CAST "x"),
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

    g_list_free_full(start_list_wave,
		     g_object_unref);
      
    i++;
  }

  return(first_x);
}
  
gint
ags_wave_editor_paste_wave(AgsWaveEditor *wave_editor,
			   AgsMachine *machine,
			   AgsNotebook *notebook,
			   xmlNode *audio_node,
			   gboolean paste_from_position,
			   guint64 position_x,
			   guint64 relative_offset)
{
  AgsTimestamp *timestamp;

  xmlNode *wave_list_node, *wave_node;
  xmlNode *timestamp_node;

  gint64 first_x, last_x;
  gboolean match_line;

  first_x = -1;

  match_line = ((AGS_WAVE_EDITOR_PASTE_MATCH_LINE & (wave_editor->flags)) != 0) ? TRUE: FALSE;
    
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;
    
  timestamp->timer.ags_offset.offset = 0;
    
  /* paste wave */
  wave_list_node = audio_node->children;

  while(wave_list_node != NULL){
    if(wave_list_node->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(wave_list_node->name,
		     BAD_CAST "wave-list",
		     14)){
	wave_node = wave_list_node->children;
	  
	while(wave_node != NULL){
	  if(wave_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(wave_node->name,
			   BAD_CAST "wave",
			   9)){
	      guint64 offset;
		
	      timestamp_node = wave_node->children;
	      offset = 0;
	  
	      while(timestamp_node != NULL){
		if(timestamp_node->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(timestamp_node->name,
				 BAD_CAST "timestamp",
				 10)){
		    offset = g_ascii_strtoull(xmlGetProp(timestamp_node,
							 BAD_CAST "offset"),
					      NULL,
					      10);
		      
		    break;
		  }
		}

		timestamp_node = timestamp_node->next;
	      }     
		
	      /* 1st attempt */
	      timestamp->timer.ags_offset.offset = (guint64) relative_offset * floor((double) position_x / (double) relative_offset);
		
	      first_x = ags_wave_editor_paste_wave_all(wave_editor,
						       machine,
						       notebook,
						       wave_node,
						       timestamp,
						       match_line,
						       paste_from_position,
						       position_x,
						       &last_x);

	      /* 2nd attempt */
	      timestamp->timer.ags_offset.offset += relative_offset;

	      ags_wave_editor_paste_wave_all(wave_editor,
					     machine,
					     notebook,
					     wave_node,
					     timestamp,
					     match_line,
					     paste_from_position,
					     position_x,
					     &last_x);
		
	    }
	  }

	  wave_node = wave_node->next;
	}	  
      }
    }

    wave_list_node = wave_list_node->next;
  }    

  g_object_unref(timestamp);

  return(first_x);
}

void
ags_wave_editor_paste(AgsWaveEditor *wave_editor)
{
  AgsWindow *window;
  AgsWaveWindow *wave_window;
  AgsWaveToolbar *wave_toolbar;
  AgsWaveEdit *wave_edit;
  AgsNotebook *notebook;
  AgsMachine *machine;
  
  AgsWave *wave;

  GObject *soundcard;

  xmlDoc *clipboard;
  xmlNode *audio_node;
  xmlNode *wave_node;
  
  gchar *buffer;

  gdouble bpm;
  guint samplerate;
  guint64 relative_offset;
  guint64 position_x;
  gint64 first_x, last_x;
  double zoom, zoom_factor;
  gdouble delay_factor;
  gboolean paste_from_position;

  if(!AGS_IS_WAVE_EDITOR(wave_editor) ||
     wave_editor->focused_wave_edit == NULL){
    return;
  }
  
  if((machine = wave_editor->selected_machine) != NULL){
    machine = wave_editor->selected_machine;

    wave_window = (AgsWaveWindow *) gtk_widget_get_ancestor(GTK_WIDGET(wave_editor),
							    AGS_TYPE_WAVE_WINDOW);
    window = (AgsWindow *) wave_window->parent_window;  

    wave_toolbar = wave_editor->wave_toolbar;
    wave_edit = wave_editor->focused_wave_edit;

    notebook = wave_editor->notebook;

    bpm = gtk_spin_button_get_value(window->navigation->bpm);

    /* zoom */
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom) - 2.0);
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom));
    
    /* check all active tabs */
    g_object_get(machine->audio,
		 "output-soundcard", &soundcard,
		 "samplerate", &samplerate,
		 NULL);
    
    delay_factor = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(soundcard));

    relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

    /* get clipboard */
    buffer = gtk_clipboard_wait_for_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    
    if(buffer == NULL){
      if(soundcard != NULL){
	g_object_unref(soundcard);
      }
      
      return;
    }

    /* get position */
    position_x = 0;
    
    if(wave_toolbar->selected_edit_mode == wave_toolbar->position){
      last_x = 0;
      paste_from_position = TRUE;

      position_x = 15.0 * delay_factor * wave_editor->focused_wave_edit->cursor_position_x * samplerate / (16.0 * bpm);
      
#ifdef DEBUG
      printf("pasting at position: [%u]\n", position_x);
#endif
    }else{
      paste_from_position = FALSE;
    }

    /* get xml tree */
    clipboard = xmlReadMemory(buffer, strlen(buffer),
			      NULL, "UTF-8",
			      XML_PARSE_HUGE);
    audio_node = xmlDocGetRootElement(clipboard);

    first_x = -1;
    
    /* iterate xml tree */
    while(audio_node != NULL){
      if(audio_node->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp(BAD_CAST "audio", audio_node->name, 6)){
	  wave_node = audio_node->children;
	
	  //	  g_message("paste");
	  first_x = ags_wave_editor_paste_wave(wave_editor,
					       machine,
					       notebook,
					       audio_node,
					       paste_from_position,
					       position_x,
					       relative_offset);
	
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
    }

    if(soundcard != NULL){
      g_object_unref(soundcard);
    }

    gtk_widget_queue_draw(GTK_WIDGET(wave_edit));
  }
}

void
ags_wave_editor_copy(AgsWaveEditor *wave_editor)
{
  AgsMachine *machine;
  AgsNotebook *notebook;
  
  xmlDoc *clipboard;
  xmlNode *audio_node, *wave_list_node, *wave_node;

  GList *start_list_wave, *list_wave;

  xmlChar *buffer;

  int size;
  guint line;
  gint i;

  if(!AGS_IS_WAVE_EDITOR(wave_editor) ||
     wave_editor->focused_wave_edit == NULL){
    return;
  }
  
  if(wave_editor->selected_machine != NULL){
    machine = wave_editor->selected_machine;

    notebook = wave_editor->notebook;
  
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL,
			    BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);

    wave_list_node = xmlNewNode(NULL,
				BAD_CAST "wave-list");
    xmlAddChild(audio_node,
		wave_list_node);

    /* create wave nodes */
    g_object_get(machine->audio,
		 "wave", &start_list_wave,
		 NULL);
    
    i = 0;

    while((i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      list_wave = start_list_wave;

      /* copy */
      while(list_wave != NULL){
	g_object_get(list_wave->data,
		     "line", &line,
		     NULL);

	if(i != line){	
	  list_wave = list_wave->next;

	  continue;
	}

	//	g_message("copy %d", i);
	wave_node = ags_wave_copy_selection(AGS_WAVE(list_wave->data));
	xmlAddChild(wave_list_node,
		    wave_node);
	
	list_wave = list_wave->next;
      }

      i++;
    }

    g_list_free_full(start_list_wave,
		     g_object_unref);
    
    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   (gchar *) buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
  }
}

void
ags_wave_editor_cut(AgsWaveEditor *wave_editor)
{
  AgsMachine *machine;
  AgsNotebook *notebook;
  
  xmlDoc *clipboard;
  xmlNode *audio_node;
  xmlNode *wave_list_node, *wave_node;

  GList *start_list_wave, *list_wave;

  xmlChar *buffer;
  
  int size;
  guint line;
  gint i;

  if(!AGS_IS_WAVE_EDITOR(wave_editor) ||
     wave_editor->focused_wave_edit == NULL){
    return;
  }
  
  if(wave_editor->selected_machine != NULL){
    machine = wave_editor->selected_machine;

    notebook = wave_editor->notebook;

    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL,
			    BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);

    wave_list_node = xmlNewNode(NULL,
				BAD_CAST "wave-list");
    xmlAddChild(audio_node,
		wave_list_node);

    /* create wave nodes */
    g_object_get(machine->audio,
		 "wave", &start_list_wave,
		 NULL);
    
    i = 0;
    
    while((i = ags_notebook_next_active_tab(notebook,
					    i)) != -1){
      list_wave = start_list_wave;

      /* cut */
      while(list_wave != NULL){
	g_object_get(list_wave->data,
		     "line", &line,
		     NULL);

	if(i != line){	
	  list_wave = list_wave->next;

	  continue;
	}

	//	g_message("cut %d", i);
	wave_node = ags_wave_cut_selection(AGS_WAVE(list_wave->data));
	xmlAddChild(wave_list_node,
		    wave_node);
	
	list_wave = list_wave->next;
      }

      i++;
    }

    g_list_free_full(start_list_wave,
		     g_object_unref);

    gtk_widget_queue_draw(GTK_WIDGET(wave_editor->focused_wave_edit));

    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   (gchar *) buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
  }
}

void
ags_wave_editor_invert(AgsWaveEditor *wave_editor)
{
  g_message("ags_wave_editor_invert() - not implemented");
}

/**
 * ags_wave_editor_new:
 *
 * Create a new #AgsWaveEditor.
 *
 * Returns: a new #AgsWaveEditor
 *
 * Since: 3.0.0
 */
AgsWaveEditor*
ags_wave_editor_new()
{
  AgsWaveEditor *wave_editor;

  wave_editor = (AgsWaveEditor *) g_object_new(AGS_TYPE_WAVE_EDITOR,
					       NULL);

  return(wave_editor);
}
