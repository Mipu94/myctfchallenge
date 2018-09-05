#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "coin.h"

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KBLU  "\x1B[34m"
#define KEND  "\033[0m"

int ReadInt()
{
	char buff[8];
	ReadStr(buff,8);
	return atoi(buff);
}

int ReadStr(char *buff, unsigned int size)
{
	int n = 0;
	while(n < size)
	{
		
		int k = read(0,&buff[n],size-n);
		if(k <= 0)
		{
	    	 puts("read error");
	     	_exit(1);
	 	}
	 	n += k;
	 	if(buff[n-1] == '\n')
	 	{
	 		buff[n-1] = 0;
	 		return n;
	 	}
 	}
 	buff[n-1] = 0;

 	return n;
}


// Init funcs
void InitMarket()
{
	pCurencies = 0;
	AddCurrency("BTC",8394.05,0.0002);
	AddCurrency("BCH",1306.81,0.0002);
	AddCurrency("ETH",713.65,0.0002);
	AddCurrency("LTC",139.49,0.0002);
	AddCurrency("EOS",13.54,0.0002);
	AddCurrency("PWN",1337,0.0002);
}

void InitUser()
{	
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);
	srand(time(NULL));
	pUser = malloc(sizeof(User));
	if(!pUser) _exit(-1);
	pUser->UserName = Demo;
	pUser->Money = 1000;
	pUser->leverage = 100;
	pUser->LevMoney = pUser->Money*pUser->leverage;
	pUser->pOrder = 0;
	pUser->type = 0;
	pUser->exp = 0;
}

// Currency funcs
void AddCurrency(char *name, double price, double spead)
{	
	Currency *node = malloc(sizeof(Currency));
	void *pname = malloc(0x10);	
	node->name  = strcpy(pname,name);
	node->price = price;
	node->pre_price = price;
	node->spead = spead;
	node->next = pCurencies;
	pCurencies = node;
}


void ShowListCurrencies()
{	
	puts("##########################");
	puts("COINS:");
	Currency *node;
	node = pCurencies;

	while(node)
	{
		printf("%s : %0.4f\n", node->name, node->price);
		node = node->next;
	}
	puts("##########################");
}

Currency *GetCurrencybyName(char *name)
{
	Currency *node;
	node = pCurencies;
	while(node)
	{
		if( !strncmp(name,node->name,3) )
			return node;
		node = node->next;
	}
	return node;
}

void MakeNewCurrency()
{
	if(pUser->type != 1 ) return;
	if(pUser->Money < 1337)
	{
		printf("you not enough money to create new coin!\n");
		return;
	}
	else
	{
		Currency *nCurrency =  malloc(sizeof(Currency));
		puts("Name: ");
		printf(">>> ");
		nCurrency->name = malloc(0x10);
		ReadStr(nCurrency->name,4);
		puts("Price( >= 10.0)");
		printf(">>> ");
		double tmp;
		scanf("%lf",&tmp);
		if(tmp < 10 )
		{
			free(nCurrency);
			return;
		}
		nCurrency->price = tmp;
		puts("spead < 0.002");
		printf(">>> ");
		scanf("%lf",&tmp);
		if( tmp <= 0 || tmp >= 0.002 )
		{
			free(nCurrency);
			return;
		}
		pUser->Money -= 500;
		nCurrency->spead = tmp;
		nCurrency->next = pCurencies;
		pCurencies = nCurrency;
	}
}

//Order funcs
void MenuOrder()
{
	puts("0. Open Order");
	puts("1. Close Order");
	puts("2. Set Stoploss");
	puts("3. Set Takeprofit");
	puts("4. Auto clear Orders");
	puts("5. Show Currencies");
	puts("6. Show List Order");
	puts("7. Back");
	printf(">>> ");
}

int ShowListOrder(Order *pOrder)
{
	Order *tpOrder;
	tpOrder = pOrder;
	char *type;
	char *color;
	int i = 0;
	double profit;
	char *status;
	char *sig = "";
	while(tpOrder)
	{
		if(tpOrder->type == 0) type = "Bought";
		else type = "Sold";
		if( tpOrder->is_close ) 
		{
			status = "Closed";
			profit = tpOrder->profit;	
		}

		if( tpOrder->is_pending) 
		{
			status = "Queue";
			if(tpOrder->type == 0) type = "Buy";
			else type = "Sell";
			profit = 0;
		}

		if(!tpOrder->is_pending && !tpOrder->is_close)
		{
			status = "Running";
			profit = CalculateProfitOrder(tpOrder);
		}

		if(profit > 0) 
		{
			color = KGRN;
			sig = "+";
		}
		else if(profit == 0) color = "";
		else 
		{
			color = KRED;
			sig = "";
		}

		printf("OrderID: %d -> %s # %s %ld %s at %0.4lf : %s%s%0.4lf%s\n",i,status,type,tpOrder->volum,tpOrder->name,tpOrder->entry_price,color,sig,profit,KEND);
		printf("Takeprofit: %s%0.4lf%s Stoploss: %s%0.4lf%s\n",KGRN,tpOrder->takeprofit,KEND,KRED,tpOrder->stoploss,KEND );
		i++;
		tpOrder = tpOrder->next;
	}
	return i;
}

