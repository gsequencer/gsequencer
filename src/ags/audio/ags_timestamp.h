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

#ifndef __AGS_TIMESTAMP_H__
#define __AGS_TIMESTAMP_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_TIMESTAMP                (ags_timestamp_get_type())
#define AGS_TIMESTAMP(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TIMESTAMP, AgsTimestamp))
#define AGS_TIMESTAMP_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TIMESTAMP, AgsTimestampClass))
#define AGS_IS_TIMESTAMP(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TIMESTAMP))
#define AGS_IS_TIMESTAMP_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TIMESTAMP))
#define AGS_TIMESTAMP_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_TIMESTAMP, AgsTimestampClass))

typedef struct _AgsTimestamp AgsTimestamp;
typedef struct _AgsTimestampClass AgsTimestampClass;

typedef enum{
  AGS_TIMESTAMP_UNIX      = 1,
  AGS_TIMESTAMP_OUTDATED  = 1 << 1,
}AgsTimestampFlags;

struct _AgsTimestamp
{
  GObject object;

  guint flags;
  
  union{
    struct _unix{
      time_t time_val;
    }unix_time;
  }timer;

  guint delay;
  guint attack;
};

struct _AgsTimestampClass
{
  GObjectClass object;
};

GType ags_timestamp_get_type(void);

/* */
AgsTimestamp* ags_timestamp_new();

#endif /*__AGS_TIMESTAMP_H__*/
