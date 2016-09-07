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

#include <ags/audio/task/ags_change_soundcard.h>

#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_devout.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_port.h>
#include <ags/audio/jack/ags_jack_devout.h>

void ags_change_soundcard_class_init(AgsChangeSoundcardClass *change_soundcard);
void ags_change_soundcard_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_change_soundcard_init(AgsChangeSoundcard *change_soundcard);
void ags_change_soundcard_connect(AgsConnectable *connectable);
void ags_change_soundcard_disconnect(AgsConnectable *connectable);
void ags_change_soundcard_finalize(GObject *gobject);

void ags_change_soundcard_launch(AgsTask *task);

/**
 * SECTION:ags_change_soundcard
 * @short_description: change soundcard
 * @title: AgsChangeSoundcard
 * @section_id:
 * @include: ags/audio/task/ags_change_soundcard.h
 *
 * The #AgsChangeSoundcard task change the specified soundcard.
 */

static gpointer ags_change_soundcard_parent_class = NULL;
static AgsConnectableInterface *ags_change_soundcard_parent_connectable_interface;

GType
ags_change_soundcard_get_type()
{
  static GType ags_type_change_soundcard = 0;

  if(!ags_type_change_soundcard){
    static const GTypeInfo ags_change_soundcard_info = {
      sizeof (AgsChangeSoundcardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_change_soundcard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChangeSoundcard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_change_soundcard_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_change_soundcard_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_change_soundcard = g_type_register_static(AGS_TYPE_TASK,
						       "AgsChangeSoundcard\0",
						       &ags_change_soundcard_info,
						       0);

    g_type_add_interface_static(ags_type_change_soundcard,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_change_soundcard);
}

void
ags_change_soundcard_class_init(AgsChangeSoundcardClass *change_soundcard)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_change_soundcard_parent_class = g_type_class_peek_parent(change_soundcard);

  /* GObjectClass */
  gobject = (GObjectClass *) change_soundcard;

  gobject->finalize = ags_change_soundcard_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) change_soundcard;

  task->launch = ags_change_soundcard_launch;
}

void
ags_change_soundcard_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_change_soundcard_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_change_soundcard_connect;
  connectable->disconnect = ags_change_soundcard_disconnect;
}

void
ags_change_soundcard_init(AgsChangeSoundcard *change_soundcard)
{
  change_soundcard->application_context = NULL;
  
  change_soundcard->use_alsa = FALSE;
  change_soundcard->use_oss = FALSE;
  change_soundcard->use_jack = FALSE;
}

void
ags_change_soundcard_connect(AgsConnectable *connectable)
{
  ags_change_soundcard_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_change_soundcard_disconnect(AgsConnectable *connectable)
{
  ags_change_soundcard_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_change_soundcard_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_change_soundcard_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_change_soundcard_launch(AgsTask *task)
{
  AgsChangeSoundcard *change_soundcard;

  GObject *soundcard, *old_soundcard;

  GList *list, *audio;
  
  change_soundcard = AGS_CHANGE_SOUNDCARD(task);
  list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(AGS_CHANGE_SOUNDCARD(task)->application_context));

  if(list != NULL){
    old_soundcard = list->data;
  }else{
    old_soundcard = NULL;
  }

  
  if(change_soundcard->use_alsa){
    ags_change_soundcard_alsa(change_soundcard);
  }

  if(change_soundcard->use_oss){
    ags_change_soundcard_oss(change_soundcard);
  }

  if(change_soundcard->use_jack){
    ags_change_soundcard_jack(change_soundcard);

    list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(AGS_CHANGE_SOUNDCARD(task)->application_context));

    if(list != NULL){
      soundcard = list->data;
    }
  }else{
    list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(AGS_CHANGE_SOUNDCARD(task)->application_context));
    soundcard = list->data;

    /* find jack */
    while(list != NULL){
      if(AGS_IS_JACK_DEVOUT(list->data)){
	soundcard = list->data;

	break;
      }
      
      list = list->next;
    }
  }

  /* verifiy first */
  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(AGS_CHANGE_SOUNDCARD(task)->application_context),
				   g_list_remove(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(AGS_CHANGE_SOUNDCARD(task)->application_context)),
						 soundcard));

  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(AGS_CHANGE_SOUNDCARD(task)->application_context),
				   g_list_prepend(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(AGS_CHANGE_SOUNDCARD(task)->application_context)),
						  soundcard));
  
  /* reset audio */
  if(old_soundcard != NULL &&
     old_soundcard != soundcard){
    audio = ags_soundcard_get_audio(AGS_SOUNDCARD(old_soundcard));
    
    ags_soundcard_set_audio(AGS_SOUNDCARD(old_soundcard),
			    NULL);

    ags_soundcard_set_audio(AGS_SOUNDCARD(soundcard),
			    audio);

    while(audio != NULL){
      g_object_set(audio->data,
		   "soundcard\0", soundcard,
		   NULL);

      audio = audio->next;
    }
  }
}

