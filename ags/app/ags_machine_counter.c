/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_machine_counter.h>

#include <ags/app/ags_machine.h>

#include <ags/i18n.h>

void ags_machine_counter_class_init(AgsMachineCounterClass *machine_counter);
void ags_machine_counter_init(AgsMachineCounter *machine_counter);
void ags_machine_counter_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_machine_counter_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_machine_counter_dispose(GObject *gobject);
void ags_machine_counter_finalize(GObject *gobject);

/**
 * SECTION:ags_machine_counter
 * @short_description: The machine counter
 * @title: AgsMachineCounter
 * @section_id:
 * @include: ags/plugin/ags_machine_counter.h
 *
 * The #AgsMachineCounter counts machines of specific type.
 */

enum{
  PROP_0,
  PROP_MACHINE_TYPE,
  PROP_FILENAME,
  PROP_EFFECT,
};

static gpointer ags_machine_counter_parent_class = NULL;

GType
ags_machine_counter_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_counter = 0;

    static const GTypeInfo ags_machine_counter_info = {
      sizeof(AgsMachineCounterClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_counter_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMachineCounter),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_counter_init,
    };

    ags_type_machine_counter = g_type_register_static(G_TYPE_OBJECT,
						      "AgsMachineCounter",
						      &ags_machine_counter_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_counter);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_counter_class_init(AgsMachineCounterClass *machine_counter)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_machine_counter_parent_class = g_type_class_peek_parent(machine_counter);

  /* GObjectClass */
  gobject = (GObjectClass *) machine_counter;

  gobject->set_property = ags_machine_counter_set_property;
  gobject->get_property = ags_machine_counter_get_property;

  gobject->dispose = ags_machine_counter_dispose;
  gobject->finalize = ags_machine_counter_finalize;

  /* properties */
  /**
   * AgsMachineCounter:machine-type:
   *
   * The assigned machine type
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_gtype("machine-type",
				  i18n_pspec("machine type"),
				  i18n_pspec("The machine type"),
				  AGS_TYPE_MACHINE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE_TYPE,
				  param_spec);

  /**
   * AgsMachineCounter:filename:
   *
   * The assigned filename.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("filename of the plugin"),
				   i18n_pspec("The filename this plugin is located in"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsMachineCounter:effect:
   *
   * The assigned effect.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_string("effect",
				   i18n_pspec("effect of the plugin"),
				   i18n_pspec("The effect this plugin is assigned with"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);  
}

void
ags_machine_counter_init(AgsMachineCounter *machine_counter)
{
  machine_counter->version = NULL;
  machine_counter->build_id = NULL;

  /*  */
  machine_counter->machine_type = G_TYPE_NONE;
  
  machine_counter->filename = NULL;
  machine_counter->effect = NULL;

  machine_counter->counter = 0;
}

void
ags_machine_counter_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsMachineCounter *machine_counter;

  machine_counter = AGS_MACHINE_COUNTER(gobject);

  switch(prop_id){
  case PROP_MACHINE_TYPE:
    {
      machine_counter->machine_type = g_value_get_gtype(value);
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_string(value);

      if(machine_counter->filename == filename){
	return;
      }
      
      if(machine_counter->filename != NULL){
	g_free(machine_counter->filename);
      }

      machine_counter->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;

      effect = (gchar *) g_value_get_string(value);

      if(machine_counter->effect == effect){
	return;
      }
      
      if(machine_counter->effect != NULL){
	g_free(machine_counter->effect);
      }

      machine_counter->effect = g_strdup(effect);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_counter_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsMachineCounter *machine_counter;

  machine_counter = AGS_MACHINE_COUNTER(gobject);

  switch(prop_id){
  case PROP_MACHINE_TYPE:
    {
      g_value_set_gtype(value, machine_counter->machine_type);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_string(value, machine_counter->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, machine_counter->effect);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_counter_dispose(GObject *gobject)
{
  AgsMachineCounter *machine_counter;

  machine_counter = AGS_MACHINE_COUNTER(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_machine_counter_parent_class)->dispose(gobject);
}

void
ags_machine_counter_finalize(GObject *gobject)
{
  AgsMachineCounter *machine_counter;

  machine_counter = AGS_MACHINE_COUNTER(gobject);
  
  /* filename and effect */
  g_free(machine_counter->filename);
  g_free(machine_counter->effect);
  
  /* call parent */
  G_OBJECT_CLASS(ags_machine_counter_parent_class)->finalize(gobject);
}

void
ags_machine_counter_increment(AgsMachineCounter *machine_counter)
{
  g_return_if_fail(AGS_IS_MACHINE_COUNTER(machine_counter));

  machine_counter->counter += 1;
}

/**
 * ags_machine_counter_new:
 * @machine_type: the machine's type
 * @filename: the plugin .so
 * @effect: the effect's string representation
 *
 * Creates an #AgsMachineCounter
 *
 * Returns: a new #AgsMachineCounter
 *
 * Since: 4.0.0
 */
AgsMachineCounter*
ags_machine_counter_new(GType machine_type,
			gchar *filename, gchar *effect)
{
  AgsMachineCounter *machine_counter;

  machine_counter = (AgsMachineCounter *) g_object_new(AGS_TYPE_MACHINE_COUNTER,
						       "machine-type", machine_type,
						       "filename", filename,
						       "effect", effect,
						       NULL);

  return(machine_counter);
}
