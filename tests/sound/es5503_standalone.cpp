#include "es5503_standalone.h"
#include <algorithm>
#include <cstring>
#include <iostream>

// ES5503 - Standalone DOC implementation based on MAME's ES5503 emulator
// Adapted from MAME's ES5503 implementation by R. Belmont

// Define static constants
const uint16_t ES5503::wavesizes[8] = { 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };
const uint32_t ES5503::wavemasks[8] = { 0x1ff00, 0x1fe00, 0x1fc00, 0x1f800, 0x1f000, 0x1e000, 0x1c000, 0x18000 };
const uint32_t ES5503::accmasks[8]  = { 0xff, 0x1ff, 0x3ff, 0x7ff, 0xfff, 0x1fff, 0x3fff, 0x7fff };
const int      ES5503::resshifts[8] = { 9, 10, 11, 12, 13, 14, 15, 16 };

// Constructor
ES5503::ES5503(uint32_t clock_rate, uint8_t *wave_memory, uint32_t memory_size) :
    m_oscsenabled(1),
    m_rege0(0xff),
    m_channel_strobe(0),
    m_output_channels(2),
    m_clock_rate(clock_rate),
    m_irq_active(false),
    m_wave_memory(wave_memory),
    m_memory_size(memory_size)
{
    // Initialize oscillators
    reset();
    
    // Calculate output rate
    m_output_rate = (m_clock_rate / 8) / (m_oscsenabled + 2);
    
    // Allocate mix buffer (1/50th of a second at maximum)
    m_mix_buffer.resize((m_output_rate/50) * m_output_channels);
}

// Set number of output channels
void ES5503::set_channels(int channels)
{
    m_output_channels = channels;
}

// Reset the chip
void ES5503::reset()
{
    m_rege0 = 0xff;
    m_channel_strobe = 0;
    m_irq_active = false;

    for (int i = 0; i < 32; i++)
    {
        m_oscillators[i].freq = 0;
        m_oscillators[i].wtsize = 0;
        m_oscillators[i].control = 0;
        m_oscillators[i].vol = 0;
        m_oscillators[i].data = 0x80;
        m_oscillators[i].wavetblpointer = 0;
        m_oscillators[i].wavetblsize = 0;
        m_oscillators[i].resolution = 0;
        m_oscillators[i].accumulator = 0;
        m_oscillators[i].irqpend = 0;
    }

    m_oscsenabled = 1;
    
    // Recalculate output rate
    m_output_rate = (m_clock_rate / 8) / (m_oscsenabled + 2);
}

// Read from register
uint8_t ES5503::read(uint16_t offset)
{
    uint8_t retval;
    int i;

    if (offset < 0xe0)
    {
        int osc = offset & 0x1f;

        switch(offset & 0xe0)
        {
            case 0:     // freq lo
                return (m_oscillators[osc].freq & 0xff);

            case 0x20:  // freq hi
                return (m_oscillators[osc].freq >> 8);

            case 0x40:  // volume
                return m_oscillators[osc].vol;

            case 0x60:  // data
                return m_oscillators[osc].data;

            case 0x80:  // wavetable pointer
                return (m_oscillators[osc].wavetblpointer>>8) & 0xff;

            case 0xa0:  // oscillator control
                return m_oscillators[osc].control;

            case 0xc0:  // bank select / wavetable size / resolution
                retval = 0;
                if (m_oscillators[osc].wavetblpointer & 0x10000)
                {
                    retval |= 0x40;
                }

                retval |= (m_oscillators[osc].wavetblsize<<3);
                retval |= m_oscillators[osc].resolution;
                return retval;
        }
    }
    else     // global registers
    {
        switch (offset)
        {
            case 0xe0:  // interrupt status
                retval = m_rege0;

                // Clear IRQ line
                m_irq_active = false;
                if (m_irq_callback) m_irq_callback(false);

                // scan all oscillators
                for (i = 0; i < m_oscsenabled; i++)
                {
                    if (m_oscillators[i].irqpend)
                    {
                        // signal this oscillator has an interrupt
                        retval = i<<1;

                        m_rege0 = retval | 0x80;

                        // and clear its flag
                        m_oscillators[i].irqpend = 0;
                        break;
                    }
                }

                // if any oscillators still need to be serviced, assert IRQ again immediately
                for (i = 0; i < m_oscsenabled; i++)
                {
                    if (m_oscillators[i].irqpend)
                    {
                        m_irq_active = true;
                        if (m_irq_callback) m_irq_callback(true);
                        break;
                    }
                }

                return retval | 0x41;

            case 0xe1:  // oscillator enable
                return (m_oscsenabled - 1) << 1;

            case 0xe2:  // A/D converter
                return 0; // Not implemented in standalone version
        }
    }

    return 0;
}

