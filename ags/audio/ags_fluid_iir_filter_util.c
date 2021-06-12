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
 *
 * FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#include <ags/audio/ags_fluid_iir_filter_util.h>

#include <ags/audio/ags_fluid_util.h>

#include <math.h>

void ags_fluid_iir_filter_util_calculate_coefficients(AgsFluidIIRFilter *iir_filter,
						      gdouble output_rate,
						      gint transition_samples);

/**
 * SECTION:ags_fluid_iir_filter_util
 * @short_description: util functions to fluid IIR filter
 * @title: AgsFluidIIRFilterUtil
 * @section_id:
 * @include: ags/audio/ags_fluid_iir_filter_util.h
 *
 * These utility functions allow you to calc and apply fluid
 * IIR filter data.
 */

void
ags_fluid_iir_filter_util_calculate_coefficients(AgsFluidIIRFilter *iir_filter,
						 gdouble output_rate,
						 gint transition_samples)
{
  /* FLUID_IIR_Q_LINEAR may switch the filter off by setting Q==0 */
  if(iir_filter->q_lin == 0.0){
    return;
  }else{
    /*
     * Those equations from Robert Bristow-Johnson's `Cookbook
     * formulae for audio EQ biquad filter coefficients', obtained
     * from Harmony-central.com / Computer / Programming. They are
     * the result of the bilinear transform on an analogue filter
     * prototype. To quote, `BLT frequency warping has been taken
     * into account for both significant frequency relocation and for
     * bandwidth readjustment'. */

    gdouble omega = (gdouble)(2.0 * M_PI) *
      (iir_filter->last_fres / output_rate);
    gdouble sin_coeff = sin(omega);
    gdouble cos_coeff = cos(omega);
    gdouble alpha_coeff = sin_coeff / (2.0f * iir_filter->q_lin);
    gdouble a0_inv = 1.0f / (1.0f + alpha_coeff);

    /* Calculate the filter coefficients. All coefficients are
     * normalized by a0. Think of `a1' as `a1/a0'.
     *
     * Here a couple of multiplications are saved by reusing common expressions.
     * The original equations should be:
     *  b0=(1.-cos_coeff)*a0_inv*0.5*filter_gain;
     *  b1=(1.-cos_coeff)*a0_inv*filter_gain;
     *  b2=(1.-cos_coeff)*a0_inv*0.5*filter_gain; */

    /* "a" coeffs are same for all 3 available filter types */
    gdouble a1_temp = -2.0f * cos_coeff * a0_inv;
    gdouble a2_temp = (1.0f - alpha_coeff) * a0_inv;

    gdouble b02_temp, b1_temp;

    switch(iir_filter->filter_type){
    case AGS_FLUID_IIR_HIGHPASS:
    {
      b1_temp = (1.0f + cos_coeff) * a0_inv * iir_filter->filter_gain;
      
      /* both b0 -and- b2 */
      b02_temp = b1_temp * 0.5f;
      
      b1_temp *= -1.0f;
    }
    break;
    case AGS_FLUID_IIR_LOWPASS:
    {
      b1_temp = (1.0f - cos_coeff) * a0_inv * iir_filter->filter_gain;

      /* both b0 -and- b2 */
      b02_temp = b1_temp * 0.5f;
    }
    break;
    default:
      /* filter disabled, should never get here */
      return;
    }

    iir_filter->compensate_incr = 0;
    
    if(iir_filter->filter_startup || (transition_samples == 0)){
      /* The filter is calculated, because the voice was started up.
       * In this case set the filter coefficients without delay.
       */
      iir_filter->a1 = a1_temp;

      iir_filter->a2 = a2_temp;

      iir_filter->b02 = b02_temp;

      iir_filter->b1 = b1_temp;

      iir_filter->filter_coeff_incr_count = 0;
    }else{
      /* The filter frequency is changed.  Calculate an increment
       * factor, so that the new setting is reached after one buffer
       * length. x_incr is added to the current value FLUID_BUFSIZE
       * times. The length is arbitrarily chosen. Longer than one
       * buffer will sacrifice some performance, though.  Note: If
       * the filter is still too 'grainy', then increase this number
       * at will.
       */

      iir_filter->a1_incr = (a1_temp - iir_filter->a1) / transition_samples;

      iir_filter->a2_incr = (a2_temp - iir_filter->a2) / transition_samples;

      iir_filter->b02_incr = (b02_temp - iir_filter->b02) / transition_samples;

      iir_filter->b1_incr = (b1_temp - iir_filter->b1) / transition_samples;
      
      if(fabs(iir_filter->b02) > 0.0001f){
	gdouble quota;

	quota = b02_temp / iir_filter->b02;

	iir_filter->compensate_incr = quota < 0.5f || quota > 2.f;
      }

      /* Have to add the increments filter_coeff_incr_count times. */
      iir_filter->filter_coeff_incr_count = transition_samples;
    }
  }
}

