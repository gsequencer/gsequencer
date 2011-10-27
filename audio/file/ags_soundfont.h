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

#ifndef __AGS_SOUNDFONT_H__
#define __AGS_SOUNDFONT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/file/ags_audio_file.h>

#include <stdio.h>

#define AGS_TYPE_SOUNDFONT                (ags_soundfont_get_type())
#define AGS_SOUNDFONT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOUNDFONT, AgsSoundfont))
#define AGS_SOUNDFONT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SOUNDFONT, AgsSoundfontClass))
#define AGS_IS_SOUNDFONT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SOUNDFONT))
#define AGS_IS_SOUNDFONT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SOUNDFONT))
#define AGS_SOUNDFONT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SOUNDFONT, AgsSoundfontClass))

typedef struct _AgsSoundfont AgsSoundfont;
typedef struct _AgsSoundfontClass AgsSoundfontClass;
typedef struct _AgsSoundfontBank AgsSoundfontBank;
typedef struct _AgsSoundfontSample AgsSoundfontSample;

typedef enum{
  AGS_SOUNDFONT_SAMPLE_MONO   = 1,
  AGS_SOUNDFONT_SAMPLE_LEFT   = 1 <<  1,
  AGS_SOUNDONFT_SAMPLE_RIGHT  = 1 <<  2,
}AgsSoundfontSampleFlags;

struct _AgsSoundfont
{
  GObject object;

  FILE *file;

  guint selected_level;

  AgsSoundfontBank **bank;

  AgsSoundfontBank *current_bank;
  guint i_sample;

  AgsSoundfontSample *current_sample;

  AgsAudioFile *audio_file;
};

struct _AgsSoundfontClass
{
  GObjectClass object;
};

struct _AgsSoundfontBank
{
  gchar *bank_name;

  size_t offset;

  AgsSoundfontSample **sample;
};

struct _AgsSoundfontSample
{
  gchar *sample_name;

  size_t offset;

  guint flags;

  guint sample_rate;
  guint frames;

  guint loop_start;
  guint loop_end;  
};

GType ags_soundfont_get_type();

AgsSoundfont* ags_soundfont_new();

#endif /*__AGS_SOUNDFONT_H__*/
