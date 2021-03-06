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

  component wine_shopAA
    port(wine_delivery, wine_selling : inout channel);
  end component;

  component patron
    port(wine_buying : inout channel);
  end component;

  signal L : channel := init_channel;
  signal R : channel := init_channel;
begin
  THE_WINERY : winery port map(wine_shipping => L );
  AA : wine_shopAA port map(wine_delivery => L, wine_selling => R);
  THE_PATRON : patron port map( wine_buying => R);
end structure;
