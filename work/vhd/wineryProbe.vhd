--\begin{comment}
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity winery is
  port(wine_shipping : inout channel := init_channel);
end winery;
--\end{comment}
architecture behavior of winery is
  signal bottle : std_logic_vector( 2 downto 0 ) := "000";
begin
  winery : process
  begin
    if probe(wine_shipping) then
      send(wine_shipping, bottle);
    end if;
    --@synthesis_off
    bottle <= bottle + 1;
    --@synthesis_on
    wait for 5 ns;
  end process winery;
end behavior;
