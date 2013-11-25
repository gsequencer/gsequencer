/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_editor.h>
#include <ags/X/ags_editor_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_notation.h>

#include <math.h>
#include <cairo.h>

void ags_editor_class_init(AgsEditorClass *editor);
void ags_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_editor_init(AgsEditor *editor);
void ags_editor_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_editor_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_editor_finalize(GObject *gobject);
void ags_editor_connect(AgsConnectable *connectable);
void ags_editor_disconnect(AgsConnectable *connectable);
void ags_editor_destroy(GtkObject *object);
void ags_editor_show(GtkWidget *widget);

void ags_editor_real_change_machine(AgsEditor *editor, AgsMachine *machine);

GtkMenu* ags_editor_popup_new(AgsEditor *editor);

enum{
  CHANGE_MACHINE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_DEVOUT,
};

static gpointer ags_editor_parent_class = NULL;
static guint editor_signals[LAST_SIGNAL];

GtkStyle *editor_style;

GType
ags_editor_get_type(void)
{
  static GType ags_type_editor = 0;

  if(!ags_type_editor){
    static const GTypeInfo ags_editor_info = {
      sizeof (AgsEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_editor = g_type_register_static(GTK_TYPE_VBOX,
					     "AgsEditor\0", &ags_editor_info,
					     0);
    
    g_type_add_interface_static(ags_type_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_editor);
}

void
ags_editor_class_init(AgsEditorClass *editor)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_editor_parent_class = g_type_class_peek_parent(editor);

  /* GObjectClass */
  gobject = (GObjectClass *) editor;

  gobject->set_property = ags_editor_set_property;
  gobject->get_property = ags_editor_get_property;

  gobject->finalize = ags_editor_finalize;

  /* properties */
  param_spec = g_param_spec_object("devout\0",
				   "assigned devout\0",
				   "The devout it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  /*  */
  editor->change_machine = ags_editor_real_change_machine;

  editor_signals[CHANGE_MACHINE] =
    g_signal_new("change_machine\0",
                 G_TYPE_FROM_CLASS (editor),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsEditorClass, change_machine),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
                 G_TYPE_OBJECT);
}

void
ags_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_editor_connect;
  connectable->disconnect = ags_editor_disconnect;
}

void
ags_editor_init(AgsEditor *editor)
{
  GtkHPaned *paned;
  GtkScrolledWindow *scrolled_window;
  GtkTable *table;
  GtkLabel *label;
  GtkAdjustment *adjustment;
  GtkHBox *hbox;
  GtkMenuToolButton *menu_button;

  g_signal_connect_after((GObject *) editor, "parent-set\0",
			 G_CALLBACK(ags_editor_parent_set_callback), editor);

  editor->flags = 0;

  editor->version = AGS_EDITOR_DEFAULT_VERSION;
  editor->build_id = AGS_EDITOR_DEFAULT_BUILD_ID;

  editor->devout = NULL;

  editor->popup = ags_editor_popup_new(editor);

  editor->toolbar = ags_toolbar_new();
  gtk_box_pack_start((GtkBox *) editor,
		     (GtkWidget *) editor->toolbar,
		     FALSE, FALSE, 0);

  paned = (GtkHPaned *) gtk_hpaned_new();
  gtk_box_pack_start((GtkBox *) editor,
		     (GtkWidget *) paned,
		     TRUE, TRUE, 0);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_paned_pack1((GtkPaned *) paned, (GtkWidget *) scrolled_window, FALSE, TRUE);
  //  gtk_widget_set_size_request((GtkWidget *) scrolled_window, 180, -1);

  editor->index_radio = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_scrolled_window_add_with_viewport(scrolled_window, (GtkWidget *) editor->index_radio);

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(editor->index_radio),
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new("Notation\0");
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  menu_button = g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
			     "stock-id\0", GTK_STOCK_EXECUTE,
			     "menu\0", editor->popup,
			     NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(menu_button),
		     FALSE, FALSE,
		     0);

  editor->selected = NULL;

  table = (GtkTable *) gtk_table_new(4, 3, FALSE);
  gtk_paned_pack2((GtkPaned *) paned, (GtkWidget *) table, TRUE, FALSE);
  
  editor->notebook = ags_notebook_new();
  gtk_table_attach(table, (GtkWidget *) editor->notebook,
		   0, 3, 0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL,
		   0, 0);

  editor->meter = ags_meter_new();
  gtk_table_attach(table, (GtkWidget *) editor->meter,
		   0, 1, 1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  editor->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_style((GtkWidget *) editor->drawing_area, editor_style);
  gtk_widget_set_events (GTK_WIDGET (editor->drawing_area), GDK_EXPOSURE_MASK
                         | GDK_LEAVE_NOTIFY_MASK
                         | GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK
                         | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK
			 );

  gtk_table_attach(table, (GtkWidget *) editor->drawing_area,
		   1, 2, 1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  editor->control.note = ags_note_new();

  editor->width = 0;
  editor->height = 0;
  editor->map_width = AGS_EDITOR_MAX_CONTROLS * 64;
  editor->map_height = 0;

  editor->control_height = 10;
  editor->control_margin_y = 2;

  editor->control_width = 1;

  editor->y0 = 0;
  editor->y1 = 0;

  editor->nth_y = 0;
  editor->stop_y = 0;

  /* AgsEditorControlCurrent is used by ags_editor_draw_segment */
  editor->control_current.control_count = AGS_EDITOR_MAX_CONTROLS;
  editor->control_current.control_width = 64;

  editor->control_current.x0 = 0;
  editor->control_current.x1 = 0;

  editor->control_current.nth_x = 0;

  /* AgsEditorControlUnit is used by ags_editor_draw_notation */
  editor->control_unit.control_count = 16 * AGS_EDITOR_MAX_CONTROLS;
  editor->control_unit.control_width = 1 * 4;

  editor->control_unit.x0 = 0;
  editor->control_unit.x1 = 0;

  editor->control_unit.nth_x = 0;
  editor->control_unit.stop_x = 0;

  /* offset for pasting from clipboard */
  editor->selected_x = 0;
  editor->selected_y = 0;

  /* GtkScrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
  editor->vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(adjustment);
  gtk_table_attach(table, (GtkWidget *) editor->vscrollbar,
		   2, 3, 1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
  editor->hscrollbar = (GtkHScrollbar *) gtk_hscrollbar_new(adjustment);
  gtk_table_attach(table, (GtkWidget *) editor->hscrollbar,
		   1, 2, 2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  editor->tact_counter = 0;
}

void
ags_editor_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsEditor *editor;

  editor = AGS_EDITOR(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = (AgsDevout *) g_value_get_object(value);

      if(editor->devout == devout)
	return;

      if(devout != NULL)
	g_object_ref(devout);

      editor->devout = devout;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_editor_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsEditor *editor;

  editor = AGS_EDITOR(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    g_value_set_object(value, editor->devout);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_editor_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

void
ags_editor_connect(AgsConnectable *connectable)
{
  GtkHPaned *hpaned;
  AgsEditor *editor;

  editor = AGS_EDITOR(connectable);

  g_signal_connect((GObject *) editor, "destroy\0",
		   G_CALLBACK(ags_editor_destroy_callback), (gpointer) editor);

  g_signal_connect((GObject *) editor, "show\0",
		   G_CALLBACK(ags_editor_show_callback), (gpointer) editor);

  g_signal_connect ((GObject *) editor->index_radio, "button_press_event\0",
                    G_CALLBACK (ags_editor_button_press_callback), (gpointer) editor);

  g_signal_connect_after ((GObject *) editor->drawing_area, "expose_event\0",
			  G_CALLBACK (ags_editor_drawing_area_expose_event), (gpointer) editor);

  g_signal_connect_after ((GObject *) editor->drawing_area, "configure_event\0",
			  G_CALLBACK (ags_editor_drawing_area_configure_event), (gpointer) editor);

  g_signal_connect ((GObject *) editor->drawing_area, "button_press_event\0",
                    G_CALLBACK (ags_editor_drawing_area_button_press_event), (gpointer) editor);

  g_signal_connect ((GObject *) editor->drawing_area, "button_release_event\0",
                    G_CALLBACK (ags_editor_drawing_area_button_release_event), (gpointer) editor);

  g_signal_connect ((GObject *) editor->drawing_area, "motion_notify_event\0",
 		    G_CALLBACK (ags_editor_drawing_area_motion_notify_event), (gpointer) editor);

  g_signal_connect_after ((GObject *) editor->vscrollbar, "value-changed\0",
			  G_CALLBACK (ags_editor_vscrollbar_value_changed), (gpointer) editor);

  g_signal_connect_after ((GObject *) editor->hscrollbar, "value-changed\0",
			  G_CALLBACK (ags_editor_hscrollbar_value_changed), (gpointer) editor);

  /*  */
  //TODO:JK: uncomment me
  //  g_signal_connect((GObject *) editor->devout, "tic\0",
  //		   G_CALLBACK(ags_editor_tic_callback), (gpointer) editor);

  /*  */
  ags_toolbar_connect(editor->toolbar);
  ags_notebook_connect(editor->notebook);
  ags_meter_connect(editor->meter);
}

void
ags_editor_disconnect(AgsConnectable *connectable)
{
}

void
ags_editor_destroy(GtkObject *object)
{
}

void
ags_editor_show(GtkWidget *widget)
{
  GList *list;
  /*
  list = gtk_container_get_children((GtkContainer *) widget);

  while(list != NULL){
    gtk_widget_show_all((GtkWidget *) list->data);

    list = list->next;
  }
  */
}

void
ags_editor_real_change_machine(AgsEditor *editor, AgsMachine *machine)
{
}

void
ags_editor_change_machine(AgsEditor *editor, AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_EDITOR(editor));

  g_object_ref((GObject *) editor);
  g_signal_emit((GObject *) editor,
		editor_signals[CHANGE_MACHINE], 0,
		machine);
  g_object_unref((GObject *) editor);
}

void
ags_editor_reset_vertically(AgsEditor *editor, guint flags)
{
  if(editor->selected != NULL){
    cairo_t *cr;
    gdouble value;

    value = GTK_RANGE(editor->vscrollbar)->adjustment->value;

    if((AGS_EDITOR_RESET_VSCROLLBAR & flags) != 0){
      GtkWidget *widget;
      GtkAdjustment *adjustment;
      guint height;

      widget = GTK_WIDGET(editor->drawing_area);
      adjustment = GTK_RANGE(editor->vscrollbar)->adjustment;
      
      if(editor->map_height > widget->allocation.height){
	height = widget->allocation.height;
	gtk_adjustment_set_upper(adjustment,
				 (gdouble) (editor->map_height - height));
	gtk_adjustment_set_value(adjustment, 0.0);
      }else{
	height = editor->map_height;
	
	gtk_adjustment_set_upper(adjustment, 0.0);
	gtk_adjustment_set_value(adjustment, 0.0);
      }
      
      editor->height = height;
    }

    editor->y0 = ((guint) round((double) value)) % editor->control_height;

    if(editor->y0 != 0){
      editor->y0 = editor->control_height - editor->y0;
    }

    editor->y1 = (editor->height - editor->y0) % editor->control_height;

    editor->nth_y = (guint) ceil(round((double) value) / (double)(editor->control_height));
    editor->stop_y = editor->nth_y + (editor->height - editor->y0 - editor->y1) / editor->control_height;

    /* refresh display */
    ags_meter_paint(editor->meter);

    cr = gdk_cairo_create(GTK_WIDGET(editor->drawing_area)->window);
    cairo_push_group(cr);

    ags_editor_draw_segment(editor, cr);
    ags_editor_draw_notation(editor, cr);

    if(editor->toolbar->selected_edit_mode == editor->toolbar->position){
      ags_editor_draw_position(editor, cr);
    }

    cairo_pop_group_to_source(cr);
    cairo_paint(cr);
  }
}

void
ags_editor_reset_horizontally(AgsEditor *editor, guint flags)
{
  if(editor->selected != NULL){
    cairo_t *cr;
    gdouble value;
    double tact_factor, zoom_factor;
    double tact;

    value = GTK_RANGE(editor->hscrollbar)->adjustment->value;

    zoom_factor = exp2(8.0 - (double) gtk_option_menu_get_history(editor->toolbar->zoom));

    tact_factor = exp2(8.0 - (double) gtk_option_menu_get_history(editor->toolbar->tact));
    tact = exp2((double) gtk_option_menu_get_history(editor->toolbar->tact) - 4.0);

    if((AGS_EDITOR_RESET_WIDTH & flags) != 0){
      editor->control_unit.control_width = (guint) (((double) editor->control_width * zoom_factor * tact));

      editor->control_current.control_count = (guint) ((double) editor->control_unit.control_count * tact);
      editor->control_current.control_width = (editor->control_width * zoom_factor * tact_factor * tact);

      editor->map_width = (guint) ((double) editor->control_current.control_count * (double) editor->control_current.control_width);
    }

    if((AGS_EDITOR_RESET_HSCROLLBAR & flags) != 0){
      GtkWidget *widget;
      GtkAdjustment *adjustment;
      guint width;

      widget = GTK_WIDGET(editor->drawing_area);
      adjustment = GTK_RANGE(editor->hscrollbar)->adjustment;

      if(editor->map_width > widget->allocation.width){
	width = widget->allocation.width;
	//	gtk_adjustment_set_upper(adjustment, (double) (editor->map_width - width));
	gtk_adjustment_set_upper(adjustment,
				 (gdouble) (editor->map_width - width));

	if(adjustment->value > adjustment->upper)
	  gtk_adjustment_set_value(adjustment, adjustment->upper);
      }else{
	width = editor->map_width;

	gtk_adjustment_set_upper(adjustment, 0.0);
	gtk_adjustment_set_value(adjustment, 0.0);
      }

      editor->width = width;
    }

    /* reset AgsEditorControlCurrent */
    if(editor->map_width > editor->width){
      editor->control_current.x0 = ((guint) round((double) value)) % editor->control_current.control_width;

      if(editor->control_current.x0 != 0){
	editor->control_current.x0 = editor->control_current.control_width - editor->control_current.x0;
      }

      editor->control_current.x1 = (editor->width - editor->control_current.x0) % editor->control_current.control_width;

      editor->control_current.nth_x = (guint) ceil((double)(value) / (double)(editor->control_current.control_width));
    }else{
      editor->control_current.x0 = 0;
      editor->control_current.x1 = 0;
      editor->control_current.nth_x = 0;
    }

    /* reset AgsEditorControlUnit */
    if(editor->map_width > editor->width){
      editor->control_unit.x0 = ((guint)round((double) value)) % editor->control_unit.control_width;

      if(editor->control_unit.x0 != 0)
	editor->control_unit.x0 = editor->control_unit.control_width - editor->control_unit.x0;
      
      editor->control_unit.x1 = (editor->width - editor->control_unit.x0) % editor->control_unit.control_width;
      
      editor->control_unit.nth_x = (guint) ceil(round((double) value) / (double) (editor->control_unit.control_width));
      editor->control_unit.stop_x = editor->control_unit.nth_x + (editor->width - editor->control_unit.x0 - editor->control_unit.x1) / editor->control_unit.control_width;
    }else{
      editor->control_unit.x0 = 0;
      editor->control_unit.x1 = 0;
      editor->control_unit.nth_x = 0;
    }

    /* refresh display */
    cr = gdk_cairo_create(GTK_WIDGET(editor->drawing_area)->window);
    cairo_push_group(cr);

    ags_editor_draw_segment(editor, cr);
    ags_editor_draw_notation(editor, cr);

    if(editor->toolbar->selected_edit_mode == editor->toolbar->position){
      ags_editor_draw_position(editor, cr);
    }

    cairo_pop_group_to_source(cr);
    cairo_paint(cr);
  }
}

void
ags_editor_draw_segment(AgsEditor *editor, cairo_t *cr)
{
  GtkWidget *widget;
  double tact;
  guint i, j;
  guint j_set;

  widget = (GtkWidget *) editor->drawing_area;

  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_rectangle(cr, 0.0, 0.0, (double) widget->allocation.width, (double) widget->allocation.height);
  cairo_fill(cr);

  cairo_set_line_width(cr, 1.0);

  cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);

  for(i = editor->y0 ; i < editor->height;){
    cairo_move_to(cr, 0.0, (double) i);
    cairo_line_to(cr, (double) editor->width, (double) i);
    cairo_stroke(cr);

    i += editor->control_height;
  }

  tact = exp2((double) gtk_option_menu_get_history(editor->toolbar->tact) - 4.0);

  i = editor->control_current.x0;
  
  if(i < editor->width &&
     tact > 1.0 ){
    j_set = editor->control_current.nth_x % ((guint) tact);
    cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);

    if(j_set != 0){
      j = j_set;
      goto ags_editor_draw_segment0;
    }
  }

  for(; i < editor->width; ){
    cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);
    
    cairo_move_to(cr, (double) i, 0.0);
    cairo_line_to(cr, (double) i, (double) editor->height);
    cairo_stroke(cr);
    
    i += editor->control_current.control_width;
    
    cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
    
    for(j = 1; i < editor->width && j < tact; j++){
    ags_editor_draw_segment0:
      cairo_move_to(cr, (double) i, 0.0);
      cairo_line_to(cr, (double) i, (double) editor->height);
      cairo_stroke(cr);
      
      i += editor->control_current.control_width;
    }
  }
}

