/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/plugin/ags_lv2_preset_manager.h>

void ags_lv2_preset_manager_class_init(AgsLv2PresetManagerClass *lv2_preset_manager);
void ags_lv2_preset_manager_init(AgsLv2PresetManager *lv2_preset_manager);
void ags_lv2_preset_manager_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_lv2_preset_manager_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_lv2_preset_manager_dispose(GObject *gobject);
void ags_lv2_preset_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2_preset_manager
 * @short_description: Singleton pattern to organize LV2 presets
 * @title: AgsLv2PresetManager
 * @section_id:
 * @include: ags/plugin/ags_lv2_preset_manager.h
 *
 * The #AgsLv2PresetManager loads/unloads LV2 presets.
 */

enum{
  PROP_0,
};

static gpointer ags_lv2_preset_manager_parent_class = NULL;

AgsLv2PresetManager *ags_lv2_preset_manager = NULL;

GType
ags_lv2_preset_manager_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lv2_preset_manager = 0;

    static const GTypeInfo ags_lv2_preset_manager_info = {
      sizeof (AgsLv2PresetManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_preset_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2PresetManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_preset_manager_init,
    };

    ags_type_lv2_preset_manager = g_type_register_static(G_TYPE_OBJECT,
							 "AgsLv2PresetManager",
							 &ags_lv2_preset_manager_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lv2_preset_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_lv2_preset_manager_class_init(AgsLv2PresetManagerClass *lv2_preset_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_lv2_preset_manager_parent_class = g_type_class_peek_parent(lv2_preset_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) lv2_preset_manager;

  gobject->set_property = ags_lv2_preset_manager_set_property;
  gobject->get_property = ags_lv2_preset_manager_get_property;

  gobject->dispose = ags_lv2_preset_manager_dispose;
  gobject->finalize = ags_lv2_preset_manager_finalize;

  /* properties */
}

void
ags_lv2_preset_manager_init(AgsLv2PresetManager *lv2_preset_manager)
{  
  /* lv2 manager mutex */
  g_rec_mutex_init(&(lv2_preset_manager->obj_mutex));


  /* initialize lv2 plugin GList */
  lv2_preset_manager->lv2_preset = NULL;
}

void
ags_lv2_preset_manager_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsLv2PresetManager *lv2_preset_manager;

  lv2_preset_manager = AGS_LV2_PRESET_MANAGER(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_preset_manager_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsLv2PresetManager *lv2_preset_manager;

  lv2_preset_manager = AGS_LV2_PRESET_MANAGER(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_preset_manager_dispose(GObject *gobject)
{
  AgsLv2PresetManager *lv2_preset_manager;

  lv2_preset_manager = AGS_LV2_PRESET_MANAGER(gobject);

  if(lv2_preset_manager->lv2_preset != NULL){
    g_list_free_full(lv2_preset_manager->lv2_preset,
		     g_object_unref);

    lv2_preset_manager->lv2_preset = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_lv2_preset_manager_parent_class)->dispose(gobject);
}

void
ags_lv2_preset_manager_finalize(GObject *gobject)
{
  AgsLv2PresetManager *lv2_preset_manager;

  GList *lv2_preset;

  lv2_preset_manager = AGS_LV2_PRESET_MANAGER(gobject);

  lv2_preset = lv2_preset_manager->lv2_preset;

  g_list_free_full(lv2_preset,
		   g_object_unref);

  if(lv2_preset_manager == ags_lv2_preset_manager){
    ags_lv2_preset_manager = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_lv2_preset_manager_parent_class)->finalize(gobject);
}

/**
 * ags_lv2_preset_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsLv2PresetManager
 *
 * Since: 2.2.0
 */
AgsLv2PresetManager*
ags_lv2_preset_manager_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_lv2_preset_manager == NULL){
    ags_lv2_preset_manager = ags_lv2_preset_manager_new();
  }
  
  g_mutex_unlock(&mutex);

  return(ags_lv2_preset_manager);
}

/**
 * ags_lv2_preset_manager_new:
 *
 * Create a new instance of #AgsLv2PresetManager
 *
 * Returns: the new #AgsLv2PresetManager
 *
 * Since: 2.2.0
 */
AgsLv2PresetManager*
ags_lv2_preset_manager_new()
{
  AgsLv2PresetManager *lv2_preset_manager;

  lv2_preset_manager = (AgsLv2PresetManager *) g_object_new(AGS_TYPE_LV2_PRESET_MANAGER,
							    NULL);

  return(lv2_preset_manager);
}
