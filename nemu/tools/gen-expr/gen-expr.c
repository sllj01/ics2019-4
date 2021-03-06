#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>


// this should be enough
static char buf[65536];

/***pa1.2  gen_rand_expr returns a string, so each gen-func updates a char into buf***/

static int expr_end = 0;/*use to point to the next location in buf for generation*/

static inline void gen_num(){
	//buf[expr_end] = '0' + rand()%10;/*error TODO: num in expr only has one digit*/
	//expr_end++;
	int len = rand()%4;/*only generate at most 4 digit number*/
	if (len != 0){
		buf[expr_end] = '1' + rand()%9;
	}else{
		buf[expr_end] = '0' + rand()%10;
	}
	expr_end++;

	for(int i=1;i<=len;i++){
		buf[expr_end] = '0' + rand()%10;/*this will generate number like 0001*/
		expr_end++;
	}
}
static inline void gen(char c){
	buf[expr_end] = c;
	expr_end++;
}
static inline void gen_rand_op(){
	switch(rand()%4){
		case 0:	buf[expr_end]='+';break;
		case 1:	buf[expr_end]='-';break;
		case 2:	buf[expr_end]='*';break;
		case 3:	buf[expr_end]='/';break;
	}
	expr_end++;
}
static inline uint32_t choose(uint32_t n){
	return (rand()%n);
}

#define SPACE if(rand()%2){gen(' ');}/*still stupid way to add random <SPACE>*/

static inline void gen_rand_expr() { /*gen_rand_expr is a random&&recursion func!!*/
  //buf[0] = '\0';
	//if(expr_end>=20)	return;/*control the length of rand_expr, fail*/
	switch(choose(3)){
		case 0:	SPACE gen_num(); SPACE
						//if(expr_end>=20) return;/*bad way to control the length of expr*/ 
						break;
		case 1: SPACE gen('('); SPACE gen_rand_expr(); SPACE gen(')'); SPACE
						break;
		default:	SPACE gen_rand_expr(); SPACE gen_rand_op(); SPACE gen_rand_expr(); SPACE
						break;
	}
}

static char code_buf[65536];/*code_buf will generate an instance of code_format*/
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);/*these means we can use rand(),it returns non-negative number*/
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    expr_end = 0;/*everytime to generate an expr should initialize this pointer*/
		gen_rand_expr();
		buf[expr_end] = '\0';/*and add '\0' to finish the generation*/

    sprintf(code_buf, code_format, buf);/*buf here is %s!!!and buff is the expr!!!*/

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);/*here code_buf is a c file!!!and wrote to .code.c!!*/
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");/*and generate a executable file*/
    if (ret != 0) continue;
		
		//int iswrong = system("/tmp/.code.c");/*this way fail*/
		//if (iswrong == -1) continue;
    fp = popen("/tmp/.expr", "r");/*this is a pipe!! /tmp/.expr is a command!!!*/
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
