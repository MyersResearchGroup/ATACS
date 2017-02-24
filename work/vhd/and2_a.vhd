LIBRARY ieee, work;
USE ieee.std_logic_1164.all;
use work.handshake.all;

ENTITY and2_a IS
     PORT(
          c0 : INOUT std_logic := '0';
          c1 : INOUT std_logic := '0';
          en : INOUT std_logic := '1';
          a0 : IN std_logic;
          a1 : IN std_logic;
          b0 : IN std_logic;
          b1 : IN std_logic;
          ce : IN std_logic
          );
END and2_a;

ARCHITECTURE behavior of and2_a IS
BEGIN

  and2 : process
  begin
    guard(ce, '1');
    guard_or(a0, '1', a1, '1');
    guard_or(b0, '1', b1, '1');
    -- evaluate                                             ab => c
    if    (a0 = '1' and b0 = '1') then                --00 => 0
      assign(c0, '1', 200, 300);
    elsif (a0 = '1' and b1 = '1') then                --01 => 0
      assign(c0, '1', 200, 300);
    elsif (a1 = '1' and b0 = '1') then                --10 => 0
      assign(c0, '1', 200, 300);
--    elsif (a1 = '1' and b1 = '1') then                --11 => 1
    else
      assign(c1, '1', 200, 300);
    end if;
    -- start reset cycle
    assign(en, '0', 200, 300);
    guard(ce, '0');
    if (c0 = '1') then
      assign (c0, '0', 200, 300);
    elsif (c1 = '1') then
      assign (c1, '0', 200, 300);
    end if;
    guard_and(a0, '0', a1, '0');
    guard_and(b0, '0', b1, '0');
    assign(en, '1', 200, 300);
  end process;

END behavior;
