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

#include <ags/X/editor/ags_automation_meta.h>
#include <ags/X/editor/ags_automation_meta_callbacks.h>

#include <ags/i18n.h>

void ags_automation_meta_class_init(AgsAutomationMetaClass *automation_meta);
void ags_automation_meta_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_meta_init(AgsAutomationMeta *automation_meta);

void ags_automation_meta_connect(AgsConnectable *connectable);
void ags_automation_meta_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_automation_meta
 * @short_description: automation_meta
 * @title: AgsAutomationMeta
 * @section_id:
 * @include: ags/X/editor/ags_automation_meta.h
 *
 * The #AgsAutomationMeta provides you information about automation editor.
 */

GType
ags_automation_meta_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_automation_meta = 0;

    static const GTypeInfo ags_automation_meta_info = {
      sizeof (AgsAutomationMetaClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_automation_meta_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutomationMeta),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_automation_meta_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_automation_meta_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_automation_meta = g_type_register_static(GTK_TYPE_VBOX,
						      "AgsAutomationMeta", &ags_automation_meta_info,
						      0);
    
    g_type_add_interface_static(ags_type_automation_meta,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_automation_meta);
  }

  return g_define_type_id__volatile;
}

void
ags_automation_meta_class_init(AgsAutomationMetaClass *automation_meta)
{
  /* empty */
}

void
ags_automation_meta_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_automation_meta_connect;
  connectable->disconnect = ags_automation_meta_disconnect;
}

void
ags_automation_meta_init(AgsAutomationMeta *automation_meta)
{
  //TODO:JK: implement me
}

void
ags_automation_meta_connect(AgsConnectable *connectable)
{
  AgsAutomationMeta *automation_meta;
  
  automation_meta = AGS_AUTOMATION_META(connectable);

  if((AGS_AUTOMATION_META_CONNECTED & (automation_meta->flags)) != 0){
    return;
  }

  automation_meta->flags |= AGS_AUTOMATION_META_CONNECTED;

  //TODO:JK: implement me
}

void
ags_automation_meta_disconnect(AgsConnectable *connectable)
{
  AgsAutomationMeta *automation_meta;

  GList *list;
  
  automation_meta = AGS_AUTOMATION_META(connectable);

  if((AGS_AUTOMATION_META_CONNECTED & (automation_meta->flags)) == 0){
    return;
  }

  automation_meta->flags &= (~AGS_AUTOMATION_META_CONNECTED);

  //TODO:JK: implement me
}

/**
 * ags_automation_meta_new:
 *
 * Create a new #AgsAutomationMeta.
 *
 * Returns: a new #AgsAutomationMeta
 *
 * Since: 3.1.0
 */
AgsAutomationMeta*
ags_automation_meta_new()
{
  AgsAutomationMeta *automation_meta;

  automation_meta = (AgsAutomationMeta *) g_object_new(AGS_TYPE_AUTOMATION_META,
						       NULL);

  return(automation_meta);
}
