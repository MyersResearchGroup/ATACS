---------------------------------------------------------------------------------------------------
--
-- Title       : ctrl
-- Design      : datapath
-- Author      : yy
-- date        : 6-8-2002
---------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use std.textio.all;
use work.nondeterminism.all;
use work.channel.all;
use work.handshake.all;

entity ctrl is
  port(
    DPdata : inout channel := init_channel(receiver => timing(1, 2));
    cr, cb, lum : inout channel := init_channel(receiver => timing(1, 2));
    Increment : inout channel := init_channel(sender => timing(1, 3));
    Mem_Access : inout channel := init_channel(sender => timing(3, 5));
    Calculate : inout channel := init_channel(sender => timing(3, 5));
    Splice : inout channel := init_channel(sender => timing(3, 5));

    cr_d : buffer std_logic_vector(7 downto 0);
    cb_d : buffer std_logic_vector(7 downto 0);
    lum_d : buffer std_logic_vector(7 downto 0);
    DPdata_d : buffer std_logic_vector(7 downto 0);

    reset : out std_logic := '0';
    mem_RW : buffer std_logic := '0'
    );
end ctrl;

architecture ctrl of ctrl is
begin
  Decoder : process
  begin
    await_any (DPdata, cb, cr, lum);
    if (probe (DPdata))then
      vassign(mem_RW, '0', 1, 2);
      receive(DPdata, DPdata_d);
      send(Mem_Access);
      send(Increment);
    elsif (probe (cb)) then
      receive(cb, cb_d);
    elsif (probe (cr)) then
      receive(cr, cr_d);
    elsif (probe(lum))then
      vassign(mem_RW, '1', 1, 3);
      receive(lum, lum_d);
      send(Calculate);
      send(Splice);
      send(Mem_Access);
    end if;
    wait for delay(3, 5);
  end process;

end ctrl;
