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

#ifndef __AGS_VST_ISTRING_RESULT_H__
#define __AGS_VST_ISTRING_RESULT_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>

#define AGS_VST_ISTRING_RESULT_IID (ags_vst_istring_result_get_iid())
#define AGS_VST_ISTRING_IID (ags_vst_istring_get_iid())

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct IStringResult AgsVstIStringResult;
  typedef struct IString AgsVstIString;

  AgsVstTUID* ags_vst_istring_result_get_iid();

  void ags_vst_istring_result_set_text(AgsVstIStringResult *istring_result, gchar *text);
  
  AgsVstTUID* ags_vst_istring_get_iid();

  void ags_vst_istring_set_text8(AgsVstIString *istring, gchar *text);
  void ags_vst_istring_set_text16(AgsVstIString *istring, gunichar2 *text);

  const gchar* ags_vst_istring_get_text8(AgsVstIString *istring);
  const gunichar2* ags_vst_istring_get_text16(AgsVstIString *istring);
  
  void ags_vst_istring_take(AgsVstIString *istring, void *s, gboolean is_wide);

  gboolean ags_vst_istring_is_wide_string(AgsVstIString *istring);

#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_ISTRING_RESULT_H__*/
