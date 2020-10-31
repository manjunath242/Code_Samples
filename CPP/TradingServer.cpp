// C++ program to demonstrate trading console and manager
/*
Operations and stdin input Formats:

BUY GFD 1111 100 order_id
SELL GFD 1111 100 order_id
MODIFY order_id SELL 2000 100
CANCEL order_id
PRINT
*/

#include <map>
#include <set>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

enum ordername { BUY, SELL };
enum ordertype { GFD, IOC };

class order {

	string orderId;
	ordername oName;
	ordertype oType;
	int quantity;
	int price;

public:

	order(string _orderId, ordername _oName, ordertype _oType, int _quantity, int _price)
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

	void debugPrint()
	{
		cout << "-------------------DEBUG DATA---------------" << endl;

		cout << "All orders:" << endl;
		for (auto it = orderTracker.begin();it != orderTracker.end();it++)
		{
			cout << it->first << " ";
		}
		cout << endl << endl;

		cout << "buy bids:" << endl;
		for (auto it = buyBids.begin();it != buyBids.end();it++)
		{
			cout << *it << " ";
		}
		cout << endl << endl;

		cout << "sell bids:" << endl;
		for (auto it = sellBids.begin();it != sellBids.end();it++)
		{
			cout << *it << " ";
		}
		cout << endl << endl;

		cout << "buy bids tracker:" << endl;
		for (auto it = buyBidTracker.begin();it != buyBidTracker.end();it++)
		{
			for (int i = 0;i < it->second.size();i++)
				cout << it->second[i] << " ";
			cout << endl;
		}
		cout << endl << endl;

		cout << "sell bids tracker:" << endl;
		for (auto it = sellBidTracker.begin();it != sellBidTracker.end();it++)
		{
			for (int i = 0;i < it->second.size();i++)
				cout << it->second[i] << " ";
			cout << endl;
		}
		cout << endl << endl;

		cout << "-------------------DEBUG DATA---------------" << endl;
	}

	order* getOrderbyId(string orderId)
	{
		order* o = nullptr;
		if (orderTracker.count(orderId)>0)
		{
			o = &orderTracker[orderId];
			return o;
		}
		return o;
	}

	void makeEntry(order &o)
	{
		if (orderTracker.count(o.getOrderId()) == 0)
		{
			if (o.getOrderName() == BUY)
			{
				processAndEntry(o, buyBidTracker, buyBids);
			}
			else if (o.getOrderName() == SELL)
			{
				processAndEntry(o, sellBidTracker, sellBids);
			}
		}
	}

	void removeEntry(order *o)
	{
		if (orderTracker.count(o->getOrderId()) > 0)
		{
			if (o->getOrderName() == BUY)
			{
				processRemoveEntry(o, buyBidTracker, buyBids);
			}
			else if (o->getOrderName() == SELL)
			{
				processRemoveEntry(o, sellBidTracker, sellBids);
			}
		}
	}

