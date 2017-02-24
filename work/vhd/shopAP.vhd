library ieee;   
use ieee.std_logic_1164.all;   
use work.nondeterminism.all;  
use work.handshake.all;   

entity shopAP is   
  port(req_wine : inout std_logic:='0';   
       ack_wine : in std_logic;   
       req_patron : in std_logic;   
       ack_patron : inout std_logic:='0');   
end shopAP;   

architecture four_phase of shopAP is 
signal x : std_logic:='0';
begin  
shop_bundled_4phase:process
begin
     guard(ack_wine,'0');        -- req_wine resets
     assign(req_wine,'1',1,2);   -- shop receives wine
     assign(x,'1',1,2);
     guard(ack_wine,'1');        -- winery calls
     assign(req_wine,'0',1,2);   -- reset ack_wine
     guard(req_patron,'1');      -- patron buys wine
     assign(ack_patron,'1',1,2); -- call patron
     assign(x,'0',1,2);
     guard(req_patron,'0');      -- ack_patron resets
     assign(ack_patron,'0',1,2); -- reset req_patron
end process;
end four_phase;  

