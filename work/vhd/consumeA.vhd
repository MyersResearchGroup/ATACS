library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity consumeA is 		
  port(x : inout channel:=active);
end consumeA;

architecture behavior of consumeA is 
  signal data : std_logic := '0';
begin
  consumeA : process
  begin
    receive(x,data);
  end process consumeA;
end behavior;