/**
 * ags_fluid_iir_filter_util_calc:
 * @output_rate: the output rate
 * @fres_mod: the fres mod
 * 
 * Calc fluid IIR filter.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_iir_filter_util_calc(AgsFluidIIRFilter *iir_filter,
			       gdouble output_rate,
			       gdouble fres_mod,
			       gint transition_samples)
{
  gdouble tmp_fres;
  
  /* calculate the frequency of the resonant filter in Hz */
  tmp_fres = ags_fluid_ct2hz(iir_filter->fres + fres_mod);

  /* FIXME - Still potential for a click during turn on, can we interpolate
     between 20khz cutoff and 0 Q? */

  /* I removed the optimization of turning the filter off when the
   * resonance frequence is above the maximum frequency. Instead, the
   * filter frequency is set to a maximum of 0.45 times the sampling
   * rate. For a 44100 kHz sampling rate, this amounts to 19845
   * Hz. The reason is that there were problems with anti-aliasing when the
   * synthesizer was run at lower sampling rates. Thanks to Stephan
   * Tassart for pointing me to this bug. By turning the filter on and
   * clipping the maximum filter frequency at 0.45*srate, the filter
   * is used as an anti-aliasing filter. */

  if(tmp_fres > 0.45f * output_rate){
    tmp_fres = 0.45f * output_rate;
  }else if(iir_filter->fres < 5.f){
    tmp_fres = 5.f;
  }

  /* if there is a significant frequency change.. */
  if(fabs(tmp_fres - iir_filter->last_fres) > 0.01f){
    ags_fluid_iir_filter_util_calculate_coefficients(iir_filter,
						     output_rate,
						     transition_samples);
  }
}

