#include "ruby.h"
#include <sys/capability.h>

#include "rcaps.h"

static VALUE rb_cCaps;

Init_rcaps()
{
  rb_cCaps = rb_define_class("Caps", rb_cObject);
  //rb_define_alloc_func(rb_cCaps, caps_allocate);

  /* Caps class methods */
  rb_define_singleton_method(rb_cCaps, "new", caps_new, -1);
  rb_define_singleton_method(rb_cCaps, "get_proc", caps_get_proc, 0);

  /* Caps instance methods */
  rb_define_method(rb_cCaps, "initialize", caps_init, -1);
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

/* Housekeeping for the GC */
//how to destruct a Caps object
static void caps_free (cap_t caps) {
    cap_free(caps);
}

/* Class/singleton methods */

static VALUE caps_new (int argc, VALUE *argv, VALUE klass) {
  cap_t caps;
  VALUE cdata;

  if (argc < 1) {
    caps = cap_init();
  } else if (argc > 1) {
    rb_raise(rb_eArgError, "More than one argument to new()");
  } else {
    Check_Type(argv[0], T_STRING);
    caps = cap_from_text(StringValuePtr(argv[0]));
    if (!caps)
      rb_raise(rb_eArgError, "Invalid capability text definition passed to new()");
  }

  cdata = Data_Wrap_Struct(klass, 0, caps_free, caps);
  rb_obj_call_init(cdata, argc, argv);

  return cdata;
}

static VALUE caps_get_proc (VALUE klass) {
  cap_t caps;
  VALUE cdata;
  VALUE argv[1];

  if ((caps = cap_get_proc()) == NULL)
    rb_raise(rb_eSystemCallError, "Error retrieving active capabilties.");

  //as this is just a convenience wrapper instead of new(), we still
  //want to call initialize.
  cdata = Data_Wrap_Struct(klass, 0, caps_free, caps);
  argv[0] = rb_funcall(cdata, rb_intern("to_s"), 0);
  rb_obj_call_init(cdata, 1, argv);

  return cdata;
}

/* Caps instance methods */

static VALUE caps_init (int argc, VALUE *argv, VALUE self) {
  // we don't do a whole lot here...
  return self;
}

// Caps#to_s implementation
static VALUE caps_to_string (VALUE self) {
  cap_t caps;
  char *text;

  Data_Get_Struct(self, struct _cap_struct, caps);
  text = cap_to_text(caps, NULL);

  return rb_str_new2(text);
}

// Caps#clear
static VALUE caps_clear (VALUE self) {
  cap_t caps;

  //we use struct _cap_struct here to shut up the gcc warning...
  Data_Get_Struct(self, struct _cap_struct, caps);

  if (cap_clear(caps) != 0)
    rb_raise(rb_eSystemCallError, "Error clearing capability set.");

  return self;
}

// Caps#activate
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
  //as we bumped our numbers by 1 from the value in the header file.
  set[0] = FIX2INT(cap) - 1;

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

  //NOTE: We increase the numbers defined in the header by 1 so that we
  //can make user submitted sets of these capabilities easer to manipulate
  //for our own purposes (and thus easier for the user to set in batches)
  rb_define_const(rb_cCaps, "CHOWN", INT2FIX(CAP_CHOWN + 1));
  rb_define_const(rb_cCaps, "DAC_OVERRIDE", INT2FIX(CAP_DAC_OVERRIDE + 1));
  rb_define_const(rb_cCaps, "DAC_READ_SEARCH", INT2FIX(CAP_DAC_READ_SEARCH + 1));
  rb_define_const(rb_cCaps, "FOWNER", INT2FIX(CAP_FOWNER + 1));
  rb_define_const(rb_cCaps, "FSETID", INT2FIX(CAP_FSETID + 1));
  rb_define_const(rb_cCaps, "KILL", INT2FIX(CAP_KILL + 1));
  rb_define_const(rb_cCaps, "SETGID", INT2FIX(CAP_SETGID + 1));
  rb_define_const(rb_cCaps, "SETUID", INT2FIX(CAP_SETUID + 1));
  rb_define_const(rb_cCaps, "LINUX_IMMUTABLE", INT2FIX(CAP_LINUX_IMMUTABLE + 1));
  rb_define_const(rb_cCaps, "NET_BIND_SERVICE", INT2FIX(CAP_NET_BIND_SERVICE + 1));
  rb_define_const(rb_cCaps, "NET_BROADCAST", INT2FIX(CAP_NET_BROADCAST + 1));
  rb_define_const(rb_cCaps, "NET_ADMIN", INT2FIX(CAP_NET_ADMIN + 1));
  rb_define_const(rb_cCaps, "NET_RAW", INT2FIX(CAP_NET_RAW + 1));
  rb_define_const(rb_cCaps, "IPC_LOCK", INT2FIX(CAP_IPC_LOCK + 1));
  rb_define_const(rb_cCaps, "IPC_OWNER", INT2FIX(CAP_IPC_OWNER + 1));
  rb_define_const(rb_cCaps, "SYS_MODULE", INT2FIX(CAP_SYS_MODULE + 1));
  rb_define_const(rb_cCaps, "SYS_RAWIO", INT2FIX(CAP_SYS_RAWIO + 1));
  rb_define_const(rb_cCaps, "SYS_CHROOT", INT2FIX(CAP_SYS_CHROOT + 1));
  rb_define_const(rb_cCaps, "SYS_PTRACE", INT2FIX(CAP_SYS_PTRACE + 1));
  rb_define_const(rb_cCaps, "SYS_PACCT", INT2FIX(CAP_SYS_PACCT + 1));
  rb_define_const(rb_cCaps, "SYS_ADMIN", INT2FIX(CAP_SYS_ADMIN + 1));
  rb_define_const(rb_cCaps, "SYS_BOOT", INT2FIX(CAP_SYS_PACCT + 1));
  rb_define_const(rb_cCaps, "SYS_NICE", INT2FIX(CAP_SYS_NICE + 1));
  rb_define_const(rb_cCaps, "SYS_RESOURCE", INT2FIX(CAP_SYS_RESOURCE + 1));
  rb_define_const(rb_cCaps, "SYS_TIME", INT2FIX(CAP_SYS_TIME + 1));
  rb_define_const(rb_cCaps, "SYS_TTY_CONFIG", INT2FIX(CAP_SYS_TTY_CONFIG + 1));
  rb_define_const(rb_cCaps, "MKNOD", INT2FIX(CAP_MKNOD + 1));
  rb_define_const(rb_cCaps, "LEASE", INT2FIX(CAP_LEASE + 1));
}
