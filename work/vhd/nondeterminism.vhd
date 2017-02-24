--\begin{verbatim}
-----------------------------
-- Name: nondeterminism.vhd
-----------------------------
library ieee;
use ieee.math_real.all;

package nondeterminism is

  constant inf : integer := 2147483647;  -- MUST BE SAME AS ATACS' INFINITY
  shared variable s1 : integer := 844396720;
  shared variable s2 : integer := 821616997;

  impure function selection(constant number : in integer)
    return integer;

  -- Returns a random delay between lower and
  -- upper bounds in ns. (i.e., [lower,upper])
  impure function delay( constant lower : in integer;
                         constant upper : in integer)
    return time;

  -- Returns a random real between lower and upper
  impure function span(constant lower : in real;
                       constant upper : in real)
    return real;
  
end nondeterminism;

package body nondeterminism is

  impure function selection(constant number : in integer)
    return integer is
    variable result : integer;
    variable tmp_real : real;
  begin
    uniform(s1, s2, tmp_real);
    result := 1 + integer(trunc( tmp_real * real(number) ) );
    return(result);
  end selection;

  -- Returns a random time value between the lower and 
  -- upper bounds (i.e., [lower,upper]).  
  impure function delay( constant lower : in integer;
                         constant upper : in integer)
    return time is
    variable result : time;
    variable tmp_real : real;
  begin
    uniform(s1, s2, tmp_real);
    result := ( ((tmp_real * real( upper - lower )) +
                 real(lower)) * 1 ns );
    return result;
  end delay;

  impure function span(constant lower : in real;
                       constant upper : in real)
    return real is
    variable result : real;
    variable tmp_real : real;
  begin
    uniform(s1, s2, tmp_real);
    result := (tmp_real * (upper - lower) + lower);
    return result;
  end span;
  
end nondeterminism;
--\end{verbatim}

