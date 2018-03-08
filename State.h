#pragma once
#include <vector>
//#include <algorithm>
#include <functional>

template <class T>
class State
{
public:
	State() : m_action(), m_name(), m_delay(0.0f) {}
	State(void (T::*f)(), const char* name = ""): m_action(f), m_name(name), m_delay(0.0f) {}
	void SetAction(void (T::*f)()) { m_action = f; }
	void SetDelay(float delay) { m_delay = delay; }
	void SetNext(State* next, std::function<bool()> f, std::function<void()> tf = nullptr)
	{ 
		m_transitions.emplace_back(next,f, tf); 
	}
	void DoAction(T* context)
	{
		if (m_action)
		{
			(context->*(m_action))();
		}
	}
	void Update(float dt)
	{
		m_delay -= (std::min)(m_delay, dt);
	}
	State* DoNext(T* context)
	{
		return GetNext(context, true);
	}

	State* GetNext(T* context, bool doImmediately = false)
	{
		for (auto i = m_transitions.begin(); i != m_transitions.end(); ++i)
		{
			if ((i->m_condition)() && m_delay == 0.0f)
			{
				if (i->m_transition)
				{
					(i->m_transition)();
				}
				if (doImmediately)
				{
					(context->*(i->m_next->m_action))();
				}
#if 0
				char buf[32];
				sprintf_s(buf, "%s => %s\n", m_name.c_str(), i->m_next->m_name.c_str());
				OutputDebugStringA(buf);
#endif
				return i->m_next;
			}
		}
		return this;
	}

private:
	struct Transition
	{
		Transition(State* next, std::function<bool()> f, std::function<void()> tf) : 
			m_next(next), m_condition(f), m_transition(tf) {}
		State* m_next;
		std::function<bool()> m_condition;
		std::function<void()> m_transition;
	};
	void (T::*m_action)();
	std::string m_name;
	std::vector<Transition> m_transitions;
	float m_delay;
};
