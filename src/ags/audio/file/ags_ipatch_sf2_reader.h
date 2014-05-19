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

#ifndef __AGS_IPATCH_SF2_READER_H__
#define __AGS_IPATCH_SF2_READER_H__

#include <glib.h>
#include <glib-object.h>
#include <libinstpatch/libinstpatch.h>

#include <ags/audio/file/ags_ipatch.h>

#define AGS_TYPE_IPATCH_SF2_READER                (ags_ipatch_sf2_reader_get_type())
#define AGS_IPATCH_SF2_READER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_IPATCH_SF2_READER, AgsIpatchSF2Reader))
#define AGS_IPATCH_SF2_READER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_IPATCH_SF2_READER, AgsIpatchSF2ReaderClass))
#define AGS_IS_IPATCH_SF2_READER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_IPATCH_SF2_READER))
#define AGS_IS_IPATCH_SF2_READER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_IPATCH_SF2_READER))
#define AGS_IPATCH_SF2_READER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_IPATCH_SF2_READER, AgsIpatchSF2ReaderClass))

typedef struct _AgsIpatchSF2Reader AgsIpatchSF2Reader;
typedef struct _AgsIpatchSF2ReaderClass AgsIpatchSF2ReaderClass;

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

  IpatchSF2Reader *reader;

  IpatchSF2 *sf2;

  int bank;
  int program;

  IpatchSF2Preset *preset;
  IpatchSF2Inst *instrument;
  IpatchSF2Sample *sample;

  int count;
};

struct _AgsIpatchSF2ReaderClass
{
  GObjectClass object;
};

GType ags_ipatch_sf2_reader_get_type();

AgsIpatchSF2Reader* ags_ipatch_sf2_reader_new();

#endif /*__AGS_IPATCH_SF2_READER_H__*/
