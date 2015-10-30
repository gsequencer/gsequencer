#include "jmath.h"
//#include "jthread.h"

int j_math_or(int a, int b);
int j_math_exclusive_or(int a, int b);
int j_math_and(int a, int b);
JList* j_math_or_list(JList *a, JList *b);
JList* j_math_exclusive_or_list(JList *a, JList *b);
JList* j_math_and_list(JList *a, JList *b);

JList* j_math_left_move_list_value(JList *a, uint b);
JList* j_math_right_move_list_value(JList *a, uint b);
JList* j_math_left_move_list(JList *list0, JList *list1);
JList* j_math_right_move_list(JList *list0, JList *list1);

//void __j_math_equal(uint a, uint b, jboolean sign, jboolean sign_equal, uint value0, uint *value1, jboolean *jump);
uint _j_math_equal(int a, int b);
jboolean j_math_equal(int a, int b);
jboolean j_math_equal_list(JList *a, JList *b);

jboolean _j_math_more(uint a, uint b);
jboolean _j_math_more_list(JList *list_a, JList *a_next, JList *list_b, JList *b_next);
jboolean j_math_more_list(JList *a, JList *b);
jboolean _j_math_less(uint a, uint b);
jboolean _j_math_less_list(JList *list_a, JList *a_next, JList *list_b, JList *b_next);
jboolean j_math_less_list(JList *a, JList *b);

void _j_math_addition(int a, int b, uint *destination, uint values);
int j_math_addition(int a, int b);
void j_math_uaddition(uint a, uint b, uint *destination, uint values);
void __j_math_addition_list(JList *a, JList *b, uint *pntr, uint value1, uint *part, JList *list);
void __j_math_addition_list_zero(JList *tmp, JList *zero, uint *pntr, uint value1, uint *part, JList *list);
void _j_math_addition_list(JList *a, JList *b, uint *destination, uint values);
JList* j_math_addition_list(JList *a, JList *b);
JList* j_math_uaddition_list(JList *a, JList *b);
void _j_math_subtraction(int a, int b, uint *destination, uint values);
int j_math_subtraction(int a, int b);
void j_math_usubtraction(uint a, uint b, uint *destination, uint values);
void _j_math_subtraction_list(JList *a, JList *b, uint *destination, uint values);
JList* j_math_subtraction_list(JList *a, JList *b);
JList* j_math_usubtraction_list(JList *a, JList *b);
int _j_math_multiplication(int a, int b);
JList* _j_math_multiplication_list(JList *a, JList *b);
int j_math_multiplication(int a, int b);
JList* j_math_multiplication_list(JList *a, JList *b);
uint j_math_umultiplication(uint a, uint b);
JList* j_math_umultiplication_list(JList *a, JList *b);
JList* _j_math_division_list(JList *a, JList *b);
int j_math_division(int a, int b, uint *destination, uint values);
void j_math_division_list(JList *a, JList *b, uint *pntr, uint values);
uint j_math_udivision(uint a, uint b);
JList* j_math_udivision_list(JList *a, JList *b);
int j_math_rest(int a, int b);
JList* j_math_rest_list(JList *a, JList *b);

//#define j_math_equal_bit(a, b, value) if(value & a)
#define j_math_more_bit(a, b, value) if(value & a){if(!(value & b)){return(FALSE);}}else if(value & b){return(TRUE);}
#define j_math_less_bit(a, b, value) if(value & a){if(!(value & b)){return(TRUE);}}else if(value & b){return(FALSE);}

typedef enum
{
  J_MATH_EQUAL,
  J_MATH_LESS,
  J_MATH_MORE,
  J_MATH_SIGN,
};

#define J_MATH_EQUAL_LESS(a, b, value)         ((J_MATH_MORE & (value = _j_math_equal(a, b))) || ((J_MATH_LESS | J_MATH_SIGN) & VALUE) || (J_MATH_EQUAL & value)) ? TRUE: FALSE)
#define J_MATH_EQUAL_MORE(a, b, value)         ((J_MATH_LESS & (value = _j_math_equal(a, b))) || ((J_MATH_MORE | J_MATH_SIGN) & VALUE) || (J_MATH_EQUAL & value)) ? TRUE: FALSE)
#define J_MATH_EQUAL_RESPECT_SIGN(a, b, value) (((J_MATH_EQUAL | J_MATH_SIGN) & (value = _j_math_equal(a, b))) ? FALSE: TRUE)
#define j_math_equal_less(a, b)                (!j_math_more(a, b))
#define j_math_equal_more(a, b)                (!j_math_less(a, b))
#define j_math_equal_less_list(a, b)            (!j_math_more_list(a, b))
#define j_math_equal_more_list(a, b)            (!j_math_less_list(a, b))

#define __J_MATH_NO_LEADING_ZEROS__

int
j_math_or(int a, int b)
{
  int value;
  
  return(value);
}

int
j_math_exclusive_or(int a, int b)
{
  int value;
  
  return(value);
}

int
j_math_and(int a, int b)
{
  int value;
  
  return(value);
}

JList*
j_math_or_list(JList *a, JList *b)
{
  JList *list0, *list1;
  
  if(a == NULL || b == NULL)
    return(NULL);

  list0 = j_list_alloc();
  goto j_math_or_list0;

  while(a != NULL && b != NULL){
    list0->prev = j_list_alloc();
    list0->prev->next = list0;
    list0 = list0->prev;
  j_math_or_list0:
    list0->data = (jpointer) (((uint) a->data) | ((uint) b->data));
    
    a = a->prev;
    b = b->prev;
  }

  if(b == NULL)
    list1 = a;
  else
    list1 = b;

  while(list1 != NULL){
    list0->prev = j_list_alloc();
    list0->prev->next = list0;
    list0 = list0->prev;
    list0->data = list1->data;

    list1 = list1->prev;
  }

  return(list0);
}

JList*
j_math_exclusive_or_list(JList *a, JList *b)
{
  JList *list0, *list1;

  if(a == NULL || b == NULL)
    return(NULL);

  list0 = j_list_alloc();
  goto j_math_exclusive_or_list0;

  while(a != NULL && b != NULL){
    list0->prev = j_list_alloc();
    list0->prev->next = list0;
    list0 = list0->prev;
  j_math_exclusive_or_list0:
    list0->data = (jpointer) (((uint) a->data) ^ ((uint) b->data));

    a = a->prev;
    b = b->prev;
  }

  if(b == NULL)
    list1 = a;
  else
    list1 = b;

  while(list1 != NULL){
    list0->prev = j_list_alloc();
    list0->prev->next = list0;
    list0 = list0->prev;
    list0->data = list1->data;

    list1 = list1->prev;
  }

  return(list0);
}

JList*
j_math_and_list(JList *a, JList *b)
{
  JList *list;

  if(a == NULL || b == NULL)
    return(NULL);

  list = j_list_alloc();
  goto j_math_and_list0;

  while(a != NULL && b != NULL){
    list->prev = j_list_alloc();
    list->prev->next = list;
    list = list->prev;
  j_math_and_list0:
    list->data = (jpointer) (((uint) a->data) & ((uint) b->data));

    a = a->prev;
    b = b->prev;
  }

  return(list);
}

JList*
j_math_left_move_list_stepwise(JList *list0, JList *list1)
{
  JList *start, *list, *list_set, *list0_set;
  JList *i0, *i1, *i_set0, *i_set1, *one;
  uint value, word;
  jboolean keep0, keep1;

  i0 = j_list_alloc();
  i_set0 =
    i0->data = (jpointer) 0;

  one = j_list_alloc();
  one->data = (jpointer) 1;

  keep1 = FALSE;
  list0_set = list0;

  start =
    list =
    list_set = j_list_alloc();
  value = 1 << (sizeof(uint) * 8 -1);

  if(j_math_less_list(list1, i0))
    goto j_math_left_move_list_stepwise0;

  for(;j_math_less_list(list1, i0);){
    j_list_destroy(i1);
    i1 = j_math_addition_list(i0, one);
    keep0 = FALSE;

    for(i0 = i_set0, word = 0, list = list_set, list0 = list0_set; j_math_less_list(list1, i0); word++){
      j_list_destroy(i1);
    j_math_left_move_list_stepwise0:
      i1 = j_math_addition_list(i0, one);

      if(!(word < sizeof(uint) * 8)){
	list->prev = j_list_alloc();
	list->prev->next = list;
	list = list->prev;
	list_set = list_set->prev;
	list0_set = list0_set->prev;
      }

      if(value & ((uint) list0->data))
	keep0 = TRUE;

      list0->data = (jpointer) (((uint) list0->data) << 1);

      if(keep1)
	list0->data = (jpointer) (((uint) list0->data) | 1);

      keep1 = keep0;

	i0 = i1;
    }

    i_set1 = j_math_subtraction_list(i_set0, one);
    j_list_destroy(i_set0);
    i_set0 = i_set1;
  }

  return(start);
}

