library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity LR is
  port(wine_delivery : inout channel := passive;
       wine_selling : inout channel := active );
end LR;

architecture behavior of LR is
  signal shelf : std_logic_vector( 2 downto 0 );
begin
  shop : process
  begin
    await(wine_delivery);
    send(wine_selling, shelf);
    receive(wine_delivery, shelf);
  end process shop;
end behavior;
