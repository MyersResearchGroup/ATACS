----------------------
-- route3.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity route3 is
  port(W, X, Y : inout channel := init_channel);
end route3;

architecture behavior of route3 is
  signal data : std_logic_vector( 2 downto 0 ) := "000";
begin
  route3 : process
    variable z : integer;
  begin
    z := selection(3);
    if (z = 1) then
      send(Y, data);
    elsif (z = 2) then
      send(X, data);
    else
      send(W, data);
    end if;
    --@synthesis_off
    data <= data + 1;
    --@synthesis_on
    wait for 5 ns;
  end process route3;
end behavior;