JList*
j_math_right_move_list_stepwise(JList *list0, JList *list1)
{
  JList *list, *stop, *list_set, *list0_set;
  JList *i0, *i1, *i_set0, *i_set1, *one;
  uint value, word;
  jboolean keep0, keep1;

  i0 = j_list_alloc();
  i_set0 =
    i0->data = (jpointer) 0;

  one = j_list_alloc();
  one->data = (jpointer) 1;

  keep1 = FALSE;
  list_set =
    list = j_list_alloc();

  stop = list0;
  list0_set =
    list0 = j_list_first(list0);
  value = 1 << (sizeof(uint) * 8 -1);

  if(j_math_less_list(list1, i0))
    goto j_math_left_move_list_stepwise0;

  for(;j_math_less_list(list1, i0) && list0 != stop;){
    j_list_destroy(i1);
    i1 = j_math_addition_list(i0, one);
    keep0 = FALSE;

    for(i0 = i_set0, word = 0, list = list_set, list0 = list0_set; j_math_less_list(list1, i0) && list0 != stop; word++){
      j_list_destroy(i1);
    j_math_left_move_list_stepwise0:
      i1 = j_math_addition_list(i0, one);

      if(!(word < sizeof(uint) * 8)){
	list->next = j_list_alloc();
	list->next->prev = list;
	list = list->next;
	list_set = list_set->next;
	list0_set = list0_set->next;
      }

      if(1 & ((uint)list0->data))
	keep0 = TRUE;

      list->data = (jpointer) (((uint) list0->data) << 1);

      if(keep1)
	list->data = (jpointer) (((uint) list0->data) | value);

      keep1 = keep0;

      i0 = i1;
      list0 = list0->next;
    }

    i_set1 = j_math_subtraction_list(i_set0, one);
    j_list_destroy(i_set0);
    i_set0 = i_set1;
  }

  return(list);
}

JList*
j_math_left_move_list(JList *list0, JList *list1)
{
  JList *i, *one, *zero, *start, *size, *list2;
  uint pntr[0];
  JList *move, *rest;
  uint value0;
  uint j, copy0, copy1, value1, right;

  one = j_list_alloc();
  one->data = (jpointer) 1;
  size = j_list_alloc();
  size->data = (jpointer) (8 * sizeof(uint));

  value0 = (1 || (1 << 1));
  j_math_division_list(list0, size, pntr, value0);

  if(list0->prev != NULL){
    list2 = list0->prev;
    goto j_math_left_move_list0;

    for(; list2 != NULL && j_math_less_list(i, move);){
      list2 = list0->prev;
    j_math_left_move_list0:
      list0 = list2;
      j_math_addition_list(i, one);
    }
  }

  start = list0;

  for(; j_math_less_list(i, move);){
    list0->prev = j_list_alloc();
    list0->prev->next = list0;
    list0 = list0->prev;
    list0->data = NULL;
    j_math_addition_list(i, one);
  }

  zero = j_list_alloc();
  zero->data = 0;

  for(j = 0, value1 = 0; j < ((uint) rest->data); j++)
    value1 |= (1 << ((8 * sizeof(uint) -1) - j));

  if(j_math_more_list(rest, zero)){
    list0 = j_list_last(start);
    right = ((8 * sizeof(uint) -1) - ((uint) rest->data));

    if(list0 != NULL){
      copy1 = 0;
      goto j_math_left_move_list1;
    }

    for(; list0 != NULL;){
      copy1 = copy0;
    j_math_left_move_list1:
      copy0 = ((uint)list0->data) & value1;
      list0->data = (jpointer) (((uint) list0->data) << ((uint) rest->data));
      list0->data = (jpointer) (((uint) list0->data) | (copy1 >> right));
      list0 = list0->next;
    }

    if(list0 == NULL){
      list0 = j_list_first(start);
      list0->prev = j_list_alloc();
    }

    list0->data = (jpointer) (((uint) list0->data) | (copy0 >> right));
  }

  return(start);
}

JList*
j_math_right_move_list(JList *list0, JList *list1)
{
  JList *i, *one, *zero, *start, *size, *list2;
  uint pntr[0];
  JList *move, *rest;
  uint value0;
  uint j, copy0, copy1, value1, left;

  one = j_list_alloc();
  one->data = (jpointer) 1;
  size = j_list_alloc();
  size->data = (jpointer) (8 * sizeof(uint));

  value0 = (1 || (1 << 1));
  j_math_division_list(list0, size, pntr, value0);

  if(list0->next != NULL){
    list2 = list0->next;
    goto j_math_right_move_list0;

    for(; list2 != NULL && j_math_less_list(i, move);){
      list2 = list0->next;
    j_math_right_move_list0:
      list0 = list2;
      j_math_addition_list(i, one);
    }
  }

  start = list0;

  for(; j_math_less_list(i, move);){
    list0->next = j_list_alloc();
    list0->next->prev = list0;
    list0 = list0->next;
    list0->data = NULL;
    j_math_addition_list(i, one);
  }

  zero = j_list_alloc();
  zero->data = 0;

  for(j = 0, value1 = 0; j < ((uint) rest->data); j++)
    value1 |= (1 << j);

  if(j_math_more_list(rest, zero)){
    list0 = j_list_first(start);
    left = ((8 * sizeof(uint) -1) - ((uint) rest->data));

    if(list0 != NULL){
      copy1 = 0;
      goto j_math_right_move_list1;
    }

    for(; list0 != NULL;){
      copy1 = copy0;
    j_math_right_move_list1:
      copy0 = ((uint)list0->data) & value1;
      list0->data = (jpointer) (((uint) list0->data) >> ((uint) rest->data));
      list0->data = (jpointer) (((uint) list0->data) | (copy1 << left));
      list0 = list0->next;
    }

    if(list0 == NULL){
      list0 = j_list_first(start);
      list0->next = j_list_alloc();
    }

    list0->data = (jpointer) (((uint) list0->data) | (copy0 << left));
  }

  return(start);
}

uint
_j_math_equal(int a, int b)
{
  uint value0, value1;
  jboolean sign, sign_equal;
  jboolean jump;
  void __j_math_equal(){
    if(value0 & a){
      if(!(value0 & b))
	if(sign_equal){
	  value1 |= J_MATH_MORE;
	  jump = TRUE;
	}else if(sign){
	  value1 |= J_MATH_LESS;
	  jump = TRUE;
	}else{
	  value1 |= J_MATH_MORE;
	  jump = TRUE;
	}
    }else if(value0 & b)
      if(sign_equal){
	value1 |= J_MATH_LESS;
	jump = TRUE;
      }else if(sign){
	value1 |= J_MATH_MORE;
	jump = TRUE;
      }else{
	value1 |= J_MATH_LESS;
	jump = TRUE;
      }
  }

  value0 = (1 << (sizeof(uint) * 8 -1));
  value1 = 0;

  if((value0 & a) && !(value0 & b)){
    value1 |= J_MATH_SIGN;
    sign = TRUE;
    
  }else if(!(value0 & a) && (value0 & b)){
    value1 |= J_MATH_SIGN;
    sign = FALSE;
  }else{
    sign_equal = TRUE;
    goto _j_math_equal0;
  }
  sign_equal = FALSE;
 _j_math_equal0:
  value0 >>= 1;

  jump = FALSE;

  for(; value0 != 1;){
    __j_math_equal();
    if(jump)
      goto _j_math_equal1;
    value0 >>= 1;
  }
  __j_math_equal();
    if(jump)
      goto _j_math_equal1;

  value1 |= J_MATH_EQUAL;

 _j_math_equal1:

  return(value1);
}

jboolean
j_math_equal(int a, int b)
{
  if(b^a)
    return(FALSE);
  else
    return(TRUE);
}

