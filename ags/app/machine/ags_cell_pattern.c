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

#include <ags/app/machine/ags_cell_pattern.h>
#include <ags/app/machine/ags_cell_pattern_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <gdk/gdkkeysyms.h>

void ags_cell_pattern_class_init(AgsCellPatternClass *cell_pattern);
void ags_cell_pattern_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_cell_pattern_init(AgsCellPattern *cell_pattern);
void ags_cell_pattern_finalize(GObject *gobject);

gboolean ags_cell_pattern_is_connected(AgsConnectable *connectable);
void ags_cell_pattern_connect(AgsConnectable *connectable);
void ags_cell_pattern_disconnect(AgsConnectable *connectable);

void ags_cell_pattern_draw_func(GtkDrawingArea *drawing_area,
				cairo_t *cr,
				int width,
				int height,
				AgsCellPattern *cell_pattern);

/**
 * SECTION:ags_cell_pattern
 * @short_description: cell pattern sequencer
 * @title: AgsCellPattern
 * @section_id:
 * @include: ags/app/machine/ags_cell_pattern.h
 *
 * The #AgsCellPattern is a composite widget to act as cell pattern sequencer.
 */

static gpointer ags_cell_pattern_parent_class = NULL;

GType
ags_cell_pattern_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_cell_pattern = 0;

    static const GTypeInfo ags_cell_pattern_info = {
      sizeof(AgsCellPatternClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_cell_pattern_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCellPattern),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_cell_pattern_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_cell_pattern_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_cell_pattern = g_type_register_static(GTK_TYPE_GRID,
						   "AgsCellPattern", &ags_cell_pattern_info,
						   0);
    
    g_type_add_interface_static(ags_type_cell_pattern,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_cell_pattern);
  }

  return g_define_type_id__volatile;
}

void
ags_cell_pattern_class_init(AgsCellPatternClass *cell_pattern)
{
  GObjectClass *gobject;

  ags_cell_pattern_parent_class = g_type_class_peek_parent(cell_pattern);

  /* GObjectClass */
  gobject = (GObjectClass *) cell_pattern;

  gobject->finalize = ags_cell_pattern_finalize;
}

void
ags_cell_pattern_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_cell_pattern_is_connected;  
  connectable->connect = ags_cell_pattern_connect;
  connectable->disconnect = ags_cell_pattern_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_cell_pattern_init(AgsCellPattern *cell_pattern)
{
  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;

  gdouble gui_scale_factor;
  GtkEventController *event_controller;

  application_context = ags_application_context_get_instance();
  
  gtk_widget_set_can_focus((GtkWidget *) cell_pattern,
			   TRUE);

  cell_pattern->flags = 0;
  cell_pattern->connectable_flags = 0;

  cell_pattern->key_mask = 0;

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  cell_pattern->cell_width = (guint) (gui_scale_factor * AGS_CELL_PATTERN_DEFAULT_CELL_WIDTH);
  cell_pattern->cell_height = (guint) (gui_scale_factor * AGS_CELL_PATTERN_DEFAULT_CELL_HEIGHT);

  cell_pattern->n_cols = AGS_CELL_PATTERN_DEFAULT_CONTROLS_HORIZONTALLY;
  cell_pattern->n_rows = AGS_CELL_PATTERN_DEFAULT_CONTROLS_VERTICALLY;

  cell_pattern->cursor_x = 0;
  cell_pattern->cursor_y = 0;

  cell_pattern->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();  

  gtk_widget_set_size_request((GtkWidget *) cell_pattern->drawing_area,
			      AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_HORIZONTALLY * cell_pattern->cell_width + 1, AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY * cell_pattern->cell_height + 1);

  gtk_widget_set_valign((GtkWidget *) cell_pattern->drawing_area,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) cell_pattern->drawing_area,
			GTK_ALIGN_FILL);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) cell_pattern->drawing_area,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_cell_pattern_key_pressed_callback), cell_pattern);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_cell_pattern_key_released_callback), cell_pattern);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_cell_pattern_modifiers_callback), cell_pattern);

  event_controller = (GtkEventController *) gtk_gesture_click_new();
  gtk_widget_add_controller((GtkWidget *) cell_pattern,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_cell_pattern_gesture_click_pressed_callback), cell_pattern);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_cell_pattern_gesture_click_released_callback), cell_pattern);

  gtk_drawing_area_set_draw_func(cell_pattern->drawing_area,
				 (GtkDrawingAreaDrawFunc) ags_cell_pattern_draw_func,
				 cell_pattern,
				 NULL);
  
  gtk_grid_attach((GtkGrid *) cell_pattern,
		   (GtkWidget *) cell_pattern->drawing_area,
		   0, 0,
		   1, 1);
    
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, (double) AGS_CELL_PATTERN_DEFAULT_CONTROLS_VERTICALLY - 1.0, 1.0, 1.0, (gdouble) AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY);

  cell_pattern->vscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,
								adjustment);

  gtk_widget_set_valign((GtkWidget *) cell_pattern->vscrollbar,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) cell_pattern->vscrollbar,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) cell_pattern,
		   (GtkWidget *) cell_pattern->vscrollbar,
		   1, 0,
		   1, 1);

  cell_pattern->hscrollbar = NULL;
  
  /* led */
  cell_pattern->active_led = 0;

  cell_pattern->hled_array = ags_led_array_new(GTK_ORIENTATION_HORIZONTAL,
					       cell_pattern->cell_width,
					       (guint) (gui_scale_factor * AGS_CELL_PATTERN_DEFAULT_CELL_HEIGHT),		       
					       cell_pattern->n_cols);

  gtk_widget_set_valign((GtkWidget *) cell_pattern->hled_array,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) cell_pattern->hled_array,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) cell_pattern,
		   (GtkWidget *) cell_pattern->hled_array,
		   0, 2,
		   1, 1);
  gtk_widget_show((GtkWidget *) cell_pattern->hled_array);

  g_signal_connect(application_context, "update-ui",
		   G_CALLBACK(ags_cell_pattern_update_ui_callback), cell_pattern);
}

