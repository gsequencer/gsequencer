/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_LISTING_EDITOR_H__
#define __AGS_LISTING_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_property_listing_editor.h>

#include <ags/audio/ags_audio.h>

#define AGS_TYPE_LISTING_EDITOR                (ags_listing_editor_get_type())
#define AGS_LISTING_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LISTING_EDITOR, AgsListingEditor))
#define AGS_LISTING_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LISTING_EDITOR, AgsListingEditorClass))
#define AGS_IS_LISTING_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LISTING_EDITOR))
#define AGS_IS_LISTING_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LISTING_EDITOR))
#define AGS_LISTING_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_LISTING_EDITOR, AgsListingEditorClass))

typedef struct _AgsListingEditor AgsListingEditor;
typedef struct _AgsListingEditorClass AgsListingEditorClass;

struct _AgsListingEditor
{
  AgsPropertyListingEditor property_listing_editor;

  GType channel_type;

  GtkVBox *child;
};

struct _AgsListingEditorClass
{
  AgsPropertyListingEditorClass property_listing_editor;
};

GType ags_listing_editor_get_type();

void ags_listing_editor_resize(AgsListingEditor *listing_editor,
			       guint pads, guint pads_old);

AgsListingEditor* ags_listing_editor_new(GType channel_type);

#endif /*__AGS_LISTING_EDITOR_H__*/
