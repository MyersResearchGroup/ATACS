--\begin{comment}
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity LSDOenv is
  port(L3, L2, L1, L0:  inout channel := init_channel(sender=>timing(3,5));
       Shift, Done: inout channel := init_channel(sender=>timing(3,5));
       O3, O2, O1, O0 : inout channel := init_channel(receiver=>timing(3,5)));
end LSDOenv;
--\end{comment}
architecture behavior of LSDOenv is
  signal Ldata, Odata : std_logic_vector( 3 downto 0 ) := "0000";
  signal Sdata : std_logic := '0';
begin
  LSDOenv : process
    variable z : integer;
  begin
    --@synthesis_off
    Ldata <= Ldata + 1;
    --@synthesis_on
    wait for delay(1,2);
    send(L3, Ldata(3), L2, Ldata(2), L1, Ldata(1), L0, Ldata(0));
    send(Done);
    receive(O3, Odata(3), O2, Odata(2), O1, Odata(1), O0, Odata(0));
    assert Ldata = Odata report "unequal data!" severity failure;
    send(Shift, Sdata);
    send(Done);
    receive(O3, Odata(3), O2, Odata(2), O1, Odata(1), O0, Odata(0));
    assert to_bitvector(Odata) = to_bitvector(Ldata) srl 1
      report "unequal data!" severity failure;
  end process LSDOenv;
end behavior;

-------------------------------------------------------------------------
--\begin{comment}
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity normal is
  port(Load, Shift_in, Done_In : inout channel :=
       init_channel(receiver => timing(2,4));
       Shift_Out, Done_Out, Data_out : inout channel :=
       init_channel(sender => timing(3,4)));
end normal;
--\end{comment}
architecture behavior of normal is
  signal u : std_logic;
begin
  normal : process
  begin
    await_any(Load, Shift_in, Done_in);
    if (probe(Load)) then
      receive(Load, u);
    elsif (probe(Shift_in)) then
      send(Shift_out, u);
      receive(Shift_in, u);
    else
      send(Done_out);
      receive(Done_in);
      send(Data_out, u);
    end if;
  end process normal;
end behavior;

-------------------------------------------------------------------------
--\begin{comment}
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.channel.all;

entity special is
  port(Load, Shift_in, Done_In: inout channel :=
       init_channel(receiver=>timing(2,4));
       Data_out : inout channel := init_channel(sender=>timing(1,6)));
end special;
--\end{comment}
architecture behavior of special is
  signal u : std_logic;
begin
  special : process
  begin
    await_any(Load, Shift_in, Done_in);
    if (probe(Load)) then
      receive(Load, u);
    elsif (probe(Shift_in)) then
      receive(Shift_in, u);
    else
      receive(Done_in);
      send(Data_out, u);
    end if;
  end process special;
end behavior;
--\begin{comment}
-------------------------------------------------------------------------

use work.channel.all;

entity shifter is
end shifter;

architecture new_structure of shifter is
  component LSDOenv
    port(L3, L2, L1, L0, Shift, Done, O3, O2, O1, O0 : inout channel);
  end component;
  component normal
    port(Load, Shift_in, Shift_out, Done_in, Done_out, Data_out : inout channel);
  end component;
  component special
    port(Load, Shift_in, Done_in, Data_out : inout channel);
  end component;
  signal L3, L2, L1, L0 : channel := init_channel;
  signal S3, S2, S1, S0 : channel := init_channel;
  signal D3, D2, D1, D0 : channel := init_channel;
  signal O3, O2, O1, O0 : channel := init_channel;
begin
  LSDO : LSDOenv port map(L3 => L3, L2 => L2, L1 => L1, L0 => L0,
                          Shift => S3,
                          Done => D3,
                          O3 => O3, O2 => O2, O1 => O1, O0 => O0);
  b3 : normal port map (
    Load => L3,
    Shift_in => S3,
    Shift_out => S2,
    Done_in => D3,
    Done_out => D2,
    Data_out => O3);
  b2 : normal port map (
    Load => L2,
    Shift_in => S2,
    Shift_out => S1,
    Done_in => D2,
    Done_out => D1,
    Data_out => O2);
  b1 : normal port map (
    Load => L1,
    Shift_in => S1,
    Shift_out => S0,
    Done_in => D1,
    Done_out => D0,
    Data_out => O1);
  UUT : special port map (
    Load => L0,
    Shift_in => S0,
    Done_in => D0,
    Data_out => O0);
end new_structure;
--\end{comment}
