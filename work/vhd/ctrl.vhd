---------------------------------------------------------------------------------------------------
--
-- Title       : ctrl
-- Design      : decoder
-- Author      : yy
--
---------------------------------------------------------------------------------------------------
--
-- File        : ctrl.vhd

LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL; 
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use std.textio.all;
use work.nondeterminism.all;
use work.channel.all;
use work.handshake.all;

ENTITY ctrl IS

  --@synthesis_off
	generic(fheight : integer := 11;
            fwidth: integer := 15;
            pagesize:integer := 256;
	    indexwidth:integer :=16);

  --@synthesis_on
        
	 PORT(
	
		 Cr_in : INOUT channel := init_channel;
		 Cb_in :  INOUT channel := init_channel;
	
		 Lum_in : INOUT channel := init_channel;

		 Data_in :  INOUT channel := init_channel;
		 Data_out : INOUT channel := init_channel;
		 x : INOUT channel := init_channel;
		 y : INOUT channel := init_channel;
		 z :INOUT channel := init_channel
	     );
END ctrl;

ARCHITECTURE arch OF ctrl IS 
	signal v_Cr_in:std_logic_vector(7 downto 0); 
	signal v_Cb_in:std_logic_vector(7 downto 0);
	signal v_Lum_in:std_logic_vector(7 downto 0);	 
	signal v_data_in:std_logic_vector(7 downto 0);	 
  	--signal stop: std_logic := '0';
BEGIN  
  ctrl:process
    --@synthesis_off
    variable dummy : integer;
    variable countdown : integer := fheight*fwidth*64;
    --@synthesis_on
  begin		  
    receive (Cr_in, v_Cr_in);	
    receive (Cb_in, v_Cb_in);  

    receive (Lum_in, v_Lum_in);
    send(x,v_Cb_in);
    send(y,v_Cr_in);
    send(z,v_Lum_in); 
    receive (Data_in,v_Data_in); 
    wait for delay(5,10);
    send (Data_out,v_Data_in);

--     receive (Lum_in, v_Lum_in);
--     send(x,v_Cb_in);
--     send(y,v_Cr_in);
--     send(z,v_Lum_in); 
--     receive (Data_in,v_Data_in); 
--     wait for delay(5,10);
--     send (Data_out,v_Data_in);

--     receive (Lum_in, v_Lum_in);
--     send(x,v_Cb_in);
--     send(y,v_Cr_in);
--     send(z,v_Lum_in); 
--     receive (Data_in,v_Data_in); 
--     wait for delay(5,10);
--     send (Data_out,v_Data_in);

--     receive (Lum_in, v_Lum_in);
--     send(x,v_Cb_in);
--     send(y,v_Cr_in);
--     send(z,v_Lum_in); 
--     receive (Data_in,v_Data_in); 
--     wait for delay(5,10);
--     send (Data_out,v_Data_in);
        
    -- stop VHDL simulation when no more data is wait for process
    --     
    --@synthesis_off
    countdown:=countdown-1;
    if countdown=0 then
      --guard(stop,'1');
      wait;
    end if;	
    --@synthesis_on
    end process ;
		
END arch;

