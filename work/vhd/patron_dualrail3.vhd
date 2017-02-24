 library ieee;  --tex comment 
 use ieee.std_logic_1164.all;  --tex comment 
 use work.nondeterminism.all;  --tex comment 
 use work.handshake.all;  --tex comment 

 entity patron_dualrail3 is  --tex comment 
   port(ack_patron2 : out std_logic:='0';  --tex comment 
        shelf2_1 : in std_logic;  --tex comment 
        shelf2_0 : in std_logic;  --tex comment 
        ack_patron1 : out std_logic:='0';  --tex comment 
        shelf1_1 : in std_logic;  --tex comment 
        shelf1_0 : in std_logic;  --tex comment 
        ack_patron0 : out std_logic:='0';  --tex comment 
        shelf0_1 : in std_logic;  --tex comment 
        shelf0_0 : in std_logic);  --tex comment 
 end patron_dualrail3;  --tex comment 

 architecture hse of patron_dualrail3 is  --tex comment 
 begin  --tex comment 
--\begin{verbatim}
patronP_dualrail:process
begin
    guard_or(shelf2_1,'1',shelf2_0,'1');
    guard_or(shelf1_1,'1',shelf1_0,'1');
    guard_or(shelf0_1,'1',shelf0_0,'1');   
    assign(ack_patron2,'1',2,5,ack_patron1,'1',2,5,ack_patron0,'1',2,5);
    guard_and(shelf2_1,'0',shelf2_0,'0');
    guard_and(shelf1_1,'0',shelf1_0,'0');
    guard_and(shelf0_1,'0',shelf0_0,'0');   
    assign(ack_patron2,'0',2,5,ack_patron1,'0',2,5,ack_patron0,'0',2,5);
end process;
--\end{verbatim}
 end hse;  --tex comment 

