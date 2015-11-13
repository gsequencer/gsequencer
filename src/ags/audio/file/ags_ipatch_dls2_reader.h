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

#ifndef __AGS_IPATCH_DLS2_READER_H__
#define __AGS_IPATCH_DLS2_READER_H__

#include <glib.h>
#include <glib-object.h>
#include <libinstpatch/libinstpatch.h>

#include <ags/audio/file/ags_ipatch.h>

#define AGS_TYPE_IPATCH_DLS2_READER                (ags_ipatch_dls2_reader_get_type())
#define AGS_IPATCH_DLS2_READER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_IPATCH_DLS2_READER, AgsIpatchDLS2Reader))
#define AGS_IPATCH_DLS2_READER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_IPATCH_DLS2_READER, AgsIpatchDLS2ReaderClass))
#define AGS_IS_IPATCH_DLS2_READER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_IPATCH_DLS2_READER))
#define AGS_IS_IPATCH_DLS2_READER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_IPATCH_DLS2_READER))
#define AGS_IPATCH_DLS2_READER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_IPATCH_DLS2_READER, AgsIpatchDLS2ReaderClass))

typedef struct _AgsIpatchDLS2Reader AgsIpatchDLS2Reader;
typedef struct _AgsIpatchDLS2ReaderClass AgsIpatchDLS2ReaderClass;

typedef enum{
  AGS_DLS2_FILENAME = 0,
  AGS_DLS2_IHDR = 1,
  AGS_DLS2_SHDR = 2,
}AgsDLS2Levels;

struct _AgsIpatchDLS2Reader
{
  GObject object;

  AgsIpatch *ipatch;

  gchar **selected;

  IpatchDLSReader *reader;

  IpatchDLS2 *dls2;

  int bank;
  int program;

  IpatchContainer *instrument;
  IpatchContainer *sample;

  int count;
};

struct _AgsIpatchDLS2ReaderClass
{
  GObjectClass object;
};

GType ags_ipatch_dls2_reader_get_type();

AgsIpatchDLS2Reader* ags_ipatch_dls2_reader_new();

#endif /*__AGS_IPATCH_DLS2_READER_H__*/
