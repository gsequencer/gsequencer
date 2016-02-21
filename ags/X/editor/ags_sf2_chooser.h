/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_SF2_CHOOSER_H__
#define __AGS_SF2_CHOOSER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/file/ags_ipatch.h>

#define AGS_TYPE_SF2_CHOOSER                (ags_sf2_chooser_get_type ())
#define AGS_SF2_CHOOSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SF2_CHOOSER, AgsSF2Chooser))
#define AGS_SF2_CHOOSER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SF2_CHOOSER, AgsSF2ChooserClass))
#define AGS_IS_SF2_CHOOSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SF2_CHOOSER))
#define AGS_IS_SF2_CHOOSER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SF2_CHOOSER))
#define AGS_SF2_CHOOSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SF2_CHOOSER, AgsSF2ChooserClass))

typedef struct _AgsSF2Chooser AgsSF2Chooser;
typedef struct _AgsSF2ChooserClass AgsSF2ChooserClass;

struct _AgsSF2Chooser
{
  GtkVBox vbox;

  AgsIpatch *ipatch;

  GtkComboBoxText *preset;
  GtkComboBoxText *instrument;
  GtkComboBoxText *sample;
};

struct _AgsSF2ChooserClass
{
  GtkVBoxClass vbox;
};

GType ags_sf2_chooser_get_type();

void ags_sf2_chooser_open(AgsSF2Chooser *sf2_chooser, gchar *filename);

gboolean ags_sf2_chooser_completed(AgsSF2Chooser *sf2_chooser);

void ags_sf2_chooser_remove_all_from_combo(GtkComboBoxText *combo);

AgsSF2Chooser* ags_sf2_chooser_new();

#endif /*__AGS_SF2_CHOOSER_H__*/
