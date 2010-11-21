#ifndef __AGS_MIXER_CALLBACKS_H__ 
#define __AGS_MIXER_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/machine/ags_mixer.h>

void ags_mixer_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMixer *mixer);
gboolean ags_mixer_destroy_callback(GtkObject *object, AgsMixer *mixer);

#endif /*__AGS_MIXER_CALLBACKS_H__ */
