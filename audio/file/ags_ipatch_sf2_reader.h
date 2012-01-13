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

typedef enum{
  AGS_SF2_FILENAME,
  AGS_SF2_PHDR,
  AGS_SF2_IHDR,
  AGS_SF2_SHDR,
}AgsSF2Levels;

struct _AgsIpatchSF2Reader
{
  GObject object;

  AgsIpatch *ipatch;

  guint nth_level;
  gchar *selected_sublevel_name;

  IpatchSF2Reader *reader;

  IpatchSF2 *sf2;
  IpatchSF2Phdr *preset;
  IpatchSF2Ihdr *instrument;
  IpatchSF2Shdr *sample;
};

struct _AgsIpatchSF2ReaderClass
{
  GObjectClass object;
};

GType ags_ipatch_sf2_reader_get_type();

AgsIpatchSF2Reader* ags_ipatch_sf2_reader_new();

#endif /*__AGS_IPATCH_SF2_READER_H__*/
