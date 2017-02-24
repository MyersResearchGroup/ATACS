---------------------------------------------------------------------------------------------------
--
-- Title       : mem
-- Design      : datapath
-- Author      : YY    
-- date        : 6-8-2002
---------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity mem is
  port(
    Mem_Access : inout channel := init_channel(receiver => timing(1, 3));
    addr_r : in std_logic_vector(15 downto 0);
    addr_w : in std_logic_vector(15 downto 0);
    Din : in std_logic_vector(7 downto 0);
    RW : in std_logic;
    Dout : out std_logic_vector(7 downto 0)
    );
end mem;

architecture mem of mem is
  constant ttMemorySize : integer := 16*4*4*256;
  type ttype is array (0 to ttMemorySize - 1)of std_logic_vector(7 downto 0);
  signal tt : ttype;
begin
  memory : process
  begin
    await(Mem_Access);
    --read delay is less that write 
    --@synthesis_off
    if (RW = '0') then
      tt(conv_integer(addr_w)) <= Din;
      wait for delay(3, 5);
    else
      Dout <= tt(conv_integer(addr_r));
      wait for delay(1, 3);
    end if;
    --@synthesis_on
    receive(Mem_Access);
  end process;
end mem;
