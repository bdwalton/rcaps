#include <sys/capability.h>

/* prototypes */
static VALUE capset_allocate (VALUE klass);
static void capset_mark (cap_t caps);
static void capset_free (cap_t caps);
static VALUE capset_to_string (VALUE self);
static VALUE caps_current (void);
