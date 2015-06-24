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

#include <ags/audio/task/ags_add_line_member.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

void ags_add_line_member_class_init(AgsAddLineMemberClass *add_line_member);
void ags_add_line_member_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_line_member_init(AgsAddLineMember *add_line_member);
void ags_add_line_member_connect(AgsConnectable *connectable);
void ags_add_line_member_disconnect(AgsConnectable *connectable);
void ags_add_line_member_finalize(GObject *gobject);

void ags_add_line_member_launch(AgsTask *task);

/**
 * SECTION:ags_add_line_member
 * @short_description: add line_member object to line
 * @title: AgsAddLineMember
 * @section_id:
 * @include: ags/audio/task/ags_add_line_member.h
 *
 * The #AgsAddLineMember task addspacks #AgsLineMember to #AgsLine.
 */

static gpointer ags_add_line_member_parent_class = NULL;
static AgsConnectableInterface *ags_add_line_member_parent_connectable_interface;

GType
ags_add_line_member_get_type()
{
  static GType ags_type_add_line_member = 0;

  if(!ags_type_add_line_member){
    static const GTypeInfo ags_add_line_member_info = {
      sizeof (AgsAddLineMemberClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_line_member_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddLineMember),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_line_member_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_line_member_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_line_member = g_type_register_static(AGS_TYPE_TASK,
						 "AgsAddLineMember\0",
						 &ags_add_line_member_info,
						 0);

    g_type_add_interface_static(ags_type_add_line_member,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_line_member);
}

void
ags_add_line_member_class_init(AgsAddLineMemberClass *add_line_member)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_add_line_member_parent_class = g_type_class_peek_parent(add_line_member);

  /* gobject */
  gobject = (GObjectClass *) add_line_member;

  gobject->finalize = ags_add_line_member_finalize;

  /* task */
  task = (AgsTaskClass *) add_line_member;

  task->launch = ags_add_line_member_launch;
}

void
ags_add_line_member_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_line_member_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_line_member_connect;
  connectable->disconnect = ags_add_line_member_disconnect;
}

void
ags_add_line_member_init(AgsAddLineMember *add_line_member)
{
  add_line_member->line = NULL;
  add_line_member->line_member = NULL;
  add_line_member->x = 0;
  add_line_member->y = 0;
  add_line_member->width = 0;
  add_line_member->height = 0;
}

void
ags_add_line_member_connect(AgsConnectable *connectable)
{
  ags_add_line_member_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_line_member_disconnect(AgsConnectable *connectable)
{
  ags_add_line_member_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_line_member_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_add_line_member_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_add_line_member_launch(AgsTask *task)
{
  AgsAddLineMember *add_line_member;

  add_line_member = AGS_ADD_LINE_MEMBER(task);

  ags_expander_add(add_line_member->line->expander,
		   (GtkWidget *) add_line_member->line_member,
		   add_line_member->x, add_line_member->y,
		   add_line_member->width, add_line_member->height);

  ags_line_member_find_port(add_line_member->line_member);

  //  gtk_widget_set_child_visible(GTK_BIN(add_line_member->line->expander)->child,
  //			       TRUE);
}

/**
 * ags_add_line_member_new:
 * @line: the #AgsLine
 * @line_member: the #AgsLineMember to add
 * @x: pack start x
 * @y: pack start y
 * @width: pack width
 * @height: pack height
 *
 * Creates an #AgsAddLineMember.
 *
 * Returns: an new #AgsAddLineMember.
 *
 * Since: 0.4
 */
AgsAddLineMember*
ags_add_line_member_new(AgsLine *line,
			AgsLineMember *line_member,
			guint x, guint y,
			guint width, guint height)
{
  AgsAddLineMember *add_line_member;

  add_line_member = (AgsAddLineMember *) g_object_new(AGS_TYPE_ADD_LINE_MEMBER,
						      NULL);

  add_line_member->line = line;
  add_line_member->line_member = line_member;
  add_line_member->x = x;
  add_line_member->y = y;
  add_line_member->width = width;
  add_line_member->height = height;

  return(add_line_member);
}
