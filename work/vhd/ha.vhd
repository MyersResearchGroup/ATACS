library ieee;
use ieee.std_logic_1164.all; 
use work.nondeterminism.all; 
use work.handshake.all;  

entity adder is 
end adder;

architecture behv of adder is
  signal a,b : std_logic := '0'; --@ in
  signal s : std_logic := '0';
begin 
process
begin
  guard(s, ((a and b) or (not a and not b)));
  assign(s,(a and not b) or (not a and b),1,2);
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
end behv; 

