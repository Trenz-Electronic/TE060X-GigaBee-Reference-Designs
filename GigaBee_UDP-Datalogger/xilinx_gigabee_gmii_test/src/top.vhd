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
-------------------------------------------------------------------------------
-- Target Platform: GigaBee XC6SLX45 (www.trenz-electronic.de)
--						  (Easily portable to other GigaBee series boards)
--
-- Description: 
-- Purpose of this design is to log changes (events) on a group of
-- inputs (probes), add a timestamp to these events and send data
-- to PC using a UDP based protocol. 
-- 
-- To demonstrate functionality without connecting of GigaBee board
-- to any external signal generator, probes are connected internally
-- to PRBS (Pseudo-Random Bit Sequence) generator IP core. If you wish
-- to connect external signals, disconnect PRBS generator (signal prbs)
-- form Probes component (port InProbes) and connect external signals
-- (e.g. toplevel port inProbes) to it. You also need to specify pins
-- for these signals in system constraint file (system.ucf).
--
-- Main features:
--
-- Number of probes: adjustable using "probes_no" generic; 
--                   tested for 16, 32 and 96 probes
--
-- Sample rate: 100 MSamples/s
--
-- Timestamp length: 32 bit
-- Timestamp resolution: 10 ns (100 MHz)
-- Timestamp overflows every approximately 42 s.
-- 
-- The communication protocol is based on broadcast UDP packets.
-- Device can transmitt data only, there is no communication
-- channel in the oposit direction. 
-- 
-- Gigabit Ethernet is only supported. If autonegotioation ends up
-- with other than gigabit speed, device will not work properly 
-- and no packets will be sent.
-- 
-- Packet structure and format:
-- Source MAC address of device: 88:77:xx:22:33:11 
-- - XX can be set by user (board_id port of udp_pkt_gen_inst)
-- source IP address: 192.168.0.1 - hardcoded
-- 
-- Destination MAC: ff:ff:ff:ff:ff:ff - broadcast mac addr
-- Destination IP: 255.255.255.255 - broadcast mac addr
-- 
-- UDP src and dst port: 8200
-- 
-- Format of UDP payload:
-- Length of data is always 1296 Bytes.
-- Logger protocol header : 16 Bytes
-- Logger protocol data length: 1280 Bytes
-- 
-- Header:
-- Protocol ID          16 bits        always 0x5445 (can be adjusted in 
--                                     headerrom.vhd)
-- Valid data length    16 bits        number of bytes of valid 
--                                     data ( from 0 to Logger 
--                                     protocol data length)
-- Packet number        32 bits        number of packet that can 
--                                     be used to detect lost packets
-- Board ID             16 bits        borad ID obtained from board_id 
--                                     signal of udp_pkt_gen_inst                                   
-- Reserved             48 bits        Reserved for future use and 
--                                     debugging purposes
-- Data:
-- Event record len:   32 bits of timestamp + probes_no
-- Max number of events per packet : 1280/ Event record len
-- Event records can not be divided between two packets.
--
-- Event record:
-- Type I - data record:
--   - standard event record
--   - consists of 32 bit timestamp and "probes_no" bit data 
--     obtained after chande on probes
-- 
-- 
-- Type II - flag record
--   - if the timestamp of event is exactly the same as the 
--      previous one, record doesn't contain data but flags 
--      signalizing errors or state of device
--
--   - there are two flags implemented now:
--      value 0x0001 - reset occured - after the device is 
--                     reset, two events signalizing reset are 
--                     send: 0x00000000 0x0001; 0x00000000 0x0001; 
--                     if the link speed autonegotiation takes too 
--                     long, this first packed can be lost
--      value 0x0002 - FIFO overflow - if internal FIFO that collects 
--                     event records before they are put into packet 
--                     and send out gets full and event is lost, 
--                     event: last timestamp 0x0002 is added to data 
--                     stream
-- 
-- 
-- If timestamp timer overflows (every 42 s on 100 MHz) 
-- event: 0x00000000 curren data; is added to data.
-- 
-- Packet is send if any of these conditions is fulfilled:
-- - there is max number of events per packed ready to be send
-- - there is less than max number of events ready, but time from last event 
--   added to FIFO is more than approx. 0.01 s (adjustable by constant 
--   MAX_IDLE_CYCLES_TO_SEND_CONST in top.vhd)
-- 
-- Capturing packets:
-- To capture packets MS VS2008 project UDP_broadcast_receiver_PC,
-- located in sw directory, can be used (in very basic case).
-- After it is executed it captures packets from logger and saves
-- them in binary form to file data.dat. To stop capturing enter 
-- key must be pressed.
-------------------------------------------------------------------------------
-- LEDS:
-- To indicate design is working 4 leds on base board are programmed to flash
-- as indication design is alive. LED 0 flashes on frequency derived from
-- GMII interface clock (125MHz), LED 1 flashes on frequency derived from
-- probes sampling clock (100MHz). LED 2 indicates PHY reset. If LED if off
-- PHY is in reset state (only during initialization). LED 3 changes state
-- whenever packet is sent.
--
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
--
-- WARNINIG!!!
--
-- In this example probes are not connected to any pins, but to internal PRBS
-- (Pseudo Random Binary Sequence) generator. Generater is intentionaly
-- controlled in such a way that packet rate is close to maximum. Broadcast 
-- packets transmitted at this high rate can cause problems in network.
-- For these reason it is strongly recommended to connect device to local
-- networks only to limit spreading of broadcast packets to corporate network.
--
-------------------------------------------------------------------------------


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.pkg_rst_gen.all;
use work.pkg_probes.all;
use work.pkg_probes2fifo.all;
use work.pkg_events2data.all;
use work.pkg_timestamp_cnt.all;
use work.pkg_pkt_trigger.all;


