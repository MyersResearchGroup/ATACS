 library ieee;   --tex comment 
 use ieee.std_logic_1164.all;   --tex comment 
 use ieee.std_logic_arith.all;  --tex comment 
 use ieee.std_logic_unsigned.all;  --tex comment 
 use work.nondeterminism.all;   --tex comment 
 use work.handshake.all;   --tex comment 

 entity wineryA2 is   --tex comment 
   port(req_wine1 : inout std_logic := '0';   --tex comment 
        ack_wine1 : in std_logic;   --tex comment 
--        bottle1 : inout std_logic_vector(2 downto 0) := "000";  --tex comment 
        req_wine2 : inout std_logic := '0';   --tex comment 
        ack_wine2 : in std_logic   --tex comment 
--        bottle2 : inout std_logic_vector(2 downto 0) := "000"
         );   --tex comment
 end wineryA2;   --tex comment 

 architecture four_phase of wineryA2 is   --tex comment 
   signal bottle : std_logic_vector(2 downto 0) := "000";  --tex comment 
 begin   --tex comment 
--\begin{verbatim}
winery:process
variable z : integer;
begin
     z := selection(2);
     case z is
     when 1 => 
       --bottle1 <= bottle after delay(1,2);
       --wait for 5 ns;
       assign(req_wine1,'1',1,5);   -- call winery
       guard(ack_wine1,'1');        -- wine delivered
       assign(req_wine1,'0',1,5);   -- reset req_wine
       guard(ack_wine1,'0');        -- ack_wine resets
     when others =>
       --bottle2 <= bottle after delay(1,2);
       --wait for 5 ns;
       assign(req_wine2,'1',1,5);   -- call winery
       guard(ack_wine2,'1');        -- wine delivered
       assign(req_wine2,'0',1,5);   -- reset req_wine
       guard(ack_wine2,'0');        -- ack_wine resets
     end case;
     --bottle <= bottle + 1 after delay(1,2); 
     --wait for 5 ns;
end process;
--\end{verbatim}
 end four_phase;   --tex comment 