Order *GetOrderIndex(Order *pOrder,int x)
{
	int i = 0;
	Order *tpOrder;
	tpOrder = pOrder;
	while(tpOrder)
	{
		if(i == x) return tpOrder;		
		tpOrder = tpOrder->next;
		i++;
	}
	return tpOrder;
}

Order *GetLastOrder(Order *pOrder)
{
	Order *pLastOrder = pOrder;
	if( !pOrder)
		return NULL;
	while(pLastOrder)
	{
		if(!pLastOrder->next)
			return pLastOrder;
		pLastOrder = pLastOrder->next;
	}
}

bool ValidInputPrice(int32_t type, double stoploss, double price, double takeprofit )
{
	if(type == 0)//buy
	{
		if(stoploss > price)
			return false;
		if(takeprofit < price)
			return false;
	}
	else
	{
		if(stoploss < price)
			return false;
		if(takeprofit > price)
			return false;	
	}
	return true;
}
void SetStoploss()
{
	u_int32_t idx;
	double sl = 0;
	int count = ShowListOrder(pUser->pOrder);
	puts("what order ID you want to change stoploss?\n>>>");
	idx = ReadInt();
	if(count <= idx) _exit(-1);
	Order *pOrder = GetOrderIndex(pUser->pOrder, idx);
	if(!pOrder) return;
	if(pOrder->is_close) return;
	printf("New Stoploss: ");
	scanf("%lf",&sl);
	if(sl <= 0) goto SSL_Invalid;
	if( ValidInputPrice(pOrder->type, sl, pOrder->entry_price, pOrder->takeprofit))
		pOrder->stoploss = sl;
	else
		SSL_Invalid:
		puts("Invalid input");
	return;
}

void SetTakeprofit()
{
	u_int32_t idx;
	double tp;
	int count = ShowListOrder(pUser->pOrder);
	printf("what order ID you want to change takeprofit?\n>>>");
	idx = ReadInt();
	if(count <= idx) _exit(-1);
	Order *pOrder = GetOrderIndex(pUser->pOrder, idx);
	if(!pOrder) return;
	if(pOrder->is_close) return;
	printf("New Takeprofit: ");
	scanf("%lf",&tp);
	if(tp <= 0) goto STP_Invalid;
	if(ValidInputPrice(pOrder->type,pOrder->stoploss, pOrder->entry_price, tp))
			pOrder->takeprofit = tp;
	else
		STP_Invalid:
		puts("Invalid input");
	return;
}


