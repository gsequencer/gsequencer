/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_MACHINE_EDITOR_LISTING_H__
#define __AGS_MACHINE_EDITOR_LISTING_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_machine_editor_pad.h>

G_BEGIN_DECLS

#define AGS_TYPE_MACHINE_EDITOR_LISTING                (ags_machine_editor_listing_get_type())
#define AGS_MACHINE_EDITOR_LISTING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_EDITOR_LISTING, AgsMachineEditorListing))
#define AGS_MACHINE_EDITOR_LISTING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_EDITOR_LISTING, AgsMachineEditorListingClass))
#define AGS_IS_MACHINE_EDITOR_LISTING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_EDITOR_LISTING))
#define AGS_IS_MACHINE_EDITOR_LISTING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_EDITOR_LISTING))
#define AGS_MACHINE_EDITOR_LISTING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE_EDITOR_LISTING, AgsMachineEditorListingClass))

typedef struct _AgsMachineEditorListing AgsMachineEditorListing;
typedef struct _AgsMachineEditorListingClass AgsMachineEditorListingClass;

struct _AgsMachineEditorListing
{
  GtkBox box;

  GType channel_type;
  
  GtkCheckButton *enabled;

  GList *pad;

  GtkBox *pad_box;
};

struct _AgsMachineEditorListingClass
{
  GtkBoxClass box;
};

GType ags_machine_editor_listing_get_type(void);

AgsMachineEditorListing* ags_machine_editor_listing_new();

G_END_DECLS

#endif /*__AGS_MACHINE_EDITOR_LISTING_H__*/
