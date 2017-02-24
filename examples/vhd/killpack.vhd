
-- all file manipulation is written for vhdl '87.

package nondeterminism is
  type integer_file is file of integer;

  procedure dump_seed(constant Seed1,Seed2 : in integer);
  procedure get_seed(variable Seed1,Seed2 : out integer);
  function random(number:integer) return integer;
        -- returns a pseudo-random number with uniform distribution in the 
        -- interval (0, number).
        -- Before the first call to random, dump_seed must be called to
        -- initialize the seed values (Seed1, Seed2).
        -- they must be in the range [1, 2147483562] and 
        -- [1, 2147483398] respectively.  The seed values are modified after 
        -- each call to random.
        -- This random number generator is portable for 32-bit computers, and
        -- it has period ~2.30584*(10**18) for each set of seed values.
  function delay(lower,upper: integer) return time; 

end nondeterminism; 



PACKAGE BODY nondeterminism is
  procedure dump_seed(constant Seed1,Seed2 : in integer) is
    file file_ptr : integer_file is out "seed.dat";
    begin
      write(file_ptr,Seed1);
      write(file_ptr,Seed2);
    end dump_seed;

  procedure get_seed(variable Seed1,Seed2 : out integer) is
      file file_ptr : integer_file is in "seed.dat";
      begin
        read(file_ptr,Seed1);
        read(file_ptr,Seed2);
      end get_seed;

    function random(number:integer) return integer is
        variable z, k: integer;
        variable Seed1,Seed2 : integer;
        variable tmp: real;
  begin
        get_seed(Seed1,Seed2);
        k := Seed1/53668;
        Seed1 := 40014 * (Seed1 - k * 53668) - k * 12211;
        if Seed1 < 0  then
                Seed1 := Seed1 + 2147483563;
        end if;
        k := Seed2/52774;
        Seed2 := 40692 * (Seed2 - k * 52774) - k * 3791;
        if Seed2 < 0  then
                Seed2 := Seed2 + 2147483399;
        end if;
        dump_seed(Seed1,Seed2);
        -- calculate random number
        z := Seed1 - Seed2;
        if z < 1 then
                z := z + 2147483562;
        end if;
        tmp :=  REAL(z)*4.656613e-10;
        return(integer(tmp*REAL(number)));
  end random;

function delay(lower,upper: integer) return time is
        variable randel : time;
        variable num,iter :    integer;
  begin
    randel := 0 ns; 
    loop
       num := random(upper-lower)+lower; 
       iter := num / 1000000000;
       num := num mod 1000000000;
       for i in 1 to iter loop
         randel := randel + 1000000000 ns;
       end loop;
       iter := num / 100000000;
       num := num mod 100000000;
       for i in 1 to iter loop
         randel := randel + 100000000 ns;
       end loop;
       iter := num / 10000000;
       num := num mod 10000000;
       for i in 1 to iter loop
         randel := randel + 10000000 ns;
       end loop;
       iter := num / 1000000;
       num := num mod 1000000;
       for i in 1 to iter loop
         randel := randel + 1000000 ns;
       end loop;
       iter := num / 100000;
       num := num mod 100000;
       for i in 1 to iter loop
         randel := randel + 100000 ns;
       end loop;
       iter := num / 10000;
       num := num mod 10000;
       for i in 1 to iter loop
         randel := randel + 10000 ns;
       end loop;
       iter := num / 1000;
       num := num mod 1000;
       for i in 1 to iter loop
         randel := randel + 1000 ns;
       end loop;
       iter := num / 100;
       num := num mod 100;
       for i in 1 to iter loop
         randel := randel + 100 ns;
       end loop;
       iter := num / 10;
       num := num mod 10;
       for i in 1 to iter loop
         randel := randel + 10 ns;
       end loop;
       iter := num;
       for i in 1 to iter loop
         randel := randel + 1 ns;
       end loop;
       return randel;
    end loop;
  end delay;

end nondeterminism;



--Create Entity:
--Library=work,Cell=comp1,View=entity
--Time:Wed May 26 17:03:23 1999
--By:killpack

LIBRARY ieee, work;
USE ieee.std_logic_1164.all;
USE work.nondeterminism.all;
ENTITY comp1 IS
    PORT(
        ack : INOUT std_ulogic := '0';
        eq : INOUT std_ulogic := '0';
        gr : INOUT std_ulogic := '0';
        le : INOUT std_ulogic := '0';
        a : IN std_ulogic;
        b : IN std_ulogic;
        req : IN std_ulogic
    );
END comp1;

-- Create Architecture:
-- Library=work,Cell=comp1,View=behavior
-- Time:May 26 16:48:13 1999
-- By:killpack

