----------------------  
-- patron.vhd  
----------------------  
library ieee;  
use ieee.std_logic_1164.all;  
use ieee.std_logic_arith.all;  
use ieee.std_logic_unsigned.all;   
use work.nondeterminism.all;  
use work.channel.all;  
  
entity patron is 		  
  port(old_shop_to_patron : inout channel:=init_channel;  
       new_shop_to_patron : inout channel:=init_channel);	   
end patron;  
  
architecture behavior of patron is   
  type wine_type is (cabernet, merlot, zinfandel, chardonnay,   
                     sauvignon_blanc, pinot_noir, riesling,   
                     bubbly);  
  signal wine_drunk : wine_type;  
  signal brown_bag : std_logic_vector( 2 downto 0 ) := "000";  
begin  
patron : process
variable z : integer;
begin
  z := selection(2);
  if (z = 1) then
    receive(old_shop_to_patron,brown_bag);
    wine_drunk <= wine_type'val(conv_integer(brown_bag));
  else
    receive(new_shop_to_patron,brown_bag);
    wine_drunk <= wine_type'val(conv_integer(brown_bag));
  end if;
  wait for 2 ns;
end process patron;
end behavior;  