library UNISIM;
use UNISIM.Vcomponents.ALL; 

Entity top Is
    Port (
     -- CLock form on board oscilator
     sys_clk   : In  std_logic;
     
     -- PHY control - reset
     rstn      : Out std_logic;
     
     -- PHY Tx side
     tx_clk	   : Out std_logic;
     tx_en		: Out std_logic;
     tx_er		: Out std_logic;
     tx_data	: Out std_logic_vector(7 downto 0);

     -- PHY Rx side
     rx_clk		: In  std_logic;
     rx_dv		: In  std_logic;
     rx_er		: In  std_logic;
     rx_data   : In  std_logic_vector(7 downto 0);
     
     -- LEDs
     led   	   : Out std_logic_vector(3 downto 0);
     
     -- Input Probes
     inProbes  : In std_logic_vector(15 downto 0)
   );
End top;

Architecture arch Of top Is 

   Component ethtxclkio Is
	PORT
	(
		datain_h		: IN STD_LOGIC ;
		datain_l		: IN STD_LOGIC ;
		outclock		: IN STD_LOGIC ;
		dataout		: OUT STD_LOGIC 
	);
	End Component ethtxclkio;

	Component ethoutput
   GENERIC(
      bit_num : positive
           );
	PORT
	(
		datain_h		: IN STD_LOGIC_VECTOR (bit_num - 1 DOWNTO 0);
		datain_l		: IN STD_LOGIC_VECTOR (bit_num - 1 DOWNTO 0);
		outclock		: IN STD_LOGIC ;
		dataout		: OUT STD_LOGIC_VECTOR (bit_num - 1 DOWNTO 0)
	);
	End Component;
   
   Component event_fifo
	   port (
   	wr_clk: IN std_logic;
	   rd_clk: IN std_logic;
   	din: IN std_logic_VECTOR(47 downto 0);
	   wr_en: IN std_logic;
   	rd_en: IN std_logic;
	   dout: OUT std_logic_VECTOR(47 downto 0);
   	full: OUT std_logic;
	   empty: OUT std_logic;
   	rd_data_count: OUT std_logic_VECTOR(9 downto 0));
   End Component;   
	
   Component udp_pkt_gen Is
	Generic(
      IGAP_LEN          : integer := 12;     -- 12 is minimul allowed by standard	
      DATA_LEN          : integer := 1280;   -- 160 Events @ 64bits/Event
      HEADER_LEN        : integer := 66;     -- 8B Preambule + 14B Eth + 20B IP + 8B UDP + 16B OurHeader = 66B 
      LENGTH_OFF        : integer := 53;       -- Where to place length of data in bytes
      BOARD_ID_MAC_OFF  : integer := 17;       -- Where to place board ID into MAC address
      PACKET_ID_OFF     : integer := 55;       -- Where to place number of packet
      USR_FLG_OFF       : integer := 61        -- Where to place user flags
	);
	Port(
		CLK				: In Std_logic;
		RST				: In Std_logic;
		
		-- Control inouts
		START			: In  Std_logic; -- Starts sending packets
      READY       : out Std_logic; -- Indicates readiness to accept start
		USR_FLAGS	: In  Std_logic_vector(15 downto 0); -- added to header on positive edge of START
		BOARD_ID    : In Std_logic_vector( 7 downto 0);
		LENGTH      : In Std_logic_vector(10 downto 0);
		
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
	End Component udp_pkt_gen;
	
   Component pll_sysclk
	Port(-- Clock in ports
		CLK_IN1           : in     std_logic;
		-- Clock out ports
		CLK_OUT1          : out    std_logic;
		CLK_OUT2          : out    std_logic;
		-- Status and control signals
		RESET             : in     std_logic;
		LOCKED            : out    std_logic
	);
	End Component;

	Component PRBS_TX_SER Is
	Port (
		CLK        : in  std_logic;                      -- synchron clock
		RESET      : in  std_logic;                      -- asynchron reset
		CLK_EN     : in  std_logic;                      -- clock enable
		PRBS_SET   : in  std_logic;                      -- set new PRBS / bit pattern
		PRBS_TYPE  : in  std_logic_vector (3 downto 0);  -- type of PRBS / bit pattern
		PRBS_INV   : in  std_logic;                      -- invert PRBS pattern
		ERR_INSERT : in  std_logic;                      -- manual error insert
		ERR_SET    : in  std_logic;                      -- set new error type
		ERR_TYPE   : in  std_logic_vector (3 downto 0);  -- error type
		TX_BIT     : out std_logic;                       -- tx serial output
		TX         : out std_logic_vector(31 downto 0)  -- tx paralel output
	);
	End Component;
   

   -- Internal clock signal -- may be from pll
  	signal int_clk     : std_logic;
	signal pll_locked  : std_logic;

   -- Blinking of alive LEDs - one on system clock one on tx clock
	signal cnt: std_logic_vector(31 downto 0);
	signal cnt_tx: std_logic_vector(31 downto 0);


   -- Reset signals
   signal rst         : std_logic;
   signal rst1        : std_logic;
   signal probes_rst  : std_logic;
	signal rst_tx      : std_logic;
   signal rst_tx_d    : std_logic;
   signal rst_tx_dd   : std_logic;
   signal ext_rst     : std_logic;

   -- Clock to PHY
	signal mii_clk_tx  : std_logic;
   signal nmii_clk_tx : std_logic;
   -- Clock for probes
   signal probes_clk  : std_logic;
   -- Internal signals
	signal tx_en_i     : std_logic;
   signal tx_en_ii    : std_logic;
	signal tx_er_i     : std_logic;
	signal tx_data_i   : std_logic_vector(7 downto 0) := (others => '0');
   signal tx_data_ii  : std_logic_vector(7 downto 0)  := (others => '0');
   signal tx_ready_i  : std_logic;
	
	signal outs_i	    :std_logic_vector(9 downto 0);
	signal outs_o	    :std_logic_vector(9 downto 0);
	
	signal fifo_rdreq  : std_logic;
   signal fifo_data_in: std_logic_vector(7 downto 0);
   signal fifo_rdempty: std_logic;
   signal fifo_data   : std_logic_vector(7 downto 0);
   signal fifo_wrreq  : std_logic;

   -- Signals from sensing probes
   signal probes_out    : std_logic_vector(15 downto 0);
   signal probes_change : std_logic;

   -- Event fifo signals
   signal event_fifo_data_in  : std_logic_vector(47 downto 0);
   signal event_fifo_data_out : std_logic_vector(47 downto 0);
   signal event_fifo_wrreq    : std_logic;
   signal event_fifo_wrfull   : std_logic;
   signal event_fifo_rdreq    : std_logic;
   signal event_fifo_rdempty  : std_logic;
   signal event_fifo_rdusedw  : std_logic_vector(10 downto 0);
   
   -- Signal triggering event collection
   signal event_trg           : std_logic;

   -- Signal triggering the new packet
	signal pkt_start         : std_logic := '0';
   -- Packet handling signals
   signal pkt_ready         : std_logic := '0';
   signal pkt_data_len      : std_logic_vector(10 downto 0);
   
   -- Timestamp counter signals
   signal timestamp	 : std_logic_vector(31 downto 0) := (others => '0');
   signal timestamp_ovf : std_logic := '0';

   -- Debugging signal
	signal cnt_a         : std_logic_vector(7 downto 0) := (others => '1');
   signal cnt_testsig   : std_logic_vector(24 downto 0) := (others => '0');
	signal prbs				: std_logic_vector(31 downto 0) := (others => '0');
   signal start_pkt_led : std_logic;
   signal prbs_clk_en   : std_logic;

Begin

   -- Pll sourced by external clock 125 MHz
	-- Ethernet clock goes to PHY CLK_OUT1 - 125MHz (Transmitter site)
   -- Internal clock CLK_OUT2 - 100MHz (probes, prbs, etc.)
   pll_sysclk_inst : pll_sysclk Port Map (
		CLK_IN1	          => sys_clk,
      CLK_OUT1           => mii_clk_tx,
		CLK_OUT2           => int_clk,
		RESET					 => '0',
		LOCKED             => pll_locked
	);

   -- Probes clock identical with internal clock
   probes_clk <= int_clk;
   
   -- RGMII clock output
	nmii_clk_tx <= not mii_clk_tx;
   tx_clk_ODDR_inst : ODDR2
   Generic Map(
      DDR_ALIGNMENT => "C0", -- Sets output alignment to "NONE", "C0", "C1" 
      INIT => '0', -- Sets initial state of the Q output to '0' or '1'
      SRTYPE => "ASYNC") -- Specifies "SYNC" or "ASYNC" set/reset
   Port Map (
      Q => tx_clk, -- 1-bit output data
      C0 => mii_clk_tx, -- 1-bit clock input
      C1 => nmii_clk_tx, -- 1-bit clock input
      CE => '1',  -- 1-bit clock enable input
      D0 => '1',   -- 1-bit data input (associated with C0)
      D1 => '0',   -- 1-bit data input (associated with C1)
      R => '0',    -- 1-bit reset input
      S => '0'     -- 1-bit set input
   );
	
   ext_rst <= not pll_locked;

   -- Reset generator
   rst_inst : rst_gen
      Generic Map(
         rst_cycles => 400000000,
         rst1_cycles => 100000
                 )
      Port Map(
         clk      => int_clk,
         ext_rst  => ext_rst,
         rst      => rst,
         rst1     => rst1
              );
   rstn <= not rst1; -- PHY reset is active low
   probes_rst <= rst;

   -- Resample rst to tx domain
   Process(mii_clk_tx)
   Begin
      If mii_clk_tx'Event And mii_clk_tx = '1' Then
         rst_tx_d <= rst;
         rst_tx_dd <= rst_tx_d;
         rst_tx <= rst_tx_dd;
      End If;
   End Process;

   -- Sensing probes
   probes_inst : probes
   Generic Map(
      probes_number => 16
              )
   Port Map(
      clk       => probes_clk,
      rst       => probes_rst,
      InProbes  => prbs(15 downto 0),
      OutProbes => probes_out,
      Change    => probes_change
           );

   -- Timestamp counter
   timestamp_cnt_inst : timestamp_cnt
   Generic Map(
      width => 32
              )
   Port Map(
      clk      => probes_clk,
      rst      => probes_rst,
      en       => '1',
      timestmp => timestamp,
      ovf      => timestamp_ovf
           );

   -- Trigger of events
   -- Trigger event if input probes changed or
   -- timestamp counter has overflown
   event_trg <= probes_change or timestamp_ovf;

   -- Interface between probes and FIFO
   -- If FIFO is full, error event is added to
   -- stream of events.
   -- Error stream is marked by duplicit
   -- timestamp.
   probes2fifo_inst : probes2fifo
   Generic Map(
      probes_number   => 16,
      timestamp_width => 32
              )
   Port Map(
      clk         => probes_clk,
      rst         => probes_rst,

      DataTrg     => event_trg,
      DataIn      => probes_out,
      timestamp   => timestamp,

      fifo_data   => event_fifo_data_in,
      fifo_wrreq  => event_fifo_wrreq,
      fifo_wrfull => event_fifo_wrfull
           );

   -- FIFO for events
   event_fifo_inst : event_fifo
	Port Map	(
		din		      => event_fifo_data_in,
		rd_clk	      => mii_clk_tx,
		rd_en		      => event_fifo_rdreq,
		wr_clk	      => probes_clk,
		wr_en		      => event_fifo_wrreq,
		dout	         => event_fifo_data_out,
		empty	         => event_fifo_rdempty,
		full	         => event_fifo_wrfull,
      rd_data_count  => event_fifo_rdusedw(9 downto 0)
	         );

   event_fifo_rdusedw(10) <= '0';

   -- Serialize data form event fifo (48bit)
   -- to input data fifo(8bit)
   -- Add padding to 64 bit
   events2data_inst : events2data
   Generic Map(
         events_width   => 48,
         data_width     => 8
              )
   Port Map (
      clk            => mii_clk_tx,--int_clk,
      rst            => rst_tx,--rst,

      events_data    => event_fifo_data_out,
      events_rdreq   => event_fifo_rdreq,
      events_rdempty => event_fifo_rdempty,

      data_rdreq     => fifo_rdreq,
      data_rdempty   => fifo_rdempty,
      data_data      => fifo_data
            );
	
   pkt_trigger_inst  :  pkt_trigger
   Generic Map(
      max_idle_time_to_send   => 1000000,
      full_pkt                => 213,
      event_number_width      => 11,
      pkt_len_width           => 11,
      bytes_per_event         => 6
              )
   Port Map(
      clk         => mii_clk_tx,
      rst         => rst_tx,

      events_no   => event_fifo_rdusedw,
      events_empty => fifo_rdempty, -- Indicates there are data in event fifo, or events2data
      ext_start   => '0',

      pkt_len     => pkt_data_len,
      pkt_send    => pkt_start,
      pkt_ready   => pkt_ready
           );

	-- Output flip-flops
	outs_i(0) <= tx_en_i;
	outs_i(1) <= tx_er_i;
	outs_i(9 downto 2) <= tx_data_i;
		
	ethoutputs_inst_phy : ethoutput 
   Generic Map(
      bit_num  => 10
              )
   Port Map (
		datain_h	 => outs_i,
		datain_l	 => outs_i,
		outclock	 => mii_clk_tx,
		dataout	 => outs_o
	);
	
	tx_data <= outs_o(9 downto 2);
	tx_en   <= outs_o(0);
	tx_er   <= outs_o(1);
  
  
   -- led assignment
   led(0) <= cnt(25);
   led(1) <= cnt_tx(25);
   led(2) <= rst;
   led(3) <= start_pkt_led;

   -- After pkt start, chenge led state
   start_pkt_led_process : Process (mii_clk_tx)
   Begin
      If mii_clk_tx'Event And mii_clk_tx = '1' Then
         If pkt_start = '1' Then
            start_pkt_led <= Not start_pkt_led;
         End If;
      End If;
   End Process;
   
   -- UDP/Ethernet core
   udp_pkt_gen_inst : udp_pkt_gen
   Generic Map(
      IGAP_LEN          => 12,     -- 12 is minimul allowed by standard	
      DATA_LEN          => 1280,   -- 160 Events @ 64B/Event + 16B Header
      HEADER_LEN        => 66,     -- 8B Preambule + 14B Eth + 20B IP + 8B UDP + 16B OurHeader = 66B 
      LENGTH_OFF        => 53,       -- Where to place length of data in bytes
      BOARD_ID_MAC_OFF  => 17,       -- Where to place board ID into MAC address
      PACKET_ID_OFF     => 55,       -- Where to place number of packet
      USR_FLG_OFF       => 61        -- Where to place user flags
	)
	Port Map(
		CLK				=> mii_clk_tx,
		RST				=> rst_tx,
		
		-- Control inouts
		START			   => pkt_start,
      READY          => pkt_ready,
		USR_FLAGS	  	=> x"cdef",
		BOARD_ID       => x"ab",
		LENGTH		   => pkt_data_len,
		
		-- Conection to packet data FIFO
		FIFO_RDREQ		=> fifo_rdreq,
		FIFO_RDEMPTY	=> fifo_rdempty,
		FIFO_DATA		=> fifo_data,
		
		-- Connection to ethernet PHY
		PHY_EN 		   => tx_en_ii,
		PHY_ER	 	   => tx_er_i,
		PHY_TX   	  	=> tx_data_ii,
      PHY_READY      => tx_ready_i
	);

   tx_data_i <= tx_data_ii;
   tx_en_i <= tx_en_ii;
   tx_ready_i <= '1';


   -- Alive counter / LED
   Process (int_clk)
   Begin
 	   If int_clk'event and int_clk = '1' Then
		   cnt <= std_logic_vector(unsigned(cnt) + 1);
	   End If;
   End Process;
  
   Process (mii_clk_tx)
   Begin
	   If mii_clk_tx'event and mii_clk_tx = '1' Then
		   cnt_tx <= std_logic_vector(unsigned(cnt_tx) + 1);
	   End If;
   End Process;


   -- Enable generater once per 16 clk not to overload channel
   prbs_clk_en <= '1' When cnt(3 downto 0) = "0000" Else '0';
   -- PRBS generator component
   prbs_inst : PRBS_TX_SER
   Port Map(
      CLK        => probes_clk,
      RESET      => probes_rst,
      CLK_EN     => prbs_clk_en,
      PRBS_SET   => '1',
      PRBS_TYPE  => "0010",
      PRBS_INV   => '0',
      ERR_INSERT => '0',
      ERR_SET    => '0',
      ERR_TYPE   => "0000",
      TX_BIT     => open,
      TX         => prbs
    );
End arch;
