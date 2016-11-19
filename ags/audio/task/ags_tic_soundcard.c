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

#include <ags/audio/task/ags_tic_soundcard.h>

#include <ags/object/ags_connectable.h>

void ags_tic_soundcard_class_init(AgsTicSoundcardClass *tic_soundcard);
void ags_tic_soundcard_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_tic_soundcard_init(AgsTicSoundcard *tic_soundcard);
void ags_tic_soundcard_connect(AgsConnectable *connectable);
void ags_tic_soundcard_disconnect(AgsConnectable *connectable);
void ags_tic_soundcard_finalize(GObject *gobject);

void ags_tic_soundcard_launch(AgsTask *task);

/**
 * SECTION:ags_tic_soundcard
 * @short_description: tic soundcard object
 * @title: AgsTicSoundcard
 * @section_id:
 * @include: ags/audio/task/ags_tic_soundcard.h
 *
 * The #AgsTicSoundcard task tics #AgsSoundcard.
 */

static gpointer ags_tic_soundcard_parent_class = NULL;
static AgsConnectableInterface *ags_tic_soundcard_parent_connectable_interface;

GType
ags_tic_soundcard_get_type()
{
  static GType ags_type_tic_soundcard = 0;

  if(!ags_type_tic_soundcard){
    static const GTypeInfo ags_tic_soundcard_info = {
      sizeof (AgsTicSoundcardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_tic_soundcard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTicSoundcard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_tic_soundcard_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_tic_soundcard_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_tic_soundcard = g_type_register_static(AGS_TYPE_TASK,
						    "AgsTicSoundcard\0",
						    &ags_tic_soundcard_info,
						    0);

    g_type_add_interface_static(ags_type_tic_soundcard,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_tic_soundcard);
}

void
ags_tic_soundcard_class_init(AgsTicSoundcardClass *tic_soundcard)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_tic_soundcard_parent_class = g_type_class_peek_parent(tic_soundcard);

  /* gobject */
  gobject = (GObjectClass *) tic_soundcard;

  gobject->finalize = ags_tic_soundcard_finalize;

  /* task */
  task = (AgsTaskClass *) tic_soundcard;

  task->launch = ags_tic_soundcard_launch;
}

void
ags_tic_soundcard_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_tic_soundcard_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_tic_soundcard_connect;
  connectable->disconnect = ags_tic_soundcard_disconnect;
}

void
ags_tic_soundcard_init(AgsTicSoundcard *tic_soundcard)
{
  tic_soundcard->soundcard = NULL;
}

void
ags_tic_soundcard_connect(AgsConnectable *connectable)
{
  ags_tic_soundcard_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_tic_soundcard_disconnect(AgsConnectable *connectable)
{
  ags_tic_soundcard_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_tic_soundcard_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_tic_soundcard_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_tic_soundcard_launch(AgsTask *task)
{
  AgsTicSoundcard *tic_soundcard;

  tic_soundcard = AGS_TIC_SOUNDCARD(task);

  ags_soundcard_tic(AGS_SOUNDCARD(tic_soundcard->soundcard));
}

/**
 * ags_tic_soundcard_new:
 * @soundcard: the #AgsSoundcard to tic
 *
 * Creates an #AgsTicSoundcard.
 *
 * Returns: an new #AgsTicSoundcard.
 *
 * Since: 0.7.105
 */
AgsTicSoundcard*
ags_tic_soundcard_new(GObject *soundcard)
{
  AgsTicSoundcard *tic_soundcard;

  tic_soundcard = (AgsTicSoundcard *) g_object_new(AGS_TYPE_TIC_SOUNDCARD,
						   NULL);

  tic_soundcard->soundcard = soundcard;

  return(tic_soundcard);
}
