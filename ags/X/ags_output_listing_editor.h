/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_OUTPUT_LISTING_EDITOR_H__
#define __AGS_OUTPUT_LISTING_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_property_listing_editor.h>

#include <ags/audio/ags_audio.h>

#define AGS_TYPE_OUTPUT_LISTING_EDITOR                (ags_output_listing_editor_get_type())
#define AGS_OUTPUT_LISTING_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OUTPUT_LISTING_EDITOR, AgsOutputListingEditor))
#define AGS_OUTPUT_LISTING_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OUTPUT_LISTING_EDITOR, AgsOutputListingEditorClass))
#define AGS_IS_OUTPUT_LISTING_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OUTPUT_LISTING_EDITOR))
#define AGS_IS_OUTPUT_LISTING_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OUTPUT_LISTING_EDITOR))
#define AGS_OUTPUT_LISTING_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_OUTPUT_LISTING_EDITOR, AgsOutputListingEditorClass))

typedef struct _AgsOutputListingEditor AgsOutputListingEditor;
typedef struct _AgsOutputListingEditorClass AgsOutputListingEditorClass;

struct _AgsOutputListingEditor
{
  AgsPropertyListingEditor property_listing_editor;

  GType channel_type;

  GtkVBox *child;

  gulong set_pads_handler;
};

struct _AgsOutputListingEditorClass
{
  AgsPropertyListingEditorClass property_listing_editor;
};

GType ags_output_listing_editor_get_type();

void ags_output_listing_editor_add_children(AgsOutputListingEditor *output_listing_editor,
					    AgsAudio *audio, guint nth_channel,
					    gboolean connect);
AgsOutputListingEditor* ags_output_listing_editor_new(GType channel_type);

#endif /*__AGS_OUTPUT_LISTING_EDITOR_H__*/
