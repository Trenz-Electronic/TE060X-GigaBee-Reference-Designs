-------------------------------------------------------------------------------
-- Project: Data Logger with Ethernet UDP output
-- Author(s): DFC Design, s.r.o. (www.dfcdesign.cz)
-- E-Mail:  info@dfcdesign.cz
-- Created: Sept 2011   
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
--                                                                           --
-- Copyright (C) 2011 Authors                                                --
--                                                                           --
-- This source file may be used and distributed without                      --
-- restriction provided that this copyright statement is not                 --
-- removed from the file and that any derivative work contains               --
-- the original copyright notice and the associated disclaimer.              --
--                                                                           --
-- This source file is free software; you can redistribute it                --
-- and-or modify it under the terms of the GNU Lesser General                --
-- Public License as published by the Free Software Foundation;              --
-- either version 3.0 of the License, or (at your option) any                --
-- later version.                                                            --
--                                                                           --
-- This source is distributed in the hope that it will be                    --
-- useful, but WITHOUT ANY WARRANTY; without even the implied                --
-- warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR                   --
-- PURPOSE.  See the GNU Lesser General Public License for more              --
-- details.                                                                  --
--                                                                           --
-- You should have received a copy of the GNU Lesser General                 --
-- Public License along with this source; If not, see                        --
-- <http://www.gnu.org/licenses/>                                            --
--                                                                           --
------------------------------------------------------------------------------- 

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.pkg_shift_reg_ld.all;



entity udp_pkt_gen is
	generic(
      IGAP_LEN          : integer := 12;       -- 12 is minimul allowed by standard	
      DATA_LEN          : integer := 1280;     -- 160 Events @ 64bits/Event
      HEADER_LEN        : integer := 66;       -- 8B Preambule + 14B Eth + 20B IP + 8B UDP + 16B OurHeader = 66B 
      BOARD_ID_OFF      : integer := 60; --59 + 60       -- Where to place board ID
      LENGTH_OFF        : integer := 53;       -- Where to place length of data in bytes
      BOARD_ID_MAC_OFF  : integer := 17;       -- Where to place board ID into MAC address
      PACKET_ID_OFF     : integer := 55;       -- Where to place number of packet
      USR_FLG_OFF       : integer := 61        -- Where to place user flags
	);
	port(
		CLK				: In Std_logic;
		RST				: In Std_logic;
		
		-- Control inouts
		START			: In  Std_logic; -- Starts sending packets
      READY       : Out Std_logic; -- Indicates readiness to accept start
		USR_FLAGS	: In  Std_logic_vector(15 downto 0); -- added to header on positive edge of START
		BOARD_ID    : In  Std_logic_vector( 7 downto 0);
      LENGTH      : In  Std_logic_vector(10 downto 0);
		
		-- Conection to packet data FIFO
		FIFO_RDREQ		: Out Std_logic;
		FIFO_RDEMPTY	: In  Std_logic;
		FIFO_DATA		: In  Std_logic_vector(7 downto 0);
		
		-- Connection to ethernet PHY
		PHY_EN 		: Out Std_logic;
		PHY_ER	 	: Out Std_logic;
		PHY_TX   	: Out Std_logic_vector(7 downto 0);
      PHY_READY   : in  Std_logic -- If 1 PHY is ready to capture data on rising edge
	);
end entity udp_pkt_gen;


