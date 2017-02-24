LIBRARY ieee, work;
USE ieee.std_logic_1164.all;
use work.handshake.all;

ENTITY and2_env IS
     PORT(
	  c0 : IN std_logic;
	  c1 : IN std_logic;
	  en : IN std_logic;
	  a0 : INOUT std_logic := '0';
	  a1 : INOUT std_logic := '0';
	  b0 : INOUT std_logic := '0';
	  b1 : INOUT std_logic := '0';
	  ce : INOUT std_logic := '0'
	  );
END and2_env;

ARCHITECTURE behavior of and2_env IS
BEGIN

  a : process
    variable choosea : integer;
  begin
    choosea := selection(2);
    case choosea is
      when 1 =>
	assign(a0, '1', 200, inf);
      when others =>
	assign(a1, '1', 200, inf);
    end case;
    guard(en, '0');
    if (a0 = '1') then
      assign(a0, '0', 200, inf);
    elsif (a1 = '1') then
      assign(a1, '0', 200, inf);
    end if;
    guard(en, '1');
  end process;

  b : process
    variable chooseb : integer;
  begin
    chooseb := selection(2);
    case chooseb is
      when 1 =>
	assign(b0, '1', 200, inf);
      when others =>
	assign(b1, '1', 200, inf);
    end case;
    guard(en, '0');
    if (b0 = '1') then
      assign(b0, '0', 200, inf);
    elsif (b1 = '1') then
      assign(b1, '0', 200, inf);
    end if;
    guard(en, '1');
  end process;

  c : process
  begin
    assign(ce, '1', 200, inf);
    guard_or(c0, '1', c1, '1');
    assign(ce, '0', 200, inf);
    guard_and(c0, '0', c1, '0');    
  end process;
  
END behavior;

