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

#ifndef __AGS_VST_TYPES_H__
#define __AGS_VST_TYPES_H__

#include <glib.h>

#include <ags/ags_api_config.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_fstrdefs.h>

#ifndef AGS_VST_VERSION_STRING
#define AGS_VST_VERSION_STRING "VST 3.6.13"
#endif

#define AGS_VST_VERSION_MAJOR (3)
#define AGS_VST_VERSION_MINOR (6)
#define AGS_VST_VERSION_SUB (13)

#define AGS_VST_VERSION ((AGS_VST_VERSION_MAJOR << 16) | (AGS_VST_VERSION_MINOR << 8) | AGS_VST_VERSION_SUB)

#ifdef __cplusplus
extern "C" {
#endif

  typedef gunichar2 AgsVstTChar;
  typedef AgsVstTChar AgsVstString128[128];
  
  typedef gint32 AgsVstMediaType;
  typedef gint32 AgsVstBusDirection;
  typedef gint32 AgsVstBusType;
  typedef gint32 AgsVstIoMode;
  typedef gint32 AgsVstUnitID;
  typedef gdouble AgsVstParamValue;
  typedef guint32 AgsVstParamID;
  typedef gint32 AgsVstProgramListID;
  typedef gint16 AgsVstCtrlNumber;

  typedef gdouble AgsVstTQuarterNotes;
  typedef gint64 AgsVstTSamples;

  typedef guint32 AgsVstColorSpec;

  static const AgsVstParamID ags_vst_no_param_id = 0xffffffff;

  typedef gfloat AgsVstSample32;
  typedef gdouble AgsVstSample64;
  typedef gdouble AgsVstSampleRate;

#if defined(AGS_OSXAPI)
  typedef unsigned long long AgsVstSpeakerArrangement;
#else
  typedef long unsigned int AgsVstSpeakerArrangement;
#endif
  
  typedef guint64 AgsVstSpeaker;
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_TYPES_H__*/