// Write to register
void ES5503::write(uint16_t offset, uint8_t data)
{
    if (offset < 0xe0)
    {
        int osc = offset & 0x1f;

        switch(offset & 0xe0)
        {
            case 0:     // freq lo
                m_oscillators[osc].freq &= 0xff00;
                m_oscillators[osc].freq |= data;
                break;

            case 0x20:  // freq hi
                m_oscillators[osc].freq &= 0x00ff;
                m_oscillators[osc].freq |= (data<<8);
                break;

            case 0x40:  // volume
                m_oscillators[osc].vol = data;
                break;

            case 0x60:  // data - ignore writes
                break;

            case 0x80:  // wavetable pointer
                m_oscillators[osc].wavetblpointer = (data<<8);
                break;

            case 0xa0:  // oscillator control
                // key on?
                if ((m_oscillators[osc].control & 1) && (!(data&1)))
                {
                    m_oscillators[osc].accumulator = 0;
                }
                m_oscillators[osc].control = data;
                break;

            case 0xc0:  // bank select / wavetable size / resolution
                if (data & 0x40)    // bank select - not used on the Apple IIgs
                {
                    m_oscillators[osc].wavetblpointer |= 0x10000;
                }
                else
                {
                    m_oscillators[osc].wavetblpointer &= 0xffff;
                }

                m_oscillators[osc].wavetblsize = ((data>>3) & 7);
                m_oscillators[osc].wtsize = wavesizes[m_oscillators[osc].wavetblsize];
                m_oscillators[osc].resolution = (data & 7);
                break;
        }
    }
    else     // global registers
    {
        switch (offset)
        {
            case 0xe0:  // interrupt status
                break;

            case 0xe1:  // oscillator enable
                // The number here is the number of oscillators to enable -1 times 2.
                // You can never have zero oscillators enabled.
                // So a value of 62 enables all 32 oscillators.
                m_oscsenabled = ((data>>1) & 0x1f) + 1;
                
                // Recalculate output rate when oscillator count changes
                m_output_rate = (m_clock_rate / 8) / (m_oscsenabled + 2);
                break;

            case 0xe2:  // A/D converter
                break;
        }
    }
}

// Read a byte from wave memory
uint8_t ES5503::read_byte(uint32_t address)
{
    if (address < m_memory_size)
    {
        return m_wave_memory[address];
    }
    return 0;
}

// Generate audio into provided buffer
void ES5503::generate_audio(int16_t *buffer, int num_samples)
{
    update_stream(buffer, num_samples);
}