void
ags_editor_draw_position(AgsEditor *editor, cairo_t *cr)
{
  guint selected_x, selected_y;
  guint x_offset[2], y_offset[2];
  guint x, y, width, height;
  gint size_width, size_height;
  
  selected_x = editor->selected_x * editor->control_unit.control_width;
  selected_y = editor->selected_y * editor->control_height;

  size_width = GTK_WIDGET(editor->drawing_area)->allocation.width;
  size_height = GTK_WIDGET(editor->drawing_area)->allocation.height;

  x_offset[0] = (guint) gtk_range_get_value(GTK_RANGE(editor->hscrollbar));
  x_offset[1] = x_offset[0] + (guint) size_width;

  y_offset[0] = (guint) gtk_range_get_value(GTK_RANGE(editor->vscrollbar));
  y_offset[1] = y_offset[0] + (guint) size_height;

  /* calculate horizontally values */
  if(selected_x < x_offset[0]){
    if(selected_x + editor->control_current.control_width > x_offset[0]){
      x = 0;
      width = selected_x + editor->control_current.control_width - x_offset[0];
    }else{
      return;
    }
  }else if(selected_x > x_offset[1]){
    return;
  }else{
    x = selected_x - x_offset[0];

    if(selected_x + editor->control_current.control_width < x_offset[1]){
      width = editor->control_current.control_width;
    }else{
      width = x_offset[1] - (selected_x + editor->control_current.control_width);
    }
  }

  /* calculate vertically values */
  if(selected_y < y_offset[0]){
    if(selected_y + editor->control_height > y_offset[0]){
      y = 0;
      height = selected_y + editor->control_height - y_offset[0];
    }else{
      return;
    }
  }else if(selected_y > y_offset[1]){
    return;
  }else{
    y = selected_y - y_offset[0];

    if(selected_y + editor->control_height < y_offset[1]){
      height = editor->control_height;
    }else{
      height = y_offset[1] - (selected_y + editor->control_height);
    }
  }

  /* draw */
  cairo_set_source_rgba(cr, 0.25, 0.5, 1.0, 0.5);
  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
  cairo_fill(cr);
}

