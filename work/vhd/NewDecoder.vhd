---------------------------------------------------------------------------------------------------
--
-- Title       : NewDecoder
-- Design      : NewDecoder
-- Author      : YY
-- date        : 6-2-2002
-------------------------------------------------------------------------------


library IEEE;
use IEEE.std_logic_1164.all;
use work.channel.all;

entity NewDecoder is
  port(
       AsyncMux_sel : out std_logic;
       failed : out std_logic
  );
end NewDecoder;

architecture NewDecoder of NewDecoder is


component hwvhdl
  generic(
       fheight : integer := 11;
       fwidth : integer := 15;
       pagesize : integer := 64
  );
  port (
       cnt_ack : in std_logic;
       dpCal_ack : in std_logic;
       mem_ack : in std_logic;
       mtt_ack : in std_logic;
       reset : in std_logic;
       AsyncMux_sel : out std_logic := '0';
       DPdata : inout channel := init_channel;
       cb : inout channel := init_channel;
       cnt_req : inout std_logic := '0';
       cr : inout channel := init_channel;
       dpCal_req : inout std_logic := '0';
       lum : inout channel := init_channel;
       mem_req : inout std_logic := '0';
       mtt_req : inout std_logic := '0';
       outp : inout channel := init_channel
  );
end component;
component swvhdl
  generic(
       fheight : integer := 11;
       fwidth : integer := 15
  );
  port (
       dpCal_req : in std_logic;
       mem_req : in std_logic;
       mtt_req : in std_logic;
       failed : out std_logic := '0';
       reset : out std_logic := '0';
       DPdata : inout channel := init_channel;
       cb : inout channel := init_channel;
       cr : inout channel := init_channel;
       dpCal_ack : inout std_logic := '0';
       lum : inout channel := init_channel;
       mem_ack : inout std_logic := '0';
       mtt_ack : inout std_logic := '0';
       outp : inout channel := init_channel
  );
end component;



signal cb : channel := init_channel;
signal cnt_ack : std_logic;
signal cnt_req : std_logic;
signal cr : channel := init_channel;
signal dpCal_ack : std_logic;
signal dpCal_req : std_logic;
signal DPdata : channel := init_channel;
signal lum : channel := init_channel;
signal mem_ack : std_logic;
signal mem_req : std_logic;
signal mtt_ack : std_logic;
signal mtt_req : std_logic;
signal outp : channel := init_channel;
signal reset : std_logic;

begin


Decoder : hwvhdl
  port map(
       AsyncMux_sel => AsyncMux_sel,
       DPdata => DPdata,
       cb => cb,
       cnt_ack => cnt_ack,
       cnt_req => cnt_req,
       cr => cr,
       dpCal_ack => dpCal_ack,
       dpCal_req => dpCal_req,
       lum => lum,
       mem_ack => mem_ack,
       mem_req => mem_req,
       mtt_ack => mtt_ack,
       mtt_req => mtt_req,
       outp => outp,
       reset => reset
  );

Enviorment : swvhdl
  port map(
       DPdata => DPdata,
       cb => cb,
       cr => cr,
       failed => failed,
       lum => lum,
       outp => outp,
       reset => reset
  );


end NewDecoder;

















