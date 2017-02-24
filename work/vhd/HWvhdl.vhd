------------------------------------------------------------------------------
--
-- Title       : HWvhdl
-- Design      : NewDecoder
-- Author      : yy:p  
--date         : 5-21-2002,6/2/2002
------------------------------------------------------------------------------


LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE ieee.std_logic_arith.ALL;
USE ieee.std_logic_unsigned.ALL;
USE std.textio.ALL;
USE work.nondeterminism.ALL;
USE work.channel.ALL;
USE work.handshake.ALL;


ENTITY HWvhdl IS  
  GENERIC(
    fheight : integer := 11;
    fwidth : integer := 15;
    pagesize : integer := 64 
    );
  PORT( 
    outp : INOUT channel:= init_channel;
    DPdata : INOUT channel:= init_channel;
    cr : INOUT channel:= init_channel;
    cb : INOUT channel:= init_channel;
    lum :  INOUT channel:= init_channel;
    AsyncMux_sel:out std_logic:='0';
    cnt_req: inout std_logic:='0';
    cnt_ack: inout std_logic;  
    mem_req: inout std_logic:='0';
    mem_ack: inout std_logic;            
    dpCal_req: inout std_logic:='0';
    dpCal_ack: inout std_logic;
    mtt_req: inout std_logic:='0';
    mtt_ack: in std_logic;
    reset: in std_logic
    );
END HWvhdl;

ARCHITECTURE arch OF HWvhdl IS 
	
  SIGNAL s_dpx:std_logic_vector(3 DOWNTO 0);     
  SIGNAL s_cb:std_logic_vector(7 DOWNTO 0);
  SIGNAL s_cr:std_logic_vector(7 DOWNTO 0);
  SIGNAL s_lum:std_logic_vector(7 DOWNTO 0);
  SIGNAL s_DPdata:std_logic_vector(7 DOWNTO 0);  
  SIGNAL s_output:std_logic_vector(7 DOWNTO 0);  
  -- signal reset : std_logic;
  CONSTANT ttMemorySize : integer := 16*4*4*256;
  TYPE ttype IS ARRAY (0 TO ttMemorySize-1) OF
    std_logic_vector(7 DOWNTO 0);
  SIGNAL tt : ttype;
  
  TYPE NumType IS ARRAY (0 TO 15) OF Integer RANGE 0 TO 15;
  CONSTANT dpi : NumType := (0,8,12,4,2,10,14,6,3,11,15,7,1,9,13,5);
BEGIN
	
  Decoder: PROCESS
  BEGIN
    await_any (DPdata, cb, cr, lum);
    if (probe (DPdata))then
      receive(DPdata, s_DPdata);
      vassign(AsyncMux_sel,'0',1,3);
      assign(mem_req, '1',3,5);
      guard(mem_ack,'1');
      assign(mem_req,'0',3,5);
      guard(mem_ack,'0');
      assign (cnt_req,'1',1,3);
      guard (cnt_ack,'1'); 
      assign (cnt_req,'0',1,3);
      guard (cnt_ack,'0');
    elsif (probe (cb)) then
      receive(cb,s_cb);
    elsif (probe (cr)) then
      receive(cr, s_cr);
    elsif (probe(lum))then
      receive(lum, s_lum);  		
      vassign(AsyncMux_sel,'1',1,3);
      assign(dpCal_req,'1',3,5);
      guard(dpCal_ack,'1');
      assign(dpCal_req,'0',3,5);
      guard(dpCal_ack,'0');
      assign(mem_req,'1',3,5);
      guard(mem_ack,'1');
      assign(mem_req,'0',3,5);
      guard(mem_ack,'0');
      send(outp,s_output);    
    end if;     
    wait for delay(3,5);
  END PROCESS; 
  
process
  VARIABLE dpx : integer := 0;
  VARIABLE cnt : integer := 0;
  VARIABLE pos : integer := 0;
  VARIABLE w : integer := fwidth*16;	
  VARIABLE index : integer; 
		
  --for MAKETTable
  TYPE  ntype IS ARRAY (0 TO 2) OF integer;
  CONSTANT ttb : ntype := (33,97,161);
  VARIABLE ttx : integer := 0;
  VARIABLE tty : integer := 0;
  VARIABLE ttz : integer := 0;
  VARIABLE ix : integer := 0;
  VARIABLE iy : integer := 0; 
  VARIABLE base : integer := 0;
begin
  guard(dpCal_req,'1');
  --@synthesis_off
  ix := conv_integer(s_cb);
  iy := conv_integer(s_cr);
  index := ((cnt/w/2) MOD 2)*8 +cnt MOD 8;   
  dpx := dpi(index);        
  wait for delay (3,5);
				
  ttx := 3;
  WHILE ix<ttb(ttx-1)+dpx*4 LOOP
    ttx := ttx-1;
  END LOOP;
  tty := 3;
  WHILE iy<ttb(tty-1)+dpx*4 LOOP
    tty := tty-1;
  END LOOP;  
  ttz := conv_integer(s_lum);
  index := dpx*(256*4*4)+(4*ttx+tty)*256+ttz;   
  wait for delay(3,5);
  cnt := cnt+1;
  --@synthesis_on
  assign(dpCal_ack,'1',3,5);
  guard(dpCal_req,'0');
  assign(dpCal_ack,'0',3,5);
end process;
  
process
begin
  guard(mem_req, '1');
  --@synthesis_off
  if (AsyncMux_sel = '0') then
    tt(pos)<= s_DPdata;  
    WAIT FOR delay(3,5); 
  else
    s_output <= tt(index);
    WAIT FOR delay(3,5);
  end if;
  --@synthesis_on
  assign(mem_ack,'1',3,5);
  guard(mem_req,'0');
  assign(mem_ack,'0',3,5);
end process;

process
begin
  --@synthesis_off
  if (reset = '1') then
    pos:=0;
  else
  --@synthesis_on
    guard(cnt_req,'1');
    --@synthesis_off
    pos:= pos+1;  
    if (pos>=ttMemorySize) then 
      pos:=0;
      wait for delay (3,5);
    end if;   
    --@synthesis_on
    assign(cnt_ack,'1',1,3); 
    guard(cnt_req,'0');
    assign(cnt_ack,'0',1,3);
  --@synthesis_off
  end if;
  --@synthesis_on
end process;

END arch;









