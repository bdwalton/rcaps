#include <sys/capability.h>

/* prototypes */
static void caps_setup_flags (void);	  //define EFFECTIVE, PERMITTED, etc.
static void caps_setup_constants (void);  //define names for capabilities

/* the standard c extension housekeeping methods */
static VALUE caps_allocate (VALUE);
static void caps_mark (cap_t);
static void caps_free (cap_t);

/* Caps class methods */
static VALUE caps_get_proc (VALUE);

/* Caps instance methods */
static VALUE caps_to_string (VALUE);
static VALUE caps_clear (VALUE);
static VALUE caps_activate (VALUE);
static VALUE captoggle (VALUE, VALUE, cap_flag_t, cap_flag_value_t);
// functions to toggle capabilities in the various sets.
static VALUE caps_SET_EFFECTIVE (VALUE, VALUE);
static VALUE caps_CLEAR_EFFECTIVE (VALUE, VALUE);
static VALUE caps_SET_PERMITTED (VALUE, VALUE);
static VALUE caps_CLEAR_PERMITTED (VALUE, VALUE);
static VALUE caps_SET_INHERITABLE (VALUE, VALUE);
static VALUE caps_CLEAR_INHERITABLE (VALUE, VALUE);

#define CAPMOD(CSET, CTOGGLE) \
static VALUE caps_## CTOGGLE ## _ ## CSET (VALUE self, VALUE cap) {\
  return(captoggle(self, cap, CAP_ ## CSET, CAP_ ## CTOGGLE));\
}
