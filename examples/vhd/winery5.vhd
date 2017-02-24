--\begin{comment}
----------------------
-- winery5.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity winery is
  port(wine_to_old_shop, wine_to_new_shop : inout channel :=
       init_channel(sender => timing(4, 5)));
end winery;

architecture behavior of winery is
  signal bottle : std_logic_vector( 2 downto 0 ) := "000";
begin
--\end{comment}
  winery : process
    variable z : integer;
  begin
    z := selection(2);
    case z is
      when 1 =>
        send(wine_to_new_shop, bottle);
      when others =>
        send(wine_to_old_shop, bottle);
    end case;
    --@synthesis_off
    bottle <= bottle + 1;
    --@synthesis_on
  end process winery;
end behavior;  --tex comment
