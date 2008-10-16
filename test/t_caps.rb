
$: << '../'

require 'test/unit'

require 'rcaps'

class TestCaps < Test::Unit::TestCase
  def test_new
    assert_nothing_raised { c = Caps.new }
    assert_nothing_raised { d = Caps.new('= ') }
    c = Caps.new
    d = Caps.new('= ')
    assert_kind_of(Caps, c)
    assert_kind_of(Caps, d)
    assert_equal('=', c.to_s)
    assert_equal('=', d.to_s)
  end

  def test_get_proc
    assert_nothing_raised { c = Caps.get_proc }
    c = Caps.get_proc
    assert_kind_of(Caps, c)
    #this assumption is based on not being root or having capabilities already
    #granted to our process...
    assert_equal('=', c.to_s)
  end

  def test_get_proc_with_pid
    assert_nothing_raised { c = Caps.get_proc($$) }
    assert_raises(TypeError) { c = Caps.get_proc('abc') }
  end

  def test_init_args
    assert_raises(TypeError) { c = Caps.new(1) }
    assert_raises(ArgumentError) { c = Caps.new('=', 1) }
  end

  def test_clear
    s = '= cap_setuid+e'
    c = Caps.new(s)
    assert_equal(s, c.to_s)
    assert_nothing_raised { c.clear }
    assert_equal('=', c.to_s)
  end

  def test_effective?
    c = Caps.new('= cap_setuid+e')
    assert(c.effective?(Caps::SETUID))
    assert(!c.effective?(Caps::CHOWN))
  end

  def test_permitted?
    c = Caps.new('= cap_setuid+p')
    assert(c.permitted?(Caps::SETUID))
    assert(!c.permitted?(Caps::CHOWN))
  end

  def test_inheritable?
    c = Caps.new('= cap_setuid+i')
    assert(c.inheritable?(Caps::SETUID))
    assert(!c.inheritable?(Caps::CHOWN))
  end

  def test_set_effective
    c = Caps.new
    c.set_effective([Caps::CHOWN, Caps::MKNOD])
    assert_equal('= cap_chown,cap_mknod+e', c.to_s)
  end

  def test_clear_effective
    c = Caps.new('= cap_chown,cap_mknod+e')
    c.clear_effective([Caps::CHOWN])
    assert_equal('= cap_mknod+e', c.to_s)
  end

  def test_set_permitted
    c = Caps.new
    c.set_permitted([Caps::SYS_BOOT, Caps::SYS_NICE])
    assert_equal('= cap_sys_boot,cap_sys_nice+p', c.to_s)
  end

  def test_clear_permitted
    c = Caps.new('= cap_chown,cap_mknod+p')
    c.clear_permitted([Caps::CHOWN])
    assert_equal('= cap_mknod+p', c.to_s)
  end

  def test_set_inheritable
    c = Caps.new
    c.set_inheritable([Caps::SYS_CHROOT, Caps::SYS_TIME])
    assert_equal('= cap_sys_chroot,cap_sys_time+i', c.to_s)
  end

  def test_clear_inheritable
    c = Caps.new('= cap_chown,cap_mknod+i')
    c.clear_inheritable([Caps::CHOWN])
    assert_equal('= cap_mknod+i', c.to_s)
  end

  def test_set_proc
    c = Caps.new('= cap_setuid+e')
    #without the ability to install capabilities (capset syscall), this will
    #fail.  we must either be root already of have somehow been granted this
    #privilege.  for lack of something similar to libfakeroot for capabilities,
    #we'll just ensure this fails for testing purposes.
    assert_raises(Errno::EPERM) { c.set_proc }
    c.clear
    assert_nothing_raised { c.set_proc }
  end

  def test_set_proc_with_pid
    c = Caps.new
    assert_raises(Errno::EPERM) { c.set_proc(1) }
    assert_nothing_raised { c.set_proc($$) }
    assert_raises(TypeError) { c.set_proc('abc') }
  end
end
