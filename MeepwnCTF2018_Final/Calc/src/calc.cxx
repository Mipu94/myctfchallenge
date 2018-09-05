#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* getenv */
#include <unistd.h>
#include "calc.h"

bool is_Zero(Number *x)
{
	if( !x->is_big  && x->value == 0 ) return true;
	return false;
}

Number* new_Number()
{
	Number* pNum = (Number*) malloc(sizeof(Number));
	pNum->is_big = false;
	pNum->sign = false;
	pNum->value = 0;
	pNum->size = 0;
	pNum->strNum = 0;
	return pNum;
}

Node *new_Node(Operator opr, Number *pNum)
{
	Node *pNode = (Node*) malloc(sizeof(Node));
	pNode->opr.op = opr.op;
	pNode->opr.priority = opr.priority;
	pNode->v1 = pNum;
	return pNode;
}

char* StrNum_clear(char* StrNum,size_t size)
{
	char *p = StrNum;
	for(int i=0; i< size; i++ )
		if (StrNum[i] == '0') p++;
		else break;
	return p; 
}
// void print_num(Number *pNum)
// {
// 	if(pNum->is_big) printf("Number_big: %s size:%zu\n",pNum->strNum,pNum->size);
// 	else printf("Number_small: %lld size:%zu\n",pNum->value,pNum->size);
// }
bool is_smaller(char* strNum1, char* strNum2)
{
	int n1= strlen(strNum1), n2 = strlen(strNum2);
	if (n1 < n2)
    return true;
    if (n2 < n1)
    return false;
 
    for (int i=0; i<n1; i++)
    if (strNum1[i] < strNum2[i])
        return true;
    else if (strNum1[i] > strNum2[i])
        return false;
    return false;
}

int toInteger(char c)
{
	if (c >= '0' && c <= '9') return c -'0';
	return 10;
}

char getCharacter()
{
	DEBUG_PRINT("getchar %c",INPUT[_index]);
	return INPUT[_index];
}

int getInteger()
{
	return toInteger(getCharacter());
}

void freeNumber(Number *pNum)
{
	char buf[MAX_NUM+0x20];
	if(pNum->is_big) 
	{
		if(IS_DEBUG)
		{
			sprintf(buf,"Freeing big num: %s",pNum->strNum);
			puts(buf);
		}
		free(pNum->strNum);
	}
	free(pNum);
}

void freeNode(Node *pNode)
{
	freeNumber(pNode->v1);
	free(pNode);
}
void eatSpaces()
{
	char c = INPUT[_index];
	while(c == ' ') c = INPUT[++_index];
}

void unexpected()
{
	printf("unexpected\n");
	return;
}
Node* top()
{
	return stack[p_stack];
}
void pop()
{
	char buf[0x70];
    if( p_stack == -1){perror("STACK IS EMPTY"); exit(-1);}
    if(IS_DEBUG)
	{
		sprintf(buf,"POP -> Current_SP: %d, OP_Priority: %d", p_stack, stack[p_stack]->opr.priority);
		puts(buf);
		// print_num(top()->v1);
	}
    freeNode(stack[p_stack--]);
}
void push(Node *pNode)
{
	char buf[0x60];
	if(IS_DEBUG)
	{
		sprintf(buf,"PUSH -> Current_SP: %d, OP_Priority: %d", p_stack, pNode->opr.priority);
		puts(buf);
		// print_num(pNode->v1);
	}
    if(p_stack == MAX_SIZE) { perror("STACKKKKK OVERFLOW");exit(-1);}
    p_stack++;
    stack[p_stack] = pNode;
}


void swap(Number **v1, Number **v2)
{
	Number *tmp;
	tmp = *v1;
	*v1	= *v2;
	*v2	= tmp;
}

int get_lenSmallnum(long long int x)
{
	int len = 1;
	while(x > 0)
	{
		x = x/10;
		len += 1;
	}	
	return len;
}

