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

#ifndef __AGS_IPATCH_H__
#define __AGS_IPATCH_H__

#include <glib.h>
#include <glib-object.h>
#include <libinstpatch/libinstpatch.h>

struct _AgsIpatchSF2Reader
{
  GObject object;

  IpatchSF2Reader *reader;
  IpatchSF2Phdr *preset;
  IpatchSF2Ihdr *instrument;
  IpatchSF2Shdr *sample;
};

struct _AgsIpatchClass
{
  GObjectClass object;
};

GType ags_ipatch_get_type();

AgsIpatch* ags_ipatch_new();

#endif /*__AGS_IPATCH_H__*/
