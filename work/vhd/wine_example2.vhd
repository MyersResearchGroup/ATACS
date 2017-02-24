library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all; 
use work.nondeterminism.all;
use work.channel.all;

entity wine_example2 is 		
end wine_example2;

architecture structure of wine_example2 is 
  component winery
    port(wine_shipping : inout channel);
  end component;

  component wine_shop
    port(wine_delivery : inout channel;
         wine_selling : inout channel);
  end component;

  component patron 
    port(wine_buying : inout channel);
  end component;

  signal winery_to_shop : channel := init_channel;
  signal shop_to_patron : channel := init_channel;
begin
  THE_WINERY : winery port map(wine_shipping => winery_to_shop);
  THE_SHOP : wine_shop port map(wine_delivery => winery_to_shop,
                                wine_selling => shop_to_patron);
  THE_PATRON : patron port map(wine_buying => shop_to_patron);
end structure;
