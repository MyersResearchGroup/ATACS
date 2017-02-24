---------------------------------------------------------------------------------------------------
--
-- Title       : display
-- Design      : decoder
-- Author      : yy:)
-- 
--
---------------------------------------------------------------------------------------------------
--
-- File        : display.vhd


LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
use ieee.std_logic_arith.all;
use std.textio.all;
use work.nondeterminism.all;
use work.channel.all;
use work.handshake.all;

ENTITY display IS
--@synthesis_off
  generic(fheight : integer := 11;
          fwidth: integer := 15;
          pagesize:integer := 256;
          indexwidth:integer :=16);
--@synthesis_on
   
  PORT(dis_data : INOUT channel := init_channel;
       failed : out std_logic := '0' );
END display;

ARCHITECTURE arch OF display IS	  
--@synthesis_off
  type memory is array (0 to fheight*fwidth*pagesize-1) of
    std_logic_vector(7 downto 0);  
  signal mem: memory;   
  constant fn: string(1 to 7):="out.dat"; 
  FILE v_ifile: text;					
  signal v_dis_data: std_logic_vector(7 downto 0);
  --signal stop: std_logic := '0';
  signal go : std_logic := '0';
  --signal nomore : std_logic := '0';     
--@synthesis_on

BEGIN
  --@synthesis_off
  init: process  
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
        REPORT "TEXT I/O OF DISPLSY read error!"
        SEVERITY warning;	
      mem(cnt) <= conv_std_logic_vector(v_int,v_std'length);	  
      cnt:=cnt+1;
      wait for 1 ns;
    end loop;
    file_close(v_ifile);   
    report "file is closed successfully!";     
    assign(go,'1',3,5);
    --guard(stop,'1');
    wait;
  end process;
  --@synthesis_on
		
  handling_PI:process
    --@synthesis_off
    constant w2:integer:= 2*fwidth*16;	 
    variable base:integer:=0;
    variable cnt:integer:=0;
    variable div2:integer:=0;
    variable where:integer:=0;
    variable w:integer:=fwidth*16;	  
    --@synthesis_on
  begin            
    --@synthesis_off
    guard(go,'1');
    --@synthesis_on
    receive(dis_data,v_dis_data);
    --@synthesis_off
    wait for delay (3,5);
    div2:=cnt/2;
    if div2 mod 2=0 then
      where:=base+div2+cnt mod 2;	
    else
      where:=base+w+cnt-div2-1;
    end if;     
    ASSERT mem(where) = v_dis_data
      REPORT "mismatch data!"
      SEVERITY warning;     
    if mem(where) /= v_dis_data then
      failed <= '1';
    end if;
    cnt:=cnt+1;	
    if cnt mod w2 = 0 then
      base:=base+w;
    end if;
    
    -- stop VHDL simulation when no more data is wait for process
    --     
    if cnt>=fheight*fwidth*pagesize then
      wait;
      --guard(nomore,'1');
    end if;
    --@synthesis_on
  end process;                  
    
END arch;
