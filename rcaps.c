/*
 * = RCaps - Ruby Capabilities
 *
 * Manipulate Linux Kernel/Process capabilities from Ruby.
 *
 * For information about how capabilities work, see: man 7 capabilities
 *
 * Author:: Ben Walton mailto:bdwalton@gmail.com
 * Copyright:: Copyright (C) 2008, Ben Walton
 * License:: This code is released under the terms of the GPL v3.
 *
 * == Examples
 *
 * === Fetch running capabilities and modify them.
 * require 'rcaps'
 *
 * c = Caps.get_proc
 *
 * c.set_inheritable([Caps::SETUID])
 *
 * c.set_proc
 *
 * === Start with capabilities in string form and modify them.
 * require 'rcaps'
 *
 * s = '= cap_chown,cap_setuid+e'
 *
 * c = Caps.new(s)
 *
 * c.set_permitted([Caps::MKNOD])
 *
 * c.set_proc
 *
 * === Start with an empty capability set and modify it.
 * require 'rcaps'
 *
 * c = Caps.new
 *
 * c.set_permitted([Caps::MKNOD])
 *
 * c.set_proc
 *
 * === Fetch running capabilities and clear them.
 * require 'rcaps'
 *
 * c = Caps.get_proc
 *
 * c.clear
 *
 * c.set_proc
 *
 * === Fetch running capabilities from another process and clear them.
 * require 'rcaps'
 *
 * c = Caps.get_proc(1234)
 *
 * c.clear
 *
 * c.set_proc(1234)
 *
 * === Fetch running capabilities and ensure a certain capability is not set.
 * require 'rcaps'
 *
 * c = Caps.get_proc
 *
 * c.clear_effective([Caps::SETUID])
 *
 * c.clear_permitted([Caps::SETUID])
 *
 * c.clear_inheritable([Caps::SETUID])
 *
 * c.set_proc
 *
 * === Strip current capabilities.
 * require 'rcaps'
 *
 * c = Caps.new
 *
 * c.set_proc
 *
 * === Fetch running capabilities and see if a certain privilege is granted.
 * require 'rcaps'
 *
 * c = Caps.get_proc
 *
 * puts "I can change my UID's!" if c.effective?(Caps::SETUID)
 */

#include "ruby.h"
//so that we get _cap_names.
#undef _POSIX_SOURCE
#include <sys/capability.h>

#include "rcaps.h"

static VALUE rb_cCaps;

Init_rcaps()
{
  rb_cCaps = rb_define_class("Caps", rb_cObject);

  /* Caps class methods */
  rb_define_singleton_method(rb_cCaps, "new", caps_new, -1);
  rb_define_singleton_method(rb_cCaps, "get_proc", caps_get_proc, -1);

  /* Caps instance methods */
  rb_define_method(rb_cCaps, "initialize", caps_init, -1);
  rb_define_method(rb_cCaps, "to_s", caps_to_string, 0);
  rb_define_method(rb_cCaps, "clear", caps_clear, 0);
  rb_define_method(rb_cCaps, "set_proc", caps_set_proc, -1);
  rb_define_method(rb_cCaps, "set_effective", caps_SET_EFFECTIVE, 1);
  rb_define_method(rb_cCaps, "clear_effective", caps_CLEAR_EFFECTIVE, 1);
  rb_define_method(rb_cCaps, "set_permitted", caps_SET_PERMITTED, 1);
  rb_define_method(rb_cCaps, "clear_permitted", caps_CLEAR_PERMITTED, 1);
  rb_define_method(rb_cCaps, "set_inheritable", caps_SET_INHERITABLE, 1);
  rb_define_method(rb_cCaps, "clear_inheritable", caps_CLEAR_INHERITABLE, 1);
  rb_define_method(rb_cCaps, "effective?", caps_EFFECTIVE, 1);
  rb_define_method(rb_cCaps, "permitted?", caps_PERMITTED, 1);
  rb_define_method(rb_cCaps, "inheritable?", caps_INHERITABLE, 1);

  /* add capability names as contants to the class */
  caps_setup_constants();
}

// Housekeeping for the GC
static void caps_free (cap_t caps) {
    cap_free(caps);
}

// Class/singleton methods

/*
 * Returns an object representing a capability set in working memory.  If
 * an argument is passed, it must be a string representing a set of capabilities
 * in string form.  This is analagous to obtaining a capability set by calling
 * the C level routine cap_from_text.
 */
static VALUE caps_new (int argc, VALUE *argv, VALUE klass) {
  cap_t caps;
  VALUE cdata, capstring;

  rb_scan_args(argc, argv, "01", &capstring);

  if (NIL_P(capstring)) {
    caps = cap_init();
  } else {
    Check_Type(capstring, T_STRING);
    caps = cap_from_text(StringValuePtr(capstring));
    if (!caps)
      rb_sys_fail("Bad capability text argument");
  }

  cdata = Data_Wrap_Struct(klass, 0, caps_free, caps);
  rb_obj_call_init(cdata, argc, argv);

  return cdata;
}

