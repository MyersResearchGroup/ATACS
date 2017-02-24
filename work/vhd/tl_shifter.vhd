--top_level for shifter
--
-- tl_shifter.vhd




library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all; 
use work.nondeterminism.all;
use work.handshake.all;


entity tl_shifter is
end tl_shifter;


architecture structure of tl_shifter is

	component hs_shifter
		port(	
			-- Data to load
			req_load    : in    std_logic;
			data_load   : in    std_logic;
			ack_load    : inout std_logic;
			-- Data shifted in
			req_shift   : in    std_logic;
			shift_value : in    std_logic;
			ack_shift   : inout std_logic;
			-- Data value to shift out
			shift_out   : inout std_logic;
			-- Done shifted in
			req_done    : in    std_logic;
			dataout     : inout std_logic;
			ack_done    : inout std_logic;
			sig_data    : inout std_logic
	 	);
	end component;

	signal sig_req_load    : std_logic;
	signal sig_data_load   : std_logic;
	signal sig_ack_load    : std_logic;
	signal sig_req_shift   : std_logic;
	signal sig_shift_value : std_logic;
	signal sig_req_done    : std_logic;
	signal sig_data_out    : std_logic;
	signal sig_ack_done    : std_logic;
	signal sig_ack_shift   : std_logic;
	signal sig_shift_out   : std_logic;
	signal sig_data : std_logic;
	

begin

	shifter : hs_shifter
	port map	(	
					req_load    => sig_req_load,
					data_load   => sig_data_load,
					ack_load    => sig_ack_load,
					req_shift   => sig_req_shift,
					shift_value => sig_shift_value,
					ack_shift   => sig_ack_shift,
					shift_out   => sig_shift_out,
					req_done    => sig_req_done,
					dataout     => sig_data_out,
					ack_done    => sig_ack_done,
					sig_data => sig_data
			 	);

	test : process
	variable z : integer:=0;
	variable t : integer;
	begin
		z := selection(3);
		t := selection(2);
	        case z is
		    when 1 =>
			case t is
			    when 1 =>
				if (sig_data_load /= '1') then
				    assign(sig_data_load,'1',1,2);
				end if;
			    when others => 
				if (sig_data_load /= '0') then
				    assign(sig_data_load,'0',1,2);
				end if;
			end case;
			assign(sig_req_load,'1',3,5);
			guard(sig_ack_load, '1');
			assign(sig_req_load,'0',1,5);
			guard(sig_ack_load, '0');
		    when 2 =>
			case t is
			    when 1 =>
				if (sig_shift_value /= '1') then
				    assign(sig_shift_value,'1',1,2);
				end if;
			    when others => 
				if (sig_shift_value /= '0') then
				    assign(sig_shift_value,'0',1,2);
				end if;
			end case;
			assign(sig_req_shift,'1',3,5);
			guard(sig_ack_shift, '1');
			assign(sig_req_shift,'0',1,5);
			guard(sig_ack_shift,'0');
		    when others =>
			assign(sig_req_done,'1',1,5);
			guard(sig_ack_done,'1');
			assign(sig_req_done,'0',1,5);
			guard(sig_ack_done,'0');
		end case;
		--wait for 5 ns;
	end process test;	
		
end structure;
