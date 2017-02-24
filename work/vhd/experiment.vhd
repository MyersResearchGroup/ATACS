--\begin{verbatim}
----------------------
-- experiment.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity experiment is
end experiment;

architecture behavior of experiment is
  type wine_type is (cabernet, merlot, zinfandel, chardonnay,
                     sauvignon_blanc, pinot_noir, riesling, bubbly);
  signal winery_to_shop1 : channel := init_channel;
  signal winery_to_shop2 : channel := init_channel;
  signal shop1_to_patron : channel := init_channel;
  signal shop2_to_patron : channel := init_channel;
  signal wine_drunk : wine_type;
  signal bottle : std_logic_vector( 2 downto 0 ) := "000";
  signal shelf : std_logic_vector( 2 downto 0 );
  signal brown_bag : std_logic_vector( 2 downto 0);
begin
  winery : process
    variable z : integer;
  begin
    z := selection(2);
    if (z = 1) then
      send(winery_to_shop2, bottle);
    else
      send(winery_to_shop1, bottle);
    end if;
    --@synthesis_off
    bottle <= bottle + 1;
    --@synthesis_on
    wait for 5 ns;
  end process winery;

  shop : process
  begin
    receive(winery_to_shop1, shelf);
    send(shop1_to_patron, shelf);
  end process shop;

  newShop : process
  begin
    receive(winery_to_shop2, shelf);
    send(shop2_to_patron, shelf);
  end process newShop;

  patron : process
  begin
    if (probe(shop1_to_patron)) then
      receive(shop1_to_patron, brown_bag);
      --@synthesis_off
      wine_drunk <= wine_type'val(conv_integer(brown_bag));
      --@synthesis_on
    elsif (probe(shop2_to_patron)) then
      receive(shop2_to_patron, brown_bag);
      --@synthesis_off
      wine_drunk <= wine_type'val(conv_integer(brown_bag));
      --@synthesis_on
    end if;
    --@synthesis_off
    wait for 2 ns;
    --@synthesis_on
  end process patron;
end behavior;
--\end{verbatim}
