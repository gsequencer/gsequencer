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

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_solver_polynomial_test_init_suite();
int ags_solver_polynomial_test_clean_suite();

void ags_solver_polynomial_test_set_property();
void ags_solver_polynomial_test_get_property();
void ags_solver_polynomial_test_set_polynomial();
void ags_solver_polynomial_test_get_polynomial();
void ags_solver_polynomial_test_set_coefficient();
void ags_solver_polynomial_test_get_coefficient();
void ags_solver_polynomial_test_get_symbol();
void ags_solver_polynomial_test_get_exponent();
void ags_solver_polynomial_test_set_coefficient_value();
void ags_solver_polynomial_test_get_coefficient_value();
void ags_solver_polynomial_test_get_exponent_value();
void ags_solver_polynomial_test_update();
void ags_solver_polynomial_test_parse();
void ags_solver_polynomial_test_add();
void ags_solver_polynomial_test_subtract();
void ags_solver_polynomial_test_multiply();
void ags_solver_polynomial_test_divide();
void ags_solver_polynomial_test_raise_power();
void ags_solver_polynomial_test_extract_root();

#define AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_POLYNOMIAL "-2.0a^(8)b"
#define AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_COEFFICIENT "-2.0"
#define AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_COEFFICIENT_VALUE (-2.0)
#define AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_EXPONENT_VALUE_0 (1)
#define AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_EXPONENT_VALUE_1 (8)
#define AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_EXPONENT_VALUE_2 (1)

#define AGS_SOLVER_POLYNOMIAL_TEST_PARSE_TERM_0 "-3.14"
#define AGS_SOLVER_POLYNOMIAL_TEST_PARSE_TERM_1 "ab"
#define AGS_SOLVER_POLYNOMIAL_TEST_PARSE_TERM_2 "2.78a^(8)b^(2)c"

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_solver_polynomial_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_solver_polynomial_test_clean_suite()
{
  return(0);
}

void
ags_solver_polynomial_test_set_property()
{
  AgsSolverPolynomial *solver_polynomial;

  AgsComplex *coefficient_value;
  
  gchar *polynomial;
  gchar *coefficient;

  double _Complex z;
  
  solver_polynomial = ags_solver_polynomial_new();

  /* polynomial */
  polynomial = g_strdup_printf("-2%sa^(%s)",
			       AGS_SYMBOLIC_PI,
			       AGS_SYMBOLIC_EULER);

  g_object_set(solver_polynomial,
	       "polynomial", polynomial,
	       NULL);

  CU_ASSERT(!g_ascii_strcasecmp(solver_polynomial->polynomial,
				polynomial));

  /* coefficient */
  coefficient = g_strdup_printf("-2%s",
				AGS_SYMBOLIC_PI);
  
  g_object_set(solver_polynomial,
	       "coefficient", coefficient,
	       NULL);

  CU_ASSERT(!g_ascii_strcasecmp(solver_polynomial->coefficient,
				coefficient));

  /* symbol */
  g_object_set(solver_polynomial,
	       "symbol", "a",
	       NULL);
  
  CU_ASSERT(g_strv_contains(solver_polynomial->symbol, "a"));

  /* coefficient value */
  coefficient_value = g_new(AgsComplex,
			    1);

  z = 1.0 + I * 0.25;
  
  ags_complex_set(coefficient_value,
		  z);

  g_object_set(solver_polynomial,
	       "coefficient-value", coefficient_value,
	       NULL);

  CU_ASSERT(ags_complex_get(solver_polynomial->coefficient_value) == z);
}

