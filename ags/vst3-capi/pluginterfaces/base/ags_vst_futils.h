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

#ifndef __AGS_VST_FUTIL_H__
#define __AGS_VST_FUTIL_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ftypes.h>

#ifdef __cplusplus
extern "C" {
#endif

  void ags_vst_util_min(void *a, void *b,
			void *retval);

  void ags_vst_util_max(void *a, void *b,
			void *retval);
  
  void ags_vst_util_abs(void *value,
			void *retval);

  void ags_vst_util_sign(void *value,
			 void *retval);

  void ags_vst_util_bound(void *minval, void *maxval, void *x,
			  void *retval);

  void ags_vst_util_swap(void *t1, void *t2);
  
  gboolean ags_vst_util_is_approximate_equal(void *t1, void *t2, void *epsilon,
					     void *retval);
  
  void ags_vst_util_to_normalized(void *value, gint32 num_steps,
				  void *retval);
  
  gint32 ags_vst_util_from_normalized(void *norm, gint32 num_steps);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_FUTIL_H__*/


