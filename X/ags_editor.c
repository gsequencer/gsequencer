#include "ags_editor.h"
#include "ags_editor_callbacks.h"

#include "../audio/ags_audio.h"
#include "../audio/ags_notation.h"

#include <math.h>
#include <cairo.h>

GType ags_editor_get_type(void);
void ags_editor_class_init(AgsEditorClass *editor);
void ags_editor_init(AgsEditor *editor);
void ags_editor_connect(AgsEditor *editor);
void ags_editor_destroy(GtkObject *object);
void ags_editor_show(GtkWidget *widget);

void ags_editor_real_change_machine(AgsEditor *editor, AgsMachine *machine);

GtkMenu* ags_editor_popup_new(AgsEditor *editor);

enum{
  CHANGE_MACHINE,
  LAST_SIGNAL,
};

static guint editor_signals[LAST_SIGNAL];

GtkStyle *editor_style;

GType
ags_editor_get_type(void)
{
  static GType editor_type = 0;

  if (!editor_type){
    static const GtkTypeInfo editor_info = {
      "AgsEditor\0",
      sizeof(AgsEditor), /* base_init */
      sizeof(AgsEditorClass), /* base_finalize */
      (GtkClassInitFunc) ags_editor_class_init,
      (GtkObjectInitFunc) ags_editor_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };
    editor_type = gtk_type_unique (GTK_TYPE_VBOX, &editor_info);
  }

  return (editor_type);
}