void
ags_solver_polynomial_test_get_property()
{
  AgsSolverPolynomial *solver_polynomial;

  AgsComplex *coefficient_value;
  AgsComplex *exponent_value;
  
  gchar **symbol;
  gchar **exponent;
  
  gchar *polynomial;
  gchar *coefficient;
  
  solver_polynomial = ags_solver_polynomial_new();

  /* polynomial */
  solver_polynomial->polynomial = g_strdup(AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_POLYNOMIAL);

  polynomial = NULL;
  
  g_object_get(solver_polynomial,
	       "polynomial", &polynomial,
	       NULL);

  CU_ASSERT(polynomial != NULL && !g_strcmp0(polynomial, AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_POLYNOMIAL));

  /* coefficient */
  solver_polynomial->coefficient = g_strdup(AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_COEFFICIENT);

  coefficient = NULL;
  
  g_object_get(solver_polynomial,
	       "coefficient", &coefficient,
	       NULL);

  CU_ASSERT(coefficient != NULL && !g_strcmp0(coefficient, AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_COEFFICIENT));

  /* symbol */
  solver_polynomial->symbol = (gchar **) g_malloc(3 * sizeof(gchar *));

  solver_polynomial->symbol[0] = "a";
  solver_polynomial->symbol[1] = "b";
  solver_polynomial->symbol[2] = NULL;

  symbol = NULL;
  
  g_object_get(solver_polynomial,
	       "symbol", &symbol,
	       NULL);

  CU_ASSERT(symbol != NULL && g_strv_contains(symbol, "a") && g_strv_contains(symbol, "b"));

  /* exponent */
  solver_polynomial->exponent = (gchar **) g_malloc(3 * sizeof(gchar *));

  solver_polynomial->exponent[0] = "8";
  solver_polynomial->exponent[1] = "1";
  solver_polynomial->exponent[2] = NULL;

  exponent = NULL;
  
  g_object_get(solver_polynomial,
	       "exponent", &exponent,
	       NULL);

  CU_ASSERT(exponent != NULL && g_strv_contains(exponent, "8") && g_strv_contains(exponent, "1"));

  /* coefficient value */
  ags_complex_set(solver_polynomial->coefficient_value,
		  AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_COEFFICIENT_VALUE);

  coefficient_value = NULL;
  
  g_object_get(solver_polynomial,
	       "coefficient-value", &coefficient_value,
	       NULL);

  CU_ASSERT(coefficient_value != NULL && ags_complex_get(coefficient_value) == AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_COEFFICIENT_VALUE);

  /* exponent value */
  solver_polynomial->exponent_value = g_new(AgsComplex,
					    3);
  
  ags_complex_set(solver_polynomial->exponent_value,
		  AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_EXPONENT_VALUE_0);

  ags_complex_set(solver_polynomial->exponent_value + 1,
		  AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_EXPONENT_VALUE_1);

  ags_complex_set(solver_polynomial->exponent_value + 2,
		  AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_EXPONENT_VALUE_2);

  exponent_value = NULL;
  
  g_object_get(solver_polynomial,
	       "exponent-value", &exponent_value,
	       NULL);

  CU_ASSERT(exponent_value != NULL &&
	    ags_complex_get(exponent_value) == AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_EXPONENT_VALUE_0 &&
	    ags_complex_get(exponent_value + 1) == AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_EXPONENT_VALUE_1 &&
	    ags_complex_get(exponent_value + 2) == AGS_SOLVER_POLYNOMIAL_TEST_GET_PROPERTY_EXPONENT_VALUE_2);
}

void
ags_solver_polynomial_test_set_polynomial()
{
  AgsSolverPolynomial *solver_polynomial;
  
  solver_polynomial = ags_solver_polynomial_new();

  ags_solver_polynomial_set_polynomial(solver_polynomial,
				       "-3.14a^(2)b");

  CU_ASSERT(solver_polynomial->polynomial != NULL &&
	    !g_strcmp0(solver_polynomial->polynomial, "-3.14a^(2)b"));
}

void
ags_solver_polynomial_test_get_polynomial()
{
  AgsSolverPolynomial *solver_polynomial;

  gchar *polynomial;
  
  solver_polynomial = ags_solver_polynomial_new();

  solver_polynomial->polynomial = g_strdup("-3.14a^(2)b");

  polynomial = ags_solver_polynomial_get_polynomial(solver_polynomial);

  CU_ASSERT(polynomial != NULL &&
	    !g_strcmp0(polynomial, "-3.14a^(2)b"));
}

void
ags_solver_polynomial_test_set_coefficient()
{
  AgsSolverPolynomial *solver_polynomial;
  
  solver_polynomial = ags_solver_polynomial_new();

  ags_solver_polynomial_set_coefficient(solver_polynomial,
					"-3.14");

  CU_ASSERT(solver_polynomial->coefficient != NULL &&
	    !g_strcmp0(solver_polynomial->coefficient, "-3.14"));
}

