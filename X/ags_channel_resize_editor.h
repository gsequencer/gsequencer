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

#ifndef __AGS_CHANNEL_RESIZE_EDITOR_H__
#define __AGS_CHANNEL_RESIZE_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_property_editor.h>

#define AGS_TYPE_CHANNEL_RESIZE_EDITOR                (ags_channel_resize_editor_get_type())
#define AGS_CHANNEL_RESIZE_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CHANNEL_RESIZE_EDITOR, AgsChannelResizeEditor))
#define AGS_CHANNEL_RESIZE_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CHANNEL_RESIZE_EDITOR, AgsChannelResizeEditorClass))
#define AGS_IS_CHANNEL_RESIZE_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CHANNEL_RESIZE_EDITOR))
#define AGS_IS_CHANNEL_RESIZE_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CHANNEL_RESIZE_EDITOR))
#define AGS_CHANNEL_RESIZE_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CHANNEL_RESIZE_EDITOR, AgsChannelResizeEditorClass))

typedef struct _AgsChannelResizeEditor AgsChannelResizeEditor;
typedef struct _AgsChannelResizeEditorClass AgsChannelResizeEditorClass;

struct _AgsChannelResizeEditor
{
  AgsPropertyEditor property_editor;

  GtkSpinButton *audio_channels;

  GtkSpinButton *input_pads;
  GtkSpinButton *output_pads;
};

struct _AgsChannelResizeEditorClass
{
  AgsPropertyEditorClass property_editor;
};

AgsChannelResizeEditor* ags_channel_resize_editor_new();

#endif /*__AGS_CHANNEL_RESIZE_EDITOR_H__*/
