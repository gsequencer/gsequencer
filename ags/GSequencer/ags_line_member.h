/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_LINE_MEMBER_H__
#define __AGS_LINE_MEMBER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_LINE_MEMBER                (ags_line_member_get_type())
#define AGS_LINE_MEMBER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LINE_MEMBER, AgsLineMember))
#define AGS_LINE_MEMBER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LINE_MEMBER, AgsLineMemberClass))
#define AGS_IS_LINE_MEMBER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LINE_MEMBER))
#define AGS_IS_LINE_MEMBER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LINE_MEMBER))
#define AGS_LINE_MEMBER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LINE_MEMBER, AgsLineMemberClass))

#define AGS_LINE_MEMBER_DEFAULT_VERSION "0.7.33"
#define AGS_LINE_MEMBER_DEFAULT_BUILD_ID "Wed Jun 15 13:48:15 UTC 2016"

typedef struct _AgsLineMember AgsLineMember;
typedef struct _AgsLineMemberClass AgsLineMemberClass;

typedef enum{
  AGS_LINE_MEMBER_CONNECTED             = 1,
  AGS_LINE_MEMBER_DEFAULT_TEMPLATE      = 1 <<  1,
  AGS_LINE_MEMBER_RESET_BY_ATOMIC       = 1 <<  2,
  AGS_LINE_MEMBER_RESET_BY_TASK         = 1 <<  3,
  AGS_LINE_MEMBER_APPLY_RECALL          = 1 <<  4,
  AGS_LINE_MEMBER_PLAY_CALLBACK_WRITE   = 1 <<  5,
  AGS_LINE_MEMBER_RECALL_CALLBACK_WRITE = 1 <<  6,
  AGS_LINE_MEMBER_CALLBACK_READ_BLOCK   = 1 <<  7,
  AGS_LINE_MEMBER_CALLBACK_WRITE_BLOCK  = 1 <<  8,
  AGS_LINE_MEMBER_APPLY_INITIAL         = 1 <<  9,
  AGS_LINE_MEMBER_BLOCK_CHAINED         = 1 << 10,
}Agslinememberflags;

typedef enum{
  AGS_LINE_MEMBER_PORT_BOOLEAN   = 1,
  AGS_LINE_MEMBER_PORT_INTEGER   = 1 <<  1,
}AgsLineMemberPortFlags;

struct _AgsLineMember
{
  GtkFrame frame;

  guint flags;
  guint port_flags;
  
  GType widget_type;
  guint widget_orientation;
  gchar *widget_label;

  AgsRecallContainer *play_container;
  AgsRecallContainer *recall_container;

  gchar *plugin_name;

  gchar *filename;
  gchar *effect;

  gchar *specifier;

  gchar *control_port;

  guint scale_precision;
  gdouble step_count;

  AgsConversion *conversion;
  
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

  GList* (*find_port)(AgsLineMember *line_member);
};

GType ags_line_member_get_type(void);

GtkWidget* ags_line_member_get_widget(AgsLineMember *line_member);
void ags_line_member_set_label(AgsLineMember *line_member,
			       gchar *label);

void ags_line_member_change_port(AgsLineMember *line_member,
				 gpointer port_data);

GList* ags_line_member_find_port(AgsLineMember *line_member);

void ags_line_member_chained_event(AgsLineMember *line_member);

AgsLineMember* ags_line_member_new();

G_END_DECLS

#endif /*__AGS_LINE_MEMBER_H__*/