uint
_j_math_equal_list(JList *list_a, JList *a_next, JList *list_b, JList *b_next)
{
  JList *list0, *list1;
  uint value, start;

  list0 = list_a;
  list1 = list_b;

  while(list0 != a_next && list1 != b_next){
    list0 = list0->next;
    list1 = list1->next;
  }

  if(list0 != a_next)
    return(2);

  if(list1 != b_next)
    return(0);

  start = 1 << (sizeof(uint) * 8 -1);

  while(list_a != a_next && list_b != b_next){
    for(value = start; value != 1;){
      if(value & (uint) list_a->data){
	if(!(value & (uint) list_b->data))
	  return(0);
      }else if(value & (uint) list_b->data)
	return(2);
    }

    list_a = list_a->next;
    list_b = list_b->next;
  }

  return(1);
}

jboolean
j_math_equal_list(JList *a, JList *b)
{
#ifdef __J_MATH_LIST_NO_LEADING_ZEROS__
  while(a != NULL && b != NULL){
    if(a->data != b->data)
      return(FALSE);

    a = a->prev;
    b = b->prev;
  }

  if(a != NULL || b != NULL)
    return(FALSE);
  else
    return(TRUE);
#else
  uint value0, value1;

  while(a->prev != NULL && b->prev != NULL){
    if(a->data != b->data)
      return(FALSE);

    a = a->prev;
    b = b->prev;
  }

  value0 = 1 << (sizeof(uint) * 8 -1);
  value1 = 1;

  if((value1 & ((uint) a->data)) != (value1 & ((uint) b->data)))
    return(FALSE);

  if(a->prev != NULL){
    if(value0 & ((uint) b->data)){
      if(value0 & ((uint) a->data))
	return(FALSE);

      return(j_math_leading_zeros(a->prev, TRUE));
    }else
      return(j_math_leading_zeros(a->prev, FALSE));
  }else if(b->prev != NULL){
    if(value0 & ((uint) a->data)){
      if(value0 & ((uint) b->data))
	return(FALSE);

      return(j_math_leading_zeros(a->prev, FALSE));
    }else
      return(j_math_leading_zeros(b->prev, TRUE));
  }else
    return(TRUE);
#endif
}

jboolean
j_math_leading_zeros(JList *list, jboolean sign){
  uint value = 1 << (sizeof(uint) * 8 -1);

  while(list->next != NULL){
    if(((uint) list->data) != 0)
      return(FALSE);
    
    list = list->next;
  }

  if(sign)
    if(((uint) list->data) == value)
      return(TRUE);
    else
      return(FALSE);
  else if(((uint) list->data) == 0)
    return(TRUE);
  else
    return(FALSE);
}

jboolean
_j_math_more(uint a, uint b)
{
  uint value;

  value = 1 <<(sizeof(int) * 8 -1);

  for(; value != 1;){
    j_math_more_bit(a, b, value);
    value = value >> 1;
  }

  if((value & a) && !(value & b))
    return(TRUE);
  else
    return(FALSE);
}

jboolean
j_math_more(int a, int b)
{
  uint value;

  value = 1 << (sizeof(int) * 8 -1);

  j_math_less_bit(a, b, value);

  for(; value != 1;){
    value = value >> 1;
    j_math_more_bit(a, b, value);
  } 

  return(FALSE);
}

jboolean
_j_math_more_list(JList *list_a, JList *a_next, JList *list_b, JList *b_next)
{
  JList *list0, *list1;
  uint value, start;

  list0 = list_a;
  list1 = list_b;

  while(list0 != a_next && list1 != b_next){
    list0 = list0->next;
    list1 = list1->next;
  }

  if(list0 != a_next)
    return(TRUE);
  else if(list1 != b_next)
    return(FALSE);

  start = 1 << (sizeof(uint) * 8 -1);

  while(list_a != a_next && list_b != b_next){
    for(value = start; value != 1;){
      j_math_more_bit((uint) list_a->data, (uint) list_b->data, value);

      value = value >> 1;
    }

    j_math_more_bit((uint) list_a->data, (uint) list_b->data, value);

    list_a = list_a->next;
    list_b = list_b->next;
  }

  if(list_a == NULL)
    return(FALSE);
  else
    return(TRUE);
}

jboolean
j_math_more_list(JList *a, JList *b)
{
  JList *list_a, *list_b;
  uint value = 1 << (sizeof(uint) * 8 -1);

#ifdef __J_MATH_NO_LEADING_ZEROS__
  list_a = j_list_first(a);
  list_b = j_list_first(b);
#elif
  list_a = j_math_cut_leading_zeros(a, TRUE);
  list_b = j_math_cut_leading_zeros(b, TRUE);
#endif

  j_math_less_bit((uint) list_a->data, (uint) list_b->data, value);

  if(list_a->data == 0)
    list_a = list_a->next;

  if(list_b->data == 0)
    list_b = list_b->next;

  return(_j_math_more_list(list_a, a->next, list_b, b->next));
}

jboolean
j_math_more_equal_list(JList *a, JList *b)
{
  JList *list_a, *list_b;
  uint value0, value1;

#ifdef __J_MATH_NO_LEADING_ZEROS__
  list_a = j_list_first(a);
  list_b = j_list_first(b);
#elif
  list_a = j_math_cut_leading_zeros(a);
  list_b = j_math_cut_leading_zeros(b);
#endif

  value0 = 1 << (sizeof(uint) * 8 -1);

  j_math_less_bit((uint) list_a->data, (uint) list_b->data, value0);

  value1 = _j_math_equal_list(list_a, a->next, list_b, b->next);

  switch(value1){
  case 2:
  case 1:
    return(TRUE);
  }

  return(FALSE);
}

jboolean
_j_math_less(uint a, uint b)
{
  uint value;

  value = 1 << (sizeof(int) * 8 -1);

  for(; value != 1;){
    j_math_less_bit(a, b, value);
    value = value >> 1;
  }

  if(!(value & a) && (value & b))
    return(TRUE);
  else
    return(FALSE);
}

jboolean
j_math_less(int a, int b)
{
  uint value, sign;

  sign = 1 << (sizeof(int) * 8 -1);

  j_math_more_bit(a, b, value);

  for(value = sign; value != 1;){
    value = value >> 1;
    j_math_less_bit(a, b, value);
  } 

  return(FALSE);
}

jboolean
_j_math_less_list(JList *list_a, JList *a_next, JList *list_b, JList *b_next)
{
  JList *list0,  *list1;
  uint value, start;

  start = 1 << (sizeof(uint) * 8 -1);

  list0 = list_a;
  list1 = list_b;
  
  while(list0 != a_next && list1 != b_next){
    list0 = list0->next;
    list1 = list1->next;
  }

  if(list0 != a_next)
    return(FALSE);
  else if(list1 != b_next)
    return(TRUE);

  if(start == 1)
    goto _j_math_less_list1;
  else{
    value = start >> 1;
    goto _j_math_less_list0;
  }

  while(list_a != a_next && list_b != b_next){
    for(value = start; value != 1;){
    _j_math_less_list0:
      j_math_less_bit((uint) list_a->data, (uint) list_b->data, value);

      value = value >> 1;
    }

  _j_math_less_list1:
    j_math_less_bit((uint) list_a->data, (uint) list_b->data, value);

    list0 = list0->next;
    list1 = list1->next;
  }

  if(list_b == b_next)
    return(FALSE);
  else
    return(TRUE);
}

jboolean
j_math_less_list(JList *a, JList *b)
{
  JList *list_a, *list_b;
  uint value;

  value = 1 << (sizeof(uint) * 8 -1);

#ifdef __J_MATH_NO_LEADING_ZEROS__
  list_a = j_list_first(a);
  list_b = j_list_first(b);
#elif
  list_a = j_math_cut_leading_zeros(a);
  list_b = j_math_cut_leading_zeros(b);
#endif

  j_math_more_bit((uint) list_a->data, (uint) list_b->data, value);

  if(((uint) list_a->data) == 0 && list_a != a)
    list_a = list_a->next;

  if(((uint) list_b->data) == 0 && list_b != b)
    list_b = list_b->next;

  return(_j_math_less_list(list_a, a->next, list_b, b->next));
}

jboolean
j_math_less_equal_list(JList *a, JList *b)
{
  JList *list_a, *list_b;
  uint value0, value1;

#ifdef __J_MATH_NO_LEADING_ZEROS__
  list_a = j_list_first(a);
  list_b = j_list_first(b);
#elif
  list_a = j_math_cut_leading_zeros(a);
  list_b = j_math_cut_leading_zeros(b);
#endif

  value0 = 1 << (sizeof(uint) * 8 -1);

  j_math_more_bit((uint) list_a->data, (uint) list_b->data, value0);

  value1 = _j_math_equal_list(list_a, a->next, list_b, b->next);

  switch(value1){
  case 0:
  case 1:
    return(TRUE);
  }

  return(FALSE);
}

