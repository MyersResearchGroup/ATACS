---------------------- --tex comment 
-- wine_example3.vhd --tex comment 
---------------------- --tex comment 
library ieee; --tex comment 
use ieee.std_logic_1164.all; --tex comment 
use ieee.std_logic_arith.all; --tex comment 
use ieee.std_logic_unsigned.all;  --tex comment 
use work.nondeterminism.all; --tex comment 
use work.channel.all; --tex comment 
 --tex comment 
entity wine_example3 is 		 --tex comment 
end wine_example3; --tex comment 
 --tex comment 
--\begin{verbatim}
architecture new_structure of wine_example3 is 
  component wine_shop
    port(wine_delivery : inout channel;
         wine_selling : inout channel);
  end component;

  component winery
    port(wine_shipping : inout channel);
  end component;

  component patron 
    port(wine_buying : inout channel);
  end component;

  signal winery_to_shop : channel := init_channel;
  signal shop1_to_shop2 : channel := init_channel;
  signal shop_to_patron : channel := init_channel;
begin
  THE_WINERY : winery port map(wine_shipping => winery_to_shop);
  OLD_SHOP : wine_shop port map(wine_delivery => winery_to_shop,
                                wine_selling => shop1_to_shop2);
  NEW_SHOP : wine_shop port map(wine_delivery => shop1_to_shop2,
                                wine_selling => shop_to_patron);
  THE_PATRON : patron port map(wine_buying => shop_to_patron);
end new_structure;
--\end{verbatim}
