#include <variant>
#include <iostream>

struct DoorState
{
    struct DoorOpened {};
    struct DoorClosed {};
    struct DoorLocked {};

    using State = std::variant<DoorOpened, DoorClosed, DoorLocked>;

    void open()
    {
        // returns the value from the functions
        m_state = std::visit(OpenEvent(), m_state);
    }

    void close()
    {
        m_state = std::visit(CloseEvent(), m_state);
    }

    void lock()
    {
        m_state = std::visit(LockEvent(), m_state);
    }

    void unlock()
    {
        m_state = std::visit(UnlockEvent(), m_state);
    }

    struct OpenEvent
    {
        State operator()(const DoorOpened&) { return DoorOpened(); }
        State operator()(const DoorClosed&) { return DoorOpened(); }
        // Cannot open locked door
        State operator()(const DoorLocked&) { return DoorLocked(); } 
    };

    struct CloseEvent
    {
        State operator()(const DoorOpened&) { return DoorClosed(); }
        State operator()(const DoorClosed&) { return DoorClosed(); }
        State operator()(const DoorLocked&) { return DoorLocked(); }
    };

    struct LockEvent
    {
        State operator()(const DoorOpened&) { return DoorOpened(); }
        State operator()(const DoorClosed&) { return DoorLocked(); }
        State operator()(const DoorLocked&) { return DoorLocked(); }
    };

    struct UnlockEvent
    {
        State operator()(const DoorOpened&) { return DoorOpened(); }
        State operator()(const DoorClosed&) { return DoorClosed(); }
        State operator())const DoorLocked&) { return DoorClosed(); }
    };

    State m_state;
}