LIBRARY ieee, work;
USE ieee.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

ENTITY buf_a_env IS
   PORT(
        le : IN std_logic;
        r0 : IN std_logic;
        r1 : IN std_logic;
        l0 : INOUT std_logic := '0';
        l1 : INOUT std_logic := '0';
        re : INOUT std_logic := '0'
    );
END buf_a_env;

ARCHITECTURE behavior of buf_a_env IS
  variable choose : integer;
BEGIN

  l : process
  begin
    choose := selection(2);
    case choose is
      when 1 => assign(l0, '1', 200, inf);
      when others =>  assign(l1, '1', 200, inf);
    end case;
    
    guard(le, '0');
    
    if (l0 = '1') then
      assign(l0, '0', 200, inf);
    else
      assign(l1, '0', 200, inf);
    end if;
    
    guard(le, '1');

  end process;

  r : process
  begin
    assign(re, '1', 200, inf);
    guard_or(r0, '1', r1, '1');
    assign(re, '0', 200, inf);
    guard_and(r0, '0', r1, '0');    
  end process;
  
END behavior;
