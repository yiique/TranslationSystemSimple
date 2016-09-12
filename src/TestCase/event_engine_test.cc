#include "EventEngine/EventEngine.h"

#include <iostream>
using namespace std;


class TestEngine
	: public EventEngine
{

private:

	/*int run()
	{
		cout << "TestEngine run" << endl;
		return 0;
	}*/

	void on_event(Event & e)
	{
		cout << "Get event : " << e._priority << endl;
	}
};

class TestData
	: public EventData
{
public:
	 const EDType GetType() const
	 {
		 return EDTYPE_LISTEN_NET_REQ;
	 }
};

int main()
{

	TestEngine engine;

	engine.Start();

	cout << "Start success." << endl;

	TestData data;

	Event e(&data);


	int x;
	cin >> x;
	engine.PostEvent(e);

	cin >> x;
	return 0;
}
