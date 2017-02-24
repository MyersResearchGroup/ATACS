library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity produceA is 		
  port(x : inout channel:=active);
end produceA;

architecture behavior of produceA is 
  signal data : std_logic := '0';
begin
  produceA : process
  begin
    send(x,data);
  end process produceA;
end behavior;
