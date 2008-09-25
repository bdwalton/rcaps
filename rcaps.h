#include <sys/capability.h>

/* prototypes */
static void caps_setup_flags (void);	  //define EFFECTIVE, PERMITTED, etc.
static void caps_setup_constants (void);  //define names for capabilities

/* the standard c extension housekeeping methods */
//static VALUE caps_allocate (VALUE);
//static void caps_mark (cap_t);
static void caps_free (cap_t);

/* Caps class methods */
static VALUE caps_init (int, VALUE *, VALUE);
static VALUE caps_new (int, VALUE *, VALUE);
static VALUE caps_get_proc (VALUE);

/* Caps instance methods */
static VALUE caps_to_string (VALUE);
static VALUE caps_clear (VALUE);
static VALUE caps_set_proc (VALUE);
static VALUE captoggle (VALUE, VALUE, cap_flag_t, cap_flag_value_t);
// functions to toggle capabilities in the various sets.
static VALUE caps_SET_EFFECTIVE (VALUE, VALUE);
static VALUE caps_CLEAR_EFFECTIVE (VALUE, VALUE);
static VALUE caps_SET_PERMITTED (VALUE, VALUE);
static VALUE caps_CLEAR_PERMITTED (VALUE, VALUE);
static VALUE caps_SET_INHERITABLE (VALUE, VALUE);
static VALUE caps_CLEAR_INHERITABLE (VALUE, VALUE);
static VALUE caps_EFFECTIVE (VALUE, VALUE);
static VALUE caps_PERMITTED (VALUE, VALUE);
static VALUE caps_INHERITABLE (VALUE, VALUE);

#define CAPTOG(CSET, CTOGGLE) \
static VALUE caps_## CTOGGLE ## _ ## CSET (VALUE self, VALUE cap) {\
  return(captoggle(self, cap, CAP_ ## CSET, CAP_ ## CTOGGLE));\
}

#define CAPQUERY(CSET) \
static VALUE caps_## CSET (VALUE self, VALUE cap) {\
  return(capisset(self, cap, CAP_ ## CSET));\
}
