----------------------
-- wine_example4.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity wine_example4 is
end wine_example4;

----\begin{verbatim}
architecture new_structure of wine_example4 is
  component wine_shop
    port(wine_delivery, wine_selling : inout channel);
  end component;

  component winery
    port(wine_to_old_shop, wine_to_new_shop : inout channel);
  end component;

  component patron
    port(old_shop_to_patron, new_shop_to_patron : inout channel);
  end component;

  signal winery_to_shop1 : channel := init_channel;
  signal winery_to_shop2 : channel := init_channel;
  signal shop1_to_patron : channel := init_channel;
  signal shop2_to_patron : channel := init_channel;
begin
  THE_WINERY : winery port map(wine_to_old_shop => winery_to_shop1,
                               wine_to_new_shop => winery_to_shop2);
  THE_SHOP : wine_shop port map(wine_delivery => winery_to_shop1,
                                wine_selling => shop1_to_patron);
  NEW_SHOP : wine_shop port map(wine_delivery => winery_to_shop2,
                                wine_selling => shop2_to_patron);
  THE_PATRON : patron port map(old_shop_to_patron => shop1_to_patron,
                               new_shop_to_patron => shop2_to_patron);
end new_structure;
----\end{verbatim}