void
ags_change_soundcard_alsa(AgsChangeSoundcard *change_soundcard)
{
  GObject *soundcard;
  
  soundcard = ags_devout_new(change_soundcard->application_context);
  AGS_DEVOUT(soundcard)->flags &= (~AGS_DEVOUT_OSS);
  AGS_DEVOUT(soundcard)->flags |= (AGS_DEVOUT_ALSA);

  AGS_DEVOUT(soundcard)->out.alsa.device = AGS_DEVOUT_DEFAULT_ALSA_DEVICE;
  
  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(change_soundcard->application_context),
				   g_list_prepend(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(change_soundcard->application_context)),
						  soundcard));
  g_object_ref(G_OBJECT(soundcard));
}

void
ags_change_soundcard_oss(AgsChangeSoundcard *change_soundcard)
{
  GObject *soundcard;
  
  soundcard = ags_devout_new(change_soundcard->application_context);
  AGS_DEVOUT(soundcard)->flags &= (~AGS_DEVOUT_ALSA);
  AGS_DEVOUT(soundcard)->flags |= (AGS_DEVOUT_OSS);

  AGS_DEVOUT(soundcard)->out.oss.device = AGS_DEVOUT_DEFAULT_OSS_DEVICE;
  
  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(change_soundcard->application_context),
				   g_list_prepend(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(change_soundcard->application_context)),
						  soundcard));
  g_object_ref(G_OBJECT(soundcard));
}

void
ags_change_soundcard_jack(AgsChangeSoundcard *change_soundcard)
{
  AgsJackServer *jack_server;

  AgsConfig *config;
  
  GObject *soundcard;
  GObject *tmp;

  config = change_soundcard->application_context->config;
  
  jack_server = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(change_soundcard->application_context))->data;
  tmp = ags_distributed_manager_register_soundcard(AGS_DISTRIBUTED_MANAGER(jack_server),
						   TRUE);

  if(tmp != NULL){
    jack_nframes_t samplerate;

    soundcard = tmp;
    ags_soundcard_set_application_context(AGS_SOUNDCARD(soundcard),
					  change_soundcard->application_context);
    ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(change_soundcard->application_context),
				     g_list_prepend(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(change_soundcard->application_context)),
						    soundcard));

    samplerate = jack_get_sample_rate(AGS_JACK_CLIENT(AGS_JACK_PORT(AGS_JACK_DEVOUT(soundcard)->jack_port)->jack_client)->client);
    ags_config_set_value(config,
			 AGS_CONFIG_SOUNDCARD,
			 "samplerate\0",
			 g_strdup_printf("%d\0", samplerate));
    g_object_set(soundcard,
		 "samplerate\0", samplerate,
		 NULL);
    
    g_object_ref(G_OBJECT(soundcard));
  }
}

/**
 * ags_change_soundcard_new:
 * @application_context: the #AgsApplicationContext
 * @use_alsa: add alsa soundcard
 * @use_oss:  add oss soundcard
 * @use_jack: add jack soundcard
 *
 * Creates an #AgsChangeSoundcard.
 *
 * Returns: an new #AgsChangeSoundcard.
 *
 * Since: 0.7.65
 */
AgsChangeSoundcard*
ags_change_soundcard_new(AgsApplicationContext *application_context,
			 gboolean use_alsa,
			 gboolean use_oss,
			 gboolean use_jack)
{
  AgsChangeSoundcard *change_soundcard;

  change_soundcard = (AgsChangeSoundcard *) g_object_new(AGS_TYPE_CHANGE_SOUNDCARD,
							 NULL);

  change_soundcard->use_alsa = use_alsa;
  change_soundcard->use_oss = use_oss;
  change_soundcard->use_jack = use_jack;
  
  return(change_soundcard);
}
