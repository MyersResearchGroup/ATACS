--\begin{verbatim}
----------------------
-- wine_example.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all; 
use work.nondeterminism.all;
use work.channel.all;

entity wine_example is 		
end wine_example;

architecture behavior of wine_example is 
  type wine_type is (cabernet, merlot, zinfandel, chardonnay, 
                sauvignon_blanc, pinot_noir, riesling, bubbly);
  signal wine_drunk : wine_type;
  signal winery_to_shop : channel := init_channel;
  signal shop_to_patron : channel := init_channel;
  signal bottle : std_logic_vector( 2 downto 0 ) := "000";
  signal shelf : std_logic_vector( 2 downto 0 );
  signal brown_bag : std_logic_vector( 2 downto 0);
begin
winery : process
begin
     send(winery_to_shop,bottle);
--     bottle <= bottle + 1;
--     wait for 5 ns;
end process winery;
  
shop : process
begin
     receive(winery_to_shop,shelf);
     send(shop_to_patron,shelf);
end process shop;

patron : process
begin
     receive(shop_to_patron,brown_bag);
--     wine_drunk <= wine_type'val(conv_integer(brown_bag));
end process patron;
end behavior;
--\end{verbatim}
