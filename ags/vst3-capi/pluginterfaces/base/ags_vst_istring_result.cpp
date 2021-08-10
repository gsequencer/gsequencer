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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_istring_result.h>

#include <pluginterfaces/base/istringresult.h>

extern "C" {

  AgsVstTUID*
  ags_vst_istring_result_get_iid()
  {
    extern const Steinberg::TUID IStringResult__iid;

    return((AgsVstTUID *) (&IStringResult__iid));
  }

  void ags_vst_istring_result_set_text(AgsVstIStringResult *istring_result, gchar *text)
  {
    ((Steinberg::IStringResult *) istring_result)->setText(text);
  }

  AgsVstTUID*
  ags_vst_istring_get_iid()
  {
    extern const Steinberg::TUID IString__iid;

    return((AgsVstTUID *) (&IString__iid));
  }

  void ags_vst_istring_set_text8(AgsVstIString *istring, gchar *text)
  {
    ((Steinberg::IString *) istring)->setText8(text);
  }

  void ags_vst_istring_set_text16(AgsVstIString *istring, gunichar2 *text)
  {
    ((Steinberg::IString *) istring)->setText16((char16_t *) text);
  }

  const gchar* ags_vst_istring_get_text8(AgsVstIString *istring)
  {
    return(((Steinberg::IString *) istring)->getText8());
  }

  const gunichar2* ags_vst_istring_get_text16(AgsVstIString *istring)
  {
    return((gunichar2 *) ((Steinberg::IString *) istring)->getText16());
  }

  void ags_vst_istring_take(AgsVstIString *istring, void *s, gboolean is_wide)
  {
    ((Steinberg::IString *) istring)->take(s, is_wide);
  }

  gboolean ags_vst_istring_is_wide_string(AgsVstIString *istring)
  {
    ((Steinberg::IString *) istring)->isWideString();
  }
  
}
