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

#ifndef __AGS_EXPORT_WAV_H__
#define __AGS_EXPORT_WAV_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_EXPORT_WAV                (ags_export_wav_get_type())
#define AGS_EXPORT_WAV(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EXPORT_WAV, AgsExportWav}))
#define AGS_EXPORT_WAV_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EXPORT_WAV, AgsExportWavClass))
#define AGS_IS_EXPORT_WAV(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EXPORT_WAV))
#define AGS_IS_EXPORT_WAV_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EXPORT_WAV))
#define AGS_EXPORT_WAV_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EXPORT_WAV, AgsExportWavClass))

typedef struct _AgsExportWav AgsExportWav;
typedef struct _AgsExportWavClass AgsExportWavClass;

struct _AgsExportWav
{
  AgsTask task;
};

struct _AgsExportWavClass
{
  AgsTaskClass task;
};

GType ags_export_wav_get_type();

AgsExportWav* ags_export_wav_new();

#endif /*__AGS_EXPORT_WAV_H__*/