int*
j_math_sort_array(int *array, int size)
{
  int *value = (int*) malloc(size * sizeof(int));
  JList *i_hash, *j_hash, *last;
  uint i, j, place, equal;

  i_hash = j_list_new(size);
  j_hash = i_hash->next;

  last = j_list_last(i_hash);
  last->next = i_hash;
  i_hash->prev = last;

  for(i = 0; i < size; i++){
    i_hash->data = (jpointer) array[i];
    i_hash = i_hash->next;
  }

  if(j_hash == NULL){
    value[0] = array[0];
    return(value);
  }
  
  for(i = 0; i < size; i++){
    for(j = i; j < size; j++){
      if((int) i_hash->data > (int) j_hash->data)
	place++;

      array[place] == (int) i_hash->data;
      j_hash= j_hash->next;
    }
    i_hash = i_hash->next;
  }

  j_hash->prev = NULL;
  i_hash->next = NULL;

  j_list_destroy(j_hash);

  return(value);
}

void
j_math_put_null_at_beginning(int *array, uint size)
{
  uint i, j;
  uint tmp;

  for(i = 0; i < size; i++){
    if(((jpointer) array[i]) == NULL){
      tmp = array[0];

      for(j = 1; j < i+1; j++){
	array[j] = tmp;
	tmp = array[j+1];
      }

      array[0] = (int) NULL;
    }
  }
}

void
j_math_put_null_at_end(int *array, uint size)
{
  uint i, j;
  uint tmp;

  for(i = 0; i < size; i++){
    if(((jpointer)array[i]) == NULL){
      tmp = array[i+1];

      for(j = i; j < size -2; j++){
	array[j] = tmp;
	tmp = array[j+1];
      }

      array[size] = (int) NULL;
    }
  }
}

uint
j_math_width(int a)
{
  uint value = 1 << (sizeof(uint) * 8 -2);
  uint width = sizeof(uint) * 8 -1;

  for(; !(value & a) && !(value & 1); width--)
    value = value >> 1;

  if(!(value & 1))
    width--;

  return(width);
}

JList*
j_math_width_list(JList *stop)
{
  JList *width0, *width1, *word, *list, *one;
  uint value;

  one = j_list_alloc();
  one->data = (jpointer) (1 | (1 << (sizeof(uint) * 8 -1)));
  word = j_list_alloc();
  word->data = (jpointer) (sizeof(uint) * 8);
  list = j_list_alloc();
  list->data = (jpointer) j_list_lenght_backward(stop);
  width0 = j_math_multiplication_list(word, list);
  free(word);
  free(list);
  width1 = width0;
  width0 = j_math_subtraction_list(width0, one);
  j_list_destroy(j_list_first(width1));

#ifdef __J_MATH_NO_LEADING_ZEROS__
  list = j_list_first(stop);
#elif
  list = j_math_cut_leading_zeros(stop);
#endif

  goto j_math_width_list0;

  while(TRUE){
    for(value = 1 << (sizeof(uint) * 8 -1);;){
      if(value & ((uint) list->data))
	goto j_math_width_list1;

      width1 = width0;
      width0 = j_math_subtraction_list(width0, one);
      j_list_destroy(j_list_first(width1));
    j_math_width_list0:

      if(value == 1)
	break;

      value = value >> 1;
    }

    if(list == stop)
      break;

    list = list->next;
  }

 j_math_width_list1:

  return(width0);
}

JList*
j_math_cut_leading_zeros(JList *stop)
{
  JList *start, *list;
  uint value = 1 << (sizeof(uint) * 8 -1);
  jboolean sign;

  if(stop->prev == NULL)
    return(stop);

  list =
    start = j_list_first(stop);

  if(value & ((uint) start->data))
    if(value ^ ((uint) start->data))
      return(stop);
    else{
      sign = TRUE;
      list = list->next;
    }

  while(list != stop && ((uint) list->data) != 0){
    list = list->next;
    free(list->prev);
  }

  if(sign){
    if(list == stop){
      start->next = stop->next;
      free(stop);
      stop = start;

      if(stop->next != NULL)
	stop->next->prev = stop;
    }else if(value & ((uint) list->data)){
      list->prev = start;
      start->next = list;
    }else{
      list->prev = NULL;
      list->data = (jpointer) (value | ((uint ) list->data));
      free(start);
      start = list;
    }
  }else{
    if(value & ((uint) list->data)){
      list->prev = j_list_alloc();
      start = list->prev;
      start->next = list;
      start->data = 0;
    }else
      start = list;
  }

  return(start);
}

JList*
j_math_cut_following_zeros(JList *stop)
{
  JList *end;

  end = j_list_last(stop);

  while(end != stop && end->data == 0)
    end = end->prev;

  j_list_destroy(end->next);

  return(end);
}

void
_j_math_addition(int a, int b, uint *destination, uint values)
{
  int value0;
  int solution;
  jboolean keep;
  uint value1, i;

  solution = 0;

  if((1 << 2) & values)
    keep = destination[1];
  else
    keep = FALSE;

  value0 = 1;

  for(; value0 != (1 << (sizeof(int) * 8 -1));){
    if(keep){
      if(value0 & a && value0 & b){
	solution |= value0;
	keep = 1;
      }else if(value0 & a || value0 & b)
	keep = 1;
      else{
	solution |= value0;
	keep = 0;
      }
    }else{
      if(value0 & a && value0 & b)
	keep = 1;
      else if(value0 & a || value0 & b){
	solution |= value0;
	keep = 0;
      }
    }

    value0 = value0<<1;
  }

  value1 = 1;
  i = 0;

  if(value1 & values){
    destination[0] = solution;
    i++;
  }

  value1 = 1 << 1;

  if(value1 & values)
    destination[i] = keep;
}

int
j_math_addition(int a, int b)
{
  uint pntr[0];
  int solution;
  jboolean keep;
  uint value0, value1;

  value0 = 1 << (sizeof(int) * 8 -1);
  value1 = 1;

  if(value0 & a){
    if(_j_math_less(a, b))
      _j_math_subtraction(a, b, pntr, value1);
    else{
      _j_math_subtraction(b, a, pntr, value1);
      solution |= value0;
    }
  }else
    _j_math_addition(a, b, pntr, value1);

  return(solution);
}


void
j_math_uaddition(uint a, uint b, uint *destination, uint values)
{
  uint pntr[0];
  uint solution;
  jboolean keep;
  uint value0, value1;
  uint i;

  value0 = 1 << (sizeof(uint) * 8 -1);
  value1 = 1 << 1;

  if(value1 & values)
    keep = destination[1];
  else
    keep = FALSE;

  _j_math_addition(a, b, pntr, (1 | (1 << 1)));

  if(keep){
    if(value0 & a){
      if(value0 & b)
	solution |= value0;

      keep = TRUE;
    }else
      if(value0 & b)
	keep = TRUE;
      else
	solution |= value0;
  }else{
    if(value0 & a){
      if(value0 & b)
	keep = TRUE;
      else
	solution |= value0;
    }else if(value0 & b)
      solution |= value0;
  }

  i = 0;
  value1 = 1;

  if(value1 & values){
    destination[i] = solution;
    i++;
  }

  value1 = 1 << 1;

  if(value1 & values)
    destination[i] = keep;
}

