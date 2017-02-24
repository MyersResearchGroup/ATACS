---------------------------------------------------------------------------------------------------
--
-- Title       : datapath
-- Design      : datapath
-- Author      : yy
-------------------------------------------------------------------------------------------------

library IEEE;

use IEEE.std_logic_1164.all;
use work.channel.all;

entity datapath is
  port(
    Reset : in std_logic;
    Increment : inout channel := init_channel;
    Calculate : inout channel := init_channel;
    mem_RW : in std_logic;
    Mem_Access : inout channel := init_channel;
    Splice : inout channel := init_channel;
    cb : in std_logic_vector(7 downto 0);
    cr : in std_logic_vector(7 downto 0);
    lum : in std_logic_vector(7 downto 0);
    mem_Din : in std_logic_vector(7 downto 0);
    mem_Dout : out std_logic_vector(7 downto 0)
    );
end datapath;

architecture datapath of datapath is
  component cnt
    port(
      Increment : inout channel := init_channel(receiver => timing(1, 3));
      DataOut : buffer std_logic_vector(15 downto 0) := (others => '0')
      );
  end component;
  component dpcal
    port(
      reset : in std_logic;
      Calculate : inout channel := init_channel(receiver => timing(1, 3));
      dp : out std_logic_vector(3 downto 0)
      );
  end component;
  component mem
    port(
      Mem_Access : inout channel := init_channel(receiver => timing(1, 3));
      addr_r : in std_logic_vector(15 downto 0);
      addr_w : in std_logic_vector(15 downto 0);
      Din : in std_logic_vector(7 downto 0);
      RW : in std_logic;
      Dout : out std_logic_vector(7 downto 0)
      );
  end component;
  component mtt
    port(
      Splice : inout channel := init_channel(receiver => timing(1, 3));
      cr : in std_logic_vector(7 downto 0);
      cb : in std_logic_vector(7 downto 0);
      lum : in std_logic_vector(7 downto 0);
      addr : out std_logic_vector(15 downto 0);
      dp : in std_logic_vector(3 downto 0)
      );
  end component;

  signal cbCal : std_logic_vector (7 downto 0);
  signal cnt_addr : std_logic_vector (15 downto 0);
  signal crCal : std_logic_vector (7 downto 0);
  signal dpIn : std_logic_vector (3 downto 0);
  signal lumCal : std_logic_vector (7 downto 0);
  signal mtt_addr : std_logic_vector (15 downto 0);

begin

  addr : mtt
    port map(
      Splice => Splice,
      addr => mtt_addr,
      cb => cb,
      cr => cr,
      dp => dpIn,
      lum => lum
      );

  dp : dpcal
    port map(
      Calculate => Calculate,
      dp => dpIn,
      reset => reset
      );

  memo : mem
    port map(
      Din => mem_Din,
      Dout => mem_Dout,
      RW => mem_RW,
      Mem_Access => Mem_Access,
      addr_r => mtt_addr,
      addr_w => cnt_addr
      );

  pos : cnt
    port map(
      Increment => Increment,
      DataOut => cnt_addr
      );

end datapath;


