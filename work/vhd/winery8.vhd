---------------------- --tex comment 
-- winery.vhd --tex comment 
---------------------- --tex comment 
library ieee;  --tex comment 
use ieee.std_logic_1164.all;  --tex comment 
use ieee.std_logic_arith.all;  --tex comment 
use ieee.std_logic_unsigned.all;  --tex comment 
use work.nondeterminism.all;  --tex comment 
use work.channel.all;  --tex comment 
--tex comment 
entity winery is  --tex comment 
  port(wine_to_old_shop : inout channel := init_channel;  --tex comment 
       wine_to_new_shop : inout channel := init_channel);  --tex comment 
end winery;  --tex comment 
--tex comment 
architecture behavior of winery is  --tex comment 
  signal bottle : std_logic_vector( 2 downto 0 ) := "000";  --tex comment 
begin  --tex comment 
--\begin{verbatim}
  winery : process
    variable one : integer := 1;
  begin
    send(wine_to_old_shop, bottle);
    --@synthesis_off
    bottle <= bottle + 1;
    --@synthesis_on
    wait for 5 ns;
    while ( one = one ) loop
      send(wine_to_new_shop, bottle);
      --@synthesis_off
      bottle <= bottle + 1;
      --@synthesis_on
      wait for 5 ns;
    end loop;
  end process winery;
--\end{verbatim}
end behavior;  --tex comment 
