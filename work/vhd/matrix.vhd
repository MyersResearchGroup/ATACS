---------------------------------------------------------------------------------------------------
--
-- Title       : matrix
-- Design      : decoder
-- Author      : yy:)

-------------------------------------------------------------------------------

LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use std.textio.all;
use work.nondeterminism.all;
use work.channel.all;
use work.handshake.all;

ENTITY matrix IS
  generic(fheight : integer := 11;
            fwidth: integer := 15;
            pagesize:integer := 64;
            fn_id: integer :=2);
   
    PORT(data : INOUT channel := init_channel);
END matrix;

ARCHITECTURE arch OF matrix IS
   
  signal v_data:std_logic_vector(7 downto 0);

  --@synthesis_off 
  type fn_type is array(0 to 2) of string(1 to 7);
  constant fn: fn_type:=("crx.dat", "cbx.dat", "lum.dat");   
  --signal stop:std_logic:='0';
  file v_ifile:text; 
  --@synthesis_on 
    
BEGIN
   
  read_file: process  
    --@synthesis_off 
    VARIABLE v_line: line;	
    VARIABLE good: boolean;
    VARIABLE v_int: integer;
    VARIABLE v_std: std_logic_vector(7 DOWNTO 0);  
    variable cnt:integer:=0;
    --@synthesis_on 
  begin
    --@synthesis_off 
    file_open(v_ifile,fn(fn_id),READ_MODE);
    while cnt<fheight*fwidth*pagesize and not endfile (v_ifile) loop
      readline(v_ifile,v_line);
      read(v_line,v_int,good);
      ASSERT good
        REPORT fn(fn_id) & " Text I/O read error!"
        SEVERITY ERROR;	
      v_data <= conv_std_logic_vector(v_int,v_std'length);	      	
      wait for delay (3,5);
      cnt:=cnt+1;
      --@synthesis_on
      send(data,v_data);
    --@synthesis_off 
    end loop;                     
    file_close(v_ifile);   
    report "file is closed successfully!"; 
    --  guard (stop,'1');
    wait;
    --@synthesis_on
end process;
        
END arch;











