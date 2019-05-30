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

#include <ags/vst3-capi/pluginterfaces/base/fplatform.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ftypes.h>
#include <ags/vst3-capi/pluginterfaces/base/smartpointer.h>

#define AGS_VST_FUNKNOWN_IID (ags_vst_funknown_get_iid());

#ifdef __cplusplus
extern "C" {
#endif

#if AGS_VST_COM_COMPATIBLE
#if AGS_VST_SMTG_OS_WINDOWS
  enum
  {
    AGS_VST_KNO_INTERFACE	= (gint32)(0x80004002L),	// E_NOINTERFACE
    AGS_VST_KRESULT_OK		= (gint32)(0x00000000L),	// S_OK
    AGS_VST_KRESULT_TRUE	= AGS_VST_KRESULT_OK,
    AGS_VST_KRESULT_FALS	= (gint32)(0x00000001L),	// S_FALSE
    AGS_VST_KINVALID_ARGUMENT	= (gint32)(0x80070057L),	// E_INVALIDARG
    AGS_VST_KNOT_IMPLEMENTED	= (gint32)(0x80004001L),	// E_NOTIMPL
    AGS_VST_KINTERNAL_ERROR	= (gint32)(0x80004005L),	// E_FAIL
    AGS_VST_KNOT_INITIALIZED	= (gint32)(0x8000FFFFL),	// E_UNEXPECTED
    AGS_VST_KOUT_OF_MEMORY	= (gint32)(0x8007000EL),	// E_OUTOFMEMORY
  };
#else
  enum
  {
    AGS_VST_KNO_INTERFACE	= (gint32)(0x80000004L),	// E_NOINTERFACE
    AGS_VST_KRESULT_OK		= (gint32)(0x00000000L),	// S_OK
    AGS_VST_KRESULT_TRUE	= AGS_VST_KRESULT_OK,
    AGS_VST_KRESULT_FALS	= (gint32)(0x00000001L),	// S_FALSE
    AGS_VST_KINVALID_ARGUMENT	= (gint32)(0x80000003L),	// E_INVALIDARG
    AGS_VST_KNOT_IMPLEMENTED	= (gint32)(0x80000001L),	// E_NOTIMPL
    AGS_VST_KINTERNAL_ERROR	= (gint32)(0x80000008L),	// E_FAIL
    AGS_VST_KNOT_INITIALIZED	= (gint32)(0x8000FFFFL),	// E_UNEXPECTED
    AGS_VST_KOUT_OF_MEMORY	= (gint32)(0x80000002L),	// E_OUTOFMEMORY
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

  typedef gint64 AGS_VST_LARGE_INT; // obsolete

  typedef gint8 AgsVstTUID[16]; ///< plain UID type
  
  gboolean ags_vst_funknown_private_iid_equal(const void* iid1, const void* iid2);

  gint32 ags_vst_funknown_private_atomic_add(gint32 *value, gint32 amount);

  typedef struct AgsVstFUID FUID;

  AgsVstFUID* ags_vst_fuid_new();
  AgsVstFUID* ags_vst_fuid_new_with_iid(uint32 l1, uint32 l2, uint32 l3, uint32 l4);
  AgsVstFUID* ags_vst_fuid_new_from_fuid(AgsVstFUID *fuid);

  void ags_vst_fuid_delete(AgsVstFUID *fuid);
  
  gboolean ags_vst_fuid_generate(AgsVstFUID *fuid);
  gboolean ags_vst_fuid_is_valid(AgsVstFUID *fuid);

  AgsVstFUID* ags_vst_fuid_equal(AgsVstFUID *destination_fuid,
				 AgsVstFUID *source_fuid);
  void ags_vst_fuid_equals(AgsVstFUID *fuid_a,
			   AgsVstFUID *fuid_b);
  void ags_vst_fuid_smaller(AgsVstFUID *fuid_a,
			    AgsVstFUID *fuid_b);
  void ags_vst_fuid_not_equals(AgsVstFUID *fuid_a,
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

  void ags_vst_fuid_to_tuid(AgsVstFUID *fuid,
			    AgsVstTUID result);
  AgsVstTUID ags_vst_tuid_get_data(AgsVstFUID *fuid,
				   AgsVstTUID tuid);
  AgsVstTUID ags_vst_fuid_to_tuid(AgsVstFUID *fuid,
				  AgsVstTUID tuid);

  AgsVstFUID* ags_vst_fuid_from_tuid(AgsVstFUID *fuid,
				     AgsVstTUID uid);
  
  typedef struct AgsVstFUnknown FUnknown;

  AgsVstTUID ags_vst_funknown_get_iid();

  gint32 ags_vst_funknown_query_interface(AgsVstFUnknown *funknown,
					  TUID _iid, void** obj);

  guint32 ags_vst_funknown_add_ref(AgsVstFUnknown *funknown);

  guint32 ags_vst_funknown_release(AgsVstFUnknown *funknown);
  
  typedef struct AgsVstI I;
  typedef struct AgsVstFUnknownPtr FUnknownPtr;

  AgsVstFunknownPtr* ags_vst_funknown_ptr_new();
  AgsVstFunknownPtr* ags_vst_funknown_ptr_new_from_template(AgsVstFUnknownPtr *funknown_ptr);
  AgsVstFunknownPtr* ags_vst_funknown_ptr_new_from_funknown(AgsVstFUnknown *funknown);
  
  void ags_funknown_get_interface(AgsVstFUnknownPtr *funknown_ptr,
				  AgsVstI *vst_interface);

  void ags_vst_funknown_ptr_set_funknown(AgsVstFunknownPtr *funknown_ptr,
					 AgsVstFUnknown *funknown);
  void ags_vst_funknown_ptr_set_funknown_interface(AgsVstFunknownPtr *funknown_ptr,
						   AgsVstFUnknown *funknown, AgsVstI *vst_interface);

  typedef struct AgsVstFReleaser FReleaser;
  
  AgsVstFReleaser* ags_freleaser_alloc(AgsVstFunknown *funknown);
  void ags_freleaser_free(AgsVstFReleaser *freleaser);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_FUNKNOWN_H__*/