void
ags_solver_polynomial_test_get_coefficient()
{
  AgsSolverPolynomial *solver_polynomial;

  gchar *coefficient;
  
  solver_polynomial = ags_solver_polynomial_new();

  solver_polynomial->coefficient = g_strdup("-3.14a^(2)b");

  coefficient = ags_solver_polynomial_get_coefficient(solver_polynomial);

  CU_ASSERT(coefficient != NULL &&
	    !g_strcmp0(coefficient, "-3.14a^(2)b"));
}

void
ags_solver_polynomial_test_get_symbol()
{
  AgsSolverPolynomial *solver_polynomial;

  gchar **symbol;
  
  solver_polynomial = ags_solver_polynomial_new();

  solver_polynomial->symbol = (gchar **) g_malloc(3 * sizeof(gchar *));

  solver_polynomial->symbol[0] = g_strdup("a");
  solver_polynomial->symbol[1] = g_strdup("b");
  solver_polynomial->symbol[2] = NULL;

  symbol = ags_solver_polynomial_get_symbol(solver_polynomial);
  
  CU_ASSERT(symbol != NULL &&
	    !g_strcmp0(solver_polynomial->symbol[0], "a") &&
	    !g_strcmp0(solver_polynomial->symbol[1], "b") &&
	    solver_polynomial->symbol[2] == NULL);
}

void
ags_solver_polynomial_test_get_exponent()
{
  AgsSolverPolynomial *solver_polynomial;

  gchar **exponent;
  
  solver_polynomial = ags_solver_polynomial_new();

  solver_polynomial->exponent = (gchar **) g_malloc(3 * sizeof(gchar *));

  solver_polynomial->exponent[0] = g_strdup("8");
  solver_polynomial->exponent[1] = g_strdup("1");
  solver_polynomial->exponent[2] = NULL;

  exponent = ags_solver_polynomial_get_exponent(solver_polynomial);
  
  CU_ASSERT(exponent != NULL &&
	    !g_strcmp0(solver_polynomial->exponent[0], "8") &&
	    !g_strcmp0(solver_polynomial->exponent[1], "1") &&
	    solver_polynomial->exponent[2] == NULL);
}

void
ags_solver_polynomial_test_set_coefficient_value()
{
  AgsSolverPolynomial *solver_polynomial;

  AgsComplex *coefficient_value;
  
  solver_polynomial = ags_solver_polynomial_new();

  coefficient_value = (AgsComplex *) g_new(AgsComplex,
					   1);

  coefficient_value->real = 1.0;
  coefficient_value->imag = -0.25;

  ags_solver_polynomial_set_coefficient_value(solver_polynomial,
					      coefficient_value);
  
  CU_ASSERT(solver_polynomial->coefficient_value->real == 1.0 &&
	    solver_polynomial->coefficient_value->imag == -0.25);
}

void
ags_solver_polynomial_test_get_coefficient_value()
{
  AgsSolverPolynomial *solver_polynomial;

  AgsComplex *coefficient_value;

  solver_polynomial = ags_solver_polynomial_new();

  solver_polynomial->coefficient_value->real = 1.0;
  solver_polynomial->coefficient_value->imag = -0.25;

  coefficient_value = ags_solver_polynomial_get_coefficient_value(solver_polynomial);
  
  CU_ASSERT(coefficient_value != NULL &&
	    coefficient_value->real == 1.0 &&
	    coefficient_value->imag == -0.25);
}

void
ags_solver_polynomial_test_get_exponent_value()
{
  AgsSolverPolynomial *solver_polynomial;

  AgsComplex *exponent_value;

  solver_polynomial = ags_solver_polynomial_new();

  solver_polynomial->exponent_value = (AgsComplex *) g_new(AgsComplex,
							   3);

  exponent_value = ags_solver_polynomial_get_exponent_value(solver_polynomial);
  
  CU_ASSERT(exponent_value == solver_polynomial->exponent_value);
}

void
ags_solver_polynomial_test_update()
{
  AgsSolverPolynomial *solver_polynomial;

  solver_polynomial = ags_solver_polynomial_new();

  //TODO:JK: implement me
}

