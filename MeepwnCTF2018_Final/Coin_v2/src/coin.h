#include <stdbool.h>



typedef struct Order_struct
{
	u_int64_t volum;
	void *next;
	char *name;
	double takeprofit;
	double entry_price;
	double stoploss;
	double profit;
	int8_t type;//buy or sell
	bool is_pending;
	bool is_close;
}Order;

typedef struct Currency_struct
{
 	char *name;
 	double price;
 	double pre_price;
 	double spead;
	void *next;
}Currency;

typedef struct User_struct
{
	char *UserName;
	double Money;
	u_int64_t LevMoney;
	Order *pOrder;
	int8_t type;// 0:demo, 1:real
	int32_t leverage;
	int32_t exp;
}User;



int CountOrder = 0;
Currency *pCurencies;
User *pUser;
bool IsAutoFree = false;
char *Demo = "Demo Account";
char *Real = "Real Account";
pthread_mutex_t lock;

int ReadInt();
int ReadStr(char *buff, unsigned int size);

void InitMarket();
void InitUser();


Currency *GetCurrencybyName(char *name);
void SetPriceCurrency(char *name, double price);
void AddCurrency(char *name, double price, double spead);
void ShowListCurrencies();
void MakeNewCurrency();
void CheckPendingOrder(Order *C_Order);
Order *GetOrderIndex(Order *pOrder,int x);
double CalculateProfitOrder(Order* pOrder);
double SumProfit();
void freeAllOrders();
void MenuOrder();
void ProcessOrder();
void CloseOrder();
void SetStoploss();
void SetTakeprofit();
void CloseTradeID();
void OpenOrder();
void Trade();

