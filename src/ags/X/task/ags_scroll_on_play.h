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

#ifndef __AGS_SCROLL_ON_PLAY_H__
#define __AGS_SCROLL_ON_PLAY_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#define AGS_TYPE_SCROLL_ON_PLAY                (ags_scroll_on_play_get_type())
#define AGS_SCROLL_ON_PLAY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCROLL_ON_PLAY, AgsScrollOnPlay))
#define AGS_SCROLL_ON_PLAY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SCROLL_ON_PLAY, AgsScrollOnPlayClass))
#define AGS_IS_SCROLL_ON_PLAY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SCROLL_ON_PLAY))
#define AGS_IS_SCROLL_ON_PLAY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SCROLL_ON_PLAY))
#define AGS_SCROLL_ON_PLAY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SCROLL_ON_PLAY, AgsScrollOnPlayClass))

typedef struct _AgsScrollOnPlay AgsScrollOnPlay;
typedef struct _AgsScrollOnPlayClass AgsScrollOnPlayClass;

struct _AgsScrollOnPlay
{
  AgsTask task;

  GtkWidget *editor;
};

struct _AgsScrollOnPlayClass
{
  AgsTaskClass task;
};

GType ags_scroll_on_play_get_type();

AgsScrollOnPlay* ags_scroll_on_play_new(GtkWidget *editor);

#endif /*__AGS_SCROLL_ON_PLAY_H__*/
