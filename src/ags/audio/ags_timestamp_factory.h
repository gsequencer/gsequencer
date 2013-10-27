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

#ifndef __AGS_TIMESTAMP_FACTORY_H__
#define __AGS_TIMESTAMP_FACTORY_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_timestamp.h>

#define AGS_TYPE_TIMESTAMP_FACTORY                (ags_timestamp_factory_get_type())
#define AGS_TIMESTAMP_FACTORY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TIMESTAMP_FACTORY, AgsTimestampFactory))
#define AGS_TIMESTAMP_FACTORY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TIMESTAMP_FACTORY, AgsTimestampFactoryClass))
#define AGS_IS_TIMESTAMP_FACTORY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TIMESTAMP_FACTORY))
#define AGS_IS_TIMESTAMP_FACTORY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TIMESTAMP_FACTORY))
#define AGS_TIMESTAMP_FACTORY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_TIMESTAMP_FACTORY, AgsTimestampFactoryClass))

typedef struct _AgsTimestampFactory AgsTimestampFactory;
typedef struct _AgsTimestampFactoryClass AgsTimestampFactoryClass;

typedef enum{
  AGS_TIMESTAMP_FACTORY_UNIX      = 1,
}AgsTimestampFactoryFlags;

struct _AgsTimestampFactory
{
  GObject object;

  guint flags;

  GList *timestamp;
};

struct _AgsTimestampFactoryClass
{
  GObjectClass object;
  
  AgsTimestamp* (*create)(AgsTimestampFactory *timestamp_factory,
			  AgsTimestamp *predecor);
};

GType ags_timestamp_factory_get_type(void);

AgsTimestamp* ags_timestamp_factory_create(AgsTimestampFactory *timestamp_factory,
					   AgsTimestamp *predecor);

/* */
AgsTimestampFactory* ags_timestamp_factory_get_instance();
AgsTimestampFactory* ags_timestamp_factory_new();

#endif /*__AGS_TIMESTAMP_FACTORY_H__*/
