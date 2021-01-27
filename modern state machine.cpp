//http://www.vishalchovatiya.com/state-design-pattern-in-modern-cpp/

#include <iostream>
#include <optional>
#include <string>

#include <boost/any.hpp>
#include <boost/foreach.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/variant/apply_visitor.hpp>

//using namespace std;
using namespace boost;

/* ---------------------------------  Events ------------------------------------------ */
class EventConnect
{
    std::string m_address = "EventConnect";
};

class EventConnected
{
     std::string m_address = "EventConnected";
};

class EventDisconnect
{
     std::string m_address = "EventDisconnect";
};

class EventTimeout
{
     std::string m_address = "EventTimeout";
};

using Event = variant<EventConnect, EventConnected, EventDisconnect, EventTimeout>;
/* ------------------------------------------------------------------------------------- */
/* ---------------------------------  States ------------------------------------------ */
class Idle
{
    uint32_t m_trial = 0;
};

class Connecting
{
public:
    std::string m_address;
    uint32_t m_trial = 0;
    static constexpr uint8_t m_max_trial = 3;
};

class Connected
{
    uint32_t m_trial = 0;
};

using State = variant<Idle, Connecting, Connected>;

class Transitions 
    : public static_visitor<optional<State>>
{
public:

    optional<State> operator()(Idle k, EventTimeout i) const
    {
        State s = Connecting{};
        return s;
    }

    optional<State> operator()(Connecting state, EventConnect event) const
    {
        ++state.m_trial;
        State s = Idle{};
        return s;
    }

 
    optional<State> operator()(Idle state, EventDisconnect event) const
    {
        State s = Connected{};
        return s;
    }

    template <typename T, typename U>
    optional<State> operator()(T ss,  U eee) const
    {
        State s = Idle{};
        return s;
    }
};

class Bluetooth
{
public:
    optional<State> m_curr_state = Idle{};

    void dispatch(Event event)
    {
        if (m_curr_state.has_value())
        {

            optional<State> new_state = apply_visitor(Transitions (), m_curr_state.value(), event);
            if (new_state)
            {
                m_curr_state = new_state;
            }
        } else
        {
            auto i = 8;
        }
    }
};

int main()
{
    Bluetooth bl;

    const Event timeout(EventTimeout{});

    bl.dispatch(timeout);

    const Event v2(EventConnect{});

    bl.dispatch(v2);

     const Event v3(EventDisconnect{});

    bl.dispatch(v3);

    return EXIT_SUCCESS;
}