void
_j_math_addition_list(JList *a, JList *b, uint *destination, uint values)
{
  JList *list, *tmp, *start, *zero;
  uint pntr[0];
  uint part;
  jboolean keep;
  uint value0, value1;
  uint i;

  keep = FALSE;

  if(a == NULL || b == NULL){
    keep = FALSE;
    start = NULL;
    list = NULL;
    goto _j_math_addition_listEND;
  }

  start =
    list = j_list_alloc();
  value1 = (1) | (1 << 1);
  __j_math_addition_list(a, b, pntr, value1, &part, list);

  while(a->prev != NULL && b->prev != NULL){
    a = a->prev;
    b = b->prev;
    list->prev = j_list_alloc();
    list->prev->next = list;
    list = list->prev;
    __j_math_addition_list(a, b, pntr, value1, &part, list);
  }

  list->prev = j_list_alloc();
  list->prev->next = list;
  list = list->prev;

  _j_math_addition((uint) a->data, (uint) b->data, pntr, value1);
  list->data = (jpointer) part;

  value0 = 1 << (sizeof(uint) * 8 -1);

  if(b->prev != NULL){
    b = b->prev;
    tmp = a;
    a = zero;
  }else if(a->prev != NULL){
    a = a->prev;
    tmp = b;
    b = zero;
  }else
    goto _j_math_addition_listEND;

  if(keep){
    if(!(value0 & ((uint) tmp->data))){
      list->data = (jpointer) (value0 | ((uint) list->data));
      keep = FALSE;
    }
  }else if(value0 & ((uint) tmp->data))
    list->data = (jpointer) (value0 | ((uint) list->data));

  zero->prev = j_list_alloc();
  zero->data = (uint) 0;

  while(tmp->prev != NULL){
    list->prev = j_list_alloc();
    list->prev->next = list;
    list = list->prev;

    __j_math_addition_list_zero(tmp, zero, pntr, value1, &part, list);

    tmp = tmp->prev;
  }

  _j_math_addition((uint) tmp->data, (uint) zero->data, pntr, value1);
  free(zero);

  if(keep){
    list->data = (jpointer) (value0 | ((uint) list->data));
    list->prev = j_list_alloc();
    list->prev->next = list;
    list = list->prev;
  }

  if(a->prev != NULL)
    a = tmp;
  else
    b = tmp;

 _j_math_addition_listEND:
  value1 = 1;
  i = 0;

  if(value1 & values){
    destination[i] = (uint) start;
    i++;
  }

  value1 <<= 1;

  if(value1 & values){
    destination[i] = (uint) list;
    i++;
  }

  value1 <<= 1;

  if(value1 & values){
    destination[i] = (uint) a;
    i++;
  }

  value1 <<= 1;

  if(value1 & values){
    destination[i] = (uint) b;
    i++;
  }
  /*
  void __j_math_addition_list(){
    j_math_uaddition((uint) a->data, (uint) b->data, pntr, value1);
    list->data = (jpointer) part;
  }

  void __j_math_addition_list_zero(){
    j_math_uaddition((uint) tmp->data, (uint) zero->data, pntr, value1);
    list->data = (jpointer) part;
  }*/
}

void
__j_math_addition_list(JList *a, JList *b, uint *pntr, uint value1, uint *part, JList *list)
{
  j_math_uaddition((uint) a->data, (uint) b->data, pntr, value1);
  list->data = (jpointer) (*part);
}

void
__j_math_addition_list_zero(JList *tmp, JList *zero, uint *pntr, uint value1, uint *part, JList *list)
{
  j_math_uaddition((uint) tmp->data, (uint) zero->data, pntr, value1);
  list->data = (jpointer) (*part);
}

JList*
j_math_addition_list(JList *a, JList *b)
{
  JList *list0, *list1;
  uint pntr[0];
  JList *point;
  JList *list;
  uint value, values;
  jboolean sign;

#ifdef __J_MATH_NO_LEADING_ZEROS__
  list0 = j_list_first(a);
  list1 = j_list_first(b);
#elif
  list0 = j_math_cut_leading_zeros(a);
  list1 = j_math_cut_leading_zeros(b);
#endif

  value = 1 << (sizeof(uint) * 8 -1);
  values = 1 | (1 << 2);

  if(value & ((uint) list0->data)){
    list0->data = (jpointer) (((uint) list0->data) ^ value);

    if(value & ((uint) list1->data)){
      list1->data = (jpointer) (((uint) list1->data) ^ value);
      _j_math_addition_list(a, b, pntr, values);
      sign = TRUE;
    }else{
      if(_j_math_less_list(list0, a->next, list1, b->next)){
	_j_math_subtraction_list(a, b, pntr, values);
	sign = TRUE;
      }else{
	 _j_math_subtraction_list(b, a, pntr, values);
	sign = FALSE;
      }
    }
  }else{
    if(value & ((uint) list1->data)){
      list1->data = (jpointer) (((uint) list1->data) ^ value);

      if(_j_math_less_list(list0, a->next, list1, b->next)){
	_j_math_subtraction_list(a, b, pntr, values);
	sign = TRUE;
      }else{
	_j_math_subtraction_list(b, a, pntr, values);
	sign = FALSE;
      }
    }else
      _j_math_addition_list(a, b, pntr, values);
  }

  if(sign)
    list->data = (jpointer) (((uint) list->data) | value);

  return(point);
}

JList*
j_math_uaddition_list(JList *a, JList *b)
{
  uint pntr[0];
  JList *point;

  _j_math_addition_list(a, b, pntr, 1);

  return(point);
}

void
_j_math_subtraction(int a, int b, uint *destination, uint values)
{
  int solution;
  jboolean keep;
  uint value0, stop;
  uint value1, i;

  stop = 1 << (sizeof(uint) * 8 -1);
  if(values & (1 << 2))
    keep = destination[1];
  solution = 0;

  for(value0 = 1; value0 != stop;){
    if(value0 & a){
      if(value0 & b){
	if(keep)
	  solution |= value0;
      }else if(!keep){
	solution |= value0;
	keep = TRUE;
      }
    }else if(value0 & b){
      if(!keep)
	solution |= value0;
    }else if(keep)
      solution |= value0;

    value0 = value0 << 1;
  }

  value1 = 1;
  i = 0;

  if(value1 & values){
    destination[i] = solution;
    i++;
  }

  value1 = 1 << 1;

  if(value1 & values)
    destination[i] = keep;
}

int
j_math_subtraction(int a, int b)
{
  uint pntr[0];
  int solution;
  jboolean keep;
  uint value, values;

  value = 1 << (sizeof(int) * 8 -1);

  values = 1 | (1 << 1);

  if(value & a){
    _j_math_addition(a, b, pntr, values);
    solution |= value;
  }else{
    if(_j_math_less(a, b)){
      _j_math_subtraction(a, b, pntr, values);
      solution |= value;
    }else
      _j_math_subtraction(b, a, pntr, values);
  }
  return(solution);
}

void
j_math_usubtraction(uint a, uint b, uint *destination, uint values)
{
  uint pntr[0];
  uint solution;
  jboolean keep;
  uint value0;
  uint value1, i;

  if((1 << 2) & values)
    keep = destination[1];

  _j_math_subtraction(a, b, pntr, (1 | (1 << 1)));
  value0 = 1 << (sizeof(uint) * 8 -1);

  if(keep){
    if(value0 & a){
      if(value0 & b)
	solution |= value0;

      keep = TRUE;
    }else if(value0 & b)
      keep = TRUE;
    else
      solution |= value0;
  }else{
    if(value0 & b){
      if(value0 & a)
	keep = TRUE;
      else
	solution |= value0;
    }else if(value0 & a)
      solution |= value0;
  }

  value1 = 1;
  i = 0;
  if(value1 & values){
    destination[i] = solution;
    i++;
  }

  value1 = 1 << 1;

  if(value1 & values)
    destination[i] = keep;
}

void
_j_math_subtraction_list(JList *a, JList *b, uint *destination, uint values)
{
  JList *point, *list,  *zero;
  uint pntr[0];
  uint part;
  jboolean keep;
  uint value1, i;

  zero = j_list_alloc();
  zero->data = (uint) 0;

  point =
    list = j_list_alloc();
  list->data = 0;

  if((1 << 2) & values)
    keep = destination[1];
  else
    keep = FALSE;

  value1 = 1 | (1 << 1);

  if(a == NULL && b == NULL){
    if(!keep)
      goto _j_math_subtraction_listEND;
    else{
      list =
	point = NULL;
      goto _j_math_subtraction_listEND;
    }
  }else{
    if(a == NULL)
      goto _j_math_subtraction_list2;
    else if(b == NULL)
      goto _j_math_subtraction_list3;
    else if(a->prev == NULL || b->prev == NULL)
      goto _j_math_subtraction_list1;
  }

  while(a->prev != NULL && b->prev != NULL){
    list->prev = j_list_alloc();
    list->prev->next = list;
    list = list->prev;
  _j_math_subtracion_list0:
    j_math_usubtraction((uint) a->data, (uint) b->data, pntr, value1);
    list->data = (jpointer) part;

    a = a->prev;
    b = b->prev;
  }

  list->prev = j_list_alloc();
  list->prev->next = list;
  list = list->prev;
 _j_math_subtraction_list1:
  _j_math_subtraction((uint) a->data, (uint) b->data, pntr, value1);
  list->data = (jpointer) part;

  a = a->prev;
  b = b->prev;

  while(a != NULL){
    list->prev = j_list_alloc();
    list->prev->next = list;
    list = list->prev;
  _j_math_subtraction_list2:
    j_math_usubtraction((uint) a->data, (uint) zero->data, pntr, value1);
    list->data = (jpointer) part;

    a = a->prev;
  }

  while(b != NULL){
    list->prev = j_list_alloc();
    list->prev->next = list;
    list = list->prev;
  _j_math_subtraction_list3:

    j_math_usubtraction((uint) zero->data, (uint) b->data, pntr, value1);
    list->data = (jpointer) part;

    b = b->prev;
  }

  j_math_cut_leading_zeros(point);
 _j_math_subtraction_listEND:

  free(zero);

  value1 = 1;
  i = 0;

  if(value1 & values){
    destination[i] = (uint) point;
    i++;
  }

  value1 <<= 1;

  if(value1 & values){
    destination[i] = (uint) list;
    i++;
  }

  value1 <<= 1;

  if(value1 & values){
    destination[i] = (uint) a;
    i++;
  }

  value1 <<= 1;

  if(value1 & values){
    destination[i] = (uint) b;
    i++;
  }
}

