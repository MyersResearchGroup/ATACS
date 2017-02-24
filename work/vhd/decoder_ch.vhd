---------------------------------------------------------------------------------------------------
--
-- Title       : decoder_ch (the top level)
-- Design      : decoder
-- Author      : yy 
---------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use work.nondeterminism.all;
use work.channel.all;
use work.handshake.all;

ENTITY decoder_ch IS
  PORT(
       failed : OUT STD_LOGIC
  );
END decoder_ch;

ARCHITECTURE decoder_ch OF decoder_ch IS


COMPONENT ctrl
  --@synthesis_off
  GENERIC(
       fheight : INTEGER := 11;
       fwidth : INTEGER := 15;
       indexwidth : INTEGER := 16;
       pagesize : INTEGER := 256
  );
  --@synthesis_on
  PORT (
       Cb_in : INOUT channel := init_channel;
       Cr_in : INOUT channel := init_channel;
       Data_in : INOUT channel := init_channel;
       Data_out : INOUT channel := init_channel;
       Lum_in : INOUT channel := init_channel;
       x : INOUT channel := init_channel;
       y : INOUT channel := init_channel;
       z : INOUT channel := init_channel
  );
END COMPONENT;
COMPONENT display
  --@synthesis_off
  GENERIC(
       fheight : INTEGER := 11;
       fwidth : INTEGER := 15;
       indexwidth : INTEGER := 16;
       pagesize : INTEGER := 256
  );

  --@synthesis_on
  PORT (
       failed : OUT STD_LOGIC := '0';
       dis_data : INOUT channel := init_channel
  );
END COMPONENT;
COMPONENT DPcomp

  --@synthesis_off
  
  GENERIC(
       fheight : INTEGER := 11;
       fwidth : INTEGER := 15;
       indexwidth : INTEGER := 16;
       pagesize : INTEGER := 256
  );

  --@synthesis_on
  PORT (
       dp_data : INOUT channel := init_channel;
       x : INOUT channel := init_channel;
       y : INOUT channel := init_channel;
       z : INOUT channel := init_channel
  );
END COMPONENT;
COMPONENT matrix

  --@synthesis_off
  GENERIC(
       fheight : INTEGER := 11;
       fn_id : INTEGER := 0;
       fwidth : INTEGER := 15;
       pagesize : INTEGER := 64
  );

  --@synthesis_on
  PORT (
       data : INOUT channel := init_channel
  );
END COMPONENT;

SIGNAL Cb_in : channel := init_channel;
SIGNAL Cr_in : channel := init_channel;
SIGNAL Dis_data : channel := init_channel;
SIGNAL dp_data : channel := init_channel;
SIGNAL Lum_in : channel := init_channel;
SIGNAL x : channel := init_channel;
SIGNAL y : channel := init_channel;
SIGNAL z : channel := init_channel;

BEGIN


Cb : matrix
--@synthesis_off
  GENERIC MAP (
       fn_id => 1
  )
--@synthesis_on
  PORT MAP(
       data => Cb_in
  );

Cr : matrix
  PORT MAP(
       data => Cr_in
  );

Lum : matrix
--@synthesis_off
  GENERIC MAP (
       fn_id => 2,
       pagesize => 256
  )
--@synthesis_on
  PORT MAP(
       data => Lum_in
  );

PI : ctrl
  PORT MAP(
       Cb_in => Cb_in,
       Cr_in => Cr_in,
       Data_in => dp_data,
       Data_out => Dis_data,
       Lum_in => Lum_in,
       x => x,
       y => y,
       z => z
  );

TTable : DPcomp
  PORT MAP(
       dp_data => dp_data,
       x => x,
       y => y,
       z => z
  );

checker : display
  PORT MAP(
       dis_data => Dis_data,
       failed => failed
  );


END decoder_ch;





