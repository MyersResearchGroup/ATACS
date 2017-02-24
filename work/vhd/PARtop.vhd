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

  component PAR
    port(A,B,C : inout channel);
  end component;

  component patron 
    port(wine_buying : inout channel);
  end component;

  signal A,B,C : channel := init_channel;
begin
  THE_WINERY: winery port map(wine_shipping => A);
  THE_PAR   : PAR    port map(A=>A, B=>B, C=>C);
  THE_B     : patron port map(wine_buying => B);
  THE_C     : patron port map(wine_buying => C);
end structure;