void
ags_solver_polynomial_test_parse()
{
  AgsSolverPolynomial *solver_polynomial;

  solver_polynomial = ags_solver_polynomial_new();

  ags_solver_polynomial_parse(solver_polynomial,
			      AGS_SOLVER_POLYNOMIAL_TEST_PARSE_TERM_0);
  
  CU_ASSERT(solver_polynomial->coefficient_value->real == -3.14 &&
	    solver_polynomial->coefficient_value->imag == 0.0);
  
  CU_ASSERT(solver_polynomial->symbol == NULL);
  
  solver_polynomial = ags_solver_polynomial_new();

  ags_solver_polynomial_parse(solver_polynomial,
			      AGS_SOLVER_POLYNOMIAL_TEST_PARSE_TERM_1);
  
  CU_ASSERT(solver_polynomial->coefficient_value->real == 1.0 &&
	    solver_polynomial->coefficient_value->imag == 0.0);

  CU_ASSERT(solver_polynomial->symbol != NULL &&
	    !g_strcmp0(solver_polynomial->symbol[0], "a") &&
	    !g_strcmp0(solver_polynomial->symbol[1], "b"));
  
  solver_polynomial = ags_solver_polynomial_new();

  ags_solver_polynomial_parse(solver_polynomial,
			      AGS_SOLVER_POLYNOMIAL_TEST_PARSE_TERM_2);
  
  CU_ASSERT(solver_polynomial->coefficient_value->real == 2.78 &&
	    solver_polynomial->coefficient_value->imag == 0.0);

  CU_ASSERT(solver_polynomial->symbol != NULL &&
	    !g_strcmp0(solver_polynomial->symbol[0], "a") &&
	    !g_strcmp0(solver_polynomial->symbol[1], "b") &&
	    !g_strcmp0(solver_polynomial->symbol[2], "c"));

  CU_ASSERT(solver_polynomial->exponent != NULL &&
	    !g_strcmp0(solver_polynomial->exponent[0], "8") &&
	    !g_strcmp0(solver_polynomial->exponent[1], "2") &&
	    !g_strcmp0(solver_polynomial->exponent[2], "1"));
}

void
ags_solver_polynomial_test_add()
{
  AgsSolverPolynomial *a, *b;
  AgsSolverPolynomial *c;

  GError *error;

  /* numeric only */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-2.5");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "5");

  error = NULL;
  c = ags_solver_polynomial_add(a,
				b,
				&error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == 2.5);

  CU_ASSERT(c->symbol == NULL);
  CU_ASSERT(c->exponent == NULL);
  
  /* with symbol */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-1.5ab");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "0.5ab");

  error = NULL;
  c = ags_solver_polynomial_add(a,
				b,
				&error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == -1.0);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && c->symbol[2] == NULL);

  /* with symbol and exponent */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-6.0a^(3.14)b");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "0.5a^(3.14)b");

  error = NULL;
  c = ags_solver_polynomial_add(a,
				b,
				&error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == -5.5);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && c->symbol[2] == NULL);
}

void
ags_solver_polynomial_test_subtract()
{
  AgsSolverPolynomial *a, *b;
  AgsSolverPolynomial *c;

  GError *error;

  /* numeric only */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-2.5");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "5");

  error = NULL;
  c = ags_solver_polynomial_subtract(a,
				     b,
				     &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == -7.5);

  CU_ASSERT(c->symbol == NULL);
  CU_ASSERT(c->exponent == NULL);
  
  /* with symbol */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-1.5ab");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "0.5ab");

  error = NULL;
  c = ags_solver_polynomial_subtract(a,
				     b,
				     &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == -2.0);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && c->symbol[2] == NULL);

  /* with symbol and exponent */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-6.0a^(3.14)b");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "0.5a^(3.14)b");

  error = NULL;
  c = ags_solver_polynomial_subtract(a,
				     b,
				     &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == -6.5);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && c->symbol[2] == NULL);
}

