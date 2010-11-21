#ifndef __AGS_PROPERTY_COLLECTION_EDITOR_CALLBACKS__
#define __AGS_PROPERTY_COLLECTION_EDITOR_CALLBACKS__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_property_collection_editor.h>

void ags_property_collection_editor_add_collection_callback(GtkButton *button,
							    AgsPropertyCollectionEditor *property_collection_editor);

void ags_property_collection_editor_remove_collection_callback(GtkButton *button,
							       GtkTable *table);

#endif /*__AGS_PROPERTY_COLLECTION_EDITOR_CALLBACKS__*/
