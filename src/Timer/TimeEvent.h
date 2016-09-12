#ifndef TIME_EVENT_H
#define TIME_EVENT_H

//MAIN
#include "EventEngine/EventData.h"


class TimeEvent
	: public EventData
{
public:
	TimeEvent(const unsigned long time_seq);
	~TimeEvent(void);

	const EDType GetType() const ;

	unsigned long GetTimeSeq() const
	{
		return m_time_seq;
	}

public:

	const unsigned long m_time_seq;

};


#endif //TIME_EVENT_H