void
ags_editor_class_init(AgsEditorClass *editor)
{
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
ags_editor_init(AgsEditor *editor)
{
  GtkHPaned *paned;
  GtkScrolledWindow *scrolled_window;
  GtkTable *table;
  GtkAdjustment *adjustment;

  g_signal_connect_after((GObject *) editor, "parent-set\0",
			 G_CALLBACK(ags_editor_parent_set_callback), editor);

  editor->flags = 0;

  editor->toolbar = ags_toolbar_new();
  gtk_box_pack_start((GtkBox *) editor,
		     (GtkWidget *) editor->toolbar,
		     FALSE, FALSE, 0);

  paned = (GtkHPaned *) gtk_hpaned_new();
  gtk_widget_set_events (GTK_WIDGET (paned), GDK_BUTTON_PRESS_MASK);
  gtk_box_pack_start((GtkBox *) editor,
		     (GtkWidget *) paned,
		     TRUE, TRUE, 0);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_paned_add1((GtkPaned *) paned, (GtkWidget *) scrolled_window);

  editor->index_radio = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_scrolled_window_add_with_viewport(scrolled_window, (GtkWidget *) editor->index_radio);

  editor->selected = NULL;

  table = (GtkTable *) gtk_table_new(4, 3, FALSE);
  gtk_paned_add2((GtkPaned *) paned, (GtkWidget *) table);

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
			 //                         | GDK_POINTER_MOTION_MASK
			 //                         | GDK_POINTER_MOTION_HINT_MASK
			 );

  gtk_table_attach(table, (GtkWidget *) editor->drawing_area,
		   1, 2, 1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  editor->width = 0;
  editor->height = 0;
  editor->map_width = 128 * 64;
  editor->map_height = 0;

  editor->control_height = 10;
  editor->control_margin_y = 2;

  editor->y0 = 0;
  editor->y1 = 0;

  editor->nth_y = 0;
  editor->stop_y = 0;

  /* AgsEditorControlCurrent is used by ags_editor_draw_segment */
  editor->control_current.control_count = 128;
  editor->control_current.control_width = 64;

  editor->control_current.x0 = 0;
  editor->control_current.x1 = 0;

  editor->control_current.nth_x = 0;

  /* AgsEditorControlUnit is used by ags_editor_draw_notation */
  editor->control_unit.control_count = 16 * 128;
  editor->control_unit.control_width = 4;

  editor->control_unit.x0 = 0;
  editor->control_unit.x1 = 0;

  editor->control_unit.nth_x = 0;
  editor->control_unit.stop_x = 0;

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

  editor->popup = ags_editor_popup_new(editor);
}

void
ags_editor_connect(AgsEditor *editor)
{
  GtkHPaned *hpaned;

  g_signal_connect((GObject *) editor, "destroy\0",
		   G_CALLBACK(ags_editor_destroy_callback), (gpointer) editor);

  g_signal_connect((GObject *) editor, "show\0",
		   G_CALLBACK(ags_editor_show_callback), (gpointer) editor);

  hpaned = (gtk_container_get_children((GtkContainer *) editor))->next->data;

  g_signal_connect ((GObject *) hpaned, "button_press_event\0",
                    G_CALLBACK (ags_editor_button_press_callback), (gpointer) editor);

  g_signal_connect_after ((GObject *) editor->drawing_area, "expose_event\0",
			  G_CALLBACK (ags_editor_drawing_area_expose_event), (gpointer) editor);

  g_signal_connect_after ((GObject *) editor->drawing_area, "configure_event\0",
			  G_CALLBACK (ags_editor_drawing_area_configure_event), (gpointer) editor);

  g_signal_connect ((GObject *) editor->drawing_area, "button_press_event\0",
                    G_CALLBACK (ags_editor_drawing_area_button_press_event), (gpointer) editor);

  g_signal_connect ((GObject *) editor->drawing_area, "button_release_event\0",
                    G_CALLBACK (ags_editor_drawing_area_button_release_event), (gpointer) editor);

  //  g_signal_connect ((GObject *) editor->drawing_area, "motion_notify_event\0",
  //		    G_CALLBACK (ags_editor_drawing_area_motion_notify_event), (gpointer) editor);

  g_signal_connect ((GObject *) editor->vscrollbar, "change-value\0",
		    G_CALLBACK (ags_editor_vscrollbar_change_value), (gpointer) editor);

  g_signal_connect ((GObject *) editor->hscrollbar, "change-value\0",
		    G_CALLBACK (ags_editor_hscrollbar_change_value), (gpointer) editor);

  ags_toolbar_connect(editor->toolbar);
  ags_notebook_connect(editor->notebook);
  ags_meter_connect(editor->meter);
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
ags_editor_draw_segment(AgsEditor *editor)
{
  GtkWidget *widget;
  cairo_t *cr;
  guint i;
  double tic;

  widget = (GtkWidget *) editor->drawing_area;

  cr = gdk_cairo_create(widget->window);

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

  tic = exp2((double) gtk_option_menu_get_history(editor->toolbar->tic) - 4.0);

  if(tic > 1){
    guint j, j_set;

    if((j_set = editor->control_current.nth_x % ((guint) tic)) != 0){
      i = editor->control_current.x0;

      if(i < editor->width){
	j = (guint) tic - j_set;
	cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);

	goto ags_editor_draw_segment0;
      }
    }else{
      i = editor->control_current.x0;
    }

    for(; i < editor->width; ){
      cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);

      cairo_move_to(cr, (double) i, 0.0);
      cairo_line_to(cr, (double) i, (double) editor->height);
      cairo_stroke(cr);

      i += editor->control_current.control_width;

      cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);

      for(j = 0; i < editor->width && j < ((guint) tic) - 1; j++){
      ags_editor_draw_segment0:
	cairo_move_to(cr, (double) i, 0.0);
	cairo_line_to(cr, (double) i, (double) editor->height);
	cairo_stroke(cr);

	i += editor->control_current.control_width;
      }
    }
  }else{
    cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);

    for(i = editor->control_current.x0 ; i < editor->width;){
      cairo_move_to(cr, (double) i, 0.0);
      cairo_line_to(cr, (double) i, (double) editor->height);
      cairo_stroke(cr);
	
      i += editor->control_current.control_width;
    }
  }
}