void OpenOrder()
{
	Currency *pCurency;
	char Cname[0x8];
	int32_t type;
	double tmpPrice;
	double in_cart_price;
	u_int32_t volum = 0;
	if (CountOrder >= 11) 
	{
		printf("MAX!!!!");
		return;
	}
	ShowListCurrencies();
	printf("What coin you want to buy?\n>>> ");
	ReadStr(Cname,4);
	pCurency = GetCurrencybyName(Cname);	
	if(!pCurency) return;
	//start order
	Order *nOrder = malloc(sizeof(Order));
	if(!nOrder) return;

	nOrder->name = pCurency->name;
	puts("0. buy");
	puts("1. sell");
	printf(">>> ");
	type = ReadInt();
	if(  type == 0 || type == 1) nOrder->type = type;
	else goto FreeOOder;
	puts("type of order: ");
	puts("0. Entry now");
	puts("1. Pending");
	printf(">>> ");
	type = ReadInt();
	
	if( type == 0)
	{
		tmpPrice = pCurency->price;
		nOrder->is_pending = 0;
	}
	else if(type == 1)
	{
		printf("Entry price: ");
		scanf("%lf",&tmpPrice);
		nOrder->is_pending = 1;
	}
	else goto FreeOOder;

	if(tmpPrice <= 0) goto FreeOOder;

	printf("Volum: ");
	volum = ReadInt();
	if( volum <= 0)
		goto FreeOOder;
	in_cart_price = (double)volum*tmpPrice*(1 + pCurency->spead);
	if( pUser->LevMoney >= in_cart_price)
	{
		pUser->LevMoney -= in_cart_price;
		nOrder->entry_price = tmpPrice;
		nOrder->volum = volum;
		//Order *cOrder = GetLastOrder(pUser->pOrder);	
	
		printf("Stoploss: ");
		scanf("%lf",&tmpPrice);
		if(tmpPrice <= 0) goto FreeOOder;
		nOrder->stoploss = tmpPrice;

		printf("Takeprofit: ");
		scanf("%lf",&tmpPrice);
		if(tmpPrice <= 0) goto FreeOOder;
		nOrder->takeprofit = tmpPrice;
		
		if(!ValidInputPrice(nOrder->type, nOrder->stoploss,nOrder->entry_price,nOrder->takeprofit))
		{	
			puts("Invalid input");
			goto FreeOOder;
		}

		//Watch here
		nOrder->is_close = 0;
		nOrder->profit = 0;
		Order *cOrder = GetLastOrder(pUser->pOrder);	
		nOrder->next = 0;

		if(cOrder == NULL) pUser->pOrder = nOrder;
		else cOrder->next = nOrder;
		CountOrder++;
		if(nOrder->is_pending)
			CheckPendingOrder(nOrder);
	}
	else
	{
		puts("Not enough money!");
		FreeOOder:
		free(nOrder);
		return;
	}
}

void CloseOrder(Order *pOrder)
{
	double profit = 0;
	Currency *pCurency = GetCurrencybyName(pOrder->name);
	if(pOrder->is_close) return;
	if(pOrder->is_pending)
	{
		pOrder->is_pending = 0;
		pOrder->is_close = 1;
		return;
	}
	profit = CalculateProfitOrder(pOrder);

	if (profit < 0)
	{
		double sumpf = SumProfit();
		if(sumpf + pUser->Money <= 0)
		{
			pUser->Money = 0;
			pUser->LevMoney = 0;
			freeAllOrders();
			pUser->pOrder = 0;
			return;
		}
	}	
	pOrder->profit = profit;
	pOrder->is_close = 1;
	pUser->Money += profit;
	pUser->LevMoney += pOrder->entry_price*pOrder->volum + profit*pUser->leverage;
	if( profit > 0 )pUser->exp += 1;
	return;
}

void CloseOrderPice(Order *pOrder, double Cprice)
{
	double profit = 0;
	Currency *pCurency = GetCurrencybyName(pOrder->name);
	if(pOrder->is_close) return;
	if(pOrder->is_pending)
	{
		pOrder->is_pending = 0;
		pOrder->is_close = 1;
		return;
	}

	if(pOrder->type == 0)//buy
		profit = ( Cprice - pOrder->entry_price - pCurency->spead*(Cprice + pOrder->entry_price) )*pOrder->volum;
	else//sell
		profit = ( pOrder->entry_price - Cprice - pCurency->spead*(Cprice + pOrder->entry_price) )*pOrder->volum;

	 // printf("%s = %lf , %lf, %lf",pOrder->name ,pOrder->entry_price - Cprice, pCurency->spead*(Cprice + pOrder->entry_price), pCurency->price);
	 // printf("closed Profit %lf\n",profit);
	pOrder->profit = profit;
	pOrder->is_close = 1;
	pUser->Money += profit;
	pUser->LevMoney += pOrder->entry_price*pOrder->volum + profit*pUser->leverage;
	if( profit > 0 )pUser->exp += 1;
	return;
}

void CloseTradeID()
{
	u_int32_t idx;
	int count = ShowListOrder(pUser->pOrder);
	puts("what order ID you want to close?");
	printf(">>> ");
	idx = ReadInt();
	if( count <= idx) return;
	Order *pOrder = GetOrderIndex(pUser->pOrder, idx);
	if(!pOrder) return;
	if( pOrder->is_pending )
	{
		pOrder->is_close = 1;
		pOrder->is_pending = 0;
		return;
	}
	CloseOrder(pOrder);
	return;
}


void Trade()
{	
	while(true)
	{
		MenuOrder();
		int choice = ReadInt();
		switch(choice)
		{
			case 0: OpenOrder(); break;
			case 1: CloseTradeID(); break;
			case 2: SetStoploss(); break;
			case 3: SetTakeprofit(); break;
			case 4: IsAutoFree = true; break;
			case 5: ShowListCurrencies();break;
			case 6: ShowListOrder(pUser->pOrder);break;
			case 7: return;
			default: puts("Invalid value!!!");break;
		}
	}
}

