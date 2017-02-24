--\begin{comment}
----------------------
-- winery9.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity winery is
  port(wine_to_old_shop, wine_to_new_shop : inout channel :=
       init_channel(sender => timing(5, 10)));

end winery;

architecture behavior of winery is
  signal bottle1 : std_logic_vector( 2 downto 0 ) := "000";
  signal bottle2 : std_logic_vector( 2 downto 0 ) := "011";
begin
--\end{comment}
  winery : process
  begin
    send(wine_to_old_shop, bottle1,
         wine_to_new_shop, bottle2);
    --@synthesis_off
    bottle1 <= bottle1 + 1;
    bottle2 <= bottle2 + 1;
    --@synthesis_on
  end process winery;
end behavior;  --tex comment
