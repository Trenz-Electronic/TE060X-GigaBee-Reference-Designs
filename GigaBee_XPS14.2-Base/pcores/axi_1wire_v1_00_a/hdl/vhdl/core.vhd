--------------------------------------------------------------------------------
-- Company		: Trenz Electronic
-- Engineer		: Oleksandr Kiyenko
--------------------------------------------------------------------------------
-- user_logic.vhd
--------------------------------------------------------------------------------
library ieee;
use ieee.STD_LOGIC_1164.all;
use ieee.STD_LOGIC_arith.all;
use ieee.STD_LOGIC_unsigned.all;
--------------------------------------------------------------------------------
entity core is
generic(
	C_CLK_FREQ			: integer := 62500000
);
port (
	clk 				: in  STD_LOGIC;
	rstn 				: in  STD_LOGIC;
	command				: in  STD_LOGIC_VECTOR(1 downto 0);
	command_wr			: in  STD_LOGIC;
	busy				: out STD_LOGIC;
	res					: out STD_LOGIC;
	onewire_I			: in  STD_LOGIC;
	onewire_O			: out STD_LOGIC;
	onewire_T			: out STD_LOGIC
);
end entity core;
--------------------------------------------------------------------------------
-- Architecture section
--------------------------------------------------------------------------------
architecture IMP of core is
--==============================================================================
-- Constants
constant CMD_RESET			: STD_LOGIC_VECTOR(1 downto 0) := "11";
constant CMD_WR0			: STD_LOGIC_VECTOR(1 downto 0) := "00";
constant CMD_WR1			: STD_LOGIC_VECTOR(1 downto 0) := "01";
constant CMD_RD				: STD_LOGIC_VECTOR(1 downto 0) := "10";

constant C_TRSTL_TIME		: INTEGER := 500;
constant C_TPRES_TIME		: INTEGER := 570;
constant C_TRSTE_TIME		: INTEGER := 900;
constant C_TWR0_TIME		: INTEGER := 60;
constant C_TLOW_TIME		: INTEGER := 6;
constant C_TSMP_TIME		: INTEGER := 18;
constant C_CYCLE_TIME		: INTEGER := 65;

-- Signals
signal cycle_cnt			: STD_LOGIC_VECTOR(31 downto 0);
type sm_state_type is (
	ST_IDLE,
	ST_RST_LOW,
	ST_RST_HIGH,
	ST_RST_END,
	ST_WR0_LOW,
	ST_WR1_LOW,
	ST_RD_LOW,
	ST_RD_HIGH,
	ST_END
);
signal sm_state				: sm_state_type := ST_IDLE;
--==============================================================================
begin
--==============================================================================
process(clk,rstn)
begin
	if(rstn = '0')then
		cycle_cnt	<= (others => '0');
	elsif(clk = '1' and clk'event)then
		if(command_wr = '1')then
			cycle_cnt	<= (others => '0');
		else
			cycle_cnt	<= cycle_cnt + 1;
		end if;
	end if;
end process;

process(clk,rstn)
begin
	if(rstn = '0')then
		sm_state	 	<= ST_IDLE;
		onewire_T		<= '1';
		res				<= '0';
		busy			<= '0';
	elsif(clk = '1' and clk'event)then
		case sm_state is
			when ST_IDLE => 
				if(command_wr = '1')then
					onewire_T	<= '0';
					busy		<= '1';
					case command is
						when CMD_RESET 	=> sm_state <= ST_RST_LOW;
						when CMD_WR0 	=> sm_state <= ST_WR0_LOW;
						when CMD_WR1 	=> sm_state <= ST_WR1_LOW;
						when CMD_RD  	=> sm_state <= ST_RD_LOW;
						when others 	=> null;
					end case;
				end if;
				
			when ST_RST_LOW => 
				if(cycle_cnt = ((C_CLK_FREQ/1000000)*C_TRSTL_TIME))then
					onewire_T	<= '1';
					sm_state	<= ST_RST_HIGH;
				end if;

			when ST_RST_HIGH => 
				if(cycle_cnt = ((C_CLK_FREQ/1000000)*C_TPRES_TIME))then
					res			<= onewire_I;
					sm_state	<= ST_RST_END;
				end if;
				
			when ST_RST_END => 
				if(cycle_cnt = ((C_CLK_FREQ/1000000)*C_TRSTE_TIME))then
					busy		<= '0';
					sm_state	<= ST_IDLE;
				end if;

			when ST_WR0_LOW => 
				if(cycle_cnt = ((C_CLK_FREQ/1000000)*C_TWR0_TIME))then
					onewire_T	<= '1';
					sm_state	<= ST_END;
				end if;

			when ST_WR1_LOW => 
				if(cycle_cnt = ((C_CLK_FREQ/1000000)*C_TLOW_TIME))then
					onewire_T	<= '1';
					sm_state	<= ST_END;
				end if;

			when ST_RD_LOW => 
				if(cycle_cnt = ((C_CLK_FREQ/1000000)*C_TLOW_TIME))then
					onewire_T	<= '1';
					sm_state	<= ST_RD_HIGH;
				end if;

			when ST_RD_HIGH => 
				if(cycle_cnt = ((C_CLK_FREQ/1000000)*C_TSMP_TIME))then
					res			<= onewire_I;
					sm_state	<= ST_END;
				end if;
			
			when ST_END => 
				if(cycle_cnt = ((C_CLK_FREQ/1000000)*C_CYCLE_TIME))then
					busy		<= '0';
					sm_state	<= ST_IDLE;
				end if;
			
		end case;
	end if;
end process;

onewire_O	<= '0';		-- It's always '0' - we control only _T
--==============================================================================
end IMP;
