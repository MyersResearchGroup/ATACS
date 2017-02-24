---------------------------------------------------------------------------------------------------
--
-- Title       : mtt
-- Design      : datapath
-- Author      : yy 
-- date        : 6/8/2002
---------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use ieee.std_logic_arith.all; 
use ieee.std_logic_unsigned.all;
USE work.nondeterminism.ALL;
USE work.channel.ALL;
USE std.textio.ALL;

entity mtt is  
  GENERIC(
    fheight : integer := 11;
    fwidth : integer := 15;
    pagesize : integer := 64 
    );
  port(
    Splice : inout channel := init_channel(receiver => timing(1,3));
    cr : in std_logic_vector(7 downto 0);
    cb : in std_logic_vector(7 downto 0);
    lum : in std_logic_vector(7 downto 0);
    addr : out std_logic_vector(15 downto 0);
    dp : in std_logic_vector(3 downto 0)
    );
end MTT;

architecture mtt of mtt is 
begin	         
  ttb_cal: process
    VARIABLE pos : integer := 0;  
    VARIABLE w : integer := fwidth*16;	
    --VARIABLE index : integer;   
    VARIABLE idp : integer;
		
    --for MAKETTable
    TYPE  ntype IS ARRAY (0 TO 2) OF integer;
    CONSTANT ttb : ntype := (33,97,161);
    VARIABLE ttx : integer := 0;
    VARIABLE tty : integer := 0;
    --VARIABLE ttz : integer := 0;
    VARIABLE ix : integer := 0;
    VARIABLE iy : integer := 0; 
  begin
    await(Splice);
    --@synthesis_off
    ix := conv_integer(cb);
    iy := conv_integer(cr);   
    idp:=conv_integer(dp);		
		
    ttx := 3;
    WHILE ix<ttb(ttx-1)+idp*4 LOOP
      ttx := ttx-1;
    END LOOP;
    tty := 3;
    WHILE iy<ttb(tty-1)+idp*4 LOOP
      tty := tty-1;
    END LOOP;  
    --ttz := conv_integer(lum);
    --index := idp*(256*4*4)+(4*ttx+tty)*256+ttz;
    addr <= dp & conv_std_logic_vector(ttx,2) &
            conv_std_logic_vector(tty,2) & lum;
    wait for delay(3,5);
    --@synthesis_on
    receive(Splice);
  end process;
end mtt;








