#include <StateMachineBase.h>

State::State(String key, void (*on_enter)(), void (*on_state)(), void (*on_exit)())
    : key(key),
      on_enter(on_enter),
      on_state(on_state),
      on_exit(on_exit)
{
}

StateMachineBase::StateMachineBase()
    : m_transitions(NULL),
      m_num_transitions(0),
      m_num_timed_transitions(0),
      m_initialized(false)
{
}

StateMachineBase::~StateMachineBase()
{
  free(m_transitions);
  free(m_timed_transitions);
  m_transitions = NULL;
  m_timed_transitions = NULL;
}

void StateMachineBase::set_initial_state(State *initial_state)
{
  m_current_state = initial_state;
   Serial.println("Initial state: " + get_current_state_key());
}

String StateMachineBase::get_current_state_key()
{
  return m_current_state->key;
}

void StateMachineBase::add_transition(State *state_from, State *state_to, int event, void (*on_transition)())
{
  if (state_from == NULL || state_to == NULL)
    return;

  Transition transition = StateMachineBase::create_transition(state_from, state_to, event, on_transition);
  m_transitions = (Transition *)realloc(m_transitions, (m_num_transitions + 1) * sizeof(Transition));
  m_transitions[m_num_transitions] = transition;
  m_num_transitions++;
}

void StateMachineBase::add_timed_transition(State *state_from, State *state_to,
                                            unsigned long interval, void (*on_transition)())
{
  if (state_from == NULL || state_to == NULL)
    return;

  Transition transition = StateMachineBase::create_transition(state_from, state_to, 0, on_transition);

  TimedTransition timed_transition;
  timed_transition.transition = transition;
  timed_transition.start = 0;
  timed_transition.interval = interval;

  m_timed_transitions = (TimedTransition *)realloc(m_timed_transitions, (m_num_timed_transitions + 1) * sizeof(TimedTransition));
  m_timed_transitions[m_num_timed_transitions] = timed_transition;
  m_num_timed_transitions++;
}

StateMachineBase::Transition StateMachineBase::create_transition(State *state_from, State *state_to, int event, void (*on_transition)())
{
  Transition t;
  t.state_from = state_from;
  t.state_to = state_to;
  t.event = event;
  t.on_transition = on_transition;

  return t;
}

void StateMachineBase::trigger(int event)
{
  if (m_initialized)
  {
    // Find the transition with the current state and given event.
    for (int i = 0; i < m_num_transitions; ++i)
    {
      if (m_transitions[i].state_from == m_current_state && m_transitions[i].event == event)
      {
        StateMachineBase::make_transition(&(m_transitions[i]));
        return;
      }
    }
  }
}

void StateMachineBase::check_timed_transitions()
{
  for (int i = 0; i < m_num_timed_transitions; ++i)
  {
    TimedTransition *transition = &m_timed_transitions[i];
    if (transition->transition.state_from == m_current_state)
    {
      if (transition->start == 0)
      {
        transition->start = millis();
      }
      else
      {
        unsigned long now = millis();
        if (now - transition->start >= transition->interval)
        {
          StateMachineBase::make_transition(&(transition->transition));
          transition->start = 0;
        }
      }
    }
  }
}

void StateMachineBase::begin()
{

  m_initialized = true;

  if (m_current_state->on_enter != NULL){
    Serial.println("On enter initial state: " + get_current_state_key());
    m_current_state->on_enter();
  }
}

void StateMachineBase::loop()
{
  if (m_current_state->on_state != NULL)
  {
    m_current_state->on_state();
  }

  StateMachineBase::check_timed_transitions();
}


void StateMachineBase::make_transition(Transition *transition)
{
  // Execute the handlers in the correct order.
  if (transition->state_from->on_exit != NULL)
    transition->state_from->on_exit();

  if (transition->on_transition != NULL)
    transition->on_transition();

  if (transition->state_to->on_enter != NULL)
    transition->state_to->on_enter();

  m_current_state = transition->state_to;

  //Initialice all timed transitions from m_current_state
  unsigned long now = millis();
  for (int i = 0; i < m_num_timed_transitions; ++i)
  {
    TimedTransition *ttransition = &m_timed_transitions[i];
    if (ttransition->transition.state_from == m_current_state)
    {
      ttransition->start = now;
    }
  }
}
