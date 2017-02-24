-----------------------------------------------------------------------------
--
-- Title       : DPcomp
-- Design      : decoder
-- author      : yy:(
------------------------------------------------------------------------------

LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;  
USE IEEE.STD_LOGIC_1164.ALL;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use std.textio.all;
use work.nondeterminism.all;
use work.channel.all;
use work.handshake.all;


ENTITY DPcomp IS
  --@synthesis_off
	 	generic(fheight : integer := 11;
            fwidth: integer := 15;
            pagesize:integer := 256;
	    indexwidth:integer :=16);

                --@synthesis_on
                
	PORT(
		 x :  INOUT channel := init_channel;
		 y :  INOUT channel := init_channel;
		 z :  INOUT channel := init_channel;
	
		 dp_data : INOUT channel := init_channel
	     );
END DPcomp;


ARCHITECTURE arch OF DPcomp IS 

	signal v_x:std_logic_vector(7 downto 0);
	signal v_y:std_logic_vector(7 downto 0);
	signal v_z:std_logic_vector(7 downto 0);
	signal v_dp_data:std_logic_vector(7 downto 0);

       
	type NumType is array (0 to 15) of Integer range 0 to 15;
  	constant dpi : NumType := (0,8,12,4,2,10,14,6,3,11,15,7,1,9,13,5);
  	type ttype is array (0 to 16*256*4*4-1) of std_logic_vector(7 downto 0);
	signal tt : ttype;	 
 	signal go:std_logic:='0';	 
	
BEGIN
  --@synthesis_off
  opFile: process	 
    constant fn: string(1 to 6):="dp.dat"; 
    file v_ifile:text; 
    VARIABLE v_line: line;	
    VARIABLE good: boolean;
    VARIABLE v_int: integer;
    VARIABLE v_std: std_logic_vector(7 DOWNTO 0);  
    variable cnt:integer:=0;	
  begin
    file_open(v_ifile,fn,READ_MODE);
    while not endfile (v_ifile) loop
      readline(v_ifile,v_line);
      read(v_line,v_int,good);
      ASSERT good
        REPORT "Text I/O FOR DPcomp read error!"
        SEVERITY ERROR;	  
      tt(cnt)<=  conv_std_logic_vector(v_int,v_std'length);	
      wait for 1 ns;
      cnt:=cnt+1;
    end loop;     
    
    file_close(v_ifile);   
    report "file is closed successfully!"; 
    assign(go,'1',3,5);
    --guard (stop,'1');
    wait;
  end process;
  --@synthesis_on
	
  MakeTT: PROCESS			
    variable dpx:integer:=0;
    variable ina:integer:=0;
    constant w2:integer:= 2*fwidth*16;	 
    variable cnt:integer:=0;
    variable div2:integer:=0;
    variable w:integer:=fwidth*16;	
    variable index: integer;   
    --for MAKETTable
    type  ntype is array (0 to 2) of Integer;
    constant ttb : ntype := (33,97,161);
    variable ttx:integer:=0;
    variable tty:integer:=0;
    variable ttz:integer:=0;
    variable ix:integer:=0;
    variable iy:integer:=0;
  begin   
--@synthesis_off
    guard(go,'1');
--@synthesis_on
    receive(x, v_x);	
    receive(y, v_y);
    receive(z, v_z);
--@synthesis_off
    wait for delay(5,10);	  --wait for is different between variable and
    -- signal wait for 0ns, do nothing for the variable, but 
    --works for signal
    div2:=cnt/2;
    index:=((cnt/w/2) mod 2)*8 +cnt mod 8;
    dpx := dpi(index);
    cnt:=cnt+1;	
    --from the MAKEttable function in c++	
    
    ix := conv_integer(v_x);
    iy := conv_integer(v_y);
    ttz := conv_integer(v_z);
    ttx := 3;
    while ix<ttb(ttx-1)+dpx*4 loop
      ttx := ttx-1;				 
    end loop;	
    tty := 3;
    while iy<ttb(tty-1)+dpx*4 loop
      tty := tty-1;				 
    end loop;	
    index := dpx*(256*16)+(4*ttx+tty)*256+ttz;				   
    v_dp_data <= tt(index);				   
    wait for delay(3,5);
--@synthesis_on
    send(dp_data, v_dp_data);
end process;
		
END arch;



