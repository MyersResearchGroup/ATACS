--\begin{comment}
----------------------
-- patron2.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity patron is
  port(old_shop_to_patron, new_shop_to_patron : inout channel :=
       init_channel(receiver => timing(1, 3)));
end patron;

architecture behavior of patron is
  type wine_type is(cabernet, merlot, zinfandel, chardonnay,
                    sauvignon_blanc, pinot_noir, riesling,
                    bubbly);
  signal wine_drunk : wine_type;
  signal brown_bag : std_logic_vector( 2 downto 0 );
begin
--\end{comment}
  patron : process
  begin
    await_any(old_shop_to_patron, new_shop_to_patron);
    if (probe(old_shop_to_patron)) then
      receive(old_shop_to_patron, brown_bag);
      --@synthesis_off
      wine_drunk <= wine_type'val(conv_integer(brown_bag));
      --@synthesis_on
    else
      receive(new_shop_to_patron, brown_bag);
      --@synthesis_off
      wine_drunk <= wine_type'val(conv_integer(brown_bag));
      --@synthesis_on
    end if;
  end process patron;
end behavior;  --tex comment