ARCHITECTURE behavior of comp1 IS
BEGIN
    comp1_pro: process
    begin
        -- wait for req to rise
        if (req = '0') then
            wait until req = '1';
        end if;
        -- evaluate and set output accordingly
        if ((A = '0' and B = '0') or (A = '1' and B = '1')) then
            eq <= '1' after delay(1,10);
            wait until eq = '1';
        elsif (A = '1' and B = '0') then
            gr <= '1' after delay(1,10);
            wait until gr = '1';
        else
            le <= '1' after delay(1,10);
            wait until le = '1';
        end if;
        -- raise ackknowlge
        ack <= '1' after delay(1,10);
        -- wait for req to fall
        if (req = '1') then
            wait until req = '0';
        end if;
        -- lower proper signal
        if (eq = '1') then
            eq <= '0' after delay(1,10);
            wait until eq = '0';
        elsif (gr = '1') then
            gr <= '0' after delay(1,10);
            wait until gr = '0';
        else
            le <= '0' after delay(1,10);
            wait until le = '0';
        end if;
        -- lower ack
        ack <= '0' after delay(1,10);
    end process comp1_pro;
END behavior;


--Create Entity:
--Library=work,Cell=comp1_tb,View=entity
--Time:Wed May 26 16:44:31 1999
--By:killpack

LIBRARY ieee, work;
USE ieee.std_logic_1164.all;
USE work.nondeterminism.all;
ENTITY comp1_tb IS
    PORT(
        a : INOUT std_ulogic := '0';
        b : INOUT std_ulogic := '0';
        req : INOUT std_ulogic := '0';
        ack : IN std_ulogic
    );
END comp1_tb;

-- Create Architecture:
-- Library=work,Cell=comp1_tb,View=behavior
-- Time:May 26 16:45:14 1999
-- By:killpack

ARCHITECTURE behavior of comp1_tb IS
BEGIN
    -- process to seed the random number generator
    seed_proc: process
    begin
        dump_seed(124754,3243344); -- found in nondeterminism
        wait;
    end process seed_proc;

    env1_proc: process
        -- pick will be a rand number between 0 and 3 telling
        -- the env what to set a and b as.
        variable pick: integer := 0;
    begin
        -- randomly set A and B
      pick := random(3); -- 0 means neither a nor b is raised
      case pick is
        when 1 =>
            A <= '1' after delay(1,10);
            wait until A = '1';
        when 2 =>
            B <= '1' after delay(1,10);
            wait until B = '1';
        when 3 =>
            A <= '1' after delay(1,10);
            B <= '1' after delay(1,10);
            wait until A = '1';
            if (B = '0') then
                wait until B = '1';
            end if;
        when others =>   -- 0 means neither a nor b is raised
        end case;
        -- raise req
        req <= '1' after delay(1,10);
        -- wait for ack to rise
        if (ack = '0') then
            wait until ack = '1';
        end if;
        -- fix data to be all 0 again
        if (A = '1') then
          A <= '0' after delay(1,10);
        end if;
        if (B = '1') then
          B <= '0' after delay(1,10);
        end if;
        if not ((A = '0') and (B = '0')) then
          wait until ((A = '0') and (B = '0'));
        end if;
        -- lower req
        req <= '0' after delay(1,10);
        -- wait for ack to fall
        if (ack = '1') then
            wait until ack = '0';
        end if;
    end process env1_proc;
END behavior;


--Create Entity:
--Library=work,Cell=comp1_top,View=entity
--Time:Thu May 27 14:06:01 1999
--By:killpack

LIBRARY ieee;
USE ieee.std_logic_1164.all;
ENTITY comp1_top IS
    PORT(
        e : INOUT std_ulogic := '0';
        g : INOUT std_ulogic := '0';
        l : INOUT std_ulogic := '0'
    );
END comp1_top;


--Netlist:
--Library=work,Cell=comp1_top,View=schematic
--Time:Thu May 27 16:17:49 1999
--By:killpack

LIBRARY ieee;
USE ieee.std_logic_1164.all;
ARCHITECTURE schematic OF comp1_top IS
    COMPONENT comp1_tb
        PORT(
            a : INOUT std_ulogic;
            b : INOUT std_ulogic;
            req : INOUT std_ulogic;
            ack : IN std_ulogic
        );
    END COMPONENT;
    
    COMPONENT comp1
        PORT(
            ack : INOUT std_ulogic;
            eq : INOUT std_ulogic;
            gr : INOUT std_ulogic;
            le : INOUT std_ulogic;
            a : IN std_ulogic;
            b : IN std_ulogic;
            req : IN std_ulogic
        );
    END COMPONENT;
    
    SIGNAL ack : std_ulogic;
    SIGNAL req : std_ulogic; --@ in
    SIGNAL b : std_ulogic; --@ in
    SIGNAL a : std_ulogic; --@ in
    
    
BEGIN
    
    
    i1 : comp1_tb 
        PORT MAP(
            ack => ack,
            req => req,
            b => b,
            a => a
        );
    
    i0 : comp1 
        PORT MAP(
            le => l,
            ack => ack,
            req => req,
            b => b,
            a => a,
            gr => g,
            eq => e
        );
    
END schematic;

-- VHDL configuration created by Hierarchy Editor 1.0
-- Source configuration: work config1_comp1_top config 
library work;
CONFIGURATION config1_comp1_top OF comp1_top IS
  FOR schematic
  FOR OTHERS : comp1_tb USE ENTITY work.comp1_tb(behavior);
    FOR behavior
    END FOR;
  END FOR;
  FOR OTHERS : comp1 USE ENTITY work.comp1(behavior);
    FOR behavior
    END FOR;
  END FOR;
END FOR;
END config1_comp1_top;
