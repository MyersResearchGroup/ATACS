--\begin{verbatim}
----------------------
-- simple.vhd
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
  signal c : channel := init_channel;
  signal x : std_logic_vector( 2 downto 0 ) := "000";
  signal y : std_logic_vector( 2 downto 0 );
begin
  P : process
  begin
    send(c, x);
    --@synthesis_off
    x <= x + 1;
    --@synthesis_on
    wait for 5 ns;
  end process winery;

  Q : process
  begin
    receive(c, y);
  end process shop;
end behavior;
--\end{verbatim}
