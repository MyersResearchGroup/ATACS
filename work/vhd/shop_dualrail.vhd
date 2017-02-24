 library ieee;  --tex comment 
 use ieee.std_logic_1164.all;  --tex comment 
 use work.nondeterminism.all;  --tex comment 
 use work.handshake.all;  --tex comment 

 entity shop_dualrail is  --tex comment 
   port(bottle1 : in std_logic;  --tex comment 
        bottle0 : in std_logic;  --tex comment 
        ack_wine : inout std_logic:='0';  --tex comment 
        ack_patron : in std_logic;  --tex comment 
        shelf1 : inout std_logic:='0';  --tex comment 
        shelf0 : inout std_logic:='0');  --tex comment 
 end shop_dualrail;  --tex comment 

 architecture hse of shop_dualrail is  --tex comment 
 begin  --tex comment 
--\begin{verbatim}
shopPA_dual_rail:process
begin
     guard(ack_patron,'0');    -- ack_patron resets
     guard_or(bottle0,'1',bottle1,'1');
     if bottle0 = '1' then
       assign(shelf0,'1',0,1);
     elsif bottle1 = '1' then
       assign(shelf1,'1',0,1);
     end if;
     assign(ack_wine,'1',0,1); -- shop receives wine
     guard(ack_patron,'1');    -- patron buys wine
     if shelf0 = '1' then
       assign(shelf0,'0',0,1);
     elsif shelf1 = '1' then
       assign(shelf1,'0',0,1);
     end if;
     guard_and(bottle0,'0',bottle1,'0');
     assign(ack_wine,'0',0,1); -- reset ack_wine
end process;
--\end{verbatim}
 end hse;  --tex comment 
