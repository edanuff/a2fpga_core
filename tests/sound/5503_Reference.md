# Ensoniq 5503 Digital Oscillator Chip Technical Reference

## 1. Introduction

The Ensoniq 5503 Digital Oscillator Chip (DOC) is a specialized digital sound synthesis chip designed for professional music synthesizers, arcade games, and home video systems. The chip contains 32 digital sampling oscillators, various control registers, and an 8-bit analog-to-digital converter. It provides high-quality music synthesis with minimal processor intervention.

## 2. System Overview

### 2.1 Key Features
- 32 digital sampling oscillators with dedicated control registers
- 16-channel voice assignment capability
- Direct addressing of 64K memory (expandable to 128K with bank select)
- Compatible with 6800/6500 family microprocessors and 65816 style processors
- Clock signals compatible with Motorola 6809e (E & Q clock outputs)
- Dynamic memory control with automatic refresh
- On-chip 8-bit linear A/D converter

### 2.2 System Architecture
The DOC and microprocessor share access to the data and address bus on alternating phases of a symmetrical system clock. When the clock is high, the processor takes control; when low, the DOC takes over. During each DOC cycle, an oscillator generates an address to read a byte from its wavetable in RAM. The data read from the wavetable is sent to two cascaded 8-bit DACs where the waveform is converted to audio.

The output section consists of:
- Volume DAC (upper DAC): controls amplitude
- Waveform DAC (lower DAC): handles waveform information

Each oscillator gets a cycle until all oscillators have updated their waveform. The next two cycles are used for refreshing dynamic RAM before starting over.

## 3. Oscillator Architecture

Oscillators are essentially address generators that step through a waveform table at a rate determined by the frequency control register and resolution register.

### 3.1 Oscillator Block Diagram
```
24 BIT ACCUMULATOR (A) → 16 BIT FREQUENCY CONTROL(FC)
              ↓                   ↓
              → 24 BIT ADDER ←
                     ↓
              A := A + FC
                     ↓
ADDRESS POINTER →    ↓
TABLE SIZE →  ADDRESS OUTPUT MULTIPLEXER ← RESOLUTION
                     ↓
            ADDRESS OUTPUT BUFFERS
```

### 3.2 Oscillator Operation
- Each update cycle, the value in the frequency control register is added to the 24-bit accumulator
- The new accumulator value forms the basis for the final 16-bit RAM address
- The resolution register determines which 16 bits of the accumulator are used for addressing
- The table size register determines how many of the available bits form the final address
- The pointer register sets the most significant bit(s) in the final address

## 4. Register Map

The DOC contains 227 registers:
- 224 oscillator control registers (7 registers per oscillator × 32 oscillators)
- 3 additional global registers (Interrupt, Enable, and A/D Converter)

### 4.1 Register Summary
| Address | Function | Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---------|----------|-------|-------|-------|-------|-------|-------|-------|-------|
| $00-$1F | FREQUENCY LOW | FL7 | FL6 | FL5 | FL4 | FL3 | FL2 | FL1 | FL0 |
| $20-$3F | FREQUENCY HIGH | FH7 | FH6 | FH5 | FH4 | FH3 | FH2 | FH1 | FH0 |
| $40-$5F | VOLUME | V7 | V6 | V5 | V4 | V3 | V2 | V1 | V0 |
| $60-$7F | WAVEFORM DATA SAMPLE | W7 | W6 | W5 | W4 | W3 | W2 | W1 | W0 |
| $80-$9F | WAVEFORM TABLE POINTER | P7 | P6 | P5 | P4 | P3 | P2 | P1 | P0 |
| $A0-$BF | CONTROL | CA3 | CA2 | CA1 | CA0 | IE | M1 | M0 | H |
| $C0-$DF | BANK SELECT/TABLE SIZE/RESOLUTION | X | BS | T2 | T1 | T0 | R2 | R1 | R0 |
| $E0 | OSCILLATOR INTERRUPT | IRQ | 1 | O4 | O3 | O2 | O1 | O0 | 1 |
| $E1 | OSCILLATOR ENABLE | X | X | E4 | E3 | E2 | E1 | E0 | X |
| $E2 | A/D CONVERTER | A7 | A6 | A5 | A4 | A3 | A2 | A1 | A0 |

Note: Bits labeled as X are not used and always read back as 1.

## 5. Register Descriptions

### 5.1 Frequency Control Low/High ($00-$1F, $20-$3F)
These registers form a 16-bit value that controls the rate at which the oscillator steps through its waveform table. The value is added to the 24-bit accumulator each time the oscillator is updated.

The fundamental output frequency can be calculated as:
- Sample Rate (SR) = CLK / ((OSC + 2) * 8)
- Output Frequency = [SR / 2^(17 + RES)] * FC

Where:
- CLK = input clock frequency
- OSC = number of enabled oscillators
- RES = Resolution register value
- FC = concatenated value of Frequency High and Low registers

