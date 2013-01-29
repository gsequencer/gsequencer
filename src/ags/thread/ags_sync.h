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

#ifndef __AGS_SYNC_H__
#define __AGS_SYNC_H__

typedef struct _AgsSync AgsSync;
typedef struct _AgsSyncClass AgsSyncClass;

typedef enum
{
  
}AgsSyncFlags;

struct _AgsSync
{
  GObject object;

  guint flags;
};


struct _AgsSyncClass
{
  GObjectClass object;
};

GType ags_sync_get_type();

AgsSync* ags_sync_new();

#endif /*__AGS_SYNC_H__*/