/*
 * Returns a new Caps object initialized with the set of capabilities from
 * the currently running process.
 * If passed a pid, the capabilities for that process will be retrieved
 * instead.
 */
static VALUE caps_get_proc (int argc, VALUE *argv, VALUE klass) {
  cap_t caps;
  VALUE cdata, optional_pid;
  pid_t pid;

  rb_scan_args(argc, argv, "01", &optional_pid);

  switch(TYPE(optional_pid)) {
    case T_NIL:
      //without an argument, we'll fetch our own current capabilities.
      pid = getpid();
      break;
    case T_FIXNUM:
      pid = (pid_t) FIX2INT(optional_pid);
      break;
    default:
      rb_raise(rb_eTypeError, "Invalid value passed as PID to retrieve capabilities from.");
      break;
  }

  if ((caps = cap_init()) == NULL)
    rb_sys_fail("Error initializing empty capability set");
  else if (capgetp(pid, caps) != 0)
    rb_sys_fail("Error retrieving capabilities from active process");

  //as this is just a convenience wrapper instead of new(), we still
  //want to call initialize.
  cdata = Data_Wrap_Struct(klass, 0, caps_free, caps);
  rb_obj_call_init(cdata, argc, argv);

  return cdata;
}

// Caps instance methods

/*
 * Returns an object representing a capability set in working memory.  If
 * an argument is passed, it must be a string representing a set of capabilities
 * in string form.  This is analagous to obtaining a capability set by calling
 * the C level routine cap_from_text.
 */
static VALUE caps_init (int argc, VALUE *argv, VALUE self) {
  // we don't do a whole lot here...
  return self;
}

/*
 * Returns a String object that contains the text representation of the
 * Caps object.  This is analagous to the C level function cap_to_text.
 */
static VALUE caps_to_string (VALUE self) {
  cap_t caps;
  char *text;

  Data_Get_Struct(self, struct _cap_struct, caps);
  text = cap_to_text(caps, NULL);

  return rb_str_new2(text);
}

/*
 * Removes all capabilties from the Caps object.  This is analogous to the C
 * level function cap_clear.
 */
static VALUE caps_clear (VALUE self) {
  cap_t caps;

  //we use struct _cap_struct here to shut up the gcc warning...
  Data_Get_Struct(self, struct _cap_struct, caps);

  if (cap_clear(caps) != 0)
    rb_sys_fail("Error clearing capability set.");

  return self;
}

/*
 * Install the Caps object into the kernel.  This is analogous to the C level
 * function cap_set_proc.
 * If a pid is given, the capabilities will be installed for that process
 * instead.  [Note: This likely won't work, but we support it anyway]
 */
static VALUE caps_set_proc (int argc, VALUE *argv, VALUE self) {
  cap_t caps;
  VALUE optional_pid;
  pid_t pid;

  Data_Get_Struct(self, struct _cap_struct, caps);
  rb_scan_args(argc, argv, "01", &optional_pid);

  switch(TYPE(optional_pid)) {
    case T_NIL:
      //without an argument, we'll fetch our own current capabilities.
      pid = getpid();
      break;
    case T_FIXNUM:
      pid = (pid_t) FIX2INT(optional_pid);
      break;
    default:
      rb_raise(rb_eTypeError, "Invalid value passed as PID to retrieve capabilities from.");
      break;
  }

  if (capsetp(pid, caps) != 0)
    rb_sys_fail("Error setting capabilities for process");

  return self;
}

//a generic function called by cap_{SET|CLEAR}_{EFFECTIVE|INHERITABLE|PERMITTED}
static VALUE captoggle(VALUE self, VALUE caplist, cap_flag_t type, cap_flag_value_t toggle) {
  VALUE arrsize, arrelem;
  cap_t caps;
  cap_value_t *set = NULL;
  int listsize, i, arrval;

  Data_Get_Struct(self, struct _cap_struct, caps);

  Check_Type(caplist, T_ARRAY);

  arrsize = rb_funcall(caplist, rb_intern("size"), 0);
  //don't allow user to set more caps at once than there are caps.
  switch (TYPE(arrsize)) {
    case T_FIXNUM:
      listsize = FIX2INT(arrsize);
      if (listsize > CAP_LEASE)
	rb_raise(rb_eArgError, "Too many capabilities to set at once");

      if ((set = malloc(listsize * sizeof(cap_value_t))) == NULL)
	rb_sys_fail("Error allocating memory for capability array");

      for (i = 0; i < listsize; i++) {
	arrelem = rb_funcall(caplist, rb_intern("[]"), 1, INT2FIX(i));
	Check_Type(arrelem, T_FIXNUM);
	arrval = FIX2INT(arrelem);
	if (arrval < CAP_CHOWN || arrval > CAP_LEASE) {
	  free(set);
	  rb_raise(rb_eArgError, "Invalid capability in list argument");
	}

	set[i] = arrval;
      }
      break;
    default: //if we're a bignum, we're way too large...
      rb_raise(rb_eArgError, "Too many capabilities to set at once");
      break;
  }

  if (cap_set_flag(caps, type, listsize, set, toggle) != 0) {
    free(set);
    rb_sys_fail("Error making capability flag change");
  }

  if (set) free(set);

  return self;
}