void
ags_editor_draw_notation(AgsEditor *editor, cairo_t *cr)
{
  AgsMachine *machine;
  GtkWidget *widget;
  AgsNote *note;
  GList *list_notation, *list_note;
  guint x_offset;
  guint control_height;
  guint x, y, width, height;
  gint selected_channel;

  if(editor->selected == NULL ||
     (machine = (AgsMachine *) g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE))) == NULL ||
     machine->audio->notation == NULL)
    return;

  widget = (GtkWidget *) editor->drawing_area;

  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);

  selected_channel = gtk_notebook_get_current_page((GtkNotebook *) editor->notebook);
  
  if(selected_channel == -1)
    return;

  list_notation = g_list_nth(machine->audio->notation,
			     selected_channel);
  list_note = AGS_NOTATION(list_notation->data)->notes;

  control_height = editor->control_height - 2 * editor->control_margin_y;

  x_offset = (guint) GTK_RANGE(editor->hscrollbar)->adjustment->value;

  /* draw controls smaller than editor->nth_x */
  while(list_note != NULL && (note = (AgsNote *) list_note->data)->x[0] < editor->control_unit.nth_x){
    if(note->x[1] >= editor->control_unit.nth_x){
      if(note->y >= editor->nth_y && note->y <= editor->stop_y){
	x = 0;
	y = (note->y - editor->nth_y) * editor->control_height + editor->y0 + editor->control_margin_y;

	width = (guint) ((double) note->x[1] * editor->control_unit.control_width - (double) x_offset);

	if(width > widget->allocation.width)
	  width = widget->allocation.width;

	height = control_height;

	/* draw note */
	cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	cairo_fill(cr);

	/* check if note is selected */
	if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	  cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);

	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_stroke(cr);

	  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	}
      }else if(note->y == (editor->nth_y - 1) && editor->y0 != 0){
	if(editor->y0 > editor->control_margin_y){
	  x = 0;
	  width = (guint) ((double) note->x[1] * (double) editor->control_unit.control_width - x_offset);

	  if(width > widget->allocation.width)
	    width = widget->allocation.width;

	  if(editor->y0 > control_height + editor->control_margin_y){
	    y = editor->y0 - (control_height + editor->control_margin_y);
	    height = control_height;
	  }else{
	    y = 0;
	    height = editor->y0 - editor->control_margin_y;
	  }

	  /* draw note */
	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_fill(cr);

	  /* check if note is selected */
	  if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	    cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);
	    
	    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	    cairo_stroke(cr);
	    
	    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	  }
	}
      }else if(note->y == (editor->stop_y + 1) && editor->y1 != 0){
	if(editor->y1 > editor->control_margin_y){
	  x = 0;
	  width = note->x[1] * editor->control_unit.control_width - x_offset;

	  if(width > widget->allocation.width)
	    width = widget->allocation.width;

	  y = (note->y - editor->nth_y) * editor->control_height + editor->control_margin_y;

	  if(editor->y1 > control_height + editor->control_margin_y){
	    height = control_height;
	  }else{
	    height = editor->y1 - editor->control_margin_y;
	  }

	  /* draw note */
	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_fill(cr);
	  
	  /* check if note is selected */
	  if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	    cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);
	    
	    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	    cairo_stroke(cr);
	    
	    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	  }
	}
      }
    }

    list_note = list_note->next;
  }

  /* draw controls equal or greater than editor->nth_x */
  while(list_note != NULL && (note = (AgsNote *) list_note->data)->x[0] <= editor->control_unit.stop_x){
    if(note->y >= editor->nth_y && note->y <= editor->stop_y){
      x = (guint) note->x[0] * editor->control_unit.control_width;
      y = (note->y - editor->nth_y) * editor->control_height +
	editor->y0 +
	editor->control_margin_y;

      width = note->x[1] * editor->control_unit.control_width - x;
      x -= x_offset;

      if(x + width > widget->allocation.width)
	width = widget->allocation.width - x;

      height = control_height;

      /* draw note*/
      cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
      cairo_fill(cr);

      /* check if note is selected */
      if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);
	
	cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	cairo_stroke(cr);
	
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
      }
    }else if(note->y == (editor->nth_y - 1) && editor->y0 != 0){
      if(editor->y0 > editor->control_margin_y){
	x = note->x[0] * editor->control_unit.control_width - x_offset;
	width = note->x[1] * editor->control_unit.control_width - x_offset - x;
      
	if(x + width > widget->allocation.width)
	  width = widget->allocation.width - x;

	if(editor->y0 > control_height + editor->control_margin_y){
	  y = editor->y0 - (control_height + editor->control_margin_y);
	  height = control_height;
	}else{
	  y = 0;
	  height = editor->y0 - editor->control_margin_y;
	}

	/* draw note */
	cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	cairo_fill(cr);

	/* check if note is selected */
	if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	  cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);

	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_stroke(cr);

	  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	}
      }
    }else if(note->y == (editor->stop_y + 1) && editor->y1 != 0){
      if(editor->y1 > editor->control_margin_y){
	x = note->x[0] * editor->control_unit.control_width - x_offset;
	width = note->x[1] * editor->control_unit.control_width - x_offset - x;
      
	if(x + width > widget->allocation.width)
	  width = widget->allocation.width - x;

	y = (note->y - editor->nth_y) * editor->control_height + editor->control_margin_y;

	if(editor->y1 > control_height + editor->control_margin_y){
	  height = control_height;
	}else{
	  height = editor->y1 - editor->control_margin_y;
	}

	/* draw note */
	cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	cairo_fill(cr);

	/* check if note is selected */
	if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	  cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);

	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_stroke(cr);

	  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	}
      }
    }

    list_note = list_note->next;
  }
}

