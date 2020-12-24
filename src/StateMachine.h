#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

struct State
{
  State(String key, void (*on_enter)(), void (*on_state)(), void (*on_exit)());
  String key;
  void (*on_enter)();
  void (*on_state)();
  void (*on_exit)();
};

class Fsm
{
public:
  Fsm(State* initial_state);
  ~Fsm();

  void add_transition(State* state_from, State* state_to, int event,
                      void (*on_transition)());

  void add_timed_transition(State* state_from, State* state_to,
                            unsigned long interval, void (*on_transition)());

  void check_timed_transitions();

  void trigger(int event);
  void loop();

  String get_current_state_key();

private:
  struct Transition
  {
    State* state_from;
    State* state_to;
    int event;
    void (*on_transition)();

  };

  struct TimedTransition
  {
    Transition transition;
    unsigned long start;
    unsigned long interval;
  };

  static Transition create_transition(State* state_from, State* state_to,
                                      int event, void (*on_transition)());

  void make_transition(Transition* transition);

private:
  State* m_current_state;
  Transition* m_transitions;
  int m_num_transitions;

  TimedTransition* m_timed_transitions;
  int m_num_timed_transitions;
  bool m_initialized;
};

#endif
