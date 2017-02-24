---------------------------------------------------------------------------------------------------
--
-- Title       : dpCal
-- Design      : datapath
-- Author      : yy    
-- date        : 6-8-2002
---------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity dpCal is
  generic(
    fwidth : integer := 15
    );
  port(
    reset : in std_logic;
    Calculate : inout channel := init_channel(receiver => timing(1, 3));
    dp : out std_logic_vector(3 downto 0)
    );
end dpCal;

architecture dpCal of dpCal is
  type NumType is array (0 to 15) of integer range 0 to 15;
  constant dpi : NumType := (0,8,12,4,2,10,14,6,3,11,15,7,1,9,13,5);
begin
  dpCalculate : process
    variable cnt : integer := 0;
    variable index : integer;
    variable w : integer := fwidth*16;
  begin
    await(Calculate);
    --@synthesis_off
    index := ((cnt/w/2) mod 2)*8 +cnt mod 8;
    dp <= conv_std_logic_vector(dpi(index), 4);
    cnt := cnt+1;
    --@synthesis_on
    wait for delay (3, 5);
    receive(Calculate);
  end process;
end dpCal;

