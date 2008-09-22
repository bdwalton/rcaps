#include "ruby.h"
#include <sys/capability.h>

#include "rcaps.h"

static VALUE rb_cCaps;

Init_rcaps()
{
  rb_cCaps = rb_define_class("Caps", rb_cObject);
  rb_define_alloc_func(rb_cCaps, caps_allocate);

  /* Caps class methods */
  rb_define_singleton_method(rb_cCaps, "get_proc", caps_get_proc, 0);

  /* Caps instance methods */
  rb_define_method(rb_cCaps, "to_s", caps_to_string, 0);
  rb_define_method(rb_cCaps, "clear", caps_clear, 0);
  rb_define_method(rb_cCaps, "activate", caps_activate, 0);
  rb_define_method(rb_cCaps, "set_effective", caps_SET_EFFECTIVE, 1);
  rb_define_method(rb_cCaps, "clear_effective", caps_CLEAR_EFFECTIVE, 1);
  rb_define_method(rb_cCaps, "set_permitted", caps_SET_PERMITTED, 1);
  rb_define_method(rb_cCaps, "clear_permitted", caps_CLEAR_PERMITTED, 1);
  rb_define_method(rb_cCaps, "set_inheritable", caps_SET_INHERITABLE, 1);
  rb_define_method(rb_cCaps, "clear_inheritable", caps_CLEAR_INHERITABLE, 1);

  /* add flags and capability names */
  caps_setup_flags();
  caps_setup_constants();
}

/* functions related to Class Caps (housekeeping stuff) */

//allocate a new Caps object
static VALUE caps_allocate (VALUE klass) {
  //cap_t caps = cap_init();
  return Data_Wrap_Struct (klass, caps_mark, caps_free, cap_init());
}

//for the garbage collector...this object is too simple.
static void caps_mark (cap_t caps) {}

//how to destruct a Caps object
static void caps_free (cap_t caps) {
    cap_free(caps);
}

/* Caps class methods */
static VALUE caps_get_proc (VALUE klass) {
  cap_t caps;

  if ((caps = cap_get_proc()) == NULL)
    rb_raise(rb_eSystemCallError, "Error retrieving active capabilties.");

  return Data_Wrap_Struct(klass, caps_mark, caps_free, caps);
}

/* Caps instance methods */

// Caps#to_s implementation
static VALUE caps_to_string (VALUE self) {
  cap_t caps;
  char *text;

  Data_Get_Struct(self, struct _cap_struct, caps);
  text = cap_to_text(caps, NULL);
  return rb_str_new2(text);
}

static VALUE caps_clear (VALUE self) {
  cap_t caps;

  //we use struct _cap_struct here to shut up the gcc warning...
  Data_Get_Struct(self, struct _cap_struct, caps);

  if (cap_clear(caps) != 0)
    rb_raise(rb_eSystemCallError, "Error clearing capability set.");

  return self;
}

static VALUE caps_activate (VALUE self) {
  cap_t caps;

  Data_Get_Struct(self, struct _cap_struct, caps);

  if (cap_set_proc(caps) != 0)
    rb_raise(rb_eSystemCallError, "Error installing capability set.");

  return self;
}

//a generic function called by cap_{SET|CLEAR}_{EFFECTIVE|INHERITABLE|PERMITTED}
static VALUE captoggle(VALUE self, VALUE cap, cap_flag_t type, cap_flag_value_t toggle) {
  cap_t caps;
  cap_value_t set[1];

  Data_Get_Struct(self, struct _cap_struct, caps);

  Check_Type(cap, T_FIXNUM);
  set[0] = FIX2INT(cap);

  if (cap_set_flag(caps, type, 1, set, toggle) != 0)
    rb_raise(rb_eSystemCallError, "Error making capability flag change.");

  return self;
}

CAPMOD(EFFECTIVE,SET);
CAPMOD(EFFECTIVE,CLEAR);
CAPMOD(PERMITTED,SET);
CAPMOD(PERMITTED,CLEAR);
CAPMOD(INHERITABLE,SET);
CAPMOD(INHERITABLE,CLEAR);

