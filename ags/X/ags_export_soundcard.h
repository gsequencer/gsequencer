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

#ifndef __AGS_EXPORT_SOUNDCARD_H__
#define __AGS_EXPORT_SOUNDCARD_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_EXPORT_SOUNDCARD                (ags_export_soundcard_get_type())
#define AGS_EXPORT_SOUNDCARD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EXPORT_SOUNDCARD, AgsExportSoundcard))
#define AGS_EXPORT_SOUNDCARD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EXPORT_SOUNDCARD, AgsExportSoundcardClass))
#define AGS_IS_EXPORT_SOUNDCARD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EXPORT_SOUNDCARD))
#define AGS_IS_EXPORT_SOUNDCARD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EXPORT_SOUNDCARD))
#define AGS_EXPORT_SOUNDCARD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EXPORT_SOUNDCARD, AgsExportSoundcardClass))

typedef struct _AgsExportSoundcard AgsExportSoundcard;
typedef struct _AgsExportSoundcardClass AgsExportSoundcardClass;

typedef enum{
  AGS_EXPORT_SOUNDCARD_CONNECTED     = 1,
}AgsExportSoundcardFlags;

struct _AgsExportSoundcard
{
  GtkVBox vbox;

  guint flags;

  GObject *soundcard;
  
  GtkEntry *filename;
  GtkButton *file_chooser_button;

  GtkComboBoxText *output_format;

  GtkButton *remove;
};

struct _AgsExportSoundcardClass
{
  GtkVBoxClass vbox;
};

GType ags_export_soundcard_get_type(void);

AgsExportSoundcard* ags_export_soundcard_new();

#endif /*__AGS_EXPORT_SOUNDCARD_H__*/
