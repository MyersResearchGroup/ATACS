LIBRARY ieee, work;
USE ieee.std_logic_1164.all;
use work.handshake.all;

ENTITY fa_a_kill IS
    PORT(
         d0 : INOUT std_logic := '0';
         d1 : INOUT std_logic := '0';
         en : INOUT std_logic := '1';
         s0 : INOUT std_logic := '0';
         s1 : INOUT std_logic := '0';
         a0 : IN std_logic;
         a1 : IN std_logic;
         b0 : IN std_logic;
         b1 : IN std_logic;
         c0 : IN std_logic;
         c1 : IN std_logic;
         de : IN std_logic;
         se : IN std_logic
    );
END fa_a_kill;

ARCHITECTURE behavior of fa_a_kill IS
BEGIN

  sum : process
  begin
    guard(se, '1');
    guard_or(a0, '1', a1, '1');
    guard_or(b0, '1', b1, '1');
    guard_or(c0, '1', c1, '1');
    -- evaluate                                             abc => s
    if    (a0 = '1' and b0 = '1' and c0 = '1') then --000 => 0
      assign(s0, '1', 200, 300);
    elsif (a0 = '1' and b0 = '1' and c1 = '1') then --001 => 1
      assign(s1, '1', 200, 300);
    elsif (a0 = '1' and b1 = '1' and c0 = '1') then --010 => 1
      assign(s1, '1', 200, 300);
    elsif (a0 = '1' and b1 = '1' and c1 = '1') then --011 => 0
      assign(s0, '1', 200, 300);
    elsif (a1 = '1' and b0 = '1' and c0 = '1') then --100 => 1
      assign(s1, '1', 200, 300);
    elsif (a1 = '1' and b0 = '1' and c1 = '1') then --101 => 0
      assign(s0, '1', 200, 300);
    elsif (a1 = '1' and b1 = '1' and c0 = '1') then --110 => 0
      assign(s0, '1', 200, 300);
    elsif (a1 = '1' and b1 = '1' and c1 = '1') then --111 => 1
      assign(s1, '1', 200, 300);
    end if;
    guard_or(d0, '1', d1, '1');  -- make sure carry has been set
    -- start reset cycle
    assign(en, '0', 200, 300);
    guard(se, '0');
    if (s0 = '1') then
      assign (s0, '0', 200, 300);
    elsif (s1 = '1') then
      assign (s1, '0', 200, 300);
    end if;
    guard_and(d0, '0', d1, '0');  -- make sure carry has reset
    guard_and(a0, '0', a1, '0');
    guard_and(b0, '0', b1, '0');
    guard_and(c0, '0', c1, '0');
    assign(en, '1', 200, 300);
  end process;

  carry : process
  begin
    guard(de, '1');
    guard_or(a0, '1', a1, '1');
    guard_or(b0, '1', b1, '1');
    if    (a0 = '1' and b0 = '1') then -- kill
      assign(d0,'1', 200, 300);
    elsif ( a1 = '1' and b1 = '1') then -- generate
      assign(d1,'1', 200, 300);
    else  -- must get c to know.
      guard_or(c0, '1', c1, '1');
      if    (a0 = '1' and b1 = '1' and c0 = '1') then  -- 010 => 0
        assign(d0, '1', 200, 300);
      elsif (a0 = '1' and b1 = '1' and c1 = '1') then  -- 011 => 1
        assign(d1, '1', 200, 300);
      elsif (a1 = '1' and b0 = '1' and c0 = '1') then  -- 100 => 0
        assign(d0, '1', 200, 300);
      elsif (a1 = '1' and b0 = '1' and c1 = '1') then  -- 101 => 1
        assign(d1, '1', 200, 300);
      end if;
    end if;
    guard(en,'0');
    guard(de,'0');
    if (d0 = '1') then
      assign (d0, '0', 200, 300);
    elsif (d1 = '1') then
      assign (d1, '0', 200, 300);
    end if;
    guard_and(a0, '0', a1, '0');
    guard_and(b0, '0', b1, '0');
    guard_and(c0, '0', c1, '0');
    guard(en, '1');
  end process;

END behavior;