	void printOrderBook()
	{
		//Print sellorders
		cout << "SELL:" << endl;
		for (auto item : sellBids)
		{
			int quantity = 0;
			vector<string> tempOrderList = sellBidTracker[item];
			for (int j = 0;j < tempOrderList.size();j++)
			{
				quantity = quantity + orderTracker[tempOrderList[j]].getQuantity();
			}
			cout << item << " " << quantity << endl;;
		}

		//Print buy orders
		cout << "BUY:" << endl;
		for (auto item : buyBids)
		{
			int quantity = 0;
			vector<string> tempOrderList = buyBidTracker[item];
			for (int j = 0;j < tempOrderList.size();j++)
			{
				quantity = quantity + orderTracker[tempOrderList[j]].getQuantity();
			}
			cout << item << " " << quantity << endl;
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

		// remove from orders map
		orderTracker.erase(o->getOrderId());

	}

	void processOrderIfTradable(order &o)
	{
		int currentOrderPrice = o.getPrice();

		if (o.getOrderName() == BUY)
		{
			// if no seller at this price return
			if (sellBids.size() > 0)
			{
				if (*sellBids.begin() > currentOrderPrice) return;

				else //trade possible
				{
					set<int> sellBidsTemp = sellBids;
					auto it = sellBids.begin();

					while (it != sellBids.end())
					{
						//if no sell price below current buy price, or when complete trade -exit
						if ((*it > currentOrderPrice) || (o.getQuantity() == 0)) break;

						vector<string> OrderList = sellBidTracker[*it];
						//try to process matching orders at this price
						OrderList = processOrderList(o, *it, OrderList);

						if (OrderList.size() > 0)
						{
							sellBidTracker[*it] = OrderList;
						}
						else
						{
							//remove entry from sellBidTracker
							sellBidTracker.erase(*it);
							sellBidsTemp.erase(*it);
						}
						it++;
					}

					sellBids = sellBidsTemp;
				}
			}
		}

		else if (o.getOrderName() == SELL)
		{
			// if no buyers at this price return
			if (buyBids.size() > 0)
			{
				if (*buyBids.begin() < currentOrderPrice) return;
				else //trade possible
				{
					set<int> buyBidsTemp = buyBids;
					auto it = buyBids.rbegin();

					while (it != buyBids.rend())
					{
						//if no buy price above current buy price, or when complete trade -exit
						if ((*it < currentOrderPrice) || (o.getQuantity() == 0)) break;

						vector<string> OrderList = buyBidTracker[*it];
						//try to process matching orders at this price
						OrderList = processOrderList(o, *it, OrderList);

						if (OrderList.size()>0)
						{
							buyBidTracker[*it] = OrderList;
						}
						else
						{
							//remove entry from buyBidTracker
							buyBidTracker.erase(*it);
							buyBidsTemp.erase(*it);
						}

						it++;
					}
					buyBids = buyBidsTemp;
				}
			}
		}
	}

	vector<string> processOrderList(order &o, const int price, vector<string> orderList)
	{
		vector<string> orderListModified = orderList;

		for (int i = 0;i <= orderList.size();i++)
		{

			if (orderTracker.empty()) break;
			if ((o.getQuantity() == 0)) break;
			if (orderTracker.count(orderList[i]) > 0)
			{
				order * matchingOrder = &orderTracker[orderList[i]];

				if (matchingOrder->getQuantity() <= o.getQuantity())
				{
					//update new quantity to currentorder
					o.setQuantity(o.getQuantity() - matchingOrder->getQuantity());
					cout << "TRADE " << matchingOrder->getOrderId() << " " << matchingOrder->getPrice() << " " << matchingOrder->getQuantity() << " " << o.getOrderId() << " " << o.getPrice() << " " << matchingOrder->getQuantity() << endl;

					//remove this matching order from book, update quantity of current order
					orderTracker.erase(orderList[i]);
					orderListModified.erase(std::remove(orderListModified.begin(), orderListModified.end(), orderList[i]), orderListModified.end());
				}
				else if (matchingOrder->getQuantity() > o.getQuantity())
				{
					//update new quantity to matchingtorder
					matchingOrder->setQuantity(matchingOrder->getQuantity() - o.getQuantity());
					cout << "TRADE " << matchingOrder->getOrderId() << " " << matchingOrder->getPrice() << " " << o.getQuantity() << " " << o.getOrderId() << " " << o.getPrice() << " " << o.getQuantity() << endl;
					o.setQuantity(0);
				}
			}
			else break;
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
			trim(line);

			vector<string> inputMembers = split(line, ' ');
			processOperation(inputMembers);
			//printOrderBook();
			oBook.debugPrint();
		}
	}

private:
	orderBook oBook;

	bool validateInput(vector<string> inputMembers)
	{
		// if empty input
		if (inputMembers.size() == 0) return false;

		// if invalid operation
		if (!((inputMembers[0] == "BUY") || (inputMembers[0] == "SELL") || (inputMembers[0] == "MODIFY") || (inputMembers[0] == "CANCEL") || (inputMembers[0] == "PRINT")))
			return false;

		if (inputMembers[0] == "BUY" || inputMembers[0] == "SELL")
		{
			if (inputMembers.size() != 5) return false;

			if (!((inputMembers[1] == "GFD" || inputMembers[1] == "IOC"))) return false;
			try {
				int temp = stoi(inputMembers[2]);
				temp = stoi(inputMembers[3]);
			}
			catch (exception e)
			{
				return false;
			}
		}

		if (inputMembers[0] == "MODIFY")
		{
			if (inputMembers.size() != 5) return false;

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
			if (inputMembers.size() != 2) return false;

			if (!oBook.getOrderbyId(inputMembers[1])) return false;
		}

		if (inputMembers[0] == "PRINT")
		{
			if (inputMembers.size() != 1) return false;
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
			cout << "Could not process this operation: more details- " << +e.what() << endl;
		}
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
		order o2 = *o1;

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

		price = stoi(orderPars[2]);
		quantity = stoi(orderPars[3]);

		orderId = orderPars[4];

		order obj(orderId, oName, oType, quantity, price);
		return obj;
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

	void trim(string& s)
	{
		size_t p = s.find_first_not_of(" \t");
		s.erase(0, p);

		p = s.find_last_not_of(" \t");
		if (string::npos != p)
			s.erase(p + 1);
	}

};

int main() {

	TradeEngine t;
	t.process();
	return 0;
}