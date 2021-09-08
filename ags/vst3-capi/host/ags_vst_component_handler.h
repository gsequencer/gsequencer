/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_VST_COMPONENT_HANDLER_H__
#define __AGS_VST_COMPONENT_HANDLER_H__

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct ComponentHandler AgsVstComponentHandler;

  AgsVstComponentHandler* ags_vst_component_handler_new();
  
  void ags_vst_component_handler_destroy(AgsVstComponentHandler *component_handler);

  int ags_vst_component_handler_connect_handler(AgsVstComponentHandler *component_handler, char *event_name, void *callback, void *data);
  void ags_vst_component_handler_disconnect_handler(AgsVstComponentHandler *component_handler, int handler_id);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_COMPONENT_HANDLER_H__*/
