#ifndef __AGS_PROPERTY_LISTING_EDITOR_H__
#define __AGS_PROPERTY_LISTING_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_property_editor.h>

#define AGS_TYPE_PROPERTY_LISTING_EDITOR                (ags_property_listing_editor_get_type())
#define AGS_PROPERTY_LISTING_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PROPERTY_LISTING_EDITOR, AgsPropertyListingEditor))
#define AGS_PROPERTY_LISTING_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PROPERTY_LISTING_EDITOR, AgsPropertyListingEditorClass))
#define AGS_IS_PROPERTY_LISTING_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PROPERTY_LISTING_EDITOR))
#define AGS_IS_PROPERTY_LISTING_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PROPERTY_LISTING_EDITOR))
#define AGS_PROPERTY_LISTING_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PROPERTY_LISTING_EDITOR, AgsPropertyListingEditorClass))

typedef struct _AgsPropertyListingEditor AgsPropertyListingEditor;
typedef struct _AgsPropertyListingEditorClass AgsPropertyListingEditorClass;

struct _AgsPropertyListingEditor
{
  AgsPropertyEditor property_editor;

  GtkVBox *child;
};

struct _AgsPropertyListingEditorClass
{
  AgsPropertyEditorClass property_editor;
};

GType ags_property_listing_editor_get_type();

AgsPropertyListingEditor* ags_property_listing_editor_new();

#endif /*__AGS_PROPERTY_LISTING_EDITOR_H__*/
