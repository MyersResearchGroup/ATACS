LIBRARY ieee, work;
USE ieee.std_logic_1164.all;
use work.handshake.all;

ENTITY and2 IS
     PORT(
	  c0 : INOUT std_logic := '0';
	  c1 : INOUT std_logic := '0';
	  en : INOUT std_logic := '1';
	  a0 : IN std_logic;
	  a1 : IN std_logic;
	  b0 : IN std_logic;
	  b1 : IN std_logic;
	  ce : IN std_logic
	  );
END and2;

ARCHITECTURE behavior of and2 IS
  signal aibar : std_logic := '1';
  signal bibar : std_logic := '1';
  signal ab : std_logic := '0';
  signal ci : std_logic := '0';
BEGIN

   cand : process
   begin
     guard(ce, '1');
     guard_or(a0, '1', a1, '1');
     guard_or(b0, '1', b1, '1');
     if    (a0 = '1' and b0 = '1') then                --00 => 0
       assign(c0, '1', 200, 300);
     elsif (a0 = '1' and b1 = '1') then                --01 => 0
       assign(c0, '1', 200, 300);
     elsif (a1 = '1' and b0 = '1') then                --10 => 0
       assign(c0, '1', 200, 300);
     elsif (a1 = '1' and b1 = '1') then                --11 => 1
       assign(c1, '1', 200, 300);
     end if;
     guard_and(en, '0', ce, '0');
     if (c0 = '1') then
       assign (c0, '0', 200, 300);
     elsif (c1 = '1') then
       assign (c1, '0', 200, 300);
     end if;
     guard(en, '1');
   end process;

   aibar <= not (a0 or a1) after delay(200, 300);
-- aibar <= a0 nor a1 after delay(200, 300);
   bibar <= not (b0 or b1) after delay(200, 300);
-- bibar <= b0 nor b1 after delay(200, 300);
   ci <= c0 or c1 after delay(200,300);

   c_element_ab : process
   begin
     guard_and (aibar, '0', bibar, '0');
     assign(ab, '1', 200, 300);
     guard_and (aibar, '1', bibar, '1');
     assign(ab, '0', 200, 300);
   end process;
   
   c_element_en : process
   begin
     guard_and (ci, '1', ab, '1');
     assign(en, '0', 200, 300);
     guard_and (ci, '0', ab, '0');
     assign(en, '1', 200, 300);
   end process;
   
END behavior;