### 5.2 Volume ($40-$5F)
Controls the volume level of the waveform data. The 8-bit volume value is multiplied by the 8-bit waveform value to form the final analog output. Note that the volume register is ignored when an oscillator is in amplitude modulation mode.

### 5.3 Data Sample ($60-$7F)
These read-only registers contain the last value read from the waveform table for each oscillator. Useful for monitoring oscillator output or for LFO/modulation applications.

### 5.4 Address Pointer ($80-$9F)
Contains the beginning page number of the waveform table address. All waveform tables must begin at the first address of a page and cannot wrap around from upper to lower memory. The number of pointer bits actually used depends on the table size.

### 5.5 Control Register ($A0-$BF)
Controls channel assignment, oscillator mode, and the halt bit.

Bit layout:
- Bits 7-4 (CA3-CA0): Channel Assignment - routes oscillator output to a particular channel
- Bit 3 (IE): Interrupt Enable - when set, allows interrupts to be passed to the OIR
- Bits 2-1 (M1-M0): Mode - determines oscillator operation mode
- Bit 0 (H): Halt Bit - indicates when oscillator has been halted

Oscillator modes:
| M1 | M0 | Mode | Description |
|----|----|--------------------|-------------|
| 0 | 0 | Free Run | Oscillator runs continuously until halted |
| 0 | 1 | One-Shot | Addresses one cycle, then halts |
| 1 | 0 | Sync/AM | Sync or amplitude modulation between paired oscillators |
| 1 | 1 | Swap | One-shot mode, then clears halt bit of next oscillator |

**Mode Details:**
- **Free Run Mode**: Oscillator continuously loops through waveform until halted by processor or encounters zero in waveform.
- **One-Shot Mode**: Oscillator cycles through waveform once, then halts.
- **Sync/AM Mode**:
  - For AM mode: Use odd/even pair (odd oscillator modulates even). Volume registers ignored. Requires analog waveform to be gated with CSTRB.
  - For Sync mode: Use even/odd pair (odd syncs to even). When first oscillator wraps, second oscillator also resets.
- **Swap Mode**: Uses even/odd pair. Oscillator runs in one-shot mode, when complete it resets and clears halt bit of next oscillator.

Note: When halt bit (H) is set and M0=1, the oscillator accumulator resets to zero.

### 5.6 Waveform Table Size/Resolution/Bank Select ($C0-$DF)
Controls three oscillator functions:

Bit layout:
- Bit 7: Not used (always reads as 1)
- Bit 6 (BS): Bank Select - extends addressing range (0=0-64K, 1=65K-128K)
- Bits 5-3 (T2-T0): Table Size - determines waveform table size
- Bits 2-0 (R2-R0): Resolution - determines which accumulator bits are used for addressing

Table sizes:
| T2 | T1 | T0 | Size (bytes) |
|----|----|----|-------------|
| 0 | 0 | 0 | 256 |
| 0 | 0 | 1 | 512 |
| 0 | 1 | 0 | 1024 |
| 0 | 1 | 1 | 2048 |
| 1 | 0 | 0 | 4096 |
| 1 | 0 | 1 | 8192 |
| 1 | 1 | 0 | 16384 |
| 1 | 1 | 1 | 32768 |

Resolution settings:
| R2 | R1 | R0 | Accumulator bits used |
|----|----|----|---------------------|
| 0 | 0 | 0 | 1 through 16 |
| 0 | 0 | 1 | 2 through 17 |
| 0 | 1 | 0 | 3 through 18 |
| 0 | 1 | 1 | 4 through 19 |
| 1 | 0 | 0 | 5 through 20 |
| 1 | 0 | 1 | 6 through 21 |
| 1 | 1 | 0 | 7 through 22 |
| 1 | 1 | 1 | 8 through 23 |

Typically, the Resolution register should be loaded with the same value as the Table Size register. Using different values affects the fundamental frequency by powers of 2.

### 5.7 Oscillator Interrupt Register ($E0)
Contains the state of the IRQ line and the number of an interrupting oscillator. When an oscillator completes a waveform table and IE=1, its number is passed to the OIR, which forces IRQ low.

Bit layout:
- Bit 7 (IRQ): IRQ state (0 = interrupt active)
- Bit 6: Always 1
- Bits 5-1 (O4-O0): Interrupting oscillator number (0-31)
- Bit 0: Always 1

If multiple oscillators generate interrupts, they are queued in a FIFO interrupt stack. Reading the OIR clears the current interrupt and loads the next one from the stack.

### 5.8 Oscillator Enable Register ($E1)
Controls the number of active oscillators. A minimum of one oscillator is always selected (reset default). To enable oscillators, multiply the desired number by 2 and load that value.

Example: To enable all 32 oscillators, load 64.

Note: There are 34 actual oscillator time slots (32 oscillators + 2 RAM refresh slots). Halving oscillators to 16 doesn't double the frequency due to refresh slots.

