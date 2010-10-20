#include "ags_synth_callbacks.h"
#include "ags_oscillator.h"

#include "../../object/ags_connectable.h"

#include "../ags_window.h"

#include <math.h>

void
ags_synth_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsSynth *synth)
{
  AgsWindow *window;

  if(old_parent != NULL)
    return;

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);
  synth->machine.name = g_strdup_printf("Default %d\0", window->counter->synth);
  window->counter->synth++;
}

gboolean
ags_synth_destroy_callback(GtkObject *object, AgsSynth *synth)
{
  ags_synth_destroy(object);

  return(TRUE);
}


void
ags_synth_lower_callback(GtkSpinButton *spin_button, AgsSynth *synth)
{
}

void
ags_synth_auto_update_callback(GtkToggleButton *toggle, AgsSynth *synth)
{
  GList *list;

  list = gtk_container_get_children((GtkContainer *) synth->oscillator->menu);

  if(toggle->active)
    while(list != NULL){
      ags_oscillator_connect(AGS_CONNECTABLE(list->data));

      list = list->next;
    }
  else
    while(list != NULL){
      ags_oscillator_disconnect(AGS_CONNECTABLE(list->data));

      list = list->next;
    }
}

void
ags_synth_update_callback(GtkButton *button, AgsSynth *synth)
{
  ags_synth_update(synth);
}
