library ieee;
use ieee.std_logic_1164.all; 
use work.nondeterminism.all; 
use work.handshake.all;  

entity adder is 
end adder;

architecture behv of adder is
  signal a,b,c : std_logic := '0'; --@ in
  signal s,d : std_logic := '0';
begin 
process
begin
  guard(d, not ((a and b) or (a and c) or (b and c)));
  vassign(d,(a and b) or (a and c) or (b and c),1,2); 
end process;
process
begin
  guard(s, not ((a and not b and not c) or (not a and b and not c) or
            (not a and not b and c) or (a and b and c)));
  vassign(s,(a and not b and not c) or (not a and b and not c) or
            (not a and not b and c) or (a and b and c),1,2); 
end process;

process
  variable z : integer;
begin
  z:=selection(2);
  case z is
  when 1 =>
    vassign(a,'1',5,10);
  when others =>
    vassign(a,'0',5,10);
  end case;
end process;

process
  variable z : integer;
begin
  z:=selection(2);
  case z is
  when 1 =>
    vassign(b,'1',5,10);
  when others =>
    vassign(b,'0',5,10);
  end case;
end process;

process
  variable z : integer;
begin
  z:=selection(2);
  case z is
  when 1 =>
    vassign(c,'1',5,10);
  when others =>
    vassign(c,'0',5,10);
  end case;
end process;
end behv; 

