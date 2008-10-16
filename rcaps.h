#include <sys/capability.h>

static void caps_setup_flags (void);	  //define EFFECTIVE, PERMITTED, etc.
static void caps_setup_constants (void);  //define names for capabilities

/* the standard c extension housekeeping methods */
static void caps_free (cap_t);

/* Caps class methods */
static VALUE caps_new (int, VALUE *, VALUE);
static VALUE caps_get_proc (int, VALUE *, VALUE);

/* Caps instance methods */
static VALUE caps_init (int, VALUE *, VALUE);
static VALUE caps_to_string (VALUE);
static VALUE caps_clear (VALUE);
static VALUE caps_set_proc (int, VALUE *, VALUE);
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
