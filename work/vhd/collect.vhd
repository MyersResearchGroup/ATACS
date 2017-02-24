----------------------
-- collect.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use work.channel.all;

entity collect is
    port(Z : inout channel:=init_channel);
end collect;

architecture behavior of collect is 
    signal data : std_logic_vector( 2 downto 0 );
begin
    collect : process
    begin
        receive(Z,data);
    end process collect;
end behavior;
