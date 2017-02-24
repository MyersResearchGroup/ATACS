---------------------------------------------------------------------------------------------------
--
-- Title       : cnt
-- Design      : datapath
-- Author      : yy     
-- date        : 6-8-2002,
---------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity cnt is
  port(
    Increment : inout channel := init_channel(receiver => timing(1, 3));
    DataOut : buffer std_logic_vector(15 downto 0) := (others => '0')
    );
end cnt;

architecture cnt of cnt is
begin
  cnt : process
  begin
    await(Increment);
    --@synthesis_off
    DataOut <= DataOut + 1;
    --@synthesis_on
    wait for delay (3, 5);
    receive(Increment);
  end process;
end cnt;





