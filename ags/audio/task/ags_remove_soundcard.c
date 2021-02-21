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

#include <ags/audio/task/ags_remove_soundcard.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/i18n.h>

void ags_remove_soundcard_class_init(AgsRemoveSoundcardClass *remove_soundcard);
void ags_remove_soundcard_init(AgsRemoveSoundcard *remove_soundcard);
void ags_remove_soundcard_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_remove_soundcard_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_remove_soundcard_dispose(GObject *gobject);
void ags_remove_soundcard_finalize(GObject *gobject);

void ags_remove_soundcard_launch(AgsTask *task);

/**
 * SECTION:ags_remove_soundcard
 * @short_description: remove soundcard object of application context
 * @title: AgsRemoveSoundcard
 * @section_id:
 * @include: ags/audio/task/ags_remove_soundcard.h
 *
 * The #AgsRemoveSoundcard task removes #AgsSoundcard of application context.
 */

static gpointer ags_remove_soundcard_parent_class = NULL;

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

GType
ags_remove_soundcard_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_remove_soundcard = 0;

    static const GTypeInfo ags_remove_soundcard_info = {
      sizeof(AgsRemoveSoundcardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_soundcard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsRemoveSoundcard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_soundcard_init,
    };

    ags_type_remove_soundcard = g_type_register_static(AGS_TYPE_TASK,
						       "AgsRemoveSoundcard",
						       &ags_remove_soundcard_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_remove_soundcard);
  }

  return g_define_type_id__volatile;
}

void
ags_remove_soundcard_class_init(AgsRemoveSoundcardClass *remove_soundcard)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_remove_soundcard_parent_class = g_type_class_peek_parent(remove_soundcard);

  /* gobject */
  gobject = (GObjectClass *) remove_soundcard;

  gobject->set_property = ags_remove_soundcard_set_property;
  gobject->get_property = ags_remove_soundcard_get_property;

  gobject->dispose = ags_remove_soundcard_dispose;
  gobject->finalize = ags_remove_soundcard_finalize;
  
  /* properties */
  /**
   * AgsRemoveSoundcard:soundcard:
   *
   * The assigned #AgsSoundcard
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard of remove soundcard"),
				   i18n_pspec("The soundcard of remove soundcard task"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) remove_soundcard;

  task->launch = ags_remove_soundcard_launch;
}

void
ags_remove_soundcard_init(AgsRemoveSoundcard *remove_soundcard)
{
  remove_soundcard->soundcard = NULL;
}

void
ags_remove_soundcard_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsRemoveSoundcard *remove_soundcard;

  remove_soundcard = AGS_REMOVE_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      if(remove_soundcard->soundcard == (GObject *) soundcard){
	return;
      }

      if(remove_soundcard->soundcard != NULL){
	g_object_unref(remove_soundcard->soundcard);
      }

      if(soundcard != NULL){
	g_object_ref(soundcard);
      }

      remove_soundcard->soundcard = (GObject *) soundcard;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_remove_soundcard_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsRemoveSoundcard *remove_soundcard;

  remove_soundcard = AGS_REMOVE_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, remove_soundcard->soundcard);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_remove_soundcard_dispose(GObject *gobject)
{
  AgsRemoveSoundcard *remove_soundcard;

  remove_soundcard = AGS_REMOVE_SOUNDCARD(gobject);
  
  if(remove_soundcard->soundcard != NULL){
    g_object_unref(remove_soundcard->soundcard);

    remove_soundcard->soundcard = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_remove_soundcard_parent_class)->dispose(gobject);
}

void
ags_remove_soundcard_finalize(GObject *gobject)
{
  AgsRemoveSoundcard *remove_soundcard;

  remove_soundcard = AGS_REMOVE_SOUNDCARD(gobject);
  
  if(remove_soundcard->soundcard != NULL){
    g_object_unref(remove_soundcard->soundcard);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_remove_soundcard_parent_class)->finalize(gobject);
}

void
ags_remove_soundcard_launch(AgsTask *task)
{
  AgsRemoveSoundcard *remove_soundcard;

  AgsApplicationContext *application_context;
  
  GList *start_list;
  
  remove_soundcard = AGS_REMOVE_SOUNDCARD(task);

  application_context = ags_application_context_get_instance();

  g_return_if_fail(AGS_IS_SOUND_PROVIDER(application_context));
  g_return_if_fail(AGS_IS_SOUNDCARD(remove_soundcard->soundcard));
  
  /* remove soundcard */
  start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  if(g_list_find(start_list, remove_soundcard->soundcard) != NULL){
    ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				     g_list_remove(start_list,
						   remove_soundcard->soundcard));

    g_object_unref(remove_soundcard->soundcard);
  }else{
    g_list_free_full(start_list,
		     (GDestroyNotify) g_object_unref);
  }
}

/**
 * ags_remove_soundcard_new:
 * @soundcard: the #AgsSoundcard to remove
 *
 * Create a new instance of #AgsRemoveSoundcard.
 *
 * Returns: the new #AgsRemoveSoundcard.
 *
 * Since: 3.0.0
 */
AgsRemoveSoundcard*
ags_remove_soundcard_new(GObject *soundcard)
{
  AgsRemoveSoundcard *remove_soundcard;

  remove_soundcard = (AgsRemoveSoundcard *) g_object_new(AGS_TYPE_REMOVE_SOUNDCARD,
							 "soundcard", soundcard,
							 NULL);

  return(remove_soundcard);
}
