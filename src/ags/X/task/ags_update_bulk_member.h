/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#ifndef __AGS_UPDATE_BULK_MEMBER_H__
#define __AGS_UPDATE_BULK_MEMBER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/X/ags_bulk_member.h>

#define AGS_TYPE_UPDATE_BULK_MEMBER                (ags_update_bulk_member_get_type())
#define AGS_UPDATE_BULK_MEMBER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_UPDATE_BULK_MEMBER, AgsUpdateBulkMember))
#define AGS_UPDATE_BULK_MEMBER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_UPDATE_BULK_MEMBER, AgsUpdateBulkMemberClass))
#define AGS_IS_UPDATE_BULK_MEMBER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_UPDATE_BULK_MEMBER))
#define AGS_IS_UPDATE_BULK_MEMBER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_UPDATE_BULK_MEMBER))
#define AGS_UPDATE_BULK_MEMBER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_UPDATE_BULK_MEMBER, AgsUpdateBulkMemberClass))

typedef struct _AgsUpdateBulkMember AgsUpdateBulkMember;
typedef struct _AgsUpdateBulkMemberClass AgsUpdateBulkMemberClass;

struct _AgsUpdateBulkMember
{
  AgsTask task;

  GtkWidget *effect_bulk;
  AgsBulkMember *bulk_member;

  guint new_size;
  guint old_size;

  gboolean pads;
};

struct _AgsUpdateBulkMemberClass
{
  AgsTaskClass task;
};

GType ags_update_bulk_member_get_type();

AgsUpdateBulkMember* ags_update_bulk_member_new(GtkWidget *effect_bulk,
						AgsBulkMember *bulk_member,
						guint new_size,
						guint old_size,
						gboolean pads);

#endif /*__AGS_UPDATE_BULK_MEMBER_H__*/