Number *add_Num(Number *v1, Number *v2)
{
	int i, width, diff, sum=0, carry=0;
	char buf[0x48];
	memset(buf,0,0x41);
	char result[MAX_NUM+8];
	memset(result,0,MAX_NUM+1);

	Number *pNum = new_Number();
    pNum->sign = v1->sign; //add only for 2 nums same sign 
	
	if( !v1->is_big && !v2->is_big) //add 2 small value
	{		
		long long int sum = v1->value + v2->value;
		sprintf(buf,"%lld",sum);
		pNum->size  = strlen(buf);
		if (sum > 2147483640) 	
		{
			pNum->is_big = 1;
			pNum->strNum = strdup(buf);
			return pNum;
		}
		pNum->value = sum;
		return pNum;
	}

	if( (!v1->is_big && v2->is_big) | (v1->is_big && !v2->is_big ) ) // add big + small
    {
    	pNum->is_big = 1;
    	if(v1->is_big) swap(&v1,&v2);//confirm v1 is small number
    	long long int v2_tail = atol( &(v2->strNum[v2->size - v1->size]) );
    	long long int tmp_value = v1->value + v2_tail;
    	sprintf(buf,"%lld",tmp_value);// more -1 for carry 
    	if(strlen(buf) == v1->size) carry = 0; //check if carry
    	else carry = 1;
		memcpy(&result[MAX_NUM - v1->size], buf, strlen(buf));
		int tmp_carry = carry;
		for (int i =v2->size -v1->size -1; i >=0; i--) // add carry to str2 remain
	    {
	        int sum = (toInteger(v2->strNum[i])+carry);
	        result[MAX_NUM -v2->size +i +tmp_carry]= sum%10 + '0';
	        carry = sum/10;
	    }
	    if(carry)
	    {
	    	if(v2->size == MAX_NUM) { perror("OVERFLOW NAH NAH NAH!"); exit(-1);}
	    	result[MAX_NUM-v2->size-1+tmp_carry] = '1';
	    	pNum->strNum = strdup(&result[MAX_NUM-v2->size-1+tmp_carry]);
	    	pNum->size = v2->size+1;
	    }
	    else
    	{
    		pNum->size = v2->size;
    		pNum->strNum = strdup(&result[MAX_NUM-v2->size+tmp_carry]);
    	}
    	return pNum;	
    }

    if( v1->is_big && v2->is_big ) // add 2 big_num
    {
    	pNum->is_big = 1;
	    if(v2->size < v1->size) swap(&v1,&v2) ; // make sure v2_size > v1_size 
	   	diff = v2->size - v1->size;
	   	
	   	for (int i=v1->size-1; i>=0; i--)
   		{
	        int sum = toInteger(v1->strNum[i]) + toInteger(v2->strNum[i+diff]) +carry;
	       	result[MAX_NUM -v1->size +i]= sum%10 + '0';
	        carry = sum/10;
   		}
	    
   		for (int i=diff-1; i>=0; i--) // add carry to str2 remain
	    {
	        int sum = (toInteger(v2->strNum[i])+carry);
	        result[MAX_NUM -v2->size +i]= sum%10 + '0';
	        carry = sum/10;
	    }

	    if(carry)// add last carry
	    {
	    	if(v2->size == MAX_NUM) { perror("OVERFLOW NAH NAH NAH!");exit(-1); }
	    	result[MAX_NUM-v2->size-1] = '1';
	    	pNum->size = v2->size+1;
	    	pNum->strNum = strdup(&result[MAX_NUM-v2->size-1]);
	    }
	    else
    	{
    		pNum->size = v2->size;
    		pNum->strNum = strdup(&result[MAX_NUM-v2->size]);
    	}
    	return pNum;
    }
    return 0;
}

