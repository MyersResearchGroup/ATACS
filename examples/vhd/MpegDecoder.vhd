---------------------------------------------------------------------------------------------------
--
-- Title       : MpegDecoder
-- Design      : datapath
-- Author      : yy :p
---------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use work.channel.all;

entity MpegDecoder is
  port(
    sampling : out std_logic;
    Display : inout std_logic_vector(7 downto 0);
    DPdata : inout channel := init_channel;
    cb : inout channel := init_channel;
    cr : inout channel := init_channel;
    lum : inout channel := init_channel
    );
end MpegDecoder;

architecture MpegDecoder of MpegDecoder is
  component ctrl
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
  end component;
  component datapath
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
  end component;


  signal Increment : channel := init_channel;  -- tells counter to count
  signal Calculate : channel := init_channel;  -- tells dpCalc to do its thing
  signal Mem_Access : channel := init_channel;  -- tells memory to handle access
  signal mem_RW : std_logic;
  signal Splice : channel := init_channel;  -- tells MTT to splice address
  signal Reset : std_logic;
  signal n_cb : std_logic_vector (7 downto 0);
  signal n_cr : std_logic_vector (7 downto 0);
  signal n_DPdata : std_logic_vector (7 downto 0);
  signal n_lum : std_logic_vector (7 downto 0);

begin


  control : ctrl
    port map(
      DPdata => DPdata,
      DPdata_d => n_DPdata,
      cb => cb,
      cb_d => n_cb,
      Increment => Increment,
      cr => cr,
      cr_d => n_cr,
      Calculate => Calculate,
      lum => lum,
      lum_d => n_lum,
      mem_RW => mem_RW,
      Mem_Access => Mem_Access,
      Splice => Splice,
      reset => Reset
      );

  datap : datapath
    port map(
      Reset => Reset,
      cb => n_cb,
      Increment => Increment,
      cr => n_cr,
      Calculate => Calculate,
      lum => n_lum,
      mem_Din => n_DPdata,
      mem_Dout => Display,
      mem_RW => mem_RW,
      Mem_Access => Mem_Access,
      Splice => Splice
      );
--@synthesis_off
  sampling <= mem_RW when probe(Mem_Access) else '0';
--@synthesis_on

end MpegDecoder;



