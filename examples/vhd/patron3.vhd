library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity patron is
  port(old_shop_to_patron : inout channel :=
       init_channel(receiver => timing(1, 2));
       new_shop_to_patron : inout channel :=
       init_channel(receiver => timing(3, 4)));
end patron;

architecture behavior of patron is
  type wine_type is (cabernet, merlot, zinfandel, chardonnay,
                     sauvignon_blanc, pinot_noir, riesling,
                     bubbly);
  signal wine_drunk1 : wine_type;
  signal wine_drunk2 : wine_type;
  signal brown_bag1 : std_logic_vector( 2 downto 0 );
  signal brown_bag2 : std_logic_vector( 2 downto 0 );
begin
  patron : process
  begin
    receive(old_shop_to_patron, brown_bag1,
            new_shop_to_patron, brown_bag2);
    --@synthesis_off
    wine_drunk1 <= wine_type'val(conv_integer(brown_bag1));
    wine_drunk2 <= wine_type'val(conv_integer(brown_bag2));
    --@synthesis_on
  end process patron;
end behavior;
