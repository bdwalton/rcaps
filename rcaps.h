#include <sys/capability.h>

/* prototypes */
static void caps_setup_flags (void);	  //define EFFECTIVE, PERMITTED, etc.
static void caps_setup_constants (void);  //define names for capabilities

/* the standard c extension housekeeping methods */
static VALUE caps_allocate (VALUE klass);
static void caps_mark (cap_t caps);
static void caps_free (cap_t caps);

/* Caps class methods */
static VALUE caps_get_proc (VALUE klass);

/* Caps instance methods */
static VALUE caps_to_string (VALUE self);
