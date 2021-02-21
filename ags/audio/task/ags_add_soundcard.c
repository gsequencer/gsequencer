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

#include <ags/audio/task/ags_add_soundcard.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/i18n.h>

void ags_add_soundcard_class_init(AgsAddSoundcardClass *add_soundcard);
void ags_add_soundcard_init(AgsAddSoundcard *add_soundcard);
void ags_add_soundcard_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_add_soundcard_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_add_soundcard_dispose(GObject *gobject);
void ags_add_soundcard_finalize(GObject *gobject);

void ags_add_soundcard_launch(AgsTask *task);

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

/**
 * SECTION:ags_add_soundcard
 * @short_description: add soundcard object to context
 * @title: AgsAddSoundcard
 * @section_id:
 * @include: ags/audio/task/ags_add_soundcard.h
 *
 * The #AgsAddSoundcard task adds #AgsSoundcard to context.
 */

static gpointer ags_add_soundcard_parent_class = NULL;

GType
ags_add_soundcard_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_add_soundcard = 0;

    static const GTypeInfo ags_add_soundcard_info = {
      sizeof(AgsAddSoundcardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_soundcard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAddSoundcard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_soundcard_init,
    };

    ags_type_add_soundcard = g_type_register_static(AGS_TYPE_TASK,
						    "AgsAddSoundcard",
						    &ags_add_soundcard_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_add_soundcard);
  }

  return g_define_type_id__volatile;
}

void
ags_add_soundcard_class_init(AgsAddSoundcardClass *add_soundcard)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_add_soundcard_parent_class = g_type_class_peek_parent(add_soundcard);

  /* gobject */
  gobject = (GObjectClass *) add_soundcard;

  gobject->set_property = ags_add_soundcard_set_property;
  gobject->get_property = ags_add_soundcard_get_property;

  gobject->dispose = ags_add_soundcard_dispose;
  gobject->finalize = ags_add_soundcard_finalize;

  /**
   * AgsAddSoundcard:soundcard:
   *
   * The assigned #AgsSoundcard
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard of add soundcard"),
				   i18n_pspec("The soundcard of add soundcard task"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) add_soundcard;

  task->launch = ags_add_soundcard_launch;
}

void
ags_add_soundcard_init(AgsAddSoundcard *add_soundcard)
{
  add_soundcard->soundcard = NULL;
}

void
ags_add_soundcard_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec)
{
  AgsAddSoundcard *add_soundcard;

  add_soundcard = AGS_ADD_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      if(add_soundcard->soundcard == (GObject *) soundcard){
	return;
      }

      if(add_soundcard->soundcard != NULL){
	g_object_unref(add_soundcard->soundcard);
      }

      if(soundcard != NULL){
	g_object_ref(soundcard);
      }

      add_soundcard->soundcard = (GObject *) soundcard;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_soundcard_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec)
{
  AgsAddSoundcard *add_soundcard;

  add_soundcard = AGS_ADD_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, add_soundcard->soundcard);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_soundcard_dispose(GObject *gobject)
{
  AgsAddSoundcard *add_soundcard;

  add_soundcard = AGS_ADD_SOUNDCARD(gobject);

  if(add_soundcard->soundcard != NULL){
    g_object_unref(add_soundcard->soundcard);

    add_soundcard->soundcard = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_add_soundcard_parent_class)->dispose(gobject);
}

void
ags_add_soundcard_finalize(GObject *gobject)
{
  AgsAddSoundcard *add_soundcard;

  add_soundcard = AGS_ADD_SOUNDCARD(gobject);

  if(add_soundcard->soundcard != NULL){
    g_object_unref(add_soundcard->soundcard);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_add_soundcard_parent_class)->finalize(gobject);
}

void
ags_add_soundcard_launch(AgsTask *task)
{
  AgsAddSoundcard *add_soundcard;

  AgsApplicationContext *application_context;
  
  GList *start_list;
  
  add_soundcard = AGS_ADD_SOUNDCARD(task);

  application_context = ags_application_context_get_instance();

  g_return_if_fail(AGS_IS_SOUND_PROVIDER(application_context));
  g_return_if_fail(AGS_IS_SOUNDCARD(add_soundcard->soundcard));

  /* add soundcard */
  start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  if(g_list_find(start_list, add_soundcard->soundcard) == NULL){
    g_object_ref(add_soundcard->soundcard);
    
    ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				     g_list_append(start_list,
						   add_soundcard->soundcard));
  }else{
    g_list_free_full(start_list,
		     (GDestroyNotify) g_object_unref);
  }
}

/**
 * ags_add_soundcard_new:
 * @soundcard: the #AgsSoundcard to add
 *
 * Create a new instance of #AgsAddSoundcard.
 *
 * Returns: the new #AgsAddSoundcard.
 *
 * Since: 3.0.0
 */
AgsAddSoundcard*
ags_add_soundcard_new(GObject *soundcard)
{
  AgsAddSoundcard *add_soundcard;

  add_soundcard = (AgsAddSoundcard *) g_object_new(AGS_TYPE_ADD_SOUNDCARD,
						   "soundcard", soundcard,
						   NULL);

  return(add_soundcard);
}
