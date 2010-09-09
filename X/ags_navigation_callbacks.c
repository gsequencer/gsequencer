#include "ags_navigation_callbacks.h"

gboolean
ags_navigation_destroy_callback(GtkObject *object, AgsNavigation *navigation)
{
  ags_navigation_destroy(object);

  return(FALSE);
}

void
ags_navigation_show_callback(GtkWidget *widget, AgsNavigation *navigation)
{
  ags_navigation_show(widget);
}

void
ags_navigation_expander_callback(GtkWidget *widget, AgsNavigation *navigation)
{
  GtkArrow *arrow = (GtkArrow *) gtk_container_get_children((GtkContainer *) widget)->data;

  widget = (GtkWidget *) gtk_container_get_children((GtkContainer *) navigation)->next->data;

  if(arrow->arrow_type == GTK_ARROW_DOWN){
    gtk_widget_hide_all(widget);
    arrow->arrow_type = GTK_ARROW_RIGHT;
  }else{
    gtk_widget_show_all(widget);
    arrow->arrow_type = GTK_ARROW_DOWN;
  }
}

void
ags_navigation_bpm_callback(GtkWidget *widget, AgsNavigation *navigation)
{
  fprintf(stdout, "ags_navigation_bpm_callback(widget, navigation)\n\0");
}

void
ags_navigation_rewind_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}

void
ags_navigation_prev_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}

void
ags_navigation_play_callback(GtkWidget *widget, AgsNavigation *navigation)
{
  //  AGS_DEVOUT_GET_CLASS(navigation->devout)->run(navigation->devout);
}

void
ags_navigation_stop_callback(GtkWidget *widget, AgsNavigation *navigation)
{
  //  AGS_DEVOUT_GET_CLASS(navigation->devout)->stop(navigation->devout);
}

void
ags_navigation_next_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}

void
ags_navigation_forward_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}

void
ags_navigation_loop_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}

void
ags_navigation_position_sec_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}

void
ags_navigation_position_min_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}

void
ags_navigation_duration_sec_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}

void
ags_navigation_duration_min_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}

void
ags_navigation_loop_left_sec_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}

void
ags_navigation_loop_left_min_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}

void
ags_navigation_loop_right_sec_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}

void
ags_navigation_loop_right_min_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}

void
ags_navigation_raster_callback(GtkWidget *widget, AgsNavigation *navigation)
{
}