// Update audio stream
void ES5503::update_stream(int16_t *buffer, int num_samples)
{
    int32_t *mixp;
    int osc, snum, i;
    uint32_t ramptr;

    // Debug output
    std::cout << "** ES5503::update_stream **" << std::endl;
    std::cout << "Oscillators enabled: " << (int)m_oscsenabled << std::endl;
    std::cout << "Output channels: " << m_output_channels << std::endl;
    std::cout << "Number of samples to generate: " << num_samples << std::endl;

    // Make sure we have a big enough buffer
    if (num_samples * m_output_channels > (int)m_mix_buffer.size())
    {
        m_mix_buffer.resize(num_samples * m_output_channels);
    }

    // Clear mix buffer
    std::fill(m_mix_buffer.begin(), m_mix_buffer.end(), 0);

    for (int chan = 0; chan < m_output_channels; chan++)
    {
        std::cout << "Processing channel " << chan << std::endl;
        
        for (osc = 0; osc < m_oscsenabled; osc++)
        {
            ES5503Osc *pOsc = &m_oscillators[osc];
            std::cout << "  Oscillator " << osc << ": control=0x" << std::hex << (int)pOsc->control 
                      << " channel=" << ((pOsc->control >> 4) & 0xF) 
                      << " halted=" << ((pOsc->control & 1) ? "yes" : "no") << std::dec << std::endl;

            if (!(pOsc->control & 1) && ((pOsc->control >> 4) & (m_output_channels - 1)) == chan)
            {
                std::cout << "  Oscillator " << osc << " active for channel " << chan << std::endl;
                uint32_t wtptr = pOsc->wavetblpointer & wavemasks[pOsc->wavetblsize], altram;
                uint32_t acc = pOsc->accumulator;
                const uint16_t wtsize = pOsc->wtsize - 1;
                uint8_t ctrl = pOsc->control;
                const uint16_t freq = pOsc->freq;
                int16_t vol = pOsc->vol;
                int8_t data = -128;
                const int resshift = resshifts[pOsc->resolution] - pOsc->wavetblsize;
                const uint32_t sizemask = accmasks[pOsc->wavetblsize];
                const int mode = (pOsc->control>>1) & 3;
                mixp = &m_mix_buffer[0] + chan;

                for (snum = 0; snum < num_samples; snum++)
                {
                    altram = acc >> resshift;
                    ramptr = altram & sizemask;

                    acc += freq;

                    // channel strobe is always valid when reading; this allows potentially banking per voice
                    m_channel_strobe = (ctrl>>4) & 0xf;
                    data = (int32_t)read_byte(ramptr + wtptr) ^ 0x80;

                    uint8_t byte_value = read_byte(ramptr + wtptr);
                    if (byte_value == 0x00)
                    {
                        std::cout << "    HALTING oscillator " << osc << " - found zero byte at address 0x" 
                                  << std::hex << (ramptr + wtptr) << std::dec
                                  << " (sample " << snum << ")" << std::endl;
                        halt_osc(osc, 1, &acc, resshift);
                    }
                    else
                    {
                        if (mode != MODE_SYNCAM)
                        {
                            int value = data * vol;
                            // Only log a few samples to avoid console spam
                            if (snum < 10) {
                                std::cout << "    Sample " << snum << ": data=" << (int)data 
                                      << " vol=" << (int)vol << " value=" << value << std::endl;
                            }
                            
                            *mixp += value;
                            if (chan == (m_output_channels - 1))
                            {
                                *mixp += value;
                                *mixp += value;
                            }
                        }
                        else
                        {
                            // if we're odd, we play nothing ourselves
                            if (osc & 1)
                            {
                                if (osc < 31)
                                {
                                    // if the next oscillator up is playing, it's volume becomes our control
                                    if (!(m_oscillators[osc + 1].control & 1))
                                    {
                                        m_oscillators[osc + 1].vol = data ^ 0x80;
                                    }
                                }
                            }
                            else    // hard sync, both oscillators play?
                            {
                                *mixp += data * vol;
                                if (chan == (m_output_channels - 1))
                                {
                                    *mixp += data * vol;
                                    *mixp += data * vol;
                                }
                            }
                        }
                        mixp += m_output_channels;

                        if (altram >= wtsize)
                        {
                            halt_osc(osc, 0, &acc, resshift);
                        }
                    }

                    // if oscillator halted, we've got no more samples to generate
                    if (pOsc->control & 1)
                    {
                        ctrl |= 1;
                        break;
                    }
                }

                pOsc->control = ctrl;
                pOsc->accumulator = acc;
                pOsc->data = data ^ 0x80;
            }
        }
    }

    // Copy mix buffer to output buffer with appropriate scaling
    std::cout << "Copying mix buffer to output (first 10 samples):" << std::endl;
    
    for (int chan = 0; chan < m_output_channels; chan++)
    {
        mixp = &m_mix_buffer[0] + chan;
        for (i = 0; i < num_samples; i++)
        {
            // Scale appropriately and convert to 16-bit output
            int32_t value = *mixp;
            int16_t scaled = (value * 8) >> 8;
            buffer[i * m_output_channels + chan] = scaled;
            
            if (i < 10) {
                std::cout << "  Sample " << i << ": mix=" << value << " scaled=" << scaled << std::endl;
            }
            
            mixp += m_output_channels;
        }
    }
}

// halt_osc: handle halting an oscillator
// onum = oscillator #
// type = 1 for 0 found in sample data, 0 for hit end of table size
void ES5503::halt_osc(int onum, int type, uint32_t *accumulator, int resshift)
{
    ES5503Osc *pOsc = &m_oscillators[onum];
    ES5503Osc *pPartner = &m_oscillators[onum^1];
    int mode = (pOsc->control>>1) & 3;
    const int partnerMode = (pPartner->control>>1) & 3;

    // check for sync mode
    if (mode == MODE_SYNCAM)
    {
        if (!(onum & 1))
        {
            // we're even, so if the odd oscillator 1 below us is playing,
            // restart it.
            if (!(m_oscillators[onum - 1].control & 1))
            {
                m_oscillators[onum - 1].accumulator = 0;
            }
        }

        // loop this oscillator for both sync and AM
        mode = MODE_FREE;
    }

    // if 0 found in sample data or mode is not free-run, halt this oscillator
    if ((mode != MODE_FREE) || (type != 0))
    {
        pOsc->control |= 1;
    }
    else    // preserve the relative phase of the oscillator when looping
    {
        uint16_t wtsize = pOsc->wtsize - 1;
        *accumulator -= (wtsize << resshift);
    }

    // if we're in swap mode, start the partner
    if (mode == MODE_SWAP)
    {
        pPartner->control &= ~1;    // clear the halt bit
        pPartner->accumulator = 0;  // and make sure it starts from the top (does this also need phase preservation?)
    }
    else
    {
        // if we're not swap and we're the even oscillator of the pair and the partner's swap
        // but we aren't, we retrigger (!!!)  Verified on IIgs hardware.
        if ((partnerMode == MODE_SWAP) && ((onum & 1)==0))
        {
            pOsc->control &= ~1;

            // preserve the phase in this case too
            uint16_t wtsize = pOsc->wtsize - 1;
            *accumulator -= (wtsize << resshift);
        }
    }
    
    // IRQ enabled for this voice?
    if (pOsc->control & 0x08)
    {
        pOsc->irqpend = 1;

        m_irq_active = true;
        if (m_irq_callback) m_irq_callback(true);
    }
}