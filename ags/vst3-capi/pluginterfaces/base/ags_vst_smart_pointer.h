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

#ifndef __AGS_VST_FSMART_POINTER_H__
#define __AGS_VST_FSMART_POINTER_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_fplatform.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstIPtr IPtr;

  AgsVstIPtr* ags_vst_iptr_new();
  void ags_vst_iptr_delete(AgsVstIPtr *iptr);

  void* ags_vst_iptr_get(AgsVstIPtr *iptr);

  AgsVstIPtr* ags_vst_iptr_move_ptr(AgsVstIPtr **iptr);

  void ags_vst_iptr_reset(void *obj);

  void* ags_vst_iptr_take(AgsVstIPtr *iptr);
  AgsVstIPtr* ags_vst_iptr_adopt(AgsVstIPtr *iptr,
				 void *obj);

  typedef struct AgsVstOPtr OPtr;

  AgsVstOPtr* ags_vst_optr_new();
  void ags_vst_optr_delete(AgsVstOPtr *optr);

  typedef struct AgsVstUsed Used;

  typedef struct AgsVstAdopt Adopt;

#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_FSMART_POINTER_H__*/