void
ags_solver_polynomial_test_multiply()
{
  AgsSolverPolynomial *a, *b;
  AgsSolverPolynomial *c;

  GError *error;

  /* numeric only */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-2.5");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "5");

  error = NULL;
  c = ags_solver_polynomial_multiply(a,
				     b,
				     &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == -12.5);

  CU_ASSERT(c->symbol == NULL);
  CU_ASSERT(c->exponent == NULL);
  
  /* with symbol */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-1.5ab");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "0.5ab");

  error = NULL;
  c = ags_solver_polynomial_multiply(a,
				     b,
				     &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == -0.75);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && c->symbol[2] == NULL);
  CU_ASSERT(c->exponent_value != NULL && ags_complex_get(c->exponent_value) == 2.0 && ags_complex_get(c->exponent_value + 1) == 2.0);

  /* with symbol */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-1.5ab");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "6.0abc");

  error = NULL;
  c = ags_solver_polynomial_multiply(a,
				     b,
				     &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == -9.0);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && !g_strcmp0(c->symbol[2], "c") && c->symbol[3] == NULL);
  CU_ASSERT(c->exponent_value != NULL && ags_complex_get(c->exponent_value) == 2.0 && ags_complex_get(c->exponent_value + 1) == 2.0 && ags_complex_get(c->exponent_value + 2) == 1.0);
  
  /* with symbol and exponent */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-6.0a^(3.14)b");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "0.5a^(3.14)b");

  error = NULL;
  c = ags_solver_polynomial_multiply(a,
				     b,
				     &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == -3.0);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && c->symbol[2] == NULL);
  CU_ASSERT(c->exponent_value != NULL && ags_complex_get(c->exponent_value) == 6.28 && ags_complex_get(c->exponent_value + 1) == 2.0);
}

void
ags_solver_polynomial_test_divide()
{
  AgsSolverPolynomial *a, *b;
  AgsSolverPolynomial *c;

  GError *error;

  /* numeric only */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-2.5");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "5");

  error = NULL;
  c = ags_solver_polynomial_divide(a,
				   b,
				   &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == -0.5);

  CU_ASSERT(c->symbol == NULL);
  CU_ASSERT(c->exponent == NULL);
  
  /* with symbol */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-1.5ab");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "0.5ab");

  error = NULL;
  c = ags_solver_polynomial_divide(a,
				   b,
				   &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == -3.0);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && c->symbol[2] == NULL);
  CU_ASSERT(c->exponent_value != NULL && ags_complex_get(c->exponent_value) == 0.0 && ags_complex_get(c->exponent_value + 1) == 0.0);

  /* with symbol */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-3.0ab");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "6.0abc");

  error = NULL;
  c = ags_solver_polynomial_divide(a,
				   b,
				   &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == -0.5);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && !g_strcmp0(c->symbol[2], "c") && c->symbol[3] == NULL);
  CU_ASSERT(c->exponent_value != NULL && ags_complex_get(c->exponent_value) == 0.0 && ags_complex_get(c->exponent_value + 1) == 0.0 && ags_complex_get(c->exponent_value + 2) == -1.0);
  
  /* with symbol and exponent */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "-6.0a^(3.14)b");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "0.5a^(3.14)b");

  error = NULL;
  c = ags_solver_polynomial_divide(a,
				   b,
				   &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == -12.0);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && c->symbol[2] == NULL);
  CU_ASSERT(c->exponent_value != NULL && ags_complex_get(c->exponent_value) == 0.0 && ags_complex_get(c->exponent_value + 1) == 0.0);
}

void
ags_solver_polynomial_test_raise_power()
{
  AgsSolverPolynomial *a, *b;
  AgsSolverPolynomial *c;

  GError *error;

  /* numeric only */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "2.0");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "5.0");

  error = NULL;
  c = ags_solver_polynomial_raise_power(a,
					b,
					&error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == 32.0);

  CU_ASSERT(c->symbol == NULL);
  CU_ASSERT(c->exponent == NULL);
  
  /* with symbol */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "4.0ab");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "0.5");

  error = NULL;
  c = ags_solver_polynomial_raise_power(a,
					b,
					&error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(ags_complex_get(c->coefficient_value) == 2.0);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && c->symbol[2] == NULL);
  CU_ASSERT(c->exponent_value != NULL && ags_complex_get(c->exponent_value) == 0.5 && ags_complex_get(c->exponent_value + 1) == 0.5);

  /* with symbol */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "3.0abc");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "2.0");

  error = NULL;
  c = ags_solver_polynomial_raise_power(a,
					b,
					&error);
  
  CU_ASSERT(error == NULL);
  
  CU_ASSERT(round(ags_complex_get(c->coefficient_value)) == 9.0);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && !g_strcmp0(c->symbol[2], "c") && c->symbol[3] == NULL);
  CU_ASSERT(c->exponent_value != NULL && round(ags_complex_get(c->exponent_value)) == 2.0 && round(ags_complex_get(c->exponent_value + 1)) == 2.0 && round(ags_complex_get(c->exponent_value + 2)) == 2.0);
  
  /* with symbol and exponent */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "32.0a^(12.0)b");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "0.25");

  error = NULL;
  c = ags_solver_polynomial_raise_power(a,
					b,
					&error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(round(ags_complex_get(c->coefficient_value)) == 2.0);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && c->symbol[2] == NULL);
  CU_ASSERT(c->exponent_value != NULL && round(ags_complex_get(c->exponent_value)) == 3.0 && round(4.0 * ags_complex_get(c->exponent_value + 1)) == 1.0);
}

