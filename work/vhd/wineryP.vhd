library ieee; 
use ieee.std_logic_1164.all; 
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all; 
use work.handshake.all; 

entity wineryP is 
  port(req_wine : in std_logic; 
       ack_wine : inout std_logic := '0'); 
end wineryP; 

architecture four_phase of wineryP is 
begin 
winery_bundled_4phase:process
begin
     guard(req_wine,'1');        -- wine delivered
     assign(ack_wine,'1',1,5);   -- call winery
     guard(req_wine,'0');        -- ack_wine resets
     assign(ack_wine,'0',1,5);   -- reset req_wine
end process;
end four_phase; 

