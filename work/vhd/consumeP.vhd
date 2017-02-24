library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity consumeP is 		
  port(x : inout channel:=passive);
end consumeP;

architecture behavior of consumeP is 
  signal data : std_logic := '0';
begin
  consumeP : process
  begin
    receive(x,data);
  end process consumeP;
end behavior;