void
ags_cell_pattern_finalize(GObject *gobject)
{
  AgsCellPattern *cell_pattern;
  
  AgsApplicationContext *application_context;

  cell_pattern = AGS_CELL_PATTERN(gobject);

  application_context = ags_application_context_get_instance();

  application_context = ags_application_context_get_instance();

  g_object_disconnect(application_context,
		      "any_signal::update-ui",
		      G_CALLBACK(ags_cell_pattern_update_ui_callback),
		      (gpointer) cell_pattern,
		      NULL);

  /* call parent */
  G_OBJECT_CLASS(ags_cell_pattern_parent_class)->finalize(gobject);
}

gboolean
ags_cell_pattern_is_connected(AgsConnectable *connectable)
{
  AgsCellPattern *cell_pattern;
  
  gboolean is_connected;
  
  cell_pattern = AGS_CELL_PATTERN(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (cell_pattern->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_cell_pattern_connect(AgsConnectable *connectable)
{
  AgsCellPattern *cell_pattern;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  /* AgsCellPattern */
  cell_pattern = AGS_CELL_PATTERN(connectable);

  cell_pattern->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect(G_OBJECT(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar)), "value_changed",
		   G_CALLBACK(ags_cell_pattern_adjustment_value_changed_callback), (gpointer) cell_pattern);
}

void
ags_cell_pattern_disconnect(AgsConnectable *connectable)
{
  AgsCellPattern *cell_pattern;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  /* AgsCellPattern */
  cell_pattern = AGS_CELL_PATTERN(connectable);

  cell_pattern->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(G_OBJECT(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar)),
		      "any_signal::value_changed",
		      G_CALLBACK(ags_cell_pattern_adjustment_value_changed_callback),
		      (gpointer) cell_pattern,
		      NULL);
}

void
ags_cell_pattern_draw_func(GtkDrawingArea *drawing_area,
			   cairo_t *cr,
			   int width,
			   int height,
			   AgsCellPattern *cell_pattern)
{
  g_return_if_fail(AGS_IS_CELL_PATTERN(cell_pattern));
  
  cairo_push_group(cr);

  /* the grid */
  ags_cell_pattern_draw_grid(cell_pattern, cr);

  /* the pattern */  
  ags_cell_pattern_draw_matrix(cell_pattern, cr);

  ags_cell_pattern_draw_cursor(cell_pattern, cr);

  /* paint */
  cairo_pop_group_to_source(cr);

  cairo_paint(cr);
}

void
ags_cell_pattern_draw_grid(AgsCellPattern *cell_pattern, cairo_t *cr)
{
  AgsMachine *machine;
  
  GtkStyleContext *style_context;
  GtkSettings *settings;

  GdkRGBA fg_color;
  GdkRGBA bg_color;

  AgsChannel *start_channel, *nth_channel;
  AgsChannel *channel, *prev_pad;

  guint input_pads;
  guint gutter;
  guint current_gutter;
  int i, j;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;

  GValue value = G_VALUE_INIT;

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						   AGS_TYPE_MACHINE);

  /* retrieve some audio fields */
  g_object_get(machine->audio,
	       "input-pads", &input_pads,
	       "input", &start_channel,
	       NULL);

  if(input_pads == 0){
    if(start_channel != NULL){
      g_object_unref(start_channel);
    }

    return;
  }

  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) cell_pattern);

  settings = gtk_settings_get_default();

  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);
    
  bg_success = gtk_style_context_lookup_color(style_context,
					      "theme_bg_color",
					      &bg_color);

  if(!fg_success ||
     !bg_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");

    gdk_rgba_parse(&bg_color,
		   "#cbd5d9");
  }
  
  if(input_pads > AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
    gutter = AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY;
  }else{
    gutter = input_pads;
  }

  current_gutter = (guint) gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar));
  
  /* clear bg */  
  cairo_set_source_rgba(cr,
			bg_color.red,
			bg_color.green,
			bg_color.blue,
			bg_color.alpha);

  cairo_rectangle(cr,
		  0.0, 0.0,
		  (gdouble) cell_pattern->cell_width * AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_HORIZONTALLY, (gdouble) gutter * cell_pattern->cell_height);

  cairo_fill(cr);

  if(input_pads - ((guint) current_gutter + AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY) > AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
    nth_channel = ags_channel_nth(start_channel,
				  input_pads - (current_gutter + AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY));
  }else if(input_pads > AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
    nth_channel = ags_channel_nth(start_channel,
				  AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY);
  }else{
    nth_channel = ags_channel_nth(start_channel,
				  input_pads - 1);
  }
  
  if(nth_channel == NULL){
    if(start_channel != NULL){
      g_object_unref(start_channel);
    }
        
    return;
  }

  channel = nth_channel;
  g_object_ref(channel);
  
  prev_pad = NULL;

  /* the grid */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.green,
			fg_color.blue,
			fg_color.alpha);

  cairo_set_line_width(cr,
		       0.625);

  for(j = 0; j < AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_HORIZONTALLY; j++){
    cairo_move_to(cr,
		  (double) j * (double) cell_pattern->cell_width, 0.0);

    cairo_line_to(cr,
		  (double) j * (double) cell_pattern->cell_width, (double) gutter * (double) cell_pattern->cell_height);

    cairo_stroke(cr);
  }
  
  for (i = 0; channel != NULL && i < gutter; i++){
    cairo_move_to(cr,
		  0.0, (double) i * (double) cell_pattern->cell_height);

    cairo_line_to(cr,
		  (double) AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_HORIZONTALLY * (double) cell_pattern->cell_width, (double) i * (double) cell_pattern->cell_height);

    cairo_stroke(cr);

    /* iterate */
    prev_pad = ags_channel_prev_pad(channel);

    g_object_unref(channel);

    channel = prev_pad;
  }

  /* unref */
  g_object_unref(start_channel);
  g_object_unref(nth_channel);

  if(prev_pad != NULL){
    g_object_unref(prev_pad);
  }
}

