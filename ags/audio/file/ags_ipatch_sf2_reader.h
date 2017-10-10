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

#ifndef __AGS_IPATCH_SF2_READER_H__
#define __AGS_IPATCH_SF2_READER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/config.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <libinstpatch/libinstpatch.h>
#endif

#include <ags/audio/file/ags_ipatch.h>

#define AGS_TYPE_IPATCH_SF2_READER                (ags_ipatch_sf2_reader_get_type())
#define AGS_IPATCH_SF2_READER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_IPATCH_SF2_READER, AgsIpatchSF2Reader))
#define AGS_IPATCH_SF2_READER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_IPATCH_SF2_READER, AgsIpatchSF2ReaderClass))
#define AGS_IS_IPATCH_SF2_READER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_IPATCH_SF2_READER))
#define AGS_IS_IPATCH_SF2_READER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_IPATCH_SF2_READER))
#define AGS_IPATCH_SF2_READER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_IPATCH_SF2_READER, AgsIpatchSF2ReaderClass))

typedef struct _AgsIpatchSF2Reader AgsIpatchSF2Reader;
typedef struct _AgsIpatchSF2ReaderClass AgsIpatchSF2ReaderClass;

/**
 * AgsSF2Levels:
 * @AGS_SF2_FILENAME: filename
 * @AGS_SF2_PHDR: preset header
 * @AGS_SF2_IHDR: instrument header
 * @AGS_SF2_SHDR: sample header
 * 
 * Enum values to describe the different levels of a Soundfont2 file.
 */
typedef enum{
  AGS_SF2_FILENAME = 0,
  AGS_SF2_PHDR = 1,
  AGS_SF2_IHDR = 2,
  AGS_SF2_SHDR = 3,
}AgsSF2Levels;

struct _AgsIpatchSF2Reader
{
  GObject object;

  AgsIpatch *ipatch;

  gchar **selected;

#ifdef AGS_WITH_LIBINSTPATCH
  IpatchSF2Reader *reader;

  IpatchSF2 *sf2;
#else
  gpointer reader;

  gpointer sf2;
#endif
  
  int bank;
  int program;

#ifdef AGS_WITH_LIBINSTPATCH
  IpatchContainer *preset;
  IpatchContainer *instrument;
  IpatchContainer *sample;
#else
  gpointer preset;
  gpointer instrument;
  gpointer sample;
#endif
  
  int count;
};

struct _AgsIpatchSF2ReaderClass
{
  GObjectClass object;
};

GType ags_ipatch_sf2_reader_get_type();

AgsIpatchSF2Reader* ags_ipatch_sf2_reader_new();

#endif /*__AGS_IPATCH_SF2_READER_H__*/
