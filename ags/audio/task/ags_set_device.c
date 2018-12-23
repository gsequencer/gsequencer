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

#include <ags/audio/task/ags_set_device.h>

#include <ags/i18n.h>

void ags_set_device_class_init(AgsSetDeviceClass *set_device);
void ags_set_device_init(AgsSetDevice *set_device);
void ags_set_device_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_set_device_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_set_device_dispose(GObject *gobject);
void ags_set_device_finalize(GObject *gobject);

void ags_set_device_launch(AgsTask *task);

/**
 * SECTION:ags_set_device
 * @short_description: set device
 * @title: AgsSetDevice
 * @section_id:
 * @include: ags/audio/task/ags_set_device.h
 *
 * The #AgsSetDevice task sets device of #AgsSoundcard or #AgsSequencer.
 */

static gpointer ags_set_device_parent_class = NULL;

enum{
  PROP_0,
  PROP_SCOPE,
  PROP_DEVICE,
};

GType
ags_set_device_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_set_device = 0;

    static const GTypeInfo ags_set_device_info = {
      sizeof(AgsSetDeviceClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_set_device_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSetDevice),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_set_device_init,
    };

    ags_type_set_device = g_type_register_static(AGS_TYPE_TASK,
						 "AgsSetDevice",
						 &ags_set_device_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_set_device);
  }

  return g_define_type_id__volatile;
}

void
ags_set_device_class_init(AgsSetDeviceClass *set_device)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_set_device_parent_class = g_type_class_peek_parent(set_device);

  /* gobject */
  gobject = (GObjectClass *) set_device;

  gobject->set_property = ags_set_device_set_property;
  gobject->get_property = ags_set_device_get_property;

  gobject->dispose = ags_set_device_dispose;
  gobject->finalize = ags_set_device_finalize;
  
  /* properties */
  /**
   * AgsSetDevice:scope:
   *
   * The assigned #AgsSoundcard or #AgsSequencer instance.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_object("scope",
				   i18n_pspec("scope to set device"),
				   i18n_pspec("The scope to set device"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCOPE,
				  param_spec);

  /**
   * AgsSetDevice:device:
   *
   * The device to set.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("device"),
				   i18n_pspec("The device"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) set_device;

  task->launch = ags_set_device_launch;
}

void
ags_set_device_init(AgsSetDevice *set_device)
{
  set_device->scope = NULL;
  set_device->device = NULL;
}

void
ags_set_device_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsSetDevice *set_device;

  set_device = AGS_SET_DEVICE(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      GObject *scope;

      scope = (GObject *) g_value_get_object(value);

      if(set_device->scope == (GObject *) scope){
	return;
      }

      if(set_device->scope != NULL){
	g_object_unref(set_device->scope);
      }

      if(scope != NULL){
	g_object_ref(scope);
      }

      set_device->scope = (GObject *) scope;
    }
    break;
  case PROP_DEVICE:
    {
      gchar *device;
      
      device = g_value_get_string(value);

      if(device == set_device->device){
	return;
      }
      
      g_free(set_device->device);
      
      set_device->device = g_strdup(device);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_device_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsSetDevice *set_device;

  set_device = AGS_SET_DEVICE(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      g_value_set_object(value, set_device->scope);
    }
    break;
  case PROP_DEVICE:
    {
      g_value_set_string(value, set_device->device);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_device_dispose(GObject *gobject)
{
  AgsSetDevice *set_device;

  set_device = AGS_SET_DEVICE(gobject);
  
  if(set_device->scope != NULL){
    g_object_unref(set_device->scope);

    set_device->scope = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_set_device_parent_class)->dispose(gobject);
}

void
ags_set_device_finalize(GObject *gobject)
{
  AgsSetDevice *set_device;

  set_device = AGS_SET_DEVICE(gobject);
  
  if(set_device->scope != NULL){
    g_object_unref(set_device->scope);
  }

  g_free(set_device->device);
  
  /* call parent */
  G_OBJECT_CLASS(ags_set_device_parent_class)->finalize(gobject);
}

void
ags_set_device_launch(AgsTask *task)
{
  AgsSetDevice *set_device;

  set_device = AGS_SET_DEVICE(task);

  /* set audio channels */
  if(AGS_IS_SOUNDCARD(set_device->scope)){
    ags_soundcard_set_device(AGS_SOUNDCARD(set_device->scope),
			     set_device->device);
  }else if(AGS_IS_SEQUENCER(set_device->scope)){
    ags_sequencer_set_device(AGS_SEQUENCER(set_device->scope),
			     set_device->device);
  }
}

/**
 * ags_set_device_new:
 * @scope: the #AgsSoundcard or #AgsSequencer to reset
 * @device: the device as string
 *
 * Create a new instance of #AgsSetDevice.
 *
 * Returns: the new #AgsSetDevice.
 *
 * Since: 2.1.0
 */
AgsSetDevice*
ags_set_device_new(GObject *scope, gchar *device)
{
  AgsSetDevice *set_device;

  set_device = (AgsSetDevice *) g_object_new(AGS_TYPE_SET_DEVICE,
					     "scope", scope,
					     "device", device,
					     NULL);

  return(set_device);
}