/*
 * Add the Capabilities listed in the first argument array to the effective
 * set.
 *
 * p1 must be an array containing Capabilities defined as constants in the Caps
 * class.
 */
static VALUE caps_SET_EFFECTIVE (VALUE self, VALUE cap) {
  return(captoggle(self, cap, CAP_EFFECTIVE, CAP_SET));
}

/*
 * Clear the Capabilities listed in the first argument array from the effective
 * set.
 *
 * p1 must be an array containing Capabilities defined as constants in the Caps
 * class.
 */
static VALUE caps_CLEAR_EFFECTIVE (VALUE self, VALUE cap) {
  return(captoggle(self, cap, CAP_EFFECTIVE, CAP_CLEAR));
}

/*
 * Add the Capabilities listed in the first argument array to the permitted
 * set.
 *
 * p1 must be an array containing Capabilities defined as constants in the Caps
 * class.
 */
static VALUE caps_SET_PERMITTED (VALUE self, VALUE cap) {
  return(captoggle(self, cap, CAP_PERMITTED, CAP_SET));
}

/*
 * Clear the Capabilities listed in the first argument array from the permitted
 * set.
 *
 * p1 must be an array containing Capabilities defined as constants in the Caps
 * class.
 */
static VALUE caps_CLEAR_PERMITTED (VALUE self, VALUE cap) {
  return(captoggle(self, cap, CAP_PERMITTED, CAP_CLEAR));
}

/*
 * Add the Capabilities listed in the first argument array to the inheritable
 * set.
 *
 * p1 must be an array containing Capabilities defined as constants in the Caps
 * class.
 */
static VALUE caps_SET_INHERITABLE (VALUE self, VALUE cap) {
  return(captoggle(self, cap, CAP_INHERITABLE, CAP_SET));
}

/*
 * Clear the Capabilities listed in the first argument array from the
 * inheritable set.
 *
 * p1 must be an array containing Capabilities defined as constants in the Caps
 * class.
 */
static VALUE caps_CLEAR_INHERITABLE (VALUE self, VALUE cap) {
  return(captoggle(self, cap, CAP_INHERITABLE, CAP_CLEAR));
}

static VALUE capisset (VALUE self, VALUE cap, cap_flag_t flag) {
  cap_t caps;
  cap_flag_value_t val;

  Data_Get_Struct(self, struct _cap_struct, caps);

  Check_Type(cap, T_FIXNUM);

  if (cap_get_flag(caps, FIX2INT(cap), flag, &val) != 0)
    rb_sys_fail("Error retrieving capability status");

  return (val == CAP_SET ? Qtrue : Qfalse);
}

/*
 * Return a boolean response indicating whether a capability is effective or
 * not within the working set.
 *
 * p1 is a constant referring to a Capability as defined in the Caps class.
 */
static VALUE caps_EFFECTIVE (VALUE self, VALUE cap) {\
  return(capisset(self, cap, CAP_EFFECTIVE));\
}

/*
 * Return a boolean response indicating whether a capability is permitted or
 * not within the working set.
 *
 * p1 is a constant referring to a Capability as defined in the Caps class.
 */
static VALUE caps_PERMITTED (VALUE self, VALUE cap) {\
  return(capisset(self, cap, CAP_PERMITTED));\
}

/*
 * Return a boolean response indicating whether a capability is inheritable or
 * not within the working set.
 *
 * p1 is a constant referring to a Capability as defined in the Caps class.
 */
static VALUE caps_INHERITABLE (VALUE self, VALUE cap) {\
  return(capisset(self, cap, CAP_INHERITABLE));\
}

static void caps_setup_constants (void) {
  int i;
  char *sname, *x;

  /* these constants represent capabilities that may be toggled on/off
   * in one of the sets of cap_flag_t enumerated list */

  /* This makes us Linux specific, as _cap_names is not part of the POSIX
   *  spec. */
  for (i = 0; _cap_names[i]; i++) {
    sname = strdup((char *)(_cap_names[i] + 4));  //strip off the cap_ part.
    x = sname;
    while (*x) {
      *x = toupper(*x);
      x++;
    }

    rb_define_const(rb_cCaps, sname, INT2FIX(i));
    free(sname);
  }
}