static void caps_setup_flags (void) {
  /* these constants are the basis of the enumerated type cap_flag_t */
  rb_define_const(rb_cCaps, "EFFECTIVE", INT2FIX(CAP_EFFECTIVE));
  rb_define_const(rb_cCaps, "PERMITTED", INT2FIX(CAP_PERMITTED));
  rb_define_const(rb_cCaps, "INHERITABLE", INT2FIX(CAP_INHERITABLE));
}

static void caps_setup_constants (void) {
  /* these constants represent capabilities that may be toggled in on/off
   * in one of the sets of cap_flag_t enumerated list */
  rb_define_const(rb_cCaps, "CHOWN", INT2FIX(CAP_CHOWN));
  rb_define_const(rb_cCaps, "DAC_OVERRIDE", INT2FIX(CAP_DAC_OVERRIDE));
  rb_define_const(rb_cCaps, "DAC_READ_SEARCH", INT2FIX(CAP_DAC_READ_SEARCH));
  rb_define_const(rb_cCaps, "FOWNER", INT2FIX(CAP_FOWNER));
  rb_define_const(rb_cCaps, "FSETID", INT2FIX(CAP_FSETID));
  rb_define_const(rb_cCaps, "KILL", INT2FIX(CAP_KILL));
  rb_define_const(rb_cCaps, "SETGID", INT2FIX(CAP_SETGID));
  rb_define_const(rb_cCaps, "SETUID", INT2FIX(CAP_SETUID));
  rb_define_const(rb_cCaps, "LINUX_IMMUTABLE", INT2FIX(CAP_LINUX_IMMUTABLE));
  rb_define_const(rb_cCaps, "NET_BIND_SERVICE", INT2FIX(CAP_NET_BIND_SERVICE));
  rb_define_const(rb_cCaps, "NET_BROADCAST", INT2FIX(CAP_NET_BROADCAST));
  rb_define_const(rb_cCaps, "NET_ADMIN", INT2FIX(CAP_NET_ADMIN));
  rb_define_const(rb_cCaps, "NET_RAW", INT2FIX(CAP_NET_RAW));
  rb_define_const(rb_cCaps, "IPC_LOCK", INT2FIX(CAP_IPC_LOCK));
  rb_define_const(rb_cCaps, "IPC_OWNER", INT2FIX(CAP_IPC_OWNER));
  rb_define_const(rb_cCaps, "SYS_MODULE", INT2FIX(CAP_SYS_MODULE));
  rb_define_const(rb_cCaps, "SYS_RAWIO", INT2FIX(CAP_SYS_RAWIO));
  rb_define_const(rb_cCaps, "SYS_CHROOT", INT2FIX(CAP_SYS_CHROOT));
  rb_define_const(rb_cCaps, "SYS_PTRACE", INT2FIX(CAP_SYS_PTRACE));
  rb_define_const(rb_cCaps, "SYS_PACCT", INT2FIX(CAP_SYS_PACCT));
  rb_define_const(rb_cCaps, "SYS_ADMIN", INT2FIX(CAP_SYS_ADMIN));
  rb_define_const(rb_cCaps, "SYS_BOOT", INT2FIX(CAP_SYS_PACCT));
  rb_define_const(rb_cCaps, "SYS_NICE", INT2FIX(CAP_SYS_NICE));
  rb_define_const(rb_cCaps, "SYS_RESOURCE", INT2FIX(CAP_SYS_RESOURCE));
  rb_define_const(rb_cCaps, "SYS_TIME", INT2FIX(CAP_SYS_TIME));
  rb_define_const(rb_cCaps, "SYS_TTY_CONFIG", INT2FIX(CAP_SYS_TTY_CONFIG));
  rb_define_const(rb_cCaps, "MKNOD", INT2FIX(CAP_MKNOD));
  rb_define_const(rb_cCaps, "LEASE", INT2FIX(CAP_LEASE));
}
