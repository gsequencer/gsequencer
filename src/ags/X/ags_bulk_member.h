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

#ifndef __AGS_BULK_MEMBER_H__
#define __AGS_BULK_MEMBER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_task.h>

#define AGS_TYPE_BULK_MEMBER                (ags_bulk_member_get_type())
#define AGS_BULK_MEMBER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_BULK_MEMBER, AgsBulkMember))
#define AGS_BULK_MEMBER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_BULK_MEMBER, AgsBulkMemberClass))
#define AGS_IS_BULK_MEMBER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_BULK_MEMBER))
#define AGS_IS_BULK_MEMBER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_BULK_MEMBER))
#define AGS_BULK_MEMBER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_BULK_MEMBER, AgsBulkMemberClass))

#define AGS_BULK_PORT(ptr) ((AgsBulkPort *)(ptr))

typedef struct _AgsBulkMember AgsBulkMember;
typedef struct _AgsBulkMemberClass AgsBulkMemberClass;
typedef struct _AgsBulkPort AgsBulkPort;

typedef enum{
  AGS_BULK_MEMBER_DEFAULT_TEMPLATE      = 1,
  AGS_BULK_MEMBER_RESET_BY_ATOMIC       = 1 << 1,
  AGS_BULK_MEMBER_RESET_BY_TASK         = 1 << 2,
  AGS_BULK_MEMBER_APPLY_RECALL          = 1 << 3,
}AgsBulkMemberFlags;

struct _AgsBulkMember
{
  GtkFrame frame;

  guint flags;

  GType widget_type;
  gchar *widget_label;

  gchar *filename;
  gchar *effect;
  
  gchar *plugin_name;
  gchar *specifier;

  gchar *control_port;

  GList *bulk_port;
  
  GType task_type;
};

struct _AgsBulkMemberClass
{
  GtkFrameClass frame;

  void (*change_port)(AgsBulkMember *bulk_member,
		      gpointer port_data);
};

struct _AgsBulkPort
{
  AgsPort *port;
  gpointer port_data;
  gboolean active;

  AgsPort *recall_port;
  gpointer recall_port_data;
  gboolean recall_active;
};

GType ags_bulk_member_get_type(void);

AgsBulkPort* ags_bulk_port_alloc(AgsPort *port,
				 AgsPort *recall_port);

GtkWidget* ags_bulk_member_get_widget(AgsBulkMember *bulk_member);
void ags_bulk_member_set_label(AgsBulkMember *bulk_member,
			       gchar *label);

void ags_bulk_member_change_port(AgsBulkMember *bulk_member,
				 gpointer port_data);

void ags_bulk_member_find_port(AgsBulkMember *bulk_member);

AgsBulkMember* ags_bulk_member_new();

#endif /*__AGS_BULK_MEMBER_H__*/
