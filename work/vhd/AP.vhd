--\begin{verbatim}
----------------------
-- wine_shop.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all; 
use work.nondeterminism.all;
use work.channel.all;

entity wine_shop is
  port(wine_delivery : inout channel:=init_channel;
       wine_selling : inout channel:=init_channel );
end wine_shop;

architecture behavior of wine_shop is 
  signal shelf : std_logic_vector( 2 downto 0 );
begin
shop : process
begin
     send(wine_selling,shelf);
     receive(wine_delivery,shelf);
end process shop;
end behavior;
--\end{verbatim}
