/* This file is part of GSequencer.
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

#include <ags/audio/task/ags_seek_devout.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_seekable.h>

#include <ags/main.h>

void ags_seek_devout_class_init(AgsSeekDevoutClass *seek_devout);
void ags_seek_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_seek_devout_init(AgsSeekDevout *seek_devout);
void ags_seek_devout_connect(AgsConnectable *connectable);
void ags_seek_devout_disconnect(AgsConnectable *connectable);
void ags_seek_devout_finalize(GObject *gobject);

void ags_seek_devout_launch(AgsTask *task);

/**
 * SECTION:ags_seek_devout
 * @short_description: seek devout object to devout loop
 * @title: AgsSeekDevout
 * @section_id:
 * @include: ags/devout/task/ags_seek_devout.h
 *
 * The #AgsSeekDevout task seeks #AgsDevout to #AgsDevoutLoop.
 */

static gpointer ags_seek_devout_parent_class = NULL;
static AgsConnectableInterface *ags_seek_devout_parent_connectable_interface;

GType
ags_seek_devout_get_type()
{
  static GType ags_type_seek_devout = 0;

  if(!ags_type_seek_devout){
    static const GTypeInfo ags_seek_devout_info = {
      sizeof (AgsSeekDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_seek_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSeekDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_seek_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_seek_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_seek_devout = g_type_register_static(AGS_TYPE_TASK,
						  "AgsSeekDevout\0",
						  &ags_seek_devout_info,
						  0);

    g_type_add_interface_static(ags_type_seek_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_seek_devout);
}

void
ags_seek_devout_class_init(AgsSeekDevoutClass *seek_devout)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_seek_devout_parent_class = g_type_class_peek_parent(seek_devout);

  /* gobject */
  gobject = (GObjectClass *) seek_devout;

  gobject->finalize = ags_seek_devout_finalize;

  /* task */
  task = (AgsTaskClass *) seek_devout;

  task->launch = ags_seek_devout_launch;
}

void
ags_seek_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_seek_devout_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_seek_devout_connect;
  connectable->disconnect = ags_seek_devout_disconnect;
}

void
ags_seek_devout_init(AgsSeekDevout *seek_devout)
{
  seek_devout->devout = NULL;
  seek_devout->steps = 0;
  seek_devout->move_forward = FALSE;
}

void
ags_seek_devout_connect(AgsConnectable *connectable)
{
  ags_seek_devout_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_seek_devout_disconnect(AgsConnectable *connectable)
{
  ags_seek_devout_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_seek_devout_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_seek_devout_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_seek_devout_launch(AgsTask *task)
{
  AgsSeekDevout *seek_devout;
  GList *audio, *recall;

  seek_devout = AGS_SEEK_DEVOUT(task);

  audio = AGS_DEVOUT(seek_devout->devout)->audio;

  while(audio != NULL){
    recall = AGS_AUDIO(audio->data)->play;

    while(recall != NULL){
      if(AGS_IS_SEEKABLE(recall->data)){
	ags_seekable_seek(AGS_SEEKABLE(recall->data),
			  seek_devout->steps,
			  seek_devout->move_forward);
      }

      recall = recall->next;
    }

    audio = audio->next;
  }
  
  if(seek_devout->move_forward){
    AGS_DEVOUT(seek_devout->devout)->tact_counter += seek_devout->steps;
  }else{
    AGS_DEVOUT(seek_devout->devout)->tact_counter -= seek_devout->steps;
  }
}

/**
 * ags_seek_devout_new:
 * @devout: the #AgsDevout to seek
 * @steps:
 * @move_forward:
 *
 * Creates an #AgsSeekDevout.
 *
 * Returns: an new #AgsSeekDevout.
 *
 * Since: 0.4
 */
AgsSeekDevout*
ags_seek_devout_new(GObject *devout,
		    guint steps,
		    gboolean move_forward)
{
  AgsSeekDevout *seek_devout;

  seek_devout = (AgsSeekDevout *) g_object_new(AGS_TYPE_SEEK_DEVOUT,
					       NULL);

  seek_devout->devout = devout;
  seek_devout->steps = steps;
  seek_devout->move_forward = move_forward;

  return(seek_devout);
}
