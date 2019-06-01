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

#ifndef __AGS_VST_FUNKNOWN_H__
#define __AGS_VST_FUNKNOWN_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_fplatform.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ftypes.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_smart_pointer.h>

#define AGS_VST_FUNKNOWN_IID (ags_vst_funknown_get_iid());

#ifdef __cplusplus
extern "C" {
#endif

#if AGS_VST_COM_COMPATIBLE
#if AGS_VST_SMTG_OS_WINDOWS
  enum
  {
    AGS_VST_KNO_INTERFACE	= (gint32)(0x80004002L),
    AGS_VST_KRESULT_OK		= (gint32)(0x00000000L),
    AGS_VST_KRESULT_TRUE	= AGS_VST_KRESULT_OK,
    AGS_VST_KRESULT_FALS	= (gint32)(0x00000001L),
    AGS_VST_KINVALID_ARGUMENT	= (gint32)(0x80070057L),
    AGS_VST_KNOT_IMPLEMENTED	= (gint32)(0x80004001L),
    AGS_VST_KINTERNAL_ERROR	= (gint32)(0x80004005L),
    AGS_VST_KNOT_INITIALIZED	= (gint32)(0x8000FFFFL),
    AGS_VST_KOUT_OF_MEMORY	= (gint32)(0x8007000EL),
  };
#else
  enum
  {
    AGS_VST_KNO_INTERFACE	= (gint32)(0x80000004L),
    AGS_VST_KRESULT_OK		= (gint32)(0x00000000L),
    AGS_VST_KRESULT_TRUE	= AGS_VST_KRESULT_OK,
    AGS_VST_KRESULT_FALS	= (gint32)(0x00000001L),
    AGS_VST_KINVALID_ARGUMENT	= (gint32)(0x80000003L),
    AGS_VST_KNOT_IMPLEMENTED	= (gint32)(0x80000001L),
    AGS_VST_KINTERNAL_ERROR	= (gint32)(0x80000008L),
    AGS_VST_KNOT_INITIALIZED	= (gint32)(0x8000FFFFL),
    AGS_VST_KOUT_OF_MEMORY	= (gint32)(0x80000002L),
  };
#endif
#else
  enum
  {
    AGS_VST_KNO_INTERFACE = -1,
    AGS_VST_KRESULT_OK,
    AGS_VST_KRESULT_TRUE = AGS_VST_KRESULT_OK,
    AGS_VST_KRESULT_FALSE,
    AGS_VST_KINVALID_ARGUMENT,
    AGS_VST_KNOT_IMPLEMENTED,
    AGS_VST_KINTERNAL_ERROR,
    AGS_VST_KNOT_INITIALIZED,
    AGS_VST_KOUT_OF_MEMORY,
  };
#endif

  typedef gint64 AGS_VST_LARGE_INT;

  typedef gint8 AgsVstTUID[16];
  
#if 0
  gboolean ags_vst_funknown_private_iid_equal(const void* iid1, const void* iid2);

  gint32 ags_vst_funknown_private_atomic_add(gint32 *value, gint32 amount);
#endif
  
  typedef struct AgsVstFUID FUID;

  AgsVstFUID* ags_vst_fuid_new();
  AgsVstFUID* ags_vst_fuid_new_with_iid(guint32 l1, guint32 l2, guint32 l3, guint32 l4);
  AgsVstFUID* ags_vst_fuid_new_from_fuid(AgsVstFUID *fuid);

  void ags_vst_fuid_delete(AgsVstFUID *fuid);
  
  gboolean ags_vst_fuid_generate(AgsVstFUID *fuid);
  gboolean ags_vst_fuid_is_valid(AgsVstFUID *fuid);

  AgsVstFUID* ags_vst_fuid_equal(AgsVstFUID *destination_fuid,
				 AgsVstFUID *source_fuid);
  gboolean ags_vst_fuid_equals(AgsVstFUID *fuid_a,
			       AgsVstFUID *fuid_b);
  gboolean ags_vst_fuid_smaller(AgsVstFUID *fuid_a,
				AgsVstFUID *fuid_b);
  gboolean ags_vst_fuid_not_equals(AgsVstFUID *fuid_a,
				   AgsVstFUID *fuid_b);

  guint32 ags_vst_fuid_get_long1(AgsVstFUID *fuid);
  guint32 ags_vst_fuid_get_long2(AgsVstFUID *fuid);
  guint32 ags_vst_fuid_get_long3(AgsVstFUID *fuid);
  guint32 ags_vst_fuid_get_long4(AgsVstFUID *fuid);

  void ags_vst_fuid_from_uint32(AgsVstFUID *fuid,
				guint32 d1, guint32 d2, guint32 d3, guint32 d4);
  void ags_vst_fuid_to_uint32(AgsVstFUID *fuid,
			      guint32 *d1, guint32 *d2, guint32 *d3, guint32 *d4);

  typedef gchar AgsVstString[64];

  void ags_vst_fuid_to_string(AgsVstFUID *fuid,
			      gchar *string);
  gboolean ags_vst_fuid_from_string(AgsVstFUID *fuid,
				    gchar *string);

  void ags_vst_fuid_to_registry_string(AgsVstFUID *fuid,
				       gchar *string);
  gboolean ags_vst_fuid_from_registry_string(AgsVstFUID *fuid,
					     gchar *string);

  enum AgsVstUIDPrintStyle
  {
    AGS_VST_KINLINE_UID,
    AGS_VST_KDECLARE_UID,
    AGS_VST_KFUID,
    AGS_VST_KCLASS_UID,
  };

  void ags_vst_fuid_print(AgsVstFUID *fuid,
			  gchar *string, gint32 style);

  void ags_vst_fuid_to_tuid_with_result(AgsVstFUID *fuid,
					AgsVstTUID *result);
  AgsVstTUID* ags_vst_fuid_to_tuid(AgsVstFUID *fuid);

  AgsVstFUID* ags_vst_fuid_from_tuid(AgsVstFUID *fuid,
				     AgsVstTUID *tuid);
  
  typedef struct AgsVstFUnknown FUnknown;

  const AgsVstTUID* ags_vst_funknown_get_iid();

  gint32 ags_vst_funknown_query_interface(AgsVstFUnknown *funknown,
					  AgsVstTUID *_iid, void **obj);

  guint32 ags_vst_funknown_add_ref(AgsVstFUnknown *funknown);

  guint32 ags_vst_funknown_release(AgsVstFUnknown *funknown);
  
  typedef struct AgsVstI I;
  typedef struct AgsVstFUnknownPtr FUnknownPtr;

#if 0  
  AgsVstFUnknownPtr* ags_vst_funknown_ptr_new_from_funknown(AgsVstFUnknown *funknown,
							    AgsVstTUID *tuid);
  
  AgsVstI*  ags_funknown_ptr_get_interface(AgsVstFUnknownPtr *funknown_ptr,
					   AgsVstTUID *tuid);

  void ags_vst_funknown_ptr_set_funknown(AgsVstFUnknownPtr *funknown_ptr,
					 AgsVstFUnknown *funknown);
  void ags_vst_funknown_ptr_set_funknown_interface(AgsVstFUnknownPtr *funknown_ptr,
						   AgsVstFUnknown *funknown, AgsVstI *vst_interface);
#endif
  
  typedef struct AgsVstFReleaser FReleaser;
  
  AgsVstFReleaser* ags_freleaser_alloc(AgsVstFUnknown *funknown);
  void ags_freleaser_free(AgsVstFReleaser *freleaser);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_FUNKNOWN_H__*/

