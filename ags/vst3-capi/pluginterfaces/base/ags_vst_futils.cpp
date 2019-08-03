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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_futils.h>

#include <pluginterfaces/base/futils.h>

extern "C" {

  void ags_vst_util_min_int32(gint32 a, gint32 b,
			      gint32 *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::Min(a, b);
    }
  }
  
  void ags_vst_util_min_float(gfloat a, gfloat b,
			      gfloat *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::Min(a, b);
    }
  }  

  void ags_vst_util_max_int32(gint32 a, gint32 b,
			      gint32 *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::Max(a, b);
    }
  }
  
  void ags_vst_util_max_float(gfloat a, gfloat b,
			      gfloat *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::Max(a, b);
    }
  }  
  
  void ags_vst_util_abs_int32(gint32 value,
			      gint32 *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::Abs(value);
    }
  }
  
  void ags_vst_util_abs_float(gfloat value,
			      gfloat *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::Abs(value);
    }
  }  

  void ags_vst_util_sign_int32(gint32 value,
			       gint32 *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::Sign(value);
    }
  }
  
  void ags_vst_util_sign_float(gfloat value,
			       gfloat *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::Sign(value);
    }
  }  

  void ags_vst_util_bound_int32(gint32 minval, gint32 maxval, gint32 x,
				gint32 *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::Bound(minval, maxval, x);
    }
  }
  
  void ags_vst_util_bound_float(gfloat minval, gfloat maxval, gfloat x,
				gfloat *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::Bound(minval, maxval, x);
    }
  }
  
  void ags_vst_util_swap_int32(gint32 *t1, gint32 *t2)
  {
    Steinberg::Swap(t1, t2);
  }
  
  void ags_vst_util_swap_float(gfloat *t1, gfloat *t2)
  {
    Steinberg::Swap(t1, t2);
  }  
  
  gboolean ags_vst_util_is_approximate_equal_int32(gint32 t1, gint32 t2, gint32 epsilon)
  {
    return(Steinberg::IsApproximateEqual(t1, t2, epsilon));
  }
  
  gboolean ags_vst_util_is_approximate_equal_float(gfloat t1, gfloat t2, gfloat epsilon)
  {
    return(Steinberg::IsApproximateEqual(t1, t2, epsilon));
  }  
  
  void ags_vst_util_to_normalized_int32(gint32 value, gint32 num_steps,
					gint32 *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::ToNormalized(value, num_steps);
    }
  }
  
  void ags_vst_util_to_normalized_float(gfloat value, gfloat num_steps,
					gfloat *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::ToNormalized(value, num_steps);
    }
  }  
  
  void ags_vst_util_from_normalized_int32(gint32 norm, gint32 num_steps,
					  gint32 *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::FromNormalized(norm, num_steps);
    }
  }
  
  void ags_vst_util_from_normalized_float(gfloat norm, gfloat num_steps,
					  gfloat *retval)
  {
    if(retval != NULL){
      retval[0] = Steinberg::FromNormalized(norm, num_steps);
    }
  }
  
}
