/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_const_string_table.h>

#include <pluginterfaces/base/conststringtable.h>

using namespace Steinberg;

extern "C" {

  /**
   * Const string table get string literal.
   *
   * @param const_string_table the const string table
   * @param str the string
   * @return the unicode string
   *
   * @since 5.0.0
   */
  gunichar2* ags_vst_const_string_table_get_string_literal(AgsVstConstStringTable *const_string_table, gchar *str)
  {
    const Steinberg::char16 *tmp_str;

    tmp_str = ((Steinberg::ConstStringTable *) const_string_table)->getString(str);

    return((gunichar2 *) tmp_str);
  }
  
  /**
   * Const string table get string character.
   *
   * @param const_string_table the const string table
   * @param str the char
   * @return the unicode char
   *
   * @since 5.0.0
   */
  gunichar2 ags_vst_const_string_table_get_string_character(AgsVstConstStringTable *const_string_table, gchar str)
  {
    Steinberg::char16 tmp_str;

    tmp_str = ((Steinberg::ConstStringTable *) const_string_table)->getString(str);

    return((gunichar2) tmp_str);
  }
  
}
