#include "TimeEvent.h"


TimeEvent::TimeEvent(const unsigned long time_seq): m_time_seq(time_seq)
{
}

TimeEvent::~TimeEvent(void)
{
}

const EDType TimeEvent::GetType() const 
{
	return EDTYPE_TIMER;
}