AgsEditor*
ags_editor_new()
{
  AgsEditor *editor;

  editor = (AgsEditor *) g_object_new(AGS_TYPE_EDITOR, NULL);

  return(editor);
}

GtkMenu*
ags_editor_popup_new(AgsEditor *editor)
{
  GtkMenu *popup;
  GtkMenuItem *item;
  GList *list;

  popup = (GtkMenu *) gtk_menu_new();
  g_object_set_data((GObject *) popup, g_type_name(AGS_TYPE_EDITOR), editor);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("add tab\0"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("remove tab\0"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("add index\0"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("remove index\0"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("link index\0"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  list = gtk_container_get_children((GtkContainer *) popup);

  g_signal_connect(G_OBJECT(list->data), "activate\0",
		   G_CALLBACK(ags_editor_popup_add_tab_callback), (gpointer) popup);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate\0",
		   G_CALLBACK(ags_editor_popup_remove_tab_callback), (gpointer) popup);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate\0",
		   G_CALLBACK(ags_editor_popup_add_index_callback), (gpointer) popup);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate\0",
		   G_CALLBACK(ags_editor_popup_remove_index_callback), (gpointer) popup);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate\0",
		   G_CALLBACK(ags_editor_popup_link_index_callback), (gpointer) popup);

  gtk_widget_show_all((GtkWidget *) popup);

  return(popup);
}