JList*
j_math_subtraction_list(JList *a, JList *b)
{
  JList *list0, *list1;
  uint pntr[0];
  JList *point;
  JList *start;
  uint value, value1;
  jboolean sign;

#ifdef __J_MATH_NO_LEADING_ZEROS__
  list0 = j_list_first(a);
  list1 = j_list_first(b);
#elif
  list0 = j_math_cut_leading_zeros(a);
  list1 = j_math_cut_leading_zeros(b);
#endif

  if(list0 != a && ((uint) list0->data) == 0)
    list0 = list0->next;

  if(list1 != b && ((uint) list1->data) == 0)
    list1 = list1->next;

  value = 1 << (sizeof(uint) * 8 -1); 
  value1 = 1 | (1 << 2);

  if(value & ((uint) list0->data)){
    if(value & ((uint) list1->data)){
      _j_math_addition_list(a, b, pntr, value1);
      sign = TRUE;
    }else if(_j_math_less_list(list0, a, list1, b)){
      _j_math_subtraction_list(a, b, pntr, value1);
      sign = TRUE;
    }else{
      _j_math_subtraction_list(b, a, pntr, value1);
      sign = FALSE;
    }
  }else{
    if(value & ((uint) list1->data)){
      if(_j_math_less_list(list0, a, list1, b)){
	_j_math_subtraction_list(a, b, pntr, value1);
	sign = TRUE;
      }else{
	_j_math_subtraction_list(b, a, pntr, value1);
	sign = FALSE;
      }
    }else{
      _j_math_addition_list(a, b, pntr, value1);
      sign = FALSE;
    }
  }

  if(value & ((uint) start->data)){
    start->prev = j_list_alloc();
    start->prev->next = start;
    start = start->prev;
    start->data = (uint) 0;
  }

  if(sign)
    start->data = (jpointer) ((uint) (start->data) | value);

  return(point);
}

JList*
j_math_usubtraction_list(JList *a, JList *b)
{
  uint pntr[0];
  JList *list;

  _j_math_subtraction_list(a, b, pntr, 1);

  return(list);
}

int
j_math_multiplication(int a, int b)
{
  int solution;
  int i;

  for(i = 0; i < b; i++)
    solution += a;

  return(solution);
}

JList*
j_math_multiplication_list(JList *a, JList *b)
{
  JList *list;

  return(list);
}

uint
j_math_umultiplication(uint a, uint b)
{
}

JList*
j_math_umultiplication_list(JList *a, JList *b)
{
  JList *list;

  return(list);
}

int
j_math_division(int a, int b, uint *destination, uint values)
{
  int value, value1, value2, value3;
  uint wa, wb, wc;
  int solution = 0;
  jboolean keep = FALSE;
  int i;
  uint value4;

  //  if(a == b){
    //    division_rest = 0;

  //    return(1);
  //  }

  wa = j_math_width(a);
  wb = j_math_width(b);
  wc = wa - wb;

  for(i = 0; i < wb; i++)
    value |= 1<<i;

  value2 = value | 1<<(i++);

  if(wc > 0)
    for(; wc >= 0; wc--){
      if(!keep){
	value1 = a>>wc;
	if(value1 > b){
	  solution |= 1<<wc;
	  a ^= value<<wc;
	  value3 = value1 - b;
	  a |= value3<<wc;
	}else
	  keep = 1;
      }else{
	value1 = a>>wc;

	solution |= 1<<wc;
	a ^= value<<wc;
	a ^= value2;
	value3 = value1 - b;
	a |= value3<<wc;

	keep = 0;
      }
      value2 <<= 1;
    }

  value4 = 1;
  i  = 0;

  if(value4 & values){
    destination[i] = solution;
    i++;
  }

  value4 <<= 1;

  if(value4 & values)
    destination[i] = value1;
}

void
j_math_division_list(JList *a0, JList *b0, uint *destination, uint values)
{
  JList *a1, *b1;
  JList *value0, *value1, *value2, *value3;
  JList *wa, *wb, *wc0, *wc1;
  JList *solution0, *solution1;
  jboolean keep = FALSE;
  JList *i0, *i1, *one, *zero;
  JList *tmp, *lm;
  uint i, value4;

  //  if(a == b){
    //    division_rest = 0;

  //    return(1);
  //  }

  wa = j_math_width_list(a0);
  wb = j_math_width_list(b0);
  wc0 = j_math_subtraction_list(wa, wb);
  
  i0 = j_list_alloc();
  i0->data = (jpointer) 0;
  one = j_list_alloc();
  one->data = (jpointer) 1;

  if(j_math_less_list(i0, wb))
    goto j_math_division_list0;

  for(; j_math_less_list(i0, wb); ){
    i1 = i0;
    j_list_destroy(i0);
  j_math_division_list0:
    lm = j_math_left_move_list(one, i0);
    tmp = value0;
    value0 = j_math_or_list(value0, lm);
    j_list_destroy(tmp);
    i0 = j_math_addition_list(i1, one);
  }

  i1 = i0;
  i0 = j_math_addition_list(i1, one);
  j_list_destroy(i1);
  lm = j_math_left_move_list_stepwise(one, i0);
  value2 = j_math_or_list(value0, lm);
  j_list_destroy(lm);

  zero = j_list_alloc();
  zero->data = (jpointer) 0;

  solution0 = 
    solution1 = j_list_alloc();
  solution0->data = (jpointer) 0;

  a0 =
    a1 = j_list_duplicate(a0);
  b0 =
    b1 = j_list_duplicate(b0);

  tmp = NULL;

  if(j_math_more_list(wc0, zero))
    if(j_math_equal_more_list(wc0, zero))
      goto j_math_division_list1;

    for(; j_math_equal_more_list(wc0, zero); ){
      j_list_destroy(wc0);
      tmp = value1;
    j_math_division_list1:
      wc1 = j_math_subtraction_list(wc0, one);

      if(!keep){
	value1 = j_math_right_move_list_stepwise(a0, wc0);
	j_list_destroy(tmp);

	if(j_math_more_list(value1, b0)){
	  lm = j_math_left_move_list_stepwise(one, wc0);
	  solution1 = j_math_or_list(solution0, lm);
	  j_list_destroy(solution0);
	  j_list_destroy(lm);
	  solution0 = solution1;
	  lm = j_math_left_move_list_stepwise(value0, wc0);
	  a1 = j_math_exclusive_or_list(a0, lm);
	  j_list_destroy(a0);
	  a0 = a1;
	  value3 = j_math_subtraction(value1, b0);
	  lm = j_math_left_move_list_stepwise(value3, wc0);
	  a1 = j_math_or_list(a0, lm);
	  j_list_destroy(a0);
	  a0 = a1;
	  j_list_destroy(lm);
	}else
	  keep = 1;
      }else{
	lm = j_math_left_move_list_stepwise(one, wc0);
	solution1 = j_math_or_list(solution0, lm);
	j_list_destroy(solution0);
	j_list_destroy(lm);
	solution0 = solution1;
	lm = j_math_left_move_list_stepwise(value0, wc0);
	a1 = j_math_exclusive_or_list(a0, lm);
	j_list_destroy(lm);
	j_list_destroy(a0);
	a0 = a1;
	a1 = j_math_exclusive_or(a0, value2);
	j_list_destroy(a0);
	a0 = a1;
	tmp = value3;
	value3 = j_math_subtraction_list(value1, b0);
	j_list_destroy(tmp);
	lm = j_math_left_move_list_stepwise(value3, wc0);
	a1 = j_math_or_list(a0, lm);
	j_list_destroy(a0);
	a0 = a1;

	keep = 0;
      }
      tmp = value2;
      value2 = j_math_left_move_list_stepwise(value2, one);
      j_list_destroy(value2);

      wc0 = wc1;
    }

  value4 = 1;
  i = 0;

  if(value4 & values){
    destination[i] = solution0;
    i++;
  }

  value4 <<= 1;

  if(value4 & values)
    destination[i] = value1;
}

