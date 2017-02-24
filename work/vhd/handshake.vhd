--\begin{packit}
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;

package handshake is

  procedure assign(signal sig:inout std_logic;constant val:std_logic;
                   constant l:integer;constant u:integer);

  procedure assign(signal sig1:inout std_logic;constant val1:std_logic;
                   constant l1:integer;constant u1:integer;
                   signal sig2:inout std_logic;constant val2:std_logic;
                   constant l2:integer;constant u2:integer);

  procedure assign(signal sig1:inout std_logic;constant val1:std_logic;
                   constant l1:integer;constant u1:integer;
                   signal sig2:inout std_logic;constant val2:std_logic;
                   constant l2:integer;constant u2:integer;
                   signal sig3:inout std_logic;constant val3:std_logic;
                   constant l3:integer;constant u3:integer);

  procedure vassign(signal sig:inout std_logic;constant val:std_logic;
                    constant l:integer;constant u:integer);

  procedure vassign(signal sig1:inout std_logic;constant val1:std_logic;
                    constant l1:integer;constant u1:integer;
                    signal sig2:inout std_logic;constant val2:std_logic;
                    constant l2:integer;constant u2:integer);

  procedure vassign(signal sig1:inout std_logic;constant val1:std_logic;
                    constant l1:integer;constant u1:integer;
                    signal sig2:inout std_logic;constant val2:std_logic;
                    constant l2:integer;constant u2:integer;
                    signal sig3:inout std_logic;constant val3:std_logic;
                    constant l3:integer;constant u3:integer);

  procedure guard(signal sig:in std_logic;constant val:std_logic);

  procedure guard_or(signal sig1:in std_logic;constant val1:std_logic;
                     signal sig2:in std_logic;constant val2:std_logic);

  procedure guard_or(signal sig1:in std_logic;constant val1:std_logic;
                     signal sig2:in std_logic;constant val2:std_logic;
                     signal sig3:in std_logic;constant val3:std_logic);

  procedure guard_and(signal sig1:in std_logic;constant val1:std_logic;
                      signal sig2:in std_logic;constant val2:std_logic);

  procedure guard_and(signal sig1:in std_logic;constant val1:std_logic;
                      signal sig2:in std_logic;constant val2:std_logic;
                      signal sig3:in std_logic;constant val3:std_logic);
end handshake;

package body handshake is

  procedure assign(signal sig:inout std_logic;constant val:std_logic;
                   constant l:integer;constant u:integer) is
  begin
    assert (sig /= val) report "Vacuous assignment" severity failure;
    sig <= val after delay(l,u);
    wait until sig = val;
  end assign;

  procedure assign(signal sig1:inout std_logic;constant val1:std_logic;
                   constant l1:integer;constant u1:integer;
                   signal sig2:inout std_logic;constant val2:std_logic;
                   constant l2:integer;constant u2:integer) is
  begin
    assert (sig1 /= val1 or sig2 /= val2)
      report "Vacuous assignment" severity failure;
    sig1 <= val1 after delay(l1,u1);
    sig2 <= val2 after delay(l2,u2);
    wait until sig1 = val1 and sig2 = val2;
  end assign;

  procedure assign(signal sig1:inout std_logic;constant val1:std_logic;
                   constant l1:integer;constant u1:integer;
                   signal sig2:inout std_logic;constant val2:std_logic;
                   constant l2:integer;constant u2:integer;
                   signal sig3:inout std_logic;constant val3:std_logic;
                   constant l3:integer;constant u3:integer) is
  begin
    assert (sig1 /= val1 or sig2 /= val2 or sig3 /= val3)
      report "Vacuous assignment" severity failure;
    sig1 <= val1 after delay(l1,u1);
    sig2 <= val2 after delay(l2,u2);
    sig3 <= val3 after delay(l3,u3);
    wait until sig1 = val1 and sig2 = val2 and sig3 = val3;
  end assign;

  procedure vassign(signal sig:inout std_logic;constant val:std_logic;
                    constant l:integer;constant u:integer) is
  begin
    if (sig /= val) then
      sig <= val after delay(l,u);
      wait until sig = val;
    end if;
  end vassign;

  procedure vassign(signal sig1:inout std_logic;constant val1:std_logic;
                    constant l1:integer;constant u1:integer;
                    signal sig2:inout std_logic;constant val2:std_logic;
                    constant l2:integer;constant u2:integer) is
  begin
    if (sig1 /= val1) then
      sig1 <= val1 after delay(l1,u1);
    end if;
    if (sig2 /= val2) then
      sig2 <= val2 after delay(l2,u2);
    end if;
    if (sig1 /= val1 or sig2 /= val2) then
      wait until sig1 = val1 and sig2 = val2;
    end if;
  end vassign;

  procedure vassign(signal sig1:inout std_logic;constant val1:std_logic;
                    constant l1:integer;constant u1:integer;
                    signal sig2:inout std_logic;constant val2:std_logic;
                    constant l2:integer;constant u2:integer;
                    signal sig3:inout std_logic;constant val3:std_logic;
                    constant l3:integer;constant u3:integer) is
  begin
    if (sig1 /= val1) then
      sig1 <= val1 after delay(l1,u1);
    end if;
    if (sig2 /= val2) then
      sig2 <= val2 after delay(l2,u2);
    end if;
    if (sig3 /= val3) then
      sig3 <= val3 after delay(l3,u3);
    end if;
    if (sig1 /= val1 or sig2 /= val2 or sig3 /= val3) then
      wait until sig1 = val1 and sig2 = val2 and sig3 = val3;
    end if;
  end vassign;

  procedure guard(signal sig:in std_logic;constant val:std_logic) is
  begin
    if (sig /= val) then
      wait until sig = val;
    end if;
  end guard;

  procedure guard_or(signal sig1:in std_logic;constant val1:std_logic;
                     signal sig2:in std_logic;constant val2:std_logic) is
  begin
    if (sig1 /= val1 and sig2 /= val2) then
      wait until sig1 = val1 or sig2 = val2;
    end if;
  end guard_or;

  procedure guard_or(signal sig1:in std_logic;constant val1:std_logic;
                     signal sig2:in std_logic;constant val2:std_logic;
                     signal sig3:in std_logic;constant val3:std_logic) is
  begin
    if (sig1 /= val1 and sig2 /= val2 and sig3 /= val3) then
      wait until sig1 = val1 or sig2 = val2 or sig3 = val3;
    end if;
  end guard_or;

  procedure guard_and(signal sig1:in std_logic;constant val1:std_logic;
                      signal sig2:in std_logic;constant val2:std_logic) is
  begin
    if ( sig1 /= val1 or sig2 /= val2 ) then
      wait until sig1 = val1 and sig2 = val2;
    end if;
  end guard_and;

  procedure guard_and(signal sig1:in std_logic;constant val1:std_logic;
                      signal sig2:in std_logic;constant val2:std_logic;
                      signal sig3:in std_logic;constant val3:std_logic) is
  begin
    if (sig1 /= val1 or sig2 /= val2 or sig3 /= val3) then
      wait until sig1 = val1 and sig2 = val2 and sig3 = val3;
    end if;
  end guard_and;

end handshake;
--\end{packit}