void
ags_editor_draw_notation(AgsEditor *editor)
{
  AgsMachine *machine;
  GtkWidget *widget;
  cairo_t *cr;
  AgsNote *note;
  GList *list_notation, *list_note;
  double tic;
  guint control_height;
  guint x, y, width, height;

  if(editor->selected == NULL ||
     (machine = (AgsMachine *) g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE))) == NULL ||
     machine->audio->notation == NULL)
    return;

  widget = (GtkWidget *) editor->drawing_area;

  cr = gdk_cairo_create(widget->window);
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);

  list_notation = g_list_nth(machine->audio->notation, gtk_notebook_get_current_page((GtkNotebook *) editor->notebook));
  list_note = AGS_NOTATION(list_notation->data)->note;

  control_height = editor->control_height - 2 * editor->control_margin_y;

  /* draw controls smaller than editor->nth_x */
  while(list_note != NULL && (note = (AgsNote *) list_note->data)->x[0] < editor->control_unit.nth_x){
    if(note->x[1] >= editor->control_unit.nth_x){
      if(note->y >= editor->nth_y && note->y <= editor->stop_y){
	x = 0;
	y = (note->y - editor->nth_y) * editor->control_height + editor->y0 + editor->control_margin_y;

	width = (note->x[1] - editor->control_unit.nth_x) * editor->control_unit.control_width;

	if(width > widget->allocation.width)
	  width = widget->allocation.width;

	height = control_height;

	cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	cairo_fill(cr);
      }else if(note->y == (editor->nth_y - 1) && editor->y0 != 0){
	if(editor->y0 > editor->control_margin_y){
	  x = 0;
	  width = (note->x[1] - editor->control_unit.nth_x) * editor->control_unit.control_width;

	  if(width > widget->allocation.width)
	    width = widget->allocation.width;

	  if(editor->y0 > control_height + editor->control_margin_y){
	    y = editor->y0 - (control_height + editor->control_margin_y);
	    height = control_height;
	  }else{
	    y = 0;
	    height = editor->y0 - editor->control_margin_y;
	  }

	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_fill(cr);
	}
      }else if(note->y == (editor->stop_y + 1) && editor->y1 != 0){
	if(editor->y1 > editor->control_margin_y){
	  x = 0;
	  width = (note->x[1] - editor->control_unit.nth_x) * editor->control_unit.control_width;

	  if(width > widget->allocation.width)
	    width = widget->allocation.width;

	  y = (note->y - editor->nth_y) * editor->control_height + editor->control_margin_y;

	  if(editor->y1 > control_height + editor->control_margin_y){
	    height = control_height;
	  }else{
	    height = editor->y1 - editor->control_margin_y;
	  }

	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_fill(cr);
	}
      }
    }

    list_note = list_note->next;
  }

  /* draw controls equal or greater than editor->nth_x */
  while(list_note != NULL && (note = (AgsNote *) list_note->data)->x[0] <= editor->control_unit.stop_x){
    if(note->y >= editor->nth_y && note->y <= editor->stop_y){
      x = (note->x[0] - editor->control_unit.nth_x) * editor->control_unit.control_width + editor->control_unit.x0;
      y = (note->y - editor->nth_y) * editor->control_height + editor->y0 + editor->control_margin_y;

      width = (note->x[1] - note->x[0]) * editor->control_unit.control_width + editor->control_unit.x0;
      
      if(x + width > widget->allocation.width)
	width = widget->allocation.width - x;

      height = control_height;

      cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
      cairo_fill(cr);
    }else if(note->y == (editor->nth_y - 1) && editor->y0 != 0){
      if(editor->y0 > editor->control_margin_y){
	x = (note->x[0] - editor->control_unit.nth_x) * editor->control_unit.control_width + editor->control_unit.x0;
	width = (note->x[1] - note->x[0]) * editor->control_unit.control_width + editor->control_unit.x0;
      
	if(x + width > widget->allocation.width)
	  width = widget->allocation.width - x;

	if(editor->y0 > control_height + editor->control_margin_y){
	  y = editor->y0 - (control_height + editor->control_margin_y);
	  height = control_height;
	}else{
	  y = 0;
	  height = editor->y0 - editor->control_margin_y;
	}

	cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	cairo_fill(cr);
      }
    }else if(note->y == (editor->stop_y + 1) && editor->y1 != 0){
      if(editor->y1 > editor->control_margin_y){
	x = (note->x[0] - editor->control_unit.nth_x) * editor->control_unit.control_width + editor->control_unit.x0;
	width = (note->x[1] - note->x[0]) * editor->control_unit.control_width + editor->control_unit.x0;
      
	if(x + width > widget->allocation.width)
	  width = widget->allocation.width - x;

	y = (note->y - editor->nth_y) * editor->control_height + editor->control_margin_y;

	if(editor->y1 > control_height + editor->control_margin_y){
	  height = control_height;
	}else{
	  height = editor->y1 - editor->control_margin_y;
	}

	cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	cairo_fill(cr);
      }
    }

    list_note = list_note->next;
  }

  fprintf(stdout, "\n\0");
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
