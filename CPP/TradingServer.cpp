// C++ program to demonstrate trading console and manager
/*
Operations and stdin input Formats:

BUY GFD 1111 100 order_id
SELL GFD 1111 100 order_id
MODIFY order_id SELL 2000 100
CANCEL order_id
PRINT
*/
#include <iostream>
#include<vector>
#include<queue>
#include<stack>
#include<set>
#include<string>
#include<map>
#include<sstream>

using namespace std;

enum ordername { BUY, SELL};
enum ordertype {GFD, IOC};

class order {

	string orderId;
	ordername oName;
	ordertype oType;
	int quantity;
	int price;

public:

	order(string _orderId, ordername _oName, ordertype _oType,	int _quantity, int _price)
		:orderId(_orderId), oName(_oName), oType(_oType), quantity(_quantity), price(_price)
	{
	}

	order()
	{
	}

	~order() 
	{
	}

	string getOrderId()
	{
		return orderId;
	}

	ordername getOrderName()
	{
		return oName;
	}

	void setOrderName(ordername _oName)
	{
		oName = _oName;
	}

	ordertype getOrderType()
	{
		return oType;
	}

	void setOrderType(ordertype _oType)
	{
		oType = _oType;
	}

	int getQuantity()
	{
		return quantity;
	}

	void setQuantity(int _quantity)
	{
		quantity = _quantity;
	}

	int getPrice()
	{
		return price;
	}

	void setPrice(int _price)
	{
		price = _price;
	}

};

class orderBook {
	set<int> buyBids;
	set<int> sellBids;

	map<int, vector<string> > buyBidTracker;
	map<int, vector<string> > sellBidTracker;

public:
	map<string, order> orderTracker;

	order* getOrderbyId(string orderId)
	{
		order* o=nullptr;
		if (orderTracker.count(orderId)>0)
		{
			o = &orderTracker[orderId];
			return o;
		}
		return o;
	}

	void makeEntry(order &o)
	{
		if (o.getOrderName() == BUY)
		{
			processAndEntry(o, buyBidTracker, buyBids);
		}
		else if (o.getOrderName() == SELL)
		{
			processAndEntry(o, sellBidTracker,sellBids);
		}
	}

	void removeEntry(order *o)
	{
		if (o->getOrderName() == BUY)
		{
			processRemoveEntry(o, buyBidTracker,buyBids);
		}
		else if (o->getOrderName() == SELL)
		{
			processRemoveEntry(o, sellBidTracker,sellBids);
		}
	}

	void printOrderBook()
	{
		//Print sellorders
		for (auto item : sellBids)
		{
			int quantity = 0;
			cout << "SELL:" << endl;
			vector<string> tempOrderList = sellBidTracker[item];
			for (int j = 0;j < tempOrderList.size();j++)
			{
				quantity = quantity + orderTracker[tempOrderList[j]].getQuantity();
			}
			cout << item << " " << quantity;
		}

		//Print buy orders
		for (auto item : buyBids)
		{
			int quantity = 0;
			cout << "BUY:" << endl;
			vector<string> tempOrderList = buyBidTracker[item];
			for (int j = 0;j < tempOrderList.size();j++)
			{
				quantity = quantity + orderTracker[tempOrderList[j]].getQuantity();
			}
			cout << item << " " << quantity;
		}

	}

private:

	void processAndEntry(order &o, map<int, vector<string> > &bidTracker, set<int> &bids)
	{
		//check if tradable before entry
		processOrderIfTradable(o);

		if ((o.getQuantity() > 0) && (o.getOrderType() == GFD))
		{
			orderTracker[o.getOrderId()] = o;
			bids.insert(o.getPrice());

			vector<string> ordersAtPrice;
			if (bidTracker.count(o.getPrice()) > 0)
			{
				ordersAtPrice = bidTracker[o.getPrice()];
			}
			ordersAtPrice.push_back(o.getOrderId());
			bidTracker[o.getPrice()] = ordersAtPrice;
		}

	}