### 5.9 A/D Converter Register ($E2)
Contains the value from the A/D converter. A read from this register initiates an A-to-D conversion. Conversion time is 26 cycles. If read before completion, the value is lost and a new conversion starts.

## 6. Address Calculation

The final address is calculated based on the Address Pointer, Table Size, and Resolution registers.

### 6.1 Address Pointer Usage
The number of pointer bits used depends on the table size:

| Table Size | Pointer Bits Used |
|------------|------------------|
| 256 | P7-P0 |
| 512 | P7-P1 |
| 1024 | P7-P2 |
| 2048 | P7-P3 |
| 4096 | P7-P4 |
| 8192 | P7-P5 |
| 16384 | P7-P6 |
| 32768 | P7 only |

### 6.2 Final Address Calculation
The final address combines bits from the Address Pointer register and the accumulator according to the selected Table Size and Resolution.

Example: For a 512-byte waveform with Resolution=7, pointer bits P1-P7 and accumulator bits 15-23 form the final address. If Resolution changes to 5, pointer bits P1-P7 and accumulator bits 12-21 form the address.

## 7. Hardware Specifications

### 7.1 Timing Characteristics
- Input Clock Cycle Time: 100-125 ns (8MHz typical)
- Clock Duty Cycle: 50%
- RAS/CAS Clock Output Frequency: 2MHz typical
- E/Q Clock Output Frequency: 1MHz typical

### 7.2 Electrical Specifications
- Operating Temperature: 0 to 50°C
- Supply Voltage: 5V ±5%
- Input Low Voltage (VIL): 0.0-0.4V
- Input High Voltage (VIH): 2.4-5.0V
- Output Low Voltage (VOL): ≤0.8V
- Output High Voltage (VOH): ≥2.4V

### 7.3 Pin Configuration
The 5503 comes in a 40-pin package with the following key pins:
- CLK [1]: Master clock input
- RAS [2], CAS [3]: Row/Column Address Strobe outputs
- Q [4], E [5]: Clock outputs compatible with M6809E
- CSTRB [6]: Channel Strobe output
- CA0-CA3 [7-10]: Channel Assign outputs
- SIG+/SIG- [15-16]: Differential waveform signal outputs
- A/D [17]: Analog/Digital converter input
- IRQ [18]: Interrupt Request output (open collector)
- RES [20]: Reset input
- CS [21]: Chip Select input
- WE [22]: Write Enable input
- A0-A15/D0-D7 [23-38]: Address/Data lines
- BS [40]: Bank Select output

## 8. Implementation Notes

### 8.1 Critical Emulation Considerations
1. **Oscillator Update Sequence**: The DOC updates each oscillator in sequence, with two additional cycles for RAM refresh.

2. **Interrupt Handling**: Interrupts are queued in a FIFO stack. Reading the OIR clears the current interrupt and loads the next.

3. **Zero Detection**: An oscillator will halt when it encounters a zero in its wavetable. Due to the frequency step size, eight consecutive zeros must be present to guarantee halting.

4. **Paired Oscillator Modes**: Sync/AM and Swap modes rely on pairs of oscillators:
   - AM Mode: Odd oscillator modulates even oscillator
   - Sync Mode: Even oscillator resets odd oscillator
   - Swap Mode: Even oscillator triggers odd oscillator

5. **Volume Handling**: The volume register is ignored in AM mode.

6. **Address Calculation**: The final address combines pointer bits and accumulator bits based on table size and resolution settings.

7. **Bus Sharing**: The DOC and CPU share the same bus, with the DOC active during phase when the E clock is low.

8. **Memory Access**: Each oscillator reads from waveform memory during its allocated time slot.

### 8.2 Frequency Calculation
The fundamental frequency calculation is:
- Sample Rate (SR) = CLK / ((OSC + 2) * 8)
- Output Frequency = [SR / 2^(17 + RES)] * FC

Where OSC is the number of enabled oscillators, RES is the Resolution register value, and FC is the 16-bit Frequency Control value.

### 8.3 Output Generation
The audio output is formed by:
1. Reading a sample from the wavetable
2. Multiplying it by the volume register value
3. Sending the result to the DACs (unless in AM mode)

### 8.4 Special Mode Handling
- In AM mode, the odd oscillator's output determines the amplitude of the even oscillator
- In Sync mode, the even oscillator resets the odd oscillator's accumulator
- In Swap mode, completing one oscillator triggers the next

## 9. Practical Usage Notes

1. Waveform tables must begin on page boundaries and cannot wrap from upper to lower memory.

2. For best halting behavior, include 8 consecutive zeros at the end of one-shot waveforms.

3. Resolution typically matches table size, but can be adjusted for octave shifts.

4. Analog waveform must be gated with CSTRB when using AM mode.

5. Reset clears all registers except the Oscillator Enable, which defaults to 1.

6. Disabled oscillators don't retain register data as they aren't refreshed.

7. To enable oscillators, multiply desired count by 2 and load into Oscillator Enable register.

8. The A/D converter takes 26 cycles for conversion.