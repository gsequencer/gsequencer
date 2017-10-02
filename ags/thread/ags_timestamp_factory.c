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

#include <ags/thread/ags_timestamp_factory.h>

#include <ags/object/ags_marshal.h>

void ags_timestamp_factory_class_init(AgsTimestampFactoryClass *timestamp_factory);
void ags_timestamp_factory_init (AgsTimestampFactory *timestamp_factory);
void ags_timestamp_factory_finalize(GObject *gobject);

AgsTimestamp* ags_timestamp_factory_real_create(AgsTimestampFactory *timestamp_factory,
						AgsTimestamp *predecor);

/**
 * SECTION:ags_timestamp_factory
 * @short_description: Factory pattern
 * @title: AgsTimestampFactory
 * @section_id:
 * @include: ags/thread/ags_timestamp_factory.h
 *
 * #AgsTimestampFactory pattern.
 */

enum{
  CREATE,
  LAST_SIGNAL,
};

static gpointer ags_timestamp_factory_parent_class = NULL;
static guint timestamp_factory_signals[LAST_SIGNAL];

static AgsTimestampFactory *ags_timestamp_factory = NULL;

GType
ags_timestamp_factory_get_type (void)
{
  static GType ags_type_timestamp_factory = 0;

  if(!ags_type_timestamp_factory){
    static const GTypeInfo ags_timestamp_factory_info = {
      sizeof (AgsTimestampFactoryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_timestamp_factory_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTimestampFactory),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_timestamp_factory_init,
    };

    ags_type_timestamp_factory = g_type_register_static(G_TYPE_OBJECT,
							"AgsTimestampFactory\0",
							&ags_timestamp_factory_info,
							0);
  }

  return (ags_type_timestamp_factory);
}

void
ags_timestamp_factory_class_init(AgsTimestampFactoryClass *timestamp_factory)
{
  GObjectClass *gobject;

  ags_timestamp_factory_parent_class = g_type_class_peek_parent(timestamp_factory);

  /* GObjectClass */
  gobject = (GObjectClass *) timestamp_factory;

  gobject->finalize = ags_timestamp_factory_finalize;

  /*  */
  timestamp_factory->create = ags_timestamp_factory_real_create;

  timestamp_factory_signals[CREATE] =
    g_signal_new("create\0",
		 G_TYPE_FROM_CLASS (timestamp_factory),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsTimestampFactoryClass, create),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__OBJECT_OBJECT,
		 G_TYPE_OBJECT, 2,
		 G_TYPE_OBJECT, G_TYPE_OBJECT);

}

void
ags_timestamp_factory_init(AgsTimestampFactory *timestamp_factory)
{
  timestamp_factory->flags = 0;
}

void
ags_timestamp_factory_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

AgsTimestamp*
ags_timestamp_factory_real_create(AgsTimestampFactory *timestamp_factory,
				  AgsTimestamp *predecor)
{
  AgsTimestamp *timestamp;

  timestamp = ags_timestamp_new();

  if(predecor != NULL){
    //TODO:JK: implement me
  }

  return(timestamp);
}

/**
 * ags_timestamp_factory_create:
 * @timestamp_factory: the #AgsTimestampFactory
 * @predecor: the preceeding #AgsTimestamp
 * 
 * 
 * Since: 1.0.0
 */
AgsTimestamp*
ags_timestamp_factory_create(AgsTimestampFactory *timestamp_factory,
			     AgsTimestamp *predecor)
{
  AgsTimestamp *timestamp;
  
  g_return_val_if_fail(AGS_IS_TIMESTAMP_FACTORY(timestamp_factory), NULL);

  g_object_ref(G_OBJECT(timestamp_factory));
  g_signal_emit(G_OBJECT(timestamp_factory),
		timestamp_factory_signals[CREATE], 0,
		predecor,
		&timestamp);
  g_object_unref(G_OBJECT(timestamp_factory));
  g_object_unref(timestamp_factory);

  return(timestamp);
}

AgsTimestampFactory*
ags_timestamp_factory_get_instance()
{
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  if(ags_timestamp_factory == NULL){
    ags_timestamp_factory = ags_timestamp_factory_new();
  }

  pthread_mutex_unlock(&(mutex));

  return(ags_timestamp_factory);
}

/**
 * ags_timestamp_factory_new:
 * 
 * Creates an #AgsTimestampFactory
 *
 * Returns: a new #AgsTimestampFactory
 *
 * Since: 1.0.0
 */
AgsTimestampFactory*
ags_timestamp_factory_new()
{
  AgsTimestampFactory *timestamp_factory;

  timestamp_factory = (AgsTimestampFactory *) g_object_new(AGS_TYPE_TIMESTAMP_FACTORY,
							   NULL);

  return(timestamp_factory);
}