void
ags_cell_pattern_draw_matrix(AgsCellPattern *cell_pattern, cairo_t *cr)
{
  AgsMachine *machine;

  AgsChannel *start_channel, *nth_channel;
  AgsChannel *channel, *prev_pad;

  guint input_pads;
  guint gutter;
  guint current_gutter;
  int i, j;

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						   AGS_TYPE_MACHINE);

  /* get some audio fields */
  g_object_get(machine->audio,
	       "input-pads", &input_pads,
	       "input", &start_channel,
	       NULL);

  if(input_pads > AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
    gutter = AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY;
  }else{
    gutter = input_pads;
  }

  current_gutter = (guint) gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar));

  nth_channel = ags_channel_nth(start_channel,
				input_pads - current_gutter - 1);

  if(nth_channel == NULL){
    if(start_channel != NULL){
      g_object_unref(start_channel);
    }

    return;
  }

  channel = nth_channel;
  g_object_ref(channel);
  
  prev_pad = NULL;

  for (i = 0; channel != NULL && i < gutter; i++){
    for(j = 0; j < AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_HORIZONTALLY; j++){
      ags_cell_pattern_redraw_gutter_point(cell_pattern, cr,
					   channel,
					   j, i);
    }
    
    /* iterate */
    prev_pad = ags_channel_prev_pad(channel);

    g_object_unref(channel);

    channel = prev_pad;
  }

  /* unref */
  g_object_unref(start_channel);
  g_object_unref(nth_channel);

  if(prev_pad != NULL){
    g_object_unref(prev_pad);
  }
}