	void processRemoveEntry(order *o, map<int, vector<string> > &bidTracker, set<int> &bids)
	{
		// remove from orders map
		if (orderTracker.count(o->getOrderId()) > 0)
		{
			orderTracker.erase(o->getOrderId());

			//remove from bids and bidsmap
			if (bidTracker.count(o->getPrice()) > 0)
			{
				//get all siblings orderat same price
				vector<string> ordersAtPrice = bidTracker[o->getPrice()];

				if (ordersAtPrice.size() > 0)
				{
					auto it = find(ordersAtPrice.begin(), ordersAtPrice.end(), o->getOrderId());
					ordersAtPrice.erase(it);
					bidTracker[o->getPrice()] = ordersAtPrice;

					//if no siblings order at this price, remove frombids and bidsmap
					if (ordersAtPrice.size() == 0)
					{
						bidTracker.erase(o->getPrice());
						bids.erase(o->getPrice());
					}
				}
			}
		}
	}

	void processOrderIfTradable(order &o)
	{
		int currentOrderPrice = o.getPrice();

		if (o.getOrderType() == BUY)
		{
			// if no seller at this price return
			if (*sellBids.begin() >currentOrderPrice) return;

			else //trade possible
			{
				set<int> sellBidsTemp = sellBids;
				auto it = sellBids.begin();

				while (it != sellBids.end())
				{
					//if no sell price below current buy price, or when complete trade -exit
					if ((*it > currentOrderPrice) || (o.getQuantity()==0)) break;

					vector<string> NewOrderList = processOrderList(o, *it);

					if (NewOrderList.size()>0)
					{
						sellBidTracker[*it] = NewOrderList;
					}
					else
					{
						//remove entry from sellBidTracker
						sellBidsTemp.erase(*it);
					}
					it++;
				}

				sellBids = sellBidsTemp;
			}
		}

		else if (o.getOrderType() == SELL)
		{
			// if no buyers at this price return
			if (*buyBids.begin() < currentOrderPrice) return;
			else //trade possible
			{
				set<int> buyBidsTemp = buyBids;
				auto it = buyBids.rbegin();

				while (it != buyBids.rend())
				{
					//if no buy price above current buy price, or when complete trade -exit
					if ((*it < currentOrderPrice) || (o.getQuantity() == 0)) break;

					//try to process matching orders at this price
					vector<string> NewOrderList =processOrderList(o, *it);

					if (NewOrderList.size()>0)
					{
						sellBidTracker[*it] = NewOrderList;
					}
					else
					{
						//remove entry from sellBidTracker
						buyBidsTemp.erase(*it);
					}

					it++;
				}
				buyBids = buyBidsTemp;
			}
		}
	}

	vector<string> processOrderList(order &o,const int price)
	{
		//get orders with best price for buying
		vector<string> orderList = sellBidTracker[price];
		vector<string> orderListModified = orderList;

		for (int i = 0;i <= orderList.size();i++)
		{
			order matchingOrder = orderTracker[orderList[i]];

			if (matchingOrder.getQuantity() <= o.getQuantity())
			{
				//update new quantity to currentorder
				o.setQuantity(o.getQuantity() - matchingOrder.getQuantity());
				cout << "TRADE " << matchingOrder.getOrderId()<<" "<<matchingOrder.getPrice() <<" " << matchingOrder.getQuantity() <<" "<< o.getOrderId()<<" "<< o.getPrice()<<" "<< matchingOrder.getQuantity() << endl;

				//remove this matching order from book, update quantity of current order
				orderTracker.erase(orderList[i]);
				orderListModified.erase(std::remove(orderListModified.begin(), orderListModified.end(), orderList[i]), orderListModified.end());
			}
			else if (matchingOrder.getQuantity() > o.getQuantity())
			{
				//update new quantity to matchingtorder
				matchingOrder.setQuantity(matchingOrder.getQuantity() - o.getQuantity());
				cout << "TRADE " << matchingOrder.getOrderId() << " " << matchingOrder.getPrice() << " " << o.getQuantity() << " " << o.getOrderId() << " " << o.getPrice() << " " << o.getQuantity() << endl;
				o.setQuantity(0);
			}
		}

		return orderListModified;
	
	}

};