void MenuMain()
{
	puts("0. My Info");
	puts("1. Trade");
	puts("2. Make New Coin");
	puts("3. Switch Real");
	puts("4. Change Leverage");
	puts("5. Quit");
	printf(">>> ");
}

void *Change_Price(void *ptr)
{
	float r;
	while(true)
	{
		sleep(2);
		pthread_mutex_lock(&lock);
		double percent ;
		Currency *pCur = pCurencies;
		while(pCur)
		{
			int x = rand() % 17 - 8;
			percent = (double)(x)/(double)10000 + (double)1;
			pCur->pre_price = pCur->price;
			pCur->price *= percent;
			pCur = pCur->next;
		}
		
		CheckPendingOrder(NULL);
		pthread_mutex_unlock(&lock);
		//ShowListOrder(pUser->pOrder);
	}
}

double CalculateProfitOrder(Order *pOrder)
{
	double profit = 0;
	Currency *pCurency = GetCurrencybyName(pOrder->name);
	if(pOrder->is_pending || pOrder->is_close)
		return pOrder->profit;
	if(pOrder->type == 0)//buy
		profit = ( pCurency->price - pOrder->entry_price - pCurency->spead*(pOrder->entry_price + pCurency->price ))*pOrder->volum;
	else//sell
		profit = (  pOrder->entry_price - pCurency->price - pCurency->spead*(pOrder->entry_price + pCurency->price ))*pOrder->volum;
	return profit;
}

double SumProfit()
{
	Order *pOrder = pUser->pOrder;
	double Sprofit = 0;
	double profit;
	while(pOrder)
	{
		profit = CalculateProfitOrder(pOrder);
		Sprofit +=  profit;
		pOrder = pOrder->next;
	}
	return Sprofit;
}


void CheckPendingOrder(Order *C_Order)
{	
	Order *pOrder = pUser->pOrder;
	if(C_Order)
		pOrder = C_Order;
	while(pOrder)
	{
		if(pOrder->is_pending)
		{
			Currency *pCur = GetCurrencybyName(pOrder->name);
			if( !pCur ) _exit(-1);
			if( pCur->pre_price <= pCur->price )
			{
				if( pCur->pre_price <= pOrder->entry_price && pOrder->entry_price <= pCur->price )
				{
					pOrder->is_pending = 0;
				}
			}
			else
			{
				if(pCur->price <= pOrder->entry_price && pOrder->entry_price <= pCur->pre_price)
				{
					pOrder->is_pending = 0;
				}
			}
		}
		pOrder = pOrder->next;
	}
}


void *MarketAutoCloseOrder()
{
	while(true)
	{
		sleep(1);
		pthread_mutex_lock(&lock);
		Order *pOrder = pUser->pOrder;
		while(pOrder)
		{
			Currency *pCur = GetCurrencybyName(pOrder->name);

			if(!pOrder->is_close && !pOrder->is_pending)
			{
				//check money < lost (Current profit)
				// double cProfit = SumProfit();
				// if(pUser->Money + cProfit <= 0)
				// {
				// 	pUser->Money = 0;
				// 	pUser->LevMoney = 0;
				// 	freeAllOrders();
				// 	pUser->pOrder = 0;
				// 	return 0;
				// }

				if( pOrder->type == 0 )//buy
				{
					if(pOrder->takeprofit != 0 && pOrder->takeprofit <= pCur->price )
					{
						CloseOrderPice(pOrder,pOrder->takeprofit);
						pOrder = pOrder->next;
						continue;
					}

					if(pOrder->stoploss != 0 && pOrder->stoploss >= pCur->price)
					{
						CloseOrderPice(pOrder,pOrder->stoploss);
					}
				}
				else//sell
				{
					if(pOrder->takeprofit != 0 && pOrder->takeprofit >= pCur->price )
					{
						CloseOrderPice(pOrder,pOrder->takeprofit);
						pOrder = pOrder->next;
						continue;
					}
					
					if(pOrder->stoploss != 0 && pOrder->stoploss <= pCur->price)
					{
						CloseOrderPice(pOrder,pOrder->stoploss);
					}
				}
			}			
			pOrder = pOrder->next;
		}
		pthread_mutex_unlock(&lock);
	}

}