uint
j_math_udivision(uint a, uint b)
{
}

JList*
j_math_udivision_list(JList *a, JList *b)
{
  JList *list;

  return(list);
}

int
j_math_rest(int a, int b)
{
  uint pntr[0];
  uint rest;

  j_math_division(a, b, pntr, (1 << 1));

  return(rest);
}

JList*
j_math_rest_list(JList *a, JList *b)
{
  uint pntr[0];
  JList *rest;

  j_math_division_list(a, b, pntr, (1 << 1));

  return(rest);
}

int
j_math_exp2(int a)
{
  int solution = a * a;

  return(solution);
}

int
j_math_exp(int a, int b)
{
  int solution;
  int i;

  if(b != 0){
    solution = a;
    for(i = 1; i < b; i++)
      solution *= a;
  }else
    solution = 1;
}

int
j_math_root2(int a)
{
  int value;

  return(value);
}

int
j_math_root(int a, int b)
{
  int value;

  return(value);
}

void
j_math_make_valid(char *string)
{
  char *string0, *string1;
  /*
  string0 = string;

  while((string0 = j_string_find_char(string0, ' ')) != NULL)
    j_string_delete(string0, 0);
  
  string0 = string;

  while((string0 = j_string_find_char(string0, '(')) != NULL){
    string1 = string0;

    while(!j_math_is_operator(string1) && string1 >= string)
      string1--;

    if(!j_math_is_function(string1))
      j_string_insert(string0, "*\0", 0);
  }
  */
}

jboolean
j_math_is_operator(char *string)
{
  if(string[0] == '=' ||
     string[0] == '+' ||
     string[0] == '-' ||
     string[0] == '*' ||
     string[0] == '/' ||
     string[0] == '%' ||
     string[0] == '<' ||
     string[0] == '>' ||
     j_string_ncompare(string, "<=\0", 2) ||
     j_string_ncompare(string, ">=\0", 2))
    return(TRUE);
  else
    return(FALSE);
}

jboolean
j_math_is_function(char *string)
{
  char *offset;
  uint lenght;

  if((offset = j_string_find_char(string, '(')) == NULL)
    return(FALSE);

  lenght = (uint) (offset - string);

  if(lenght == 2){
    return(j_string_ncompare("ln\0", string, lenght));
  }else if(lenght == 3){
    if(j_string_ncompare("sin\0", string, lenght) ||
       j_string_ncompare("cos\0", string, lenght) ||
       j_string_ncompare("tan\0", string, lenght) ||
       j_string_ncompare("log\0", string, lenght))
      return(TRUE);
    else
      return(FALSE);
  }else
    return(FALSE);
}

jboolean
j_math_is_function_backward(char *start, char *string)
{
  for(; string != start && *string != '('; string--);

  string--;

  if(string == start)
    return(FALSE);
  else{
    if((string = string -2) <= start)
      return(FALSE);
    else
      if(j_string_ncompare("ln\0", string))
	return(TRUE);
      else if((string--) <= start)
	return(FALSE);
      else if(j_string_ncompare("sin\0", string))
	return(TRUE);
      else if(j_string_ncompare("cos\0", string))
	return(TRUE);
      else if(j_string_ncompare("tan\0", string))
	return(TRUE);
      else if(j_string_ncompare("log\0", string))
	return(TRUE);
      else
	return(FALSE);
  }
}

char*
j_math_get_function(char *string)
{
  char *end;
  jboolean success;

#ifdef __J_ALTERNATE_H__
  j_regexp("/%s/[[sin],[cos],[tan],[log],[ln]]([[a-z]*,[A-Z]*,[0-9]*,[_]*]$0)/%p$0/", string, end);
#else
  success = FALSE;

  if((end = j_string_find(string, "sin(\0")) != NULL)
    success = TRUE;
  else if((end = j_string_find(string, "cos(\0")) != NULL)
    success = TRUE;
  else if((end = j_string_find(string, "ln(\0")) != NULL)
    success = TRUE;
  else if((end = j_string_find(string, "log(\0")) != NULL)
    success = TRUE;
  else if((end = j_string_find(string, "tan(\0")) != NULL)
    success = TRUE;

  if(!success)
    return(NULL);
#endif
  return(end);
}

char*
j_math_get_function_backward(char *start, char *string)
{
  for(; string >= start; string--){
  }
}

char*
j_math_get_operator(char *string)
{
  for(; *string != '+' || *string != '-' || *string != '*' || *string != '/' || *string != '%' || *string != '\0'; string++);

  if(*string == '\0')
    return(NULL);
  else
    return(string);
}

char*
j_math_get_first_level_operator(char *string)
{
#ifdef __J_ALTERNATE_H__
  char *multiply, *over, *rest;

  multiply = j_string_find_char(string, '*');
  over = j_string_find_char(string, '/');
  rest = j_string_find_char(string, '%');

  if((multiply == NULL || over < multiply) && (rest == NULL || over < rest))
    return(over);
  else if(rest == NULL || over < rest)
    return(multiply);
  else
    return(rest);
#endif

  for(; *string != '*' || *string != '/' || *string != '%'; string++);

  if(string != '\0')
    return(NULL);
  else
    return(string);
}

char*
j_math_get_second_level_operator(char *string)
{
#ifdef __J_ALTERNATE_H__
  char *plus, *minus;

  minus = j_string_find_char(string, '-');
  plus = j_string_find_char(string, '+');

  if(minus == NULL || plus < minus)
    return(plus);
  else
    return(minus);
#endif

  for(; *string != '-' || *string != '+'; string++);

  if(*string != '\0')
    return(string);
  else
    return(NULL);
}

int
j_math_solve_arithmetic(char *string)
{
}
/*
int
j_math_solve_arithmetic_text(JText *text)
{
}
*/
char*
j_math_get_variable(char *string)
{
  char *offset;

  //  j_regexp("/%s/[' '*]$0[[[A-Z],[a-z],[_]]+,[[A-Z],[a-z],[_],[0-9]]*]/%p$0/\0", string, offset);

  return(offset);
}

char*
j_math_algebra_get_variable_backward(char *start, char *string)
{
  char *offset;

  //  j_regexp_backward("/[%s-%s]/*$0[[[A-Z],[a-z],[_]]+,[[A-Z],[a-z],[_],[0-9]]*][' '*]['+','-','*','/','%'][['(',')']*]/%p$0/\0", start, string, offset);

  return(offset);
}