Number *sub_Num(Number *v1, Number *v2)
{
	char result[MAX_NUM+8];
	memset(result,0,MAX_NUM+1);
	Number *pNum = new_Number();
	int i, width, diff, sum=0, carry=0;
	if(!v1->is_big && !v2->is_big) // sub 2 small value
	{
		if(v1->sign) swap(&v1,&v2);// make sure v1 > v2
		long long int tmp = v1->value - v2->value;

		if(tmp < 0) 
		{
			pNum->sign = true;
			pNum->value = -tmp;
			sprintf(result,"%lld",-tmp);
			pNum->size  = strlen(result);
		}
		else 
		{
			sprintf(result,"%lld",tmp);
			pNum->size  = strlen(result);
			pNum->value = tmp;
		}

		return pNum;		
	}
	if( (!v1->is_big && v2->is_big) | (v1->is_big && !v2->is_big ) ) // sub big and small
	{
    	if(v1->is_big) swap(&v1,&v2);//confirm v1 is small number
    	pNum->sign = v2->sign;
    	memcpy(&result[1], &(v2->strNum[v2->size - v1->size]), v1->size);
		long long int v2_tail = atol(&result[1]);
		if(v2_tail < v1->value) 
		{
			carry = 1;
			result[0]='1';
			v2_tail = atol(result);
		} 
    	long long int tmp_value = v2_tail - v1->value;
    	sprintf(&result[MAX_NUM-v1->size],"%lld",tmp_value);
    	for (i =v2->size - v1->size -1; i >=0; i--) // sub carry to str2 remain
	    {
	    	if(carry && toInteger(v2->strNum[i])==0)
	    	{
	    		carry = 1;
	    		sum = 9;
	    	}
	    	else
	    	{
	    		sum = toInteger(v2->strNum[i])-carry;
	    		carry = 0;
	    	}
	        
	        result[MAX_NUM -v2->size +i]= sum + '0';
	    }
	    char *p = StrNum_clear(&result[MAX_NUM -v2->size],v2->size);
		pNum->size = strlen(p);
		if(atol(p) > 2147483640)
		{
			pNum->is_big = true;
			pNum->strNum = strdup(p);
			pNum->size = strlen(p);
		}
		else
		{
			pNum->value = atol(p);
			pNum->size = strlen(p);
		}
		return pNum;
	}
	if( v1->is_big && v2->is_big ) // sub 2 big_num
	{
		if(is_smaller(v2->strNum,v1->strNum)) swap(&v1,&v2);// confirm value(v1) <= value(v2) that mean v1->size <= v2->size
		pNum->sign = v2->sign;
		diff = v2->size - v1->size;
		//go: v2->strNum - v1->strNum 
	   	for (int i=v1->size-1; i >=0; i--) 
   		{
			if( toInteger(v1->strNum[i])+carry > toInteger(v2->strNum[i+diff]) )
			{
				sum = 10 + toInteger(v2->strNum[i+diff]) - toInteger(v1->strNum[i]) - carry;
				carry = 1;
			}
			else
			{
				sum = toInteger(v2->strNum[i+diff]) - toInteger(v1->strNum[i]) - carry;
				carry = 0;
			}

	       	result[MAX_NUM -v1->size +i]=sum + '0';
   		}

		for (int i=diff-1; i>=0; i--) // sub carry v2 remain 
	    {
	    	if(carry && toInteger(v2->strNum[i])==0)
	    	{
	    		carry = 1;
	    		sum = 9;
	    	}
	    	else
	    	{
	    		sum = toInteger(v2->strNum[i])-carry;
	    		carry = 0;
	    	}
	        result[MAX_NUM -v2->size +i]= sum + '0';
	    }
	    char *p = StrNum_clear(&result[MAX_NUM -v2->size],v2->size);
		pNum->size = strlen(p);
		if(atol(p) > 2147483640)
		{
			pNum->is_big = true;
			pNum->strNum = strdup(p);
			pNum->size = strlen(p);
		}
		else
		{
			pNum->value = atol(p);
			pNum->size = strlen(p);
		}
		return pNum;
	}
	return 0;
}

void add_nullbyte(char*data)
{
	int i = 0;
	while(data[i] != 0)
	{
		if(data[i] == 10)
		{
			data[i] = 0;
			return;
		}
		i++;
	}
}
Number *mul_Num(Number *v1, Number *v2)
{
	Number *pNum = new_Number();
	char result[MAX_NUM+8];
	memset(result,0,MAX_NUM+1);
	if(v1->is_big | v2->is_big)
	{
		if(!UNEXP)
		{
			UNEXP = true;
			BUF_UNEXP = strdup("");
			printf("Sorry, We haven't supported for multiple big numbers yet!\n You can leave your info to help us contribute :)\n>> ");
			if(!AtributeInfo) AtributeInfo = (char*)calloc(1,0x30);
			read(0,AtributeInfo,47);
			add_nullbyte(AtributeInfo);
		}
		return pNum;
	}
	long long int tmp = v1->value*v2->value;
	pNum->sign = v1->sign^v2->sign;
	sprintf(result,"%lld",tmp);
	pNum->size = strlen(result);
	if(tmp > 2147483640)
	{
		pNum->is_big = 1;
		pNum->strNum = strdup(result);
	}
	else pNum->value = tmp;
	return pNum;
}

