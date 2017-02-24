 library ieee;    --tex comment 
 use ieee.std_logic_1164.all;    --tex comment 
 use work.nondeterminism.all;   --tex comment 
 use work.handshake.all;    --tex comment 

 entity shopPA_2phase is    --tex comment 
   port(req_wine : in std_logic;    --tex comment 
        ack_wine : inout std_logic:='0';    --tex comment 
        --bottle : in std_logic_vector(2 downto 0);   --tex comment 
        req_patron : inout std_logic:='0';    --tex comment 
        ack_patron : in std_logic    --tex comment 
        --shelf : inout std_logic_vector(2 downto 0)
         );   --tex comment 
 end shopPA_2phase;    --tex comment 

 architecture two_phase of shopPA_2phase is   --tex comment 
 begin   --tex comment 
--\begin{verbatim}
shop_bundled_2phase:process
begin
     guard(req_wine,not ack_wine);        -- winery calls
     --shelf <= bottle after delay(1,2);
     --wait for 5 ns;
     --ack_wine <= '1' after delay(1,2);   -- shop receives wine
     --wait until ack_wine = '1';
     vassign(ack_wine,not ack_wine,1,2);
     assign(req_patron,not req_patron,1,2); -- call patron
     guard(ack_patron,req_patron);      -- patron buys wine
end process;
--\end{verbatim}
 end two_phase;   --tex comment 