/**
 * ags_fluid_iir_filter_util_apply_s8:
 * @iir_filter: the #AgsFluidIIRFilter-struct containing filter parameters
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * 
 * Perform fluid IIR filter on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_iir_filter_util_apply_s8(AgsFluidIIRFilter *iir_filter,
				   gint8 *destination,
				   gint8 *source,
				   guint buffer_length)
{
  if(iir_filter->filter_type == AGS_FLUID_IIR_DISABLED || iir_filter->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = iir_filter->hist1;
    dsp_hist2 = iir_filter->hist2;
    
    dsp_a1 = iir_filter->a1;
    dsp_a2 = iir_filter->a2;
    dsp_b02 = iir_filter->b02;
    dsp_b1 = iir_filter->b1;

    dsp_filter_coeff_incr_count = iir_filter->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = iir_filter->a1_incr;
      dsp_a2_incr = iir_filter->a2_incr;
      dsp_b02_incr = iir_filter->b02_incr;
      dsp_b1_incr = iir_filter->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(iir_filter->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    iir_filter->hist1 = dsp_hist1;
    iir_filter->hist2 = dsp_hist2;
    
    iir_filter->a1 = dsp_a1;
    iir_filter->a2 = dsp_a2;
    iir_filter->b02 = dsp_b02;
    iir_filter->b1 = dsp_b1;
    
    iir_filter->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_apply_s16:
 * @iir_filter: the #AgsFluidIIRFilter-struct containing filter parameters
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * 
 * Perform fluid IIR filter on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_iir_filter_util_apply_s16(AgsFluidIIRFilter *iir_filter,
				    gint16 *destination,
				    gint16 *source,
				    guint buffer_length)
{
  if(iir_filter->filter_type == AGS_FLUID_IIR_DISABLED || iir_filter->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = iir_filter->hist1;
    dsp_hist2 = iir_filter->hist2;
    
    dsp_a1 = iir_filter->a1;
    dsp_a2 = iir_filter->a2;
    dsp_b02 = iir_filter->b02;
    dsp_b1 = iir_filter->b1;

    dsp_filter_coeff_incr_count = iir_filter->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = iir_filter->a1_incr;
      dsp_a2_incr = iir_filter->a2_incr;
      dsp_b02_incr = iir_filter->b02_incr;
      dsp_b1_incr = iir_filter->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(iir_filter->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    iir_filter->hist1 = dsp_hist1;
    iir_filter->hist2 = dsp_hist2;
    
    iir_filter->a1 = dsp_a1;
    iir_filter->a2 = dsp_a2;
    iir_filter->b02 = dsp_b02;
    iir_filter->b1 = dsp_b1;
    
    iir_filter->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_apply_s24:
 * @iir_filter: the #AgsFluidIIRFilter-struct containing filter parameters
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * 
 * Perform fluid IIR filter on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_iir_filter_util_apply_s24(AgsFluidIIRFilter *iir_filter,
				    gint32 *destination,
				    gint32 *source,
				    guint buffer_length)
{
  if(iir_filter->filter_type == AGS_FLUID_IIR_DISABLED || iir_filter->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = iir_filter->hist1;
    dsp_hist2 = iir_filter->hist2;
    
    dsp_a1 = iir_filter->a1;
    dsp_a2 = iir_filter->a2;
    dsp_b02 = iir_filter->b02;
    dsp_b1 = iir_filter->b1;

    dsp_filter_coeff_incr_count = iir_filter->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = iir_filter->a1_incr;
      dsp_a2_incr = iir_filter->a2_incr;
      dsp_b02_incr = iir_filter->b02_incr;
      dsp_b1_incr = iir_filter->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(iir_filter->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    iir_filter->hist1 = dsp_hist1;
    iir_filter->hist2 = dsp_hist2;
    
    iir_filter->a1 = dsp_a1;
    iir_filter->a2 = dsp_a2;
    iir_filter->b02 = dsp_b02;
    iir_filter->b1 = dsp_b1;
    
    iir_filter->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_apply_s32:
 * @iir_filter: the #AgsFluidIIRFilter-struct containing filter parameters
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * 
 * Perform fluid IIR filter on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_iir_filter_util_apply_s32(AgsFluidIIRFilter *iir_filter,
				    gint32 *destination,
				    gint32 *source,
				    guint buffer_length)
{
  if(iir_filter->filter_type == AGS_FLUID_IIR_DISABLED || iir_filter->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = iir_filter->hist1;
    dsp_hist2 = iir_filter->hist2;
    
    dsp_a1 = iir_filter->a1;
    dsp_a2 = iir_filter->a2;
    dsp_b02 = iir_filter->b02;
    dsp_b1 = iir_filter->b1;

    dsp_filter_coeff_incr_count = iir_filter->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = iir_filter->a1_incr;
      dsp_a2_incr = iir_filter->a2_incr;
      dsp_b02_incr = iir_filter->b02_incr;
      dsp_b1_incr = iir_filter->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(iir_filter->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    iir_filter->hist1 = dsp_hist1;
    iir_filter->hist2 = dsp_hist2;
    
    iir_filter->a1 = dsp_a1;
    iir_filter->a2 = dsp_a2;
    iir_filter->b02 = dsp_b02;
    iir_filter->b1 = dsp_b1;
    
    iir_filter->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_apply_s64:
 * @iir_filter: the #AgsFluidIIRFilter-struct containing filter parameters
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * 
 * Perform fluid IIR filter on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_iir_filter_util_apply_s64(AgsFluidIIRFilter *iir_filter,
				    gint64 *destination,
				    gint64 *source,
				    guint buffer_length)
{
  if(iir_filter->filter_type == AGS_FLUID_IIR_DISABLED || iir_filter->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = iir_filter->hist1;
    dsp_hist2 = iir_filter->hist2;
    
    dsp_a1 = iir_filter->a1;
    dsp_a2 = iir_filter->a2;
    dsp_b02 = iir_filter->b02;
    dsp_b1 = iir_filter->b1;

    dsp_filter_coeff_incr_count = iir_filter->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = iir_filter->a1_incr;
      dsp_a2_incr = iir_filter->a2_incr;
      dsp_b02_incr = iir_filter->b02_incr;
      dsp_b1_incr = iir_filter->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(iir_filter->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    iir_filter->hist1 = dsp_hist1;
    iir_filter->hist2 = dsp_hist2;
    
    iir_filter->a1 = dsp_a1;
    iir_filter->a2 = dsp_a2;
    iir_filter->b02 = dsp_b02;
    iir_filter->b1 = dsp_b1;
    
    iir_filter->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_apply_float:
 * @iir_filter: the #AgsFluidIIRFilter-struct containing filter parameters
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * 
 * Perform fluid IIR filter on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_iir_filter_util_apply_float(AgsFluidIIRFilter *iir_filter,
				      gfloat *destination,
				      gfloat *source,
				      guint buffer_length)
{
  if(iir_filter->filter_type == AGS_FLUID_IIR_DISABLED || iir_filter->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = iir_filter->hist1;
    dsp_hist2 = iir_filter->hist2;
    
    dsp_a1 = iir_filter->a1;
    dsp_a2 = iir_filter->a2;
    dsp_b02 = iir_filter->b02;
    dsp_b1 = iir_filter->b1;

    dsp_filter_coeff_incr_count = iir_filter->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = iir_filter->a1_incr;
      dsp_a2_incr = iir_filter->a2_incr;
      dsp_b02_incr = iir_filter->b02_incr;
      dsp_b1_incr = iir_filter->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(iir_filter->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    iir_filter->hist1 = dsp_hist1;
    iir_filter->hist2 = dsp_hist2;
    
    iir_filter->a1 = dsp_a1;
    iir_filter->a2 = dsp_a2;
    iir_filter->b02 = dsp_b02;
    iir_filter->b1 = dsp_b1;
    
    iir_filter->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_apply_double:
 * @iir_filter: the #AgsFluidIIRFilter-struct containing filter parameters
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * 
 * Perform fluid IIR filter on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_iir_filter_util_apply_double(AgsFluidIIRFilter *iir_filter,
				       gdouble *destination,
				       gdouble *source,
				       guint buffer_length)
{
  if(iir_filter->filter_type == AGS_FLUID_IIR_DISABLED || iir_filter->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = iir_filter->hist1;
    dsp_hist2 = iir_filter->hist2;
    
    dsp_a1 = iir_filter->a1;
    dsp_a2 = iir_filter->a2;
    dsp_b02 = iir_filter->b02;
    dsp_b1 = iir_filter->b1;

    dsp_filter_coeff_incr_count = iir_filter->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = iir_filter->a1_incr;
      dsp_a2_incr = iir_filter->a2_incr;
      dsp_b02_incr = iir_filter->b02_incr;
      dsp_b1_incr = iir_filter->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(iir_filter->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = source[dsp_i] - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	destination[dsp_i] = dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1;

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    iir_filter->hist1 = dsp_hist1;
    iir_filter->hist2 = dsp_hist2;
    
    iir_filter->a1 = dsp_a1;
    iir_filter->a2 = dsp_a2;
    iir_filter->b02 = dsp_b02;
    iir_filter->b1 = dsp_b1;
    
    iir_filter->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}

/**
 * ags_fluid_iir_filter_util_apply_complex:
 * @iir_filter: the #AgsFluidIIRFilter-struct containing filter parameters
 * @destination: the destination audio buffer
 * @source: the source audio buffer
 * @buffer_length: the buffer length
 * 
 * Perform fluid IIR filter on @buffer and return the result in @output_buffer.
 * 
 * Since: 3.8.12
 */