void
ags_solver_polynomial_test_extract_root()
{
  AgsSolverPolynomial *a, *b;
  AgsSolverPolynomial *c;

  GError *error;

  /* numeric only */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "27.0");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "3.0");

  error = NULL;
  c = ags_solver_polynomial_extract_root(a,
					 b,
					 &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(round(ags_complex_get(c->coefficient_value)) == 3.0);

  CU_ASSERT(c->symbol == NULL);
  CU_ASSERT(c->exponent == NULL);
  
  /* with symbol */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "4.0ab");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "0.5");

  error = NULL;
  c = ags_solver_polynomial_extract_root(a,
					 b,
					 &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(round(ags_complex_get(c->coefficient_value)) == 16.0);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && c->symbol[2] == NULL);
  CU_ASSERT(c->exponent_value != NULL && round(ags_complex_get(c->exponent_value)) == 2.0 && round(ags_complex_get(c->exponent_value + 1)) == 2.0);

  /* with symbol */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "4.0abc");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "2.0");

  error = NULL;
  c = ags_solver_polynomial_extract_root(a,
					 b,
					 &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(round(ags_complex_get(c->coefficient_value)) == 2.0);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && !g_strcmp0(c->symbol[2], "c") && c->symbol[3] == NULL);
  CU_ASSERT(c->exponent_value != NULL && ags_complex_get(c->exponent_value) == 0.5 && ags_complex_get(c->exponent_value + 1) == 0.5 && ags_complex_get(c->exponent_value + 2) == 0.5);
  
  /* with symbol and exponent */
  a = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(a,
			      "32.0a^(12.0)b");

  b = ags_solver_polynomial_new();
  ags_solver_polynomial_parse(b,
			      "4.0");

  error = NULL;
  c = ags_solver_polynomial_extract_root(a,
					 b,
					 &error);

  CU_ASSERT(error == NULL);
  
  CU_ASSERT(round(ags_complex_get(c->coefficient_value)) == 2.0);

  CU_ASSERT(c->symbol != NULL && !g_strcmp0(c->symbol[0], "a") && !g_strcmp0(c->symbol[1], "b") && c->symbol[2] == NULL);
  CU_ASSERT(c->exponent_value != NULL && round(ags_complex_get(c->exponent_value)) == 3.0 && round(4.0 * ags_complex_get(c->exponent_value + 1)) == 1.0);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsSolverPolynomialTest", ags_solver_polynomial_test_init_suite, ags_solver_polynomial_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSolverPolynomial set property", ags_solver_polynomial_test_set_property) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial get property", ags_solver_polynomial_test_get_property) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial set polynomial", ags_solver_polynomial_test_set_polynomial) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial get polynomial", ags_solver_polynomial_test_get_polynomial) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial set coefficient", ags_solver_polynomial_test_set_coefficient) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial get coefficient", ags_solver_polynomial_test_get_coefficient) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial get symbol", ags_solver_polynomial_test_get_symbol) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial get exponent", ags_solver_polynomial_test_get_exponent) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial set coefficient value", ags_solver_polynomial_test_set_coefficient_value) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial get coefficient value", ags_solver_polynomial_test_get_coefficient_value) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial get exponent value", ags_solver_polynomial_test_get_exponent_value) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial update", ags_solver_polynomial_test_update) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial parse", ags_solver_polynomial_test_parse) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial add", ags_solver_polynomial_test_add) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial subtract", ags_solver_polynomial_test_subtract) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial multiply", ags_solver_polynomial_test_multiply) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial divide", ags_solver_polynomial_test_divide) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial raise power", ags_solver_polynomial_test_raise_power) == NULL) ||
     (CU_add_test(pSuite, "test of AgsSolverPolynomial extract root", ags_solver_polynomial_test_extract_root) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
