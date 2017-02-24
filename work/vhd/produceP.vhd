library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity produceP is 		
  port(x : inout channel:=passive);
end produceP;

architecture behavior of produceP is 
  signal data : std_logic := '0';
begin
  produceP : process
  begin
    send(x,data);
  end process produceP;
end behavior;
