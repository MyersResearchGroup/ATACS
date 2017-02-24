library ieee;
use ieee.std_logic_1164.all;
use work.channel.all;

entity right is
  port(y : inout channel := init_channel(timing(1,2)));
end right;

architecture behavior of right is
  signal data : std_logic := '0';
begin
  right : process
  begin
    --if(probe(y)) then
    receive(y, data);
    --end if;
  end process right;
end behavior;
