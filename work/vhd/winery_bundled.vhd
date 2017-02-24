library ieee;  --tex comment 
use ieee.std_logic_1164.all;  --tex comment 
use ieee.std_logic_arith.all;  --tex comment 
use ieee.std_logic_unsigned.all;  --tex comment 
use work.nondeterminism.all;  --tex comment 
use work.handshake.all;  --tex comment 

entity winery_bundled is  --tex comment 
  port(req_wine : inout std_logic := '0';  --tex comment 
       ack_wine : in std_logic;  --tex comment 
       bottle : inout std_logic_vector(2 downto 0) := "000"  --tex comment 
       );  --tex comment 
end winery_bundled;  --tex comment 

architecture four_phase of winery_bundled is  --tex comment 
begin  --tex comment 
--\begin{verbatim}
  winery_bundled_4phase : process
  begin
    assign(req_wine, '1', 5, inf);  -- call winery
    guard(ack_wine, '1');  -- wine delivered
    assign(req_wine, '0', 5, 7);  -- reset req_wine
    guard(ack_wine, '0');  -- ack_wine resets
    --@synthesis_off
    bottle <= bottle + 1 after delay(1, 2);
    wait for 5 ns;
    --@synthesis_on
  end process;
--\end{verbatim}
end four_phase;  --tex comment 
