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

#ifndef __AGS_LINE_MEMBER_H__
#define __AGS_LINE_MEMBER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_task.h>

#define AGS_TYPE_LINE_MEMBER                (ags_line_member_get_type())
#define AGS_LINE_MEMBER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LINE_MEMBER, AgsLineMember))
#define AGS_LINE_MEMBER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LINE_MEMBER, AgsLineMemberClass))
#define AGS_IS_LINE_MEMBER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LINE_MEMBER))
#define AGS_IS_LINE_MEMBER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LINE_MEMBER))
#define AGS_LINE_MEMBER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LINE_MEMBER, AgsLineMemberClass))

typedef struct _AgsLineMember AgsLineMember;
typedef struct _AgsLineMemberClass AgsLineMemberClass;

typedef enum{
  AGS_LINE_MEMBER_DEFAULT_TEMPLATE      = 1,
  AGS_LINE_MEMBER_RESET_BY_ATOMIC       = 1 << 1,
  AGS_LINE_MEMBER_RESET_BY_TASK         = 1 << 2,
  AGS_LINE_MEMBER_APPLY_RECALL          = 1 << 3,
  AGS_LINE_MEMBER_PLAY_CALLBACK_WRITE   = 1 << 4,
  AGS_LINE_MEMBER_RECALL_CALLBACK_WRITE = 1 << 5,
  AGS_LINE_MEMBER_CALLBACK_READ_BLOCK   = 1 << 6,
  AGS_LINE_MEMBER_CALLBACK_WRITE_BLOCK  = 1 << 7,
}AgsLineMemberFlags;

struct _AgsLineMember
{
  GtkFrame frame;

  guint flags;

  GType widget_type;
  gchar *widget_label;

  gchar *plugin_name;
  gchar *specifier;

  gchar *control_port;
  
  AgsPort *port;
  gpointer port_data;
  gboolean active;

  AgsPort *recall_port;
  gpointer recall_port_data;
  gboolean recall_active;

  GType task_type;
};

struct _AgsLineMemberClass
{
  GtkFrameClass frame;

  void (*change_port)(AgsLineMember *line_member,
		      gpointer port_data);
};

GType ags_line_member_get_type(void);

GtkWidget* ags_line_member_get_widget(AgsLineMember *line_member);
void ags_line_member_set_label(AgsLineMember *line_member,
			       gchar *label);

void ags_line_member_change_port(AgsLineMember *line_member,
				 gpointer port_data);

void ags_line_member_find_port(AgsLineMember *line_member);

AgsLineMember* ags_line_member_new();

#endif /*__AGS_LINE_MEMBER_H__*/
