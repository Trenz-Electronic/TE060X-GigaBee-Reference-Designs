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

library proc_common_v3_00_a;
use proc_common_v3_00_a.proc_common_pkg.all;
--------------------------------------------------------------------------------
entity user_logic is
generic(
	C_CLK_FREQ			 	: integer			  := 62500000;
	C_NUM_REG				: integer			  := 4;
	C_SLV_DWIDTH			: integer			  := 32
);
port (
	onewire_I				: in  STD_LOGIC;
	onewire_O				: out STD_LOGIC;
	onewire_T				: out STD_LOGIC;
	Bus2IP_Clk				: in  STD_LOGIC;
	Bus2IP_Resetn			: in  STD_LOGIC;
	Bus2IP_Data				: in  STD_LOGIC_VECTOR(C_SLV_DWIDTH-1 downto 0);
	Bus2IP_BE				: in  STD_LOGIC_VECTOR(C_SLV_DWIDTH/8-1 downto 0);
	Bus2IP_RdCE				: in  STD_LOGIC_VECTOR(C_NUM_REG-1 downto 0);
	Bus2IP_WrCE				: in  STD_LOGIC_VECTOR(C_NUM_REG-1 downto 0);
	IP2Bus_Data				: out STD_LOGIC_VECTOR(C_SLV_DWIDTH-1 downto 0);
	IP2Bus_RdAck			: out STD_LOGIC;
	IP2Bus_WrAck			: out STD_LOGIC;
	IP2Bus_Error			: out STD_LOGIC
);

attribute MAX_FANOUT : string;
attribute SIGIS : string;

attribute SIGIS of Bus2IP_Clk		: signal is "CLK";
attribute SIGIS of Bus2IP_Resetn	: signal is "RST";
--------------------------------------------------------------------------------
end entity user_logic;
--------------------------------------------------------------------------------
-- Architecture section
--------------------------------------------------------------------------------
architecture IMP of user_logic is
--==============================================================================
-- Components
component core is
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
end component core;

-- Constants
constant C_STATUS_REG_ADDR	: INTEGER := 0;
constant C_CONTROL_REG_ADDR	: INTEGER := 1;
constant C_STATUS_REG_SEL	: STD_LOGIC_VECTOR( 3 downto 0) := "1000";
constant C_CONTROL_REG_SEL	: STD_LOGIC_VECTOR( 3 downto 0) := "0100";
-- Signals
signal status_reg			: STD_LOGIC_VECTOR(C_SLV_DWIDTH-1 downto 0);
signal control_reg			: STD_LOGIC_VECTOR(C_SLV_DWIDTH-1 downto 0);
signal control_wr			: STD_LOGIC;
signal busy					: STD_LOGIC;
signal res					: STD_LOGIC;
signal slv_reg_write_sel	: STD_LOGIC_VECTOR(3 downto 0);
signal slv_reg_read_sel		: STD_LOGIC_VECTOR(3 downto 0);
signal slv_ip2bus_data		: STD_LOGIC_VECTOR(C_SLV_DWIDTH-1 downto 0);
signal slv_read_ack			: STD_LOGIC;
signal slv_write_ack		: STD_LOGIC;
--==============================================================================
begin
--==============================================================================
slv_reg_write_sel	<= Bus2IP_WrCE(3 downto 0);
slv_reg_read_sel	<= Bus2IP_RdCE(3 downto 0);
slv_write_ack		<= '0' when Bus2IP_WrCE = "0000" else '1';
slv_read_ack		<= '0' when Bus2IP_RdCE = "0000" else '1';

process(Bus2IP_Resetn, Bus2IP_Clk)
begin
	if(Bus2IP_Resetn = '0')then
		control_reg			<= (others => '0');
		control_wr			<= '0';
	elsif(Bus2IP_Clk'event and Bus2IP_Clk = '1')then
		if(slv_reg_write_sel = C_CONTROL_REG_SEL)then
			control_reg		<= Bus2IP_Data;
			control_wr		<= '1';
		else
			control_wr		<= '0';
		end if;
	end if;
end process;

status_reg	<= x"0000000" & "00" & res & busy;
process(slv_reg_read_sel, status_reg, control_reg)
begin
	case slv_reg_read_sel is
		when C_STATUS_REG_SEL 	=> slv_ip2bus_data <= status_reg;
		when C_CONTROL_REG_SEL	=> slv_ip2bus_data <= control_reg;
		when others => slv_ip2bus_data <= (others => '0');
	end case;
end process;

core_inst: core
generic map(
	C_CLK_FREQ		=> C_CLK_FREQ
)
port map(
	clk 			=> Bus2IP_Clk,
	rstn 			=> Bus2IP_Resetn,
	command			=> control_reg(1 downto 0),
	command_wr		=> control_wr,
	busy			=> busy,
	res				=> res,
	onewire_I		=> onewire_I,
	onewire_O		=> onewire_O,
	onewire_T		=> onewire_T
);

IP2Bus_Data		<= slv_ip2bus_data when slv_read_ack = '1' else (others => '0');
IP2Bus_WrAck	<= slv_write_ack;
IP2Bus_RdAck	<= slv_read_ack;
IP2Bus_Error	<= '0';
--------------------------------------------------------------------------------
end IMP;
