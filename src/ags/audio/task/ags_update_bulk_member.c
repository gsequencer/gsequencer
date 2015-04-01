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

#include <ags/audio/task/ags_update_bulk_member.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

#include <ags/X/ags_line.h>
#include <ags/X/ags_effect_line.h>

void ags_update_bulk_member_class_init(AgsUpdateBulkMemberClass *update_bulk_member);
void ags_update_bulk_member_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_update_bulk_member_init(AgsUpdateBulkMember *update_bulk_member);
void ags_update_bulk_member_connect(AgsConnectable *connectable);
void ags_update_bulk_member_disconnect(AgsConnectable *connectable);
void ags_update_bulk_member_finalize(GObject *gobject);

void ags_update_bulk_member_launch(AgsTask *task);

/**
 * SECTION:ags_update_bulk_member
 * @short_description: add line_member object to line
 * @title: AgsUpdateBulkMember
 * @section_id:
 * @include: ags/audio/task/ags_update_bulk_member.h
 *
 * The #AgsUpdateBulkMember task addspacks #AgsLineMember to #AgsLine.
 */

static gpointer ags_update_bulk_member_parent_class = NULL;
static AgsConnectableInterface *ags_update_bulk_member_parent_connectable_interface;

GType
ags_update_bulk_member_get_type()
{
  static GType ags_type_update_bulk_member = 0;

  if(!ags_type_update_bulk_member){
    static const GTypeInfo ags_update_bulk_member_info = {
      sizeof (AgsUpdateBulkMemberClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_update_bulk_member_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsUpdateBulkMember),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_update_bulk_member_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_update_bulk_member_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_update_bulk_member = g_type_register_static(AGS_TYPE_TASK,
						 "AgsUpdateBulkMember\0",
						 &ags_update_bulk_member_info,
						 0);

    g_type_add_interface_static(ags_type_update_bulk_member,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_update_bulk_member);
}

void
ags_update_bulk_member_class_init(AgsUpdateBulkMemberClass *update_bulk_member)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_update_bulk_member_parent_class = g_type_class_peek_parent(update_bulk_member);

  /* gobject */
  gobject = (GObjectClass *) update_bulk_member;

  gobject->finalize = ags_update_bulk_member_finalize;

  /* task */
  task = (AgsTaskClass *) update_bulk_member;

  task->launch = ags_update_bulk_member_launch;
}

void
ags_update_bulk_member_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_update_bulk_member_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_update_bulk_member_connect;
  connectable->disconnect = ags_update_bulk_member_disconnect;
}

void
ags_update_bulk_member_init(AgsUpdateBulkMember *update_bulk_member)
{
  update_bulk_member->effect_bulk = NULL;
  update_bulk_member->bulk_member = NULL;
  update_bulk_member->new_size = 0;
  update_bulk_member->old_size = 0;
  update_bulk_member->pads = FALSE;
}

void
ags_update_bulk_member_connect(AgsConnectable *connectable)
{
  ags_update_bulk_member_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_update_bulk_member_disconnect(AgsConnectable *connectable)
{
  ags_update_bulk_member_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_update_bulk_member_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_update_bulk_member_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_update_bulk_member_launch(AgsTask *task)
{
  AgsUpdateBulkMember *update_bulk_member;

  update_bulk_member = AGS_UPDATE_BULK_MEMBER(task);

  //TODO:JK: implement me
}

AgsUpdateBulkMember*
ags_update_bulk_member_new(GtkWidget *effect_bulk,
			   AgsBulkMember *bulk_member,
			   guint new_size,
			   guint old_size,
			   gboolean pads)
{
  AgsUpdateBulkMember *update_bulk_member;

  update_bulk_member = (AgsUpdateBulkMember *) g_object_new(AGS_TYPE_UPDATE_BULK_MEMBER,
							    NULL);

  update_bulk_member->effect_bulk = effect_bulk;
  update_bulk_member->bulk_member = bulk_member;
  update_bulk_member->new_size = new_size;
  update_bulk_member->old_size = old_size;
  update_bulk_member->pads = pads;
  
  return(update_bulk_member);
}

