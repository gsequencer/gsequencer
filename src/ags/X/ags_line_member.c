/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/X/ags_line_member.h>
#include <ags/X/ags_line_member_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/X/ags_pad.h>

#include <ags/audio/ags_channel.h>

void ags_line_member_class_init(AgsLineMemberClass *line_member);
void ags_line_member_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_line_member_init(AgsLineMember *line_member);
void ags_line_member_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_line_member_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_line_member_connect(AgsConnectable *connectable);
void ags_line_member_disconnect(AgsConnectable *connectable);
void ags_line_member_finalize(GtkObject *object);

enum{
  LAST_SIGNAL,
};

enum{
  PROP_0,
};

static gpointer ags_line_member_parent_class = NULL;
static guint line_member_signals[LAST_SIGNAL];

GType
ags_line_member_get_type(void)
{
  static GType ags_type_line_member = 0;

  if(!ags_type_line_member){
    static const GTypeInfo ags_line_member_info = {
      sizeof(AgsLineMemberClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_line_member_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLineMember),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_line_member_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_line_member_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_line_member = g_type_register_static(GTK_TYPE_BIN,
						  "AgsLineMember\0", &ags_line_member_info,
						  0);

    g_type_add_interface_static(ags_type_line_member,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_line_member);
}

void
ags_line_member_class_init(AgsLineMemberClass *line_member)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_line_member_parent_class = g_type_class_peek_parent(line_member);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(line_member);

  gobject->set_property = ags_line_member_set_property;
  gobject->get_property = ags_line_member_get_property;

  gobject->finalize = ags_line_member_finalize;
}

void
ags_line_member_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_line_member_connect;
  connectable->disconnect = ags_line_member_disconnect;
}

void
ags_line_member_init(AgsLineMember *line_member)
{
}

void
ags_line_member_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  /* empty */
}

void
ags_line_member_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  /* empty */
}

void
ags_line_member_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_line_member_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_line_member_finalize(GtkObject *object)
{
  /* empty */
}

AgsLineMember*
ags_line_member_new()
{
  AgsLineMember *line_member;

  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
					       NULL);

  return(line_member);
}
