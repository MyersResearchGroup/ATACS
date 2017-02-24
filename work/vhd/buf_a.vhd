LIBRARY ieee, work;
USE ieee.std_logic_1164.all;
use work.handshake.all;

ENTITY buf_a IS
   PORT(
        le : INOUT std_logic := '1';
        r0 : INOUT std_logic := '0';
        r1 : INOUT std_logic := '0';
        l0 : IN std_logic;
        l1 : IN std_logic;
        re : IN std_logic
    );
END buf_a;

ARCHITECTURE behavior of buf_a IS
BEGIN

  buf : process
  begin
    guard(re, '1');
    guard_or(l0, '1', l1, '1');

    if    (l0 = '1') then
      assign(r0, '1', 200, 300);
    else 
      assign(r1, '1', 200, 300);
    end if;

    assign(le, '0', 200, 300);
    guard(re, '0');

    if (r0 = '1') then
      assign (r0, '0', 200, 300);
    elsif (r1 = '1') then
      assign (r1, '0', 200, 300);
    end if;

    guard_and(l0, '0', l1, '0');    

    assign(le, '1', 200, 300);

  end process;
  
END behavior;



