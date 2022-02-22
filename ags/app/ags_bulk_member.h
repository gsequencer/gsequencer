/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_BULK_MEMBER_H__
#define __AGS_BULK_MEMBER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_BULK_MEMBER                (ags_bulk_member_get_type())
#define AGS_BULK_MEMBER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_BULK_MEMBER, AgsBulkMember))
#define AGS_BULK_MEMBER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_BULK_MEMBER, AgsBulkMemberClass))
#define AGS_IS_BULK_MEMBER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_BULK_MEMBER))
#define AGS_IS_BULK_MEMBER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_BULK_MEMBER))
#define AGS_BULK_MEMBER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_BULK_MEMBER, AgsBulkMemberClass))

#define AGS_BULK_PORT(ptr) ((AgsBulkPort *)(ptr))

#define AGS_BULK_MEMBER_DEFAULT_VERSION "0.7.21"
#define AGS_BULK_MEMBER_DEFAULT_BUILD_ID "CEST 01-03-2016 00:23"

typedef struct _AgsBulkMember AgsBulkMember;
typedef struct _AgsBulkMemberClass AgsBulkMemberClass;
typedef struct _AgsBulkPort AgsBulkPort;

typedef enum{
  AGS_BULK_MEMBER_CONNECTED             = 1,
  AGS_BULK_MEMBER_DEFAULT_TEMPLATE      = 1 <<  1,
  AGS_BULK_MEMBER_RESET_BY_ATOMIC       = 1 <<  2,
  AGS_BULK_MEMBER_RESET_BY_TASK         = 1 <<  3,
  AGS_BULK_MEMBER_APPLY_RECALL          = 1 <<  4,
  AGS_BULK_MEMBER_NO_UPDATE             = 1 <<  5,
  AGS_BULK_MEMBER_APPLY_INITIAL         = 1 <<  6,
}AgsBulkMemberFlags;

typedef enum{
  AGS_BULK_MEMBER_PORT_BOOLEAN   = 1,
  AGS_BULK_MEMBER_PORT_INTEGER   = 1 <<  1,
}AgsBulkMemberPortFlags;

struct _AgsBulkMember
{
  GtkFrame frame;

  guint flags;
  guint port_flags;
  
  gchar *version;
  gchar *build_id;

  GList *handler;
  
  GType widget_type;
  guint widget_orientation;
  gchar *widget_label;

  AgsRecallContainer *play_container;
  AgsRecallContainer *recall_container;

  gchar *plugin_name;
  
  gchar *filename;
  gchar *effect;
  
  gchar *specifier;

  guint port_index;
  gchar *control_port;

  guint scale_precision;
  gdouble step_count;

  AgsConversion *conversion;
  
  GList *bulk_port;
  GList *recall_bulk_port;
  
  GType task_type;
};

struct _AgsBulkMemberClass
{
  GtkFrameClass frame;

  void (*change_port)(AgsBulkMember *bulk_member,
		      gpointer port_data);

  GList* (*find_port)(AgsBulkMember *bulk_member);
};

struct _AgsBulkPort
{
  AgsPort *port;

  guint pad;
  guint audio_channel;
  
  gpointer port_data;
  gboolean active;
};

GType ags_bulk_member_get_type(void);

AgsBulkPort* ags_bulk_port_alloc(AgsPort *port, guint pad, guint audio_channel);
void ags_bulk_port_free(AgsBulkPort *bulk_port);

GList* ags_bulk_port_find(GList *list, AgsPort *port);

GtkWidget* ags_bulk_member_get_widget(AgsBulkMember *bulk_member);
void ags_bulk_member_set_label(AgsBulkMember *bulk_member,
			       gchar *label);

void ags_bulk_member_change_port(AgsBulkMember *bulk_member,
				 gpointer port_data);

GList* ags_bulk_member_find_port(AgsBulkMember *bulk_member);

GList* ags_bulk_member_find_effect_and_specifier(GList *bulk_member,
						 gchar *filename, gchar *effect,
						 gchar *specifier);

void ags_bulk_member_remap_bulk_port(AgsBulkMember *bulk_member);

AgsBulkMember* ags_bulk_member_new();

G_END_DECLS

#endif /*__AGS_BULK_MEMBER_H__*/