void
j_math_sort_brackets(char *string, uint *destination, uint flags)
{
  char **opened, **closed;
  char *string0, *string1;
  uint size;
  uint *level, level0;
  char ***bracket;
  uint i, j, k;
  jboolean valid;
  jboolean previous_level = FALSE;
  uint value;

  size = j_string_count_char(string, '(');
  opened = (char**) malloc(size * sizeof(char*));
  closed = (char**) malloc(size * sizeof(char*));
  string0 =
    string1 = string;

  for(i = 0; i < size; i++){
    string0 =
      opened[i] = j_string_find_char(string0, '(');
    string1 =
      closed[i] = j_string_find_char(string1, ')');
  }

  if(string1 == NULL || j_string_find_char(string1, ')') != NULL){
    valid = FALSE;
    opened = NULL;
    closed = NULL;
    level = NULL;
    goto j_math_sort_bracketsEND1;
  }

  level = (uint*) malloc(size * sizeof(uint));
  bracket = (char***) malloc(size * sizeof(char**));

  for(i = 0, level0 = 0; i < size; i++){
    for(j = 0; j < size -1; j++){
      if(opened[j] < closed[i]){
	if(opened[j+1] > closed[i]){
	  level[i] = level0;
	  bracket[i] = (char**) malloc(2 * sizeof(char*));
	  bracket[i][0] = opened[j];
	  bracket[i][1] = closed[i];

	  if(previous_level){
	    level0 -= 1;
	    previous_level = FALSE;
	  }
	  break;
	}else{
	  previous_level = TRUE;
	  level0++;
	}
      }else{
	valid = FALSE;
	i++;
	goto j_math_sort_bracketsEND0;
      }
    }
  }

  if(opened[size -1] > closed[0]){
    if(opened[size -1] < closed[size -1]){
      for(j = 1; j < size; j++)
	if(opened[size -1] > closed[j]){
	  bracket[size - 1][0] = opened[size -1];
	  bracket[size - 1][1] = closed[j -1];
	  goto j_math_sort_brackets0;
	}else
	  level0--;
      bracket[size - 1][0] = opened[0];
      bracket[size - 1][1] = closed[0];
    j_math_sort_brackets0:

      level[size -1] = level0;
    }else{
      valid = FALSE;
      goto j_math_sort_bracketsEND0;
    }
  }else if(opened[size -1] < closed[0]){
    level[size -1] = size -1;
    bracket[size -1][0] = opened[size -1];
    bracket[size -1][1] = closed[0];
  }else{
    valid = FALSE;
    goto j_math_sort_bracketsEND0;
  }

  for(i = 0; i < size -1; i++){
    for(j = i; j < size -1; j++){
      if(level[j] < level[i]){
	closed[i] = bracket[j][0];
	opened[i] = bracket[j][1];

	for(k = i + 1; k <= j; k++){
	  opened[k] = bracket[k][0];
	  closed[k] = bracket[k][1];
	}
      }
    }
  }

  valid = TRUE;
 j_math_sort_bracketsEND0:
  for(i -= 1; i >= 0; i--)
    free(bracket[i]);

  free(bracket);

 j_math_sort_bracketsEND1:

  value = 1;
  i = 0;

  if(value & flags){
    destination[0] = (uint) opened;
    i++;
  }else if(opened != NULL)
    free(opened);

  value <<= 1;

  if(value & flags){
    destination[i] = (uint) closed;
    i++;
  }else if(closed != NULL)
    free(closed);

  value <<= 1;

  if(value & flags){
    destination[i] = (uint) level;
    i++;
  }else if(level != NULL)
    free(level);

  value <<= 1;

  if(value & flags){
    destination[i] = (uint) valid;
    i++;
  }

  value <<= 1;

  if(value & flags)
    destination[i] = (uint) size;
}

char*
j_math_algebra_brackets_out(char *string)
{
  /*
  JList *va, *va_current;
  char *solution, *current;
  char *operator, *operator_end;
  char *variable_end;
  uint lenght;
  jboolean function;
  char *string;
  uint pntr[0];
  char **opened, **closed;
  uint *level, size;
  jboolean valid;
  uint value;
  uint i, j, k, l;

  solution = NULL;
  va = NULL;
  // solution = j_regexp("/%s/*(/%s/", string, solution);
  value = 1 | (1 << 1) | (1 << 2) | (1 << 3);
  j_math_sort_brackets(string, pntr, value);

  if(!valid){
    solution = NULL;
    goto j_math_brackets_outEND;
  };

  for(i = 0; i < size; i++){
    va = j_list_append(va);
    va->data =
      va_current = j_list_alloc();
    goto j_math_algebra_brackets_out0;

    for(j = i; level[j] == level[i] && i < size; i++){
      for(current = opened[i]; current < closed[i];){
	if(j_regexp("/%s/%p[$0['*','/','%','+','-']*$1,[[[[[a-z],[A-Z],[_]]+,[[a-z],[A-Z],[_],[0-9]]*]]$2,[%p[[ln,log,sin,cos,tan],'(',*, ')'%p]]]]/%p$0%p$1%p$2/\0", string, current0, current0, closed[i], operator, operator_end, variable_end)){
	  va_current = j_list_append(va_current);
	j_math_algebra_brackets_out0:
	  va_current = j_list_append(va_current);
	  va_current->data = operator;
	  va_current = j_list_append(va_current);
	  va_current->data = operator_end;
	  va_current->data = current;
	  va_current = j_list_append(va_current);
	  va_current->data = variable_end;
	  current = variable_end++;
	}else if(j_regexp("/%s/%p[$0['+','-','*','/','%']$1*]/%p$0%p$1/\0", string, current, operator, operator_end)){
	  va_current = j_list_append(va_current);
	  va_current->data = operator;
	  va_current = j_list_append(va_current);
	  va_current->data = operator_end;
	}
      }
    }
  }

 j_math_brackets_outEND:
  free(opened);
  free(closed);
  free(level);

  va = j_list_first(va);

  while(va != NULL){
    va_current = va;

    while(va_current != NULL){
      //      solution = j_string_insert(solution, va_current->data, );

      va_current = va_current->next;
    }
    va = va->next;
  }

  return(solution);
  */
  /*
  if((current1 = closing_bracket = j_string_find_char(string, ')')) != NULL && (current0 = opening_bracket = j_string_find_char_backwards(string, closing_bracket, '(')) != NULL){
    string = j_string_duplicate(string);
    function = FALSE;
    goto j_math_algebra_brackets_out1;
  }else
    return(string);

  while(TRUE){
    va = j_list_append(va);
    function = FALSE;
  j_math_algebra_brackets_out0:
    if((current1 = closing_bracket = j_string_find_char(current1, ')')) != NULL && (current0 = opening_bracket = j_string_find_char_backward(string, current0, '(')) != NULL)
      break;
  j_math_algebra_brackets_out1:
    if(current0 != string){
      current0--;
    }else
      goto j_math_algebra_brackets_out2;

    if(){
    }else if(j_regexp("/%s/*$0['-', '+', '*', '/', '%']*[ln,log,cos,sin,tan]%p[' '*]([[[[[a-z]*, [A-Z]*, [_]*]+, [0-9]*], [0-9]*]+])$1/%p$0%p$1/\0", string, current, variable)){
      current0--;
      goto j_math_algebra_brackets_out0;
    }else if(j_regexp("/%s/*$0['-', '+', '*', '/', '%']*[[a-z]*,[A-Z]*,[0-9]*,[_]*['+', '-', '*', '/', '%']*]$1[' ']*%p*\0/%p$0%p$1/\0", string, current0, variable, variable_end)){
      va = ;
    }

 j_math_algebra_brackets_out2:
    for(current = opening_bracket; current != closing_bracket; current++){
      if(*current == ' '){
      }else if(){
      }
    }
*/
    /*
    sign = closing_bracket[1];
    operator0 = string;
    variable0 = j_math_algebra_get_variable_backward(string, opening_bracket);

    if((operator0 = j_math_get_first_level_operator(operator)) != NULL && operator0 < opening_bracket)
      goto j_math_algebra_brackets_out0;
    else
      goto j_math_algebra_brackets_out1;
    */
    /* 
    while(TRUE){
      operator0 = j_math_get_first_level_operator(operator);

      if(operator0 == NULL || operator0 > opening_bracket){
	if((variable = j_math_algebra_get_variable(operator1) == NULL || variable > opening_bracket){
	  if(*variable0 != NULL && *variable0 < opening_bracket && (*operator1 == NULL || *operator1 < *variable0)){
	  }else if(*operator1 == '+'){
	  }else if(*operator1 == '-'){
	  }else if(*operator1 == '*'){
	  }else if(*operator1 == '/'){
	  }else if(*operator1 == '%'){
	  }

	  if(*variable1 != NULL && *variable0 < opening_bracket && (*operator1 == NULL || *operator1 < *variable0)){
	  }else if(*operator3 == '-'){
	  }else if(*operator3 == '+'){
	  }else if(*operator3 == '*'){
	  }else if(*operator3 == '/'){
	  }else if(*operator3 == '%'){
	  }
	}else{
	}

	goto j_math_algebra_bracket_out0;
      }

    j_math_algebra_brackets_out0:
      operator1 = operator0;
    }
    */
  /*
  j_math_algebra_brackets_out1:
    if(string == opening_bracket){
    }
  */
}

char*
j_math_algebra_operators_out(char *string)
{
  char **var;
  uint var_count;
  char operator;
}

char*
j_math_algebra_functions_out(char *string)
{
}

int*
j_math_algebra_solve(char *string, char *operation)
{
}
/*
int*
j_math_algebra_solve_text(JText *text)
{
}
*/
int*
j_math_gauss(char **string, uint count)
{
}