void *AutoClearClosedOrders()
{

	while(true)
	{
		sleep(3);
		if(IsAutoFree == false) continue;
		pthread_mutex_lock(&lock);
		Order *node = pUser->pOrder;
		Order *tmp = pUser->pOrder;
		while(node)
		{
			if(node->is_close)
			{
				if(pUser->pOrder == node )
				{
					pUser->pOrder = node->next;
					free(node);
					node = pUser->pOrder;
					tmp = node;
					continue;
				}
				tmp->next = node->next;
				free(node);
				node = tmp->next;
				continue;
			}
			tmp = node;
			node = node->next;
		}
		pthread_mutex_unlock(&lock);
	}
}

void freeAllOrders()
{
	Order *node = pUser->pOrder;
	while(node)
	{
		//printf("FREE ALL: %p \n",node);
		free(node);
		node = node->next;
	}
}

void MarketRun()
{
	int error;
	pthread_t ptPrice,ptClose,ptClearOrder;
	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init has failed\n");	
        _exit(-1);
    }
	error = pthread_create( &ptPrice, NULL, Change_Price, NULL );
    if (error != 0) 
	{
		printf("\nThread can't be created :[%s]", strerror(error));
		_exit(-1);
	}
	error = pthread_create( &ptClose, NULL, MarketAutoCloseOrder, NULL );
    if (error != 0) 
	{
		printf("\nThread can't be created :[%s]", strerror(error));
		_exit(-1);
	}
	error = pthread_create( &ptClearOrder, NULL, AutoClearClosedOrders, NULL );
    if (error != 0) 
	{
		printf("\nThread can't be created :[%s]", strerror(error));
		_exit(-1);
	}
}

void MyInfo()
{
	if(pUser->type == 0 )
		printf("UserInfo: %s\nMoney: %0.2f\nAccount Type: Demo\nLeverage: %d\nLevMoney: %ld\nExp: %d\n",pUser->UserName,pUser->Money,pUser->leverage,pUser->LevMoney,pUser->exp);
	else
		printf("UserInfo: %s\nMoney: %0.2f\nAccount Type: Real\nLeverage: %d\nLevMoney: %ld\nExp: %d\n",pUser->UserName,pUser->Money,pUser->leverage,pUser->LevMoney,pUser->exp);

}

void ChangeLeverage()
{


	if(pUser->type == 0) 
	{
		puts("Newbie will be safer to choose a low leverage");
		printf("Leverage:\n1: 20\n2: 50\n3: 100\n>>> ");
	}
	else printf("Leverage:\n1: 20\n2: 50\n3: 100\n4: 150 \n5: 250\n>>> ");

	int x = ReadInt();
	switch(x)
	{
		case 1: pUser->leverage = 20; break;
		case 2: pUser->leverage = 50; break;
		case 3: pUser->leverage = 100; break;
		case 4: 
			if(pUser->type == 1)
				pUser->leverage = 150;
			break;
		case 5:
			if(pUser->type == 1)
				pUser->leverage = 250;
			break;

		default: break;
	}
}

void SwitchReal()
{
	if(pUser->type == 1)
	{
		puts("Real account already!");
		return;
	}
	char buf[2];
	if(pUser->exp < 6)
	{ 	
		printf("You need more exp in trading to switch to real account\n");
		return;
	}
	pUser->type = 1;
	pUser->UserName = Real;
	pUser->Money = 1300;
	freeAllOrders();
	printf("Do you want to change leverage for real account?(y/n)\n>>>");
	ReadStr(buf,2);
	if(buf[0] == 'y')
		ChangeLeverage();
	pUser->LevMoney = pUser->Money*pUser->leverage;
	pUser->pOrder = 0;
	pUser->type = 1;
	CountOrder = 0;
	IsAutoFree = false;
}

bool CheckCloseAllOrder()
{
	Order *pOrder = pUser->pOrder;
	while(pOrder)
	{
		if(!pOrder->is_close && !pOrder->is_pending )
				return false;
		pOrder = pOrder->next;
	}	
	return true;
}

void ProcessLeverage()
{
	if(!CheckCloseAllOrder())
	{
		puts("You need close all orders before change leverage!");
		return;
	}
	ChangeLeverage();
	pUser->LevMoney = pUser->Money*pUser->leverage;
}

int main()
{
	InitUser();
	InitMarket();
	MarketRun();	
	while(true)
	{
		MenuMain();
		int choice = ReadInt();
		switch(choice)
		{	
			case 0: MyInfo(); break;
			case 1: Trade(); break;
			case 2: MakeNewCurrency(); break;
			case 3: SwitchReal(); break;
			case 4: ProcessLeverage(); break;
			case 5: puts("Thank you!"); _exit(0);
			default: break;
		} 
	}
	pthread_mutex_destroy(&lock);
	return 0;
}

