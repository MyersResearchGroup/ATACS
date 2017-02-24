----------------------  
-- winery.vhd  
----------------------  
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity winery is
  port(wine_to_old_shop : inout channel := init_channel;
       wine_to_new_shop : inout channel := init_channel);
end winery;

architecture behavior of winery is
  signal bottle : std_logic_vector( 2 downto 0 ) := "000";
begin
--\begin{verbatim}
  winery : process
  begin
    if (probe(wine_to_new_shop)) then
      send(wine_to_new_shop, bottle);
      --@synthesis_off
      bottle <= bottle + 1 after delay(1, 2);
      --@synthesis_on
      wait for 5 ns;
    elsif (probe(wine_to_old_shop)) then
      send(wine_to_old_shop, bottle);
      --@synthesis_off
      bottle <= bottle + 1 after delay(1, 2);
      --@synthesis_on
      wait for 5 ns;
    else
      wait for 5 ns;
    end if;
  end process winery;
--\end{verbatim}
end behavior;
