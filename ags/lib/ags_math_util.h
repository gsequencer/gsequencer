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

#ifndef __AGS_MATH_UTIL__
#define __AGS_MATH_UTIL__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_MATH_UTIL         (ags_math_util_get_type())

#define AGS_SYMBOLIC_EULER "ℯ"
#define AGS_SYMBOLIC_PI "𝜋"
#define AGS_SYMBOLIC_INFINIT "∞"
#define AGS_SYMBOLIC_COMPLEX_UNIT "𝑖"

#define AGS_SUBSCRIPT_0 "₀"
#define AGS_SUBSCRIPT_1 "₁"
#define AGS_SUBSCRIPT_2 "₂"
#define AGS_SUBSCRIPT_3 "₃"
#define AGS_SUBSCRIPT_4 "₄"
#define AGS_SUBSCRIPT_5 "₅"
#define AGS_SUBSCRIPT_6 "₆"
#define AGS_SUBSCRIPT_7 "₇"
#define AGS_SUBSCRIPT_8 "₈"
#define AGS_SUBSCRIPT_9 "₉"

typedef struct _AgsMathUtil AgsMathUtil;

struct _AgsMathUtil
{
  //empty
};

GType ags_math_util_get_type(void);

void ags_math_util_find_parenthesis_all(gchar *str,
					gint **open_position, gint **close_position,
					guint *open_position_count, guint *close_position_count);
void ags_math_util_find_exponent_parenthesis(gchar *str,
					     gint **exponent_open_position, gint **exponent_close_position,
					     guint *exponent_open_position_count, guint *exponent_close_position_count);
void ags_math_util_find_function_parenthesis(gchar *str,
					     gint **function_open_position, gint **function_close_position,
					     guint *function_open_position_count, guint *function_close_position_count);
void ags_math_util_find_term_parenthesis(gchar *str,
					 gint **term_open_position, gint **term_close_position,
					 guint *term_open_position_count, guint *term_close_position_count);

gboolean ags_math_util_match_sign(gchar *offset,
				  gchar *end_ptr,
				  gchar **start_offset, gchar **end_offset);
gboolean ags_math_util_match_coefficient(gchar *offset,
					 gchar *end_ptr,
					 gchar **start_offset, gchar **end_offset);
gboolean ags_math_util_match_symbol(gchar *offset,
				    gchar *end_ptr,
				    gchar **start_offset, gchar **end_offset);
gboolean ags_math_util_match_exponent(gchar *offset,
				      gchar *end_ptr,
				      gchar **start_offset, gchar **end_offset);
gboolean ags_math_util_match_operator(gchar *offset,
				      gchar *end_ptr,
				      gchar **start_offset, gchar **end_offset);
gboolean ags_math_util_match_function(gchar *offset,
				      gchar *end_ptr,
				      gchar **start_offset, gchar **end_offset);

gboolean ags_math_util_coefficient_to_complex(gchar *coefficient,
					      AgsComplex *value);

AgsComplex* ags_math_util_multiply_coefficient_all(gchar **coefficient,
						   guint *value_count);

gchar* ags_math_util_find_function(gchar *str);

gchar* ags_math_util_find_symbol(gchar *str);
gchar** ags_math_util_find_symbol_all(gchar *str);

gboolean ags_math_util_is_term(gchar *term);

void ags_math_util_split_polynomial(gchar *polynomial,
				    gchar ***factor, gchar ***factor_exponent);
void ags_math_util_split_sum(gchar *sum,
			     gchar ***summand);

G_END_DECLS

#endif /*__AGS_MATH_UTIL__*/
