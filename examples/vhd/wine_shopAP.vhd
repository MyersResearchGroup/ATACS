library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity wine_shopAP is
  port(wine_delivery : inout channel := active(receiver => timing(1, 2));
       wine_selling : inout channel := passive(sender => timing(1, 2)));
end wine_shopAP;

architecture behavior of wine_shopAP is
  signal shelf : std_logic_vector( 2 downto 0 );
begin
  shop : process
  begin
    receive(wine_delivery, shelf);
    send(wine_selling, shelf);
  end process shop;
end behavior;