architecture arch of udp_pkt_gen is

   -- Components
   component headerrom is
   	generic 
   	(
   		DATA_WIDTH : natural;
   		ADDR_WIDTH : natural 
   	);  
   	port 
   	(
   		clk		: in std_logic;
   		addr	 : in natural range 0 to 2**ADDR_WIDTH - 1;
   		q		: out std_logic_vector((DATA_WIDTH -1) downto 0)
   	);
   end component;
   
  component CRC_gen 
  port(
    Reset       : In std_logic;
    Clk         : In std_logic;
    Init        : In std_logic;
    Frame_data  : In std_logic_vector(7 downto 0);
    Data_en     : In std_logic;
    CRC_rd      : In std_logic;
    CRC_rd_en   : In std_logic;
    CRC_end     : Out std_logic;
    CRC_out     : Out std_logic_vector(7 downto 0)
  );
  end component;
   

	
   -- Signal declaraions
	type STATE_UDP_PKT_GEN_T is (SIPG, WAIT_FOR_START, SEND_HEADER, SEND_DATA_FIFO, SEND_CRC);
	
	signal state_udp_pkt_gen_n	: STATE_UDP_PKT_GEN_T;
	signal state_udp_pkt_gen_c	: STATE_UDP_PKT_GEN_T;
	
   -- PHY Signals
	signal phy_en_i   	   : Std_logic;
   signal phy_en_ii      : Std_logic;
   --signal phy_en_iii     : Std_logic;
	signal phy_er_i   	   : Std_logic;
	signal phy_tx_i		     : Std_logic_vector(7 downto 0);
	signal phy_tx_ii 		   : Std_logic_vector(7 downto 0);
   signal phy_tx_iii		   : Std_logic_vector(7 downto 0);
   signal phy_ready_i      : Std_logic;
	
   -- FIFO signals
	signal fifo_rdreq_i 	   : Std_logic;
   signal fifo_rdreq_ii	   : Std_logic;
	signal fifo_rdempty_i	: Std_logic;
   signal fifo_rdempty_i_s : Std_logic;
   signal fifo_rdempty_i_c : Std_logic;
	signal fifo_data_i		: Std_logic_vector(7 downto 0);
   signal fifo_data_pad_i  : Std_logic_vector(7 downto 0);
   signal fifo_inuse_i     : Std_logic;
   signal fifo_inuse_ii    : Std_logic;

   -- CRC signals
   signal crc_init_i       : std_logic;
	signal crc_rd_i         : std_logic;
	signal crc_rd_ii        : std_logic;
	signal crc_en_i         : std_logic;
	signal crc_en_ii        : std_logic;
   signal crc_en_ii_r      : std_logic;
	signal crc_data_i       : std_logic_vector(7 downto 0);

   -- Counter of interframe gaps
   signal igap_cnt         : integer range 0 to IGAP_LEN;
   
   -- Header address counter
   signal hdr_addr_cnt     : integer range 0 to 127; -- Header no longer than 127Bytes
   signal pkt_hdr_var      : std_logic_vector(7 downto 0);
   signal pkt_hdr_var_e    : std_logic;
   signal pkt_hdr_var_e_r  : std_logic;
   
   -- Data counter
   signal fifo_data_cnt    : integer range 0 to DATA_LEN + 4; -- + 4B of Eth. CRC
   -- Max data to send     
   signal length_r         : integer range 0 to (2**length'length - 1);

   -- Data signal from header ROM
   signal header_data_i    : std_logic_vector(7 downto 0);

   -- Data going to CRC module
   signal frame_data_i     : std_logic_vector(7 downto 0);
   
   -- Counter of sent packets
   signal pkt_cnt : std_logic_vector(31 downto 0);

   -- Signalizes start of packet (samples inputs)
   signal start_edge       : std_logic;

   signal var_inputs       : std_logic_vector(10*8 - 1 downto 0);

   -- Signals for BOARD_ID resampling
   signal BOARD_ID_i       : std_logic_vector(7 downto 0);
   signal BOARD_ID_ii      : std_logic_vector(7 downto 0);

begin

   ------------------------------------------------------------ 
   -- udp_pkt_gens main state machine
   ------------------------------------------------------------

   -- Process that advances state of the main statemachine of udp_pkt_gen
	PROCESS (CLK)
	BEGIN
		If CLK'event And CLK = '1' Then
			If RST = '1' Then
				state_udp_pkt_gen_c <= SIPG;
			Elsif PHY_READY = '1' Then
				state_udp_pkt_gen_c <= state_udp_pkt_gen_n;
			End If;
		End If; -- Clk
	END PROCESS;
	
   -- State machine transaction logic
	PROCESS (state_udp_pkt_gen_c, START, igap_cnt, fifo_data_cnt, hdr_addr_cnt)
	BEGIN

      state_udp_pkt_gen_n <= state_udp_pkt_gen_c;

      Case state_udp_pkt_gen_c is
      When SIPG =>
         If igap_cnt = IGAP_LEN -1 Then
            state_udp_pkt_gen_n <= WAIT_FOR_START;
         End If;
      When WAIT_FOR_START =>
         If START = '1' Then
            state_udp_pkt_gen_n <= SEND_HEADER;
         End If;
      When SEND_HEADER =>
         If hdr_addr_cnt = HEADER_LEN - 1 Then
            state_udp_pkt_gen_n <= SEND_DATA_FIFO;
         End If;
      When SEND_DATA_FIFO =>
         If fifo_data_cnt = DATA_LEN - 1 Then
            state_udp_pkt_gen_n <= SEND_CRC;
         End If;
      When SEND_CRC =>
         If fifo_data_cnt = DATA_LEN + 4 - 1 Then
            state_udp_pkt_gen_n <= SIPG;
         End If;
      When Others =>
         state_udp_pkt_gen_n <= SIPG;
      End Case;

	END PROCESS;

   -- State machine actions
   PROCESS (state_udp_pkt_gen_c, hdr_addr_cnt, fifo_data_cnt, length_r, fifo_rdreq_i)
   BEGIN
      phy_en_i <= '0';
      phy_er_i <= '0';
      fifo_rdreq_i <= '0';
      crc_rd_i <= '0';
      crc_en_i <= '0';
      READY <= '0';
      fifo_inuse_i <= '0';

      Case (state_udp_pkt_gen_c) is
      --When SIGP =>
      When WAIT_FOR_START =>
         READY <= '1';
      When SEND_HEADER =>
         phy_en_i <= '1' after 1 ns;
         If hdr_addr_cnt >= 8 Then
            crc_en_i <= '1' after 1 ns;
         End If;
      When SEND_DATA_FIFO =>
         phy_en_i <= '1' after 1 ns;
         crc_en_i <= '1' after 1 ns;
         fifo_inuse_i <= '1';
         If fifo_data_cnt < length_r Then
            fifo_rdreq_i <= '1' after 1 ns;
         Else
            fifo_rdreq_i <= '0' after 1 ns;
         End If;
      When SEND_CRC =>
         phy_en_i <= '1' after 1 ns;
         crc_rd_i <= '1' after 1 ns;
      When others =>
      End Case;

   END PROCESS;

   -- If input buffer is empty, add pad and dont
   -- continue with data even if they come to fifo
   PROCESS(CLK)
   BEGIN
      If clk'Event And clk = '1' Then
         If state_udp_pkt_gen_c = SEND_DATA_FIFO Then
            If fifo_rdempty_i = '1' Then
               fifo_rdempty_i_s <= '1';
            End If;
         Else
            fifo_rdempty_i_s <= '0';
         End If;
      End If;
   END PROCESS;
   fifo_rdempty_i_c <= fifo_rdempty_i_s or fifo_rdempty_i;

   -- On the start, load lenght data
   PROCESS(CLK)
   BEGIN
      If clk'Event And clk = '1' Then
         If state_udp_pkt_gen_c = WAIT_FOR_START And START = '1' Then
            length_r <= to_integer(unsigned(length));
         End If;
      End If;
   END PROCESS;

   ------------------------------------------------------------ 
   -- END - udp_pkt_gens main state machine
   ------------------------------------------------------------

    -- On the start, load lenght data
   start_edge <= '1' When state_udp_pkt_gen_c = WAIT_FOR_START And START = '1' Else '0';


   -- Inter frame gap counter
   PROCESS (CLK)
   BEGIN
       If clk'event And clk = '1' Then
           If RST = '1' or not (state_udp_pkt_gen_c = SIPG) Then
              igap_cnt <= 0;
           Elsif PHY_READY = '1' Then 
              igap_cnt <= igap_cnt + 1 after 1 ns;
           End If;
       End If;
   END PROCESS;

   -- Header address counter
   PROCESS (CLK)
   BEGIN
      If clk'event And clk = '1' Then
         If RST = '1' or Not(state_udp_pkt_gen_c = SEND_HEADER) Then
            hdr_addr_cnt <= 0;
         ElsIf state_udp_pkt_gen_c = SEND_HEADER And PHY_READY = '1' Then
            hdr_addr_cnt <= hdr_addr_cnt + 1 after 1 ns;
         End If;
      End If;
   END PROCESS;

   -- FIFO read bytes counter
   PROCESS (CLK)
   BEGIN
      If clk'event And clk = '1' Then
         If RST = '1' Or Not((state_udp_pkt_gen_c = SEND_DATA_FIFO) Or (state_udp_pkt_gen_c = SEND_CRC)) Then
            fifo_data_cnt <= 0;
         ElsIf ((state_udp_pkt_gen_c = SEND_DATA_FIFO) Or (state_udp_pkt_gen_c = SEND_CRC)) And PHY_READY = '1' Then
            fifo_data_cnt <= fifo_data_cnt + 1 after 1 ns;
         End If;
      End If;
   END PROCESS;

   -- Counter of sent packets - this value is used as packet ID
   PROCESS (CLK)
   BEGIN
      If clk'event And clk = '1' Then
         If RST = '1' Then
            pkt_cnt <= x"00000000";--x"ffffffff"; -- The first packet has to have number 0;
         ElsIf state_udp_pkt_gen_c = WAIT_FOR_START And state_udp_pkt_gen_n = SEND_HEADER Then
            pkt_cnt <= std_logic_vector(unsigned(pkt_cnt) + 1) after 1 ns;
         End If;
      End If;
   END PROCESS;


   -- composing the output signal
   phy_tx_i <= crc_data_i when crc_rd_ii = '1' else frame_data_i;

   pkt_hdr_var_e <= '1' When  (hdr_addr_cnt = LENGTH_OFF) Or
                              (hdr_addr_cnt = LENGTH_OFF + 1) Or

                              (hdr_addr_cnt = BOARD_ID_MAC_OFF) Or

                              (hdr_addr_cnt = PACKET_ID_OFF) Or
                              (hdr_addr_cnt = PACKET_ID_OFF + 1) Or
                              (hdr_addr_cnt = PACKET_ID_OFF + 2) Or
                              (hdr_addr_cnt = PACKET_ID_OFF + 3) Or

                              (hdr_addr_cnt = BOARD_ID_OFF) Or

                              (hdr_addr_cnt = USR_FLG_OFF) Or
                              (hdr_addr_cnt = USR_FLG_OFF + 1) else '0';

   
   frame_data_i <= pkt_hdr_var When pkt_hdr_var_e = '1' Else header_data_i When  (fifo_inuse_ii = '0') else
                   fifo_data_pad_i;

   fifo_data_pad_i <= fifo_data_i  when fifo_rdreq_ii   = '1' And  fifo_rdempty_i_c = '0' else
                      x"00";


   -- BOARD_ID resampling
   PROCESS (CLK)
   BEGIN
      If clk'event and clk = '1' Then
         BOARD_ID_i  <= BOARD_ID;
         BOARD_ID_ii <= BOARD_ID_i;
      End if;
   END PROCESS;


   pkt_hdr_var_e_r <= pkt_hdr_var_e And PHY_READY;
   var_inputs <= usr_flags & BOARD_ID_ii & pkt_cnt & "00000" & length & BOARD_ID_ii;
   shift_reg_ld_inst : shift_reg_ld
   Generic Map(
      word_width => 8,
      stages_num => 10
              )
   Port Map(
      clk      => clk,
      rst      => rst,
      ld       => start_edge,
      oe       => pkt_hdr_var_e_r,
      datain   => var_inputs,
      dataout  => pkt_hdr_var
           );


   PHY_EN <= phy_en_ii;
   PHY_ER <= phy_er_i;

   -- Header ROM instantiation
   headerrom_inst : headerrom 
   	generic map
   	(
       DATA_WIDTH  => 8,
   		ADDR_WIDTH  => 7 
   	)
   	port map
   	(
   		clk		   => CLK,
   		addr	    => hdr_addr_cnt,
   		q		   => header_data_i
   	);


   -- CRC Module instatiation
   crc_init_i  <= not(crc_en_ii or crc_rd_ii);
   crc_en_ii_r <= crc_en_ii And PHY_READY after 1 ns;

   crc_gen_inst  : CRC_gen 
      port map(
         Reset       => RST,
         Clk         => CLK,
         Init        => crc_init_i,
         Frame_data  => phy_tx_iii, --frame_data_i,
         Data_en     => crc_en_ii_r,
         CRC_rd      => crc_rd_ii,
         CRC_rd_en   => PHY_READY,
         CRC_end     => open,
         CRC_out     => crc_data_i
      );

   -- Connection to FIFO
   FIFO_RDREQ       <= fifo_rdreq_ii And PHY_READY And not fifo_rdempty_i And not fifo_rdempty_i_s;
	fifo_rdempty_i   <= FIFO_RDEMPTY;
	fifo_data_i      <= FIFO_DATA;


   -- Adjust cycles in pipeline
   PROCESS (CLK)
   BEGIN
      If clk'event And clk = '1' Then
         If RST = '1' Then
            phy_en_ii  <= '0';
            --phy_en_iii <= '0';
            crc_en_ii  <= '0';
            crc_rd_ii  <= '0';
            fifo_rdreq_ii <= '0';
            phy_tx_ii  <= (others => '0');
            fifo_inuse_ii <= '0';
         Else
            if PHY_READY = '1' Then
               phy_en_ii  <= phy_en_i after 1 ns;
               --phy_en_iii <= phy_en_ii after 1 ns;
               crc_en_ii  <= crc_en_i after 1 ns;
               crc_rd_ii  <= crc_rd_i after 1 ns;
               fifo_rdreq_ii <= fifo_rdreq_i after 1 ns;
               fifo_inuse_ii <= fifo_inuse_i after 1 ns;
            end if;
            phy_ready_i <= PHY_READY;
            If phy_ready_i = '1' Then
               phy_tx_ii  <= phy_tx_i;
            end If;
         End If;
      End If;
   END PROCESS;   
   
   phy_tx_iii <= phy_tx_ii When phy_ready_i = '0' else phy_tx_i;
   PHY_TX <= phy_tx_iii;
   
end arch;
