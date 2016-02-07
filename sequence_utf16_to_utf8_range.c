int
main(int argc, char **argv)
{  
  char *lower, *upper;
  char *str;

  unsigned int upper_value, lower_value;
  unsigned int msb;

  unsigned int bit_one = 0x80000000;
  
  static char **mask_1_byte = {
    "0xxxxxxx\0",
    NULL,
  };
  static char **mask_2_byte = {
    "110xxxxx\0",
    "10xxxxxx\0",
    NULL,
  };
  static char **mask_3_byte = {
    "1110xxxx\0",
    "110xxxxx\0",
    "10xxxxxx\0",
    NULL,
  };
  static char **mask_4_byte = {
    "11110xxx\0",
    "1110xxxx\0",
    "110xxxxx\0",
    "10xxxxxx\0",
    NULL,
  };
  static char **mask_5_byte = {
    "1111110xx\0",
    "111110xxx\0",
    "11110xxxx\0",
    "1110xxxxx\0",
    "10xxxxxxx\0",
    NULL,
  };
  static char **mask_6_byte = {
    "1111110x\0",
    "10xxxxxx\0",
    "10xxxxxx\0",
    "10xxxxxx\0",
    "10xxxxxx\0",
    "10xxxxxx\0",
    NULL,
  };

  static unsigned int *range = {
    0x00000000,
    0x0000007f,
    0x00000080,
    0x000007ff,
    0x00000800,
    0x0000ffff,
    0x00010000,
    0x001fffff,
    0x00200000,
    0x03ffffff,
    0x04000000,
    0x7fffffff,
  };

  if(argc != 3){
    fprintf(stderr, "takes 2 arguments lower and upper\n\0");
    return(-1);
  }
  
  lower = argv[1];
  upper = argv[2];

  sscanf(lower,
	 "%x\0",
	 &lower_value);
  
  sscanf(upper,
	 "%x\0",
	 &upper_value);

  str = NULL;

  if(){
  }

  printf("%s\n\0", str);
  
  return(0);
}
