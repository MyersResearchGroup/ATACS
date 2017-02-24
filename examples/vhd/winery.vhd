--\begin{verbatim}
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
  port(wine_shipping : inout channel := init_channel(sender => timing(1, 2)));
end winery;

architecture behavior of winery is
  signal bottle : std_logic_vector( 2 downto 0 ) := "000";
begin
  winery : process
  begin
    send(wine_shipping, bottle);
    --@synthesis_off
    bottle <= bottle + 1;
    --@synthesis_on
  end process winery;
end behavior;
--\end{verbatim}