void correct_zero(Number *v1,Number *v2)
{
	char buf[MAX_NUM+8];

		if(v1->is_big) sprintf(buf,"%s", v1->strNum);
		else sprintf(buf,"%lld", v1->value);
		if(!UNEXP) printf("Can't calculate %s/%lld\nAuto correction: %s/1\n",buf,v2->value,buf);
		v2->value = 1;
}

Number *div_Num(Number *v1, Number *v2)
{
	Number *pNum = new_Number();
	int tmp=0, c=0;

	if(!v2->is_big && v2->value == 0) correct_zero(v1,v2);
	if(v2->is_big && !v1->is_big) return pNum; // small_num/big_num = 0
	if(v1->is_big || v2->is_big)
	{
		if(!UNEXP)
		{
			UNEXP = true;
			BUF_UNEXP = strdup("");
			printf("Sorry, We haven't supported for division big numbers yet!\n You can leave your info to help us contribute :)\n>> ");
			if(!AtributeInfo) AtributeInfo = (char*)calloc(1,0x30);
			read(0,AtributeInfo,47);
			add_nullbyte(AtributeInfo);
		}
		return pNum;
	}

	pNum->sign = v1->sign^v2->sign;
	tmp = (int)v1->value/(int)v2->value;
	pNum->value = tmp;
	if(tmp == 0)
	{
		pNum->size = 1;
		return pNum;
	}
	while(tmp)
	{
		tmp = tmp/10;
		c++;
	}
	pNum->size = c;
	return pNum;
}

Number *parseNumberDecimal() 
{
	char buf[MAX_NUM+8];
	memset(buf,0,MAX_NUM+1);
	int idx = 0;
	Number* pNum = new_Number();
	for (int d; (d = getInteger()) <= 9; _index++)
	{
		buf[idx++] = getCharacter();
		if( idx > MAX_NUM-1) { perror("Your number too big"); exit(-1); }	  
		if (pNum->value > 2147483640)
		{
			pNum->is_big = 1;
		}
		else pNum->value = pNum->value*10 + d;
	}
	char *p = StrNum_clear(buf,idx);
	if(pNum->is_big) pNum->strNum = strdup(p);
	pNum->size = strlen(p);
	return pNum;
}

Operator parseOp()
{
	eatSpaces();
	DEBUG_PRINT("OPERATOR CHAR %c\n",getCharacter());
	switch (getCharacter())
	{
	  case '+': _index++;	return Operator(OPERATOR_ADD,	1);
	  case '-': _index++;	return Operator(OPERATOR_SUB,	1);
	  case '*': _index++;	return Operator(OPERATOR_MUL, 	2);
	  case '/': _index++;	return Operator(OPERATOR_DIV,	2);
	  case ')': case '\x00':	return Operator(OPERATOR_NOPE,	0);
	  default:	if(!UNEXP) 
            	{
            		BUF_UNEXP = strdup("Invalid syntax Operator!\n");
            		UNEXP=true;
            	}
            	return Operator(OPERATOR_NOPE,	0);
	}
}

Number *calculate(Number *v1, Number *v2, Operator op)
{
	// DEBUG_PRINT("calculate: ");
	// print_num(v1);
	// print_num(v2);
	Number *pNum;
	switch (op.op)
	{
		case OPERATOR_ADD:	
			if( (v1->sign^v2->sign) == 0) pNum = add_Num(v1,v2);
			else pNum = sub_Num(v1,v2); break;
		case OPERATOR_SUB:
			v2->sign = 1^v2->sign;
			if( (v1->sign^v2->sign) == 0) pNum = add_Num(v1,v2);
			else pNum = sub_Num(v1,v2); break;
		case OPERATOR_MUL:	pNum = mul_Num(v1,v2); break;
		case OPERATOR_DIV:	pNum = div_Num(v1,v2); break;
		default:	pNum = new_Number();
	}
	//freeNumber(v1); will be freed at pop
	freeNumber(v2);
	return pNum;
}

