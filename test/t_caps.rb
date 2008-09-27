
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
  end

  def test_get_proc
    assert_nothing_raised { c = Caps.get_proc }
    c = Caps.get_proc
    assert_kind_of(Caps, c)
  end
end
