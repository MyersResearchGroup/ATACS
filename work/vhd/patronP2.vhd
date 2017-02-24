 library ieee;   --tex comment 
 use ieee.std_logic_1164.all;   --tex comment 
 use work.nondeterminism.all;   --tex comment 
 use work.handshake.all;   --tex comment 

 entity patronP2 is   --tex comment 
   port(req_patron1 : in std_logic;   --tex comment 
        ack_patron1 : inout std_logic:='0';   --tex comment 
--        shelf1 : in std_logic_vector(2 downto 0);   --tex comment 
        req_patron2 : in std_logic;   --tex comment 
        ack_patron2 : inout std_logic:='0'   --tex comment 
--        shelf2 : in std_logic_vector(2 downto 0)
         );   --tex comment 
 end patronP2;   --tex comment 

 architecture four_phase of patronP2 is   --tex comment 
--   signal brown_bag : std_logic_vector(2 downto 0);   --tex comment 
 begin   --tex comment 
--\begin{verbatim}
patronP:process
begin
     if (req_patron1 = '1') then
--       brown_bag <= shelf1 after delay(1,2);
--       wait for 5 ns;
       assign(ack_patron1,'1',1,5);  -- patron buys wine
       guard(req_patron1,'0');       -- req_patron resets
       assign(ack_patron1,'0',1,5);  -- reset ack_patron
     elsif (req_patron2 = '1') then
--       brown_bag <= shelf2 after delay(1,2);
--       wait for 5 ns;
       assign(ack_patron2,'1',1,5);  -- patron buys wine
       guard(req_patron2,'0');       -- req_patron resets
       assign(ack_patron2,'0',1,5);  -- reset ack_patron
     end if;
     wait for 2 ns;
end process;
--\end{verbatim}
 end four_phase;   --tex comment 

