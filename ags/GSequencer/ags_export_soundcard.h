/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_EXPORT_SOUNDCARD                (ags_export_soundcard_get_type())
#define AGS_EXPORT_SOUNDCARD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EXPORT_SOUNDCARD, AgsExportSoundcard))
#define AGS_EXPORT_SOUNDCARD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EXPORT_SOUNDCARD, AgsExportSoundcardClass))
#define AGS_IS_EXPORT_SOUNDCARD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EXPORT_SOUNDCARD))
#define AGS_IS_EXPORT_SOUNDCARD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EXPORT_SOUNDCARD))
#define AGS_EXPORT_SOUNDCARD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EXPORT_SOUNDCARD, AgsExportSoundcardClass))

#define AGS_EXPORT_SOUNDCARD_FORMAT_WAV "wav"
#define AGS_EXPORT_SOUNDCARD_FORMAT_FLAC "flac"
#define AGS_EXPORT_SOUNDCARD_FORMAT_AIFF "aiff"
#define AGS_EXPORT_SOUNDCARD_FORMAT_OGG "ogg"
#define AGS_EXPORT_SOUNDCARD_FORMAT_MP3 "mp3"
#define AGS_EXPORT_SOUNDCARD_FORMAT_AAC "aac"
#define AGS_EXPORT_SOUNDCARD_FORMAT_MP4 "mp4"
#define AGS_EXPORT_SOUNDCARD_FORMAT_MKV "mkv"
#define AGS_EXPORT_SOUNDCARD_FORMAT_WEBM "webm"
#define AGS_EXPORT_SOUNDCARD_FORMAT_MPEG "mpg"
 
typedef struct _AgsExportSoundcard AgsExportSoundcard;
typedef struct _AgsExportSoundcardClass AgsExportSoundcardClass;

typedef enum{
  AGS_EXPORT_SOUNDCARD_CONNECTED     = 1,
}AgsExportSoundcardFlags;

struct _AgsExportSoundcard
{
  GtkBox box;

  guint flags;

  GObject *soundcard;

  GtkComboBoxText *backend;
  GtkComboBoxText *card;
  
  GtkEntry *filename;
  GtkButton *file_chooser_button;

  GtkComboBoxText *output_format;
};

struct _AgsExportSoundcardClass
{
  GtkBoxClass box;
};

GType ags_export_soundcard_get_type(void);

/* soundcard */
gboolean ags_export_soundcard_set_backend(AgsExportSoundcard *export_soundcard,
					  gchar *backend);

void ags_export_soundcard_refresh_card(AgsExportSoundcard *export_soundcard);
gboolean ags_export_soundcard_set_card(AgsExportSoundcard *export_soundcard,
				       gchar *card);

/* filename */
void ags_export_soundcard_set_filename(AgsExportSoundcard *export_soundcard,
				       gchar *filename);

/* format */
void ags_export_soundcard_set_format(AgsExportSoundcard *export_soundcard,
				     gchar *format);

/*  */
AgsExportSoundcard* ags_export_soundcard_new();

G_END_DECLS

#endif /*__AGS_EXPORT_SOUNDCARD_H__*/
