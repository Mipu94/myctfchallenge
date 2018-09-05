#define DEBUG 0

#if DEBUG == 1
#define DEBUG_PRINT(...) printf( __VA_ARGS__ );
#else
#define DEBUG_PRINT(...)
#endif


#define MAX_SIZE 256
#define MAX_NUM 0x100



struct Number{
	bool sign;
	bool is_big;
	long long int value;
	size_t size;
	char *strNum;
};

enum
{
	OPERATOR_NOPE,	// ) or (
	OPERATOR_ADD,	// +
	OPERATOR_SUB,	// -
	OPERATOR_MUL,	// *
	OPERATOR_DIV,	// /
	OPERATOR_MOD,	// %
};


struct Operator
{
	int op; //operation
	int priority;
	Operator(int operation, int prio) :
	  op(operation),
	  priority(prio)
	{ }
};

//1 node = value + operator
struct Node
{
	Operator opr;
	Number *v1;
	int getPriority() const
    {
      return opr.priority;
    }
    bool isNOPE() const
    {
      return opr.op == OPERATOR_NOPE;
    }
};

int p_stack = -1;
size_t _index = 0;
char *INPUT;
bool UNEXP=false;
char *BUF_UNEXP;
void *MMAP_PTR;
bool IS_DEBUG=false;
Node *stack[MAX_SIZE];
char *AtributeInfo;



Number *parseValue();
Number* parsesyntax();
 