class TradeEngine 
{
public:
	void process()
	{
		string line;
		while (getline(cin, line))
		{
			vector<string> inputMembers = split(line, ' ');
			processOperation(inputMembers);
		}
	}

private:
	orderBook oBook;

	bool validateInput(vector<string> inputMembers)
	{
		// if empty input
		if (inputMembers.size() == 0) return false;

		// if invalid operation
		if (!((inputMembers[0] == "BUY") || (inputMembers[0] == "SELL") || (inputMembers[0] == "MODIFY") || (inputMembers[0] == "CANCEL")))
			return false;

		if (inputMembers[0] == "BUY" || inputMembers[0] == "SELL")
		{
			if (!((inputMembers[1] == "GFD" || inputMembers[1] == "IOC"))) return false;
			try {
				int temp = stoi(inputMembers[2]);
				temp = stoi(inputMembers[3]);
			}
			catch(exception e)
			{
				return false;
			}
		}

		if (inputMembers[0] == "MODIFY")
		{
			if (!oBook.getOrderbyId(inputMembers[1])) return false;
			if (!((inputMembers[2] == "BUY" || inputMembers[2] == "SELL"))) return false;

			try {
				int temp = stoi(inputMembers[3]);
				temp = stoi(inputMembers[4]);
			}
			catch (exception e)
			{
				return false;
			}
		}

		if (inputMembers[0] == "CANCEL")
		{
			if (!oBook.getOrderbyId(inputMembers[1])) return false;
		}

		return true;
	}

	void processOperation(vector<string> inputMembers)
	{
		try {

			if (validateInput(inputMembers))
			{
				if (inputMembers[0] == "BUY" || inputMembers[0] == "SELL")
				{
					order o = createOrderObj(inputMembers);
					makeEntryToOrderBook(o);
				}
				else if ((inputMembers[0] == "CANCEL"))
				{
					order * o = oBook.getOrderbyId(inputMembers[1]);
					removeEntryFromOrderBook(o);
				}
				else if ((inputMembers[0] == "MODIFY"))
				{
					modifyOrder(inputMembers);
				}
				else if ((inputMembers[0] == "PRINT"))
				{
					printOrderBook();
				}
			}
		}
		catch (exception e)
		{
			cout << "Could not process this operation: more details- " << + e.what()<< endl;
		}
	}

	order createOrderObj(vector<string> orderPars)
	{
		ordername oName;
		ordertype oType;
		int quantity;
		int price;
		string orderId;

		if (orderPars[0] == "BUY") oName = BUY;
		else  oName = SELL;

		if (orderPars[1] == "GFD") oType = GFD;
		else  oType = IOC;

		quantity = stoi(orderPars[2]);
		price = stoi(orderPars[3]);

		orderId = orderPars[4];

		order obj(orderId, oName, oType, quantity, price);
		return obj;
	}

	void makeEntryToOrderBook(order &o)
	{
		oBook.makeEntry(o);
	}

	void removeEntryFromOrderBook(order *o)
	{
		oBook.removeEntry(o);
	}

	void modifyOrder(vector<string> inputMembers)
	{
		
		order *o1 = oBook.getOrderbyId(inputMembers[1]);
		order o2= *o1;
		
		if (inputMembers[2] == "BUY") o2.setOrderName(BUY);
		else  o2.setOrderName(SELL);

		o2.setPrice(stoi(inputMembers[3]));
		o2.setQuantity(stoi(inputMembers[4]));

		oBook.removeEntry(o1);
		oBook.makeEntry(o2);

	}

	void printOrderBook()
	{
		oBook.printOrderBook();
	}

	vector<string> split(string str, char delimiter) {
		vector<string> strvec;
		stringstream ss(str);
		string tok;

		while (getline(ss, tok, delimiter)) {
			strvec.push_back(tok);
		}

		return strvec;
	}

};

int main()
{
	TradeEngine t;
	t.process();

	return 0;
}