library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity winery is
  port(wine_to_old_shop, wine_to_new_shop : inout channel := passive);
end winery;

architecture behavior of winery is
  type wine_type is (cabernet, merlot, zinfandel, chardonnay,
                     sauvignon_blanc, pinot_noir, riesling,
                     bubbly);
  signal bottle : std_logic_vector( 2 downto 0 ) := "000";
begin
  winery : process
    variable z : integer;
  begin
    z := selection(2);
    if ( z = 1 ) then
      send(wine_to_new_shop, bottle);
    else
      send(wine_to_old_shop, bottle);
    end if;
    --@synthesis_off
    bottle <= bottle + 1;
    --@synthesis_on
    wait for 5 ns;
  end process winery;
end behavior;