void
ags_fluid_iir_filter_util_apply_complex(AgsFluidIIRFilter *iir_filter,
					AgsComplex *destination,
					AgsComplex *source,
					guint buffer_length)
{
  if(iir_filter->filter_type == AGS_FLUID_IIR_DISABLED || iir_filter->q_lin == 0.0){
    return;
  }else{
    /* IIR filter sample history */
    gdouble dsp_hist1;
    gdouble dsp_hist2;
    
    /* IIR filter coefficients */
    gdouble dsp_a1;
    gdouble dsp_a2;
    gdouble dsp_b02;
    gdouble dsp_b1;
    gint dsp_filter_coeff_incr_count;
      
    gdouble dsp_centernode;
    gint dsp_i;
      
    /* filter (implement the voice filter according to SoundFont standard) */
    dsp_hist1 = iir_filter->hist1;
    dsp_hist2 = iir_filter->hist2;
    
    dsp_a1 = iir_filter->a1;
    dsp_a2 = iir_filter->a2;
    dsp_b02 = iir_filter->b02;
    dsp_b1 = iir_filter->b1;

    dsp_filter_coeff_incr_count = iir_filter->filter_coeff_incr_count;
    
    /* Two versions of the filter loop. One, while the filter is
     * changing towards its new setting. The other, if the filter
     * doesn't change.
     */

    if(dsp_filter_coeff_incr_count > 0){
      gdouble dsp_a1_incr;
      gdouble dsp_a2_incr;
      gdouble dsp_b02_incr;
      gdouble dsp_b1_incr;
      
      dsp_a1_incr = iir_filter->a1_incr;
      dsp_a2_incr = iir_filter->a2_incr;
      dsp_b02_incr = iir_filter->b02_incr;
      dsp_b1_incr = iir_filter->b1_incr;

      /* Increment is added to each filter coefficient filter_coeff_incr_count times. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = ags_complex_get(source + dsp_i) - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;

	ags_complex_set(destination + dsp_i,
			dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1);

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;

	if(dsp_filter_coeff_incr_count-- > 0){
	  gdouble old_b02;

	  old_b02 = dsp_b02;

	  dsp_a1 += dsp_a1_incr;
	  dsp_a2 += dsp_a2_incr;
	  dsp_b02 += dsp_b02_incr;
	  dsp_b1 += dsp_b1_incr;

	  /* Compensate history to avoid the filter going havoc with large frequency changes */
	  if(iir_filter->compensate_incr && fabs(dsp_b02) > 0.001f){
	    gdouble compensate;

	    compensate = old_b02 / dsp_b02;

	    dsp_hist1 *= compensate;
	    dsp_hist2 *= compensate;
	  }
	}
      } /* for dsp_i */
    }else{ /* The filter parameters are constant.  This is duplicated to save time. */
      for(dsp_i = 0; dsp_i < buffer_length; dsp_i++){
	/* The filter is implemented in Direct-II form. */
	dsp_centernode = ags_complex_get(source + dsp_i) - dsp_a1 * dsp_hist1 - dsp_a2 * dsp_hist2;
	
	ags_complex_set(destination + dsp_i,
			dsp_b02 * (dsp_centernode + dsp_hist2) + dsp_b1 * dsp_hist1);

	dsp_hist2 = dsp_hist1;
	dsp_hist1 = dsp_centernode;
      }
    }
    
    iir_filter->hist1 = dsp_hist1;
    iir_filter->hist2 = dsp_hist2;
    
    iir_filter->a1 = dsp_a1;
    iir_filter->a2 = dsp_a2;
    iir_filter->b02 = dsp_b02;
    iir_filter->b1 = dsp_b1;
    
    iir_filter->filter_coeff_incr_count = dsp_filter_coeff_incr_count;
  }
}
