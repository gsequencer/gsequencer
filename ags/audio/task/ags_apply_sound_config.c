/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/task/ags_apply_sound_config.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/i18n.h>

void ags_apply_sound_config_class_init(AgsApplySoundConfigClass *apply_sound_config);
void ags_apply_sound_config_init(AgsApplySoundConfig *apply_sound_config);
void ags_apply_sound_config_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_apply_sound_config_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_apply_sound_config_dispose(GObject *gobject);
void ags_apply_sound_config_finalize(GObject *gobject);

void ags_apply_sound_config_launch(AgsTask *task);

/**
 * SECTION:ags_apply_sound_config
 * @short_description: apply sound config
 * @title: AgsApplySoundConfig
 * @section_id:
 * @include: ags/audio/task/ags_apply_sound_config.h
 *
 * The #AgsApplySoundConfig task apply the specified sound config.
 */

enum{
  PROP_0,
  PROP_CONFIG_DATA,
};

static gpointer ags_apply_sound_config_parent_class = NULL;

GType
ags_apply_sound_config_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_apply_sound_config = 0;

    static const GTypeInfo ags_apply_sound_config_info = {
      sizeof(AgsApplySoundConfigClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_sound_config_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsApplySoundConfig),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_sound_config_init,
    };

    ags_type_apply_sound_config = g_type_register_static(AGS_TYPE_TASK,
							 "AgsApplySoundConfig",
							 &ags_apply_sound_config_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_apply_sound_config);
  }

  return g_define_type_id__volatile;
}

void
ags_apply_sound_config_class_init(AgsApplySoundConfigClass *apply_sound_config)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;
  
  ags_apply_sound_config_parent_class = g_type_class_peek_parent(apply_sound_config);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_sound_config;

  gobject->set_property = ags_apply_sound_config_set_property;
  gobject->get_property = ags_apply_sound_config_get_property;

  gobject->dispose = ags_apply_sound_config_dispose;
  gobject->finalize = ags_apply_sound_config_finalize;

  /* properties */
  /**
   * AgsApplySoundConfig:config-data:
   *
   * The assigned sound config data as string.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_string("config-data",
				   i18n_pspec("sound config data"),
				   i18n_pspec("The sound config data to apply"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONFIG_DATA,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_sound_config;

  task->launch = ags_apply_sound_config_launch;
}

void
ags_apply_sound_config_init(AgsApplySoundConfig *apply_sound_config)
{
  apply_sound_config->config_data = NULL;
}

void
ags_apply_sound_config_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsApplySoundConfig *apply_sound_config;

  apply_sound_config = AGS_APPLY_SOUND_CONFIG(gobject);

  switch(prop_id){
  case PROP_CONFIG_DATA:
    {
      gchar *config_data;

      config_data = g_value_get_string(value);

      if(config_data == apply_sound_config->config_data){
	return;
      }

      g_free(apply_sound_config->config_data);
      
      apply_sound_config->config_data = g_strdup(config_data);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_sound_config_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsApplySoundConfig *apply_sound_config;

  apply_sound_config = AGS_APPLY_SOUND_CONFIG(gobject);

  switch(prop_id){
  case PROP_CONFIG_DATA:
    {
      g_value_set_string(value,
			 apply_sound_config->config_data);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_sound_config_dispose(GObject *gobject)
{
  AgsApplySoundConfig *apply_sound_config;

  apply_sound_config = AGS_APPLY_SOUND_CONFIG(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_apply_sound_config_parent_class)->dispose(gobject);
}

void
ags_apply_sound_config_finalize(GObject *gobject)
{
  AgsApplySoundConfig *apply_sound_config;

  apply_sound_config = AGS_APPLY_SOUND_CONFIG(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_apply_sound_config_parent_class)->finalize(gobject);
}

void
ags_apply_sound_config_launch(AgsTask *task)
{
  AgsApplySoundConfig *apply_sound_config;

  apply_sound_config = AGS_APPLY_SOUND_CONFIG(task);

  //TODO:JK: implement me
}

/**
 * ags_apply_sound_config_new:
 * @config_data: the config file as string data
 *
 * Creates a new instance of #AgsApplySoundConfig.
 *
 * Returns: the new #AgsApplySoundConfig.
 *
 * Since: 2.1.0
 */
AgsApplySoundConfig*
ags_apply_sound_config_new(gchar *config_data)
{
  AgsApplySoundConfig *apply_sound_config;

  apply_sound_config = (AgsApplySoundConfig *) g_object_new(AGS_TYPE_APPLY_SOUND_CONFIG,
							    "config-data", config_data,
							    NULL);

  return(apply_sound_config);
}
