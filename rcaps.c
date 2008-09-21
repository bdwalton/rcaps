#include "ruby.h"
#include <sys/capability.h>

#include "rcaps.h"

static VALUE rb_mCaps;
static VALUE rb_cCapSet;

Init_rcaps()
{
  rb_mCaps = rb_define_module("Capabilities");
  rb_cCapSet = rb_define_class_under(rb_mCaps, "Set", rb_cObject);
  rb_define_alloc_func(rb_cCapSet, capset_allocate);

  /* Capabilities mdoule methods */
  rb_define_method(rb_mCaps, "current", caps_current, 0);

  /* Capabilities::Set methods */
  rb_define_method(rb_cCapSet, "to_s", capset_to_string, 0);

  /* these constants are the basis of the enumerated type cap_flag_t */
  rb_define_const(rb_mCaps, "EFFECTIVE", INT2FIX(CAP_EFFECTIVE));
  rb_define_const(rb_mCaps, "PERMITTED", INT2FIX(CAP_PERMITTED));
  rb_define_const(rb_mCaps, "INHERITABLE", INT2FIX(CAP_INHERITABLE));

  /* these constants represent capabilities that may be toggled in on/off
   * in one of the sets of cap_flag_t enumerated list */
  rb_define_const(rb_mCaps, "CHOWN", INT2FIX(CAP_CHOWN));
  rb_define_const(rb_mCaps, "DAC_OVERRIDE", INT2FIX(CAP_DAC_OVERRIDE));
  rb_define_const(rb_mCaps, "DAC_READ_SEARCH", INT2FIX(CAP_DAC_READ_SEARCH));
  rb_define_const(rb_mCaps, "FOWNER", INT2FIX(CAP_FOWNER));
  rb_define_const(rb_mCaps, "FSETID", INT2FIX(CAP_FSETID));
  rb_define_const(rb_mCaps, "KILL", INT2FIX(CAP_KILL));
  rb_define_const(rb_mCaps, "SETGID", INT2FIX(CAP_SETGID));
  rb_define_const(rb_mCaps, "SETUID", INT2FIX(CAP_SETUID));
  rb_define_const(rb_mCaps, "LINUX_IMMUTABLE", INT2FIX(CAP_LINUX_IMMUTABLE));
  rb_define_const(rb_mCaps, "NET_BIND_SERVICE", INT2FIX(CAP_NET_BIND_SERVICE));
  rb_define_const(rb_mCaps, "NET_BROADCAST", INT2FIX(CAP_NET_BROADCAST));
  rb_define_const(rb_mCaps, "NET_ADMIN", INT2FIX(CAP_NET_ADMIN));
  rb_define_const(rb_mCaps, "NET_RAW", INT2FIX(CAP_NET_RAW));
  rb_define_const(rb_mCaps, "IPC_LOCK", INT2FIX(CAP_IPC_LOCK));
  rb_define_const(rb_mCaps, "IPC_OWNER", INT2FIX(CAP_IPC_OWNER));
  rb_define_const(rb_mCaps, "SYS_MODULE", INT2FIX(CAP_SYS_MODULE));
  rb_define_const(rb_mCaps, "SYS_RAWIO", INT2FIX(CAP_SYS_RAWIO));
  rb_define_const(rb_mCaps, "SYS_CHROOT", INT2FIX(CAP_SYS_CHROOT));
  rb_define_const(rb_mCaps, "SYS_PTRACE", INT2FIX(CAP_SYS_PTRACE));
  rb_define_const(rb_mCaps, "SYS_PACCT", INT2FIX(CAP_SYS_PACCT));
  rb_define_const(rb_mCaps, "SYS_ADMIN", INT2FIX(CAP_SYS_ADMIN));
  rb_define_const(rb_mCaps, "SYS_BOOT", INT2FIX(CAP_SYS_PACCT));
  rb_define_const(rb_mCaps, "SYS_NICE", INT2FIX(CAP_SYS_NICE));
  rb_define_const(rb_mCaps, "SYS_RESOURCE", INT2FIX(CAP_SYS_RESOURCE));
  rb_define_const(rb_mCaps, "SYS_TIME", INT2FIX(CAP_SYS_TIME));
  rb_define_const(rb_mCaps, "SYS_TTY_CONFIG", INT2FIX(CAP_SYS_TTY_CONFIG));
  rb_define_const(rb_mCaps, "MKNOD", INT2FIX(CAP_MKNOD));
  rb_define_const(rb_mCaps, "LEASE", INT2FIX(CAP_LEASE));
}

/* Module methods belonging to Capabilities */

// return a Capabilities::Set object representing the current kernel
// capabilities of the running process
static VALUE caps_current (void) {

}

/* functions related to Capabilities::Set */

//allocate a new Capabilities::Set object
static VALUE capset_allocate (VALUE klass) {
  cap_t capset = cap_init();
  return Data_Wrap_Struct (klass, capset_mark, capset_free, capset);

}

//for the garbage collector...this object is too simple.
static void capset_mark (cap_t caps) {}

//how to destruct a Capabilities::Set object
static void capset_free (cap_t caps) {
    cap_free(caps);
}

static VALUE capset_to_string (VALUE self) {
  cap_t caps;
  char *text;

  Data_Get_Struct (self, cap_t, caps);
  text = cap_to_text(caps, NULL);
  return(rb_str_new2(text));
}


