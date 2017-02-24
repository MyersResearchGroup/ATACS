 library ieee;  --tex comment 
 use ieee.std_logic_1164.all;  --tex comment 
 use work.nondeterminism.all;  --tex comment 
 use work.handshake.all;  --tex comment 

 entity patron_dualrail is  --tex comment 
   port(ack_patron : out std_logic:='0';  --tex comment 
        shelf1 : in std_logic;  --tex comment 
        shelf0 : in std_logic);  --tex comment 
 end patron_dualrail;  --tex comment 

 architecture hse of patron_dualrail is  --tex comment 
 begin  --tex comment 
--\begin{verbatim}
patronP_dualrail:process
begin
     guard_or(shelf1,'1',shelf0,'1'); -- shop calls
     assign(ack_patron,'1',5,inf);                -- buys wine
     guard_and(shelf1,'0',shelf0,'0');
     assign(ack_patron,'0',5,7);             
end process;
--\end{verbatim}
 end hse;  --tex comment 

