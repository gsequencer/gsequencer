#include "ags_meter.h"
#include "ags_meter_callbacks.h"

#include "../ags_editor.h"

#include <math.h>

GType ags_meter_get_type(void);
void ags_meter_class_init(AgsMeterClass *meter);
void ags_meter_init(AgsMeter *meter);
void ags_meter_connect(AgsMeter *meter);
void ags_meter_destroy(GtkObject *object);
void ags_meter_show(GtkWidget *widget);

GtkStyle *meter_style;

GType
ags_meter_get_type(void)
{
  static GType meter_type = 0;

  if (!meter_type){
    static const GtkTypeInfo meter_info = {
      "AgsMeter\0",
      sizeof(AgsMeter), /* base_init */
      sizeof(AgsMeterClass), /* base_finalize */
      (GtkClassInitFunc) ags_meter_class_init,
      (GtkObjectInitFunc) ags_meter_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    meter_type = gtk_type_unique (GTK_TYPE_DRAWING_AREA, &meter_info);
  }

  return (meter_type);
}

void
ags_meter_class_init(AgsMeterClass *meter)
{
}

void
ags_meter_init(AgsMeter *meter)
{
  GtkWidget *widget;

  widget = (GtkWidget *) meter;
  gtk_widget_set_style(widget, meter_style);
  gtk_widget_set_size_request(widget, 60, -1);
  gtk_widget_set_events (GTK_WIDGET (meter), GDK_EXPOSURE_MASK
                         | GDK_LEAVE_NOTIFY_MASK
                         | GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK
			 );
}

void
ags_meter_connect(AgsMeter *meter)
{
  g_signal_connect((GObject *) meter, "destroy\0",
		   G_CALLBACK(ags_meter_destroy_callback), (gpointer) meter);

  g_signal_connect_after((GObject *) meter, "show\0",
			 G_CALLBACK(ags_meter_show_callback), (gpointer) meter);

  g_signal_connect((GObject *) meter, "expose_event\0",
  		   G_CALLBACK(ags_meter_expose_event), (gpointer) meter);

  g_signal_connect((GObject *) meter, "configure_event\0",
  		   G_CALLBACK(ags_meter_configure_event), (gpointer) meter);
}

void
ags_meter_destroy(GtkObject *object)
{
}

void
ags_meter_show(GtkWidget *widget)
{
}

void
ags_meter_paint(AgsMeter *meter)
{
  AgsEditor *editor;
  GtkWidget *widget;
  cairo_t *cr;
  guint bitmap;
  guint y[2];
  guint i, i_stop, j, j0;

  widget = (GtkWidget *) meter;
  editor = (AgsEditor *) gtk_widget_get_ancestor(widget, AGS_TYPE_EDITOR);

  bitmap = 0x52a52a; // description of the keyboard

  j = (guint) ceil(GTK_RANGE(editor->vscrollbar)->adjustment->value / (double) editor->control_height);
  j = j % 12;

  y[0] = (guint) round(GTK_RANGE(editor->vscrollbar)->adjustment->value) % editor->control_height;

  if(y[0] != 0){
    y[0] = editor->control_height - y[0];
  }

  y[1] = ((guint) widget->allocation.height - y[0]) % editor->control_height;
  i_stop = (widget->allocation.height - y[0] - y[1]) / editor->control_height;

  cr = gdk_cairo_create(widget->window);

  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_rectangle(cr, 0.0, 0.0, (double) widget->allocation.width, (double) widget->allocation.height);
  cairo_fill(cr);

  cairo_set_line_width(cr, 1.0);
  cairo_set_source_rgb(cr, 0.75, 0.75, 0.75);

  if(y[0] != 0){
    j0 = (j != 0) ? j -1: 11;

    if(((1 << j0) & bitmap) != 0){
      cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);

      cairo_rectangle(cr, 0.0, 0.0, 2.0 / 3.0 * (double) widget->allocation.width, (double) y[0]);
      cairo_fill(cr); 	

      cairo_set_source_rgb(cr, 0.75, 0.75, 0.75);

      if(y[0] > editor->control_height / 2){
	cairo_move_to(cr, 2.0 / 3.0 * (double) widget->allocation.width, (double) (y[0] - editor->control_height / 2));
	cairo_line_to(cr, (double) widget->allocation.width, (double) (y[0] - editor->control_height / 2));
	cairo_stroke(cr);
      }

      cairo_move_to(cr, (double) widget->allocation.width, 0.0);
      cairo_line_to(cr, (double) widget->allocation.width, (double) y[0]);
      cairo_stroke(cr);
    }else{
      if(((1 << (j0 + 1)) & bitmap) == 0){
	cairo_move_to(cr, 0.0, (double) y[0]);
	cairo_line_to(cr, (double) widget->allocation.width, (double) y[0]);
	cairo_stroke(cr);
      }

      cairo_move_to(cr, (double) widget->allocation.width, 0.0);
      cairo_line_to(cr, (double) widget->allocation.width, (double) y[0]);
      cairo_stroke(cr);
    }
  }

  for(i = 0; i < i_stop; i++){
    if(((1 << j) & bitmap) != 0){
      // draw semi tone key
      cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);

      cairo_rectangle(cr, 0.0, (double) (i * editor->control_height + y[0]), 2.0 / 3.0 * (double) widget->allocation.width, (double) editor->control_height);
      cairo_fill(cr); 	

      cairo_set_source_rgb(cr, 0.75, 0.75, 0.75);

      cairo_move_to(cr, 2.0 / 3.0 * (double) widget->allocation.width, (double) (i * editor->control_height + y[0] + editor->control_height / 2));
      cairo_line_to(cr, (double) widget->allocation.width, (double) (i * editor->control_height + y[0] + editor->control_height / 2));
      cairo_stroke(cr);

      cairo_move_to(cr, (double) widget->allocation.width, (double) (i * editor->control_height + y[0]));
      cairo_line_to(cr, (double) widget->allocation.width, (double) (i * editor->control_height + y[0] + editor->control_height));
      cairo_stroke(cr);
    }else{
      // no semi tone key
      if(((1 << (j + 1)) & bitmap) == 0){
	cairo_move_to(cr, 0.0, (double) (i * editor->control_height + y[0] + editor->control_height));
	cairo_line_to(cr, (double) widget->allocation.width, (double) (i * editor->control_height + y[0] + editor->control_height));
	cairo_stroke(cr);
      }

      cairo_move_to(cr, (double) widget->allocation.width, (double) (i * editor->control_height + y[0]));
      cairo_line_to(cr, (double) widget->allocation.width, (double) (i * editor->control_height + y[0] + editor->control_height));
      cairo_stroke(cr);
    }

    if(j == 11)
      j = 0;
    else
      j++;
  }

  if(y[1] != 0){
    j0 = j;

    if(((1 << j0) & bitmap) != 0){
      cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);

      cairo_rectangle(cr, 0.0, (double) (widget->allocation.height - y[1]), 2.0 / 3.0 * (double) widget->allocation.width, (double) y[1]);
      cairo_fill(cr); 	

      cairo_set_source_rgb(cr, 0.75, 0.75, 0.75);

      if(y[1] > editor->control_height / 2){
	cairo_move_to(cr, 2.0 / 3.0 * (double) widget->allocation.width, (double) (widget->allocation.height - y[1] + editor->control_height / 2));
	cairo_line_to(cr, (double) widget->allocation.width, (double) (widget->allocation.height - y[1] + editor->control_height / 2));
	cairo_stroke(cr);
      }

      cairo_move_to(cr, (double) widget->allocation.width, (double) (widget->allocation.height - y[1]));
      cairo_line_to(cr, (double) widget->allocation.width, (double) widget->allocation.height);
      cairo_stroke(cr);
    }else{
      cairo_move_to(cr, (double) widget->allocation.width, (double) (widget->allocation.height - y[1]));
      cairo_line_to(cr, (double) widget->allocation.width, (double) widget->allocation.height);
      cairo_stroke(cr);
    }
  }
}

AgsMeter*
ags_meter_new()
{
  AgsMeter *meter;

  meter = (AgsMeter *) g_object_new(AGS_TYPE_METER, NULL);

  return(meter);
}
