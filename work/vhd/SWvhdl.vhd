---------------------------------------------------------------------------------------------------
--
-- Title       : SWvhdl
-- Design      : NewDecoder
-- Author      : yy:(
-- date:         5-21-2002,6/2
------------------------------------------------------------------------------------------------


LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE ieee.std_logic_arith.ALL;
USE std.textio.ALL;
USE work.nondeterminism.ALL;
USE work.channel.ALL;
USE work.handshake.ALL;


ENTITY SWvhdl IS
	GENERIC(
		fheight : integer := 11;
		fwidth: integer := 15
		);
	PORT(
		outp : INOUT channel := init_channel;
		DPdata :  INOUT channel := init_channel;
		cr : INOUT channel := init_channel;
		cb :  INOUT channel := init_channel;
		lum :  INOUT channel := init_channel; 
		failed : OUT std_logic:='0'; 
                reset:out std_logic:='1'
		);
END SWvhdl;

ARCHITECTURE arch OF SWvhdl IS
	FILE fh_cbx:text;           
	FILE fh_crx:text;    
	FILE fh_lum:text;    
	FILE fh_dp:text;
	FILE fh_out:text;
	SIGNAL s_cb:std_logic_vector(7 DOWNTO 0);
	SIGNAL s_cr:std_logic_vector(7 DOWNTO 0);
	SIGNAL s_lum:std_logic_vector(7 DOWNTO 0);
	SIGNAL s_DPdata:std_logic_vector(7 DOWNTO 0);
	SIGNAL s_output:std_logic_vector(7 DOWNTO 0); 
	SIGNAL s_display:std_logic_vector(7 DOWNTO 0); 
	SIGNAL s_sampling:std_logic:='0';
BEGIN    
	read_file: PROCESS 
		VARIABLE v_line: line;	
		VARIABLE good: boolean;
		VARIABLE v_int: integer;
		
	BEGIN 
          wait for delay (3,5);
         --@synthesis_off
          reset<='0';
         --@synthesis_on
          wait for delay (3,5);
		-- open/read/send translation table to decoder
		--
		file_open(fh_dp,"dp.dat",READ_MODE); 		
		--@synthesis_off
		WHILE NOT endfile(fh_dp) LOOP
			--@synthesis_on
			--read data from dp.dat
			readline(fh_dp,v_line);
			read(v_line,v_int,good); 
			ASSERT good
			REPORT " dp.dat Text I/O read error!"
			SEVERITY ERROR;	
			s_DPdata <= conv_std_logic_vector(v_int,8);
			WAIT FOR 1 ns;
			send(DPdata,s_DPdata); 
			--@synthesis_off
		END LOOP;   
		--@synthesis_on
                file_close(fh_dp);   
		REPORT "dp_file is closed successfully!"; 
		
		file_open(fh_out,"out.dat",READ_MODE);	
		file_open(fh_cbx,"cbx.dat",READ_MODE); 
		file_open(fh_crx,"crx.dat",READ_MODE);
		file_open(fh_lum,"lum.dat",READ_MODE);
		
		--@synthesis_off
		WHILE NOT endfile (fh_cbx) LOOP
			--@synthesis_on
			--read data from cbx.dat
			readline(fh_cbx,v_line);
			read(v_line,v_int,good);
			ASSERT good
			REPORT " cbx.dat Text I/O read error!"
			SEVERITY ERROR;	
			s_cb <= conv_std_logic_vector(v_int,8);	      	
			WAIT FOR delay (3,5);
			
			--read data from crx.dat			
			readline(fh_crx,v_line);
			read(v_line,v_int,good);
			ASSERT good
			REPORT " crx.dat Text I/O read error!"
			SEVERITY ERROR;	
			s_cr <= conv_std_logic_vector(v_int,8);	      	
			WAIT FOR delay (3,5);
			
			send(cb,s_cb);
                        send(cr,s_cr);
			--read 4 data from lum.dat
			--@synthesis_off
			FOR dummy IN 1 TO 4 LOOP
				--@synthesis_on
				readline(fh_lum,v_line);
				read(v_line,v_int,good);
				ASSERT good
				REPORT " lum.dat Text I/O read error!"
				SEVERITY ERROR;	
				s_lum <= conv_std_logic_vector(v_int,8);
				WAIT FOR delay (3,5);
				send(lum,s_lum);  
				receive(outp, s_output);
				--  Testing
				--read file from out.dat
				--@synthesis_off
				readline(fh_out,v_line);
				read(v_line,v_int,good);
				ASSERT good
				REPORT " out.dat Text I/O read error!"
				SEVERITY ERROR;	
				s_display <= conv_std_logic_vector(v_int,8);
				WAIT FOR delay (1,1); 
				s_sampling <= '1';
				ASSERT s_output=s_display
				REPORT "Miss-match! "
				SEVERITY WARNING;
				IF s_output/=s_display THEN
					failed <= '1';
				END IF;    
				WAIT FOR delay (1,1);
				s_sampling <= '0';
				--@synthesis_on
				--@synthesis_off
			END LOOP;		
		END LOOP;
		--@synthesis_on
		file_close(fh_out);   
		REPORT "out_file is closed successfully!";  
		file_close(fh_cbx);   
		REPORT "cbx_file is closed successfully!";  
		file_close(fh_crx);   
		REPORT "crx_file is closed successfully!"; 
		file_close(fh_lum);   
		REPORT "lum_file is closed successfully!"; 
		WAIT;
	END PROCESS;    
END arch;




