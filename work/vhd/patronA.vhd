library ieee; 
use ieee.std_logic_1164.all; 
use work.nondeterminism.all; 
use work.handshake.all; 

entity patronA is 
  port(req_patron : inout std_logic:='0'; 
       ack_patron : in std_logic);
end patronA; 

architecture four_phase of patronA is 
begin 
patronA_bundled_4phase:process
begin
     assign(req_patron,'1',1,5);  -- patron buys wine
     guard(ack_patron,'1');       -- shop calls 
     assign(req_patron,'0',1,5);  -- reset ack_patron
     guard(ack_patron,'0');       -- req_patron resets
end process;
end four_phase; 

