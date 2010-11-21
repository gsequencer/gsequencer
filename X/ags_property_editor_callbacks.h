#ifndef __AGS_PROPERTY_EDITOR_CALLBACKS_H__
#define __AGS_PROPERTY_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_property_editor.h>

void ags_property_editor_enable_callback(GtkToggleButton *toggle,
					 AgsPropertyEditor *property_editor);

#endif /*__AGS_PROPERTY_EDITOR_CALLBACKS_H__*/