Number* parsesyntax() //sub esp 0x20
{
	Number 	*pNum ; 
	Node	*pNode; 
	pNode = new_Node(Operator(OPERATOR_NOPE, 0), new_Number());
	push(pNode);//  new or start '(' = OPERATOR_NOPE + 0	
	pNum = parseValue(); //get v1
	
	while (p_stack != -1)//stack !empty
	{
	 	Operator op(parseOp()); // parse an operator (+, -, *, /)
		while ( op.priority  <= top()->getPriority() ) //priority next operator < back
		{
			
			if(top()->isNOPE())
			{
				pop();
				return pNum;
			}
			pNum = calculate(top()->v1, pNum, top()->opr);
			// DEBUG_PRINT("RESULT CALC->");
			// print_num(pNum);
			pop();
		}
	  // store on stack_ and continue parsing
	  pNode = new_Node(op, pNum);
	  push(pNode);
	  // parse value on the right
	  pNum = parseValue();
	}
	return 0;
}

Number *parseValue()
{
	Number *val;
	eatSpaces();
	DEBUG_PRINT("parseValue %c\n",getCharacter());
	switch (getCharacter())
	{
	  case '0': case '1': case '2': case '3': case '4': case '5':
	  case '6': case '7': case '8': case '9':
	            val = parseNumberDecimal();
	            //print_num(val);
	            break;
	  case '(': _index++;
	            val = parsesyntax();
	             //DEBUG_PRINT("parsesyntax -> ");
	             //print_num(val);
	            eatSpaces();
	            if (getCharacter() != ')')
	            {
	                	if(!UNEXP) 
	                	{
	                		// DEBUG_PRINT("Invalid syntax1\n");
	                		BUF_UNEXP = strdup("Invalid syntax!\n");
	                		UNEXP=true;
	                	}
	            }
	            _index++; break;
	  case '+': _index++; val =  parseValue(); break;
	  case '-': _index++; val =  parseValue(); val->sign^=1;  break;
	  default : 	if(!UNEXP)
	  				{
		  				BUF_UNEXP = strdup("Invalid syntax!\n");
		                UNEXP = true;
	                }
	                val= new_Number();
	}
	// DEBUG_PRINT("parseValue -> ");
	//print_num(val);
	return val;
}
void PlayGame()
{
	puts("Welcome to FINAL MeePwnCTF2018!!!");
	puts("input something like: 111111111111111111111111+2+(999/11 )-22222 and I will give you my result :)");
	char buf[0x20];
	while(true)
	{
		printf("INPUT >>");
		int n = read(0,INPUT,0x300);
		INPUT[n-1]=0;
		if(!strncmp(INPUT,"quit",4)) break;
		Number *pNum = parsesyntax();
		if(UNEXP)
		{
			puts(BUF_UNEXP);
			free(BUF_UNEXP);
			BUF_UNEXP = 0;
			UNEXP = 0;
		}
		else
		{
			if(pNum->is_big) write(1,pNum->strNum,pNum->size);
			else printf("%lld",pNum->value);
		}
		printf("P %p",AtributeInfo);
		freeNumber(pNum);
		puts("");
		_index = 0;
	}
}

int main(){
	setvbuf(stdin, NULL, _IONBF, 0);
  	setvbuf(stdout, NULL, _IONBF, 0);
	INPUT = (char*) malloc(0x300);
	char buf[0x40];
	PlayGame();
	if(AtributeInfo)
	{
		FILE *ptr = fopen("/home/mipu94/mychallenge/data","w");
		if(ptr == NULL ) { perror("open file error"); exit(-1);}
		sprintf(buf, "Contact info: %s",AtributeInfo);
		fscanf(ptr,"%s",buf);
		printf("%s, tell me the module you want to contribute?\n>> ",buf);
		scanf("%64s",buf);
		fscanf(ptr,"%s",buf);
		
	}

	puts("Thanks you for playing MeePwnCTF2018!!!");
	return 1;
}