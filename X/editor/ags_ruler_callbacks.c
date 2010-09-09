#include "ags_ruler_callbacks.h"

gboolean
ags_ruler_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsRuler *ruler)
{
  ags_ruler_paint(ruler);

  return(TRUE);
}

gboolean
ags_ruler_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsRuler *ruler)
{
  ags_ruler_paint(ruler);

  return(FALSE);
}
