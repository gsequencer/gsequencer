/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_ICON_LINK_H__
#define __AGS_ICON_LINK_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_ICON_LINK                (ags_icon_link_get_type())
#define AGS_ICON_LINK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ICON_LINK, AgsIconLink))
#define AGS_ICON_LINK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ICON_LINK, AgsIconLinkClass))
#define AGS_IS_ICON_LINK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ICON_LINK))
#define AGS_IS_ICON_LINK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ICON_LINK))
#define AGS_ICON_LINK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ICON_LINK, AgsIconLinkClass))

#define AGS_ICON_LINK_DEFAULT_SEGMENT_WIDTH (7)
#define AGS_ICON_LINK_DEFAULT_SEGMENT_HEIGHT (7)
#define AGS_ICON_LINK_DEFAULT_SEGMENT_PADDING (3)
#define AGS_ICON_LINK_DEFAULT_SEGMENT_COUNT (10)

typedef struct _AgsIconLink AgsIconLink;
typedef struct _AgsIconLinkClass AgsIconLinkClass;

typedef enum{
  AGS_ICON_LINK_HIGHLIGHT            = 1,
}AgsIconLinkFlags;

struct _AgsIconLink
{
  GtkBox box;

  guint flags;
  
  GtkImage *icon;

  GtkLabel *link;

  gchar *action;
};

struct _AgsIconLinkClass
{
  GtkBoxClass box;

  void (*clicked)(AgsIconLink *icon_link);
};

GType ags_icon_link_get_type(void);

/* flags */
gboolean ags_icon_link_test_flags(AgsIconLink *icon_link,
				  guint flags);
void ags_icon_link_set_flags(AgsIconLink *icon_link,
			     guint flags);
void ags_icon_link_unset_flags(AgsIconLink *icon_link,
			       guint flags);

/* getter/setter */
gchar* ags_icon_link_get_icon_name(AgsIconLink *icon_link);
void ags_icon_link_set_icon_name(AgsIconLink *icon_link,
				 gchar *icon_name);

gchar* ags_icon_link_get_action(AgsIconLink *icon_link);
void ags_icon_link_set_action(AgsIconLink *icon_link,
			      gchar *action);

gchar* ags_icon_link_get_link_text(AgsIconLink *icon_link);
void ags_icon_link_set_link_text(AgsIconLink *icon_link,
				 gchar *link_text);

/* events */
void ags_icon_link_clicked(AgsIconLink *icon_link);

/* instantiate */
AgsIconLink* ags_icon_link_new(gchar *icon_name,
			       gchar *action,
			       gchar *link_text);

G_END_DECLS

#endif /*__AGS_ICON_LINK_H__*/
