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

#ifndef __AGS_VST_COMPONENT_BASE_H__
#define __AGS_VST_COMPONENT_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstComponentBase _AgsVstComponentBase;
  
  AgsVstComponentBase* ags_vst_component_base_new();
  void ags_vst_component_base_finalize(AgsVstComponentBase *component_base);

  void ags_vst_component_base_ref(AgsVstComponentBase *component_base);
  void ags_vst_component_base_unref(AgsVstComponentBase *component_base);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_COMPONENT_BASE_H__*/