void
ags_cell_pattern_draw_cursor(AgsCellPattern *cell_pattern, cairo_t *cr)
{
  guint i, j;

  if(cell_pattern->cursor_y >= gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar)) &&
     cell_pattern->cursor_y < gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar)) + cell_pattern->n_rows){
    i = cell_pattern->cursor_y - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar));
    j = cell_pattern->cursor_x;
    
    if((AGS_CELL_PATTERN_CURSOR_ON & (cell_pattern->flags)) != 0){
      ags_cell_pattern_highlight_gutter_point(cell_pattern, cr,
					      j, i);
    }else{
      ags_cell_pattern_unpaint_gutter_point(cell_pattern, cr,
					    j, i);
    }
  }
}

void
ags_cell_pattern_redraw_gutter_point(AgsCellPattern *cell_pattern, cairo_t *cr,
				     AgsChannel *channel,
				     guint j, guint i)
{
  AgsMachine *machine;

  GList *start_pattern;
  
  gboolean do_highlight;

  if(channel == NULL ||
     channel->pattern == NULL){
    return;
  }

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						   AGS_TYPE_MACHINE);

  /* get channel fields */
  g_object_get(channel,
	       "pattern", &start_pattern,
	       NULL);

  /* redraw */
  do_highlight = ags_pattern_get_bit(start_pattern->data,
				     machine->bank_0,
				     machine->bank_1,
				     j);
  
  if(do_highlight){
    ags_cell_pattern_highlight_gutter_point(cell_pattern, cr,
					    j, i);
  }else{
    ags_cell_pattern_unpaint_gutter_point(cell_pattern, cr,
					  j, i);
  }

  g_list_free_full(start_pattern,
		   g_object_unref);
}

void
ags_cell_pattern_highlight_gutter_point(AgsCellPattern *cell_pattern, cairo_t *cr,
					guint j, guint i)
{
  GtkStyleContext *style_context;
  GtkSettings *settings;

  GdkRGBA fg_color;

  gboolean dark_theme;
  gboolean fg_success;
  
  style_context = gtk_widget_get_style_context((GtkWidget *) cell_pattern);

  settings = gtk_settings_get_default();

  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);
  
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.green,
			fg_color.blue,
			fg_color.alpha);
  
  cairo_rectangle(cr,
		  (gdouble) j * (gdouble) cell_pattern->cell_width + 1.0, (gdouble) i * (gdouble) cell_pattern->cell_height + 1.0,
		  (gdouble) cell_pattern->cell_width - 1.0, (gdouble) cell_pattern->cell_height - 1.0);

  cairo_fill(cr);
}

void
ags_cell_pattern_unpaint_gutter_point(AgsCellPattern *cell_pattern, cairo_t *cr,
				      guint j, guint i)
{
  GtkStyleContext *style_context;
  GtkSettings *settings;

  GdkRGBA bg_color;

  gboolean dark_theme;
  gboolean bg_success;
  
  style_context = gtk_widget_get_style_context((GtkWidget *) cell_pattern);

  settings = gtk_settings_get_default();

  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  /* colors */
  bg_success = gtk_style_context_lookup_color(style_context,
					      "theme_bg_color",
					      &bg_color);
  
  cairo_set_source_rgba(cr,
			bg_color.red,
			bg_color.green,
			bg_color.blue,
			bg_color.alpha);
  
  cairo_rectangle(cr,
		  (gdouble) j * (gdouble) cell_pattern->cell_width + 1.0, (gdouble) i * (gdouble) cell_pattern->cell_height + 1.0,
		  (gdouble) cell_pattern->cell_width - 1.0, (gdouble) cell_pattern->cell_height - 1.0);

  cairo_fill(cr);
}

/**
 * ags_cell_pattern_new:
 *
 * Creates an #AgsCellPattern
 *
 * Returns: a new #AgsCellPattern
 *
 * Since: 3.0.0
 */
AgsCellPattern*
ags_cell_pattern_new()
{
  AgsCellPattern *cell_pattern;

  cell_pattern = (AgsCellPattern *) g_object_new(AGS_TYPE_CELL_PATTERN,
						 NULL);

  return(cell_pattern);
}
