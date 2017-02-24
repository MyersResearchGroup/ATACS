library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;

entity patron is
  port(wine_buying : inout channel := init_channel(receiver => timing(5, 10)));
end patron;

architecture behavior of patron is
  type wine_type is (cabernet, merlot, zinfandel, chardonnay,
                     sauvignon_blanc, pinot_noir, riesling,
                     bubbly);
  signal wine_drunk : wine_type;
  signal brown_bag : std_logic_vector( 2 downto 0 );
begin
  patron : process
  begin
    receive(wine_buying, brown_bag);
    --@synthesis_off
    wine_drunk <= wine_type'val(conv_integer(brown_bag));
    --@synthesis_on
  end process patron;
end behavior;
